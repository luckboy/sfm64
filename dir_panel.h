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
#ifndef _DIR_PANEL_H
#define _DIR_PANEL_H

#include <cbm.h>

#define DIR_PANEL_WIDTH                 (16 + 2 + 5 + 3 + 2)
#define DIR_PANEL_HEIGHT                (25 - 3)
#define DIR_PANEL_VIEW_WIDTH            (DIR_PANEL_WIDTH - 2)
#define DIR_PANEL_VIEW_HEIGHT           (DIR_PANEL_HEIGHT - 3)

#define DIR_PANEL_MAX                   4

#define DIR_PANEL_STATUS_UNLOADED       0
#define DIR_PANEL_STATUS_LOADING        1
#define DIR_PANEL_STATUS_LOADED         2
#define DIR_PANEL_STATUS_ERROR          3

struct dir_list_elem
{
  char is_selected;
  struct cbm_dirent entry;
};

struct dir_panel
{
  unsigned char device;
  unsigned char status;
  const char *error;
  char has_header_dir_entry;
  struct cbm_dirent header_dir_entry;
  char has_tail_dir_entry;
  struct cbm_dirent tail_dir_entry;
  struct dir_list_elem *dir_list;
  unsigned dir_list_length;
  unsigned view_y;
  unsigned cursor_y;
};

extern struct dir_panel dir_panels[DIR_PANEL_MAX];

extern struct dir_panel *current_dir_panel;

void initialize_dir_panels(void);
void finalize_dir_panels(void);

void dir_panel_draw(struct dir_panel *dir_panel);
void dir_panel_reload(struct dir_panel *dir_panel);
char dir_panel_is_loaded(struct dir_panel *dir_panel);
void dir_panel_move_cursor_up(struct dir_panel *dir_panel);
void dir_panel_move_cursor_down(struct dir_panel *dir_panel);
void dir_panel_select_or_unselect(struct dir_panel *dir_panel);

#endif
