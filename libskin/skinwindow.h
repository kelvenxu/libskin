/* vi: set sw=4 ts=4: */
/*
 * @file: libskin/skinwindow.h A main skin window widget.
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
 * */

#ifndef __SKINWINDOW_H__
#define __SKINWINDOW_H__  1

#include <glib-object.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <libgnomecanvas/libgnomecanvas.h>

G_BEGIN_DECLS

#define SKIN_TYPE_WINDOW              (skin_window_get_type())
#define SKIN_WINDOW(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), SKIN_TYPE_WINDOW, SkinWindow)) 
#define SKIN_WINDOW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), SKIN_TYPE_WINDOW, SkinWindowClass)) 
#define SKIN_IS_WINDOW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), SKIN_TYPE_WINDOW))
#define SKIN_IS_WINDOW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE((klass), SKIN_TYPE_WINDOW))
#define SKIN_WINDOW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), SKIN_TYPE_WINODW, SkinWindowClass))

typedef struct _SkinWindow             SkinWindow;
typedef struct _SkinWindowClass        SkinWindowClass;
typedef struct _SkinWindowPrivate        SkinWindowPrivate;

struct _SkinWindow
{
	GtkWidget parent;

	GtkWidget* widget;
	GtkWidget* canvas;
	GnomeCanvasGroup* canvas_root;
	GnomeCanvasItem* canvas_item;
	GdkPixbuf* image;
	gboolean movable;
	gboolean resizable;

	gchar* test_string;

	gint width;
	gint height;

	SkinWindowPrivate* priv;
};

struct _SkinWindowClass
{
	GtkWidgetClass parent_class;
	void (*right_button_press) (SkinWindow* skin_window, gpointer* user_data);
	void (*delete_event) (SkinWindow* skin_window, gpointer* user_data);
	void (*move_event) (SkinWindow* skin_window, gpointer* user_data);
};

GType skin_window_get_type(void) G_GNUC_CONST;

SkinWindow* skin_window_new(GdkPixbuf* pixbuf);
void skin_window_destroy(SkinWindow* skin_window);
void skin_window_show(SkinWindow* skin_window);
void skin_window_hide(SkinWindow* skin_window);
void skin_window_move(SkinWindow* skin_window, gint x, gint y);
void skin_window_set_image(SkinWindow* skin_window, GdkPixbuf* pixbuf);

G_END_DECLS

#endif /*__SKINWINDOW_H__ */
