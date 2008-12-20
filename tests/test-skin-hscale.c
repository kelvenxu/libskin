/* vi: set sw=4 ts=4: */
/*
 * test-skin-hscale.c
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

#include <gtk/gtk.h>
#include <glib.h>
#include <libgnomecanvas/libgnomecanvas.h>
#include <libskin/skin.h>

gboolean cb_skin_window_right_button_press(SkinWindow* skin_window, 
				SkinHScale *hscale)
{
	printf("sdfasf\n");
	GdkPixbuf* fill = gdk_pixbuf_new_from_file("tests/progress_fill.png", NULL);
	GdkPixbuf* thumb = gdk_pixbuf_new_from_file("tests/progress_thumb.png", NULL);
	g_object_set(G_OBJECT(hscale), 
			"x1", 132.0,
			"y1", 207.0,
			"x2", 291.0,
			"y1", 216.0,
			"fill-pixbuf", fill,
			"thumb-pixbuf", thumb,
			"min", 0.0,
			"max", 100.0,
			"value", 80.0,
			NULL);
	return FALSE;
}

int main()
{
	SkinWindow* skin_window;
	SkinHScale *progress;
	GnomeCanvasGroup *root;
	printf("This program tests libskin skin_window.\n");
	gtk_init(NULL, NULL);

	GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("tests/player_skin.png", NULL);
	if(!pixbuf)
	{
		return -1;
	}

	skin_window = skin_window_new(pixbuf);
	root = skin_window_get_canvas_root(skin_window);


	GdkPixbuf* fill = gdk_pixbuf_new_from_file("tests/progress_fill.png", NULL);
	GdkPixbuf* thumb = gdk_pixbuf_new_from_file("tests/progress_thumb.png", NULL);
	progress = skin_hscale_new(root, 
			"x1", 132.0,
			"y1", 207.0,
			"x2", 291.0,
			"y1", 216.0,
			"fill-pixbuf", fill,
			"thumb-pixbuf", thumb,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);

	g_signal_connect(G_OBJECT(skin_window), 
			"right-button-press", 
			G_CALLBACK(cb_skin_window_right_button_press), 
			progress);

	skin_window_show(skin_window);
	gtk_main();
	
	return 0;
}

