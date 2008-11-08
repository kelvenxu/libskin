/* vi: set sw=4 ts=4: */
/*
 * skincheckbutton.c
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

#include "skincheckbutton.h"

#define SUBPIXBUF 4

enum {
	CLICKED,
	ACTIVATED,
    LAST_SIGNAL
};

enum {
    PROP_0,
};

static int signals[LAST_SIGNAL];

#define SKIN_CHECK_BUTTON_GET_PRIVATE(obj)  (G_TYPE_INSTANCE_GET_PRIVATE((obj), SKIN_TYPE_CHECK_BUTTON, SkinCheckButtonPrivate))

struct _SkinCheckButtonPrivate
{
	GdkPixbuf *subpixbuf[SUBPIXBUF];
	gdouble x0;
	gdouble y0;
	gboolean checked;
};

static void skin_check_button_set_property  (GObject          *object,
                                         guint             prop_id,
                                         const GValue     *value,
                                         GParamSpec       *pspec);
static void skin_check_button_get_property  (GObject          *object,
                                         guint             prop_id,
                                         GValue           *value,
                                         GParamSpec       *pspec);

static gint cb_event (GnomeCanvasItem *item, 
						GdkEvent *event, 
						SkinCheckButton* skin_check_button);
static gboolean cb_event_after(SkinCheckButton* button);

//G_DEFINE_TYPE (SkinCheckButton, skin_check_button, SKIN_TYPE_CHECK_BUTTON);

static gint 
cb_event(GnomeCanvasItem *item, GdkEvent *event, SkinCheckButton* button)
{
	static gboolean is_pressing = FALSE;
	SkinCheckButtonPrivate *priv = button->priv;

	switch (event->type) 
	{
	case GDK_BUTTON_PRESS:
		if(event->button.button == 1) 
		{
			gnome_canvas_item_set(item, "pixbuf", priv->subpixbuf[2], NULL);
			is_pressing = TRUE;
		}
		break;
	case GDK_MOTION_NOTIFY:
		if(!is_pressing)
			gnome_canvas_item_set(item, "pixbuf", priv->subpixbuf[1], NULL);
		break;
	case GDK_BUTTON_RELEASE:
		gnome_canvas_item_set(item, "pixbuf", priv->subpixbuf[1], NULL);
		priv->checked = !priv->checked;
		g_signal_emit(button, signals[CLICKED], 0, NULL);
		is_pressing = FALSE;
		break;
	default:
		gnome_canvas_item_set(item, "pixbuf", priv->subpixbuf[0], NULL);
		break;
	}

	//FIXME: how to make things better?
	g_timeout_add(500, (GSourceFunc)cb_event_after, button);
	return FALSE;
}

static gboolean
cb_event_after(SkinCheckButton* button)
{
	skin_check_button_set_active(button, button->priv->checked);
	return FALSE;
}
static void
skin_check_button_class_init (SkinCheckButtonClass *class)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (class);

    gobject_class->set_property = skin_check_button_set_property;
    gobject_class->get_property = skin_check_button_get_property;

    g_type_class_add_private (class, sizeof (SkinCheckButtonPrivate));

	signals[CLICKED] = g_signal_new("clicked", 
			G_TYPE_FROM_CLASS(gobject_class),
			G_SIGNAL_RUN_LAST,
			G_STRUCT_OFFSET(SkinCheckButtonClass, clicked),
			NULL, 
			NULL,
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE, 
			0, 
			NULL);

	signals[ACTIVATED] = g_signal_new("activated", 
			G_TYPE_FROM_CLASS(gobject_class),
			G_SIGNAL_RUN_LAST,
			G_STRUCT_OFFSET(SkinCheckButtonClass, activated),
			NULL, 
			NULL,
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE, 
			0, 
			NULL);

	class->clicked = NULL;
	class->activated = NULL;
}

static void
skin_check_button_init (SkinCheckButton *skin_check_button)
{
    skin_check_button->priv = SKIN_CHECK_BUTTON_GET_PRIVATE (skin_check_button);
}

GType
skin_check_button_get_type(void)
{
	static GType skin_check_button_type = 0;
	
	if(!skin_check_button_type)
	{
		static const GTypeInfo skin_check_button_info =
		{
			sizeof(SkinCheckButtonClass),
			NULL,
			NULL,
			(GClassInitFunc)skin_check_button_class_init,
			NULL,
			NULL,
			sizeof(SkinCheckButton),
			0,
			(GInstanceInitFunc)skin_check_button_init
		};

		skin_check_button_type = 
		g_type_register_static(gnome_canvas_pixbuf_get_type(),
				"SkinCheckButton", &skin_check_button_info, 0);
	}

	return skin_check_button_type;
}

static void
skin_check_button_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    SkinCheckButton *skin_check_button;

    skin_check_button = SKIN_CHECK_BUTTON (object);

    switch (prop_id)
    {

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
skin_check_button_get_property (GObject      *object,
                            guint         prop_id,
                            GValue       *value,
                            GParamSpec   *pspec)
{
    SkinCheckButton *skin_check_button;

    skin_check_button = SKIN_CHECK_BUTTON (object);

    switch (prop_id)
    {

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

SkinCheckButton*
skin_check_button_new(GnomeCanvasGroup *root, GdkPixbuf *pixbuf, gdouble x, gdouble y)
{
	SkinCheckButton *button;
	GnomeCanvasItem *item;
	gdouble pw;
	gdouble ph;
	gdouble w;
	gdouble h;
	gint i;
	
	item = gnome_canvas_item_new(root,
			skin_check_button_get_type(),
			NULL);

	button = SKIN_CHECK_BUTTON(item);

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

	button->priv->x0 = x;
	button->priv->y0 = y;

	g_signal_connect(G_OBJECT(item), "event", G_CALLBACK(cb_event), button);

    return button;
}

void
skin_check_button_set_active(SkinCheckButton *button, gboolean is_active)
{
	SkinCheckButtonPrivate *priv = button->priv;
	priv->checked = is_active;
	if(priv->checked)
	{
		gnome_canvas_item_set(GNOME_CANVAS_ITEM(button), 
				"pixbuf", priv->subpixbuf[1], 
				NULL);
		g_signal_emit(button, signals[ACTIVATED], 0, NULL);
	}
	else
	{
		gnome_canvas_item_set(GNOME_CANVAS_ITEM(button), 
				"pixbuf", priv->subpixbuf[0], 
				NULL);
	}
}

gboolean
skin_check_button_get_active(SkinCheckButton *button)
{
	return button->priv->checked;
}

void skin_check_button_show(SkinCheckButton *button)
{
	gnome_canvas_item_show(GNOME_CANVAS_ITEM(button));
}

void skin_check_button_hide(SkinCheckButton *button)
{
	gnome_canvas_item_hide(GNOME_CANVAS_ITEM(button));
}
