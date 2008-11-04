/* vi: set sw=4 ts=4: */
/*
 * skinvolumebutton.c
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

#include "skinvolumebutton.h"

G_DEFINE_TYPE(SkinVolumeButton, skin_volume_button, GNOME_TYPE_CANVAS_RECT);

#define SKIN_VOLUME_BUTTON_GET_PRIVATE(o)\
	(G_TYPE_INSTANCE_GET_PRIVATE ((o), SKIN_TYPE_VOLUME_BUTTON, SkinVolumeButtonPrivate))

struct _SkinVolumeButtonPrivate 
{
	GnomeCanvasGroup *root;

	GnomeCanvasItem *bar_item;
	GnomeCanvasItem *thumb_item;
	GnomeCanvasItem *fill_item;

	GdkPixbuf *bar_pixbuf;
	GdkPixbuf *thumb_pixbuf;
	GdkPixbuf *fill_pixbuf;

	gdouble x;
	gdouble y;
	gdouble width;
	gdouble height; 

	gdouble value;
	gdouble max;
	gdouble min;

	gboolean vertical;
	guint width_set : 1;
	guint height_set : 1;
};

/* Object argument IDs */
enum {
	PROP_0,
	PROP_BAR_PIXBUF,
	PROP_THUMB_PIXBUF,
	PROP_FILL_PIXBUF,
	PROP_WIDTH,
	PROP_WIDTH_SET,
	PROP_HEIGHT,
	PROP_HEIGHT_SET,
	PROP_X,
	PROP_Y
};

static void skin_volume_button_dispose (SkinVolumeButton *self);
static void skin_volume_button_finalize (SkinVolumeButton *self);

static void skin_volume_button_init (SkinVolumeButton *self);
static void skin_volume_button_class_init (SkinVolumeButtonClass *self_class);
static void skin_volume_button_set_property (GObject            *object,
				  guint               param_id,
				  const GValue       *value,
				  GParamSpec         *pspec);
static void skin_volume_button_get_property (GObject            *object,
				  guint               param_id,
				  GValue             *value,
				  GParamSpec         *pspec);

static void skin_volume_button_update(SkinVolumeButton *button);
static void update_position(SkinVolumeButton *button);

static void
skin_volume_button_dispose (SkinVolumeButton *self)
{
}

static void
skin_volume_button_finalize (SkinVolumeButton *self)
{
}

static void
skin_volume_button_init (SkinVolumeButton *self)
{
	SkinVolumeButtonPrivate *priv;
	//priv = SKIN_VOLUME_BUTTON_GET_PRIVATE (self);
	priv = g_new0(SkinVolumeButtonPrivate, 1);

	self->priv = priv;

	priv->root = NULL;
	priv->bar_item = NULL;
	priv->thumb_item = NULL;
	priv->fill_item = NULL;
	priv->bar_pixbuf = NULL;
	priv->thumb_pixbuf = NULL;
	priv->fill_pixbuf = NULL;
	priv->width = 10.0;
	priv->height = 10.0;
	priv->x = 100.0;
	priv->y = 100.0;

	priv->value = 0.0;
	priv->max = 100.0;
	priv->min = 0.0;

	priv->vertical = FALSE;
}

