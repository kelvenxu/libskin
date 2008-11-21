/* vi: set sw=4 ts=4: */
/*
 * skinscrollbar.c
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

#include "skinscrollbar.h"

#define SUBPIXBUF 3

enum {
	CLICKED,
	ACTIVATED,
    LAST_SIGNAL
};

enum {
    PROP_0,
};

static int signals[LAST_SIGNAL];

G_DEFINE_TYPE (SkinScrollBar, skin_scroll_bar, GNOME_TYPE_CANVAS_GROUP);

#define SKIN_SCROLL_BAR_GET_PRIVATE(o)\
	(G_TYPE_INSTANCE_GET_PRIVATE ((o), SKIN_TYPE_SCROLL_BAR, SkinScrollBarPrivate))

struct _SkinScrollBarPrivate 
{
	GnomeCanvasGroup *root;

	GnomeCanvasItem *top_item;
	GnomeCanvasItem *bottom_item;
	GnomeCanvasItem *bar_item;
	GnomeCanvasItem *thumb_item;

	GdkPixbuf *button_pixbuf;
	GdkPixbuf *bar_pixbuf;
	GdkPixbuf *thumb_pixbuf;

	GdkPixbuf *top_subpb[SUBPIXBUF];
	GdkPixbuf *bottom_subpb[SUBPIXBUF];
	GdkPixbuf *thumb_subpb[SUBPIXBUF];

	gdouble x1;
	gdouble y1;
	gdouble x2;
	gdouble y2;

	gdouble thumb_begin;
	gdouble thumb_end;
	gdouble thumb_range;
	gdouble thumb_height;

	GtkAdjustment *adj;
};


static void
skin_scroll_bar_dispose (SkinScrollBar *self)
{
}

static void
skin_scroll_bar_finalize (SkinScrollBar *self)
{
}

static void
skin_scroll_bar_init (SkinScrollBar *self)
{
	SkinScrollBarPrivate *priv;

	priv = SKIN_SCROLL_BAR_GET_PRIVATE (self);
	self->priv = priv;
	priv->adj = NULL;
}

static void
skin_scroll_bar_class_init (SkinScrollBarClass *self_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (self_class);

	g_type_class_add_private (self_class, sizeof (SkinScrollBarPrivate));
	object_class->dispose = (void (*) (GObject *object)) skin_scroll_bar_dispose;
	object_class->finalize = (void (*) (GObject *object)) skin_scroll_bar_finalize;
}

static void
skin_scroll_bar_update_adjustment(SkinScrollBar *bar, gdouble value)
{
	SkinScrollBarPrivate *priv;
	gdouble val;

	g_return_if_fail(SKIN_IS_SCROLL_BAR(bar));
	g_return_if_fail(GTK_IS_ADJUSTMENT(bar->priv->adj));

	priv = bar->priv;

	val = value * (priv->adj->upper - priv->adj->page_size - priv->adj->lower);
	gtk_adjustment_set_value(priv->adj, val);
	gtk_adjustment_value_changed(priv->adj);
}

static gint 
cb_thumb_event(GnomeCanvasItem *item, GdkEvent *event, SkinScrollBar* bar)
{
	static gboolean is_pressing = FALSE;
	static gdouble y;
	gdouble item_y;
	gdouble x1, y1, x2, y2;
	gdouble yy;

	SkinScrollBarPrivate *priv = bar->priv;

	item_y = event->button.y;
	switch (event->type) 
	{
	case GDK_BUTTON_PRESS:
		if(event->button.button == 1) 
		{
			gnome_canvas_item_set(item, "pixbuf", priv->thumb_subpb[1], NULL);
			is_pressing = TRUE;
			y = item_y;
		}
		break;
	case GDK_MOTION_NOTIFY:
		if(is_pressing)
		{
			gnome_canvas_item_get_bounds(item, &x1, &y1, &x2, &y2);
			if((y1 > priv->thumb_begin && y2 < priv->thumb_end)
					|| (y1 == priv->thumb_begin && item_y - y > 0)
					|| (y2 == priv->thumb_end && item_y - y < 0))
			{
				yy = item_y - y;
				if(yy > 1)
				{
					y++;
					yy = 1;
				}
				else if(yy < -1)
				{
					y--;
					yy = -1;
				}
				gnome_canvas_item_move(item, 0.0, yy);
				gdouble value = (y1 - priv->thumb_begin) / (priv->thumb_range - priv->thumb_height);
				skin_scroll_bar_update_adjustment(bar, value);
			}
		}
		break;
	case GDK_BUTTON_RELEASE:
		gnome_canvas_item_set(item, "pixbuf", priv->thumb_subpb[1], NULL);
		is_pressing = FALSE;
		break;
	default:
		gnome_canvas_item_set(item, "pixbuf", priv->thumb_subpb[0], NULL);
		break;
	}

	return FALSE;
}

static void 
skin_scroll_bar_construct(SkinScrollBar *bar)
{
	SkinScrollBarPrivate *priv;
	GnomeCanvasGroup *group;
	gdouble w, h;
	gint i;

	g_return_if_fail(SKIN_IS_SCROLL_BAR(bar));

	priv = bar->priv;
	group = GNOME_CANVAS_GROUP(bar);

	w = (gdouble)gdk_pixbuf_get_width(priv->bar_pixbuf);
	priv->x1 = priv->x2 - w;

	gnome_canvas_item_set(GNOME_CANVAS_ITEM(group), 
			"x", priv->x1, 
			"y", priv->y1, 
			NULL);

	// 以下是在group中的位置
	w = (gdouble)gdk_pixbuf_get_width(priv->button_pixbuf) / 3.0;
	h = (gdouble)gdk_pixbuf_get_height(priv->button_pixbuf) / 2.0;

	priv->thumb_begin = h;
	priv->thumb_end = (priv->y2 - priv->y1) - h;
	priv->thumb_range = priv->thumb_end - priv->thumb_begin;

	for(i = 0; i < SUBPIXBUF; ++i)
	{
		priv->top_subpb[i] = gdk_pixbuf_new_subpixbuf(priv->button_pixbuf, 
				(int)(i * w), 0, (int)w, (int)h);
		priv->bottom_subpb[i] = gdk_pixbuf_new_subpixbuf(priv->button_pixbuf, 
				(int)(i * w), (int)h, (int)w, (int)h);
	}

	w = (gdouble)gdk_pixbuf_get_width(priv->thumb_pixbuf) / 3.0;
	h = (gdouble)gdk_pixbuf_get_height(priv->thumb_pixbuf);
	priv->thumb_height = h;
	for(i = 0; i < SUBPIXBUF; ++i)
	{
		priv->thumb_subpb[i] = gdk_pixbuf_new_subpixbuf(priv->thumb_pixbuf, 
				(int)(i * w), 0, (int)w, (int)h);
	}

	priv->top_item =  gnome_canvas_item_new(group,
			gnome_canvas_pixbuf_get_type(),
			"x", 0.0,
			"y", 0.0,
			"pixbuf", priv->top_subpb[0],
			"width", w,
			"width-set", TRUE,
			NULL);

	priv->bottom_item = gnome_canvas_item_new(group,
			gnome_canvas_pixbuf_get_type(),
			"x", 0.0,
			"y", priv->thumb_end,
			"pixbuf", priv->bottom_subpb[0],
			"width", priv->x2 - priv->x1,
			"width-set", TRUE,
			NULL);

	GdkPixbuf *pixbuf = gdk_pixbuf_scale_simple(priv->bar_pixbuf, 
			priv->x2 - priv->x1, priv->thumb_range, 
			GDK_INTERP_BILINEAR);
	priv->bar_item = gnome_canvas_item_new(group,
			gnome_canvas_pixbuf_get_type(),
			"x", 0,0,
			"y", priv->thumb_begin,
			"pixbuf", pixbuf,
			NULL);

	priv->thumb_item = gnome_canvas_item_new(group,
			gnome_canvas_pixbuf_get_type(),
			"x", 0.0,
			"y", priv->thumb_begin,
			"pixbuf", priv->thumb_subpb[0],
			NULL);

	g_signal_connect(G_OBJECT(priv->thumb_item), "event", G_CALLBACK(cb_thumb_event), bar);
}

// 对于x1, y1, x2, y2, 我们期望使用playlist位置数据
SkinScrollBar *
skin_scroll_bar_new(GnomeCanvasGroup *root, 
		GdkPixbuf *button_pixbuf, 
		GdkPixbuf *bar_pixbuf, 
		GdkPixbuf *thumb_pixbuf,
		gdouble x1, //not used 
		gdouble y1,
		gdouble x2,
		gdouble y2)
{
	SkinScrollBar *bar;
	SkinScrollBarPrivate *priv;
	GnomeCanvasItem *item;

	g_return_val_if_fail(GNOME_IS_CANVAS_GROUP(root), NULL);
	g_return_val_if_fail(GDK_IS_PIXBUF(button_pixbuf), NULL);
	g_return_val_if_fail(GDK_IS_PIXBUF(bar_pixbuf), NULL);
	g_return_val_if_fail(GDK_IS_PIXBUF(thumb_pixbuf), NULL);
	g_return_val_if_fail((x1 >= 0.0 || y1 >= 0.0), NULL);
	g_return_val_if_fail((x2 >= 0.0 || y2 >= 0.0), NULL);
	g_return_val_if_fail((x2 > x1 || y2 > y1), NULL);

	item = gnome_canvas_item_new(root,
			skin_scroll_bar_get_type(),
			NULL);

	bar = SKIN_SCROLL_BAR(item);
	priv = bar->priv;

	priv->root = root;
	priv->button_pixbuf = gdk_pixbuf_copy(button_pixbuf);
	priv->bar_pixbuf = gdk_pixbuf_copy(bar_pixbuf);
	priv->thumb_pixbuf = gdk_pixbuf_copy(thumb_pixbuf);
	priv->y1 = y1;
	priv->x2 = x2;
	priv->y2 = y2;

	skin_scroll_bar_construct(bar);

	return bar;
}

void
skin_scroll_bar_set_adjustment(SkinScrollBar *bar, GtkAdjustment *adj)
{
	g_return_if_fail(SKIN_IS_SCROLL_BAR(bar));
	g_return_if_fail(GTK_IS_ADJUSTMENT(adj));

	bar->priv->adj = adj;
}

gdouble
skin_scroll_bar_get_width(SkinScrollBar *bar)
{
	g_return_val_if_fail(SKIN_IS_SCROLL_BAR(bar), 0.0);
	
	return bar->priv->x2 - bar->priv->x1;
}

