/* vi: set sw=4 ts=4: */
/*
 * skinhscale.c
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

#include "skinhscale.h"

#define SUBPIXBUF 4

G_DEFINE_TYPE(SkinHScale, skin_hscale, GNOME_TYPE_CANVAS_RECT);

#define SKIN_HSCALE_GET_PRIVATE(o)\
	(G_TYPE_INSTANCE_GET_PRIVATE ((o), SKIN_TYPE_HSCALE, SkinHScalePrivate))

struct _SkinHScalePrivate 
{
	GnomeCanvasGroup *root;
	gdouble x1;
	gdouble y1;
	gdouble x2;
	gdouble y2;
	GdkPixbuf *fill_pixbuf;
	GdkPixbuf *thumb_pixbuf;
	GdkPixbuf *thumb_subpixbuf[SUBPIXBUF];
	GnomeCanvasItem *fill_item;
	GnomeCanvasItem *thumb_item;
	
	gboolean has_fill;
	gboolean has_thumb;
	gboolean has_fill_item;
	gboolean has_thumb_item;
	gboolean need_pixbuf_update;
	gboolean need_value_update;

	// range 
	gdouble min;
	gdouble max;
	// current value
	gdouble value;
};

enum 
{
	VALUE_CHANGED,
    LAST_SIGNAL
};

/* Object argument IDs */
enum 
{
	PROP_0,
	PROP_FILL_PIXBUF,
	PROP_THUMB_PIXBUF,
	PROP_MIN,
	PROP_MAX,
	PROP_VALUE
};

static int signals[LAST_SIGNAL];

static void skin_hscale_dispose (SkinHScale *self);
static void skin_hscale_finalize (SkinHScale *self);
static void skin_hscale_init (SkinHScale *self);
static void skin_hscale_class_init (SkinHScaleClass *self_class);
static void skin_hscale_set_property (GObject      *object,
									guint         prop_id,
									const GValue *value,
									GParamSpec   *pspec);
static void skin_hscale_get_property (GObject      *object,
									guint         prop_id,
									GValue       *value,
									GParamSpec   *pspec);
static void skin_hscale_pixbuf_update(SkinHScale *hscale);
static void skin_hscale_value_update(SkinHScale *hscale);
static void skin_hscale_update(SkinHScale *hscale);

static void
skin_hscale_dispose (SkinHScale *self)
{
}

static void
skin_hscale_finalize (SkinHScale *self)
{
}

static void
skin_hscale_init (SkinHScale *self)
{
	SkinHScalePrivate *priv;
	priv = g_new0(SkinHScalePrivate, 1);
	self->priv = priv;

	priv->root = NULL;
	priv->fill_pixbuf = NULL;
	priv->thumb_pixbuf = NULL;
	priv->fill_item = NULL;
	priv->thumb_item = NULL;
	priv->has_fill = FALSE;
	priv->has_thumb = FALSE;
	priv->has_fill_item = FALSE;
	priv->has_thumb_item = FALSE;

	priv->x1 = 0.0;
	priv->y1 = 0.0;
	priv->x2 = 0.0;
	priv->y2 = 0.0;
	priv->min = 0.0;
	priv->max = 0.0;
	priv->value = 0.0;
}

