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
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "cmd_channel.h"
#include "dir_panel.h"
#include "screen.h"
#include "util.h"

struct dir_panel dir_panels[DIR_PANEL_MAX];

struct dir_panel *current_dir_panel;

void initialize_dir_panels(void)
{
  unsigned char i;
  for(i = 0; i < DIR_PANEL_MAX; i++) {
    dir_panels[i].device = 8 + i;
    dir_panels[i].status = DIR_PANEL_STATUS_UNLOADED;
    dir_panels[i].has_header_dir_entry = 0;
    dir_panels[i].has_tail_dir_entry = 0;
    dir_panels[i].dir_list = NULL;
    dir_panels[i].dir_list_length = 0;
    dir_panels[i].view_y = 0;
    dir_panels[i].cursor_y = 0;
  }
  current_dir_panel = &dir_panels[0];
}

void finalize_dir_panels(void)
{
  unsigned char i;
  for(i = 0; i < DIR_PANEL_MAX; i++) {
    if(dir_panels[i].dir_list != NULL)
      free(dir_panels[i].dir_list);
  }
}

static void draw_header_dir_entry(struct dir_panel *dir_panel)
{
  cputc(0xb0);
  if(dir_panel->has_header_dir_entry) {
    size_t i, len;
    cprintf("Dev%02d", (unsigned) (dir_panel->device));
    cputc(0x60);
    safely_cputs(dir_panel->header_dir_entry.name);
    len = strlen(dir_panel->header_dir_entry.name);
    for(i = 0; i < 16 - len; i++) {
      cputc(0x60);
    }
    for(i = 0; i < 4; i++) {
      cputc(0x60);
    }
  } else {
    size_t i;
    for(i = 0; i < DIR_PANEL_WIDTH - 2; i++) {
      cputc(0x60);
    }
  }
  cputc(0xae);
}

static void draw_tail_dir_entry(struct dir_panel *dir_panel)
{
  cputc(0xad);
  if(dir_panel->has_tail_dir_entry) {
    size_t i;
    cprintf("%5u ", dir_panel->tail_dir_entry.size);
    cputs("blocks free");
    for(i = 0; i < 16 - 11; i++) {
      cputc(0x60);
    }
    for(i = 0; i < 4; i++) {
      cputc(0x60);
    }
  } else {
    size_t i;
    for(i = 0; i < DIR_PANEL_WIDTH - 2; i++) {
      cputc(0x60);
    }
  }
  cputc(0xbd);
}

static void draw_dir_list_elem(struct dir_panel *dir_panel, unsigned y)
{
  struct dir_list_elem *elem = &(dir_panel->dir_list[y]);
  size_t i, len;
  cputc(0xdd);
  revers(elem->is_selected ^ (y == dir_panel->cursor_y));
  if((y == dir_panel->cursor_y)) textcolor(SCREEN_COLOR_CURSOR);
  cprintf("%5u", elem->entry.size);
  cputc(' ');
  safely_cputs(elem->entry.name);
  len = strlen(elem->entry.name);
  for(i = 0; i < 16 - len; i++) {
    cputc(' ');
  }
  cputc(' ');
  switch(elem->entry.type) {
  case _CBM_T_SEQ:
    cputs("seq");
    break;
  case _CBM_T_PRG:
    cputs("prg");
    break;
  case _CBM_T_USR:
    cputs("usr");
    break;
  case _CBM_T_REL:
    cputs("rel");
    break;
  case _CBM_T_VRP:
    cputs("vrp");
    break;
  case _CBM_T_DEL:
    cputs("del");
    break;
  case _CBM_T_CBM:
    cputs("cbm");
    break;
  case _CBM_T_DIR:
    cputs("dir");
    break;
  case _CBM_T_LNK:
    cputs("lnk");
    break;
  default:
    cputs("oth");
    break;
  }
  revers(0);
  if((y == dir_panel->cursor_y)) textcolor(SCREEN_COLOR_FOREGROUND);
  cputc(0xdd); 
}

