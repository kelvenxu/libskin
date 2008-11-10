/* vi: set sw=4 ts=4: */
/*
 * skinbuilder.c
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

/*
 * 想法：
 * 根据皮肤文件SkinArchive生成皮肤窗口，并提供各部件的索引,就象GtkBuilder一样.
 * 将各个部件构造出来并放在相应的位置上，但不设初值，不处理信号
 *
 * GtkBuilder中使用了GHashTable来存储各个object
 */
#include "skinbuilder.h"
#include "skinarchive.h"
#include "skinwindow.h"
#include "skinbutton.h"
#include "skincheckbutton.h"
#include "skintogglebutton.h"
#include "skinvolumebutton.h"
#include "skindynamictext.h"
#include "skinhscale.h"
#include "skinvscale.h"

#define TOOLBAR_ITEMS 7

G_DEFINE_TYPE (SkinBuilder, skin_builder, G_TYPE_OBJECT);

#define SKIN_BUILDER_GET_PRIVATE(o)\
	(G_TYPE_INSTANCE_GET_PRIVATE ((o), SKIN_TYPE_BUILDER, SkinBuilderPrivate))

struct _SkinBuilderPrivate 
{
	int i;
	SkinArchive *ar;
	GHashTable *objects;
};


static void
skin_builder_dispose (SkinBuilder *self)
{
}

static void
skin_builder_finalize (SkinBuilder *self)
{
	SkinBuilderPrivate *priv = self->priv;
	g_hash_table_destroy(priv->objects);
}

static void
skin_builder_init (SkinBuilder *self)
{
	g_print("object init\n");
	SkinBuilderPrivate *priv;
	priv = g_new0(SkinBuilderPrivate, 1);

	self->priv = priv;

	priv->ar = NULL;
	priv->objects = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
	g_print("object init done\n");
}

static void
skin_builder_class_init (SkinBuilderClass *self_class)
{
	g_print("class init\n");
	GObjectClass *object_class = G_OBJECT_CLASS (self_class);

	g_type_class_add_private (self_class, sizeof (SkinBuilderPrivate));
	object_class->dispose = (void (*) (GObject *object)) skin_builder_dispose;
	object_class->finalize = (void (*) (GObject *object)) skin_builder_finalize;
	g_print("class initdone\n");
}

static void
add_object(SkinBuilder *builder, GObject *obj, const char *name)
{
	g_hash_table_insert(builder->priv->objects, g_strdup(name), obj);
}

