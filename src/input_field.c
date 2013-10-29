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
#include <stdlib.h>

struct utf8_char {
	unsigned char *str;
	struct utf8_char *next;
	struct utf8_char *previous;
};

int valid_args(int y, int x, int len);
int string_to_list(unsigned char *str, struct utf8_char *cur_char);
int insert_new_utf8_char(struct utf8_char *cur_char);
int set_start_i(struct utf8_char *cur_char, int len);
void print_string(int y,
                  int x,
                  struct utf8_char *cur_char,
                  int len,
                  int start_i,
                  int password);
void free_previous_utf8_char(struct utf8_char *cur_char);
int cur_i(struct utf8_char *cur_char);
int build_string(struct utf8_char *cur_char, unsigned char **str);
void free_utf8_chars(struct utf8_char *cur_char);

/**
 * Creates a field for string input with the width specified by len.
 * The fields leftmost character will be located at the coordinates
 * specified by y and x.
 * Resulting string will be stored in the string that str points to.
 * If str is pointing to an already existing string, that string will be
 * printed as the default value in the field.
 *
 * Return values:
 *     0 - Exit OK
 *     1 - Memory allocation error
 *     2 - Invalid arguments
 */
int input_field(int y, int x, int len, unsigned char **str, int password)
{
	unsigned char utf8_index;
	unsigned char bitmask;
	unsigned char *tmpptr;

	if (!valid_args(y, x, len)) {
		return 2;
	}

	struct utf8_char *cur_char = malloc(sizeof(struct utf8_char));

	if (!cur_char) {
		return 1;
	}

	cur_char->str = (unsigned char *)" ";
	cur_char->next = NULL;
	cur_char->previous = NULL;

	if (*str && **str) {
		string_to_list(*str, cur_char);
	}

	int ch, start_i = set_start_i(cur_char, len);

	do {
		print_string(y, x, cur_char, len, start_i, password);
		ch = getch();

		if (ch == KEY_BACKSPACE) {
			if (cur_char->previous) {
				free_previous_utf8_char(cur_char);

				if (start_i > 0) {
					--start_i;
				}
			}
		} else if (ch == KEY_LEFT) {
			if (cur_char->previous) {
				cur_char = cur_char->previous;

				if (start_i && cur_i(cur_char) == start_i) {
					--start_i;
				}
			}
		} else if (ch == KEY_RIGHT) {
			if (cur_char->next) {
				cur_char = cur_char->next;

				if ((cur_i(cur_char) == start_i + len - 1
				     && cur_char->next
				     && cur_char->next->next)
				    || cur_i(cur_char) == start_i + len) {
					++start_i;
				}
			}
		} else if (ch == KEY_IC) {
			;
		} else if (ch == KEY_DC) {
			if (cur_char->next) {
				cur_char = cur_char->next;
				free_previous_utf8_char(cur_char);

				if (start_i > 0
				    && cur_i(cur_char) != start_i + len - 1
				    && !(cur_i(cur_char)
				           == start_i + len - 2
				         && cur_char->next
				         && cur_char->next->next)) {
					--start_i;
				}
			}
		} else if (ch == KEY_HOME) {
			while (cur_char->previous) {
				cur_char = cur_char->previous;
			}

			start_i = 0;
		} else if (ch == KEY_END) {
			while (cur_char->next) {
				cur_char = cur_char->next;
			}

			start_i = set_start_i(cur_char, len);
		} else if (ch == '\t') {
			;
		} else if (ch == '\n' || ch == '\r' || ch == KEY_ENTER) {
			start_i = 0;
		} else {
			if (insert_new_utf8_char(cur_char)) {
				; /* Memory allocation error */
			}

			cur_char->previous->str = malloc(2);
			cur_char->previous->str[0] = ch;
			utf8_index = 1;

			if (cur_char->previous->str[0] & 0x80) {
				bitmask = 0x40;

				while (cur_char->previous->str[0] & bitmask) {
					tmpptr = realloc(
					             cur_char->previous->str,
					             utf8_index + 2);

					if (!tmpptr) {
						; /* Memory allocation error */
					}

					cur_char->previous->str = tmpptr;
					cur_char->previous->str[utf8_index] =
					    getch();
					++utf8_index;
					bitmask >>= 1;
				}
			}

			cur_char->previous->str[utf8_index] = '\0';

			if (cur_i(cur_char) >= start_i + len
			    || (cur_i(cur_char) == start_i + len - 1
			        && cur_char->next && cur_char->next->next)) {
				++start_i;
			}
		}
	} while (ch != '\n' && ch != '\r' && ch != KEY_ENTER);

	print_string(y, x, cur_char, len, start_i, password);
	build_string(cur_char, str);
	free_utf8_chars(cur_char);

	return 0;
}

/**
 * Validates the arguments of function input_field.
 *
 * Return values:
 *     0 - Arguments not valid
 *     1 - Arguments valid
 */
int valid_args(int y, int x, int len)
{
	int maxy, maxx;

	getmaxyx(stdscr, maxy, maxx);

	if (y < 0 || y >= maxy
	    || len < 3 || len > maxx
	    || x < 0 || x > maxx - len) {
		return 0;
	}

	return 1;
}

