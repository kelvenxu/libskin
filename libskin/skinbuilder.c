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
#include <glib/gi18n.h>
#include "skinarchive.h"
#include "skinwindow.h"
#include "skinbutton.h"
#include "skincheckbutton.h"
#include "skintogglebutton.h"
#include "skinvolumebutton.h"
#include "skindigitaltime.h"
#include "skindynamictext.h"
#include "skinstatusbar.h"
#include "skinhscale.h"
#include "skinscrollbar.h"
#include "skinvscale.h"
#include "skinlyric.h"

#define TOOLBAR_ITEMS 7

G_DEFINE_TYPE (SkinBuilder, skin_builder, G_TYPE_OBJECT);

#define SKIN_BUILDER_GET_PRIVATE(o)\
	(G_TYPE_INSTANCE_GET_PRIVATE ((o), SKIN_TYPE_BUILDER, SkinBuilderPrivate))

struct _SkinBuilderPrivate 
{
	//int i;
	SkinArchive *ar;
	GHashTable *objects;

	gchar *skinfile;
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
	SkinBuilderPrivate *priv;
	priv = g_new0(SkinBuilderPrivate, 1);

	self->priv = priv;

	priv->ar = NULL;
	priv->objects = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
}

static void
skin_builder_class_init (SkinBuilderClass *self_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (self_class);

	g_type_class_add_private (self_class, sizeof (SkinBuilderPrivate));
	object_class->dispose = (void (*) (GObject *object)) skin_builder_dispose;
	object_class->finalize = (void (*) (GObject *object)) skin_builder_finalize;
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
	GnomeCanvasGroup *root;

	g_return_if_fail(SKIN_IS_BUILDER(builder));

	player = builder->priv->ar->player;
	
	SkinWindow *window = skin_window_new(player->window.img);
	add_object(builder, G_OBJECT(window), "player-window");
	skin_window_move(window, player->window.x1, player->window.y1);

	root = skin_window_get_canvas_root(window);

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
			"font", player->info.font,
			"size-points", (gdouble)player->info.font_size,
			"size-set", TRUE,
			"title", "Linux Music Player - Little Pudding",
			"artist", "Kelven Xu",
			"album", "kelvenxu@gmail.com",
			"format", "www.lmplayer.org",
			NULL);
	add_object(builder, G_OBJECT(info), "player-info");

	SkinStatusBar *statusbar = skin_status_bar_new(root, 
			"x1", (gdouble)(player->status.x1),
			"y1", (gdouble)(player->status.y1),
			"x2", (gdouble)(player->status.x2),
			"y2", (gdouble)(player->status.y2),
			"color", player->status.color,
			"align", player->status.align,
			"text", _("Stop"),
			NULL);
	add_object(builder, G_OBJECT(statusbar), "player-statusbar");

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

	SkinDigitalTime *led = skin_digital_time_new(root, player->led.img,
			(gdouble)(player->led.x1),
			(gdouble)(player->led.y1));
	add_object(builder, G_OBJECT(led), "player-led");

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

	SkinCheckButton *mute = skin_check_button_new(root,
			player->mute.img,
			player->mute.x1, player->mute.y1);
	add_object(builder, G_OBJECT(mute), "player-mute");

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

	GnomeCanvasItem *visualbox = gnome_canvas_item_new(root,
			gnome_canvas_widget_get_type(),
			"x", (gdouble)player->visual.x1,
			"y", (gdouble)player->visual.y1,
			"width", (gdouble)(player->visual.x2 - player->visual.x1),
			"height", (gdouble)(player->visual.y2 - player->visual.y1),
			"anchor", GTK_ANCHOR_NORTH_WEST,
			"size-pixels", TRUE,
			NULL);
	add_object(builder, G_OBJECT(visualbox), "player-visualbox");

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
	skin_window_move(window, eq->window.x1, eq->window.y1);

	root = skin_window_get_canvas_root(window);

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

	// SkinVScale的(x1,y1)在左下角，(x2,y2)在右上角
	SkinVScale *preamp = skin_vscale_new(root,
			"x1", (gdouble)(eq->preamp.x1),
			"y1", (gdouble)(eq->preamp.y2),
			"x2", (gdouble)(eq->preamp.x2),
			"y2", (gdouble)(eq->preamp.y1),
			"fill-pixbuf", eq->preamp.fill_img,
			"thumb-pixbuf", eq->preamp.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(preamp), "equalizer-preamp");

	// SkinVScale的(x1,y1)在左下角，(x2,y2)在右上角
	SkinVScale *eqfactor0 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1),
			"y1", (gdouble)(eq->eqfactor.y2),
			"x2", (gdouble)(eq->eqfactor.x2),
			"y2", (gdouble)(eq->eqfactor.y1),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor0), "equalizer-eqfactor0");

	// SkinVScale的(x1,y1)在左下角，(x2,y2)在右上角
	SkinVScale *eqfactor1 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval,
			"y1", (gdouble)(eq->eqfactor.y2),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval,
			"y2", (gdouble)(eq->eqfactor.y1),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor1), "equalizer-eqfactor1");

	SkinVScale *eqfactor2 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval * 2.0,
			"y1", (gdouble)(eq->eqfactor.y2),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval * 2.0,
			"y2", (gdouble)(eq->eqfactor.y1),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor2), "equalizer-eqfactor2");

	SkinVScale *eqfactor3 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval * 3.0,
			"y1", (gdouble)(eq->eqfactor.y2),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval * 3.0,
			"y2", (gdouble)(eq->eqfactor.y1),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor3), "equalizer-eqfactor3");
	SkinVScale *eqfactor4 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval * 4.0,
			"y1", (gdouble)(eq->eqfactor.y2),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval * 4.0,
			"y2", (gdouble)(eq->eqfactor.y1),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor4), "equalizer-eqfactor4");
	SkinVScale *eqfactor5 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval * 5.0,
			"y1", (gdouble)(eq->eqfactor.y2),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval * 5.0,
			"y2", (gdouble)(eq->eqfactor.y1),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor5), "equalizer-eqfactor5");
	SkinVScale *eqfactor6 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval * 6.0,
			"y1", (gdouble)(eq->eqfactor.y2),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval * 6.0,
			"y2", (gdouble)(eq->eqfactor.y1),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor6), "equalizer-eqfactor6");
	SkinVScale *eqfactor7 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval * 7.0,
			"y1", (gdouble)(eq->eqfactor.y2),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval * 7.0,
			"y2", (gdouble)(eq->eqfactor.y1),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor7), "equalizer-eqfactor7");
	SkinVScale *eqfactor8 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval * 8.0,
			"y1", (gdouble)(eq->eqfactor.y2),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval * 8.0,
			"y2", (gdouble)(eq->eqfactor.y1),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor8), "equalizer-eqfactor8");

	SkinVScale *eqfactor9 = skin_vscale_new(root,
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval * 9.0,
			"y1", (gdouble)(eq->eqfactor.y2),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval * 9.0,
			"y2", (gdouble)(eq->eqfactor.y1),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			"min", 0.0,
			"max", 100.0,
			"value", 50.0,
			NULL);
	add_object(builder, G_OBJECT(eqfactor9), "equalizer-eqfactor9");
}

