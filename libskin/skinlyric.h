/* vi: set sw=4 ts=4: */
/*
 * skinlyric.h
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

#ifndef __SKINLYRIC_H__
#define __SKINLYRIC_H__  1

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS
#define SKIN_TYPE_LYRIC (skin_lyric_get_type ())
#define SKIN_LYRIC(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SKIN_TYPE_LYRIC, SkinLyric))
#define SKIN_LYRIC_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SKIN_TYPE_LYRIC, SkinLyricClass))
#define SKIN_IS_LYRIC(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SKIN_TYPE_LYRIC))
#define SKIN_IS_LYRIC_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SKIN_TYPE_LYRIC))
#define SKIN_LYRIC_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SKIN_TYPE_LYRIC, SkinLyricClass))

typedef struct _SkinLyricPrivate SkinLyricPrivate;

typedef struct 
{ 
	GtkLayout parent;

	SkinLyricPrivate *priv;
} SkinLyric;

typedef struct
{
	GtkLayoutClass parent;
} SkinLyricClass;

GType skin_lyric_get_type();

SkinLyric *skin_lyric_new();

void skin_lyric_get_size(SkinLyric *lyric, gint *width, gint *height);
void skin_lyric_set_size(SkinLyric *lyric, gint width, gint height);
gboolean skin_lyric_add_file(SkinLyric *lyric, const gchar *file);
void skin_lyric_set_current_second(SkinLyric *lyric, gint sec);
void skin_lyric_set_bg_color(SkinLyric *lyric, const GdkColor *color);
void skin_lyric_set_text_color(SkinLyric *lyric, const GdkColor *color);
void skin_lyric_set_highlight_color(SkinLyric *lyric, const GdkColor *color);

void skin_lyric_set_color(SkinLyric *lyric, const GdkColor *bg,
										   const GdkColor *text,
										   const GdkColor *hilight);

G_END_DECLS
#endif /*__SKINLYRIC_H__ */
