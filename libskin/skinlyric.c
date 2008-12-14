/* vi: set sw=4 ts=4: */
/*
 * skinlyric.c
 *
 * This file is part of libskin.
 *
 * Copyright (C) 2008 - kelvenxu <kelvenxu@gmail.com>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 * */

#include "skinlyric.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <glib/gstdio.h>

G_DEFINE_TYPE (SkinLyric, skin_lyric, GTK_TYPE_LAYOUT); 

#define SKIN_LYRIC_GET_PRIVATE(o)\
	(G_TYPE_INSTANCE_GET_PRIVATE ((o), SKIN_TYPE_LYRIC, SkinLyricPrivate))

typedef struct LyricLine
{
	// 在da中的位置 
	gint x;
	gint y;

	// 时间标签
	gint sec;
	gint ms;

	// 歌词文本
	gchar *text;
} LyricLine;

struct _SkinLyricPrivate 
{
	GtkWidget *alignment;
	GtkWidget *da;

	GdkPixmap *pixmap;

	// 为在da上输出字符串
	GdkGC *da_gc;
	PangoContext *pango_context;
	PangoLayout *pango_layout;

	GdkColor bg;
	GdkColor fg;
	GdkColor current;

	gint da_width;
	gint da_height;

	gint width;
	gint height;

	// 存储时间，其中的text指向texts中存储的歌词
	GList *lines;

	// 只存储歌词文本本身
	GList *texts;
	gint nlines;
	gint index;

	// 指示是否读入了歌词
	gboolean loaded;

	gint inter; // FIXME: 字高 ＋ 间隙

	gchar *songname;
	gchar *lyricname;

	gchar *title;
	gchar *artist;
	gchar *album;
	gchar *author;
	gint offset;
};


static void
skin_lyric_dispose (SkinLyric *self)
{
}

static void
skin_lyric_finalize (SkinLyric *self)
{
}

static void
skin_lyric_init (SkinLyric *self)
{
	SkinLyricPrivate *priv;
	priv = SKIN_LYRIC_GET_PRIVATE(self);
	self->priv = priv;

	priv->alignment = NULL;
	priv->da = NULL;
	priv->pixmap = NULL;

	priv->da_gc = NULL;
	priv->pango_context = NULL;
	priv->pango_layout = NULL;

	priv->songname = NULL;
	priv->lyricname = NULL;

	priv->inter = 14;

	priv->bg.red = 0;
	priv->bg.green = 0;
	priv->bg.blue = 0;

	priv->fg.red = 54740;
	priv->fg.green = 54740;
	priv->fg.blue = 54740;

	priv->current.red = 65535;
	priv->current.green = 65535;
	priv->current.blue = 65535;

	priv->lines = NULL;
	priv->nlines = 0;
	priv->index = 0;

	priv->loaded = FALSE;
}

static void
skin_lyric_class_init (SkinLyricClass *self_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (self_class);

	g_type_class_add_private (self_class, sizeof (SkinLyricPrivate));
	object_class->dispose = (void (*) (GObject *object)) skin_lyric_dispose;
	object_class->finalize = (void (*) (GObject *object)) skin_lyric_finalize;
}

static 
void update_pixmap(SkinLyric *lyric)
{
	GList *iter;
	LyricLine *line;
	GdkGC *gc;
	PangoContext *context;
	PangoLayout *layout;
	SkinLyricPrivate *priv = lyric->priv;
	gint n;

	if(priv->loaded == FALSE)
		return; //FIXME:

	if(priv->lines == NULL)
		return;

	if(priv->pixmap != NULL)
	{
		gdk_pixmap_unref(priv->pixmap);
	}

	n = g_list_length(priv->lines);

	priv->da_height = (n + 4) * priv->inter;

	priv->pixmap = gdk_pixmap_new(priv->da->window,
			priv->da_width, 
			priv->da_height,
			-1); 

	if(priv->pixmap == NULL)
		return;

	gc = gdk_gc_new(priv->da->window);
	gdk_gc_set_rgb_fg_color(gc, &priv->bg);
	gdk_draw_rectangle(priv->pixmap, gc, TRUE,
			0, 0,
			priv->da_width, 
			priv->da_height);

	context = gtk_widget_get_pango_context(priv->da);
	layout = pango_layout_new(context);
	pango_layout_set_width(layout, priv->da_width * PANGO_SCALE);
	pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
	gdk_gc_set_rgb_fg_color(gc, &priv->fg);

	for(iter = priv->lines; iter; iter = iter->next)
	{
		line = (LyricLine*)iter->data;
		if(line->text == NULL) 
			continue;
		pango_layout_set_text(layout, line->text, -1);
		gdk_draw_layout(priv->pixmap, gc, line->x, line->y, layout);
	}

	gtk_widget_set_size_request(priv->da, priv->da_width, priv->da_height);
}

