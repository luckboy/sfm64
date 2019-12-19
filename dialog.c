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
#include <cbm.h>
#include <conio.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include "dialog.h"
#include "screen.h"
#include "util.h"

struct input_dialog
{
  unsigned char width;
  unsigned char height;
  const char *title;
  const struct input *inputs;
  unsigned char input_count;
  unsigned char focus_index;
};

struct progress_dialog
{
  unsigned char width;
  unsigned char height;
  const char *title;
  const struct progress *progresses;
  unsigned char progress_count;
};

struct message_dialog
{
  unsigned char width;
  unsigned char height;
  const char *title;
  const char *message;
  unsigned char focus_index;
};

struct yes_no_dialog
{
  unsigned char width;
  unsigned char height;
  const char *title;
  const char *message;
  unsigned char focus_index;
};

static struct input_dialog input_dialog;
static struct progress_dialog progress_dialog;
static struct message_dialog message_dialog;
static struct yes_no_dialog yes_no_dialog;

void initialize_dialogs(void)
{
  input_dialog.width = 0;
  input_dialog.height = 0;
  input_dialog.title = NULL;
  input_dialog.inputs = NULL;
  input_dialog.input_count = 0;
  input_dialog.focus_index = 0;
  progress_dialog.width = 0;
  progress_dialog.height = 0;
  progress_dialog.title = NULL;
  progress_dialog.progresses = NULL;
  progress_dialog.progress_count = 0;
  message_dialog.width = 0;
  message_dialog.height = 0;
  message_dialog.title = NULL;
  message_dialog.message = NULL;
  message_dialog.focus_index = 0;
  yes_no_dialog.width = 0;
  yes_no_dialog.height = 0;
  yes_no_dialog.title = NULL;
  yes_no_dialog.message = NULL;
  yes_no_dialog.focus_index = 0;
}

void finalize_dialogs(void) {}

static void draw_button(const char *label, char is_focused)
{
  unsigned char len = strlen(label);
  unsigned char i, n = center(6, len);
  revers(!is_focused);
  textcolor(SCREEN_COLOR_FOREGROUND);
  for(i = 0; i < n; i++) {
    cputc(' ');
  }
  cputs(label);
  for(i = 0; i < 6 - n - len; i++) {
    cputc(' ');
  }
  revers(1);
  textcolor(SCREEN_COLOR_CURSOR);
}

static void draw_title(const char *title, unsigned char width)
{
  unsigned char len = strlen(title);
  unsigned char i, n = center(width, len);
  revers(1);
  textcolor(SCREEN_COLOR_CURSOR);
  for(i = 0; i < n; i++) {
    cputc(' ');
  }
  safely_cputs(title);
  for(i = 0; i < width - n - len; i++) {
    cputc(' ');
  }
  revers(0);
  textcolor(SCREEN_COLOR_FOREGROUND);
}

static void draw_label(const char *label, unsigned char width)
{
  unsigned char i, len = strlen(label);
  revers(1);
  textcolor(SCREEN_COLOR_CURSOR);
  cputc(' ');
  safely_cputs(label);
  for(i = 0; i < width - len - 1; i++) {
    cputc(' ');
  }
  revers(0);
  textcolor(SCREEN_COLOR_FOREGROUND);
}

static void draw_text(char *text, unsigned max_text_len, unsigned char width, char is_focused)
{
  unsigned char i, len = strlen(text);
  revers(1);
  textcolor(SCREEN_COLOR_CURSOR);
  cputc(' ');
  revers(0);
  textcolor(SCREEN_COLOR_FOREGROUND);
  safely_cputs(text);
  if(is_focused) {
    revers(1);
    cputc(' ');
    revers(0);
  } else {
    cputc(' ');
  }
  for(i = 0; i < max_text_len + 1 - len - 1; i++) {
    cputc(' ');
  }
  revers(1);
  textcolor(SCREEN_COLOR_CURSOR);
  for(i = 0; i < width - max_text_len - 1 - 1; i++) {
    cputc(' ');
  }
  revers(0);
  textcolor(SCREEN_COLOR_FOREGROUND);
}