static void 
create_player_window(SkinBuilder *builder)
{
	PlayerArchive *player;

	g_print("create player window\n");
	g_return_if_fail(SKIN_IS_BUILDER(builder));

	player = builder->priv->ar->player;
	
	SkinWindow *window = skin_window_new(player->window.img);
	add_object(builder, G_OBJECT(window), "player-window");

	GnomeCanvasGroup *root = window->canvas_root;

	SkinButton *pause = skin_button_new(root, 
			player->pause.img, 
			player->pause.x1, player->pause.y1);
	add_object(builder, G_OBJECT(pause), "player-pause");

	SkinButton *play = skin_button_new(root, 
			player->play.img, 
			player->play.x1, player->play.y1);
	add_object(builder, G_OBJECT(play), "player-play");

	SkinButton *stop = skin_button_new(root, 
			player->stop.img, 
			player->stop.x1, player->stop.y1);
	add_object(builder, G_OBJECT(stop), "player-stop");

	SkinButton *prev = skin_button_new(root, 
			player->prev.img, 
			player->prev.x1, player->prev.y1);
	add_object(builder, G_OBJECT(prev), "player-prev");

	SkinButton *next = skin_button_new(root, 
			player->next.img, 
			player->next.x1, player->next.y1);
	add_object(builder, G_OBJECT(next), "player-next");

	SkinButton *exit = skin_button_new(root, 
			player->exit.img, 
			player->exit.x1, player->exit.y1);
	add_object(builder, G_OBJECT(exit), "player-exit");

	SkinButton *minimize = skin_button_new(root, 
			player->minimize.img, 
			player->minimize.x1, player->minimize.y1);
	add_object(builder, G_OBJECT(minimize), "player-minimize");

	SkinButton *minimode = skin_button_new(root, 
			player->minimode.img, 
			player->minimode.x1, player->minimode.y1);
	add_object(builder, G_OBJECT(minimode), "player-minimode");

	SkinButton *open = skin_button_new(root, 
			player->open.img, 
			player->open.x1, player->open.y1);
	add_object(builder, G_OBJECT(open), "player-open");
	
	SkinDynamicText *info = skin_dynamic_text_new(root, 
			"x1", (gdouble)(player->info.x1),
			"y1", (gdouble)(player->info.y1),
			"x2", (gdouble)(player->info.x2),
			"y2", (gdouble)(player->info.y2),
			"color", player->info.color,
			"title", "title --",
			"artist", "artist --",
			"album", "album ==",
			"format", "format --",
			NULL);
	add_object(builder, G_OBJECT(info), "player-info");

	SkinHScale *progressbar = skin_hscale_new(root,
			"x1", (gdouble)(player->progress.x1),
			"y1", (gdouble)(player->progress.y1),
			"x2", (gdouble)(player->progress.x2),
			"y2", (gdouble)(player->progress.y2),
			"fill-pixbuf", player->progress.fill_img,
			"thumb-pixbuf", player->progress.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(progressbar), "player-progressbar");

	SkinCheckButton *playlist = skin_check_button_new(root,
			player->playlist.img,
			player->playlist.x1, player->playlist.y1);
	add_object(builder, G_OBJECT(playlist), "player-playlist");

	SkinCheckButton *lyric = skin_check_button_new(root,
			player->lyric.img,
			player->lyric.x1, player->lyric.y1);
	add_object(builder, G_OBJECT(lyric), "player-lyric");

	SkinCheckButton *equalizer = skin_check_button_new(root,
			player->equalizer.img,
			player->equalizer.x1, player->equalizer.y1);
	add_object(builder, G_OBJECT(equalizer), "player-equalizer");

	SkinHScale *volume = skin_hscale_new(root,
			"x1", (gdouble)(player->volume.x1),
			"y1", (gdouble)(player->volume.y1),
			"x2", (gdouble)(player->volume.x2),
			"y2", (gdouble)(player->volume.y2),
			"fill-pixbuf", player->volume.fill_img,
			"thumb-pixbuf", player->volume.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(volume), "player-volume");

	/*
	SkinVolumeButton *volume = skin_volume_button_new(root, 
			"fill_pixbuf", player->volume.img,
			"bar_pixbuf", player->volume.bar_img,
			"thumb_pixbuf", player->volume.thumb_img,
			"x", player->volume.x1,
			"y", player->volume.y1,
			"width", player->volume.x2 - player->volume.x1,
			"height", player->volume.y2 - player->volume.y1);
	add_object(builder, G_OBJECT(volume), "player-volume-button");
	*/
}