static gboolean
layout_expose_cb(GtkWidget *widget, GdkEventExpose *event, SkinLyric *lyric)
{
	SkinLyricPrivate *priv = lyric->priv;
	GdkGC *gc;

	gc = gdk_gc_new(GTK_LAYOUT(widget)->bin_window);
	gdk_gc_set_rgb_fg_color(gc, &priv->bg);
	gdk_draw_rectangle(GTK_LAYOUT(widget)->bin_window, gc, TRUE,
			0, 0,
			widget->allocation.width, widget->allocation.height);

	return TRUE;
}

static gboolean
da_expose_cb(GtkWidget *widget, GdkEventExpose *event, SkinLyric *lyric)
{ 
	SkinLyricPrivate *priv = lyric->priv;
	
	if(priv->da_gc == NULL)
		priv->da_gc = gdk_gc_new(widget->window);

	if(priv->pixmap == NULL)
	{
		gdk_gc_set_rgb_fg_color(priv->da_gc, &priv->bg);
		gdk_draw_rectangle(widget->window, priv->da_gc, TRUE,
				0, 0,
				widget->allocation.width, 
				widget->allocation.height);
	}
	else
	{
		gdk_draw_drawable(widget->window,
				widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
				lyric->priv->pixmap,
				0, 0,
				0, 0,
				lyric->priv->da_width,
				lyric->priv->da_height);
	}
	return TRUE;
}

SkinLyric *
skin_lyric_new()
{
	SkinLyric *lyric;
	SkinLyricPrivate *priv;

	lyric = g_object_new(SKIN_TYPE_LYRIC, NULL);

	priv = lyric->priv;
	priv->alignment = gtk_alignment_new(0, 0, 1, 1);
	gtk_widget_show(priv->alignment);

	priv->da = gtk_drawing_area_new();
	gtk_widget_show(priv->da);

	//priv->da_gc = gdk_gc_new(priv->da->window);
	priv->pango_context = gtk_widget_get_pango_context(priv->da);
	priv->pango_layout = pango_layout_new(priv->pango_context);

	gtk_container_add(GTK_CONTAINER(priv->alignment), priv->da);
	gtk_container_add(GTK_CONTAINER(lyric), priv->alignment);

	gtk_widget_set_events(priv->da, GDK_EXPOSURE_MASK);

	g_signal_connect(G_OBJECT(lyric), "expose-event", G_CALLBACK(layout_expose_cb), lyric);
	g_signal_connect(G_OBJECT(priv->da), "expose-event", G_CALLBACK(da_expose_cb), lyric);

	return lyric;
}

void skin_lyric_get_size(SkinLyric *lyric, gint *width, gint *height)
{
	g_return_if_fail(SKIN_IS_LYRIC(lyric));

	*width = lyric->priv->da_width;
	*height = lyric->priv->da_height;
}

void
skin_lyric_set_size(SkinLyric *lyric, gint width, gint height)
{
	g_return_if_fail(SKIN_IS_LYRIC(lyric));

	gtk_widget_set_size_request(GTK_WIDGET(lyric), width, height);
	gtk_widget_set_size_request(lyric->priv->da, width, height);

	lyric->priv->da_width = width;
	lyric->priv->da_height = height;

	update_pixmap(lyric);
}

//NOTE: str: [mm:ss.ss
static void 
str2time(const gchar* str, int* sec, int* msec)
{
	//str: [mm:ss.ss
	int m = 0, s = 0, ms = 0;
	int len = strlen(str);
	gchar p[3];

	if(len >= 3)
	{
		p[0] = str[1];
		p[1] = str[2];
		p[2] = '\0';
		m = atoi(p);
	}

	if(len >= 6)
	{
		p[0] = str[4];
		p[1] = str[5];
		p[2] = '\0';
		s = atoi(p);
	}

	if(len >= 9)
	{
		p[0] = str[7];
		p[1] = str[8];
		p[2] = '\0';
		ms = atoi(p);
	}

	*sec = m * 60 + s;
	*msec = ms;
}

static gboolean
parse_lyric_line(SkinLyric *lyric, const gchar *line, gint line_no_)
{
	char** p = NULL;
	char* text = NULL;
	int sec, ms; 
	int old_sec = -1;
	gboolean flag = FALSE;

	SkinLyricPrivate *priv = lyric->priv;
	char** parray = g_strsplit(line, "]", 5);

	//find lyric text string
	for(p = parray; *p; ++p)
	{
		if((*p)[0] == '[' && (*p)[3] == ':')
			continue;
		else
		{
			text = g_strdup(*p);
			priv->texts = g_list_prepend(priv->texts, text);
			break;
		}
	}

	for(p = parray; *p; ++p)
	{ 
		if((*p)[0] == '[' && (*p)[3] == ':')
		{
			flag = TRUE;
			str2time(*p, &sec, &ms);

			if(old_sec == sec) //FIXME: 解决两个歌词时间相同的问题
				++sec;
			else
				old_sec = sec;

			LyricLine* data = g_new0(LyricLine, 1);
			data->text = text;
			data->sec = sec;
			data->ms = ms;
			//data->x = 0;
			//data->y = priv->inter * line_no;
			// 这里没有顺序
			priv->lines = g_list_prepend(priv->lines, data);
		}
	}

	g_strfreev(parray);

	return flag;
}