static void
skin_volume_button_class_init (SkinVolumeButtonClass *self_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (self_class);

	g_type_class_add_private (self_class, sizeof (SkinVolumeButtonPrivate));
	object_class->dispose = (void (*) (GObject *object)) skin_volume_button_dispose;
	object_class->finalize = (void (*) (GObject *object)) skin_volume_button_finalize;
	object_class->set_property = skin_volume_button_set_property;
	object_class->get_property = skin_volume_button_get_property;

	//object_class->destroy = gnome_canvas_pixbuf_destroy;

	g_object_class_install_property(object_class, PROP_BAR_PIXBUF,
			 g_param_spec_object("bar_pixbuf", NULL, NULL, 
				 GDK_TYPE_PIXBUF, (G_PARAM_READABLE | G_PARAM_WRITABLE)));

	g_object_class_install_property(object_class, PROP_THUMB_PIXBUF,
			 g_param_spec_object("thumb_pixbuf", NULL, NULL, 
				 GDK_TYPE_PIXBUF, (G_PARAM_READABLE | G_PARAM_WRITABLE)));

	g_object_class_install_property(object_class, PROP_FILL_PIXBUF,
			 g_param_spec_object("fill_pixbuf", NULL, NULL, 
				 GDK_TYPE_PIXBUF, (G_PARAM_READABLE | G_PARAM_WRITABLE)));

	g_object_class_install_property(object_class, PROP_WIDTH,
			 g_param_spec_double("width", NULL, NULL, 
				 -G_MAXDOUBLE, G_MAXDOUBLE, 0, 
				 (G_PARAM_READABLE | G_PARAM_WRITABLE)));

	g_object_class_install_property(object_class, PROP_WIDTH_SET,
			 g_param_spec_boolean ("width_set", NULL, NULL,
				   FALSE, (G_PARAM_READABLE | G_PARAM_WRITABLE)));

	g_object_class_install_property(object_class, PROP_HEIGHT,
			 g_param_spec_double ("height", NULL, NULL,
				  -G_MAXDOUBLE, G_MAXDOUBLE, 0, 
				  (G_PARAM_READABLE | G_PARAM_WRITABLE)));

	g_object_class_install_property(object_class, PROP_HEIGHT_SET,
			 g_param_spec_boolean ("height_set", NULL, NULL, 
				 FALSE, (G_PARAM_READABLE | G_PARAM_WRITABLE)));

	g_object_class_install_property(object_class, PROP_X,
			 g_param_spec_double ("x", NULL, NULL, 
				 -G_MAXDOUBLE, G_MAXDOUBLE, 0, 
				 (G_PARAM_READABLE | G_PARAM_WRITABLE)));

	g_object_class_install_property(object_class, PROP_Y,
			 g_param_spec_double ("y", NULL, NULL, 
				 -G_MAXDOUBLE, G_MAXDOUBLE, 0, 
				 (G_PARAM_READABLE | G_PARAM_WRITABLE)));
}

static void
skin_volume_button_set_property (GObject            *object,
				  guint               param_id,
				  const GValue       *value,
				  GParamSpec         *pspec)
{
	GdkPixbuf *pixbuf;
	double val;

	SkinVolumeButton *button;
	SkinVolumeButtonPrivate *priv;

	g_return_if_fail(object != NULL);
	g_return_if_fail(SKIN_IS_VOLUME_BUTTON(object));

	button = SKIN_VOLUME_BUTTON(object);
	priv = button->priv;

	switch(param_id) {
	case PROP_BAR_PIXBUF:
		if(g_value_get_object(value))
			pixbuf = GDK_PIXBUF(g_value_get_object(value));
		else
			pixbuf = NULL;
		priv->bar_pixbuf = pixbuf;
		skin_volume_button_update(button);
		break;

	case PROP_THUMB_PIXBUF:
		if(g_value_get_object(value))
			pixbuf = GDK_PIXBUF(g_value_get_object(value));
		else
			pixbuf = NULL;
		priv->thumb_pixbuf = pixbuf;
		skin_volume_button_update(button);
		break;

	case PROP_FILL_PIXBUF:
		if(g_value_get_object(value))
			pixbuf = GDK_PIXBUF(g_value_get_object(value));
		else
			pixbuf = NULL;
		priv->fill_pixbuf = pixbuf;
		skin_volume_button_update(button);
		break;

	case PROP_WIDTH:
		val = g_value_get_double (value);
		g_return_if_fail (val >= 0.0);
		priv->width = val;
		skin_volume_button_update(button);
		break;

	case PROP_WIDTH_SET:
		priv->width_set = g_value_get_boolean (value);
		skin_volume_button_update(button);
		break;

	case PROP_HEIGHT:
		val = g_value_get_double (value);
		g_return_if_fail (val >= 0.0);
		priv->height = val;
		skin_volume_button_update(button);
		break;

	case PROP_HEIGHT_SET:
		priv->height_set = g_value_get_boolean (value);
		skin_volume_button_update(button);
		break;

	case PROP_X:
		priv->x = g_value_get_double (value);
		skin_volume_button_update(button);
		break;

	case PROP_Y:
		priv->y = g_value_get_double (value);
		skin_volume_button_update(button);
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
		break;
	}
}

