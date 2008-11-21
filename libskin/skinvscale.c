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

#include "skinvscale.h"

#define SUBPIXBUF 4

G_DEFINE_TYPE(SkinVScale, skin_vscale, GNOME_TYPE_CANVAS_RECT);

#define SKIN_VSCALE_GET_PRIVATE(o)\
	(G_TYPE_INSTANCE_GET_PRIVATE ((o), SKIN_TYPE_VSCALE, SkinVScalePrivate))

struct _SkinVScalePrivate 
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

static void skin_vscale_dispose (SkinVScale *self);
static void skin_vscale_finalize (SkinVScale *self);
static void skin_vscale_init (SkinVScale *self);
static void skin_vscale_class_init (SkinVScaleClass *self_class);
static void skin_vscale_set_property (GObject      *object,
									guint         prop_id,
									const GValue *value,
									GParamSpec   *pspec);
static void skin_vscale_get_property (GObject      *object,
									guint         prop_id,
									GValue       *value,
									GParamSpec   *pspec);
static void skin_vscale_pixbuf_update(SkinVScale *hscale);
static void skin_vscale_value_update(SkinVScale *hscale);
static void skin_vscale_update(SkinVScale *hscale);

static void
skin_vscale_dispose (SkinVScale *self)
{
}

static void
skin_vscale_finalize (SkinVScale *self)
{
}

static void
skin_vscale_init (SkinVScale *self)
{
	SkinVScalePrivate *priv;
	priv = g_new0(SkinVScalePrivate, 1);
	self->priv = priv;

	priv->root = NULL;
	priv->fill_pixbuf = NULL;
	priv->thumb_pixbuf = NULL;
	priv->fill_item = NULL;
	priv->thumb_item = NULL;

	priv->has_fill = FALSE;
	priv->has_thumb = FALSE;

	priv->x1 = 0.0;
	priv->y1 = 0.0;
	priv->x2 = 0.0;
	priv->y2 = 0.0;
	priv->min = 0.0;
	priv->max = 0.0;
	priv->value = 0.0;
}

