/* Copyright (C) 2013 Max Wällstedt
 * This file is part of NSpot.
 *
 * NSpot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NSpot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NSpot.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <curses.h>
#include <libspotify/api.h>

#include "input_field.h"

#include <stdlib.h>

void draw_box(int y, int x, int height, int width);

void nspot_login()
{
	unsigned char *username = NULL;
	unsigned char *password = NULL;
	const char *blob = NULL;
	int i;

	erase();
	draw_box(2, 4, 12, 37);
	mvprintw(3, 6, "Enter Spotify account or Facebook");
	mvprintw(4, 7, "details or leave blank to login");
	mvprintw(5, 11, "with stored credentials");

	draw_box(7, 6, 3, 33);
	draw_box(10, 6, 3, 33);
	mvprintw(11, 7, "Password");

	curs_set(1);
	input_field(8, 7, 31, &username, 0);
	curs_set(0);

	if (*username) {
		for (i = 0; i < 8; ++i)
			mvaddch(11, 7 + i, ' ');

		curs_set(1);
		input_field(11, 7, 31, &password, 1);
		curs_set(0);

		if (!*password) {
			free(password);
			password = NULL;
		}
	} else {
		free(username);
		username = NULL;
	}

	/* Do login checks here and stuff */

	if (username)
		free(username);
	if (password)
		free(password);
}

void draw_box(int y, int x, int height, int width)
{
	int i;

	mvprintw(y, x, "\xE2\x94\x8C");

	for (i = 1; i <= width - 2; ++i)
		mvprintw(y, x + i, "\xE2\x94\x80");

	mvprintw(y, x + width - 1, "\xE2\x94\x90");

	for (i = 1; i <= height - 2; ++i) {
		mvprintw(y + i, x, "\xE2\x94\x82");
		mvprintw(y + i, x + width - 1, "\xE2\x94\x82");
	}

	mvprintw(y + height - 1, x, "\xE2\x94\x94");

	for (i = 1; i <= width - 2; ++i)
		mvprintw(y + height - 1, x + i, "\xE2\x94\x80");

	mvprintw(y + height - 1, x + width - 1, "\xE2\x94\x98");
}
