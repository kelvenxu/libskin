/* vi: set sw=4 ts=4: */
/*
 * skincheckbutton.h
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

#ifndef __SKINCHECKBUTTON_H__
#define __SKINCHECKBUTTON_H__  1


#include    <glib-object.h>
#include	<glib.h>
#include	<gtk/gtk.h>
#include	<libgnomecanvas/libgnomecanvas.h>

G_BEGIN_DECLS

#define SKIN_TYPE_CHECK_BUTTON              (skin_check_button_get_type ())
#define SKIN_CHECK_BUTTON(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), SKIN_TYPE_CHECK_BUTTON, SkinCheckButton)) 
#define SKIN_CHECK_BUTTON_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), SKIN_TYPE_CHECK_BUTTON, SkinCheckButtonClass))
#define SKIN_IS_CHECK_BUTTON(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SKIN_TYPE_CHECK_BUTTON))
#define SKIN_IS_CHECK_BUTTON_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), SKIN_TYPE_CHECK_BUTTON))
#define SKIN_CHECK_BUTTON_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), SKIN_TYPE_CHECK_BUTTON, SkinCheckButtonClass))

typedef struct _SkinCheckButton             SkinCheckButton;
typedef struct _SkinCheckButtonClass        SkinCheckButtonClass;
typedef struct _SkinCheckButtonPrivate        SkinCheckButtonPrivate;

/* 这样的button只有一个图片，靠位置的微移来响应用户的行为
 * 主要用在如播放列表上方的工具条上 */
struct _SkinCheckButton
{
	GnomeCanvasPixbuf parent;
	SkinCheckButtonPrivate *priv;
};

struct _SkinCheckButtonClass
{
	GnomeCanvasPixbufClass parent_class;
	void (*clicked) (SkinCheckButton *skin_check_button); 
	void (*activated) (SkinCheckButton *skin_check_button); 
};

GType      skin_check_button_get_type           (void) G_GNUC_CONST;

SkinCheckButton* skin_check_button_new(GnomeCanvasGroup *root, GdkPixbuf *pixbuf, gdouble x, gdouble y);

void skin_check_button_set_active(SkinCheckButton *button, gboolean is_active);
gboolean skin_check_button_get_active(SkinCheckButton *button);
void skin_check_button_show(SkinCheckButton *button);
void skin_check_button_hide(SkinCheckButton *button);

G_END_DECLS

#endif /*__SKINCHECKBUTTON_H__ */