static void
cb_lyric_change_size(SkinWindow *window, gint rx, gint ry, SkinBuilder *builder)
{
	SkinButton *button;
	GnomeCanvasItem *item;
	gdouble width, height;

	button = (SkinButton*)skin_builder_get_object(builder, "lyric-close");
	if(button)
		gnome_canvas_item_move(GNOME_CANVAS_ITEM(button), rx, 0);
	button = (SkinButton*)skin_builder_get_object(builder, "lyric-ontop");
	if(button)
		gnome_canvas_item_move(GNOME_CANVAS_ITEM(button), rx, 0);

    item = (GnomeCanvasItem *)skin_builder_get_object(builder, "lyric-lyricbox");
	if(item)
	{
		g_object_get(G_OBJECT(item), 
				"width", &width,
				"height", &height,
				NULL);
		printf("width: %f height: %f rx: %d ry: %d\n", width, height, rx, ry);
		gnome_canvas_item_set(item, 
				"width", width + (gdouble)rx,
				"height", height + (gdouble)ry,
				NULL);

		SkinLyric *lyricview = (SkinLyric *)skin_builder_get_object(builder, "lyric-lyricview");
		skin_lyric_set_size(lyricview, (gint)(width + rx), (gint)(height + ry));
	}
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
	skin_window_move(window, lyric->window.x1, lyric->window.y1);

	root = skin_window_get_canvas_root(window);

	SkinButton *close = skin_button_new(root,
			lyric->close.img,
			lyric->close.x1, lyric->close.y1);
	add_object(builder, G_OBJECT(close), "lyric-close");

	SkinCheckButton *ontop = skin_check_button_new(root,
			lyric->ontop.img,
			lyric->ontop.x1, lyric->ontop.y1);
	add_object(builder, G_OBJECT(ontop), "lyric-ontop");

	GnomeCanvasItem *lyricbox = gnome_canvas_item_new(root,
			gnome_canvas_widget_get_type(),
			"x", (gdouble)lyric->lyric.x1,
			"y", (gdouble)lyric->lyric.y1,
			"width", (gdouble)(lyric->lyric.x2 - lyric->lyric.x1),
			"height", (gdouble)(lyric->lyric.y2 - lyric->lyric.y1),
			"anchor", GTK_ANCHOR_NORTH_WEST,
			"size-pixels", TRUE,
			NULL);
	add_object(builder, G_OBJECT(lyricbox), "lyric-lyricbox");

	SkinLyric *lyricview = skin_lyric_new();
	skin_lyric_set_size(lyricview, 
			lyric->lyric.x2 - lyric->lyric.x1,
			lyric->lyric.y2 - lyric->lyric.y1);
	skin_lyric_set_bg_color(lyricview, &lyric->attr.color_bg);
	skin_lyric_set_text_color(lyricview, &lyric->attr.color_text);
	skin_lyric_set_highlight_color(lyricview, &lyric->attr.color_hilight);

	gnome_canvas_item_set(lyricbox,
			"widget", GTK_WIDGET(lyricview),
			NULL);

	add_object(builder, G_OBJECT(lyricview), "lyric-lyricview");

	g_signal_connect(G_OBJECT(window), "change-size", 
			G_CALLBACK(cb_lyric_change_size), builder);
	printf("item_set lyricview\n");
}

