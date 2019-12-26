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
#include <stddef.h>
#include "file.h"
#include "screen.h"
#include "text.h"
#include "util.h"

struct text
{
  unsigned line_count;
  unsigned max_line_char_count;
  unsigned view_y;
  unsigned view_x;
  char *view_y_ptr;
};

static struct text text;

void initialize_text(void)
{
  text.line_count = 0;
  text.max_line_char_count = 0;
  text.view_y = 0;
  text.view_x = 0;
  text.view_y_ptr = NULL;
}

void finalize_text(void) {}

void text_set(void)
{
  char *s = view_file.content;
  char *end = view_file.content + view_file.size;
  unsigned line_char_count = 0;
  text.line_count = 0;
  text.max_line_char_count = 0;
  while(1) {
    if(s >= end || *s == '\n') {
      text.line_count++;
      text.max_line_char_count = umax(text.max_line_char_count, line_char_count);
      line_char_count = 0;
      if(s >= end) break;
    } else
      line_char_count++;
    s++;
  }
  text.view_y = 0;
  text.view_x = 0;
  text.view_y_ptr = view_file.content;
}

void text_draw(void)
{
  char *s;
  char *end = view_file.content + view_file.size;
  unsigned y, max_y;
  gotoxy(0, 0);
  if(text.view_y + screen_height - 1 <= text.line_count)
    max_y = text.view_y + screen_height - 1;
  else
    max_y = text.line_count;
  s = text.view_y_ptr;
  for(y = text.view_y; y < max_y; y++) {
    unsigned x;
    for(x = 0; x < text.view_x; x++) {
      if(s < end && *s != '\n') s++;
    }
    for(; x < text.view_x + screen_width; x++) {
      if(s < end && *s != '\n') {
        safely_cputc(*s);
        s++;
      } else
        cputc(' ');
    }
    if(y + 1 < max_y) {
      while(s < end && *s != '\n') s++;
      /* \n */
      if(s < end) s++;
    }
  }
  for(; y < text.view_y + screen_height - 1; y++) {
    unsigned screen_x;
    for(screen_x = 0; screen_x < screen_width; screen_x++) {
      cputc(' ');
    }
  }
}

void text_move_view_up(void)
{
  if(text.view_y > 0) {
    text.view_y--;
    if(text.view_y_ptr > view_file.content) {
      /* \n */
      text.view_y_ptr--;
      if(text.view_y_ptr > view_file.content) {
        text.view_y_ptr--;
        while(text.view_y_ptr > view_file.content) {
          if(*(text.view_y_ptr) == '\n') {
            text.view_y_ptr++;
            break;
          }
          text.view_y_ptr--;
        }
      }
    }
    text_draw();
  }
}

void text_move_view_down(void)
{
  if(text.view_y + screen_height - 1 < text.line_count) {
    char *end = view_file.content + view_file.size;
    text.view_y++;
    while(text.view_y_ptr < end && *(text.view_y_ptr) != '\n') {
      text.view_y_ptr++;
    }
    /* \n */
    if(text.view_y_ptr < end) text.view_y_ptr++;
    text_draw();
  }
}

void text_move_view_left(void)
{
  if(text.view_x > 0) {
    text.view_x--;
    text_draw();
  }
}

void text_move_view_right(void)
{
  if(text.view_x + screen_width < text.max_line_char_count) {
    text.view_x++;
    text_draw();
  }
}
