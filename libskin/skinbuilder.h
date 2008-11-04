/* vi: set sw=4 ts=4: */
/*
 * skinbuilder.h
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

#ifndef __SKINBUILDER_H__
#define __SKINBUILDER_H__  1

#include <glib-object.h>
#include <glib.h>
#include <libskin/skinarchive.h>

G_BEGIN_DECLS

#define SKIN_TYPE_BUILDER              (skin_builder_get_type ())
#define SKIN_BUILDER(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), SKIN_TYPE_BUILDER, SkinBuilder)) 
#define SKIN_BUILDER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), SKIN_TYPE_BUILDER, SkinBuilderClass))
#define SKIN_IS_BUILDER(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SKIN_TYPE_BUILDER))
#define SKIN_IS_BUILDER_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), SKIN_TYPE_BUILDER))
#define SKIN_BUILDER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), SKIN_TYPE_BUILDER, SkinBuilderClass))

typedef struct _SkinBuilder SkinBuilder;
typedef struct _SkinBuilderClass SkinBuilderClass;
typedef struct _SkinBuilderPrivate SkinBuilderPrivate;

struct _SkinBuilder
{
	GObject parent;

	SkinBuilderPrivate *priv;
};

struct _SkinBuilderClass
{
	GObjectClass parent_class;
};

GType skin_builder_get_type(void) G_GNUC_CONST;

SkinBuilder *skin_builder_new();

gint skin_builder_add_from_archive(SkinBuilder *builder, SkinArchive *ar);

GObject *skin_builder_get_object(SkinBuilder *builder, const gchar *name);

G_END_DECLS

#endif /*__SKINBUILDER_H__ */