static void
cb_playlist_change_size(SkinWindow *window, gint rx, gint ry, SkinBuilder *builder)
{
	SkinButton *button;
	button = (SkinButton *)skin_builder_get_object(builder, "playlist-close");
	if(button)
		gnome_canvas_item_move(GNOME_CANVAS_ITEM(button), rx, 0);

	GnomeCanvasItem *item;
	item = (GnomeCanvasItem*)skin_builder_get_object(builder, "playlist-playlistbox");
	if(item)
	{
		gdouble width, height;
		g_object_get(G_OBJECT(item),
				"width", &width,
				"height", &height,
				NULL);
		gnome_canvas_item_set(item,
				"width", width + rx,
				"height", height + ry,
				NULL);
	}

	SkinScrollBar *scrollbar;

	scrollbar = (SkinScrollBar*)skin_builder_get_object(builder, "playlist-scrollbar");
	if(scrollbar)
	{
		gdouble x;
		g_object_get(G_OBJECT(scrollbar), "x", &x, NULL);
		gnome_canvas_item_set((GnomeCanvasItem*)scrollbar, "x", x + rx, NULL);
		x = skin_scroll_bar_get_height(scrollbar);
		skin_scroll_bar_set_height(scrollbar, x + ry);
	}
}

static gchar *tb_items[TOOLBAR_ITEMS] = {
	"toolbar-add",
	"toolbar-remove",
	"toolbar-list",
	"toolbar-sort",
	"toolbar-search",
	"toolbar-edit",
	"toolbar-mode"
};

