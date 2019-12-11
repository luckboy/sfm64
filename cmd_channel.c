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
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "cmd_channel.h"

#define CMD_CHANNEL_MAX         4

struct cmd_channel
{
  unsigned char lfn;
  char is_opened;
  char message[39];
};

static struct cmd_channel cmd_channels[CMD_CHANNEL_MAX];

void initialize_cmd_channels(void)
{
  unsigned char i;
  for(i = 0; i < CMD_CHANNEL_MAX; i++) {
    cmd_channels[i].lfn = 16 + i;
    cmd_channels[i].is_opened = 0;
  }
}

void finalize_cmd_channels(void)
{
  unsigned char i;
  for(i = 0; i < CMD_CHANNEL_MAX; i++) {
    if(cmd_channels[i].is_opened) cbm_close(cmd_channels[i].lfn);
  }
}

static char *cbm_get_line(unsigned char lfn, char *buf, int count)
{
  char is_new_line = 0;
  int i;
  _oserror = cbm_k_chkin(lfn);
  if(_oserror != 0) return NULL;
  if(count > 0) {
    for(i = 0; i < count - 1; i++) {
      char c;
      if(cbm_k_readst() != 0)  break;
      c = cbm_k_basin();
      if((cbm_k_readst() & 0xbf) != 0) break;
      if(c != '\n') {
        buf[i] = c;
      } else {
        is_new_line = 1;
        break;
      }
    }
    buf[i] = 0;
  }
  if(!is_new_line) {
    for(i = 0; i < count; i++) {
      char c;
      if(cbm_k_readst() != 0)  break;
      c = cbm_k_basin();
      if((cbm_k_readst() & 0xbf) != 0) break;
      if(c == '\n') break;
    }
  }
  cbm_k_clrch();
  return buf;
}

static int open_cmd_channel(unsigned char device)
{
  unsigned char i = device - 8;
  if(!cmd_channels[i].is_opened) {
    unsigned char res;
    res = cbm_open(cmd_channels[i].lfn, device, 15, "");
    if(res != 0) {
      cbm_close(cmd_channels[i].lfn);
      return -1;
    }
    cmd_channels[i].is_opened = 1;
  }
  return 0;
}

int cmd_channel_read(unsigned char device, const char **msg)
{
  unsigned char i = device - 8;
  int res = open_cmd_channel(device);
  if(res == -1) return -1;
  if(cbm_get_line(cmd_channels[i].lfn, cmd_channels[i].message, 39) == NULL) {
    cmd_channel_close(device);
    return -1;
  }
  sscanf(cmd_channels[i].message, "%d", &res);
  *msg = cmd_channels[i].message;
  return res >= 20 ? res : 0;
}

int cmd_channel_write(unsigned char device, const char *cmd)
{
  unsigned char i = device - 8;
  int res = open_cmd_channel(device);
  if(res == -1) return -1;
  res = cbm_write(cmd_channels[i].lfn, cmd, strlen(cmd));
  if(res == -1) {
    cmd_channel_close(device);
    return -1;
  }
  return 0;
}

void cmd_channel_close(unsigned device)
{
  unsigned char i = device - 8;
  if(cmd_channels[i].is_opened) {
    cbm_close(cmd_channels[i].lfn);
    cmd_channels[i].is_opened = 0;
  }
}
