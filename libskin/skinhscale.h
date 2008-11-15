/* vi: set sw=4 ts=4: */
/*
 * skinhscale.h
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

#ifndef __SKINHSCALE_H__
#define __SKINHSCALE_H__  1

#include <glib-object.h>
#include <libgnomecanvas/libgnomecanvas.h>

G_BEGIN_DECLS

typedef struct _SkinHScalePrivate SkinHScalePrivate;

typedef struct 
{
	GnomeCanvasRect parent;

	SkinHScalePrivate *priv;
} SkinHScale;

typedef struct 
{
	GnomeCanvasRectClass parent;
	void (*value_changed) (SkinHScale *hscale);
} SkinHScaleClass;

#define SKIN_TYPE_HSCALE (skin_hscale_get_type ())
#define SKIN_HSCALE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SKIN_TYPE_HSCALE, SkinHScale))
#define SKIN_HSCALE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SKIN_TYPE_HSCALE, SkinHScaleClass))
#define SKIN_IS_HSCALE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SKIN_TYPE_HSCALE))
#define SKIN_IS_HSCALE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SKIN_TYPE_HSCALE))
#define SKIN_HSCALE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SKIN_TYPE_HSCALE, SkinHScaleClass))


GType skin_hscale_get_type();

SkinHScale * skin_hscale_new(GnomeCanvasGroup *root, const char *first_arg_name, ...);
void skin_hscale_set_range(SkinHScale *hscale, gdouble min, gdouble max);
void skin_hscale_set_value(SkinHScale *hscale, gdouble value);
void skin_hscale_set_range_and_value(SkinHScale *hscale, gdouble min, gdouble max, gdouble value);

G_END_DECLS

#endif /*__SKINHSCALE_H__ */
