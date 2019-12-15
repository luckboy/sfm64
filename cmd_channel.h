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
#ifndef _CMD_CHANNEL_H
#define _CMD_CHANNEL_H

void initialize_cmd_channels(void);
void finalize_cmd_channels(void);

int cmd_channel_read(unsigned char device, const char **msg, char must_open);
int cmd_channel_write(unsigned char device, const char *cmd, char must_open);
void cmd_channel_close(unsigned device);

#endif
