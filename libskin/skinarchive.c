/* vi: set sw=4 ts=4: */
/*
 * skinarchive.c
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

#include "skinarchive.h"
#include <glib.h>
#include <glib/gstdio.h>
#include <unistd.h>
#include <string.h>

enum {
    LAST_SIGNAL
};

enum {
    PROP_0,
};

//static int signals[LAST_SIGNAL];

#define SKIN_ARCHIVE_GET_PRIVATE(obj)  (G_TYPE_INSTANCE_GET_PRIVATE((obj), SKIN_TYPE_ARCHIVE, SkinArchivePrivate))

struct _SkinArchivePrivate
{
	gchar *path;
};

static void skin_archive_class_init(SkinArchiveClass *class);
static void skin_archive_init(SkinArchive *skin_archive);

static void skin_archive_set_property  (GObject          *object,
                                         guint             prop_id,
                                         const GValue     *value,
                                         GParamSpec       *pspec);
static void skin_archive_get_property  (GObject          *object,
                                         guint             prop_id,
                                         GValue           *value,
                                         GParamSpec       *pspec);

static char * uncompress_skin_archive(const gchar *zipfilename);
static void delete_temp_dir(const gchar *dir);

static xmlXPathObjectPtr xml_nodeset_get_from_xpath(xmlDocPtr doc, const xmlChar * xpath);
static gboolean position_parse(const gchar *str, 
							int *x1, 
							int *y1, 
							int *x2, 
							int *y2);
static gboolean read_item_with_xpath(xmlDocPtr doc, 
											const gchar* xpath, 
											SkinItem *item, 
											const char* skin_path);
static void load_player_archive(xmlDocPtr doc, PlayerArchive* ma, const gchar* path);
static void load_lyric_archive(xmlDocPtr doc, LyricArchive* ma, const gchar* path);
static void load_equalizer_archive(xmlDocPtr doc, EqualizerArchive* ma, const gchar* path);
static void load_playlist_archive(xmlDocPtr doc, PlaylistArchive* ma, const gchar* path);
static void load_mini_archive(xmlDocPtr doc, MiniArchive* ma, const gchar* path);
static gboolean load_playlist_attr(const gchar* filename, PlaylistAttr* attr);
static gboolean load_lyric_attr( const gchar* filename, LyricAttr* attr);
static gboolean load_visual_attr(const gchar* filename, VisualAttr* attr);
static gboolean do_load(SkinArchive *ar, const char *filename);

G_DEFINE_TYPE (SkinArchive, skin_archive, G_TYPE_OBJECT)//SKIN_TYPE_ARCHIVE)

static void
skin_archive_class_init (SkinArchiveClass *class)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (class);

    gobject_class->set_property = skin_archive_set_property;
    gobject_class->get_property = skin_archive_get_property;

    g_type_class_add_private (class, sizeof (SkinArchivePrivate));

}

static void
skin_archive_init (SkinArchive *ar)
{
    ar->priv = SKIN_ARCHIVE_GET_PRIVATE(ar);
	
	ar->info = g_new0(SkinArchiveInfo, 1);
	ar->player = g_new0(PlayerArchive, 1);
	ar->lyric = g_new0(LyricArchive, 1);
	ar->equalizer = g_new0(EqualizerArchive, 1);
	ar->playlist = g_new0(PlaylistArchive, 1);
	ar->mini = g_new0(MiniArchive, 1);
}

static void
skin_archive_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    SkinArchive *skin_archive;

    skin_archive = SKIN_ARCHIVE (object);

    switch (prop_id)
    {

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
skin_archive_get_property (GObject      *object,
                            guint         prop_id,
                            GValue       *value,
                            GParamSpec   *pspec)
{
    SkinArchive *skin_archive;

    skin_archive = SKIN_ARCHIVE (object);

    switch (prop_id)
    {

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

SkinArchive *skin_archive_new()
{
	g_print("skin_archive_new\n");
	return g_object_new(SKIN_TYPE_ARCHIVE, NULL);
}


gboolean 
skin_archive_load(SkinArchive *ar, const char* filename)
{
	gchar *xml = NULL;

	g_print("skin_archive_load start\n");
	g_return_val_if_fail(ar != NULL, FALSE);
	g_return_val_if_fail(filename != NULL, FALSE);

	g_print("uncompress_skin_archive\n");
	ar->priv->path = uncompress_skin_archive(filename);

	g_return_val_if_fail(ar->priv->path != NULL, FALSE);

	g_print("load skin.xml\n");
	xml = g_build_filename(ar->priv->path, "skin.xml", NULL);
	do_load(ar, xml);
	g_free(xml);

	g_print("load lyric.xml\n");
	xml = g_build_filename(ar->priv->path, "lyric.xml", NULL);
	load_lyric_attr(xml, &(ar->lyric->attr));
	g_free(xml);
	
	g_print("load visual.xml\n");
	xml = g_build_filename(ar->priv->path, "visual.xml", NULL);
	load_visual_attr(xml, &(ar->player->attr));
	g_free(xml);

	g_print("load playlist.xml\n");
	xml = g_build_filename(ar->priv->path, "playlist.xml", NULL);
	load_playlist_attr(xml, &(ar->playlist->attr));
	g_free(xml);

	g_print("load done\n");
	return TRUE;
}

static void 
delete_temp_dir(const gchar *dir)
{
	gchar cmd[512];
	sprintf(cmd, "rm -rf %s", dir);
	system(cmd);
}

static char *
uncompress_skin_archive(const gchar *zipfilename)
{
	char cmd[512];

	const gchar *tempdir = NULL;
	char subdir[256] = "lmplayer.XXXXXX";
	char *fulldir = NULL; 

	char *flag = NULL;
	int re = -1;

	tempdir = g_get_tmp_dir();
	fulldir = g_build_path(G_DIR_SEPARATOR_S, tempdir, subdir, NULL);

	flag = mkdtemp(fulldir);

	if(flag)
	{
		sprintf(cmd, "unzip -o -q %s -d %s", zipfilename, fulldir);
		re = system(cmd);
	}
	
	if(re == 0)
		return fulldir;

	if(flag)
		delete_temp_dir(fulldir);

	g_free(fulldir);
	return NULL;
}

static xmlXPathObjectPtr 
xml_nodeset_get_from_xpath(xmlDocPtr doc, const xmlChar * xpath)
{
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;
    context = xmlXPathNewContext(doc);
    if (context == NULL) 
	{
		return NULL;
    }
    result = xmlXPathEvalExpression(xpath, context);
    xmlXPathFreeContext(context);
    if (result == NULL) 
	{
		return NULL;
    }
    if (xmlXPathNodeSetIsEmpty(result->nodesetval)) 
	{
		xmlXPathFreeObject(result);
		return NULL;
    }
    return result;
}

static gboolean
position_parse(const gchar *str, int *x1, int *y1, int *x2, int *y2)
{
	g_return_val_if_fail(str != NULL, FALSE);

	gchar **p = g_strsplit(str, ",", 4);

	g_return_val_if_fail(p[0] != NULL, FALSE);
	g_return_val_if_fail(p[1] != NULL, FALSE);
	g_return_val_if_fail(p[2] != NULL, FALSE);
	g_return_val_if_fail(p[3] != NULL, FALSE);
	*x1 = atoi(p[0]);
	*y1 = atoi(p[1]);
	*x2 = atoi(p[2]);
	*y2 = atoi(p[3]);

	g_strfreev(p);

	return TRUE;
}

static gboolean 
read_item_with_xpath(xmlDocPtr doc, 
								const gchar* xpath, 
								SkinItem *item, 
								const char* skin_path)
{
	xmlXPathObjectPtr app_result;
	xmlNodeSetPtr nodeset;
	xmlNodePtr cur;
	gchar* value;
	gboolean flag = FALSE;

    app_result = xml_nodeset_get_from_xpath(doc, (const xmlChar *)xpath);
    if (app_result == NULL) 
		return FALSE;

	nodeset = app_result->nodesetval;
	cur = nodeset->nodeTab[0];

	g_print("position\n");
	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"position");
	if(value && (strlen(value) > 0))
	{
		flag = position_parse(value, &item->x1, &item->y1, &item->x2, &item->y2);
		xmlFree(value);
		g_return_val_if_fail(flag, FALSE);
	}
	
	g_print("align\n");
	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"align");
	if(value && (strlen(value) > 0))
	{
		if(strcmp(value, "right") ==0)
			item->align = 2;
		else if(strcmp(value, "center") == 0)
			item->align = 1;
		else
			item->align = 0;
		xmlFree(value);
	}
	else
		item->align = 0;

	g_print("vertical\n");
	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"vertical");
	if(value && (strlen(value) > 0))
	{
		if(strcmp(value, "false") == 0)
			item->vertical = 0;
		else
			item->vertical = 1;

		xmlFree(value);
	}
	else 
		item->vertical = 0;

	g_print("color\n");
	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"color");
	if(value && (strlen(value) > 0))
		gdk_color_parse(value, &(item->color));

	g_print("font\n");
	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"font");
	if(value != NULL)
		item->font = value;
	else
		item->font = NULL;

	g_print("font_size\n");
	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"font_size");
	if(value && (strlen(value) > 0))
		item->font_size = atoi(value);
	else
		item->font_size = 10;

	g_print("img\n");
	item->img = NULL;
	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"image");
	if(value && (strlen(value) > IMG_SUFFIX_LENGTH))
	{
		gchar* full_path = g_build_filename(skin_path, value, NULL);
		xmlFree(value);
		if(g_access(full_path, R_OK) == 0)
		{
			item->img = gdk_pixbuf_new_from_file(full_path, NULL);
		}

		g_free(full_path);
	}
	
	g_print("bar_img\n");
	item->bar_img = NULL;
	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"bar_image");
	if(value && (strlen(value) > IMG_SUFFIX_LENGTH))
	{
		gchar* full_path = g_build_filename(skin_path, value, NULL);
		xmlFree(value);
		g_assert(!g_access(full_path, R_OK));
		item->bar_img = gdk_pixbuf_new_from_file(full_path, NULL);
		g_free(full_path);
		g_assert(item->bar_img);
	}

	item->buttons_img = NULL;
	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"buttons_image");
	if(value && (strlen(value) > IMG_SUFFIX_LENGTH))
	{
		gchar* full_path = g_build_filename(skin_path, value, NULL);
		xmlFree(value);
		g_assert(!g_access(full_path, R_OK));
		item->buttons_img = gdk_pixbuf_new_from_file(full_path, NULL);
		g_free(full_path);
		g_assert(item->buttons_img);
	}

	item->thumb_img = NULL;
	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"thumb_image");
	if(value && (strlen(value) > IMG_SUFFIX_LENGTH))
	{
		gchar* full_path = g_build_filename(skin_path, value, NULL);
		xmlFree(value);
		item->thumb_img = gdk_pixbuf_new_from_file(full_path, NULL);
		g_free(full_path);
		g_assert(item->thumb_img);
	}

	item->fill_img = NULL;
	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"fill_image");
	if(value && (strlen(value) > IMG_SUFFIX_LENGTH))
	{
		gchar* full_path = g_build_filename(skin_path, value, NULL);
		xmlFree(value);
		item->fill_img = gdk_pixbuf_new_from_file(full_path, NULL);
		g_free(full_path);
	}

	item->hot_img = NULL;
	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"hot_image"); 
	if(value && (strlen(value) > IMG_SUFFIX_LENGTH))
	{
		gchar* full_path = g_build_filename(skin_path, value, NULL);
		xmlFree(value);
		item->hot_img = gdk_pixbuf_new_from_file(full_path, NULL);
		g_free(full_path);
	}

	xmlXPathFreeObject(app_result);

	return TRUE;
}

static gboolean
read_info(xmlDocPtr doc, SkinArchiveInfo *info)
{
	xmlXPathObjectPtr app_result;
	xmlNodeSetPtr nodeset;
	xmlNodePtr cur;
	xmlChar* value;

    app_result = xml_nodeset_get_from_xpath(doc, (const xmlChar*)XP_SKIN);
    if (app_result == NULL) 
		return FALSE;

	nodeset = app_result->nodesetval;
	cur = nodeset->nodeTab[0];

	value = xmlGetProp(cur, (const xmlChar *)"version");
	if(value)
		info->version = (gchar*)value;
	else
		info->version = NULL;

	value = xmlGetProp(cur, (const xmlChar *)"name");
	if(value)
		info->name = (gchar*)value;
	else
		info->name = NULL;

	value = xmlGetProp(cur, (const xmlChar *)"author");
	if(value)
		info->author = (gchar*)value;
	else
		info->author = NULL;

	value = xmlGetProp(cur, (const xmlChar *)"email");
	if(value)
		info->email = (gchar*)value;
	else
		info->email = NULL;

	value = xmlGetProp(cur, (const xmlChar *)"url");
	if(value)
		info->url = (gchar*)value;
	else
		info->url = NULL;

	return TRUE;
}

static void 
load_player_archive( xmlDocPtr doc, PlayerArchive* pa, const gchar* path)
{
	g_return_if_fail(doc != NULL);
	g_return_if_fail(pa != NULL);
	g_return_if_fail(path != NULL);

	g_print("window\n");
	read_item_with_xpath(doc, XP_PLAYER_WINDOW, &(pa->window), path); 
	g_print("window\n");
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_PLAY, &(pa->play), path);
	g_print("window\n");
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_PAUSE, &(pa->pause), path);
	g_print("window\n");
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_PREV, &(pa->prev), path);
	g_print("window\n");
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_NEXT, &(pa->next), path);
	g_print("window\n");
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_STOP, &(pa->stop), path);
	g_print("window\n");
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_MUTE, &(pa->mute), path);
	g_print("window\n");
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_OPEN, &(pa->open), path);
	g_print("window\n");
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_LYRIC, &(pa->lyric), path); 
	g_print("window\n");
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_EQUALIZER, &(pa->equalizer), path);
	g_print("window\n");
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_PLAYLIST, &(pa->playlist), path); 
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_MINIMIZE, &(pa->minimize), path);
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_MINIMODE, &(pa->minimode), path);
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_EXIT, &(pa->exit), path);
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_PROGRESS, &(pa->progress), path);
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_VOLUME, &(pa->volume), path);
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_ICON, &(pa->icon), path);
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_INFO, &(pa->info), path);
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_STATUS, &(pa->status), path);
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_LED, &(pa->led), path);
	read_item_with_xpath(doc, XP_PLAYER_WINDOW_VISUAL, &(pa->visual), path);
	g_print("window\n");
}

static void 
load_lyric_archive( xmlDocPtr doc, LyricArchive* la, const gchar* path)
{
	read_item_with_xpath(doc, XP_LYRIC_WINDOW, &(la->window), path);
	read_item_with_xpath(doc, XP_LYRIC_WINDOW_LYRIC, &(la->lyric), path);
	read_item_with_xpath(doc, XP_LYRIC_WINDOW_CLOSE, &(la->close), path);
	read_item_with_xpath(doc, XP_LYRIC_WINDOW_ONTOP, &(la->ontop), path);
}

static void 
load_equalizer_archive( xmlDocPtr doc, EqualizerArchive* eqa, const gchar* path)
{
	read_item_with_xpath(doc, XP_EQUALIZER_WINDOW, &(eqa->window), path);
	read_item_with_xpath(doc, XP_EQUALIZER_WINDOW_CLOSE, &(eqa->close), path);
	read_item_with_xpath(doc, XP_EQUALIZER_WINDOW_ENABLED, &(eqa->enabled), path);
	read_item_with_xpath(doc, XP_EQUALIZER_WINDOW_PROFILE, &(eqa->profile), path);
	read_item_with_xpath(doc, XP_EQUALIZER_WINDOW_RESET, &(eqa->reset), path);
	read_item_with_xpath(doc, XP_EQUALIZER_WINDOW_BALANCE, &(eqa->balance), path);
	read_item_with_xpath(doc, XP_EQUALIZER_WINDOW_SURROUND, &(eqa->surround), path);
	read_item_with_xpath(doc, XP_EQUALIZER_WINDOW_PREAMP, &(eqa->preamp), path);
	read_item_with_xpath(doc, XP_EQUALIZER_WINDOW_EQFACTOR, &(eqa->eqfactor), path);
}

static void 
load_playlist_archive( xmlDocPtr doc, PlaylistArchive* pla, const gchar* path)
{
	read_item_with_xpath(doc, XP_PLAYLIST_WINDOW, &(pla->window), path);
	read_item_with_xpath(doc, XP_PLAYLIST_WINDOW_CLOSE, &(pla->close), path);
	read_item_with_xpath(doc, XP_PLAYLIST_WINDOW_TOOLBAR, &(pla->toolbar), path);
	read_item_with_xpath(doc, XP_PLAYLIST_WINDOW_SCROLLBAR, &(pla->scrollbar), path);
	read_item_with_xpath(doc, XP_PLAYLIST_WINDOW_PLAYLIST, &(pla->playlist), path);
}

static void 
load_mini_archive(xmlDocPtr doc, MiniArchive* ma, const gchar* path)
{
	read_item_with_xpath(doc, XP_MINI_WINDOW, &(ma->window), path);
	read_item_with_xpath(doc, XP_MINI_WINDOW_PLAY, &(ma->play), path);
	read_item_with_xpath(doc, XP_MINI_WINDOW_PAUSE, &(ma->pause), path);
	read_item_with_xpath(doc, XP_MINI_WINDOW_PREV, &(ma->prev), path);
	read_item_with_xpath(doc, XP_MINI_WINDOW_NEXT, &(ma->next), path);
	read_item_with_xpath(doc, XP_MINI_WINDOW_STOP, &(ma->stop), path);
	read_item_with_xpath(doc, XP_MINI_WINDOW_LYRIC, &(ma->lyric), path);
	read_item_with_xpath(doc, XP_MINI_WINDOW_ICON, &(ma->icon), path);
	read_item_with_xpath(doc, XP_MINI_WINDOW_MINIMIZE, &(ma->minimize), path);
	read_item_with_xpath(doc, XP_MINI_WINDOW_MINIMODE, &(ma->minimode), path);
	read_item_with_xpath(doc, XP_MINI_WINDOW_EXIT, &(ma->exit), path);
	read_item_with_xpath(doc, XP_MINI_WINDOW_INFO, &(ma->info), path);
}

static gboolean 
load_playlist_attr(const gchar* filename, PlaylistAttr* attr)
{
	xmlDocPtr doc;
	xmlXPathObjectPtr app_result;
	xmlNodeSetPtr nodeset;
	xmlNodePtr cur;
	gchar* value;

	if((filename == NULL) || (attr == NULL))
		return FALSE;

	doc = xmlReadFile(filename, NULL, 256);
	if(doc == NULL)
	{
		fprintf(stderr, "Unable to parse file: %s\n", filename);
		return FALSE;
	}

    app_result = xml_nodeset_get_from_xpath(doc, (const xmlChar *)XP_PLAYLIST_ATTR);
    if (app_result == NULL) 
		return FALSE;

	nodeset = app_result->nodesetval;
	cur = nodeset->nodeTab[0];

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"Font");
	if(value)
		attr->font = value;
	else
		attr->font = NULL;

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"Color_Text");
	if(value)
		gdk_color_parse(value, &(attr->color_text));

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"Color_Hilight");
	if(value)
		gdk_color_parse(value, &(attr->color_hilight));

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"Color_Bkgnd");
	if(value)
		gdk_color_parse(value, &(attr->color_bg));

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"Color_Number");
	if(value)
		gdk_color_parse(value, &(attr->color_number));

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"Color_Duration");
	if(value)
		gdk_color_parse(value, &(attr->color_duration));

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"Color_Select");
	if(value)
		gdk_color_parse(value, &(attr->color_select));

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"Color_Bkgnd2");
	if(value)
		gdk_color_parse(value, &(attr->color_bg2));

	xmlXPathFreeObject(app_result);
	xmlFreeDoc(doc);
	return TRUE;
}

static gboolean 
load_lyric_attr( const gchar* filename, LyricAttr* attr)
{
	xmlDocPtr doc;
	xmlXPathObjectPtr app_result;
	xmlNodeSetPtr nodeset;
	xmlNodePtr cur;
	gchar* value;

	if((filename == NULL) || (attr == NULL))
		return FALSE;

	doc = xmlReadFile(filename, NULL, 256);
	if(doc == NULL)
	{
		fprintf(stderr, "Unable to parse file: %s\n", filename);
		return FALSE;
	}

    app_result = xml_nodeset_get_from_xpath(doc, (const xmlChar *)XP_LYRIC_ATTR);
    if (app_result == NULL) 
		return FALSE;

	nodeset = app_result->nodesetval;
	cur = nodeset->nodeTab[0];

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"Font");
	if(value)
		attr->font = value;
	else
		attr->font = NULL;

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"TextColor");
	if(value)
		gdk_color_parse(value, &(attr->color_text));

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"HilightColor");
	if(value)
		gdk_color_parse(value, &(attr->color_hilight));

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"BkgndColor");
	if(value)
		gdk_color_parse(value, &(attr->color_bg));

	xmlXPathFreeObject(app_result);
	xmlFreeDoc(doc);
	return TRUE;
}

static gboolean 
load_visual_attr(const gchar* filename, VisualAttr* attr)
{
	xmlDocPtr doc;
	xmlXPathObjectPtr app_result;
	xmlNodeSetPtr nodeset;
	xmlNodePtr cur;
	gchar* value;

	if((filename == NULL) || (attr == NULL))
		return FALSE;

	doc = xmlReadFile(filename, NULL, 256);
	if(doc == NULL)
	{
		fprintf(stderr, "Unable to parse file: %s\n", filename);
		return FALSE;
	}

    app_result = xml_nodeset_get_from_xpath(doc, (const xmlChar *)XP_VISUAL_ATTR);
    if (app_result == NULL) 
		return FALSE;

	nodeset = app_result->nodesetval;
	cur = nodeset->nodeTab[0];

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"SpectrumTopColor");
	if(value)
		gdk_color_parse(value, &(attr->color_spectrum_top));

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"SpectrumBtmColor");
	if(value)
		gdk_color_parse(value, &(attr->color_spectrum_bottom));

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"SpectrumMidColor");
	if(value)
		gdk_color_parse(value, &(attr->color_spectrum_mid));

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"SpectrumWidth");
	if(value)
		attr->spectrum_width = value;
	else
		attr->spectrum_width = NULL;

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"BlurSpeed");
	if(value)
		attr->blur_speed = atoi(value);

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"Blur");
	if(value)
		attr->blur = atoi(value);

	value = (gchar *)xmlGetProp(cur, (const xmlChar *)"BlurScopeColor");
	if(value)
		gdk_color_parse(value, &(attr->color_blur_scope));

	xmlXPathFreeObject(app_result);
	xmlFreeDoc(doc);
	return TRUE;
}
static gboolean 
do_load(SkinArchive *ar, const char *filename)
{
	xmlDocPtr doc;

	g_print("do_load start\n");
    doc = xmlReadFile(filename, NULL, 256);	//解析文件   
	g_return_val_if_fail(doc != NULL, FALSE);
	
	g_assert(ar->info != NULL);
	read_info(doc, ar->info);
	g_print("load player\n");
	load_player_archive(doc, ar->player, ar->priv->path);
	g_print("load equ\n");
	load_equalizer_archive(doc, ar->equalizer, ar->priv->path);
	g_print("load lyric\n");
	load_lyric_archive(doc, ar->lyric, ar->priv->path);
	g_print("load playlist\n");
	load_playlist_archive(doc, ar->playlist, ar->priv->path);
	g_print("load mini\n");
	load_mini_archive(doc, ar->mini, ar->priv->path);

	xmlFreeDoc(doc);

	g_print("do_load end\n");
	return TRUE;
}

