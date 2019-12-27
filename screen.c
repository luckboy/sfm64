/*
 * Simple file manager for Commodore 64.
 * Copyright (C) 2019 Łukasz Szpakowski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <conio.h>
#include "screen.h"

unsigned char screen_width;
unsigned char screen_height;

static unsigned char saved_border_color;
static unsigned char saved_bg_color;
static unsigned char saved_fg_color;

void initialize_screen(void)
{
  saved_border_color = bordercolor(SCREEN_COLOR_BACKGROUND);
  saved_bg_color = bgcolor(SCREEN_COLOR_BACKGROUND);
  saved_fg_color = textcolor(SCREEN_COLOR_FOREGROUND);
  clrscr();
  screensize(&screen_width, &screen_height);
}

void finalize_screen(void)
{
  bordercolor(saved_border_color);
  bgcolor(saved_bg_color);
  textcolor(saved_fg_color);
  clrscr();
}

void screen_clear(void)
{ clrscr(); }
