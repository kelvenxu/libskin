
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

#define SUBPIXBUF 12

enum {
    PROP_0,
};

#define SKIN_DIGITAL_TIME_GET_PRIVATE(obj)  (G_TYPE_INSTANCE_GET_PRIVATE((obj), SKIN_TYPE_DIGITAL_TIME, SkinDigitalTimePrivate))


struct _SkinDigitalTimePrivate
{
	GdkPixbuf *subpixbuf[SUBPIXBUF];
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
skin_digital_time_init (SkinDigitalTime *skin_digital_time)
{
    skin_digital_time->priv = SKIN_DIGITAL_TIME_GET_PRIVATE (skin_digital_time);
}

GType
skin_digital_time_get_type(void)
{
	static GType skin_digital_time_type = 0;
	
	if(!skin_digital_time_type)
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

		skin_digital_time_type = 
		g_type_register_static(gnome_canvas_pixbuf_get_type(),
				"SkinDigitalTime", &skin_digital_time_info, 0);
	}

	return skin_digital_time_type;
}

SkinDigitalTime*
skin_digital_time_new(GnomeCanvasGroup *root, GdkPixbuf *pixbuf, gdouble x, gdouble y)
{
	SkinDigitalTime *digital_time;
	GnomeCanvasItem *item;
	gdouble pw;
	gdouble ph;
	gdouble w;
	gdouble h;
	gint i;
	
	item = gnome_canvas_item_new(root,
			skin_digital_time_get_type(),
			NULL);

	digital_time = SKIN_DIGITAL_TIME(item);

	pw = gdk_pixbuf_get_width(pixbuf);
	ph = gdk_pixbuf_get_height(pixbuf);
	  
	w = pw / SUBPIXBUF;
	h = ph;
	
	for(i = 0; i < SUBPIXBUF; ++i)
	{
		digital_time->priv->subpixbuf[i] = gdk_pixbuf_new_subpixbuf(pixbuf, w * i, 0, w, h);
	}

	gnome_canvas_item_set(item, 
			"pixbuf", digital_time->priv->subpixbuf[SUBPIXBUF - 1], 
			"x", x, 
			"y", y, 
			NULL);

    return digital_time;
}

static void
skin_digital_time_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    SkinDigitalTime *skin_digital_time;

    skin_digital_time = SKIN_DIGITAL_TIME (object);

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
    SkinDigitalTime *skin_digital_time;

    skin_digital_time = SKIN_DIGITAL_TIME (object);

    switch (prop_id)
    {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}


void skin_digital_time_show(SkinDigitalTime *digital_time)
{
	gnome_canvas_item_show(GNOME_CANVAS_ITEM(digital_time));
}

void skin_digital_time_hide(SkinDigitalTime *digital_time)
{
	gnome_canvas_item_hide(GNOME_CANVAS_ITEM(digital_time));
}

void skin_digital_time_set_value(SkinDigitalTime *digital_time, int value)
{
	g_return_if_fail((value < SUBPIXBUF) && (value >= 0));

	gnome_canvas_item_set(GNOME_CANVAS_ITEM(digital_time),
			"pixbuf", digital_time->priv->subpixbuf[value],
			NULL);
}

