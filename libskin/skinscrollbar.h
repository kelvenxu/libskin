/* vi: set sw=4 ts=4: */
/*
 * skinscrollbar.h
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

#ifndef __SKINSCROLLBAR_H__
#define __SKINSCROLLBAR_H__  1

#include    <glib-object.h>
#include	<glib.h>
#include	<gtk/gtk.h>
#include	<libgnomecanvas/libgnomecanvas.h>

G_BEGIN_DECLS

#define SKIN_TYPE_SCROLL_BAR (skin_scroll_bar_get_type ())
#define SKIN_SCROLL_BAR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SKIN_TYPE_SCROLL_BAR, SkinScrollBar))
#define SKIN_SCROLL_BAR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SKIN_TYPE_SCROLL_BAR, SkinScrollBarClass))
#define SKIN_IS_SCROLL_BAR(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SKIN_TYPE_SCROLL_BAR))
#define SKIN_IS_SCROLL_BAR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SKIN_TYPE_SCROLL_BAR))
#define SKIN_SCROLL_BAR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SKIN_TYPE_SCROLL_BAR, SkinScrollBarClass))
typedef struct _SkinScrollBarPrivate SkinScrollBarPrivate;

typedef struct 
{
	GnomeCanvasGroup parent;

	SkinScrollBarPrivate *priv;
} SkinScrollBar;

typedef struct 
{
	GnomeCanvasGroupClass parent;
} SkinScrollBarClass;

GType skin_scroll_bar_get_type();

SkinScrollBar *skin_scroll_bar_new(GnomeCanvasGroup *root, 
									GdkPixbuf *button_pixbuf, 
									GdkPixbuf *bar_pixbuf, 
									GdkPixbuf *thumb_pixbuf,
									gdouble x1, //not used 
									gdouble y1,
									gdouble x2,
									gdouble y2);

void skin_scroll_bar_rebuild(SkinScrollBar *bar, 
								GdkPixbuf *button_pixbuf, 
								GdkPixbuf *bar_pixbuf, 
								GdkPixbuf *thumb_pixbuf,
								gdouble x1, //not used 
								gdouble y1,
								gdouble x2,
								gdouble y2);

void skin_scroll_bar_set_adjustment(SkinScrollBar *bar, GtkAdjustment *adj);
gdouble skin_scroll_bar_get_width(SkinScrollBar *bar);
gdouble skin_scroll_bar_get_height(SkinScrollBar *bar);
void skin_scroll_bar_set_height(SkinScrollBar *bar, gdouble height);

G_END_DECLS

#endif /*__SKINSCROLLBAR_H__ */
