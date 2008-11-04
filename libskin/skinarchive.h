/* vi: set sw=4 ts=4: */
/*
 * skinarchive.h
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

#ifndef __SKINARCHIVE_H__
#define __SKINARCHIVE_H__  1

#include <glib-object.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <libxml/xmlreader.h>
#include <libxml/xpath.h>

G_BEGIN_DECLS

#define MAX_DIR 256
#define IMG_SUFFIX_LENGTH 4

/* XP: means x-path */
#define XP_SKIN				"/skin"

#define XP_PLAYER_WINDOW 	"/skin/player_window"
#define XP_PLAYER_WINDOW_PLAY 			XP_PLAYER_WINDOW"/play"
#define XP_PLAYER_WINDOW_PAUSE 			XP_PLAYER_WINDOW"/pause"
#define XP_PLAYER_WINDOW_PREV 			XP_PLAYER_WINDOW"/prev"
#define XP_PLAYER_WINDOW_NEXT 			XP_PLAYER_WINDOW"/next"
#define XP_PLAYER_WINDOW_STOP 			XP_PLAYER_WINDOW"/stop"
#define XP_PLAYER_WINDOW_MUTE 			XP_PLAYER_WINDOW"/mute"
#define XP_PLAYER_WINDOW_OPEN 			XP_PLAYER_WINDOW"/open"
#define XP_PLAYER_WINDOW_LYRIC 			XP_PLAYER_WINDOW"/lyric"
#define XP_PLAYER_WINDOW_EQUALIZER 		XP_PLAYER_WINDOW"/equalizer"
#define XP_PLAYER_WINDOW_PLAYLIST 		XP_PLAYER_WINDOW"/playlist"
#define XP_PLAYER_WINDOW_MINIMIZE 		XP_PLAYER_WINDOW"/minimize"
#define XP_PLAYER_WINDOW_MINIMODE 		XP_PLAYER_WINDOW"/minimode"
#define XP_PLAYER_WINDOW_EXIT 			XP_PLAYER_WINDOW"/exit"
#define XP_PLAYER_WINDOW_PROGRESS 		XP_PLAYER_WINDOW"/progress"
#define XP_PLAYER_WINDOW_VOLUME 			XP_PLAYER_WINDOW"/volume"
#define XP_PLAYER_WINDOW_ICON 			XP_PLAYER_WINDOW"/icon"
#define XP_PLAYER_WINDOW_INFO 			XP_PLAYER_WINDOW"/info"
#define XP_PLAYER_WINDOW_STATUS 			XP_PLAYER_WINDOW"/status"
#define XP_PLAYER_WINDOW_LED 				XP_PLAYER_WINDOW"/led"
#define XP_PLAYER_WINDOW_VISUAL 			XP_PLAYER_WINDOW"/visual"

#define XP_LYRIC_WINDOW 	"/skin/lyric_window"
#define XP_LYRIC_WINDOW_LYRIC 	XP_LYRIC_WINDOW"/lyric"
#define XP_LYRIC_WINDOW_CLOSE 	XP_LYRIC_WINDOW"/close"
#define XP_LYRIC_WINDOW_ONTOP 	XP_LYRIC_WINDOW"/ontop"

#define XP_EQUALIZER_WINDOW "/skin/equalizer_window"
#define XP_EQUALIZER_WINDOW_CLOSE	XP_EQUALIZER_WINDOW"/close"
#define XP_EQUALIZER_WINDOW_ENABLED	XP_EQUALIZER_WINDOW"/enabled"
#define XP_EQUALIZER_WINDOW_PROFILE	XP_EQUALIZER_WINDOW"/profile"
#define XP_EQUALIZER_WINDOW_RESET	XP_EQUALIZER_WINDOW"/reset"
#define XP_EQUALIZER_WINDOW_BALANCE	XP_EQUALIZER_WINDOW"/balance"
#define XP_EQUALIZER_WINDOW_SURROUND	XP_EQUALIZER_WINDOW"/surround"
#define XP_EQUALIZER_WINDOW_PREAMP	XP_EQUALIZER_WINDOW"/preamp"
#define XP_EQUALIZER_WINDOW_EQFACTOR	XP_EQUALIZER_WINDOW"/eqfactor"

#define XP_PLAYLIST_WINDOW "/skin/playlist_window"
#define XP_PLAYLIST_WINDOW_CLOSE	XP_PLAYLIST_WINDOW"/close"
#define XP_PLAYLIST_WINDOW_TOOLBAR	XP_PLAYLIST_WINDOW"/toolbar"
#define XP_PLAYLIST_WINDOW_SCROLLBAR	XP_PLAYLIST_WINDOW"/scrollbar"
#define XP_PLAYLIST_WINDOW_PLAYLIST	XP_PLAYLIST_WINDOW"/playlist"

#define XP_MINI_WINDOW 	"/skin/mini_window"
#define XP_MINI_WINDOW_PLAY 			XP_MINI_WINDOW"/play"
#define XP_MINI_WINDOW_PAUSE 			XP_MINI_WINDOW"/pause"
#define XP_MINI_WINDOW_PREV 			XP_MINI_WINDOW"/prev"
#define XP_MINI_WINDOW_NEXT 			XP_MINI_WINDOW"/next"
#define XP_MINI_WINDOW_STOP 			XP_MINI_WINDOW"/stop"
#define XP_MINI_WINDOW_LYRIC 			XP_MINI_WINDOW"/lyric"
#define XP_MINI_WINDOW_ICON 			XP_MINI_WINDOW"/icon"
#define XP_MINI_WINDOW_MINIMIZE 		XP_MINI_WINDOW"/minimize"
#define XP_MINI_WINDOW_MINIMODE 		XP_MINI_WINDOW"/minimode"
#define XP_MINI_WINDOW_EXIT 			XP_MINI_WINDOW"/exit"
#define XP_MINI_WINDOW_INFO 			XP_MINI_WINDOW"/info"