/* Get_property handler for the pixbuf canvasi item */
static void
skin_volume_button_get_property (GObject            *object,
				  guint               param_id,
				  GValue             *value,
				  GParamSpec         *pspec)
{
	SkinVolumeButton *button;
	SkinVolumeButtonPrivate *priv;

	g_return_if_fail(object != NULL);
	g_return_if_fail(SKIN_IS_VOLUME_BUTTON(object));

	button = SKIN_VOLUME_BUTTON(object);
	priv = button->priv;

	switch(param_id) {
	case PROP_BAR_PIXBUF:
		g_value_set_object(value, G_OBJECT(priv->bar_pixbuf));
		break;

	case PROP_THUMB_PIXBUF:
		g_value_set_object(value, G_OBJECT(priv->thumb_pixbuf));
		break;

	case PROP_FILL_PIXBUF:
		g_value_set_object(value, G_OBJECT(priv->fill_pixbuf));
		break;

	case PROP_WIDTH:
		g_value_set_double(value, priv->width);
		break;

	case PROP_WIDTH_SET:
		g_value_set_boolean(value, priv->width_set);
		break;

	case PROP_HEIGHT:
		g_value_set_double(value, priv->height);
		break;

	case PROP_HEIGHT_SET:
		g_value_set_boolean(value, priv->height_set);
		break;

	case PROP_X:
		g_value_set_double(value, priv->x);
		break;

	case PROP_Y:
		g_value_set_double(value, priv->y);
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
		break;
	}
}

static void
skin_volume_button_update(SkinVolumeButton *button)
{
	GnomeCanvasItem *item;
	SkinVolumeButtonPrivate *priv;

	g_return_if_fail(SKIN_IS_VOLUME_BUTTON(button));

	g_print("update\n");
	item = GNOME_CANVAS_ITEM(button);
	priv = button->priv;

	g_return_if_fail(GNOME_IS_CANVAS_ITEM(item));

	printf("x: %f, y: %f, width: %f, height: %f\n", 
			priv->x, priv->y, priv->width, priv->height);
	gnome_canvas_item_set(item, 
			"x1", priv->x,
			"y1", priv->y,
			"x2", priv->width + priv->x,
			"y2", priv->height + priv->y,
			"fill-color-rgba", 0x00000000,
			"outline-color-rgba", 0x00000000,
			NULL);

	g_print("bar\n");
	if(GDK_IS_PIXBUF(priv->bar_pixbuf))
	{
		if(!GNOME_IS_CANVAS_PIXBUF(priv->bar_item))
		{
			priv->bar_item = gnome_canvas_item_new(priv->root, 
					gnome_canvas_pixbuf_get_type(),
					"x", priv->x,
					"y", priv->y,
					"pixbuf", priv->bar_pixbuf, 
					NULL);
		}
		else
		{
			gnome_canvas_item_set(priv->bar_item, 
					"x", priv->x,
					"y", priv->y,
					"pixbuf", priv->bar_pixbuf,
					NULL);
			gnome_canvas_item_show(priv->bar_item);
		}
	}
	else
	{
		if(GNOME_IS_CANVAS_PIXBUF(priv->bar_item))
		{
			gnome_canvas_item_hide(priv->bar_item);
		}
	}

	printf("thumb\n");
	if(GDK_IS_PIXBUF(priv->thumb_pixbuf))
	{
		if(!GNOME_IS_CANVAS_PIXBUF(priv->thumb_item))
		{
			priv->bar_item = gnome_canvas_item_new(priv->root, 
					gnome_canvas_pixbuf_get_type(),
					"x", priv->x,
					"y", priv->y,
					"pixbuf", priv->thumb_pixbuf, 
					NULL);
		}
		else
		{
			gnome_canvas_item_set(priv->thumb_item, 
					"x", priv->x,
					"y", priv->y,
					"pixbuf", priv->thumb_pixbuf,
					NULL);
			gnome_canvas_item_show(priv->thumb_item);
		}
	}
	else
	{
		if(GNOME_IS_CANVAS_PIXBUF(priv->thumb_item))
		{
			gnome_canvas_item_hide(priv->thumb_item);
		}
	}

	printf("fill\n");
	if(GDK_IS_PIXBUF(priv->fill_pixbuf))
	{
		if(!GNOME_IS_CANVAS_PIXBUF(priv->fill_item))
		{
			priv->bar_item = gnome_canvas_item_new(priv->root, 
					gnome_canvas_pixbuf_get_type(),
					"x", priv->x,
					"y", priv->y,
					"pixbuf", priv->fill_pixbuf, 
					NULL);
		}
		else
		{
			gnome_canvas_item_set(priv->fill_item, 
					"x", priv->x,
					"y", priv->y,
					"pixbuf", priv->fill_pixbuf,
					NULL);
			gnome_canvas_item_show(priv->fill_item);
		}
	}
	else
	{
		if(GNOME_IS_CANVAS_PIXBUF(priv->fill_item))
		{
			gnome_canvas_item_hide(priv->fill_item);
		}
	}
}

