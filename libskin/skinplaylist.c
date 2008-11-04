/* vi: set sw=4 ts=4: */
/*
 * skinplaylist.c
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

/**
 * 想法：由两个tree组成，一个是rlist，播放列表名称的列表，一个是llist，
 * 与rlist中选中项对应的歌曲列表
 *
 * 中间是一个杆，可以左右拉，从而改变rlist和llist的大小
 * 用一个 GtkHPanel?
 */
#include "skinplaylist.h"
