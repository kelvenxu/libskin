/* vi: set sw=4 ts=4: */
/*
 * test_skin_window.c
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

#include <libskin/skinwindow.h>
#include <libskin/skinbutton.h>
#include <libskin/skinvolumebutton.h>
#include <libskin/skindigitaltime.h>

gboolean cb_skin_window_right_button_press(SkinWindow* skin_window, 
				gpointer user_data)
{
	GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("tests/player_skin2.png", NULL);
	skin_window_move(skin_window, 20, 20);
	skin_window_set_image(skin_window, pixbuf);
	return FALSE;
}

gboolean cb_destroy_event()
{
	g_print("test: destroy_event\n");
	gtk_main_quit();
	return FALSE;
}

gboolean cb_clicked_event(SkinButton* button, SkinDigitalTime *time)
{
	static int i = 0;
	g_print("test: SkinButton 'clicked'_event\n");

	skin_button_set_sensitive(button, FALSE);
	skin_digital_time_set_value(time, i);

	++i;
	return FALSE;
}

void cb_skin_window_move_event()
{
	g_print("skin window moved\n");
}


void create_player_window()
{
}

void create_playlist_window()
{
}

void create_lyric_window()
{
}

void create_equalizer_window()
{
}

int main()
{
	SkinWindow* skin_window;
	printf("This program tests libskin skin_window.\n");
	gtk_init(NULL, NULL);

	//skin_window = skin_window_new();
	GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("tests/player_skin.png", NULL);
	//GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("tests/logo.svg", NULL);
	if(!pixbuf)
	{
		return -1;
	}

	skin_window = skin_window_new(pixbuf);

	skin_window_set_resizeable(skin_window, TRUE);
	g_signal_connect(G_OBJECT(skin_window), 
			"right-button-press", 
			G_CALLBACK(cb_skin_window_right_button_press), 
			NULL);

	g_signal_connect(G_OBJECT(skin_window), 
			"move-event", 
			G_CALLBACK(cb_skin_window_move_event), 
			NULL);
	g_signal_connect(G_OBJECT(skin_window), 
			"delete-event", 
			G_CALLBACK(cb_destroy_event), 
		NULL);
	
	//skin_window->test_string = g_strdup("Hello, gobject!\n");

	GdkPixbuf *pb = gdk_pixbuf_new_from_file("tests/number.png", NULL);

	GnomeCanvasGroup *root = skin_window_get_canvas_root(skin_window);
	SkinDigitalTime *time_label = skin_digital_time_new(root, pb, 50, 50);
	//skin_digital_time_show(time_label);

	pb = gdk_pixbuf_new_from_file("tests/play.png", NULL);

	SkinButton *button = skin_button_new(root, pb, 140, 140);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(cb_clicked_event), time_label);

	pb = gdk_pixbuf_new_from_file("tests/volume_fill.png", NULL);
	SkinVolumeButton *volume = skin_volume_button_new(root, "fill_pixbuf", pb, NULL);

	g_object_set(G_OBJECT(volume), 
			"fill-pixbuf", pb, 
			"x", 200.0,
			"y", 200.0,
			"width", 50.0,
			"height", 50.0,
			NULL);
	skin_button_show(button);
	skin_window_show(skin_window);
	gtk_main();
	
	return 0;
}