static void
update_xy_coordinate(SkinLyric *lyric)
{
	GList *iter;
	LyricLine *line;
	gint n;

	for(iter = lyric->priv->lines, n = 0; iter; iter = iter->next, ++n)
	{
		line = (LyricLine*)iter->data;
		line->x = 0;
		line->y = n * lyric->priv->inter;
	}
}

static gint
lyric_line_compare(LyricLine *line1, LyricLine *line2)
{
	if(line1->sec == line2->sec)
		return 0;
	if(line1->sec > line2->sec)
		return 1;

	return -1;
}

static gboolean
parse_lyric_file_without_check(SkinLyric *lyric, const gchar *filename)
{
	gchar buf[1024];
	gchar *pbuf;
	gint n;
	gboolean flag = FALSE;
	SkinLyricPrivate *priv = lyric->priv;

	FILE* fp = fopen(filename, "r");

	if(fp == NULL) return FALSE;

	n = 0;
	while(fgets(buf, 1024, fp))
	{
		g_strstrip(buf);
		if(strlen(buf) == 0 || buf[0] != '[')
			continue;

		if(g_utf8_validate(buf, -1, NULL))
			pbuf = g_strdup(buf);
		else 
			pbuf = g_convert(buf, -1, "utf-8", "gb18030", NULL, NULL, NULL);

		//title:
		if(strncmp(pbuf, "[ti:", 4) == 0)
		{
			pbuf[strlen(pbuf) - 1] = '\0';
			priv->title = (gchar*)g_strdup(&pbuf[4]);
			flag = TRUE;
			g_free(pbuf);
			continue;
		}

		//artist:
		if(strncmp(pbuf, "[ar:", 4) == 0)
		{
			pbuf[strlen(pbuf) - 1] = '\0';
			priv->artist = g_strdup(&pbuf[4]);
			flag = TRUE;
			g_free(pbuf);
			continue; 
		}

		//album:
		if(strncmp(pbuf, "[al:", 4) == 0)
		{
			pbuf[strlen(pbuf) - 1] = '\0';
			priv->album = g_strdup(&pbuf[4]);
			flag = TRUE;
			g_free(pbuf);
			continue;
		}

		//author
		if(strncmp(pbuf, "[by:", 4) == 0)
		{
			pbuf[strlen(pbuf) - 1] = '\0';
			priv->author = g_strdup(&pbuf[4]);
			flag = TRUE;
			g_free(pbuf);
			continue;
		}

		//offset:
		if(strncmp(pbuf, "[offset:", 8) == 0)
		{
			pbuf[strlen(pbuf) - 1] = '\0';
			priv->offset = atoi(&pbuf[8]);
			flag = TRUE;
			g_free(pbuf);
			continue;
		}

		flag = parse_lyric_line(lyric, pbuf, n);
		++n;
		g_free(pbuf);
	}

	fclose(fp);

	priv->lines = g_list_sort(priv->lines, (GCompareFunc)lyric_line_compare);
	update_xy_coordinate(lyric);

	return flag;
}


gboolean
skin_lyric_add_file(SkinLyric *lyric, const gchar *file)
{
	g_return_val_if_fail(SKIN_IS_LYRIC(lyric), FALSE);
	g_return_val_if_fail(file != NULL, FALSE);

	if(g_access(file, R_OK) != 0) return FALSE;

	if(lyric->priv->lines != NULL) 
	{
		g_list_free(lyric->priv->lines);
		lyric->priv->lines = NULL;
		lyric->priv->da_height = 0;
	}

	lyric->priv->loaded = parse_lyric_file_without_check(lyric, file);
	if(lyric->priv->loaded)
	{
		gtk_layout_move(GTK_LAYOUT(lyric), lyric->priv->alignment, 0, 0);
		
		gdk_window_invalidate_rect(lyric->priv->da->window, NULL, FALSE);
	}

	update_pixmap(lyric);

	return lyric->priv->loaded;
}

