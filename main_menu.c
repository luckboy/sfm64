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
#include <string.h>
#include "dir_panel.h"
#include "main_menu.h"
#include "screen.h"
#include "util.h"

void main_menu_draw(void)
{
  static char *menu[MAIN_MENU_HEIGHT] = {
    "8-8  9-9  R-Reload dir C-Copy N-Rename  ",
    "0-10 1-11 D-Delete L-Load S-Save F-Free ",
    "          V-View Q-Quit                 "
  };
  unsigned char i;
  for(i = 0; i < MAIN_MENU_HEIGHT; i++) {
    size_t len = strlen(menu[i]);
    gotoxy(center_x(len), screen_height - MAIN_MENU_HEIGHT + i);
    cputs(menu[i]);
  }
}

void main_menu_loop(void)
{
  unsigned char is_exit = 0;
  while(!is_exit) {
    switch(cgetc()) {
    case CH_CURS_UP:
      dir_panel_move_cursor_up(current_dir_panel);
      break;
    case CH_CURS_DOWN:
      dir_panel_move_cursor_down(current_dir_panel);
      break;
    case ' ':
      dir_panel_select_or_unselect(current_dir_panel);
      break;
    case '8':
      current_dir_panel = &dir_panels[0];
      if(!dir_panel_is_loaded(current_dir_panel))
        dir_panel_reload(current_dir_panel);
      else
        dir_panel_draw(current_dir_panel);
      break;
    case '9':
      current_dir_panel = &dir_panels[1];
      if(!dir_panel_is_loaded(current_dir_panel))
        dir_panel_reload(current_dir_panel);
      else
        dir_panel_draw(current_dir_panel);
      break;
    case '0':
      current_dir_panel = &dir_panels[2];
      if(!dir_panel_is_loaded(current_dir_panel))
        dir_panel_reload(current_dir_panel);
      else
        dir_panel_draw(current_dir_panel);
      break;
    case '1':
      current_dir_panel = &dir_panels[3];
      if(!dir_panel_is_loaded(current_dir_panel))
        dir_panel_reload(current_dir_panel);
      else
        dir_panel_draw(current_dir_panel);
      break;
    case 'r':
      dir_panel_reload(current_dir_panel);
      break;
    case 'q':
      is_exit = 1;
      break;
    }
  }
}
