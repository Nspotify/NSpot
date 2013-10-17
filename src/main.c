/* Copyright (C) 2013 max
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

int main()
{
	initscr();
	curs_set(0);

	mvprintw(4, 0,
"          IIIIIIIIII\n"
"        IIIIIIIIIIIIII\n"
"       IIIIIIIIIIIIIIII\n"
"      IIIIIIIIIIIIIIIIII      MMM                         MM  MMM\n"
"     III              III   MM   MM                   MM      MM\n"
"     IIIIIIIIIIIIIIII  II   MM      MMMMMM     MMMM  MMMM MM MMMMMM     MM\n"
"     IIII           IIIII    MMMMM  MM    MM MM    MM MM  MM  MM  MM   MM\n"
"     IIIIIIIIIIIIII  IIII        MM MM    MM MM    MM MM  MM  MM   MM MM\n"
"     IIII          IIIIII   MM   MM MM    MM MM    MM MM  MM  MM    MMM\n"
"      IIIIIIIIIIII  IIII      MMM   MMMMMM     MMMM   MMM MM  MM    MM\n"
"       IIIIIIIIIIIIIIII             MM                            MMM\n"
"        IIIIIIIIIIIIII\n"
"          IIIIIIIIII\n");

	printw("\n\n\t\t\t\t\tPress any key to quit");

	getch();
	endwin();

	return 0;
}