static void
create_playlist_window(SkinBuilder *builder)
{
	PlaylistArchive *pl;
	GdkPixbuf *pixbuf;
	GnomeCanvasGroup *root;
	gdouble width, height;
	gdouble x;
	gint i;

	g_return_if_fail(SKIN_IS_BUILDER(builder));

	pl = builder->priv->ar->playlist;

	SkinWindow *window = skin_window_new(pl->window.img);
	add_object(builder, G_OBJECT(window), "playlist-window");
	skin_window_move(window, pl->window.x1, pl->window.y1);

	root = skin_window_get_canvas_root(window);

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
		add_object(builder, G_OBJECT(tb), tb_items[i]);
	}

	GnomeCanvasItem *playlistbox = gnome_canvas_item_new(root,
			gnome_canvas_widget_get_type(),
			"x", (gdouble)pl->playlist.x1,
			"y", (gdouble)pl->playlist.y1,
			"width", (gdouble)(pl->playlist.x2 - pl->playlist.x1),
			"height", (gdouble)(pl->playlist.y2 - pl->playlist.y1),
			"anchor", GTK_ANCHOR_NORTH_WEST,
			"size-pixels", TRUE,
			NULL);
	add_object(builder, G_OBJECT(playlistbox), "playlist-playlistbox");
	
	SkinScrollBar *bar = skin_scroll_bar_new(root,
			pl->scrollbar.buttons_img,
			pl->scrollbar.bar_img,
			pl->scrollbar.thumb_img,
			(gdouble)pl->playlist.x1,
			(gdouble)pl->playlist.y1,
			(gdouble)pl->playlist.x2,
			(gdouble)pl->playlist.y2);
	add_object(builder, G_OBJECT(bar), "playlist-scrollbar");

	gdouble w = skin_scroll_bar_get_width(bar);
	gnome_canvas_item_set(playlistbox, 
			"width", (gdouble)(pl->playlist.x2 - pl->playlist.x1) - w,
			NULL);

	g_signal_connect(G_OBJECT(window), "change-size", 
			G_CALLBACK(cb_playlist_change_size), builder);
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
	skin_window_move(window, mini->window.x1, mini->window.y1);

	root = skin_window_get_canvas_root(window);

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

	// FIXME:
	//SkinButton *icon = skin_button_new(root,
	//		mini->icon.img, 
	//		mini->icon.x1, mini->icon.y1);
	if(mini->icon.img)
	{
		GnomeCanvasItem *icon = gnome_canvas_item_new(root,
				gnome_canvas_pixbuf_get_type(),
				"pixbuf", mini->icon.img,
				"x", mini->icon.x1,
				"y", mini->icon.y1,
				NULL);
		add_object(builder, G_OBJECT(icon), "mini-icon");
	}

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

	SkinDynamicText *info = skin_dynamic_text_new(root, 
			"x1", (gdouble)(mini->info.x1),
			"y1", (gdouble)(mini->info.y1),
			"x2", (gdouble)(mini->info.x2),
			"y2", (gdouble)(mini->info.y2),
			"color", mini->info.color,
			"title", "title --",
			"artist", "artist --",
			"album", "album ==",
			"format", "format --",
			NULL);
	add_object(builder, G_OBJECT(info), "mini-info");
}

