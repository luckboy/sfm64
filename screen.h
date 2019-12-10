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
#ifndef _SCREEN_H
#define _SCREEN_H

#include <c64.h>

#define SCREEN_COLOR_BACKGROUND         COLOR_GRAY3
#define SCREEN_COLOR_FOREGROUND         COLOR_GRAY1
#define SCREEN_COLOR_CURSOR             COLOR_GRAY2

extern unsigned char screen_width;
extern unsigned char screen_height;

void initialize_screen(void);
void finalize_screen(void);

void screen_clear(void);

#endif
