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
#ifndef _DIALOG_H
#define _DIALOG_H

void initialize_dialogs(void);
void finalize_dialogs(void);

struct input
{
  const char *label;
  char *text;
  unsigned max_text_len;
};

void input_dialog_set(const char *title, const struct input *inputs, unsigned char count);
void input_dialog_draw(void);
char input_dialog_loop(void);

struct progress
{
  const char *label;
  unsigned count;
  unsigned max;
};

void progress_dialog_set(const char *title, const struct progress *progresses, unsigned char count);
void progress_dialog_draw(void);

void message_dialog_set(const char *title, const char *msg);
void message_dialog_draw(void);
void message_dialog_loop(void);

void yes_no_dialog_set(const char *title, const char *msg);
void yes_no_dialog_draw(void);
char yes_no_dialog_loop(void);

void about_dialog_set(void);
void about_dialog_draw(void);
void about_dialog_loop(void);

#endif