static void
set_player_window_prop(SkinBuilder *builder)
{
	PlayerArchive *player;

	g_return_if_fail(SKIN_IS_BUILDER(builder));

	player = builder->priv->ar->player;

	SkinWindow *window = (SkinWindow*)skin_builder_get_object(builder, "player-window");
	skin_window_set_pixbuf(window, player->window.img);
	skin_window_move(window, player->window.x1, player->window.y1);

	SkinButton *button;
	button = (SkinButton*)skin_builder_get_object(builder, "player-pause");
	skin_button_set_pixbuf(button, player->pause.img);
	skin_button_set_position(button, player->pause.x1, player->pause.y1);

	button = (SkinButton*)skin_builder_get_object(builder, "player-play");
	skin_button_set_pixbuf(button, player->play.img);
	skin_button_set_position(button, player->play.x1, player->play.y1);

	button = (SkinButton*)skin_builder_get_object(builder, "player-stop");
	skin_button_set_pixbuf(button, player->stop.img);
	skin_button_set_position(button, player->stop.x1, player->stop.y1);

	button = (SkinButton*)skin_builder_get_object(builder, "player-prev");
	skin_button_set_pixbuf(button, player->prev.img);
	skin_button_set_position(button, player->prev.x1, player->prev.y1);

	button = (SkinButton*)skin_builder_get_object(builder, "player-next");
	skin_button_set_pixbuf(button, player->next.img);
	skin_button_set_position(button, player->next.x1, player->next.y1);

	button = (SkinButton*)skin_builder_get_object(builder, "player-exit");
	skin_button_set_pixbuf(button, player->exit.img);
	skin_button_set_position(button, player->exit.x1, player->exit.y1);

	button = (SkinButton*)skin_builder_get_object(builder, "player-minimize");
	skin_button_set_pixbuf(button, player->minimize.img);
	skin_button_set_position(button, player->minimize.x1, player->minimize.y1);

	button = (SkinButton*)skin_builder_get_object(builder, "player-minimode");
	skin_button_set_pixbuf(button, player->minimode.img);
	skin_button_set_position(button, player->minimode.x1, player->minimode.y1);

	button = (SkinButton*)skin_builder_get_object(builder, "player-open");
	skin_button_set_pixbuf(button, player->open.img);
	skin_button_set_position(button, player->open.x1, player->open.y1);

	SkinDynamicText *info = (SkinDynamicText*)skin_builder_get_object(builder, "player-info");
	g_object_set(G_OBJECT(info),
			"x1", (gdouble)(player->info.x1),
			"y1", (gdouble)(player->info.y1),
			"x2", (gdouble)(player->info.x2),
			"y2", (gdouble)(player->info.y2),
			"color", player->info.color,
			"font", player->info.font,
			"size-points", (gdouble)player->info.font_size,
			"size-set", TRUE,
			NULL);

	SkinStatusBar *statusbar = (SkinStatusBar*)skin_builder_get_object(builder, "player-statusbar");
	g_object_set(G_OBJECT(statusbar),
			"x1", (gdouble)(player->status.x1),
			"y1", (gdouble)(player->status.y1),
			"x2", (gdouble)(player->status.x2),
			"y2", (gdouble)(player->status.y2),
			"color", player->status.color,
			"align", player->status.align,
			NULL);

	SkinHScale *progressbar = (SkinHScale*)skin_builder_get_object(builder, "player-progressbar");
	g_object_set(G_OBJECT(progressbar),
			"x1", (gdouble)(player->progress.x1),
			"y1", (gdouble)(player->progress.y1),
			"x2", (gdouble)(player->progress.x2),
			"y2", (gdouble)(player->progress.y2),
			"fill-pixbuf", player->progress.fill_img,
			"thumb-pixbuf", player->progress.thumb_img,
			NULL);

	SkinDigitalTime *led;
	led = (SkinDigitalTime*)skin_builder_get_object(builder, "player-led");
	skin_digital_time_set_pixbuf(led, player->led.img);
	skin_digital_time_set_position(led, (gdouble)(player->led.x1), (gdouble)(player->led.y1));

	SkinCheckButton *cbutton;
	cbutton = (SkinCheckButton*)skin_builder_get_object(builder, "player-playlist");
	skin_check_button_set_pixbuf(cbutton, player->playlist.img);
	skin_check_button_set_position(cbutton, player->playlist.x1, player->playlist.y1);

	cbutton = (SkinCheckButton*)skin_builder_get_object(builder, "player-lyric");
	skin_check_button_set_pixbuf(cbutton, player->lyric.img);
	skin_check_button_set_position(cbutton, player->lyric.x1, player->lyric.y1);

	cbutton = (SkinCheckButton*)skin_builder_get_object(builder, "player-equalizer");
	skin_check_button_set_pixbuf(cbutton, player->equalizer.img);
	skin_check_button_set_position(cbutton, player->equalizer.x1, player->equalizer.y1);

	cbutton = (SkinCheckButton*)skin_builder_get_object(builder, "player-mute");
	skin_check_button_set_pixbuf(cbutton, player->mute.img);
	skin_check_button_set_position(cbutton, player->mute.x1, player->mute.y1);

	SkinHScale *volume = (SkinHScale*)skin_builder_get_object(builder, "player-volume");
	g_object_set(G_OBJECT(volume),
			"x1", (gdouble)(player->volume.x1),
			"y1", (gdouble)(player->volume.y1),
			"x2", (gdouble)(player->volume.x2),
			"y2", (gdouble)(player->volume.y2),
			"fill-pixbuf", player->volume.fill_img,
			"thumb-pixbuf", player->volume.thumb_img,
			NULL);

	GnomeCanvasItem *visualbox = (GnomeCanvasItem*)skin_builder_get_object(builder, "player-visualbox");
	gnome_canvas_item_set(visualbox,
			"x", (gdouble)player->visual.x1,
			"y", (gdouble)player->visual.y1,
			"width", (gdouble)(player->visual.x2 - player->visual.x1),
			"height", (gdouble)(player->visual.y2 - player->visual.y1),
			NULL);
}

