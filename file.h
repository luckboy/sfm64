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
#ifndef _FILE_H
#define _FILE_H

struct file
{
  char *content;
  unsigned size;
};

struct file_ext
{
  char name[17];
  unsigned size_in_blocks;
  unsigned char type;
};

extern struct file view_file;

extern struct file loaded_file;
extern struct file_ext loaded_file_ext;


void initialize_files(void);
void finalize_files(void);

void file_free(struct file *file);

#endif
