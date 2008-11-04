/* vi: set sw=4 ts=4: */
/*
 * skinbuilder.c
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
#include "skinvolumebutton.h"


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
	SkinButton *play = skin_button_new(root, 
			player->play.img, 
			player->play.x1, player->play.y1);
	add_object(builder, G_OBJECT(play), "player-play-button");

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
	return 0;
}

GObject *skin_builder_get_object(SkinBuilder *builder, const gchar *name)
{
	g_return_val_if_fail(SKIN_BUILDER(builder), NULL);
	g_return_val_if_fail(name != NULL, NULL);

	return g_hash_table_lookup(builder->priv->objects, name);
}
