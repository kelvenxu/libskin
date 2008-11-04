/* vi: set sw=4 ts=4: */
/*
 * test_skin_archive.c
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

#include <libskin/skinarchive.h>


int main()
{
	SkinArchive *ar;
	gboolean re;

	g_type_init();
	ar = skin_archive_new();

	g_return_val_if_fail(SKIN_IS_ARCHIVE(ar), -1);

	re = skin_archive_load(ar, "tests/tt.zip");
	if(re)
	{
		printf("archive Information: \n");
		printf("\tname: %s\n", ar->info->name);
		printf("\tauthor: %s\n", ar->info->author);
		printf("\temail: %s\n", ar->info->email);
	}

	return 0;
}
