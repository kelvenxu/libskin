/* vi: set sw=4 ts=4: */
/*
 * skinlyric.c
 *
 * This file is part of ________.
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
	priv->songname = NULL;
	priv->lyricname = NULL;

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

static gboolean
da_expose_cb(GtkWidget *widget, GdkEventExpose *event, SkinLyric *lyric)
{ 
	GList *iter;
	LyricLine *line;
	GdkGC *gc;
	PangoContext *context;
	PangoLayout *layout;
	SkinLyricPrivate *priv = lyric->priv;

	gc = gdk_gc_new(widget->window);
	gdk_gc_set_rgb_fg_color(gc, &priv->bg);
	gdk_draw_rectangle(widget->window, gc, TRUE,
			0, 0,
			priv->da_width, priv->da_height);

	if(!priv->loaded)
		return TRUE;

	context = gtk_widget_get_pango_context(widget);
	layout = pango_layout_new(context);
	gdk_gc_set_rgb_fg_color(gc, &priv->fg);

	iter = priv->lines;
	for(iter = priv->lines; iter; iter = iter->next)
	{
		line = (LyricLine*)iter->data;
		pango_layout_set_text(layout, line->text, -1);
		gdk_draw_layout(widget->window, gc, line->x, line->y, layout);
	}

	line = (LyricLine*)g_list_nth_data(priv->lines, priv->index);
	gdk_gc_set_rgb_fg_color(gc, &priv->current);
	pango_layout_set_text(layout, line->text, -1);
	gdk_draw_layout(widget->window, gc, line->x, line->y, layout);

	gdk_draw_line(widget->window,
			widget->style->fg_gc[GTK_WIDGET_STATE (widget)], 
			0, 0,
			200, 200);

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
	priv->da = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(priv->alignment), priv->da);
	gtk_container_add(GTK_CONTAINER(lyric), priv->alignment);

	gtk_widget_set_events(priv->da, GDK_EXPOSURE_MASK);
	g_signal_connect(G_OBJECT(priv->da), "expose-event", G_CALLBACK(da_expose_cb), NULL);

	return lyric;
}

void
skin_lyric_set_size(SkinLyric *lyric, gint width, gint height)
{
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
parse_lyric_line(SkinLyric *lyric, const gchar *line)
{
	char** p;
	char* text;
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
			priv->lines = g_list_append(priv->lines, data);
		}
	}

	g_strfreev(parray);

	return flag;
}

static gboolean
parse_lyric_file_without_check(SkinLyric *lyric, const gchar *filename)
{
	gchar buf[1024];
	gchar *pbuf;
	SkinLyricPrivate *priv = lyric->priv;
	gboolean flag = FALSE;

	FILE* fp = fopen(filename, "r");

	if(!fp) return FALSE;

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

		flag = parse_lyric_line(lyric, pbuf);
		g_free(pbuf);
	}

	return flag;
}

gboolean
skin_lyric_add_file(SkinLyric *lyric, const gchar *file)
{
	g_return_val_if_fail(SKIN_IS_LYRIC(lyric), FALSE);
	g_return_val_if_fail(file != NULL, FALSE);

	if(g_access(file, R_OK) != 0) return FALSE;

	if(lyric->priv->lines) 
	{
		g_list_free(lyric->priv->lines);
		lyric->priv->lines = NULL;
	}

	return parse_lyric_file_without_check(lyric, file);
}

void 
skin_lyric_set_current_second(SkinLyric *lyric, gint sec)
{
}

void 
skin_lyric_set_bg_color(SkinLyric *lyric, const GdkColor *color)
{
}

void 
skin_lyric_set_fg_color(SkinLyric *lyric, const GdkColor *color)
{
}

void 
skin_lyric_set_active_color(SkinLyric *lyric, const GdkColor *color)
{
}

