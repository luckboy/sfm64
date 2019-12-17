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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd_channel.h"
#include "dialog.h"
#include "dir_panel.h"
#include "main_menu.h"
#include "screen.h"
#include "util.h"

#define BUFFER_SIZE                     256
#define PROGRESS_MAX                    18

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

static void redraw(void)
{
  screen_clear();
  main_menu_draw();
  dir_panel_draw(current_dir_panel);
}

static char check_file_types_for_copy(void)
{
  unsigned i;
  for(i = 0; i < current_dir_panel->selected_elem_index_count; i++) {
    unsigned j = current_dir_panel->selected_elem_indices[i];
    unsigned char file_type = current_dir_panel->dir_list[j].entry.type;
    if(file_type != _CBM_T_SEQ && file_type != _CBM_T_PRG && file_type != _CBM_T_USR) return 0;
  }
  return 1;
}

static int str_to_file_type_for_copy(const char *s)
{
  if(strcmp(s, "s") == 0 || strcmp(s, "seq") == 0)
    return _CBM_T_SEQ;
  else if(strcmp(s, "p") == 0 || strcmp(s, "prg") == 0)
    return _CBM_T_PRG;
  else if(strcmp(s, "u") == 0 || strcmp(s, "usr") == 0)
    return _CBM_T_USR;
  else if(*s == 0)
    return -1;
  else
    return -2;
}

static char *file_type_to_str_for_copy(unsigned char file_type)
{
  switch(file_type) {
  case _CBM_T_SEQ:
    return "s";
  case _CBM_T_USR:
    return "u";
  default:
    return "p";
  }
}

static char *file_type_to_str_for_copy2(int file_type, unsigned char default_file_type)
{
  if(file_type == -1)
    return file_type_to_str_for_copy(default_file_type);
  else
    return file_type_to_str_for_copy(file_type);
}

static char check_prefix_and_suffix_length(const char *prefix, const char *suffix)
{
  unsigned i;
  size_t prefix_len = strlen(prefix);
  size_t suffix_len = strlen(suffix);
  for(i = 0; i < current_dir_panel->selected_elem_index_count; i++) {
    unsigned j = current_dir_panel->selected_elem_indices[i];
    size_t name_len = strlen(current_dir_panel->dir_list[j].entry.name);
    if(prefix_len + name_len + suffix_len > 16) return 0;
  }
  return 1;
}

static int delete_file(unsigned char device, const char *file_name, const char **msg)
{
  static char buf[16 +2 + 1];
  int res;
  sprintf(buf, "s:%s", file_name);
  res = cmd_channel_write(device, buf, 1);
  if(res == -1) return -1;
  res = cmd_channel_read(device, msg, 0);
  if(res == -1) return -1;
  cmd_channel_close(device);
  return res;
}

static void reload_or_set_status_to_unloaded(unsigned char device)
{
  if(current_dir_panel->device == device)
    dir_panel_reload(current_dir_panel);
  else
    dir_panel_set_status_to_unloaded(&dir_panels[device - 8]);
}

