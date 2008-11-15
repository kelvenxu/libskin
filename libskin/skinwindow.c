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

	// Position
	gint x;
	gint y;
	gint width;
	gint height;
};

#define SKIN_WINDOW_GET_PRIVATE(obj)  (G_TYPE_INSTANCE_GET_PRIVATE((obj), SKIN_TYPE_WINDOW, SkinWindowPrivate))

enum {
	RIGHT_BUTTON_PRESS,
	//DELETE_EVENT,
	MOVE_EVENT,
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
static gboolean 
skin_window_canvas_item_event(GnomeCanvasItem *item, 
								GdkEvent *event, 
								//GtkWindow* window);
								SkinWindow* skin_window);

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

    g_type_class_add_private (class, sizeof (SkinWindowPrivate));

	g_print("new singal: right-button-signal\n");
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

static gboolean 
skin_window_canvas_item_event(GnomeCanvasItem *item, 
								GdkEvent *event, 
								SkinWindow* skin_window)
{
	GtkWindow* window = GTK_WINDOW(skin_window);
	static gint win_x0, win_y0;
	static gboolean is_pressing = FALSE;

	switch (event->type) 
	{
	case GDK_BUTTON_PRESS:
		if(event->button.button == 1) 
		{
			is_pressing = TRUE;
			win_x0 = (gint)((GdkEventButton*)event)->x; 
			win_y0 = (gint)((GdkEventButton*)event)->y; 
		}

		break;

	case GDK_MOTION_NOTIFY:
		if(is_pressing)
		{
			//窗体的移动是针对窗体的左上角坐标而言的
			gtk_window_move(window,
					(gint)((GdkEventButton*)event)->x_root - win_x0, 
					(gint)((GdkEventButton*)event)->y_root - win_y0);

			g_signal_emit(skin_window,
					widget_signals[MOVE_EVENT],
					0 /* details */,
					NULL);

			break;
		}
		else
		{
			//printf("motion\n");
			break;
		}
		break;

	case GDK_BUTTON_RELEASE:
		is_pressing = FALSE;
		//printf("release...\n");
		if(event->button.button == 3)
		{
			g_signal_emit(skin_window,
					widget_signals[RIGHT_BUTTON_PRESS],
					0 /* details */,
					NULL);
		}
		break;

	default:
		//printf("else what...\n");
		//鼠标离开的状态
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

	gtk_widget_set_size_request(window, width, height);

	gdk_pixbuf_render_pixmap_and_mask(pixbuf, &pixmap, &mask, 100);
	gtk_widget_shape_combine_mask(window, mask, 0, 0);
}


static void
skin_window_construct(SkinWindow* skin_window, 
		GdkPixbuf* pixbuf)
{
	GtkWidget *widget;
	g_return_if_fail(SKIN_IS_WINDOW(skin_window));
	g_return_if_fail(GDK_IS_PIXBUF(pixbuf));

	widget = GTK_WIDGET(skin_window);

	skin_window->canvas = gnome_canvas_new_aa();
	skin_window->canvas_root = gnome_canvas_root(GNOME_CANVAS(skin_window->canvas));

	gnome_canvas_set_center_scroll_region(GNOME_CANVAS(skin_window->canvas), FALSE);
	skin_window->canvas_item = gnome_canvas_item_new(skin_window->canvas_root,
							gnome_canvas_pixbuf_get_type(),
							"pixbuf", pixbuf,
							NULL);

	gtk_container_add(GTK_CONTAINER(widget), skin_window->canvas);

	gtk_widget_decorated_with_pixbuf(widget, pixbuf); 

	g_signal_connect(G_OBJECT(skin_window->canvas_item), 
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
	
	gnome_canvas_item_set(window->canvas_item,
			"pixbuf", pixbuf,
			NULL);
}
