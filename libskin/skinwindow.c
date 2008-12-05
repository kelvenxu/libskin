/* vi: set sw=4 ts=4: */
/*
 * @file: libskin/skinwindow.c A main skin window widget.
 *
 * @Copyright (C) 2008 - kelvenxu <kelvenxu@gmail.com>.
 *
 * This file is part of libskin.
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
 */

#include "skinwindow.h"
#include "skin-marshal.h"
#include <libgnomecanvas/libgnomecanvas.h>
#include <gtk/gtk.h>

struct _SkinWindowPrivate
{
	gboolean dispose_has_run;

	GtkWidget *canvas;
	GnomeCanvasGroup *root;
	GnomeCanvasItem *item;

	// Position
	gint x;
	gint y;
	gint width;
	gint height;

	// 最小尺寸
	gint min_width;
	gint min_height;

	// 改变尺寸时图片缩放的位置
	gint resize_x1;
	gint resize_y1;
	gint resize_x2;
	gint resize_y2;

	GdkPixbuf *pixbuf;
	gboolean resizeable;
	gint resize_step;
	
	gint movebar_width;
	gint corner_size;

	// 鼠标切换的标志
	gboolean cursor_flag; 
};

#define SKIN_WINDOW_GET_PRIVATE(obj)  (G_TYPE_INSTANCE_GET_PRIVATE((obj), SKIN_TYPE_WINDOW, SkinWindowPrivate))

enum {
	RIGHT_BUTTON_PRESS,
	//DELETE_EVENT,
	MOVE_EVENT,
	CHANGE_SIZE,
    LAST_SIGNAL
};

enum {
    PROP_0,
};

/* --- variables --- */
static guint widget_signals[LAST_SIGNAL] = { 0 };

/* --- prototypes --- */
static void skin_window_set_property  (GObject          *object,
                                         guint             prop_id,
                                         const GValue     *value,
                                         GParamSpec       *pspec);
static void skin_window_get_property  (GObject          *object,
                                         guint             prop_id,
                                         GValue           *value,
                                         GParamSpec       *pspec);

//static void gtk_window_get_rect(GtkWindow *win, GdkRectangle *rect);
static gboolean skin_window_canvas_item_event(GnomeCanvasItem *item, GdkEvent *event, SkinWindow* skin_window);

static void skin_window_class_init(SkinWindowClass *class);
static void skin_window_init (GTypeInstance *instance, gpointer g_class);
static void gtk_widget_decorated_with_pixbuf(GtkWidget* window, 
										GdkPixbuf* pixbuf);

/* --- functions --- */
GType
skin_window_get_type(void)
{
	static GType skin_window_type;

	if (!skin_window_type) {
		static const GTypeInfo object_info = {
			sizeof (SkinWindowClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) skin_window_class_init,
			(GClassFinalizeFunc) NULL,
			NULL,			/* class_data */
			sizeof (SkinWindow),
			0,			/* n_preallocs */
			(GInstanceInitFunc) skin_window_init,
			NULL			/* value_table */
		};

		skin_window_type = g_type_register_static(GTK_TYPE_WINDOW, 
								"SkinWindow",
							    &object_info, 
								0);
	}

	return skin_window_type;
}

static void 
skin_window_class_init(SkinWindowClass *class)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (class);

    gobject_class->set_property = skin_window_set_property;
    gobject_class->get_property = skin_window_get_property;

	class->right_button_press = NULL;
	class->delete_event = NULL;
	class->move_event = NULL;
	class->change_size = NULL;

    g_type_class_add_private (class, sizeof (SkinWindowPrivate));

	widget_signals[RIGHT_BUTTON_PRESS] = 
			g_signal_new("right-button-press",
						G_TYPE_FROM_CLASS(gobject_class),
						G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
						G_STRUCT_OFFSET(SkinWindowClass, right_button_press),
						NULL,
						NULL,
						g_cclosure_marshal_VOID__VOID,
						G_TYPE_NONE,
						0, 
						NULL);

	widget_signals[MOVE_EVENT] = 
			g_signal_new("move-event",
						G_TYPE_FROM_CLASS(gobject_class),
						G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
						G_STRUCT_OFFSET(SkinWindowClass, move_event),
						NULL, 
						NULL,
						g_cclosure_marshal_VOID__VOID,
						G_TYPE_NONE,
						0, 
						NULL);

	widget_signals[CHANGE_SIZE] = 
			g_signal_new("change-size",
						G_TYPE_FROM_CLASS(gobject_class),
						G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
						G_STRUCT_OFFSET(SkinWindowClass, change_size),
						NULL,
						NULL,
						gtk_marshal_VOID__INT_INT,
						G_TYPE_NONE,
						2,
						G_TYPE_INT, 
						G_TYPE_INT);

}

