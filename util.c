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
#include <ctype.h>
#include <string.h>
#include "screen.h"
#include "util.h"

int max(int x, int y)
{ return x < y ? y : x; } 

int min(int x, int y)
{ return x < y ? x : y; }

unsigned umax(unsigned x, unsigned y)
{ return x < y ? y : x; } 

unsigned umin(unsigned x, unsigned y)
{ return x < y ? x : y; }

unsigned char center_x(unsigned char width)
{ return (screen_width >> 1) - ((width + 1) >> 1); }

unsigned char center_y(unsigned char height)
{ return (screen_height >> 1) - ((height + 1) >> 1); }

unsigned char center(unsigned char n, unsigned char i)
{ return (n >> 1) - ((i + 1) >> 1); }

unsigned char even(unsigned char i)
{ return ((i + 1) >> 1) << 1; }

void safely_cputc(char c)
{
  if(!iscntrl(c))
    cputc(c);
  else
    cputc('.');
}

void safely_cputs(const char *s)
{
  while(*s != 0) {
    safely_cputc(*s);
    s++;
  }
}

char check_file_name(const char *file_name)
{
  const char *s = file_name;
  while(*s) {
    if(strchr("*:=?@", *s) != NULL) return 0;
    s++;
  }
  return 1;
}
