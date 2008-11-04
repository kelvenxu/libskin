/* vi: set sw=4 ts=4: */
/*
 * skinbutton.h
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

#ifndef __SKIN_BUTTON_H__
#define __SKIN_BUTTON_H__  1


#include    <glib-object.h>
#include	<glib.h>
#include	<gtk/gtk.h>
#include	<libgnomecanvas/libgnomecanvas.h>

G_BEGIN_DECLS

#define SKIN_TYPE_BUTTON              (skin_button_get_type ())
#define SKIN_BUTTON(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), SKIN_TYPE_BUTTON, SkinButton)) 
#define SKIN_BUTTON_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), SKIN_TYPE_BUTTON, SkinButtonClass))
#define SKIN_IS_BUTTON(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SKIN_TYPE_BUTTON))
#define SKIN_IS_BUTTON_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), SKIN_TYPE_BUTTON))
#define SKIN_BUTTON_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), SKIN_TYPE_BUTTON, SkinButtonClass))


typedef struct _SkinButton             SkinButton;
typedef struct _SkinButtonClass        SkinButtonClass;
typedef struct _SkinButtonPrivate        SkinButtonPrivate;

struct _SkinButton
{
	GnomeCanvasPixbuf parent;
	SkinButtonPrivate *priv;
};

struct _SkinButtonClass
{
	GnomeCanvasPixbufClass parent_class;
	void (*clicked) (SkinButton *skin_button); 
};

GType      skin_button_get_type           (void) G_GNUC_CONST;

SkinButton* skin_button_new(GnomeCanvasGroup *root, GdkPixbuf *pixbuf, gdouble x, gdouble y);

void skin_button_show(SkinButton *button);
void skin_button_hide(SkinButton *button);

G_END_DECLS

#endif /*__SKIN_BUTTON_H__ */