static void
skin_window_init (GTypeInstance *instance,
					gpointer g_class)
{
    SkinWindow *self = (SkinWindow*)instance;

	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, SKIN_TYPE_WINDOW, 
			SkinWindowPrivate);

	self->priv->x = 0;
	self->priv->y = 0;
	self->priv->width = -1;
	self->priv->height = -1;

	self->priv->pixbuf = NULL;
	self->priv->resizeable = FALSE;

	self->priv->resize_step = 5;
	self->priv->movebar_width = 32;
	self->priv->corner_size = 24;

	self->priv->cursor_flag = FALSE;
}

static void
skin_window_set_property(GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    SkinWindow *skinwindow;

    skinwindow = SKIN_WINDOW(object);

    switch (prop_id)
    {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
skin_window_get_property (GObject      *object,
                            guint         prop_id,
                            GValue       *value,
                            GParamSpec   *pspec)
{
    SkinWindow *skinwindow;

    skinwindow = SKIN_WINDOW(object);

    switch (prop_id)
    {

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

/*
static void 
gtk_window_get_rect(GtkWindow *win, GdkRectangle *rect)
{
	gtk_window_get_position(win, &(rect->x), &(rect->y));
	gtk_window_get_size(win, &(rect->width), &(rect->height));
}
*/

static gboolean 
skin_window_canvas_item_event(GnomeCanvasItem *item, 
								GdkEvent *event, 
								SkinWindow* skin_window)
{
	SkinWindowPrivate *priv = skin_window->priv;
	GtkWindow* window = GTK_WINDOW(skin_window);
	GtkWidget* widget = GTK_WIDGET(skin_window);

	static gint win_x0, win_y0;
	static gboolean is_pressing = FALSE;

	gint x, y;
	static gint old_x, old_y;
	static gint count = 0;

	x = (gint)((GdkEventButton*)event)->x; 
	y = (gint)((GdkEventButton*)event)->y; 
	switch (event->type) 
	{
	case GDK_BUTTON_PRESS:
		if(event->button.button == 1) 
		{
			is_pressing = TRUE;
			win_x0 = x;//(gint)((GdkEventButton*)event)->x; 
			win_y0 = y;//(gint)((GdkEventButton*)event)->y; 
			old_x = x;
			old_y = y;
		}

		break;

	case GDK_MOTION_NOTIFY:
		// 当不在resize状态时，可以移动窗体
		if(is_pressing && !priv->cursor_flag)
		{
			//窗体的移动是针对窗体的左上角坐标而言的
			gtk_window_move(window,
					(gint)((GdkEventButton*)event)->x_root - win_x0, 
					(gint)((GdkEventButton*)event)->y_root - win_y0);

			g_signal_emit(skin_window,
					widget_signals[MOVE_EVENT],
					0 /* details */,
					NULL);
		}

		if(!priv->resizeable) break;

		//static gint delay = 0;
		if((priv->cursor_flag == FALSE) 
				&& (x > priv->width - priv->corner_size) 
				&& (y > priv->height - priv->corner_size)) 
		{
			GdkCursor *cursor = gdk_cursor_new(GDK_BOTTOM_RIGHT_CORNER);
			gdk_window_set_cursor(widget->window, cursor);
			priv->cursor_flag = TRUE;
		}
		else if(priv->cursor_flag && (is_pressing == FALSE))
		{
			gdk_window_set_cursor(widget->window, NULL);
			priv->cursor_flag = FALSE;
		}

		if(is_pressing && priv->cursor_flag && (++count % priv->resize_step == 0))
		{
			gint rx = 0, ry = 0;
			if(x > old_x)
			{
				priv->width += priv->resize_step;
				rx = priv->resize_step;
			}
			else if(x < old_x)
			{
				priv->width -= priv->resize_step;
				rx = -1 * priv->resize_step;
			}

			if(y > old_y)
			{
				priv->height += priv->resize_step;
				ry = priv->resize_step;
			}
			else if(y < old_y)
			{
				priv->height -= priv->resize_step;
				ry = -1 * priv->resize_step;
			}
			
			if(priv->height < priv->min_height)
			{
				ry = 0;
				priv->height = priv->min_height;
			}
			if(priv->width < priv->min_width)
			{
				rx = 0;
				priv->width = priv->min_width;
			}

			//GdkPixbuf *p = gdk_pixbuf_scale_simple(priv->pixbuf, 
			//		priv->width, 
			//		priv->height,
			//		GDK_INTERP_BILINEAR);

			static GdkPixbuf *des = NULL;

			if(rx != 0)
			{
				priv->resize_x1 = 159;
				priv->resize_y1 = 81;
				priv->resize_x2 = 160;
				priv->resize_y2 = 92;
				// 所有操作都是在原始图上做的，即priv->pixbuf
				printf("left_p\n");
				GdkPixbuf *left_p = gdk_pixbuf_new_subpixbuf(priv->pixbuf, 
						0, 0, 
						priv->resize_x1, 
						priv->min_height);
				printf("mid_p\n");
				GdkPixbuf *mid_p = gdk_pixbuf_new_subpixbuf(priv->pixbuf,
						priv->resize_x1, 0,
						priv->resize_x2 - priv->resize_x1,
						priv->min_height);
				printf("right_p\n");
				GdkPixbuf *right_p = gdk_pixbuf_new_subpixbuf(priv->pixbuf, 
						priv->resize_x2, 0, 
						priv->min_width - priv->resize_x2, 
						priv->min_height);
				
				printf("scaled mid_p\n");
				GdkPixbuf *scaled_mid_p = gdk_pixbuf_scale_simple(mid_p, 
						priv->width - (priv->resize_x1 + priv->min_width - priv->resize_x2),
						priv->min_height,
						GDK_INTERP_BILINEAR);

				printf("new des\n");
				des = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 
						priv->width, priv->min_height);

				printf("composite left_p\n");
				gdk_pixbuf_composite(left_p, des, 0, 0, 
						priv->resize_x1,
						priv->min_height,
						0.0, 0.0,
						1.0, 1.0,
						GDK_INTERP_BILINEAR,
						255);
				printf("composite mid_p\n");
				gdk_pixbuf_composite(scaled_mid_p, des, priv->resize_x1, 0, 
						priv->width - (priv->resize_x1 + priv->min_width - priv->resize_x2),
						priv->min_height,
						0.0, 0.0,
						1.0, 1.0,
						GDK_INTERP_BILINEAR,
						255);
				printf("composite right_p\n");
				gdk_pixbuf_composite(right_p, des, priv->width - (priv->min_width - priv->resize_x2) - 20, 0, 
						priv->min_width - priv->resize_x2, 
						priv->min_height,
						0.0, 0.0,
						1.0, 1.0,
						GDK_INTERP_BILINEAR,
						255);
				printf("composite right_p\n");
			//}


			//if(des != NULL)
			//{
				gnome_canvas_item_set(priv->item,
						"pixbuf", des,
						//"width", (gdouble)priv->width,
						//"height", (gdouble)priv->height,
						//"width-set", TRUE,
						//"height-set", TRUE,
						NULL);

				gtk_widget_decorated_with_pixbuf(widget, des);
				g_object_unref(des);

				old_x = x;
				old_y = y;

				ry = 0;
				g_signal_emit(widget, widget_signals[CHANGE_SIZE], 0, rx, ry);
			}
		}
		break;
	case GDK_BUTTON_RELEASE:
		is_pressing = FALSE;
		if(event->button.button == 3)
		{
			g_signal_emit(skin_window,
					widget_signals[RIGHT_BUTTON_PRESS],
					0 /* details */,
					NULL);
		}
		break;

	default:
		// 鼠标离开的状态
		break;
	}

	return FALSE;
}

static void 
gtk_widget_decorated_with_pixbuf(GtkWidget* window, 
		GdkPixbuf* pixbuf)
{
	GdkPixmap *pixmap;
	GdkBitmap *mask;
	int width, height;

	g_return_if_fail(GTK_IS_WIDGET(window));
	g_return_if_fail(GDK_IS_PIXBUF(pixbuf));

	gtk_window_set_decorated(GTK_WINDOW(window), FALSE);

	width = gdk_pixbuf_get_width(pixbuf);
	height = gdk_pixbuf_get_height(pixbuf);

	gtk_window_resize(GTK_WINDOW(window), width, height);
	gtk_widget_set_size_request(window, width, height);

	gdk_pixbuf_render_pixmap_and_mask(pixbuf, &pixmap, &mask, 100);
	gtk_widget_shape_combine_mask(window, mask, 0, 0);
}


static void
skin_window_construct(SkinWindow* skin_window, 
		GdkPixbuf* pixbuf)
{
	GtkWidget *widget;
	SkinWindowPrivate *priv;

	g_return_if_fail(SKIN_IS_WINDOW(skin_window));
	g_return_if_fail(GDK_IS_PIXBUF(pixbuf));

	priv = skin_window->priv;
	widget = GTK_WIDGET(skin_window);

	priv->canvas = gnome_canvas_new_aa();
	priv->root = gnome_canvas_root(GNOME_CANVAS(priv->canvas));

	gnome_canvas_set_center_scroll_region(GNOME_CANVAS(priv->canvas), FALSE);
	priv->item = gnome_canvas_item_new(priv->root,
							gnome_canvas_pixbuf_get_type(),
							"pixbuf", pixbuf,
							NULL);

	gtk_container_add(GTK_CONTAINER(widget), priv->canvas);

	gtk_widget_decorated_with_pixbuf(widget, pixbuf); 

	priv->width = gdk_pixbuf_get_width(pixbuf);
	priv->height = gdk_pixbuf_get_height(pixbuf);
	priv->min_width = priv->width;
	priv->min_height = priv->height;

	g_signal_connect(G_OBJECT(priv->item), 
					"event",
					G_CALLBACK(skin_window_canvas_item_event), 
					skin_window);
}

/**
 * skin_window_new:
 *
 * Create a skin window widget.
 *
 * Return: a new #GtkWidget.
 */
SkinWindow *
skin_window_new(GdkPixbuf* pixbuf)
{

	SkinWindow *window;
	g_return_val_if_fail(GDK_IS_PIXBUF(pixbuf), NULL);

	//window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	window = g_object_new(SKIN_TYPE_WINDOW, NULL);
	
	window->priv->pixbuf = pixbuf;
	skin_window_construct(window, pixbuf);

	return window;
}

void skin_window_destroy(SkinWindow* window)
{
	g_return_if_fail(SKIN_IS_WINDOW(window));
	gtk_widget_destroy(GTK_WIDGET(window));
}

void skin_window_show(SkinWindow* window)
{
	g_return_if_fail(SKIN_IS_WINDOW(window));
	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_window_move(GTK_WINDOW(window), 
			window->priv->x, 
			window->priv->y);
}

void skin_window_hide(SkinWindow* window)
{
	g_return_if_fail(SKIN_IS_WINDOW(window));
	gtk_window_get_position(GTK_WINDOW(window),
			&window->priv->x,
			&window->priv->y);
	gtk_widget_hide_all(GTK_WIDGET(window));
}

void skin_window_move(SkinWindow* window, gint x, gint y)
{
	g_return_if_fail(SKIN_IS_WINDOW(window));
	gtk_window_move(GTK_WINDOW(window), x, y);
	window->priv->x = x;
	window->priv->y = y;
}

void skin_window_set_image(SkinWindow* window,
						GdkPixbuf* pixbuf)
{
	g_return_if_fail(SKIN_IS_WINDOW(window));
	g_return_if_fail(GDK_IS_PIXBUF(pixbuf));

	gtk_widget_decorated_with_pixbuf(GTK_WIDGET(window), pixbuf);
	
	gnome_canvas_item_set(window->priv->item,
			"pixbuf", pixbuf,
			NULL);
}

void skin_window_set_pixbuf(SkinWindow* window, GdkPixbuf* pixbuf)
{
	skin_window_set_image(window, pixbuf);
}

GnomeCanvasGroup *
skin_window_get_canvas_root(SkinWindow *window)
{
	g_return_val_if_fail(SKIN_IS_WINDOW(window), NULL);

	return window->priv->root;
}

void
skin_window_set_resizeable(SkinWindow *window, gboolean resizeable)
{
	g_return_if_fail(SKIN_IS_WINDOW(window));

	window->priv->resizeable = resizeable;
}

void 
skin_window_set_resize_rect(SkinWindow *window, gint x1, gint y1, gint x2, gint y2)
{
	g_return_if_fail(SKIN_IS_WINDOW(window));

	window->priv->resize_x1 = x1;
	window->priv->resize_y1 = y1;
	window->priv->resize_x2 = x2;
	window->priv->resize_y2 = y2;
}