static void
skin_vscale_class_init (SkinVScaleClass *self_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (self_class);

	g_type_class_add_private (self_class, sizeof (SkinVScalePrivate));
	object_class->dispose = (void (*) (GObject *object)) skin_vscale_dispose;
	object_class->finalize = (void (*) (GObject *object)) skin_vscale_finalize;
	object_class->set_property = skin_vscale_set_property;
	object_class->get_property = skin_vscale_get_property;

	signals[VALUE_CHANGED] = g_signal_new("value_changed", 
			G_TYPE_FROM_CLASS(object_class),
			G_SIGNAL_RUN_LAST,
			G_STRUCT_OFFSET(SkinVScaleClass, value_changed),
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
skin_vscale_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
	SkinVScale *hscale;
	SkinVScalePrivate *priv;
	GdkPixbuf *pixbuf;
	gdouble val;

	g_return_if_fail(object != NULL);
	g_return_if_fail(SKIN_IS_VSCALE(object));

    hscale = SKIN_VSCALE(object);
	priv = hscale->priv;

    switch (prop_id)
    {
		case PROP_FILL_PIXBUF:
			if(g_value_get_object(value))
				pixbuf = GDK_PIXBUF(g_value_get_object(value));
			else
				pixbuf = NULL;
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
			skin_vscale_update(hscale);
			break;
		case PROP_THUMB_PIXBUF:
			if(g_value_get_object(value))
				pixbuf = GDK_PIXBUF(g_value_get_object(value));
			else
				pixbuf = NULL;
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
			skin_vscale_update(hscale);
			break;
		case PROP_MIN:
			priv->min = g_value_get_double(value);
			priv->need_value_update = TRUE;
			skin_vscale_update(hscale);
			break;
		case PROP_MAX:
			priv->max = g_value_get_double(value);
			priv->need_value_update = TRUE;
			skin_vscale_update(hscale);
			break;
		case PROP_VALUE:
			val = g_value_get_double(value);
			if(val != priv->value)
			{
				priv->value = val;
				priv->need_value_update = TRUE;
			}
			skin_vscale_update(hscale);
			break;
		default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

static void
skin_vscale_get_property (GObject      *object,
                            guint         prop_id,
                            GValue       *value,
                            GParamSpec   *pspec)
{
	SkinVScale *hscale;
	SkinVScalePrivate *priv;

	g_return_if_fail(object != NULL);
	g_return_if_fail(SKIN_IS_VSCALE(object));

    hscale = SKIN_VSCALE(object);
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
cb_rect_event(GnomeCanvasItem *item, GdkEvent *event, SkinVScale* hscale)
{
	static gboolean is_pressing = FALSE;
	SkinVScalePrivate *priv;
	
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
			priv->value = (priv->y1 - event->button.y) / (priv->y1 - priv->y2) * (priv->max - priv->min);
			priv->need_value_update = TRUE;
			skin_vscale_value_update(hscale);
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
cb_fill_event(GnomeCanvasItem *item, GdkEvent *event, SkinVScale* hscale)
{
	static gboolean is_pressing = FALSE;
	SkinVScalePrivate *priv;

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
			priv->value = (priv->y1 - event->button.y) / (priv->y1 - priv->y2) * (priv->max - priv->min);
			priv->need_value_update = TRUE;
			skin_vscale_value_update(hscale);
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
cb_thumb_event(GnomeCanvasItem *item, GdkEvent *event, SkinVScale* hscale)
{
	static gboolean is_pressing = FALSE;
	static gdouble y; //FIXME: static变量在声明了多个控件后可能出现混乱

	SkinVScalePrivate *priv;
	gdouble item_y;
	gdouble new_y;

	priv = hscale->priv;
	item_y = event->button.y;

	switch (event->type) 
	{
	case GDK_BUTTON_PRESS:
		if(event->button.button == 1) 
		{
			gnome_canvas_item_set(item, "pixbuf", priv->thumb_subpixbuf[2], NULL);
			is_pressing = TRUE;
			y = item_y;
		}
		break;
	case GDK_MOTION_NOTIFY:
		if(is_pressing && (item_y > priv->y2 && item_y < priv->y1))
		{
			new_y = item_y;
		
			y = new_y;
			priv->value = (y - priv->y1) / (priv->y1 - priv->y2) * (priv->max - priv->min);
			priv->need_value_update = TRUE;
			skin_vscale_value_update(hscale);

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
skin_vscale_pixbuf_update(SkinVScale *hscale)
{
	SkinVScalePrivate *priv;
	gdouble pw, ph;
	gint i;

	priv = hscale->priv;

	if(!priv->need_pixbuf_update)
		return;

	if(priv->fill_pixbuf && GDK_IS_PIXBUF(priv->fill_pixbuf) && !priv->fill_item)
	{
		priv->fill_item = gnome_canvas_item_new(hscale->priv->root, 
				gnome_canvas_pixbuf_get_type(), 
				"x-in-pixels", TRUE,
				"y-in-pixels", TRUE,
				"x", priv->x1,
				"y", priv->y1,
				NULL);

		priv->has_fill = TRUE;
		g_signal_connect(G_OBJECT(priv->fill_item), "event", G_CALLBACK(cb_fill_event), hscale);
	}

	if(priv->thumb_pixbuf && GDK_IS_PIXBUF(priv->thumb_pixbuf) && !priv->thumb_item)
	{
		pw = gdk_pixbuf_get_width(priv->thumb_pixbuf);
		ph = gdk_pixbuf_get_height(priv->thumb_pixbuf);

		pw = pw / SUBPIXBUF;

		for(i = 0; i < SUBPIXBUF; ++i)
		{
			priv->thumb_subpixbuf[i] = gdk_pixbuf_new_subpixbuf(priv->thumb_pixbuf,
					(gint)(i * pw), 0, (gint)pw, (gint)ph);
		}

		pw = pw - gdk_pixbuf_get_width(priv->fill_pixbuf);
		priv->thumb_item = gnome_canvas_item_new(hscale->priv->root, 
				gnome_canvas_pixbuf_get_type(),
				"x-in-pixels", TRUE,
				"y-in-pixels", TRUE,
				"x", priv->x1 - pw / 2.0,
				"y", priv->y1,
				"pixbuf", priv->thumb_subpixbuf[0],
				NULL);

		priv->has_thumb = TRUE;
		g_signal_connect(G_OBJECT(priv->thumb_item), "event", G_CALLBACK(cb_thumb_event), hscale);
	}
	
	priv->need_pixbuf_update = FALSE;
}

static void 
skin_vscale_value_update(SkinVScale *hscale)
{
	SkinVScalePrivate *priv;
	GdkPixbuf *pixbuf;
	gint width;
	gdouble range;
	gdouble position;

	priv = hscale->priv;

	if(!priv->need_value_update)
		return;

	range = priv->max - priv->min;
	if(range > 0)
	{
		// y1 > y2
		position = priv->value / range * (priv->y1 - priv->y2);
	}
	else
	{
		position = 0.0;
	}

	if(priv->has_fill)
	{
		if(position > 1.0)
		{
			gnome_canvas_item_show(priv->fill_item);
			width = gdk_pixbuf_get_width(priv->fill_pixbuf);
			pixbuf = gdk_pixbuf_new_subpixbuf(priv->fill_pixbuf, 0, 0, width, (gint)position);
			gnome_canvas_item_set(priv->fill_item,
					"y", priv->y1 - position,
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
				"y", priv->y1 - position, 
				"pixbuf", priv->thumb_subpixbuf[0],
				NULL);
	}

	priv->need_value_update = FALSE;
}

static void 
skin_vscale_update(SkinVScale *hscale)
{
	g_return_if_fail(SKIN_IS_VSCALE(hscale));

	g_object_get(G_OBJECT(hscale),
			"x1", &hscale->priv->x1,
			"y1", &hscale->priv->y1,
			"x2", &hscale->priv->x2,
			"y2", &hscale->priv->y2,
			NULL);

	skin_vscale_pixbuf_update(hscale);
	skin_vscale_value_update(hscale);
}

static void
skin_vscale_construct(SkinVScale *hscale, 
						GnomeCanvasGroup *root, 
						const gchar *first_arg_name, 
						va_list args)
{
	g_return_if_fail(GNOME_IS_CANVAS_GROUP(root));
	g_return_if_fail(SKIN_IS_VSCALE(hscale));

	hscale->priv->root = root;
	g_object_set_valist(G_OBJECT(hscale), first_arg_name, args);
	skin_vscale_update(hscale);
}

// 其(x1, y1)在左下角, (x2,y2)在右上角

SkinVScale *
skin_vscale_new(GnomeCanvasGroup *root, const char *first_arg_name, ...)
{
	SkinVScale *hscale;
	SkinVScalePrivate *priv;
	GnomeCanvasItem *item;
	va_list args;

	g_return_val_if_fail(GNOME_IS_CANVAS_GROUP(root), NULL);

	item = gnome_canvas_item_new(root,
			skin_vscale_get_type(),
			"fill-color-rgba", 0xff00ff00, //只是为了能够影响鼠标事件
			NULL);

	g_return_val_if_fail(GNOME_IS_CANVAS_ITEM(item), NULL);

	hscale = SKIN_VSCALE(item);
	priv = hscale->priv;

	g_signal_connect(G_OBJECT(item), "event", G_CALLBACK(cb_rect_event), hscale);
	va_start(args, first_arg_name);
	skin_vscale_construct(hscale, root, first_arg_name, args);
	va_end(args);

	return hscale;
}

void
skin_vscale_set_value(SkinVScale *hscale, gdouble value)
{
	g_return_if_fail(SKIN_IS_VSCALE(hscale));
	g_return_if_fail(value >= 0.0);

	if(hscale->priv->value == value)
		return;

	hscale->priv->value = value;
	hscale->priv->need_value_update = TRUE;

	skin_vscale_value_update(hscale);
}