static void 
create_equalizer_window(SkinBuilder *builder)
{
	EqualizerArchive *eq;
	GnomeCanvasGroup *root;
	g_print("create eq window\n");
	g_return_if_fail(SKIN_IS_BUILDER(builder));
	eq = builder->priv->ar->equalizer;

	SkinWindow *window = skin_window_new(eq->window.img);
	add_object(builder, G_OBJECT(window), "equalizer-window");

	root = window->canvas_root;

	SkinButton *close = skin_button_new(root,
			eq->close.img,
			eq->close.x1, eq->close.y1);
	add_object(builder, G_OBJECT(close), "equalizer-close");

	SkinCheckButton *enabled = skin_check_button_new(root, 
			eq->enabled.img,
			eq->enabled.x1, eq->enabled.y1);
	add_object(builder, G_OBJECT(enabled), "equalizer-enabled");

	SkinCheckButton *profile = skin_check_button_new(root, 
			eq->profile.img,
			eq->profile.x1, eq->profile.y1);
	add_object(builder, G_OBJECT(profile), "equalizer-profile");

	SkinCheckButton *reset = skin_check_button_new(root, 
			eq->reset.img,
			eq->reset.x1, eq->reset.y1);
	add_object(builder, G_OBJECT(reset), "equalizer-reset");

	SkinHScale *balance = skin_hscale_new(root,
			"x1", (gdouble)(eq->balance.x1),
			"y1", (gdouble)(eq->balance.y1),
			"x2", (gdouble)(eq->balance.x2),
			"y2", (gdouble)(eq->balance.y2),
			"fill-pixbuf", eq->balance.fill_img,
			"thumb-pixbuf", eq->balance.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(balance), "equalizer-balance");

	SkinHScale *surround = skin_hscale_new(root,
			"x1", (gdouble)(eq->surround.x1),
			"y1", (gdouble)(eq->surround.y1),
			"x2", (gdouble)(eq->surround.x2),
			"y2", (gdouble)(eq->surround.y2),
			"fill-pixbuf", eq->surround.fill_img,
			"thumb-pixbuf", eq->surround.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(surround), "equalizer-surround");

	SkinVScale *preamp = skin_vscale_new(root,
			"x1", (gdouble)(eq->preamp.x1),
			"y1", (gdouble)(eq->preamp.y1),
			"x2", (gdouble)(eq->preamp.x2),
			"y2", (gdouble)(eq->preamp.y2),
			"fill-pixbuf", eq->preamp.fill_img,
			"thumb-pixbuf", eq->preamp.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(preamp), "equalizer-preamp");

	SkinVScale *eqfactor0 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1),
			"y1", (gdouble)(eq->eqfactor.y1),
			"x2", (gdouble)(eq->eqfactor.x2),
			"y2", (gdouble)(eq->eqfactor.y2),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor0), "equalizer-eqfactor0");

	SkinVScale *eqfactor1 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval,
			"y1", (gdouble)(eq->eqfactor.y1),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval,
			"y2", (gdouble)(eq->eqfactor.y2),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor1), "equalizer-eqfactor1");

	SkinVScale *eqfactor2 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval * 2.0,
			"y1", (gdouble)(eq->eqfactor.y1),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval * 2.0,
			"y2", (gdouble)(eq->eqfactor.y2),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor2), "equalizer-eqfactor2");

	SkinVScale *eqfactor3 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval * 3.0,
			"y1", (gdouble)(eq->eqfactor.y1),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval * 3.0,
			"y2", (gdouble)(eq->eqfactor.y2),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor3), "equalizer-eqfactor3");
	SkinVScale *eqfactor4 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval * 4.0,
			"y1", (gdouble)(eq->eqfactor.y1),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval * 4.0,
			"y2", (gdouble)(eq->eqfactor.y2),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor4), "equalizer-eqfactor4");
	SkinVScale *eqfactor5 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval * 5.0,
			"y1", (gdouble)(eq->eqfactor.y1),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval * 5.0,
			"y2", (gdouble)(eq->eqfactor.y2),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor5), "equalizer-eqfactor5");
	SkinVScale *eqfactor6 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval * 6.0,
			"y1", (gdouble)(eq->eqfactor.y1),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval * 6.0,
			"y2", (gdouble)(eq->eqfactor.y2),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor6), "equalizer-eqfactor6");
	SkinVScale *eqfactor7 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval * 7.0,
			"y1", (gdouble)(eq->eqfactor.y1),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval * 7.0,
			"y2", (gdouble)(eq->eqfactor.y2),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor7), "equalizer-eqfactor7");
	SkinVScale *eqfactor8 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval * 8.0,
			"y1", (gdouble)(eq->eqfactor.y1),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval * 8.0,
			"y2", (gdouble)(eq->eqfactor.y2),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor8), "equalizer-eqfactor8");

	SkinVScale *eqfactor9 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval * 9.0,
			"y1", (gdouble)(eq->eqfactor.y1),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval * 9.0,
			"y2", (gdouble)(eq->eqfactor.y2),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor9), "equalizer-eqfactor9");
}

static void
create_lyric_window(SkinBuilder *builder)
{
	LyricArchive *lyric;
	GnomeCanvasGroup *root;

	g_return_if_fail(SKIN_IS_BUILDER(builder));

	lyric = builder->priv->ar->lyric;

	SkinWindow *window = skin_window_new(lyric->window.img);
	add_object(builder, G_OBJECT(window), "lyric-window");

	root = window->canvas_root;

	SkinButton *close = skin_button_new(root,
			lyric->close.img,
			lyric->close.x1, lyric->close.y1);
	add_object(builder, G_OBJECT(close), "lyric-close");

	SkinCheckButton *ontop = skin_check_button_new(root,
			lyric->ontop.img,
			lyric->ontop.x1, lyric->ontop.y1);
	add_object(builder, G_OBJECT(ontop), "lyric-ontop");
}

