
/* vi: set sw=4 ts=4: */
/*
 * skindigitaltime.h
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

#ifndef __SKIN_DIGITAL_TIME_H__
#define __SKIN_DIGITAL_TIME_H__  1


#include    <glib-object.h>
#include	<glib.h>
#include	<gtk/gtk.h>
#include	<libgnomecanvas/libgnomecanvas.h>

G_BEGIN_DECLS

#define DIGITAL_TIME_SEPARATOR 10
#define DIGITAL_TIME_NONE 11

#define SKIN_TYPE_DIGITAL_TIME              (skin_digital_time_get_type ())
#define SKIN_DIGITAL_TIME(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), SKIN_TYPE_DIGITAL_TIME, SkinDigitalTime)) 
#define SKIN_DIGITAL_TIME_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), SKIN_TYPE_DIGITAL_TIME, SkinDigitalTimeClass))
#define SKIN_IS_DIGITAL_TIME(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SKIN_TYPE_DIGITAL_TIME))
#define SKIN_IS_DIGITAL_TIME_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), SKIN_TYPE_DIGITAL_TIME))
#define SKIN_DIGITAL_TIME_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), SKIN_TYPE_DIGITAL_TIME, SkinDigitalTimeClass))


typedef struct _SkinDigitalTime             SkinDigitalTime;
typedef struct _SkinDigitalTimeClass        SkinDigitalTimeClass;
typedef struct _SkinDigitalTimePrivate        SkinDigitalTimePrivate;

struct _SkinDigitalTime
{
	GnomeCanvasPixbuf parent;
	SkinDigitalTimePrivate *priv;
};

struct _SkinDigitalTimeClass
{
	GnomeCanvasPixbufClass parent_class;
};

GType      skin_digital_time_get_type           (void) G_GNUC_CONST;

SkinDigitalTime* skin_digital_time_new(GnomeCanvasGroup *root, GdkPixbuf *pixbuf, gdouble x, gdouble y);

void skin_digital_time_show(SkinDigitalTime *digital_time);
void skin_digital_time_hide(SkinDigitalTime *digital_time);

void skin_digital_time_set_value(SkinDigitalTime *digital_time, int value);

G_END_DECLS

#endif /*__SKIN_DIGITAL_TIME_H__ */
