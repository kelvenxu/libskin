/* vi: set sw=4 ts=4: */
/*
 * skindynamictext.h
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

#ifndef __SKINDYNAMICTEXT_H__
#define __SKINDYNAMICTEXT_H__  1

#include <glib-object.h>
#include <glib.h>
#include <libgnomecanvas/libgnomecanvas.h>

G_BEGIN_DECLS

#define SKIN_TYPE_DYNAMIC_TEXT (skin_dynamic_text_get_type ())
#define SKIN_DYNAMIC_TEXT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SKIN_TYPE_DYNAMIC_TEXT, SkinDynamicText))
#define SKIN_DYNAMIC_TEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SKIN_TYPE_DYNAMIC_TEXT, SkinDynamicTextClass))
#define SKIN_IS_DYNAMIC_TEXT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SKIN_TYPE_DYNAMIC_TEXT))
#define SKIN_IS_DYNAMIC_TEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SKIN_TYPE_DYNAMIC_TEXT))
#define SKIN_DYNAMIC_TEXT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SKIN_TYPE_DYNAMIC_TEXT, SkinDynamicTextClass))

typedef struct _SkinDynamicTextPrivate SkinDynamicTextPrivate;

typedef struct 
{
	GnomeCanvasText parent;

	SkinDynamicTextPrivate *priv;
} SkinDynamicText;

typedef struct 
{
	GnomeCanvasTextClass parent;
	void (*clicked) (SkinDynamicText *dtext); 
} SkinDynamicTextClass;



GType skin_dynamic_text_get_type() G_GNUC_CONST;

SkinDynamicText *skin_dynamic_text_new(GnomeCanvasGroup *root, const gchar *first_arg_name, ...);

void skin_dynamic_text_set_text(SkinDynamicText *dtext, const gchar *text);

G_END_DECLS

#endif /*__SKINDYNAMICTEXT_H__ */