static void
set_equalizer_window_prop(SkinBuilder *builder)
{
	EqualizerArchive *eq;

	g_return_if_fail(SKIN_IS_BUILDER(builder));

	eq = builder->priv->ar->equalizer;

	SkinWindow *window = (SkinWindow*)skin_builder_get_object(builder, "equalizer-window");
	skin_window_set_pixbuf(window, eq->window.img);
	skin_window_move(window, eq->window.x1, eq->window.y1);

	SkinButton *button;
	button = (SkinButton*)skin_builder_get_object(builder, "equalizer-close");
	skin_button_set_pixbuf(button, eq->close.img);
	skin_button_set_position(button, eq->close.x1, eq->close.y1);

	SkinCheckButton *cbutton;
	cbutton = (SkinCheckButton*)skin_builder_get_object(builder, "equalizer-enabled");
	skin_check_button_set_pixbuf(cbutton, eq->enabled.img);
	skin_check_button_set_position(cbutton, eq->enabled.x1, eq->enabled.y1);

	cbutton = (SkinCheckButton*)skin_builder_get_object(builder, "equalizer-profile");
	skin_check_button_set_pixbuf(cbutton, eq->profile.img);
	skin_check_button_set_position(cbutton, eq->profile.x1, eq->profile.y1);

	cbutton = (SkinCheckButton*)skin_builder_get_object(builder, "equalizer-reset");
	skin_check_button_set_pixbuf(cbutton, eq->reset.img);
	skin_check_button_set_position(cbutton, eq->reset.x1, eq->reset.y1);

	SkinHScale *hscale = (SkinHScale*)skin_builder_get_object(builder, "equalizer-balance");
	g_object_set(G_OBJECT(hscale),
			"x1", (gdouble)(eq->balance.x1),
			"y1", (gdouble)(eq->balance.y1),
			"x2", (gdouble)(eq->balance.x2),
			"y2", (gdouble)(eq->balance.y2),
			"fill-pixbuf", eq->balance.fill_img,
			"thumb-pixbuf", eq->balance.thumb_img,
			NULL);

	hscale = (SkinHScale*)skin_builder_get_object(builder, "equalizer-surround");
	g_object_set(G_OBJECT(hscale),
			"x1", (gdouble)(eq->surround.x1),
			"y1", (gdouble)(eq->surround.y1),
			"x2", (gdouble)(eq->surround.x2),
			"y2", (gdouble)(eq->surround.y2),
			"fill-pixbuf", eq->surround.fill_img,
			"thumb-pixbuf", eq->surround.thumb_img,
			NULL);

	SkinVScale *vscale = (SkinVScale*)skin_builder_get_object(builder, "equalizer-preamp");
	g_object_set(G_OBJECT(vscale),
			"x1", (gdouble)(eq->preamp.x1),
			"y1", (gdouble)(eq->preamp.y2),
			"x2", (gdouble)(eq->preamp.x2),
			"y2", (gdouble)(eq->preamp.y1),
			"fill-pixbuf", eq->preamp.fill_img,
			"thumb-pixbuf", eq->preamp.thumb_img,
			NULL);

	gchar name[32];
	gint i;
	for(i = 0; i < 10; ++i)
	{
		sprintf(name, "equalizer-eqfactor%d", i);
		vscale = (SkinVScale*)skin_builder_get_object(builder, name);
		g_object_set(G_OBJECT(vscale),
			"x1", (gdouble)(eq->eqfactor.x1) + eq->window.eq_interval * i,
			"y1", (gdouble)(eq->eqfactor.y2),
			"x2", (gdouble)(eq->eqfactor.x2) + eq->window.eq_interval * i,
			"y2", (gdouble)(eq->eqfactor.y1),
			"fill-pixbuf", eq->eqfactor.fill_img,
			"thumb-pixbuf", eq->eqfactor.thumb_img,
			NULL);

	}
}