#define XP_PLAYLIST_ATTR				"/ttplayer_playlist/PlayList"
#define XP_LYRIC_ATTR					"/ttplayer_lyric/Lyric"
#define XP_VISUAL_ATTR					"/ttplayer_visual/Visual"


#define SKIN_TYPE_ARCHIVE              (skin_archive_get_type ())
#define SKIN_ARCHIVE(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), SKIN_TYPE_ARCHIVE, SkinArchive)) 
#define SKIN_ARCHIVE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), SKIN_TYPE_ARCHIVE, SkinArchiveClass))
#define SKIN_IS_ARCHIVE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SKIN_TYPE_ARCHIVE))
#define SKIN_IS_ARCHIVE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), SKIN_TYPE_ARCHIVE))
#define SKIN_ARCHIVE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), SKIN_TYPE_ARCHIVE, SkinArchiveClass))


typedef struct _SkinArchiveInfo
{
	gchar* name;
	gchar* author;
	gchar* email;
	gchar* url;
	gchar* path;
	gchar* version;
	gboolean used;
} SkinArchiveInfo;

typedef struct _SkinItem
{
	int x1;
	int y1;
	int x2;
	int y2;

	GdkPixbuf* img; 
	GdkPixbuf* bar_img;
	GdkPixbuf* buttons_img;
	GdkPixbuf* thumb_img;
	GdkPixbuf* fill_img;
	GdkPixbuf* hot_img;

	GdkColor color;
	gchar* font;
	gint font_size;
	gint align; /*! 0: left 1: center 2: right */
	gint vertical; /*! 0: not vertical 1: vertical */
	gint eq_interval;
} SkinItem;

typedef struct _PlaylistAttr
{
	gchar* font;
	GdkColor color_text;
	GdkColor color_hilight;
	GdkColor color_bg;
	GdkColor color_number;
	GdkColor color_duration;
	GdkColor color_select;
	GdkColor color_bg2;
} PlaylistAttr;

typedef struct _LyricAttr
{
	gchar* font;
	GdkColor color_text;
	GdkColor color_hilight;
	GdkColor color_bg;
} LyricAttr;

typedef struct _VisualAttr
{
	GdkColor color_spectrum_top;
	GdkColor color_spectrum_bottom;
	GdkColor color_spectrum_mid;
	GdkColor color_spectrum_peak;
	gchar* spectrum_width; //FIXME:
	gint blur_speed;
	gint blur;
	GdkColor color_blur_scope;
} VisualAttr;

typedef struct _PlayerArchive
{
	SkinItem window;
	SkinItem play;
	SkinItem pause;
	SkinItem prev;
	SkinItem next;
	SkinItem stop;
	SkinItem mute;
	SkinItem open;
	SkinItem lyric;
	SkinItem equalizer;
	SkinItem playlist;
	SkinItem minimize;
	SkinItem minimode;
	SkinItem exit;
	SkinItem progress;
	SkinItem volume;
	SkinItem icon;
	SkinItem info;
	SkinItem status;
	SkinItem led;
	SkinItem visual;
	VisualAttr attr;
} PlayerArchive;

typedef struct _LyricArchive
{
	SkinItem window;
	SkinItem lyric;
	SkinItem close;
	SkinItem ontop;
	LyricAttr attr;
	gboolean stand_alone;
} LyricArchive;

typedef struct _EqualizerArchive
{
	SkinItem window;
	SkinItem close;
	SkinItem enabled;
	SkinItem profile;
	SkinItem reset;
	SkinItem balance;
	SkinItem surround;
	SkinItem preamp;
	SkinItem eqfactor;
	gboolean stand_alone;
} EqualizerArchive;

typedef struct _PlaylistArchive
{
	SkinItem window;
	SkinItem close;
	SkinItem toolbar;
	SkinItem scrollbar;
	SkinItem playlist;
	PlaylistAttr attr;
	gboolean stand_alone;
} PlaylistArchive;

typedef struct _MiniArchive
{
	SkinItem window;
	SkinItem play;
	SkinItem pause;
	SkinItem prev;
	SkinItem next;
	SkinItem stop;
	SkinItem lyric;
	SkinItem icon;
	SkinItem minimize;
	SkinItem minimode;
	SkinItem exit;
	SkinItem info;
} MiniArchive;

typedef struct _SkinArchivePrivate SkinArchivePrivate;

typedef struct _SkinArchive
{
	GObject parent;

	SkinArchiveInfo *info;
	PlayerArchive *player;
	LyricArchive *lyric;
	EqualizerArchive *equalizer;
	PlaylistArchive *playlist;
	MiniArchive *mini;

	SkinArchivePrivate *priv;
} SkinArchive;

typedef struct _SkinArchiveClass
{
	GObjectClass parent_class;
} SkinArchiveClass;

GType skin_archive_get_type(void) G_GNUC_CONST;
SkinArchive *skin_archive_new();
gboolean skin_archive_load(SkinArchive *ar, const char *filename);

G_END_DECLS

#endif /*__SKINARCHIVE_H__ */
