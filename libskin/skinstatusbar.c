/* vi: set sw=4 ts=4: */
/*
 * skinstatusbar.c
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

#include "skinstatusbar.h"
#include <glib/gi18n.h>

G_DEFINE_TYPE(SkinStatusBar, skin_status_bar, GNOME_TYPE_CANVAS_TEXT);

#define SKIN_STATUS_BAR_GET_PRIVATE(o)\
	(G_TYPE_INSTANCE_GET_PRIVATE ((o), SKIN_TYPE_STATUS_BAR, SkinStatusBarPrivate))

struct _SkinStatusBarPrivate 
{
	gdouble x1;
	gdouble y1;
	gdouble x2;
	gdouble y2;

	gchar *text;
	gchar *color;
	gint align;
	GnomeCanvasGroup *root;
};


/* Object argument IDs */
enum {
	PROP_0,
	PROP_X1,
	PROP_Y1,
	PROP_X2,
	PROP_Y2,
	PROP_ALIGN,
	PROP_COLOR,
	PROP_TEXT
};

static void skin_status_bar_dispose (SkinStatusBar *self);
static void skin_status_bar_finalize (SkinStatusBar *self);
static void skin_status_bar_init (SkinStatusBar *self);
static void skin_status_bar_class_init (SkinStatusBarClass *self_class);
static void skin_status_bar_set_property (GObject      *object,
											guint         prop_id,
											const GValue *value,
											GParamSpec   *pspec);
static void skin_status_bar_get_property (GObject      *object,
											guint         prop_id,
											GValue       *value,
											GParamSpec   *pspec);
static void skin_status_bar_update(SkinStatusBar *status_bar);
static void skin_status_bar_construct(SkinStatusBar *status_bar, 
											GnomeCanvasGroup *root, 
											const gchar *first_arg_name, 
											va_list args);

static void
skin_status_bar_dispose (SkinStatusBar *self)
{
}

static void
skin_status_bar_finalize (SkinStatusBar *self)
{
}

static void
skin_status_bar_init (SkinStatusBar *self)
{
	SkinStatusBarPrivate *priv;
	priv = g_new0(SkinStatusBarPrivate, 1);
	self->priv = priv;
}