static void
set_lyric_window_prop(SkinBuilder *builder)
{
	LyricArchive *lyric;

	g_return_if_fail(SKIN_IS_BUILDER(builder));

	lyric = builder->priv->ar->lyric;

	SkinWindow *window = (SkinWindow*)skin_builder_get_object(builder, "lyric-window");
	skin_window_set_pixbuf(window, lyric->window.img);
	skin_window_move(window, lyric->window.x1, lyric->window.y1);

	SkinButton *button;
	button = (SkinButton*)skin_builder_get_object(builder, "lyric-close");
	skin_button_set_pixbuf(button, lyric->close.img);
	skin_button_set_position(button, lyric->close.x1, lyric->close.y1);

	SkinCheckButton *cbutton;
	cbutton = (SkinCheckButton*)skin_builder_get_object(builder, "lyric-ontop");
	skin_check_button_set_pixbuf(cbutton, lyric->ontop.img);
	skin_check_button_set_position(cbutton, lyric->ontop.x1, lyric->ontop.y1);

	GnomeCanvasItem *lyricbox = (GnomeCanvasItem*)skin_builder_get_object(builder, "lyric-lyricbox");
	gnome_canvas_item_set(lyricbox,
			"x", (gdouble)lyric->lyric.x1,
			"y", (gdouble)lyric->lyric.y1,
			"width", (gdouble)(lyric->lyric.x2 - lyric->lyric.x1),
			"height", (gdouble)(lyric->lyric.y2 - lyric->lyric.y1),
			NULL);

	SkinLyric *lyricview = (SkinLyric*)skin_builder_get_object(builder, "lyric-lyricview");
	skin_lyric_set_size(lyricview, 
			lyric->lyric.x2 - lyric->lyric.x1,
			lyric->lyric.y2 - lyric->lyric.y1);
	skin_lyric_set_bg_color(lyricview, &lyric->attr.color_bg);
	skin_lyric_set_text_color(lyricview, &lyric->attr.color_text);
	skin_lyric_set_highlight_color(lyricview, &lyric->attr.color_hilight);
}

static void
set_playlist_window_prop(SkinBuilder *builder)
{
	PlaylistArchive *pl;
	gint i;

	g_return_if_fail(SKIN_IS_BUILDER(builder));

	pl = builder->priv->ar->playlist;

	SkinWindow *window = (SkinWindow*)skin_builder_get_object(builder, "playlist-window");
	skin_window_set_pixbuf(window, pl->window.img);
	skin_window_move(window, pl->window.x1, pl->window.y1);

	SkinButton *button;
	button = (SkinButton*)skin_builder_get_object(builder, "playlist-close");
	skin_button_set_pixbuf(button, pl->close.img);
	skin_button_set_position(button, pl->close.x1, pl->close.y1);

	gdouble width = (gdouble)gdk_pixbuf_get_width(pl->toolbar.img);
	gdouble height = (gdouble)gdk_pixbuf_get_height(pl->toolbar.img);
	width = width / TOOLBAR_ITEMS;
	gdouble x = pl->toolbar.x1;

	for(i = 0; i < TOOLBAR_ITEMS; ++i)
	{
		GdkPixbuf *pixbuf = gdk_pixbuf_new_subpixbuf(pl->toolbar.img,
				i * width, 0, width, height);

		x = pl->toolbar.x1 + i * width;
		SkinToggleButton *tb = (SkinToggleButton*)skin_builder_get_object(builder, tb_items[i]);
		skin_toggle_button_set_pixbuf(tb, pixbuf);
		skin_toggle_button_set_position(tb, x, pl->toolbar.y1);
	}

	GnomeCanvasItem *playlistbox = 
		(GnomeCanvasItem*)skin_builder_get_object(builder, "playlist-playlistbox");
	gnome_canvas_item_set(playlistbox,
			"x", (gdouble)pl->playlist.x1,
			"y", (gdouble)pl->playlist.y1,
			"width", (gdouble)(pl->playlist.x2 - pl->playlist.x1),
			"height", (gdouble)(pl->playlist.y2 - pl->playlist.y1),
			NULL);

	SkinScrollBar *bar = (SkinScrollBar*)skin_builder_get_object(builder, "playlist-scrollbar");
	skin_scroll_bar_rebuild(bar,
			pl->scrollbar.buttons_img,
			pl->scrollbar.bar_img,
			pl->scrollbar.thumb_img,
			(gdouble)pl->playlist.x1,
			(gdouble)pl->playlist.y1,
			(gdouble)pl->playlist.x2,
			(gdouble)pl->playlist.y2);

	gdouble w = skin_scroll_bar_get_width(bar);
	gnome_canvas_item_set(playlistbox, 
			"width", (gdouble)(pl->playlist.x2 - pl->playlist.x1) - w,
			NULL);


}

