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

#include "skinbutton.h"

#define SUBPIXBUF 4

enum {
	CLICKED,
    LAST_SIGNAL
};

enum {
    PROP_0,
};

static int signals[LAST_SIGNAL];

#define SKIN_BUTTON_GET_PRIVATE(obj)  (G_TYPE_INSTANCE_GET_PRIVATE((obj), SKIN_TYPE_BUTTON, SkinButtonPrivate))

struct _SkinButtonPrivate
{
	GdkPixbuf *subpixbuf[SUBPIXBUF];
	gboolean sensitive;
};

static void skin_button_set_property  (GObject          *object,
                                         guint             prop_id,
                                         const GValue     *value,
                                         GParamSpec       *pspec);
static void skin_button_get_property  (GObject          *object,
                                         guint             prop_id,
                                         GValue           *value,
                                         GParamSpec       *pspec);

static gint cb_event (GnomeCanvasItem *item, 
						GdkEvent *event, 
						SkinButton* skin_button);

//G_DEFINE_TYPE (SkinButton, skin_button, SKIN_TYPE_BUTTON);

static gint 
cb_event (GnomeCanvasItem *item, GdkEvent *event, SkinButton* button)
{
	static gboolean is_pressing = FALSE;

	if(!button->priv->sensitive) return FALSE;

	switch (event->type) 
	{
	case GDK_BUTTON_PRESS:
		if(event->button.button == 1) 
		{
			gnome_canvas_item_set(item, "pixbuf", button->priv->subpixbuf[2], NULL);
			is_pressing = TRUE;
		}
		break;
	case GDK_MOTION_NOTIFY:
		if(!is_pressing)
			gnome_canvas_item_set(item, "pixbuf", button->priv->subpixbuf[1], NULL);
		break;
	case GDK_BUTTON_RELEASE:
		gnome_canvas_item_set(item, "pixbuf", button->priv->subpixbuf[1], NULL);
		g_signal_emit(button, signals[CLICKED], 0, NULL);
		is_pressing = FALSE;
		break;
	default:
		gnome_canvas_item_set(item, "pixbuf", button->priv->subpixbuf[0], NULL);
		break;
	}

	return FALSE;
}

static void
skin_button_class_init (SkinButtonClass *class)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (class);

    gobject_class->set_property = skin_button_set_property;
    gobject_class->get_property = skin_button_get_property;

    g_type_class_add_private (class, sizeof (SkinButtonPrivate));

	signals[CLICKED] = g_signal_new("clicked", 
			G_TYPE_FROM_CLASS(gobject_class),
			G_SIGNAL_RUN_LAST,
			G_STRUCT_OFFSET(SkinButtonClass, clicked),
			NULL, 
			NULL,
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE, 
			0, 
			NULL);

	class->clicked = NULL;
}

static void
skin_button_init (SkinButton *button)
{
    button->priv = SKIN_BUTTON_GET_PRIVATE(button);
	button->priv->sensitive = TRUE;
}

GType
skin_button_get_type(void)
{
	static GType skin_button_type = 0;
	
	if(!skin_button_type)
	{
		static const GTypeInfo skin_button_info =
		{
			sizeof(SkinButtonClass),
			NULL,
			NULL,
			(GClassInitFunc)skin_button_class_init,
			NULL,
			NULL,
			sizeof(SkinButton),
			0,
			(GInstanceInitFunc)skin_button_init
		};

		skin_button_type = 
		g_type_register_static(gnome_canvas_pixbuf_get_type(),
				"SkinButton", &skin_button_info, 0);
	}

	return skin_button_type;
}


static void
skin_button_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    SkinButton *skin_button;

    skin_button = SKIN_BUTTON (object);

    switch (prop_id)
    {

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
skin_button_get_property (GObject      *object,
                            guint         prop_id,
                            GValue       *value,
                            GParamSpec   *pspec)
{
    SkinButton *skin_button;

    skin_button = SKIN_BUTTON (object);

    switch (prop_id)
    {

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

SkinButton*
skin_button_new(GnomeCanvasGroup *root, GdkPixbuf *pixbuf, gdouble x, gdouble y)
{
	SkinButton *button;
	GnomeCanvasItem *item;
	gdouble pw;
	gdouble ph;
	gdouble w;
	gdouble h;
	gint i;
	
	g_return_val_if_fail(GNOME_IS_CANVAS_GROUP(root), NULL);
	g_return_val_if_fail(GDK_IS_PIXBUF(pixbuf), NULL);
	g_return_val_if_fail(x >= 0.0, NULL);
	g_return_val_if_fail(y >= 0.0, NULL);

	item = gnome_canvas_item_new(root,
			skin_button_get_type(),
			NULL);

	button = SKIN_BUTTON(item);

	pw = gdk_pixbuf_get_width(pixbuf);
	ph = gdk_pixbuf_get_height(pixbuf);
	  
	w = pw / SUBPIXBUF;
	h = ph;
	
	for(i = 0; i < SUBPIXBUF; ++i)
	{
		button->priv->subpixbuf[i] = gdk_pixbuf_new_subpixbuf(pixbuf, w * i, 0, w, h);
	}

	gnome_canvas_item_set(item, 
			"pixbuf", button->priv->subpixbuf[0], 
			"x", x, 
			"y", y, 
			NULL);

	g_signal_connect(G_OBJECT(item), "event", G_CALLBACK(cb_event), button);

    return button;
}

void skin_button_show(SkinButton *button)
{
	g_return_if_fail(SKIN_IS_BUTTON(button));
	gnome_canvas_item_show(GNOME_CANVAS_ITEM(button));
}

void skin_button_hide(SkinButton *button)
{
	g_return_if_fail(SKIN_IS_BUTTON(button));
	gnome_canvas_item_hide(GNOME_CANVAS_ITEM(button));
}

void 
skin_button_set_sensitive(SkinButton *button, gboolean sensitive)
{
	g_return_if_fail(SKIN_IS_BUTTON(button));

	button->priv->sensitive = sensitive;
	if(sensitive)
	{
		gnome_canvas_item_set(GNOME_CANVAS_ITEM(button), 
				"pixbuf", button->priv->subpixbuf[0], 
				NULL);
	}
	else
	{
		gnome_canvas_item_set(GNOME_CANVAS_ITEM(button), 
				"pixbuf", button->priv->subpixbuf[3], 
				NULL);
	}

}

gboolean 
skin_button_get_sensitive(SkinButton *button)
{
	g_return_val_if_fail(SKIN_IS_BUTTON(button), FALSE);

	return button->priv->sensitive;
}