static void
create_playlist_window(SkinBuilder *builder)
{
	PlaylistArchive *pl;
	GdkPixbuf *pixbuf;
	GnomeCanvasGroup *root;
	gdouble width, height;
	gdouble x;
	gint i;
	gchar *tb_items[TOOLBAR_ITEMS] = {
		"toolbar-add",
		"toolbar-remove",
		"toolbar-list",
		"toolbar-sort",
		"toolbar-search",
		"toolbar-edit",
		"toolbar-mode"
	};

	g_return_if_fail(SKIN_IS_BUILDER(builder));

	pl = builder->priv->ar->playlist;

	SkinWindow *window = skin_window_new(pl->window.img);
	add_object(builder, G_OBJECT(window), "playlist-window");

	root = window->canvas_root;

	SkinButton *close = skin_button_new(root,
			pl->close.img,
			pl->close.x1, pl->close.y1);
	add_object(builder, G_OBJECT(close), "playlist-close");

	width = (gdouble)gdk_pixbuf_get_width(pl->toolbar.img);
	height = (gdouble)gdk_pixbuf_get_height(pl->toolbar.img);
	width = width / TOOLBAR_ITEMS;
	x = pl->toolbar.x1;

	SkinToggleButton *tb;
	for(i = 0; i < TOOLBAR_ITEMS; ++i)
	{
		pixbuf = gdk_pixbuf_new_subpixbuf(pl->toolbar.img,
				i * width, 0, width, height);

		x = pl->toolbar.x1 + i * width;
		tb = skin_toggle_button_new(root, pixbuf, x, pl->toolbar.y1);
		add_object(builder, G_OBJECT(close), tb_items[i]);
	}
}

static void
create_mini_window(SkinBuilder *builder)
{
	MiniArchive *mini;
	GnomeCanvasGroup *root;

	g_return_if_fail(SKIN_IS_BUILDER(builder));

	mini = builder->priv->ar->mini;

	SkinWindow *window = skin_window_new(mini->window.img);
	add_object(builder, G_OBJECT(window), "mini-window");

	root = window->canvas_root;

	SkinButton *pause = skin_button_new(root,
			mini->pause.img, 
			mini->pause.x1, mini->pause.y1);
	add_object(builder, G_OBJECT(pause), "mini-pause");

	SkinButton *play = skin_button_new(root,
			mini->play.img, 
			mini->play.x1, mini->play.y1);
	add_object(builder, G_OBJECT(play), "mini-play");

	SkinButton *prev = skin_button_new(root,
			mini->prev.img, 
			mini->prev.x1, mini->prev.y1);
	add_object(builder, G_OBJECT(prev), "mini-prev");

	SkinButton *next = skin_button_new(root,
			mini->next.img, 
			mini->next.x1, mini->next.y1);
	add_object(builder, G_OBJECT(next), "mini-next");

	SkinButton *stop = skin_button_new(root,
			mini->stop.img, 
			mini->stop.x1, mini->stop.y1);
	add_object(builder, G_OBJECT(stop), "mini-stop");

	SkinCheckButton *lyric = skin_check_button_new(root,
			mini->lyric.img, 
			mini->lyric.x1, mini->lyric.y1);
	add_object(builder, G_OBJECT(lyric), "mini-lyric");

	SkinButton *icon = skin_button_new(root,
			mini->icon.img, 
			mini->icon.x1, mini->icon.y1);
	add_object(builder, G_OBJECT(icon), "mini-icon");

	SkinButton *minimize = skin_button_new(root,
			mini->minimize.img, 
			mini->minimize.x1, mini->minimize.y1);
	add_object(builder, G_OBJECT(minimize), "mini-minimize");

	SkinButton *minimode = skin_button_new(root,
			mini->minimode.img, 
			mini->minimode.x1, mini->minimode.y1);
	add_object(builder, G_OBJECT(minimode), "mini-minimode");

	SkinButton *exit = skin_button_new(root,
			mini->exit.img, 
			mini->exit.x1, mini->exit.y1);
	add_object(builder, G_OBJECT(exit), "mini-exit");
}

SkinBuilder *skin_builder_new()
{
	SkinBuilder *builder;
	g_print("new builder\n");
	builder = SKIN_BUILDER(g_object_new(SKIN_TYPE_BUILDER, NULL));
	g_print("new builder done\n");
	return builder; 
}

gint skin_builder_add_from_archive(SkinBuilder *builder, SkinArchive *ar)
{
	g_print("add archive\n");
	g_return_val_if_fail(SKIN_IS_BUILDER(builder), -1);
	g_return_val_if_fail(SKIN_IS_ARCHIVE(ar), -1);

	g_print("add archive\n");
	builder->priv->ar = ar;
	create_player_window(builder);
	create_equalizer_window(builder);
	create_lyric_window(builder);
	create_playlist_window(builder);
	create_mini_window(builder);
	return 0;
}

GObject *skin_builder_get_object(SkinBuilder *builder, const gchar *name)
{
	g_return_val_if_fail(SKIN_BUILDER(builder), NULL);
	g_return_val_if_fail(name != NULL, NULL);

	return g_hash_table_lookup(builder->priv->objects, name);
}