static void draw_empty(unsigned char width)
{
  unsigned char i;
  revers(1);
  textcolor(SCREEN_COLOR_CURSOR);
  for(i = 0; i < width; i++) {
    cputc(' ');
  }
  revers(0);
  textcolor(SCREEN_COLOR_FOREGROUND);
}

static void draw_one_button(const char *label, unsigned char width, char is_focused)
{
  unsigned char i, n = center(width, 6);
  revers(1);
  textcolor(SCREEN_COLOR_CURSOR);
  for(i = 0; i < n; i++) {
    cputc(' ');
  }
  draw_button(label, is_focused);
  for(i = 0; i < width - 6 - n; i++) {
    cputc(' ');
  }
  revers(0);
  textcolor(SCREEN_COLOR_FOREGROUND);  
}

static void draw_two_buttons(const char *label1, const char *label2, unsigned char width, char is_focused1, char is_focused2)
{
  unsigned char i, n = center(width, 12 + 2);
  revers(1);
  textcolor(SCREEN_COLOR_CURSOR);
  for(i = 0; i < n; i++) {
    cputc(' ');
  }
  draw_button(label1, is_focused1);
  cputs("  ");
  draw_button(label2, is_focused2);
  for(i = 0; i < width - 12 - 2 - n; i++) {
    cputc(' ');
  }
  revers(0);
  textcolor(SCREEN_COLOR_FOREGROUND);  
}

static void draw_progress(unsigned count, unsigned max, unsigned char width)
{
  unsigned char i;
  revers(1);
  textcolor(SCREEN_COLOR_CURSOR);
  cputc(' ');
  textcolor(SCREEN_COLOR_FOREGROUND);
  for(i = 0; i < count; i++) {
    cputc(' ');
  }
  revers(0);
  for(i = 0; i < max - count; i++) {
    cputc(' ');
  }
  revers(1);
  textcolor(SCREEN_COLOR_CURSOR);
  for(i = 0; i < width - max - 1; i++) {
    cputc(' ');
  }
  revers(0);
  textcolor(SCREEN_COLOR_FOREGROUND);  
}

static void draw_message(const char *msg, unsigned char width)
{ draw_title(msg, width); }

/*
 * An input dialog.
 */

void input_dialog_set(const char *title, const struct input *inputs, unsigned char count)
{
  unsigned char max_width;
  unsigned char i;
  input_dialog.title = title;
  input_dialog.inputs = inputs;
  input_dialog.input_count = count;
  input_dialog.focus_index = 0;
  max_width = even(strlen(input_dialog.title)) + 2;
  for(i = 0; i < input_dialog.input_count; i++) {
    const struct input *input = &(input_dialog.inputs[i]);
    size_t len = strlen(input->label);
    unsigned char even_len = even(len);
    unsigned char even_max_len = even(input->max_text_len + 1);
    max_width = max(max_width, even_len + 2);
    max_width = max(max_width, even_max_len + 2);
  }
  max_width = max(max_width, 12 + 2 + 2);
  input_dialog.width = max_width;
  input_dialog.height = input_dialog.input_count * 3 + 2 + 2;
}

void input_dialog_draw(void)
{
  unsigned char i;
  unsigned char x = center_x(input_dialog.width);
  unsigned char y = center_y(input_dialog.height);
  char is_focused1, is_focused2;
  gotoxy(x, y);
  draw_title(input_dialog.title, input_dialog.width);
  y++;
  for(i = 0; i < input_dialog.input_count; i++, y += 3) {
    const struct input *input = &(input_dialog.inputs[i]);
    char is_focused = (i == input_dialog.focus_index);
    gotoxy(x, y);
    draw_empty(input_dialog.width);
    gotoxy(x , y + 1);
    draw_label(input->label, input_dialog.width);
    gotoxy(x , y + 2);
    draw_text(input->text, input->max_text_len, input_dialog.width, is_focused);
  }
  gotoxy(x, y);
  draw_empty(input_dialog.width);
  y++;
  gotoxy(x, y);
  is_focused1 = (input_dialog.input_count == input_dialog.focus_index);
  is_focused2 = (input_dialog.input_count + 1 == input_dialog.focus_index);
  draw_two_buttons("OK", "Cancel", input_dialog.width, is_focused1, is_focused2);
  y++;
  gotoxy(x, y);
  draw_empty(input_dialog.width);
}

