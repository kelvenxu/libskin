/* vi: set sw=4 ts=4: */
/*
 * test-skin-builder.c
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

#include <gtk/gtk.h>
#include <glib.h>

#include <libskin/skinwindow.h>
#include <libskin/skinbutton.h>
#include <libskin/skinvolumebutton.h>
#include <libskin/skindigitaltime.h>
#include <libskin/skinstatusbar.h>
#include <libskin/skinarchive.h>
#include <libskin/skinbuilder.h>

static gboolean quit(SkinWindow *window, gpointer user_data)
{
	gtk_main_quit();
	return FALSE;
}
static gboolean play_action(SkinButton *button, gpointer user_data)
{
	g_print("play...\n");
	return FALSE;
}

int main(int argc, char *argv[])
{
	SkinWindow *player_window;
	SkinWindow *equalizer_window;
	SkinWindow *lyric_window;
	SkinWindow *playlist_window;
	SkinWindow *mini_window;
	SkinButton *play;

	gtk_init(NULL, NULL);

	SkinArchive *ar = skin_archive_new();
	if(!SKIN_IS_ARCHIVE(ar)) return -1;

	gboolean re = skin_archive_load(ar, "tests/tt.zip");

	if(!re) return -1;

	SkinBuilder *builder = skin_builder_new();
	g_return_val_if_fail(SKIN_IS_BUILDER(builder), -1);

	skin_builder_add_from_archive(builder, ar);

	player_window = SKIN_WINDOW(skin_builder_get_object(builder, "player-window"));
	play = SKIN_BUTTON(skin_builder_get_object(builder, "player-play"));
	g_return_val_if_fail(SKIN_IS_WINDOW(player_window), -1);

	g_signal_connect(G_OBJECT(player_window), "delete-event", G_CALLBACK(quit), NULL);
	g_signal_connect(G_OBJECT(play), "clicked", G_CALLBACK(play_action), NULL);

	equalizer_window = SKIN_WINDOW(skin_builder_get_object(builder, "equalizer-window"));
	lyric_window = SKIN_WINDOW(skin_builder_get_object(builder, "lyric-window"));
	playlist_window = SKIN_WINDOW(skin_builder_get_object(builder, "playlist-window"));
	mini_window = SKIN_WINDOW(skin_builder_get_object(builder, "mini-window"));

	SkinStatusBar *statusbar = (SkinStatusBar*)skin_builder_get_object(builder, "player-statusbar");
	skin_status_bar_set_text(statusbar, "Stop!");

	skin_window_show(player_window);
	skin_window_show(equalizer_window);
	skin_window_show(lyric_window);
	skin_window_show(playlist_window);
	skin_window_show(mini_window);

	gtk_main();

	return 0;
}

