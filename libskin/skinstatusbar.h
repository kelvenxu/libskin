/* vi: set sw=4 ts=4: */
/*
 * skinstatusbar.h
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

#ifndef __SKINSTATUSBAR_H__
#define __SKINSTATUSBAR_H__  1

#include <glib-object.h>
#include <libgnomecanvas/libgnomecanvas.h>

G_BEGIN_DECLS

#define SKIN_TYPE_STATUS_BAR (skin_status_bar_get_type ())
#define SKIN_STATUS_BAR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SKIN_TYPE_STATUS_BAR, SkinStatusBar))
#define SKIN_STATUS_BAR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SKIN_TYPE_STATUS_BAR, SkinStatusBarClass))
#define SKIN_IS_STATUS_BAR(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SKIN_TYPE_STATUS_BAR))
#define SKIN_IS_STATUS_BAR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SKIN_TYPE_STATUS_BAR))
#define SKIN_STATUS_BAR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SKIN_TYPE_STATUS_BAR, SkinStatusBarClass))

typedef struct _SkinStatusBarPrivate SkinStatusBarPrivate;

typedef struct 
{
	GnomeCanvasText parent;

	SkinStatusBarPrivate *priv;
} SkinStatusBar;

typedef struct
{
	GnomeCanvasTextClass parent;
} SkinStatusBarClass;

GType skin_status_bar_get_type();

SkinStatusBar *skin_status_bar_new(GnomeCanvasGroup *root, const gchar *first_arg_name, ...);
void skin_status_bar_set_text(SkinStatusBar *dtext, const gchar *text);

G_END_DECLS

#endif /*__SKINSTATUSBAR_H__ */