void 
skin_lyric_set_current_second(SkinLyric *lyric, gint sec)
{
	gint w, h;
	gint n;
	gboolean find = FALSE;
	GList *iter = NULL;
	LyricLine *line = NULL;
	SkinLyricPrivate *priv = NULL;

	g_return_if_fail(SKIN_IS_LYRIC(lyric));
	g_return_if_fail(sec >= 0);

	priv = lyric->priv;
	gtk_widget_get_size_request(GTK_WIDGET(lyric), &w, &h);

	for(iter = priv->lines, n = 0; iter; iter = iter->next, ++n)
	{
		line = iter->data;
		if(line && line->sec == sec)
		{
			find = TRUE;
			break;
		}
		else if(line && line->sec > sec)
		{
			find = FALSE;
			break;
		}

		find = FALSE;
	}
	
	if(find && line)
	{
		if(line->text != NULL)
		{
			gdk_gc_set_rgb_fg_color(priv->da_gc, &priv->current);
			pango_layout_set_text(priv->pango_layout, line->text, -1);
			pango_layout_set_width(priv->pango_layout, priv->da_width * PANGO_SCALE);
			pango_layout_set_alignment(priv->pango_layout, PANGO_ALIGN_CENTER);

			// 注意：这里是在da上直接绘，而不是在pixmap上
			gdk_draw_layout(priv->da->window, priv->da_gc, line->x, line->y, priv->pango_layout);
		}
		gtk_layout_move(GTK_LAYOUT(lyric), priv->alignment, 0, h / 2 - line->y);

		if(n > 0)
		{
			line = (LyricLine*)g_list_nth_data(priv->lines, n - 1);

			if(line->text != NULL)
			{
				gdk_gc_set_rgb_fg_color(priv->da_gc, &priv->fg);
				pango_layout_set_text(priv->pango_layout, line->text, -1);
				pango_layout_set_width(priv->pango_layout, priv->da_width * PANGO_SCALE);
				pango_layout_set_alignment(priv->pango_layout, PANGO_ALIGN_CENTER);

				// 注意：这里是在da上直接绘，而不是在pixmap上
				gdk_draw_layout(priv->da->window, priv->da_gc, line->x, line->y, priv->pango_layout);
			}
		}
	}
}

void 
skin_lyric_set_bg_color(SkinLyric *lyric, const GdkColor *color)
{
	g_return_if_fail(SKIN_IS_LYRIC(lyric));
	g_return_if_fail(color != NULL);
	
	lyric->priv->bg.red = color->red;
	lyric->priv->bg.green = color->green;
	lyric->priv->bg.blue = color->blue;
	lyric->priv->bg.pixel = color->pixel;

	if(GDK_IS_WINDOW(lyric->priv->da->window))
		gdk_window_invalidate_rect(lyric->priv->da->window, NULL, FALSE);
}

void 
skin_lyric_set_text_color(SkinLyric *lyric, const GdkColor *color)
{
	g_return_if_fail(SKIN_IS_LYRIC(lyric));
	g_return_if_fail(color != NULL);
	
	lyric->priv->fg.red = color->red;
	lyric->priv->fg.green = color->green;
	lyric->priv->fg.blue = color->blue;
	lyric->priv->fg.pixel = color->pixel;

	if(GDK_IS_WINDOW(lyric->priv->da->window))
		gdk_window_invalidate_rect(lyric->priv->da->window, NULL, FALSE);
}

void 
skin_lyric_set_highlight_color(SkinLyric *lyric, const GdkColor *color)
{
	g_return_if_fail(SKIN_IS_LYRIC(lyric));
	g_return_if_fail(color != NULL);
	
	lyric->priv->current.red = color->red;
	lyric->priv->current.green = color->green;
	lyric->priv->current.blue = color->blue;
	lyric->priv->current.pixel = color->pixel;

	if(GDK_IS_WINDOW(lyric->priv->da->window))
		gdk_window_invalidate_rect(lyric->priv->da->window, NULL, FALSE);
}

void
skin_lyric_set_color(SkinLyric *lyric, const GdkColor *bg,
									   const GdkColor *text,
									   const GdkColor *hilight)
{
	g_return_if_fail(SKIN_IS_LYRIC(lyric));

	if(bg)
	{
		lyric->priv->bg.red = bg->red;
		lyric->priv->bg.green = bg->green;
		lyric->priv->bg.blue = bg->blue;
		lyric->priv->bg.pixel = bg->pixel;
	}

	if(text)
	{
		lyric->priv->fg.red = text->red;
		lyric->priv->fg.green = text->green;
		lyric->priv->fg.blue = text->blue;
		lyric->priv->fg.pixel = text->pixel;
	}

	if(hilight)
	{
		lyric->priv->current.red = hilight->red;
		lyric->priv->current.green = hilight->green;
		lyric->priv->current.blue = hilight->blue;
		lyric->priv->current.pixel = hilight->pixel;
	}

	update_pixmap(lyric);
}

