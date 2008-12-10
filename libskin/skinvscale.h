/* vi: set sw=4 ts=4: */
/*
 * skinhscale.h
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

#ifndef __SKINVSCALE_H__
#define __SKINVSCALE_H__  1

#include <glib-object.h>
#include <libgnomecanvas/libgnomecanvas.h>

G_BEGIN_DECLS

typedef struct _SkinVScalePrivate SkinVScalePrivate;

typedef struct 
{
	GnomeCanvasRect parent;

	SkinVScalePrivate *priv;
} SkinVScale;

typedef struct 
{
	GnomeCanvasRectClass parent;
	void (*value_changed) (SkinVScale *hscale);
} SkinVScaleClass;

#define SKIN_TYPE_VSCALE (skin_vscale_get_type ())
#define SKIN_VSCALE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SKIN_TYPE_VSCALE, SkinVScale))
#define SKIN_VSCALE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SKIN_TYPE_VSCALE, SkinVScaleClass))
#define SKIN_IS_VSCALE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SKIN_TYPE_VSCALE))
#define SKIN_IS_VSCALE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SKIN_TYPE_VSCALE))
#define SKIN_VSCALE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SKIN_TYPE_VSCALE, SkinVScaleClass))


GType skin_vscale_get_type();

SkinVScale * skin_vscale_new(GnomeCanvasGroup *root, const char *first_arg_name, ...);
void skin_vscale_set_value(SkinVScale *hscale, gdouble value);
gdouble skin_vscale_get_value(SkinVScale *vscale);

G_END_DECLS

#endif /*__SKINVSCALE_H__ */