static void copy_files(void)
{
  static char dst_file_name[17];
  static char dst_prefix[17];
  static char dst_suffix[17];
  static char dst_device_buf[17];
  static char dst_file_type_buf[17];
  static char file_name_with_colon[18];
  static struct input inputs_for_one_file[3] = {
    {
      "Dest device:",
      dst_device_buf,
      16
    },
    {
      "Dest file name:",
      dst_file_name,
      16
    },
    {
      "Dest file type:",
      dst_file_type_buf,
      16
    }
  };
  static struct input inputs_for_many_files[4] = {
    {
      "Dest device:",
      dst_device_buf,
      16
    },
    {
      "Dest prefix:",
      dst_prefix,
      16
    },
    {
      "Dest suffix:",
      dst_suffix,
      16
    },
    {
      "Dest file type:",
      dst_file_type_buf,
      16
    }
  };
  static struct progress progresses[2] = {
    {
      file_name_with_colon,
      0,
      PROGRESS_MAX
    },
    {
      "Copying files:",
      0,
      PROGRESS_MAX
    }
  };
  static char src_cbm_file_name[16 + 3 + 1 + 1 + 1];
  static char dst_cbm_file_name[16 + 3 + 1 + 1 + 1];
  static char buf[BUFFER_SIZE];
  unsigned char dst_device;
  int dst_file_type;
  char are_many_files;
  unsigned *selected_elem_indices;
  unsigned selected_elem_index_count;
  unsigned i;
  selected_elem_indices = dir_panel_selected_elem_indices(current_dir_panel, &selected_elem_index_count);
  if(selected_elem_indices == NULL) {
    message_dialog_set("Error", "Out of memory");
    message_dialog_draw();
    message_dialog_loop();
    redraw();
    return;
  }
  if(selected_elem_index_count == 0) {
    message_dialog_set("No files", "No selected files");
    message_dialog_draw();
    message_dialog_loop();
    redraw();
    return;
  }  
  if(!check_file_types_for_copy()) {
    message_dialog_set("Not support", "Not support for file type");
    message_dialog_draw();
    message_dialog_loop();
    redraw();
    return;
  }
  are_many_files = (selected_elem_index_count > 1);
  sprintf(dst_device_buf, "%u", (unsigned) (current_dir_panel->device));
  if(are_many_files) {
    dst_prefix[0] = 0;
    dst_suffix[0] = 0;
  } else {
    unsigned i = selected_elem_indices[0];
    strcpy(dst_file_name, current_dir_panel->dir_list[i].entry.name);
  }
  dst_file_type_buf[0] = 0;
  while(1) {
    if(are_many_files)
      input_dialog_set("Copy", inputs_for_many_files, 4);
    else
      input_dialog_set("Copy", inputs_for_one_file, 3);
    input_dialog_draw();
    if(!input_dialog_loop()) {
      redraw();
      return;
    }
    redraw();
    dst_device = atoi(dst_device_buf);
    if(dst_device < 8 || dst_device > 11) {
      message_dialog_set("Field", "Incorrect dest device");
      message_dialog_draw();
      message_dialog_loop();
      redraw();
      continue;
    }
    if(are_many_files) {
      if(!check_prefix_and_suffix_length(dst_prefix, dst_suffix)) {
        message_dialog_set("Field", "Dest prefix or dest suffix is too long");
        message_dialog_draw();
        message_dialog_loop();
        redraw();
        continue;
      }
      if(!check_file_name(dst_prefix)) {
        message_dialog_set("Field", "Incorrect dest prefix");
        message_dialog_draw();
        message_dialog_loop();
        redraw();
        continue;
      }
      if(!check_file_name(dst_suffix)) {
        message_dialog_set("Field", "Incorrect dest suffix");
        message_dialog_draw();
        message_dialog_loop();
        redraw();
        continue;
      }
    } else {
      if(!check_file_name(dst_file_name)) {
        message_dialog_set("Field", "Incorrect dest file name");
        message_dialog_draw();
        message_dialog_loop();
        redraw();
        continue;
      }
    }
    dst_file_type = str_to_file_type_for_copy(dst_file_type_buf);
    if(dst_file_type == -2) {
      message_dialog_set("Field", "Incorrect dest file type");
      message_dialog_draw();
      message_dialog_loop();
      redraw();
      continue;
    }
    if(current_dir_panel->device == dst_device && 
      (are_many_files ?
        dst_prefix[0] == 0 && dst_suffix[0] == 0 :
        strcmp(current_dir_panel->dir_list[selected_elem_indices[0]].entry.name, dst_file_name) == 0)) {
      message_dialog_set("Field", "Can't copy to same files");
      message_dialog_draw();
      message_dialog_loop();
      redraw();
      continue;
    }
    break;
  }
  progresses[0].count = 0;
  progresses[1].count = 0;
  progress_dialog_set("Copying", progresses, 2);
  for(i = 0; i < selected_elem_index_count; i++) {
    unsigned j = selected_elem_indices[i];
    unsigned char src_device = current_dir_panel->device;
    char *src_file_name = current_dir_panel->dir_list[j].entry.name;
    unsigned char src_file_type = current_dir_panel->dir_list[j].entry.type;
    unsigned size = current_dir_panel->dir_list[j].entry.size;
    unsigned char res;
    unsigned char src_lfn = 14, dst_lfn = 15;
    unsigned blocks;
    int res2, res3;
    const char *error;
    char is_stop;
    sprintf(file_name_with_colon, "%s:", src_file_name);
    if(size != 0)
      progresses[0].count = 0;
    else
      progresses[0].count = PROGRESS_MAX;
    progress_dialog_draw();
    sprintf(src_cbm_file_name, "%s,%s,r", src_file_name, file_type_to_str_for_copy(src_file_type));
    if(are_many_files) {
      dst_file_name[0] = 0;
      strcat(dst_file_name, dst_prefix);
      strcat(dst_file_name, src_file_name);
      strcat(dst_file_name, dst_suffix);
    }
    sprintf(dst_cbm_file_name, "%s,%s,w", dst_file_name,  file_type_to_str_for_copy2(dst_file_type, src_file_type));
    res = cbm_open(src_lfn, src_device, 0, src_cbm_file_name);
    if(res != 0) {
      redraw();
      message_dialog_set("Error", _stroserror(_oserror));
      cbm_close(src_lfn);
      message_dialog_draw();
      message_dialog_loop();
      break;
    }
    res2 = cmd_channel_read(src_device, &error, 1);
    if(res2 == -1) {
      redraw();
      message_dialog_set("Error", _stroserror(_oserror));
      cbm_close(src_lfn);
      message_dialog_draw();
      message_dialog_loop();
      break;
    } else if(res != 0) {
      redraw();
      message_dialog_set("Error", error);
      cbm_close(src_lfn);
      cmd_channel_close(src_device);
      message_dialog_draw();
      message_dialog_loop();
      break;
    }
    res2 = delete_file(dst_device, dst_file_name, &error);
    if(res2 == -1) {
      redraw();
      message_dialog_set("Error", _stroserror(_oserror));
      cbm_close(src_lfn);
      message_dialog_draw();
      message_dialog_loop();
      break;
    }
    res = cbm_open(dst_lfn, dst_device, 1, dst_cbm_file_name);
    if(res != 0) {
      redraw();
      message_dialog_set("Error", _stroserror(_oserror));
      cbm_close(dst_lfn);
      cbm_close(src_lfn);
      cmd_channel_close(src_device);
      message_dialog_draw();
      message_dialog_loop();
      break;
    }
    res2 = cmd_channel_read(dst_device, &error, 1);
    if(res2 == -1) {
      redraw();
      message_dialog_set("Error", _stroserror(_oserror));
      cbm_close(dst_lfn);
      cbm_close(src_lfn);
      cmd_channel_close(src_device);
      message_dialog_draw();
      message_dialog_loop();
      break;
    } else if(res != 0) {
      redraw();
      message_dialog_set("Error", error);
      cbm_close(dst_lfn);
      cmd_channel_close(dst_device);
      cbm_close(src_lfn);
      cmd_channel_close(src_device);
      message_dialog_draw();
      message_dialog_loop();
      break;
    }
    is_stop = 0;
    blocks = 0;
    while(1) {
      res2 = cbm_read(src_lfn, buf, BUFFER_SIZE);
      if(res2 == -1) {
        redraw();
        message_dialog_set("Error", _stroserror(_oserror));
        cbm_close(dst_lfn);
        cmd_channel_close(dst_device);
        cbm_close(src_lfn);
        cmd_channel_close(src_device);
        message_dialog_draw();
        message_dialog_loop();
        is_stop = 1;
        break;
      } else if(res2 == 0)
        break;
      res3 = cbm_write(dst_lfn, buf, res2);
      if(res2 == -1) {
        redraw();
        message_dialog_set("Error", _stroserror(_oserror));
        cbm_close(dst_lfn);
        cmd_channel_close(dst_device);
        cbm_close(src_lfn);
        cmd_channel_close(src_device);
        message_dialog_draw();
        message_dialog_loop();
        is_stop = 1;
        break;
      }
      blocks++;
      if(size != 0)
        progresses[0].count = (((unsigned long) blocks) * PROGRESS_MAX) / size;
      else
        progresses[0].count = PROGRESS_MAX;
      progresses[0].count = umin(PROGRESS_MAX, progresses[0].count);
      progress_dialog_draw();
    }
    if(is_stop) break;
    cbm_close(dst_lfn);
    cmd_channel_close(dst_device);
    cbm_close(src_lfn);
    cmd_channel_close(src_device);
    progresses[1].count = (((unsigned long) (i + 1)) * PROGRESS_MAX) / selected_elem_index_count;
    progress_dialog_draw();
  }
  redraw();
  reload_or_set_status_to_unloaded(dst_device);
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
    case 'c':
      copy_files();
      break;
    case 'q':
      is_exit = 1;
      break;
    }
  }
}
