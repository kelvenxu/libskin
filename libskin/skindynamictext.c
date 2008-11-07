/* vi: set sw=4 ts=4: */
/*
 * skindynamictext.c
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

#include "skindynamictext.h"
#include <glib/gi18n.h>

G_DEFINE_TYPE(SkinDynamicText, skin_dynamic_text, GNOME_TYPE_CANVAS_TEXT);

#define SKIN_DYNAMIC_TEXT_GET_PRIVATE(o)\
	(G_TYPE_INSTANCE_GET_PRIVATE ((o), SKIN_TYPE_DYNAMIC_TEXT, SkinDynamicTextPrivate))

struct _SkinDynamicTextPrivate 
{
	gdouble x1;
	gdouble y1;
	gdouble x2;
	gdouble y2;

	gchar *text[4];
	gchar *title;
	gchar *artist;
	gchar *album;
	gchar *format;
	gchar *color;
	gint align;
	GnomeCanvasGroup *root;
};

enum {
	CLICKED,
    LAST_SIGNAL
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
	PROP_TITLE,
	PROP_ARTIST,
	PROP_ALBUM,
	PROP_FORMAT
};

static int signals[LAST_SIGNAL];

static void skin_dynamic_text_dispose (SkinDynamicText *self);
static void skin_dynamic_text_finalize (SkinDynamicText *self);
static void skin_dynamic_text_init (SkinDynamicText *self);
static void skin_dynamic_text_class_init (SkinDynamicTextClass *self_class);

static void skin_dynamic_text_set_property (GObject            *object,
				  guint               param_id,
				  const GValue       *value,
				  GParamSpec         *pspec);
static void skin_dynamic_text_get_property (GObject            *object,
				  guint               param_id,
				  GValue             *value,
				  GParamSpec         *pspec);

static gint cb_event (GnomeCanvasItem *item, GdkEvent *event, SkinDynamicText* dtext);
static void skin_dynamic_text_update(SkinDynamicText *dtext);
static void skin_dynamic_text_construct(SkinDynamicText *dtext, 
										GnomeCanvasGroup *root, 
										const gchar *first_arg_name, 
										va_list args);

static void
skin_dynamic_text_dispose (SkinDynamicText *self)
{
}

static void
skin_dynamic_text_finalize (SkinDynamicText *self)
{
}

static void
skin_dynamic_text_init (SkinDynamicText *self)
{
	SkinDynamicTextPrivate *priv;

	priv = g_new0(SkinDynamicTextPrivate, 1);
	self->priv = priv;
}

static void
skin_dynamic_text_class_init (SkinDynamicTextClass *self_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (self_class);

	g_type_class_add_private (self_class, sizeof (SkinDynamicTextPrivate));
	object_class->dispose = (void (*) (GObject *object)) skin_dynamic_text_dispose;
	object_class->finalize = (void (*) (GObject *object)) skin_dynamic_text_finalize;
	object_class->set_property = skin_dynamic_text_set_property;
	object_class->get_property = skin_dynamic_text_get_property;

	signals[CLICKED] = g_signal_new("clicked", 
			G_TYPE_FROM_CLASS(object_class),
			G_SIGNAL_RUN_LAST,
			G_STRUCT_OFFSET(SkinDynamicTextClass, clicked),
			NULL, 
			NULL,
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE, 
			0, 
			NULL);

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
	g_object_class_install_property(object_class, PROP_TITLE, 
			g_param_spec_string("title", _("Title"), _("Title, as string"), NULL, 
				(G_PARAM_READABLE | G_PARAM_WRITABLE)));
	g_object_class_install_property(object_class, PROP_ARTIST, 
			g_param_spec_string("artist", _("Artist"), _("Artist, as string"), NULL, 
				(G_PARAM_READABLE | G_PARAM_WRITABLE)));
	g_object_class_install_property(object_class, PROP_ALBUM, 
			g_param_spec_string("album", _("Album"), _("Album, as string"), NULL, 
				(G_PARAM_READABLE | G_PARAM_WRITABLE)));
	g_object_class_install_property(object_class, PROP_FORMAT, 
			g_param_spec_string("format", _("Format"), _("Format, as string"), NULL, 
				(G_PARAM_READABLE | G_PARAM_WRITABLE)));
}

static void
skin_dynamic_text_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
	SkinDynamicText *dtext;
	SkinDynamicTextPrivate *priv;

	g_return_if_fail(object != NULL);
	g_return_if_fail(SKIN_IS_DYNAMIC_TEXT(object));

    dtext = SKIN_DYNAMIC_TEXT(object);
	priv = dtext->priv;

    switch (prop_id)
    {
		case PROP_X1:
			priv->x1 = g_value_get_double(value);
			skin_dynamic_text_update(dtext);
			break;
		case PROP_Y1:
			priv->y1 = g_value_get_double(value);
			skin_dynamic_text_update(dtext);
			break;
		case PROP_X2:
			priv->x2 = g_value_get_double(value);
			skin_dynamic_text_update(dtext);
			break;
		case PROP_Y2:
			priv->y2 = g_value_get_double(value);
			skin_dynamic_text_update(dtext);
			break;
		case PROP_ALIGN:
			priv->align = g_value_get_int(value);
			skin_dynamic_text_update(dtext);
			break;
		case PROP_COLOR:
			priv->color = g_value_dup_string(value);
			skin_dynamic_text_update(dtext);
			break;
		case PROP_TITLE:
			priv->title = g_value_dup_string(value);
			skin_dynamic_text_update(dtext);
			break;
		case PROP_ARTIST:
			priv->artist = g_value_dup_string(value);
			skin_dynamic_text_update(dtext);
			break;
		case PROP_ALBUM:
			priv->album = g_value_dup_string(value);
			skin_dynamic_text_update(dtext);
			break;
		case PROP_FORMAT:
			priv->format = g_value_dup_string(value);
			skin_dynamic_text_update(dtext);
			break;
		default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

static void
skin_dynamic_text_get_property (GObject      *object,
                            guint         prop_id,
                            GValue       *value,
                            GParamSpec   *pspec)
{
	SkinDynamicText *dtext;
	SkinDynamicTextPrivate *priv;

	g_return_if_fail(object != NULL);
	g_return_if_fail(SKIN_IS_DYNAMIC_TEXT(object));

    dtext = SKIN_DYNAMIC_TEXT(object);
	priv = dtext->priv;

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
		case PROP_TITLE:
			g_value_set_string(value, priv->title);
			break;
		case PROP_ARTIST:
			g_value_set_string(value, priv->artist);
			break;
		case PROP_ALBUM:
			g_value_set_string(value, priv->album);
			break;
		case PROP_FORMAT:
			g_value_set_string(value, priv->format);
			break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static gint 
cb_event(GnomeCanvasItem *item, GdkEvent *event, SkinDynamicText* dtext)
{
	static gboolean is_pressing = FALSE;

	switch (event->type) 
	{
	case GDK_BUTTON_PRESS:
		if(event->button.button == 1) 
			is_pressing = TRUE;
		break;
	case GDK_MOTION_NOTIFY:
		break;
	case GDK_BUTTON_RELEASE:
		g_signal_emit(dtext, signals[CLICKED], 0, NULL);
		is_pressing = FALSE;
		break;
	default:
		break;
	}

	return FALSE;
}

static void
skin_dynamic_text_update(SkinDynamicText *dtext)
{
	GnomeCanvasItem *item;
	SkinDynamicTextPrivate *priv;
	gdouble width;
	gdouble height;

	g_return_if_fail(SKIN_IS_DYNAMIC_TEXT(dtext));

	item = GNOME_CANVAS_ITEM(dtext);
	priv = dtext->priv;

	width = priv->x2 - priv->x1;
	height = priv->y2 - priv->y1;

	priv->text[0] = priv->title;
	priv->text[1] = priv->artist;
	priv->text[2] = priv->album;
	priv->text[3] = priv->format;

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

	if(priv->title)
	{
		gnome_canvas_item_set(item, "text", priv->title, NULL);
	}
}

static gboolean update_text(SkinDynamicText *dtext)
{
	static gint i = 1;

	g_return_val_if_fail(SKIN_IS_DYNAMIC_TEXT(dtext), TRUE);

	if(i >= 4) i = 0;
	printf("text: %s\n", dtext->priv->text[i]);
	gnome_canvas_item_set(GNOME_CANVAS_ITEM(dtext),
			"text", dtext->priv->text[i], NULL);

	++i;
	return TRUE;
}

static void
skin_dynamic_text_construct(SkinDynamicText *dtext, 
							GnomeCanvasGroup *root, 
							const gchar *first_arg_name, 
							va_list args)
{
	g_return_if_fail(GNOME_IS_CANVAS_GROUP(root));
	g_return_if_fail(SKIN_IS_DYNAMIC_TEXT(dtext));

	dtext->priv->root = root;
	g_object_set_valist(G_OBJECT(dtext), first_arg_name, args);

	skin_dynamic_text_update(dtext);

	g_timeout_add(2000, (GSourceFunc)update_text, dtext);
}

SkinDynamicText *
skin_dynamic_text_new(GnomeCanvasGroup *root, const gchar *first_arg_name, ...)
{
	SkinDynamicText *dtext;
	SkinDynamicTextPrivate *priv;
	GnomeCanvasItem *item;
	va_list args;

	g_return_val_if_fail(GNOME_IS_CANVAS_GROUP(root), NULL);
	
	item = gnome_canvas_item_new(root,
			skin_dynamic_text_get_type(),
			NULL);

	g_return_val_if_fail(GNOME_IS_CANVAS_ITEM(item), NULL);

	dtext = SKIN_DYNAMIC_TEXT(item);
	priv = dtext->priv;

	g_signal_connect(G_OBJECT(item), "event", G_CALLBACK(cb_event), dtext);

	va_start(args, first_arg_name);
	skin_dynamic_text_construct(dtext, root, first_arg_name, args);
	va_end(args);

	return dtext;
}

void 
skin_dynamic_text_set_text(SkinDynamicText *dtext, const gchar *text)
{
}