static void
skin_status_bar_class_init (SkinStatusBarClass *self_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (self_class);

	g_type_class_add_private (self_class, sizeof (SkinStatusBarPrivate));
	object_class->dispose = (void (*) (GObject *object)) skin_status_bar_dispose;
	object_class->finalize = (void (*) (GObject *object)) skin_status_bar_finalize;
	object_class->set_property = skin_status_bar_set_property;
	object_class->get_property = skin_status_bar_get_property;

	g_object_class_install_property(object_class, PROP_X1,
			 g_param_spec_double ("x1", NULL, NULL,
				  -G_MAXDOUBLE, G_MAXDOUBLE, 0,
				  (G_PARAM_READABLE | G_PARAM_WRITABLE)));
	g_object_class_install_property(object_class, PROP_Y1,
			 g_param_spec_double ("y1", NULL, NULL,
				  -G_MAXDOUBLE, G_MAXDOUBLE, 0,
				  (G_PARAM_READABLE | G_PARAM_WRITABLE)));
	g_object_class_install_property(object_class, PROP_X2,
			 g_param_spec_double ("x2", NULL, NULL,
				  -G_MAXDOUBLE, G_MAXDOUBLE, 0,
				  (G_PARAM_READABLE | G_PARAM_WRITABLE)));
	g_object_class_install_property(object_class, PROP_Y2,
			 g_param_spec_double ("y2", NULL, NULL,
				  -G_MAXDOUBLE, G_MAXDOUBLE, 0,
				  (G_PARAM_READABLE | G_PARAM_WRITABLE)));
	g_object_class_install_property(object_class, PROP_ALIGN,
			 g_param_spec_int ("align", _("Text align"), _("Text align"), 0, 
				 G_MAXINT, PANGO_WEIGHT_NORMAL,
				 G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property(object_class, PROP_COLOR, 
			g_param_spec_string("color", _("Color"), _("Text color, as string"), NULL, 
				(G_PARAM_READABLE | G_PARAM_WRITABLE)));
	/*
	g_object_class_install_property(object_class, PROP_TEXT, 
			g_param_spec_string("text", _("Status message"), _("Text, as string"), NULL, 
				(G_PARAM_READABLE | G_PARAM_WRITABLE)));
	*/
}

static void
skin_status_bar_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
	SkinStatusBar *status_bar;
	SkinStatusBarPrivate *priv;

	g_return_if_fail(object != NULL);
	g_return_if_fail(SKIN_IS_STATUS_BAR(object));

    status_bar = SKIN_STATUS_BAR(object);
	priv = status_bar->priv;

    switch (prop_id)
    {
		case PROP_X1:
			priv->x1 = g_value_get_double(value);
			skin_status_bar_update(status_bar);
			break;
		case PROP_Y1:
			priv->y1 = g_value_get_double(value);
			skin_status_bar_update(status_bar);
			break;
		case PROP_X2:
			priv->x2 = g_value_get_double(value);
			skin_status_bar_update(status_bar);
			break;
		case PROP_Y2:
			priv->y2 = g_value_get_double(value);
			skin_status_bar_update(status_bar);
			break;
		case PROP_ALIGN:
			priv->align = g_value_get_int(value);
			skin_status_bar_update(status_bar);
			break;
		case PROP_COLOR:
			priv->color = g_value_dup_string(value);
			skin_status_bar_update(status_bar);
			break;
			/*
		case PROP_TEXT:
			priv->text = g_value_dup_string(value);
			skin_status_bar_update(status_bar);
			break;
			*/
		default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

static void
skin_status_bar_get_property (GObject      *object,
                            guint         prop_id,
                            GValue       *value,
                            GParamSpec   *pspec)
{
	SkinStatusBar *status_bar;
	SkinStatusBarPrivate *priv;

	g_return_if_fail(object != NULL);
	g_return_if_fail(SKIN_IS_STATUS_BAR(object));

    status_bar= SKIN_STATUS_BAR(object);
	priv = status_bar->priv;

    switch (prop_id)
    {
		case PROP_X1:
			g_value_set_double(value, priv->x1);
			break;
		case PROP_Y1:
			g_value_set_double(value, priv->y1);
			break;
		case PROP_X2:
			g_value_set_double(value, priv->x2);
			break;
		case PROP_Y2:
			g_value_set_double(value, priv->y2);
			break;
		case PROP_ALIGN:
			g_value_set_double(value, priv->align);
			break;
		case PROP_COLOR:
			g_value_set_string(value, priv->color);
			break;
			/*
		case PROP_TEXT:
			g_value_set_string(value, priv->text);
			break;
			*/
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
skin_status_bar_update(SkinStatusBar *status_bar)
{
	GnomeCanvasItem *item;
	SkinStatusBarPrivate *priv;
	gdouble width;
	gdouble height;

	g_return_if_fail(SKIN_IS_STATUS_BAR(status_bar));

	item = GNOME_CANVAS_ITEM(status_bar);
	priv = status_bar->priv;

	width = priv->x2 - priv->x1;
	height = priv->y2 - priv->y1;

	gnome_canvas_item_set(item, 
			"x", priv->x1,
			"y", priv->y1,
			"clip", TRUE,
			"clip-width", width,
			"clip-height", height,
			"fill-color", priv->color,
			"anchor", GTK_ANCHOR_NW,
			NULL);
	if(priv->align == 0)
		gnome_canvas_item_set(item,
				"justification", GTK_JUSTIFY_LEFT,
				NULL);
	else if(priv->align == 1)
		gnome_canvas_item_set(item,
				"justification", GTK_JUSTIFY_CENTER,
				NULL);
	else
		gnome_canvas_item_set(item,
				"justification", GTK_JUSTIFY_RIGHT,
				NULL);

	g_object_get(G_OBJECT(item), "text", &priv->text, NULL);
	/*
	if(priv->text)
	{
		gnome_canvas_item_set(item, "text", priv->text, NULL);
	}
	*/
}

static void
skin_status_bar_construct(SkinStatusBar *status_bar, 
							GnomeCanvasGroup *root, 
							const gchar *first_arg_name, 
							va_list args)
{
	g_return_if_fail(GNOME_IS_CANVAS_GROUP(root));
	g_return_if_fail(SKIN_IS_STATUS_BAR(status_bar));

	status_bar->priv->root = root;
	g_object_set_valist(G_OBJECT(status_bar), first_arg_name, args);

	skin_status_bar_update(status_bar);
}

SkinStatusBar *
skin_status_bar_new(GnomeCanvasGroup *root, const gchar *first_arg_name, ...)
{
	SkinStatusBar *status_bar;
	SkinStatusBarPrivate *priv;
	GnomeCanvasItem *item;
	va_list args;

	g_return_val_if_fail(GNOME_IS_CANVAS_GROUP(root), NULL);
	
	item = gnome_canvas_item_new(root,
			skin_status_bar_get_type(),
			NULL);

	g_return_val_if_fail(GNOME_IS_CANVAS_ITEM(item), NULL);

	status_bar = SKIN_STATUS_BAR(item);
	priv = status_bar->priv;

	va_start(args, first_arg_name);
	skin_status_bar_construct(status_bar, root, first_arg_name, args);
	va_end(args);

	return status_bar;
}

void 
skin_status_bar_set_text(SkinStatusBar *status_bar, const gchar *text)
{
	g_return_if_fail(SKIN_IS_STATUS_BAR(status_bar));
	g_return_if_fail(text != NULL);
	gnome_canvas_item_set(GNOME_CANVAS_ITEM(status_bar), 
			"text", text, 
			NULL);
}

