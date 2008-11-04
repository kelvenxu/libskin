/* vi: set sw=4 ts=4: */
/*
 * skinbutton.c
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

#include "skintogglebutton.h"

enum {
	CLICKED,
    LAST_SIGNAL
};

enum {
    PROP_0,
};

static int signals[LAST_SIGNAL];

#define SKIN_TOGGLE_BUTTON_GET_PRIVATE(obj)  (G_TYPE_INSTANCE_GET_PRIVATE((obj), SKIN_TYPE_TOGGLE_BUTTON, SkinToggleButtonPrivate))

struct _SkinToggleButtonPrivate
{
	GdkPixbuf *pixbuf;
	gdouble x0;
	gdouble y0;
};

static void skin_toggle_button_set_property  (GObject          *object,
                                         guint             prop_id,
                                         const GValue     *value,
                                         GParamSpec       *pspec);
static void skin_toggle_button_get_property  (GObject          *object,
                                         guint             prop_id,
                                         GValue           *value,
                                         GParamSpec       *pspec);

static gint cb_event (GnomeCanvasItem *item, 
						GdkEvent *event, 
						SkinToggleButton* skin_toggle_button);

//G_DEFINE_TYPE (SkinToggleButton, skin_toggle_button, SKIN_TYPE_TOGGLE_BUTTON);

static gint 
cb_event (GnomeCanvasItem *item, GdkEvent *event, SkinToggleButton* button)
{
	static gboolean is_pressing = FALSE;

	switch (event->type) 
	{
	case GDK_BUTTON_PRESS:
		if(event->button.button == 1) 
		{
			gnome_canvas_item_set(item, "y", button->priv->y0 + 1.0, NULL);
			is_pressing = TRUE;
		}
		break;
	case GDK_MOTION_NOTIFY:
		if(!is_pressing)
			gnome_canvas_item_set(item, "y", button->priv->y0 + 1.0, NULL);
		break;
	case GDK_BUTTON_RELEASE:
		gnome_canvas_item_set(item, "y", button->priv->y0, NULL);
		g_signal_emit(button, signals[CLICKED], 0, NULL);
		is_pressing = FALSE;
		break;
	default:
		gnome_canvas_item_set(item, "y", button->priv->y0, NULL);
		break;
	}

	return FALSE;
}

static void
skin_toggle_button_class_init (SkinToggleButtonClass *class)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (class);

    gobject_class->set_property = skin_toggle_button_set_property;
    gobject_class->get_property = skin_toggle_button_get_property;

    g_type_class_add_private (class, sizeof (SkinToggleButtonPrivate));

	signals[CLICKED] = g_signal_new("clicked", 
			G_TYPE_FROM_CLASS(gobject_class),
			G_SIGNAL_RUN_LAST,
			G_STRUCT_OFFSET(SkinToggleButtonClass, clicked),
			NULL, 
			NULL,
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE, 
			0, 
			NULL);

	class->clicked = NULL;
}

static void
skin_toggle_button_init (SkinToggleButton *skin_toggle_button)
{
    skin_toggle_button->priv = SKIN_TOGGLE_BUTTON_GET_PRIVATE (skin_toggle_button);
}

GType
skin_toggle_button_get_type(void)
{
	static GType skin_toggle_button_type = 0;
	
	if(!skin_toggle_button_type)
	{
		static const GTypeInfo skin_toggle_button_info =
		{
			sizeof(SkinToggleButtonClass),
			NULL,
			NULL,
			(GClassInitFunc)skin_toggle_button_class_init,
			NULL,
			NULL,
			sizeof(SkinToggleButton),
			0,
			(GInstanceInitFunc)skin_toggle_button_init
		};

		skin_toggle_button_type = 
		g_type_register_static(gnome_canvas_pixbuf_get_type(),
				"SkinToggleButton", &skin_toggle_button_info, 0);
	}

	return skin_toggle_button_type;
}

SkinToggleButton*
skin_toggle_button_new(GnomeCanvasGroup *root, GdkPixbuf *pixbuf, gdouble x, gdouble y)
{
	SkinToggleButton *button;
	GnomeCanvasItem *item;
	
	item = gnome_canvas_item_new(root,
			skin_toggle_button_get_type(),
			NULL);

	button = SKIN_TOGGLE_BUTTON(item);

	button->priv->pixbuf = gdk_pixbuf_copy(pixbuf);

	gnome_canvas_item_set(item, 
			"pixbuf", button->priv->pixbuf, 
			"x", x, 
			"y", y, 
			NULL);

	button->priv->x0 = x;
	button->priv->y0 = y;

	g_signal_connect(G_OBJECT(item), "event", G_CALLBACK(cb_event), button);

    return button;
}

SkinToggleButton*
skin_toggle_button_new_from_pixbuf(GnomeCanvasGroup *root, GdkPixbuf *pixbuf)
{
	return NULL;
}

static void
skin_toggle_button_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    SkinToggleButton *skin_toggle_button;

    skin_toggle_button = SKIN_TOGGLE_BUTTON (object);

    switch (prop_id)
    {

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
skin_toggle_button_get_property (GObject      *object,
                            guint         prop_id,
                            GValue       *value,
                            GParamSpec   *pspec)
{
    SkinToggleButton *skin_toggle_button;

    skin_toggle_button = SKIN_TOGGLE_BUTTON (object);

    switch (prop_id)
    {

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}


void skin_toggle_button_show(SkinToggleButton *button)
{
	gnome_canvas_item_show(GNOME_CANVAS_ITEM(button));
}

void skin_toggle_button_hide(SkinToggleButton *button)
{
	gnome_canvas_item_hide(GNOME_CANVAS_ITEM(button));
}