/**
 * Inserts each character in str to a new node in the utf8_char list.
 *
 * Return values:
 *     0 - Exit OK
 *     1 - Memory allocation error
 */
int string_to_list(unsigned char *str, struct utf8_char *cur_char)
{
	unsigned char utf8_index;
	unsigned char bitmask;
	unsigned char *tmpptr;

	while (*str) {
		if (insert_new_utf8_char(cur_char)) {
			return 1;
		}

		cur_char->previous->str = malloc(2);
		cur_char->previous->str[0] = *str;
		++str;
		utf8_index = 1;

		if (cur_char->previous->str[0] & 0x80) {
			bitmask = 0x40;

			while (cur_char->previous->str[0] & bitmask) {
				tmpptr = realloc(cur_char->previous->str,
				                 utf8_index + 2);

				if (!tmpptr) {
					return 1;
				}

				cur_char->previous->str = tmpptr;
				cur_char->previous->str[utf8_index] = *str;
				++str;
				++utf8_index;
				bitmask >>= 1;
			}
		}

		cur_char->previous->str[utf8_index] = '\0';
	}

	return 0;
}

/**
 * Inserts new node in the utf8_char list, before cur_char.
 *
 * Return values:
 *     0 - Exit OK
 *     1 - Memory allocation error
 *     2 - cur_char not allocated
 */
int insert_new_utf8_char(struct utf8_char *cur_char)
{
	if (!cur_char) {
		return 2;
	}

	struct utf8_char *tmp_char = malloc(sizeof(struct utf8_char));

	if (!tmp_char) {
		return 1;
	}

	tmp_char->next = cur_char;
	tmp_char->previous = cur_char->previous;

	if (cur_char->previous) {
		cur_char->previous->next = tmp_char;
	}

	cur_char->previous = tmp_char;

	return 0;
}

/**
 * Returns the initial value of start_i,
 * the index of the first character to be printed.
 */
int set_start_i(struct utf8_char *cur_char, int len)
{
	int str_len = 1;

	while (cur_char->previous) {
		cur_char = cur_char->previous;
	}

	while (cur_char->next) {
		++str_len;
		cur_char = cur_char->next;
	}

	if (str_len > len) {
		return (str_len - len);
	} else {
		return 0;
	}
}

void print_string(int y,
                  int x,
                  struct utf8_char *cur_char,
                  int len,
                  int start_i,
                  int password)
{
	int str_i = 0, cur_x, tmp_start_i = start_i;
	struct utf8_char *tmp_char = cur_char;

	while (tmp_char->previous) {
		tmp_char = tmp_char->previous;
	}

	while (start_i) {
		tmp_char = tmp_char->next;
		--start_i;
	}

	while (str_i < len && tmp_char) {
		if (password && tmp_char->next)
			mvprintw(y, x + str_i, "\xE2\x80\xA2");
		else
			mvprintw(y, x + str_i, "%s", tmp_char->str);

		if (tmp_char == cur_char) {
			cur_x = x + str_i;
		}

		++str_i;
		tmp_char = tmp_char->next;
	}

	if (tmp_char && tmp_char->next) {
		mvprintw(y, x + len - 1, "\xc2\xbb");
	}

	if (tmp_start_i > 0) {
		mvprintw(y, x, "\xc2\xab"); /* left-pointing double */
	}                                   /* angle quotation mark */

	move(y, cur_x);
}

void free_previous_utf8_char(struct utf8_char *cur_char)
{
	struct utf8_char *tmp_char = cur_char->previous;

	if (tmp_char->previous) {
		cur_char->previous = tmp_char->previous;
		tmp_char->previous->next = cur_char;
	} else {
		cur_char->previous = NULL;
	}

	if (tmp_char->str) {
		free(tmp_char->str);
	}

	free(tmp_char);
}

int cur_i(struct utf8_char *cur_char)
{
	int i = 0;
	struct utf8_char *tmp_char = cur_char;

	while (tmp_char->previous) {
		tmp_char = tmp_char->previous;
	}

	while (tmp_char != cur_char) {
		++i;
		tmp_char = tmp_char->next;
	}

	return i;
}

int build_string(struct utf8_char *cur_char, unsigned char **str)
{
	int str_i = 0, utf8_index;

	while (cur_char->previous) {
		cur_char = cur_char->previous;
	}

	if (*str)
		free(*str);

	*str = malloc(1);

	while (cur_char->next) {
		utf8_index = 0;

		while (cur_char->str[utf8_index]) {
			*str = realloc(*str, str_i + 2);
			*(*str + str_i) = cur_char->str[utf8_index];
			++utf8_index;
			++str_i;
		}

		cur_char = cur_char->next;
	}

	*(*str + str_i) = '\0';

	return 0;
}

/**
 * Frees every node the utf8_char list and their string member.
 */
void free_utf8_chars(struct utf8_char *cur_char)
{
	while (cur_char->previous) {
		cur_char = cur_char->previous;
	}

	while (cur_char->next) {
		if (cur_char->str) {
			free(cur_char->str);
		}

		cur_char = cur_char->next;
		free(cur_char->previous);
	}

	free(cur_char);
}
