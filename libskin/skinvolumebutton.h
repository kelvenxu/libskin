/* vi: set sw=4 ts=4: */
/*
 * skinvolumebutton.h
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

#ifndef __SKINVOLUMEBUTTON_H__
#define __SKINVOLUMEBUTTON_H__  1

#include    <glib-object.h>
#include	<glib.h>
#include	<gtk/gtk.h>
#include	<libgnomecanvas/libgnomecanvas.h>

G_BEGIN_DECLS

#define SKIN_TYPE_VOLUME_BUTTON (skin_volume_button_get_type ())
#define SKIN_VOLUME_BUTTON(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SKIN_TYPE_VOLUME_BUTTON, SkinVolumeButton))
#define SKIN_VOLUME_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SKIN_TYPE_VOLUME_BUTTON, SkinVolumeButtonClass))
#define SKIN_IS_VOLUME_BUTTON(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SKIN_TYPE_VOLUME_BUTTON))
#define SKIN_IS_VOLUME_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SKIN_TYPE_VOLUME_BUTTON))
#define SKIN_VOLUME_BUTTON_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SKIN_TYPE_VOLUME_BUTTON, SkinVolumeButtonClass))

typedef struct _SkinVolumeButtonPrivate SkinVolumeButtonPrivate;

typedef struct 
{
	GnomeCanvasRect parent;

	SkinVolumeButtonPrivate *priv;
} SkinVolumeButton;

typedef struct 
{
	GnomeCanvasRectClass parent;
} SkinVolumeButtonClass;


GType skin_volume_button_get_type() G_GNUC_CONST;
SkinVolumeButton * skin_volume_button_new(GnomeCanvasGroup *root, const gchar *first_arg_name, ...);

G_END_DECLS
#endif /*__SKINVOLUMEBUTTON_H__ */
