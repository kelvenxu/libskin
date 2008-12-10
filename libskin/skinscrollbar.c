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

//static int signals[LAST_SIGNAL];

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
	gboolean self_value_changed;
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
	priv->self_value_changed = TRUE;
	gtk_adjustment_set_value(priv->adj, val);
	gtk_adjustment_value_changed(priv->adj);
}

static gint 
cb_thumb_event(GnomeCanvasItem *item, GdkEvent *event, SkinScrollBar* bar)
{
	static gboolean is_pressing = FALSE;
	static gdouble old_y;
	static gint count;
	gdouble item_y;

	SkinScrollBarPrivate *priv = bar->priv;

	item_y = event->button.y;
	switch (event->type) 
	{
	case GDK_BUTTON_PRESS:
		if(event->button.button == 1) 
		{
			gnome_canvas_item_set(item, "pixbuf", priv->thumb_subpb[1], NULL);
			is_pressing = TRUE;
			old_y = item_y;
		}
		break;
	case GDK_MOTION_NOTIFY:
		if(is_pressing && ++count % 8 == 0)
		{
			gdouble x1, y1, x2, y2;

			gnome_canvas_item_get_bounds(item, &x1, &y1, &x2, &y2);
			if(y1 >= priv->thumb_begin && y2 <= priv->thumb_end)
			{
				gdouble yy = item_y - old_y;

				gdouble dy1 = y1 + yy;
				gdouble dy2 = y2 + yy;
				
					
				if(dy1 < priv->thumb_begin)
				{
					yy = yy + (priv->thumb_begin - dy1);
				}

				if(dy2 > priv->thumb_end)
				{
					yy = yy - (dy2 - priv->thumb_end);
				}

				gnome_canvas_item_move(item, 0.0, yy);
				gdouble value = (y1 - priv->thumb_begin) / (priv->thumb_range - priv->thumb_height);
				skin_scroll_bar_update_adjustment(bar, value);
				old_y = item_y;
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

static gint 
cb_top_event(GnomeCanvasItem *item, GdkEvent *event, SkinScrollBar* bar)
{
	SkinScrollBarPrivate *priv = bar->priv;

	switch (event->type) 
	{
	case GDK_BUTTON_PRESS:
		if(event->button.button == 1) 
		{
			gnome_canvas_item_set(item, "pixbuf", priv->top_subpb[1], NULL);
			if(priv->adj->value > priv->adj->lower + 16)
				priv->adj->value -= 16;
			else
				priv->adj->value = priv->adj->lower;

			gtk_adjustment_value_changed(priv->adj);
		}
		break;
	case GDK_MOTION_NOTIFY:
			gnome_canvas_item_set(item, "pixbuf", priv->top_subpb[1], NULL);
		break;
	case GDK_BUTTON_RELEASE:
		gnome_canvas_item_set(item, "pixbuf", priv->top_subpb[1], NULL);
		break;
	default:
		gnome_canvas_item_set(item, "pixbuf", priv->top_subpb[0], NULL);
		break;
	}

	return FALSE;
}

static gint 
cb_bottom_event(GnomeCanvasItem *item, GdkEvent *event, SkinScrollBar* bar)
{
	SkinScrollBarPrivate *priv = bar->priv;

	switch (event->type) 
	{
	case GDK_BUTTON_PRESS:
		if(event->button.button == 1) 
		{
			gnome_canvas_item_set(item, "pixbuf", priv->bottom_subpb[1], NULL);
			if(priv->adj->value < priv->adj->upper - priv->adj->page_size - 16)
				priv->adj->value += 16;
			else
				priv->adj->value = priv->adj->upper - priv->adj->page_size;

			gtk_adjustment_value_changed(priv->adj);
		}
		break;
	case GDK_MOTION_NOTIFY:
			gnome_canvas_item_set(item, "pixbuf", priv->bottom_subpb[1], NULL);
		break;
	case GDK_BUTTON_RELEASE:
		gnome_canvas_item_set(item, "pixbuf", priv->bottom_subpb[1], NULL);
		break;
	default:
		gnome_canvas_item_set(item, "pixbuf", priv->bottom_subpb[0], NULL);
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

	g_signal_connect(G_OBJECT(priv->top_item), "event", G_CALLBACK(cb_top_event), bar);
	g_signal_connect(G_OBJECT(priv->bottom_item), "event", G_CALLBACK(cb_bottom_event), bar);
	g_signal_connect(G_OBJECT(priv->thumb_item), "event", G_CALLBACK(cb_thumb_event), bar);
}

// 对于x1, y1, x2, y2, 我期望使用的是playlist位置数据
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

static void
adj_value_changed_cb(GtkAdjustment *adj, SkinScrollBar *bar)
{
	SkinScrollBarPrivate *priv;
	gdouble y;
	gdouble x1, y1, x2, y2;

	priv = bar->priv;
	if(priv->self_value_changed)
	{
		// NOTE: 当SkinScrollBar自己改变了值时，这里不作处理
		priv->self_value_changed = FALSE;
		return;
	}

	gnome_canvas_item_get_bounds(priv->thumb_item, &x1, &y1, &x2, &y2);

	y = adj->value / (adj->upper - adj->lower - adj->page_size) 
		* (priv->thumb_range - priv->thumb_height)
		+ priv->thumb_begin;

	gnome_canvas_item_move(priv->thumb_item, 0.0, y - y1);
}

void
skin_scroll_bar_set_adjustment(SkinScrollBar *bar, GtkAdjustment *adj)
{
	g_return_if_fail(SKIN_IS_SCROLL_BAR(bar));
	g_return_if_fail(GTK_IS_ADJUSTMENT(adj));

	bar->priv->adj = adj;
	g_signal_connect(G_OBJECT(adj), "value-changed", G_CALLBACK(adj_value_changed_cb), bar);
}

gdouble
skin_scroll_bar_get_width(SkinScrollBar *bar)
{
	g_return_val_if_fail(SKIN_IS_SCROLL_BAR(bar), 0.0);
	
	return bar->priv->x2 - bar->priv->x1;
}

gdouble 
skin_scroll_bar_get_height(SkinScrollBar *bar)
{
	g_return_val_if_fail(SKIN_IS_SCROLL_BAR(bar), 0.0);

	return bar->priv->y2 - bar->priv->y1;
}

/**
 * skin_scroll_bar_set_height:
 * @bar: a SkinScrollBar.
 * @height:
 *
 * 设置SkinScrollBar的高度，其中变化的是fill和下面的一个.
 */
void
skin_scroll_bar_set_height(SkinScrollBar *bar, gdouble height)
{
	SkinScrollBarPrivate *priv;
	gdouble rh;

	g_return_if_fail(SKIN_IS_SCROLL_BAR(bar));

	priv = bar->priv;
	rh = height - (priv->y2 - priv->y1);
	priv->y2 = priv->y2 + rh;
	priv->thumb_end = priv->thumb_end + rh;
	priv->thumb_range = priv->thumb_range + rh;

	gnome_canvas_item_set(priv->bar_item,
			"height", priv->thumb_range,
			"height-set", TRUE,
			NULL);

	gnome_canvas_item_set(priv->bottom_item,
			"y", priv->thumb_end,
			NULL);

	gnome_canvas_item_set(priv->thumb_item,
			"y", priv->thumb_begin,
			NULL);
}

static void 
skin_scroll_bar_reconstruct(SkinScrollBar *bar)
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
		if(priv->top_subpb[i])
			g_object_unref(priv->top_subpb[i]);
		if(priv->bottom_subpb[i])
			g_object_unref(priv->bottom_subpb[i]);

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
		if(priv->thumb_subpb[i])
			g_object_unref(priv->thumb_subpb[i]);

		priv->thumb_subpb[i] = gdk_pixbuf_new_subpixbuf(priv->thumb_pixbuf, 
				(int)(i * w), 0, (int)w, (int)h);
	}

	gnome_canvas_item_set(priv->top_item,
			"x", 0.0,
			"y", 0.0,
			"pixbuf", priv->top_subpb[0],
			"width", w,
			"width-set", TRUE,
			NULL);

	gnome_canvas_item_set(priv->bottom_item,
			"x", 0.0,
			"y", priv->thumb_end,
			"pixbuf", priv->bottom_subpb[0],
			"width", priv->x2 - priv->x1,
			"width-set", TRUE,
			NULL);

	GdkPixbuf *pixbuf = gdk_pixbuf_scale_simple(priv->bar_pixbuf, 
			priv->x2 - priv->x1, priv->thumb_range, 
			GDK_INTERP_BILINEAR);

	gnome_canvas_item_set(priv->bar_item,
			"x", 0,0,
			"y", priv->thumb_begin,
			"pixbuf", pixbuf,
			NULL);

	gnome_canvas_item_set(priv->thumb_item,
			"x", 0.0,
			"y", priv->thumb_begin,
			"pixbuf", priv->thumb_subpb[0],
			NULL);

}

void 
skin_scroll_bar_rebuild(SkinScrollBar *bar, 
		GdkPixbuf *button_pixbuf, 
		GdkPixbuf *bar_pixbuf, 
		GdkPixbuf *thumb_pixbuf,
		gdouble x1, //not used 
		gdouble y1,
		gdouble x2,
		gdouble y2)
{
	SkinScrollBarPrivate *priv;

	g_return_if_fail(SKIN_IS_SCROLL_BAR(bar));
	g_return_if_fail(GDK_IS_PIXBUF(button_pixbuf));
	g_return_if_fail(GDK_IS_PIXBUF(bar_pixbuf));
	g_return_if_fail(GDK_IS_PIXBUF(thumb_pixbuf));
	g_return_if_fail((x1 >= 0.0 || y1 >= 0.0));
	g_return_if_fail((x2 >= 0.0 || y2 >= 0.0));
	g_return_if_fail((x2 > x1 || y2 > y1));

	priv = bar->priv;

	if(priv->button_pixbuf)
	{
		g_object_unref(priv->button_pixbuf);
	}

	if(priv->bar_pixbuf)
	{
		g_object_unref(priv->bar_pixbuf);
	}

	if(priv->thumb_pixbuf)
	{
		g_object_unref(priv->thumb_pixbuf);
	}

	priv->button_pixbuf = gdk_pixbuf_copy(button_pixbuf);
	priv->bar_pixbuf = gdk_pixbuf_copy(bar_pixbuf);
	priv->thumb_pixbuf = gdk_pixbuf_copy(thumb_pixbuf);

	priv->y1 = y1;
	priv->x2 = x2;
	priv->y2 = y2;

	skin_scroll_bar_reconstruct(bar);
}