char input_dialog_loop(void)
{
  char is_exit = 0;
  char is_ok = 0;
  while(!is_exit) {
    char c = cgetc();
    unsigned char i = input_dialog.focus_index;
    switch(c) {
    case CH_DEL:
      i = input_dialog.focus_index;
      if(i < input_dialog.input_count) {
        const struct input *input = &(input_dialog.inputs[i]);
        size_t len = strlen(input->text);
        if(len > 0) {
          input->text[len - 1] = 0;
          input_dialog_draw();
        }
      }
      break;
    case CH_CURS_UP:
      if(i > 0) {
        input_dialog.focus_index--;
        input_dialog_draw();
      }
      break;
    case CH_CURS_DOWN:
      if(i < input_dialog.input_count) {
        input_dialog.focus_index++;
        input_dialog_draw();
      }
      break;
    case CH_CURS_LEFT:
      if(i == input_dialog.input_count + 1) {
        input_dialog.focus_index--;
        input_dialog_draw();
      }
      break;
    case CH_CURS_RIGHT:
      if(i == input_dialog.input_count) {
        input_dialog.focus_index++;
        input_dialog_draw();
      }
      break;
    case '\n':
      if(i < input_dialog.input_count) {
        input_dialog.focus_index++;
        input_dialog_draw();
      } else {
        is_exit = 1;
        is_ok = (i == input_dialog.input_count);
      }
      break;
    case CH_STOP:
    case CH_ESC:
      is_exit = 1;
      is_ok = 0;
      break;
    default:
      if(!iscntrl(c)) {
        if(i < input_dialog.input_count) {
          const struct input *input = &(input_dialog.inputs[i]);
          size_t len = strlen(input->text);
          if(len < input_dialog.inputs[i].max_text_len) {
            input->text[len] = c;
            input->text[len + 1] = 0;
            input_dialog_draw();
          }
        } else {
          if(c == ' ') {
            is_exit = 1;
            is_ok = (i == input_dialog.input_count);
          }
        }
      }
      break;
    }
  }
  return is_ok;
}

/*
 * A progress dialog.
 */

void progress_dialog_set(const char *title, const struct progress *progresses, unsigned char count)
{
  unsigned char max_width;
  unsigned char i;
  progress_dialog.title = title;
  progress_dialog.progresses = progresses;
  progress_dialog.progress_count = count;
  max_width = even(strlen(progress_dialog.title)) + 2;
  for(i = 0; i < progress_dialog.progress_count; i++) {
    const struct progress *progress = &(progress_dialog.progresses[i]);
    size_t len = strlen(progress->label);
    unsigned char even_len = even(len);
    unsigned char even_max = even(progress->max);
    max_width = max(max_width, even_len + 2);
    max_width = max(max_width, even_max + 2);
  }
  max_width = max(max_width, 12 + 2 + 2);
  progress_dialog.width = max_width;
  progress_dialog.height = progress_dialog.progress_count * 3 + 2;
}

void progress_dialog_draw(void)
{
  unsigned char i;
  unsigned char x = center_x(progress_dialog.width);
  unsigned char y = center_y(progress_dialog.height);
  gotoxy(x, y);
  draw_title(progress_dialog.title, progress_dialog.width);
  y++;
  for(i = 0; i < progress_dialog.progress_count; i++, y += 3) {
    const struct progress *progress = &(progress_dialog.progresses[i]);
    gotoxy(x, y);
    draw_empty(progress_dialog.width);
    gotoxy(x , y + 1);
    draw_label(progress->label, progress_dialog.width);
    gotoxy(x , y + 2);
    draw_progress(progress->count, progress->max, progress_dialog.width);    
  }
  gotoxy(x, y);
  draw_empty(progress_dialog.width);
}

