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
#include <stddef.h>
#include <stdlib.h>
#include "file.h"

struct file view_file;

struct file loaded_file;
struct file_ext loaded_file_ext;

void initialize_files(void)
{
  view_file.content = NULL;
  view_file.size = 0;
  loaded_file.content = NULL;
  loaded_file.size = 0;
}

void finalize_files(void)
{
  if(view_file.content != NULL) free(view_file.content);
  if(loaded_file.content != NULL) free(loaded_file.content);
}

void file_free(struct file *file)
{
  if(file->content != NULL) {
    free(file->content);
    file->content = NULL;
    file->size = 0;
  }
}
