/* Copyright (c) 2013, Philipp Tölke
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef INCLUDE_VFS_H
#define INCLUDE_VFS_H

#include "ff.h"
#include <stddef.h>

#define vfs_load_plugin(x)
#define bcopy(src, dest, len) memmove(dest, src, len)


typedef struct
{
    short date;
    short time;
} time_t;
typedef DIR vfs_dir_t;
typedef FIL vfs_file_t;
typedef struct
{
    long st_size;
    char st_mode;
    time_t st_mtime;
} vfs_stat_t;
typedef struct
{
    char name[13];
} vfs_dirent_t;
typedef FIL vfs_t;

struct tm
{
    int tm_year;
    int tm_mon;
    int tm_mday;
    int tm_hour;
    int tm_min;
};

#define time(x)
#define vfs_eof f_eof
#define VFS_ISDIR(st_mode) ((st_mode) & AM_DIR)
#define VFS_ISREG(st_mode) !((st_mode) & AM_DIR)
#define vfs_rename(vfs, from, to) f_rename(from, to)
#define VFS_IRWXU 0
#define VFS_IRWXG 0
#define VFS_IRWXO 0
#define vfs_mkdir(vfs, name, mode) f_mkdir(name)
#define vfs_rmdir(vfs, name) f_unlink(name)
#define vfs_remove(vfs, name) f_unlink(name)
#define vfs_chdir(vfs, dir) f_chdir(dir)
char *vfs_getcwd(vfs_t *vfs, void *, int dummy);
int vfs_read (void *buffer, int dummy, int len, vfs_file_t *file);
int vfs_write (void *buffer, int dummy, int len, vfs_file_t *file);
vfs_dirent_t *vfs_readdir(vfs_dir_t *dir);
vfs_file_t *vfs_open(vfs_t *vfs, const char *filename, const char *mode);
vfs_t *vfs_openfs(void);
void vfs_close(vfs_t *vfs);
int vfs_stat(vfs_t *vfs, const char *filename, vfs_stat_t *st);
void vfs_closedir(vfs_dir_t *dir);
vfs_dir_t *vfs_opendir(vfs_t *vfs, const char *path);
struct tm *gmtime(time_t *c_t);

#endif /* INCLUDE_VFS_H */
