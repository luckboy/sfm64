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
#include <string.h>
#include "dialog.h"
#include "screen.h"
#include "text.h"
#include "util.h"
#include "view_menu.h"

void view_menu_draw(void)
{
  char *menu = "A-About Q-Quit                          ";
  size_t len = strlen(menu);
  gotoxy(center_x(len), screen_height - VIEW_MENU_HEIGHT);
  cputs(menu);
}

static void redraw()
{
  screen_clear();
  view_menu_draw();
  text_draw();
}

void view_menu_loop(void)
{
  char is_exit = 0;
  while(!is_exit) {
    switch(cgetc()) {
    case CH_CURS_UP:
      text_move_view_up();
      break;
    case CH_CURS_DOWN:
      text_move_view_down();
      break;
    case CH_CURS_LEFT:
      text_move_view_left();
      break;
    case CH_CURS_RIGHT:
      text_move_view_right();
      break;
    case 'a':
      about_dialog_set();
      about_dialog_draw();
      about_dialog_loop();
      redraw();
      break;
    case CH_STOP:
    case CH_ESC:
    case 'q':
      is_exit = 1;
      break;
    }
  }
}