static void draw_empty(void)
{
  unsigned char i;
  cputc(0xdd);
  for(i = 0; i < DIR_PANEL_VIEW_WIDTH; i++) {
    cputc(' ');
  }
  cputc(0xdd);
}

void dir_panel_draw(struct dir_panel *dir_panel)
{
  const char *msg = NULL;
  size_t i, len;
  unsigned char screen_x = center_x(DIR_PANEL_WIDTH);
  gotoxy(center_x(DIR_PANEL_WIDTH), 0);
  draw_header_dir_entry(dir_panel);
  if(dir_panel->status == DIR_PANEL_STATUS_LOADED) {
    unsigned char screen_y;
    size_t y, max_y;
    if(dir_panel->view_y + DIR_PANEL_VIEW_HEIGHT <= dir_panel->dir_list_length)
      max_y = dir_panel->view_y + DIR_PANEL_VIEW_HEIGHT;
    else
      max_y = dir_panel->dir_list_length;
    for(screen_y = 1, y = dir_panel->view_y; y < max_y; y++, screen_y++) {
      gotoxy(screen_x, screen_y);
      draw_dir_list_elem(dir_panel, y);
    }
    for(; y < dir_panel->view_y + DIR_PANEL_VIEW_HEIGHT; y++, screen_y++) {
      gotoxy(screen_x, screen_y);
      draw_empty();
    }
  } else {
    unsigned char screen_y;
    size_t y;
    for(screen_y = 1, y = 0; y < DIR_PANEL_VIEW_HEIGHT; y++, screen_y++) {
      gotoxy(screen_x, screen_y);
      draw_empty();
    }
  }
  gotoxy(screen_x, DIR_PANEL_HEIGHT - 2);
  draw_tail_dir_entry(dir_panel);
  if(dir_panel->status == DIR_PANEL_STATUS_LOADING) 
    msg = "Loading directory ...";
  else if(dir_panel->status == DIR_PANEL_STATUS_ERROR)
    msg = dir_panel->error;
  gotoxy(0, DIR_PANEL_HEIGHT - 1);
  for(i = 0; i < screen_width; i++) {
    cputc(' ');
  }
  if(msg != NULL) {
    len = strlen(msg);
    gotoxy(center_x(len), DIR_PANEL_HEIGHT - 1);
    safely_cputs(msg);
  }
}

