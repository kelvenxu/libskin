
/* vi: set sw=4 ts=4: */
/*
 * skindigitaltime.c
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

#include "skindigitaltime.h"
#include <math.h>

#define TIME_SP 10
#define TIME_NONE 11

#define SUBPIXBUF 12

enum {
    PROP_0,
};

#define SKIN_DIGITAL_TIME_GET_PRIVATE(obj)  (G_TYPE_INSTANCE_GET_PRIVATE((obj), SKIN_TYPE_DIGITAL_TIME, SkinDigitalTimePrivate))


struct _SkinDigitalTimePrivate
{
	gdouble x;
	gdouble y;

	// 单个元素的尺寸
	gdouble w;
	gdouble h;

	GdkPixbuf *pixbuf;
	GdkPixbuf *subpixbuf[SUBPIXBUF];

	GnomeCanvasGroup *root;
	GnomeCanvasItem *items[5];

	gint value;
};

static void skin_digital_time_set_property  (GObject          *object,
                                         guint             prop_id,
                                         const GValue     *value,
                                         GParamSpec       *pspec);
static void skin_digital_time_get_property  (GObject          *object,
                                         guint             prop_id,
                                         GValue           *value,
                                         GParamSpec       *pspec);

static void
skin_digital_time_class_init (SkinDigitalTimeClass *class)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (class);

    gobject_class->set_property = skin_digital_time_set_property;
    gobject_class->get_property = skin_digital_time_get_property;

    g_type_class_add_private (class, sizeof (SkinDigitalTimePrivate));
}

static void
skin_digital_time_init (SkinDigitalTime *dt)
{
    dt->priv = SKIN_DIGITAL_TIME_GET_PRIVATE(dt);
	dt->priv->value = 0;
}

GType
skin_digital_time_get_type(void)
{
	static GType dt_type = 0;
	
	if(!dt_type)
	{
		static const GTypeInfo skin_digital_time_info =
		{
			sizeof(SkinDigitalTimeClass),
			NULL,
			NULL,
			(GClassInitFunc)skin_digital_time_class_init,
			NULL,
			NULL,
			sizeof(SkinDigitalTime),
			0,
			(GInstanceInitFunc)skin_digital_time_init
		};

		dt_type = g_type_register_static(gnome_canvas_group_get_type(),
				"SkinDigitalTime", &skin_digital_time_info, 0);
	}

	return dt_type;
}

static void
skin_digital_time_construct(SkinDigitalTime *dt)
{
	GnomeCanvasGroup *group;
	SkinDigitalTimePrivate *priv;
	gdouble w;
	int i;

	g_return_if_fail(SKIN_IS_DIGITAL_TIME(dt));

	priv = dt->priv;

	group = GNOME_CANVAS_GROUP(dt);

	w = (gdouble)gdk_pixbuf_get_width(priv->pixbuf);

	priv->w = w / (gdouble)SUBPIXBUF;
	priv->h = (gdouble)gdk_pixbuf_get_height(priv->pixbuf);

	priv->w = floor(priv->w + 0.5);
	for(i = 0; i < SUBPIXBUF - 1; ++i)
	{
		priv->subpixbuf[i] = 
			gdk_pixbuf_new_subpixbuf(priv->pixbuf, (gint)(priv->w * i), 0, (gint)priv->w, (gint)priv->h);
	}

	priv->subpixbuf[SUBPIXBUF - 1] = 
		gdk_pixbuf_new_subpixbuf(priv->pixbuf, (gint)(w - priv->w), 0, (gint)priv->w, (gint)priv->h);

	// 是相对于group的x，y
	priv->items[0] = gnome_canvas_item_new(group, 
			gnome_canvas_pixbuf_get_type(),
			"pixbuf", dt->priv->subpixbuf[TIME_NONE],
			"x", 0.0,
			"y", 0.0,
			NULL);

	priv->items[1] = gnome_canvas_item_new(group, 
			gnome_canvas_pixbuf_get_type(),
			"pixbuf", dt->priv->subpixbuf[TIME_NONE],
			"x", priv->w + 1.0,
			"y", 0.0,
			NULL);

	priv->items[2] = gnome_canvas_item_new(group, 
			gnome_canvas_pixbuf_get_type(),
			"pixbuf", dt->priv->subpixbuf[TIME_SP],
			"x", priv->w * 2.0 + 1.0,
			"y", 0.0,
			NULL);

	priv->items[3] = gnome_canvas_item_new(group, 
			gnome_canvas_pixbuf_get_type(),
			"pixbuf", dt->priv->subpixbuf[TIME_NONE],
			"x", priv->w * 3.0 + 1.0,
			"y", 0.0,
			NULL);

	priv->items[4] = gnome_canvas_item_new(group, 
			gnome_canvas_pixbuf_get_type(),
			"pixbuf", dt->priv->subpixbuf[TIME_NONE],
			"x", priv->w * 4.0 + 1.0,
			"y", 0.0,
			NULL);
}

SkinDigitalTime*
skin_digital_time_new(GnomeCanvasGroup *root, GdkPixbuf *pixbuf, gdouble x, gdouble y)
{
	SkinDigitalTime *digital_time;
	SkinDigitalTimePrivate *priv;
	GnomeCanvasItem *item;
	
	g_return_val_if_fail(GNOME_IS_CANVAS_GROUP(root), NULL);
	g_return_val_if_fail(GDK_IS_PIXBUF(pixbuf), NULL);
	g_return_val_if_fail((x >= 0.0 || y >= 0.0), NULL);
	
	item = gnome_canvas_item_new(root,
			skin_digital_time_get_type(),
			"x", x,
			"y", y,
			NULL);

	digital_time = SKIN_DIGITAL_TIME(item);

	priv = digital_time->priv;
	g_assert(priv);
	priv->root = root;
	priv->x = x;
	priv->y = y;
	priv->pixbuf = gdk_pixbuf_copy(pixbuf);

	skin_digital_time_construct(digital_time);

    return digital_time;
}

static void
skin_digital_time_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    SkinDigitalTime *dt;

    dt = SKIN_DIGITAL_TIME (object);

    switch (prop_id)
    {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
skin_digital_time_get_property (GObject      *object,
                            guint         prop_id,
                            GValue       *value,
                            GParamSpec   *pspec)
{
    SkinDigitalTime *dt;

    dt = SKIN_DIGITAL_TIME (object);

    switch (prop_id)
    {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}


void skin_digital_time_show(SkinDigitalTime *dt)
{
	gnome_canvas_item_show(GNOME_CANVAS_ITEM(dt));
}

void skin_digital_time_hide(SkinDigitalTime *dt)
{
	gnome_canvas_item_hide(GNOME_CANVAS_ITEM(dt));
}

void 
skin_digital_time_set_value(SkinDigitalTime *dt, int value)
{
	int m1, m2;
	int s1, s2; //MM:SS >> m1m2:s1s2

	g_return_if_fail(SKIN_IS_DIGITAL_TIME(dt));
	g_return_if_fail(value >= 0);

	dt->priv->value = value;

	if(value > 3600) value = 3600;

	m2 = value / 60;
	m1 = m2 / 10;
	m2 = m2 % 10;

	s2 = value % 60;
	s1 = s2 / 10;
	s2 = s2 % 10;

	gnome_canvas_item_set(dt->priv->items[0],
			"pixbuf", dt->priv->subpixbuf[m1],
			NULL);
	gnome_canvas_item_set(dt->priv->items[1],
			"pixbuf", dt->priv->subpixbuf[m2],
			NULL);

	gnome_canvas_item_set(dt->priv->items[3],
			"pixbuf", dt->priv->subpixbuf[s1],
			NULL);
	gnome_canvas_item_set(dt->priv->items[4],
			"pixbuf", dt->priv->subpixbuf[s2],
			NULL);
}

void
skin_digital_time_set_pixbuf(SkinDigitalTime *dt, GdkPixbuf *pixbuf)
{
	SkinDigitalTimePrivate *priv;
	gdouble w;
	gint i;

	g_return_if_fail(SKIN_IS_DIGITAL_TIME(dt));
	
	priv = dt->priv;
	if(priv->pixbuf)
		g_object_unref(priv->pixbuf);

	priv->pixbuf = pixbuf;
	w = (gdouble)gdk_pixbuf_get_width(priv->pixbuf);
	priv->w = w / (gdouble)SUBPIXBUF;
	priv->h = (gdouble)gdk_pixbuf_get_height(priv->pixbuf);
	priv->w = floor(priv->w + 0.5);

	for(i = 0; i < SUBPIXBUF - 1; ++i)
	{
		if(priv->subpixbuf[i])
			g_object_unref(priv->subpixbuf[i]);

		priv->subpixbuf[i] = 
			gdk_pixbuf_new_subpixbuf(priv->pixbuf, (gint)(priv->w * i), 0, (gint)priv->w, (gint)priv->h);
	}

	if(priv->subpixbuf[SUBPIXBUF - 1])
		g_object_unref(priv->subpixbuf[SUBPIXBUF - 1]);
	priv->subpixbuf[SUBPIXBUF - 1] = 
		gdk_pixbuf_new_subpixbuf(priv->pixbuf, (gint)(w - priv->w), 0, (gint)priv->w, (gint)priv->h);

	gnome_canvas_item_set(priv->items[0],
			"x", 0.0,
			"y", 0.0,
			NULL);
	gnome_canvas_item_set(priv->items[1],
			"x", priv->w + 1.0,
			"y", 0.0,
			NULL);
	gnome_canvas_item_set(priv->items[2],
			"x", priv->w * 2.0 + 1.0,
			"y", 0.0,
			"pixbuf", priv->subpixbuf[TIME_SP],
			NULL);
	gnome_canvas_item_set(priv->items[3],
			"x", priv->w * 3.0 + 1.0,
			"y", 0.0,
			NULL);
	gnome_canvas_item_set(priv->items[4],
			"x", priv->w * 4.0 + 1.0,
			"y", 0.0,
			NULL);

	skin_digital_time_set_value(dt, priv->value);
}

void
skin_digital_time_set_position(SkinDigitalTime *dt, gdouble x, gdouble y)
{
	g_return_if_fail(SKIN_IS_DIGITAL_TIME(dt));

	gnome_canvas_item_set(GNOME_CANVAS_ITEM(dt),
			"x", x,
			"y", y,
			NULL);
}