static void
set_mini_window_prop(SkinBuilder *builder)
{
	MiniArchive *mini;

	g_return_if_fail(SKIN_IS_BUILDER(builder));

	mini = builder->priv->ar->mini;

	SkinWindow *window = (SkinWindow*)skin_builder_get_object(builder, "mini-window");
	skin_window_set_pixbuf(window, mini->window.img);
	skin_window_move(window, mini->window.x1, mini->window.y1);

	SkinButton *button;
	button = (SkinButton*)skin_builder_get_object(builder, "mini-pause");
	skin_button_set_pixbuf(button, mini->pause.img);
	skin_button_set_position(button, mini->pause.x1, mini->pause.y1);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-play");
	skin_button_set_pixbuf(button, mini->play.img);
	skin_button_set_position(button, mini->play.x1, mini->play.y1);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-stop");
	skin_button_set_pixbuf(button, mini->stop.img);
	skin_button_set_position(button, mini->stop.x1, mini->stop.y1);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-prev");
	skin_button_set_pixbuf(button, mini->prev.img);
	skin_button_set_position(button, mini->prev.x1, mini->prev.y1);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-next");
	skin_button_set_pixbuf(button, mini->next.img);
	skin_button_set_position(button, mini->next.x1, mini->next.y1);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-exit");
	skin_button_set_pixbuf(button, mini->exit.img);
	skin_button_set_position(button, mini->exit.x1, mini->exit.y1);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-minimize");
	skin_button_set_pixbuf(button, mini->minimize.img);
	skin_button_set_position(button, mini->minimize.x1, mini->minimize.y1);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-minimode");
	skin_button_set_pixbuf(button, mini->minimode.img);
	skin_button_set_position(button, mini->minimode.x1, mini->minimode.y1);

	GnomeCanvasItem *icon = (GnomeCanvasItem*)skin_builder_get_object(builder, "mini-icon");
	if(icon)
	{
		gnome_canvas_item_set(icon,
				"pixbuf", mini->icon.img,
				"x", mini->icon.x1,
				"y", mini->icon.y1,
				NULL);
	}

	SkinDynamicText *info = (SkinDynamicText*)skin_builder_get_object(builder, "mini-info");
	g_object_set(G_OBJECT(info),
			"x1", (gdouble)(mini->info.x1),
			"y1", (gdouble)(mini->info.y1),
			"x2", (gdouble)(mini->info.x2),
			"y2", (gdouble)(mini->info.y2),
			"color", mini->info.color,
			"font", mini->info.font,
			"size-points", (gdouble)mini->info.font_size,
			"size-set", TRUE,
			NULL);

	SkinCheckButton *cbutton;
	cbutton = (SkinCheckButton*)skin_builder_get_object(builder, "mini-lyric");
	skin_check_button_set_pixbuf(cbutton, mini->lyric.img);
	skin_check_button_set_position(cbutton, mini->lyric.x1, mini->lyric.y1);
}

SkinBuilder *
skin_builder_new()
{
	SkinBuilder *builder;
	builder = SKIN_BUILDER(g_object_new(SKIN_TYPE_BUILDER, NULL));
	return builder; 
}

gint 
skin_builder_add_from_archive(SkinBuilder *builder, SkinArchive *ar)
{
	g_return_val_if_fail(SKIN_IS_BUILDER(builder), -1);
	g_return_val_if_fail(SKIN_IS_ARCHIVE(ar), -1);

	if(builder->priv->ar == NULL)
	{
		builder->priv->ar = ar;
		g_object_ref(builder->priv->ar);

		create_player_window(builder);
		create_equalizer_window(builder);
		create_lyric_window(builder);
		create_playlist_window(builder);
		create_mini_window(builder);

		return 0;
	}
	else
	{
		// TODO: builder->priv->ar == ar?
		g_object_unref(builder->priv->ar);
		builder->priv->ar = ar;
		g_object_ref(builder->priv->ar);
		
		set_player_window_prop(builder);
		set_equalizer_window_prop(builder);
		set_lyric_window_prop(builder);
		set_playlist_window_prop(builder);
		set_mini_window_prop(builder);

		return 0;
	}
}

GObject *
skin_builder_get_object(SkinBuilder *builder, const gchar *name)
{
	g_return_val_if_fail(SKIN_BUILDER(builder), NULL);
	g_return_val_if_fail(name != NULL, NULL);

	return g_hash_table_lookup(builder->priv->objects, name);
}