/*
 * A message dialog.
 */

void message_dialog_set(const char *title, const char *msg)
{
  unsigned char max_width;
  message_dialog.title = title;
  message_dialog.message = msg;
  message_dialog.focus_index = 0;
  max_width = even(strlen(message_dialog.title)) + 2;
  max_width = max(max_width, even(strlen(message_dialog.message)) + 2);
  max_width = max(max_width, 6 + 2);
  message_dialog.width = max_width;
  message_dialog.height = 2 + 2 + 2;
}

void message_dialog_draw(void)
{
  unsigned char x = center_x(message_dialog.width);
  unsigned char y = center_y(message_dialog.height);
  gotoxy(x, y);
  draw_title(message_dialog.title, message_dialog.width);
  y++;
  gotoxy(x, y);
  draw_empty(message_dialog.width);
  y++;
  gotoxy(x, y);
  draw_message(message_dialog.message, message_dialog.width);
  y++;
  gotoxy(x, y);
  draw_empty(message_dialog.width);
  y++;  
  gotoxy(x, y);
  draw_one_button("OK", message_dialog.width, 1);
  y++;
  gotoxy(x, y);
  draw_empty(message_dialog.width);
}

void message_dialog_loop(void)
{
  char is_exit = 0;
  while(!is_exit) {
    switch(cgetc()) {
    case '\n':
    case ' ':
    case CH_STOP:
    case CH_ESC:
      is_exit = 1;
      break;
    }
  }
}

/*
 * An yes/no dialog.
 */

void yes_no_dialog_set(const char *title, const char *msg)
{
  unsigned char max_width;
  yes_no_dialog.title = title;
  yes_no_dialog.message = msg;
  yes_no_dialog.focus_index = 0;
  max_width = even(strlen(yes_no_dialog.title)) + 2;
  max_width = max(max_width, even(strlen(yes_no_dialog.message)) + 2);
  max_width = max(max_width, 12 + 2 + 2);
  yes_no_dialog.width = max_width;
  yes_no_dialog.height = 2 + 2 + 2;
}

void yes_no_dialog_draw(void)
{
  unsigned char x = center_x(yes_no_dialog.width);
  unsigned char y = center_y(yes_no_dialog.height);
  char is_focused1, is_focused2;
  gotoxy(x, y);
  draw_title(yes_no_dialog.title, yes_no_dialog.width);
  y++;
  gotoxy(x, y);
  draw_empty(yes_no_dialog.width);
  y++;
  gotoxy(x, y);
  draw_message(yes_no_dialog.message, yes_no_dialog.width);
  y++;
  gotoxy(x, y);
  draw_empty(yes_no_dialog.width);
  y++;  
  gotoxy(x, y);
  is_focused1 = (0 == yes_no_dialog.focus_index);
  is_focused2 = (1 == yes_no_dialog.focus_index);
  draw_two_buttons("Yes", "no", yes_no_dialog.width, is_focused1, is_focused2);
  y++;
  gotoxy(x, y);
  draw_empty(yes_no_dialog.width);
}

char yes_no_dialog_loop(void)
{
  char is_exit = 0;
  char is_yes = 0;
  while(!is_exit) {
    unsigned char i = yes_no_dialog.focus_index;
    switch(cgetc()) {
    case CH_CURS_LEFT:
      if(i == 1) {
        yes_no_dialog.focus_index--;
        yes_no_dialog_draw();
      }
      break;
    case CH_CURS_RIGHT:
      if(i == 0) {
        yes_no_dialog.focus_index++;
        yes_no_dialog_draw();
      }
      break;
    case '\n':
    case ' ':
      is_exit = 1;
      is_yes = (i == 0);
      break;
    case CH_STOP:
    case CH_ESC:
      is_exit = 1;
      is_yes = 0;
      break;
    }
  }
  return is_yes;
}