static void
skin_hscale_class_init (SkinHScaleClass *self_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (self_class);

	g_type_class_add_private (self_class, sizeof (SkinHScalePrivate));
	object_class->dispose = (void (*) (GObject *object)) skin_hscale_dispose;
	object_class->finalize = (void (*) (GObject *object)) skin_hscale_finalize;
	object_class->set_property = skin_hscale_set_property;
	object_class->get_property = skin_hscale_get_property;

	signals[VALUE_CHANGED] = g_signal_new("value_changed", 
			G_TYPE_FROM_CLASS(object_class),
			G_SIGNAL_RUN_LAST,
			G_STRUCT_OFFSET(SkinHScaleClass, value_changed),
			NULL, 
			NULL,
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE, 
			0, 
			NULL);

	g_object_class_install_property(object_class, PROP_FILL_PIXBUF,
			 g_param_spec_object ("fill_pixbuf", NULL, NULL, 
				 GDK_TYPE_PIXBUF, 
				 (G_PARAM_READABLE | G_PARAM_WRITABLE)));
	g_object_class_install_property(object_class, PROP_THUMB_PIXBUF,
			 g_param_spec_object ("thumb_pixbuf", NULL, NULL, 
				 GDK_TYPE_PIXBUF, 
				 (G_PARAM_READABLE | G_PARAM_WRITABLE)));
	g_object_class_install_property(object_class, PROP_MIN,
			 g_param_spec_double ("min", NULL, NULL,
				 -G_MAXDOUBLE, G_MAXDOUBLE, 0,
				 (G_PARAM_READABLE | G_PARAM_WRITABLE)));
	g_object_class_install_property(object_class, PROP_MAX,
			 g_param_spec_double ("max", NULL, NULL,
				 -G_MAXDOUBLE, G_MAXDOUBLE, 0,
				 (G_PARAM_READABLE | G_PARAM_WRITABLE)));
	g_object_class_install_property(object_class, PROP_VALUE,
			 g_param_spec_double ("value", NULL, NULL,
				 -G_MAXDOUBLE, G_MAXDOUBLE, 0,
				 (G_PARAM_READABLE | G_PARAM_WRITABLE)));

	self_class->value_changed = NULL;
}

