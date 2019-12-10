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
#include "dir_panel.h"
#include "screen.h"
#include "main_menu.h"

int main(void)
{
  initialize_screen();
  initialize_dir_panels();
  main_menu_draw();
  dir_panel_reload(current_dir_panel);
  main_menu_loop();
  finalize_dir_panels();
  finalize_screen();
  return 0;
}
