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
#ifndef _UTIL_H
#define _UTIL_H

int max(int x, int y);
int min(int x, int y);
unsigned char center_x(unsigned char width);
unsigned char center_y(unsigned char height);
unsigned char center(unsigned char n, unsigned char i);
unsigned char even(unsigned char i);
void safely_cputc(char c);
void safely_cputs(const char *s);

#endif
