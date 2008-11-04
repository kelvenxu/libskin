/* vi: set sw=4 ts=4: */
/*
 * skintogglebutton.h
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

#ifndef __SKINTOGGLEBUTTON_H__
#define __SKINTOGGLEBUTTON_H__  1

#include    <glib-object.h>
#include	<glib.h>
#include	<gtk/gtk.h>
#include	<libgnomecanvas/libgnomecanvas.h>

G_BEGIN_DECLS

#define SKIN_TYPE_TOGGLE_BUTTON              (skin_toggle_button_get_type ())
#define SKIN_TOGGLE_BUTTON(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), SKIN_TYPE_TOGGLE_BUTTON, SkinToggleButton)) 
#define SKIN_TOGGLE_BUTTON_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), SKIN_TYPE_TOGGLE_BUTTON, SkinToggleButtonClass))
#define SKIN_IS_TOGGLE_BUTTON(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SKIN_TYPE_TOGGLE_BUTTON))
#define SKIN_IS_TOGGLE_BUTTON_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), SKIN_TYPE_TOGGLE_BUTTON))
#define SKIN_TOGGLE_BUTTON_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), SKIN_TYPE_TOGGLE_BUTTON, SkinToggleButtonClass))

typedef struct _SkinToggleButton             SkinToggleButton;
typedef struct _SkinToggleButtonClass        SkinToggleButtonClass;
typedef struct _SkinToggleButtonPrivate        SkinToggleButtonPrivate;

/* 这样的button只有一个图片，靠位置的微移来响应用户的行为
 * 主要用在如播放列表上方的工具条上 */
struct _SkinToggleButton
{
	GnomeCanvasPixbuf parent;
	SkinToggleButtonPrivate *priv;
};

struct _SkinToggleButtonClass
{
	GnomeCanvasPixbufClass parent_class;
	void (*clicked) (SkinToggleButton *skin_toggle_button); 
};

GType      skin_toggle_button_get_type           (void) G_GNUC_CONST;

SkinToggleButton* skin_toggle_button_new(GnomeCanvasGroup *root, GdkPixbuf *pixbuf, gdouble x, gdouble y);

void skin_toggle_button_show(SkinToggleButton *button);
void skin_toggle_button_hide(SkinToggleButton *button);

G_END_DECLS

#endif /*__SKINTOGGLEBUTTON_H__ */