static void
skin_hscale_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
	SkinHScale *hscale;
	SkinHScalePrivate *priv;
	GdkPixbuf *pixbuf;
	gdouble val;

	g_return_if_fail(object != NULL);
	g_return_if_fail(SKIN_IS_HSCALE(object));

    hscale = SKIN_HSCALE(object);
	priv = hscale->priv;

    switch (prop_id)
    {
		case PROP_FILL_PIXBUF:
			if(g_value_get_object(value))
			{
				pixbuf = GDK_PIXBUF(g_value_get_object(value));
				priv->has_fill = TRUE;
			}
			else
			{
				pixbuf = NULL;
				priv->has_fill = FALSE;
			}

			if(pixbuf != priv->fill_pixbuf)
			{
				if(pixbuf)
				{
					gdk_pixbuf_ref(pixbuf);
				}
				if(priv->fill_pixbuf)
					gdk_pixbuf_unref(priv->fill_pixbuf);

				priv->fill_pixbuf = pixbuf;
			}
			priv->need_pixbuf_update = TRUE;
			skin_hscale_update(hscale);
			break;
		case PROP_THUMB_PIXBUF:
			if(g_value_get_object(value))
			{
				pixbuf = GDK_PIXBUF(g_value_get_object(value));
				priv->has_thumb = TRUE;
			}
			else
			{
				pixbuf = NULL;
				priv->has_thumb = FALSE;
			}

			if(pixbuf != priv->thumb_pixbuf)
			{
				if(pixbuf)
				{
					gdk_pixbuf_ref(pixbuf);
				}
				if(priv->thumb_pixbuf)
					gdk_pixbuf_unref(priv->thumb_pixbuf);

				priv->thumb_pixbuf = pixbuf;
			}
			priv->need_pixbuf_update = TRUE;
			skin_hscale_update(hscale);
			break;
		case PROP_MIN:
			priv->min = g_value_get_double(value);
			priv->need_value_update = TRUE;
			skin_hscale_update(hscale);
			break;
		case PROP_MAX:
			priv->max = g_value_get_double(value);
			priv->need_value_update = TRUE;
			skin_hscale_update(hscale);
			break;
		case PROP_VALUE:
			val = g_value_get_double(value);
			if(val != priv->value)
			{
				priv->value = val;
				priv->need_value_update = TRUE;
			}
			skin_hscale_update(hscale);
			break;
		default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

static void
skin_hscale_get_property (GObject      *object,
                            guint         prop_id,
                            GValue       *value,
                            GParamSpec   *pspec)
{
	SkinHScale *hscale;
	SkinHScalePrivate *priv;

	g_return_if_fail(object != NULL);
	g_return_if_fail(SKIN_IS_HSCALE(object));

    hscale = SKIN_HSCALE(object);
	priv = hscale->priv;

    switch (prop_id)
    {
		case PROP_FILL_PIXBUF:
			g_value_set_object(value, G_OBJECT(priv->fill_pixbuf));
			break;
		case PROP_THUMB_PIXBUF:
			g_value_set_object(value, G_OBJECT(priv->thumb_pixbuf));
			break;
		/*
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
		*/
		case PROP_MIN:
			g_value_set_double(value, priv->min);
			break;
		case PROP_MAX:
			g_value_set_double(value, priv->max);
			break;
		case PROP_VALUE:
			g_value_set_double(value, priv->value);
			break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static gint 
cb_rect_event(GnomeCanvasItem *item, GdkEvent *event, SkinHScale* hscale)
{
	static gboolean is_pressing = FALSE;
	SkinHScalePrivate *priv;
	
	priv = hscale->priv;

	switch (event->type) 
	{
	case GDK_BUTTON_PRESS:
		if(event->button.button == 1) 
			is_pressing = TRUE;
		break;
	case GDK_MOTION_NOTIFY:
		break;
	case GDK_BUTTON_RELEASE:
		if(is_pressing)
		{
			priv->value = (event->button.x - priv->x1) / (priv->x2 - priv->x1) * (priv->max - priv->min);
			priv->need_value_update = TRUE;
			skin_hscale_value_update(hscale);
			g_signal_emit(hscale, signals[VALUE_CHANGED], 0, NULL);
			is_pressing = FALSE;
		}
		break;
	default:
		break;
	}
	return FALSE;
}

static gint 
cb_fill_event(GnomeCanvasItem *item, GdkEvent *event, SkinHScale* hscale)
{
	static gboolean is_pressing = FALSE;
	SkinHScalePrivate *priv;

	priv = hscale->priv;

	switch (event->type) 
	{
	case GDK_BUTTON_PRESS:
		if(event->button.button == 1) 
		{
			is_pressing = TRUE;
		}
			
		break;
	case GDK_MOTION_NOTIFY:
		break;
	case GDK_BUTTON_RELEASE:
		if(is_pressing)
		{
			priv->value = (event->button.x - priv->x1) / (priv->x2 - priv->x1) * (priv->max - priv->min);
			priv->need_value_update = TRUE;
			skin_hscale_value_update(hscale);
			g_signal_emit(hscale, signals[VALUE_CHANGED], 0, NULL);
			is_pressing = FALSE;
		}
		break;
	default:
		break;
	}

	return FALSE;
}

static gint 
cb_thumb_event(GnomeCanvasItem *item, GdkEvent *event, SkinHScale* hscale)
{
	static gboolean is_pressing = FALSE;
	static gdouble x;

	SkinHScalePrivate *priv;
	gdouble item_x;
	gdouble new_x;

	priv = hscale->priv;

	item_x = event->button.x;

	switch (event->type) 
	{
	case GDK_BUTTON_PRESS:
		if(event->button.button == 1) 
		{
			gnome_canvas_item_set(item, "pixbuf", priv->thumb_subpixbuf[2], NULL);
			is_pressing = TRUE;
			x = item_x;
		}
		break;
	case GDK_MOTION_NOTIFY:
		if(is_pressing && (item_x > priv->x1 && item_x < priv->x2))
		{
			new_x = item_x;
			x = new_x;
			priv->value = (x - priv->x1) / (priv->x2 - priv->x1) * (priv->max - priv->min);
			priv->need_value_update = TRUE;
			skin_hscale_value_update(hscale);

			g_signal_emit(hscale, signals[VALUE_CHANGED], 0, NULL);
		}
		gnome_canvas_item_set(item, "pixbuf", priv->thumb_subpixbuf[1], NULL);
		break;
	case GDK_BUTTON_RELEASE:
		gnome_canvas_item_set(item, "pixbuf", priv->thumb_subpixbuf[1], NULL);
		is_pressing = FALSE;
		break;
	default:
		gnome_canvas_item_set(item, "pixbuf", priv->thumb_subpixbuf[0], NULL);
		break;
	}

	return FALSE;
}

static void 
skin_hscale_pixbuf_update(SkinHScale *hscale)
{
	SkinHScalePrivate *priv;
	gdouble pw, ph;
	gint i;

	priv = hscale->priv;

	if(!priv->need_pixbuf_update)
		return;

	if(priv->has_fill && !priv->has_fill_item)
	{
		priv->fill_item = gnome_canvas_item_new(hscale->priv->root, 
				gnome_canvas_pixbuf_get_type(), 
				"x", priv->x1,
				"y", priv->y1,
				"x-in-pixels", TRUE,
				"y-in-pixels", TRUE,
				NULL);

		priv->has_fill_item = TRUE;
		g_signal_connect(G_OBJECT(priv->fill_item), "event", G_CALLBACK(cb_fill_event), hscale);
	}
	else if(priv->has_fill && priv->has_fill_item)
	{
		gnome_canvas_item_set(priv->fill_item,
				"x-in-pixels", TRUE,
				"y-in-pixels", TRUE,
				"x", priv->x1,
				"y", priv->y1,
				//"pixbuf", priv->fill_pixbuf,
				NULL);
	}
	else if(!priv->has_fill && priv->fill_item)
	{
		gnome_canvas_item_hide(priv->fill_item);
	}

	if(priv->has_thumb && !priv->has_thumb_item)
	{

		pw = gdk_pixbuf_get_width(priv->thumb_pixbuf);
		ph = gdk_pixbuf_get_height(priv->thumb_pixbuf);

		pw = pw / SUBPIXBUF;

		for(i = 0; i < SUBPIXBUF; ++i)
		{
			priv->thumb_subpixbuf[i] = gdk_pixbuf_new_subpixbuf(priv->thumb_pixbuf,
					(gint)(i * pw), 0, (gint)pw, (gint)ph);
		}

		if(priv->has_fill)
			ph = ph - gdk_pixbuf_get_height(priv->fill_pixbuf);

		priv->thumb_item = gnome_canvas_item_new(hscale->priv->root, 
				gnome_canvas_pixbuf_get_type(),
				"x", priv->x1,
				"x-in-pixels", TRUE,
				"y", priv->y1 - ph / 2.0,
				"y-in-pixels", TRUE,
				"pixbuf", priv->thumb_subpixbuf[0],
				NULL);

		priv->has_thumb_item = TRUE;
		g_signal_connect(G_OBJECT(priv->thumb_item), "event", G_CALLBACK(cb_thumb_event), hscale);
	}
	else if(priv->has_thumb && priv->has_thumb_item)
	{
		pw = gdk_pixbuf_get_width(priv->thumb_pixbuf);
		ph = gdk_pixbuf_get_height(priv->thumb_pixbuf);

		pw = pw / SUBPIXBUF;

		for(i = 0; i < SUBPIXBUF; ++i)
		{
			if(priv->thumb_subpixbuf[i])
				g_object_unref(priv->thumb_subpixbuf[i]);

			priv->thumb_subpixbuf[i] = gdk_pixbuf_new_subpixbuf(priv->thumb_pixbuf,
					(gint)(i * pw), 0, (gint)pw, (gint)ph);
		}

		if(priv->has_fill)
			pw = pw - gdk_pixbuf_get_width(priv->fill_pixbuf);

		gnome_canvas_item_set(priv->thumb_item,
				"x-in-pixels", TRUE,
				"y-in-pixels", TRUE,
				"x", priv->x1 - pw / 2.0,
				"y", priv->y1,
				"pixbuf", priv->thumb_subpixbuf[0],
				NULL);

		//FIXME: 应不应在此调用？ 
		//skin_hscale_value_update(hscale);
	}
	else if(!priv->has_thumb && priv->has_thumb_item)
	{
		gnome_canvas_item_hide(priv->thumb_item);
	}
	
	priv->need_pixbuf_update = FALSE;
}

static void 
skin_hscale_value_update(SkinHScale *hscale)
{
	SkinHScalePrivate *priv;
	GdkPixbuf *pixbuf;
	gint height;
	gdouble range;
	gdouble position;
	gdouble thumb_width;

	priv = hscale->priv;

	if(!priv->need_value_update)
		return;

	if(priv->has_thumb)
	{
		thumb_width = gdk_pixbuf_get_width(priv->thumb_pixbuf);
	}
	else
		thumb_width = 0.0;

	range = priv->max - priv->min;
	if(range > 0)
	{
		position = (range - (priv->max - priv->value)) / range * (priv->x2 - priv->x1 - thumb_width / 4.0);
	}
	else
	{
		position = 0.0;
	}

	if(priv->has_fill)
	{
		if(position > 1.0) // position > 0.0 will get some warnings
		{
			gnome_canvas_item_show(priv->fill_item);
			height = gdk_pixbuf_get_height(priv->fill_pixbuf);
			pixbuf = gdk_pixbuf_new_subpixbuf(priv->fill_pixbuf, 0, 0, (gint)(position + 0.5), height);
			gnome_canvas_item_set(priv->fill_item,
					"pixbuf", pixbuf,
					NULL);
		}
		else
		{
			gnome_canvas_item_hide(priv->fill_item);
		}
	}

	if(priv->has_thumb)
	{
		gnome_canvas_item_set(priv->thumb_item,
				"x", priv->x1 + position,
				NULL);
	}

	priv->need_value_update = FALSE;
}

static void 
skin_hscale_update(SkinHScale *hscale)
{
	g_return_if_fail(SKIN_IS_HSCALE(hscale));

	g_object_get(G_OBJECT(hscale),
			"x1", &hscale->priv->x1,
			"y1", &hscale->priv->y1,
			"x2", &hscale->priv->x2,
			"y2", &hscale->priv->y2,
			NULL);

	skin_hscale_pixbuf_update(hscale);
	skin_hscale_value_update(hscale);
}

static void
skin_hscale_construct(SkinHScale *hscale, 
						GnomeCanvasGroup *root, 
						const gchar *first_arg_name, 
						va_list args)
{
	g_return_if_fail(GNOME_IS_CANVAS_GROUP(root));
	g_return_if_fail(SKIN_IS_HSCALE(hscale));

	hscale->priv->root = root;
	g_object_set_valist(G_OBJECT(hscale), first_arg_name, args);
	skin_hscale_update(hscale);
}


SkinHScale *
skin_hscale_new(GnomeCanvasGroup *root, const char *first_arg_name, ...)
{
	SkinHScale *hscale;
	SkinHScalePrivate *priv;
	GnomeCanvasItem *item;
	va_list args;

	g_return_val_if_fail(GNOME_IS_CANVAS_GROUP(root), NULL);

	item = gnome_canvas_item_new(root,
			skin_hscale_get_type(),
			"fill-color-rgba", 0xff00ff00, //只是为了能够影响鼠标事件
			NULL);

	g_return_val_if_fail(GNOME_IS_CANVAS_ITEM(item), NULL);

	hscale = SKIN_HSCALE(item);
	priv = hscale->priv;

	g_signal_connect(G_OBJECT(item), "event", G_CALLBACK(cb_rect_event), hscale);
	va_start(args, first_arg_name);
	skin_hscale_construct(hscale, root, first_arg_name, args);
	va_end(args);

	return hscale;
}

void
skin_hscale_set_range(SkinHScale *hscale, gdouble min, gdouble max)
{
	g_return_if_fail(SKIN_IS_HSCALE(hscale));
	g_return_if_fail(min >= 0.0 && max > min);


	hscale->priv->min = min;
	hscale->priv->max = max;
	hscale->priv->need_value_update = TRUE;
	skin_hscale_value_update(hscale);
}

void
skin_hscale_set_value(SkinHScale *hscale, gdouble value)
{
	g_return_if_fail(SKIN_IS_HSCALE(hscale));
	g_return_if_fail(value >= 0.0);

	//if(hscale->priv->value == value)
	//	return;

	hscale->priv->value = value;
	hscale->priv->need_value_update = TRUE;
	skin_hscale_value_update(hscale);
}

void
skin_hscale_set_range_and_value(SkinHScale *hscale, gdouble min, gdouble max, gdouble value)
{
	g_return_if_fail(SKIN_IS_HSCALE(hscale));
	g_return_if_fail(min >= 0.0 && max > min && value >= min && value <= max);

	hscale->priv->min = min;
	hscale->priv->max = max;
	hscale->priv->value = value;
	hscale->priv->need_value_update = TRUE;
	skin_hscale_value_update(hscale);
}

gdouble 
skin_hscale_get_value(SkinHScale *hscale)
{
	g_return_val_if_fail(SKIN_IS_HSCALE(hscale), -1.0);
	return hscale->priv->value;
}