void dir_panel_reload(struct dir_panel *dir_panel)
{
  static struct cbm_dirent entry;
  unsigned char lfn = 14;
  unsigned char res;
  int res2;
  size_t i, capacity;
  if(dir_panel->dir_list != NULL) {
    free(dir_panel->dir_list);
    dir_panel->dir_list = NULL;
  }
  dir_panel->status = DIR_PANEL_STATUS_LOADING;
  dir_panel->has_header_dir_entry = 0;
  dir_panel->has_tail_dir_entry = 0;
  dir_panel_draw(dir_panel);
  res = cbm_opendir(lfn, dir_panel->device, "$");
  if(res != 0) {
    dir_panel->error = _stroserror(_oserror);
    cbm_closedir(lfn);
    dir_panel->status = DIR_PANEL_STATUS_ERROR;
    dir_panel->has_header_dir_entry = 0;
    dir_panel->has_tail_dir_entry = 0;
    dir_panel->view_y = 0;
    dir_panel->cursor_y = 0;
    dir_panel_draw(dir_panel);
    return;
  }
  res2 = cmd_channel_read(dir_panel->device, &(dir_panel->error), 1);
  if(res2 == -1) {
    dir_panel->error = _stroserror(_oserror);
    cbm_closedir(lfn);
    dir_panel->status = DIR_PANEL_STATUS_ERROR;
    dir_panel->has_header_dir_entry = 0;
    dir_panel->has_tail_dir_entry = 0;
    dir_panel->view_y = 0;
    dir_panel->cursor_y = 0;
    dir_panel_draw(dir_panel);
    return;    
  } else if(res2 > 0) {
    cbm_closedir(lfn);
    cmd_channel_close(dir_panel->device);
    dir_panel->status = DIR_PANEL_STATUS_ERROR;
    dir_panel->has_header_dir_entry = 0;
    dir_panel->has_tail_dir_entry = 0;
    dir_panel->view_y = 0;
    dir_panel->cursor_y = 0;
    dir_panel_draw(dir_panel);
    return;
  }
  capacity = 8;
  dir_panel->dir_list = malloc(sizeof(struct dir_list_elem) * capacity);
  if(dir_panel->dir_list == NULL) {
    dir_panel->error = "Out of memory";
    cbm_closedir(lfn);
    cmd_channel_close(dir_panel->device);
    dir_panel->status = DIR_PANEL_STATUS_ERROR;
    dir_panel->has_header_dir_entry = 0;
    dir_panel->has_tail_dir_entry = 0;
    dir_panel->view_y = 0;
    dir_panel->cursor_y = 0;
    dir_panel_draw(dir_panel);
    return;
  }
  dir_panel->has_header_dir_entry = 0;
  dir_panel->has_tail_dir_entry = 0;
  i = 0;
  while(1) {
    res = cbm_readdir(lfn, &entry);
    if(res == 0) {
      if(entry.type == _CBM_T_HEADER) {
        dir_panel->has_header_dir_entry = 1;
        dir_panel->header_dir_entry = entry;
      } else {
        if(i >= capacity) {
          capacity += 8;
          dir_panel->dir_list = realloc(dir_panel->dir_list, sizeof(struct dir_list_elem) * capacity);
          if(dir_panel->dir_list == NULL) {
            dir_panel->error = "Out of memory";
            cbm_closedir(lfn);
            cmd_channel_close(dir_panel->device);
            dir_panel->status = DIR_PANEL_STATUS_ERROR;
            dir_panel->has_header_dir_entry = 0;
            dir_panel->has_tail_dir_entry = 0;
            dir_panel->view_y = 0;
            dir_panel->cursor_y = 0;
            dir_panel_draw(dir_panel);
            return;
          }
        }
        dir_panel->dir_list[i].is_selected = 0;
        dir_panel->dir_list[i].entry = entry;
        i++;
      }
    } else if(res == 2) {
      dir_panel->has_tail_dir_entry = 1;
      dir_panel->tail_dir_entry = entry;
    } else
      break;
  }
  dir_panel->dir_list_length = i;
  cbm_close(lfn);
  cmd_channel_close(dir_panel->device);
  dir_panel->status = DIR_PANEL_STATUS_LOADED;
  dir_panel->view_y = 0;
  dir_panel->cursor_y = 0;
  dir_panel_draw(dir_panel);
}

char dir_panel_is_loaded(struct dir_panel *dir_panel)
{ return dir_panel->status == DIR_PANEL_STATUS_LOADED || dir_panel->status == DIR_PANEL_STATUS_ERROR; }

void dir_panel_move_cursor_up(struct dir_panel *dir_panel)
{
  if(dir_panel->cursor_y > 0) {
    dir_panel->cursor_y--;
    if(dir_panel->cursor_y < dir_panel->view_y) dir_panel->view_y--;
    dir_panel_draw(dir_panel);
  }
}

void dir_panel_move_cursor_down(struct dir_panel *dir_panel)
{
  if(dir_panel->dir_list_length == 0) return;
  if(dir_panel->cursor_y < dir_panel->dir_list_length - 1) {
    dir_panel->cursor_y++;
    if(dir_panel->cursor_y > dir_panel->view_y + DIR_PANEL_VIEW_HEIGHT - 1) dir_panel->view_y++;
    dir_panel_draw(dir_panel);
  }
}

void dir_panel_select_or_unselect(struct dir_panel *dir_panel)
{
  if(dir_panel->dir_list_length == 0) return;
  dir_panel->dir_list[dir_panel->cursor_y].is_selected ^= 1;
  dir_panel_draw(dir_panel);
}