static void
update_position(SkinVolumeButton *button)
{
	SkinVolumeButtonPrivate *priv;
	GdkPixbuf *p = NULL;
	gdouble factor;

	priv = button->priv;
	factor = priv->width / priv->max;
	gdouble y_new = priv->value * factor;

	if(priv->vertical)
	{
	}
	else
	{
		if(GNOME_IS_CANVAS_ITEM(priv->bar_pixbuf))
		{
			//TODO:
		}
		if(GNOME_IS_CANVAS_ITEM(priv->thumb_pixbuf))
		{
			gnome_canvas_item_set(priv->thumb_item,
					"x", priv->x + y_new,
					NULL);
		}
		if(GNOME_IS_CANVAS_ITEM(priv->fill_pixbuf))
		{
			p = gdk_pixbuf_new_subpixbuf(priv->fill_pixbuf,
					0, 0,
					priv->width, y_new);
			gnome_canvas_item_set(priv->fill_item,
					"pixbuf", p,
					"width", y_new,
					NULL);
		}
	}
}

static void
skin_volume_button_construct(SkinVolumeButton *button, 
							GnomeCanvasGroup *root, 
							const gchar *first_arg_name, 
							va_list args)
{
	g_return_if_fail(GNOME_IS_CANVAS_GROUP(root));
	g_return_if_fail(SKIN_IS_VOLUME_BUTTON(button));

	button->priv->root = root;
	g_object_set_valist(G_OBJECT(button), first_arg_name, args);

	skin_volume_button_update(button);
}

SkinVolumeButton *
skin_volume_button_new(GnomeCanvasGroup *root, const gchar *first_arg_name, ...)
{
	SkinVolumeButton *button;
	GnomeCanvasItem *item;
	va_list args;

	g_return_val_if_fail(GNOME_IS_CANVAS_GROUP(root), NULL);

	item = gnome_canvas_item_new(root,
			skin_volume_button_get_type(),
			NULL);
	
	button = SKIN_VOLUME_BUTTON(item);

	va_start(args, first_arg_name);
	skin_volume_button_construct(button, root, first_arg_name, args);
	va_end(args);

	return button;
}

void skin_volume_button_set_range(SkinVolumeButton *button, gdouble max, gdouble min)
{
	g_return_if_fail(SKIN_IS_VOLUME_BUTTON(button));
	
	button->priv->max = max;
	button->priv->min = min;
}


void skin_volume_button_set_value(SkinVolumeButton *button, gdouble value)
{
	g_return_if_fail(SKIN_IS_VOLUME_BUTTON(button));
	
	if(button->priv->value != value)
	{
		button->priv->value = value;

		//Signal: value-changed
		//TODO: update ui
		update_position(button);
	}
}
