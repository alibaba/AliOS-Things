/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2018 Paul Sokolovsky
 * Copyright (c) 2014-2018 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>

#include "py/mpconfig.h"
#include "py/runtime.h"
#include "py/objtuple.h"
#include "py/objstr.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/mpthread.h"
#include "extmod/vfs.h"
#include "extmod/misc.h"
#include "HaasLog.h"
#include "genhdr/mpversion.h"
#include "aos/kernel.h"
#include "dirent.h"

#if MICROPY_VFS_POSIX
#include "vfs_posix.h"
#endif

extern const mp_obj_type_t mp_fat_vfs_type;

STATIC const qstr os_uname_info_fields[] = {
    MP_QSTR_sysname, MP_QSTR_nodename,
    MP_QSTR_release, MP_QSTR_version, MP_QSTR_machine
};
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_sysname_obj, MICROPY_PY_SYS_PLATFORM);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_nodename_obj, MICROPY_PY_SYS_PLATFORM);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_release_obj, MICROPY_VERSION_STRING);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_version_obj, MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_machine_obj, MICROPY_HW_BOARD_NAME " with " MICROPY_HW_MCU_NAME);

STATIC MP_DEFINE_ATTRTUPLE(
    os_uname_info_obj,
    os_uname_info_fields,
    5,
    (mp_obj_t)&os_uname_info_sysname_obj,
    (mp_obj_t)&os_uname_info_nodename_obj,
    (mp_obj_t)&os_uname_info_release_obj,
    (mp_obj_t)&os_uname_info_version_obj,
    (mp_obj_t)&os_uname_info_machine_obj
    );

STATIC mp_obj_t os_uname(void) {
    return (mp_obj_t)&os_uname_info_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_uname_obj, os_uname);

STATIC mp_obj_t os_urandom(mp_obj_t num) {
    mp_int_t n = mp_obj_get_int(num);
    vstr_t vstr;
    vstr_init_len(&vstr, n);
    uint32_t r = 0;
    for (int i = 0; i < n; i++) {
        if ((i & 3) == 0) {
            r = aos_rand(); // returns 32-bit random number
        }
        vstr.buf[i] = r;
        r >>= 8;
    }
    vstr.buf[n] = '\0';
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_urandom_obj, os_urandom);

static int up_one_level(char *s)
{
    char *tail;

    if (!s)
        return -1;

    tail = s + strlen(s) - 1;
    if (*tail == '/')
        tail--;

    while (*tail != '\0' && *tail != '/')
        tail--;

    if (*tail == '\0') {
        return -1;
    } else {
        *(tail + 1) = '\0';
        return 0;
    }
}

char *py_get_realpath(const char *path, char *resolved_path, unsigned int len)
{
    char *ret, *p = (char *)path, *r = resolved_path;

    if (!path || !r || len < 1)
        return NULL;

    memset(r, 0, len);

    // deal with heading char
    if (p[0] != '/') {
        // relative path
        ret = getcwd(r, len);
        if (!ret)
            return NULL;

        // add tailing '/' if no
        if (r[strlen(r) - 1] != '/') {
            r[strlen(r)] = '/';
        }

        r += strlen(r);
    } else {
        // absolute path
        r[0] = '/';
        r++;
    }

    // iterate to exclude '.', '..'. '/'
    while (*p != '\0') {
        while (*p == '/')
            p++;
        if (*p == '\0')
            break;

        if (*p == '.') {
            p++;
            // end with '.'
            if (*p == '\0')
                break;

            if (*p == '.') {
                // '..' or '../'
                if ((*(p + 1) != '/') && (*(p + 1) != '\0')) {
                    printf("Invalid path %s\r\n", path);
                    return NULL;
                } else {
                    // '..' case
                    p++;
                    // if (*p == '/') {
                    if (up_one_level(resolved_path) != 0) {
                        printf("Failed to go up now. Invalid path %s\r\n", path);
                        return NULL;
                    }

                    r = resolved_path + strlen(resolved_path);
                    // }

                    // end with '.'
                    if (*p == '\0') {
                        break;
                    }
                }
            } else {
                if ((*p != '/') && (*p != '\0')) {
                    printf("Invalid path %s\r\n", path);
                    return NULL;
                } else {
                    // '.' case
                    p++;
                }
            }
        }

        while (*p == '/')
            p++;
        if (*p == '\0')
            break;

        // if another round of ./.., just continue
        if (*p == '.')
            continue;

        // path string may be found now, save to r
        while ((*p != '/') && (*p != '\0'))
            *r++ = *p++;

        // add taling '/' if necessary
        if (*(r - 1) != '/') {
            *r++ = '/';
        }
    }

    // exclude the tailing '/', just in case it is a file
    if ((resolved_path[strlen(resolved_path) - 1] == '/') &&
        (strlen(resolved_path) != 1)) {
        resolved_path[strlen(resolved_path) - 1] = '\0';
    }

    return resolved_path;
}

STATIC mp_obj_t mod_os_stat(mp_obj_t path_in) {
    struct stat sb;
    const char *path = mp_obj_str_get_str(path_in);
    int res = stat(path, &sb);

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
    t->items[0] = MP_OBJ_NEW_SMALL_INT(sb.st_mode);
    t->items[1] = mp_obj_new_int_from_uint(sb.st_ino);
    t->items[2] = mp_obj_new_int_from_uint(sb.st_dev);
    t->items[3] = mp_obj_new_int_from_uint(sb.st_nlink);
    t->items[4] = mp_obj_new_int_from_uint(sb.st_uid);
    t->items[5] = mp_obj_new_int_from_uint(sb.st_gid);
    t->items[6] = mp_obj_new_int_from_uint(sb.st_size);
    t->items[7] = mp_obj_new_int_from_uint(sb.st_atime);
    t->items[8] = mp_obj_new_int_from_uint(sb.st_mtime);
    t->items[9] = mp_obj_new_int_from_uint(sb.st_ctime);
    return MP_OBJ_FROM_PTR(t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_os_stat_obj, mod_os_stat);

STATIC mp_obj_t mod_os_statvfs(mp_obj_t path_in) {
    struct statfs sb;
    const char *path = mp_obj_str_get_str(path_in);
    int res = statfs(path, &sb);

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(9, NULL));
    t->items[0] = MP_OBJ_NEW_SMALL_INT(sb.f_type);
    t->items[1] = MP_OBJ_NEW_SMALL_INT(sb.f_bsize);
    t->items[2] = MP_OBJ_NEW_SMALL_INT(sb.f_blocks);
    t->items[3] = MP_OBJ_NEW_SMALL_INT(sb.f_bfree);
    t->items[4] = MP_OBJ_NEW_SMALL_INT(sb.f_bavail);
    t->items[5] = MP_OBJ_NEW_SMALL_INT(sb.f_files);
    t->items[6] = MP_OBJ_NEW_SMALL_INT(sb.f_ffree);
    t->items[7] = MP_OBJ_NEW_SMALL_INT(sb.f_fsid);
    t->items[8] = MP_OBJ_NEW_SMALL_INT(sb.f_namelen);
    return MP_OBJ_FROM_PTR(t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_os_statvfs_obj, mod_os_statvfs);


STATIC mp_obj_t mod_os_remove(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);

    char abspath[256] = {0};
    path = py_get_realpath(path, abspath, sizeof(abspath));

    MP_THREAD_GIL_EXIT();
    int r = unlink(path);
    MP_THREAD_GIL_ENTER();

    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_os_remove_obj, mod_os_remove);


static int py_mv(char *from, char *to)
{
    int fd_from = 0, fd_to = 0;
    char buf[128] = {0};
    int rlen = 0, wlen = 0, ret = -1, isdir = false;
    struct stat s;

    char abspath_from[256] = {0}, abspath_to[256] = {0};

    from = py_get_realpath(from, abspath_from, sizeof(abspath_from));
    to = py_get_realpath(to, abspath_to, sizeof(abspath_to));
    if (!from || !to) {
        LOG_E("Failed to get real path!\r\n");
        return -1;
    }

    if (!stat(to, &s)) {
        if (S_ISDIR(s.st_mode)) {
            char *p = strrchr(from, '/');

            if (!p)
                ret = asprintf(&to, "%s/%s", to, from);
            else
                ret = asprintf(&to, "%s%s", to, p);
            if (ret < 0) {
                LOG_E("asprintf failed\n");
                return -1;
            }
            isdir = true;
        }
    }

    ret = rename(from, to);
    if (ret < 0 && errno != EXDEV) {
        LOG_E("rename %s to %s failed - %s\n", from, to, strerror(errno));
        return -1;
    } else if (ret == 0) {
        return 0;
    }

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0) {
        LOG_E("open %s failed - %s\n", from, strerror(errno));
        return -1;
    }

    fd_to = open(to, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd_to < 0) {
        LOG_E("open %s failed - %s\n", to, strerror(errno));
        goto close_from;
    }

    while ((rlen = read(fd_from, buf, 128))) {
        if (rlen < 0) {
            LOG_E("read %s failed - %s\n", from, strerror(errno));
            goto close_to;
        }

        wlen = write(fd_to, buf, rlen);
        if (wlen != rlen) {
            LOG_E("write %s failed - %s\n", to, strerror(errno));
            goto close_to;
        }
    }

    ret = unlink(from);
    if (ret) {
        LOG_E("unlink %s failed - %s\n", from, strerror(errno));
        goto close_to;
    }

    ret = 0;
close_to:
    close(fd_to);
close_from:
    close(fd_from);
    if (isdir)
        free(to);
    return ret;
}

STATIC mp_obj_t mod_os_rename(mp_obj_t old_path_in, mp_obj_t new_path_in) {
    const char *old_path = mp_obj_str_get_str(old_path_in);
    const char *new_path = mp_obj_str_get_str(new_path_in);

    MP_THREAD_GIL_EXIT();
    int r = rename(old_path, new_path);
    MP_THREAD_GIL_ENTER();

    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_os_rename_obj, mod_os_rename);

int py_rrmdir(const char *path)
{
    struct stat s;
    DIR *pdir = NULL;
    struct dirent *entry = NULL;
    int ret = -1;
    char *dir, *p;

    if (!path)
        return -EINVAL;

    dir = strdup(path);
    p = dir + strlen(dir) - 1;
    while ((*p == '/') && (p > dir)) {
        *p = '\0';
        p--;
    }

    if (stat(dir, &s) || !S_ISDIR(s.st_mode)) {
        LOG_E("%s is neither existed nor a directory\n", dir);
        goto out;
    }

    pdir = opendir(dir);
    if (!pdir) {
        LOG_E("opendir %s failed - %s\n", dir, strerror(errno));
        goto out;
    }

    ret = 0;
    while ((ret == 0) && (entry = readdir(pdir))) {
        char fpath[128];

        snprintf(fpath, 128, "%s/%s", dir, entry->d_name);

        ret = stat(fpath, &s);
        if (ret) {
            LOG_E("stat %s failed\n", fpath);
            break;
        }

        if (!strcmp(entry->d_name, "."))
            continue;
        if (!strcmp(entry->d_name, ".."))
            continue;

        if (S_ISDIR(s.st_mode))
            ret = py_rrmdir(fpath);
        else
            ret = unlink(fpath);
    }

    closedir(pdir);
    if (ret == 0) {
        ret = rmdir(dir);
        if (ret)
            LOG_E("rmdir %s failed\n", dir);
    }
out:
    free(dir);
    return ret;
}


STATIC mp_obj_t mod_os_rmdir(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    char abspath[256] = {0};

    path = py_get_realpath(path, abspath, sizeof(abspath));

    MP_THREAD_GIL_EXIT();
    int r = rrmdir(path);
    MP_THREAD_GIL_ENTER();

    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_os_rmdir_obj, mod_os_rmdir);

STATIC mp_obj_t mod_os_system(mp_obj_t cmd_in) {
    const char *cmd = mp_obj_str_get_str(cmd_in);

    MP_THREAD_GIL_EXIT();
    int r = system(cmd);
    MP_THREAD_GIL_ENTER();

    return MP_OBJ_NEW_SMALL_INT(r);
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_os_system_obj, mod_os_system);

STATIC mp_obj_t mod_os_getenv(mp_obj_t var_in) {
    const char *s = getenv(mp_obj_str_get_str(var_in));
    if (s == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(s, strlen(s));
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_os_getenv_obj, mod_os_getenv);

STATIC mp_obj_t mod_os_putenv(mp_obj_t key_in, mp_obj_t value_in) {
    const char *key = mp_obj_str_get_str(key_in);
    const char *value = mp_obj_str_get_str(value_in);

    int ret = setenv(key, value, 1);
    if (ret == -1) {
        mp_raise_OSError(errno);
    }

    return MP_OBJ_NEW_SMALL_INT(ret);
}
MP_DEFINE_CONST_FUN_OBJ_2(mod_os_putenv_obj, mod_os_putenv);

STATIC mp_obj_t mod_os_unsetenv(mp_obj_t key_in) {
    const char *key = mp_obj_str_get_str(key_in);
    int ret = unsetenv(key);
    if (ret == -1) {
        mp_raise_OSError(errno);
    }
    return MP_OBJ_NEW_SMALL_INT(ret);
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_os_unsetenv_obj, mod_os_unsetenv);

static int py_mkdir_do(char *path, int flags)
{
    char *s = NULL;
    char abspath[256] = {0};
    #define MKDIR_FLAGS_PARENTS (1 << 0)
    path = py_get_realpath(path, abspath, sizeof(abspath));
    if (!path) {
        LOG_E("Failed to get real path!\r\n");
        return -1;
    }

    /*
     * All of operations must base on root directory
     * As alios has not root dierctory, we can operate '/data' but not '/'
     */
#define MOUNT_BASE_DIR "/"
    if (strncmp(path, MOUNT_BASE_DIR, strlen(MOUNT_BASE_DIR))) {
        LOG_E("make directory must base on %s\n", MOUNT_BASE_DIR);
        return -1;
    }

    if (path[0] == '.') {
        if (path[1] == '\0')
            return 0;
        if (path[1] == '.' && path[2] == '\0')
            return 0;
    }

    if (flags & MKDIR_FLAGS_PARENTS)
        s = path + strlen(MOUNT_BASE_DIR);
    while (1) {
        struct stat st;

        if (flags & MKDIR_FLAGS_PARENTS) {
            /* in case of tailing '/', such as '/data/a/' */
            if (*(s++) == '\0')
                break;
            s = strchr(s, '/');
            if (s)
                *s = '\0';
        }

        if (!stat(path, &st)) {
            if (S_ISDIR(st.st_mode))
                goto next;
            LOG_E("make failed - %s already existed and not direcotry\n", path);
            return -1;
        }

        if (mkdir(path, 0777) < 0) {
            LOG_E("mkdir %s failed\n", path);
            return -1;
        }

next:
        if (!s)
            break;
        *s = '/';
    }
    return 0;
}

STATIC mp_obj_t mod_os_mkdir(mp_obj_t path_in) {
    // TODO: Accept mode param
    const char *path = mp_obj_str_get_str(path_in);
  
    MP_THREAD_GIL_EXIT();
    int r = py_mkdir_do(path, 0777);
    MP_THREAD_GIL_ENTER();

    return mp_obj_new_int(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_os_mkdir_obj, mod_os_mkdir);

typedef struct _mp_obj_listdir_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    DIR *dir;
} mp_obj_listdir_t;

STATIC mp_obj_t listdir_next(mp_obj_t self_in) {
    mp_obj_listdir_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->dir == NULL) {
        goto done;
    }
    MP_THREAD_GIL_EXIT();
    struct dirent *dirent = readdir(self->dir);
    if (dirent == NULL) {
        closedir(self->dir);
        MP_THREAD_GIL_ENTER();
        self->dir = NULL;
    done:
        return MP_OBJ_STOP_ITERATION;
    }
    MP_THREAD_GIL_ENTER();

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL));
    t->items[0] = mp_obj_new_str(dirent->d_name, strlen(dirent->d_name));

    #ifdef _DIRENT_HAVE_D_TYPE
    #ifdef DTTOIF
    t->items[1] = MP_OBJ_NEW_SMALL_INT(DTTOIF(dirent->d_type));
    #else
    if (dirent->d_type == DT_DIR) {
        t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFDIR);
    } else if (dirent->d_type == DT_REG) {
        t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFREG);
    } else {
        t->items[1] = MP_OBJ_NEW_SMALL_INT(dirent->d_type);
    }
    #endif
    #else
    // DT_UNKNOWN should have 0 value on any reasonable system
    t->items[1] = MP_OBJ_NEW_SMALL_INT(0);
    #endif

    #ifdef _DIRENT_HAVE_D_INO
    t->items[2] = MP_OBJ_NEW_SMALL_INT(dirent->d_ino);
    #else
    t->items[2] = MP_OBJ_NEW_SMALL_INT(0);
    #endif
    return MP_OBJ_FROM_PTR(t);
}

STATIC mp_obj_t mod_os_ilistdir(size_t n_args, const mp_obj_t *args) {
    const char *path = ".";
    if (n_args > 0) {
        path = mp_obj_str_get_str(args[0]);
    }

    char abspath[256] = {0};
    path = py_get_realpath(path, abspath, sizeof(abspath));

    mp_obj_listdir_t *o = m_new_obj(mp_obj_listdir_t);
    o->base.type = &mp_type_polymorph_iter;
    MP_THREAD_GIL_EXIT();
    o->dir = opendir(path);
    MP_THREAD_GIL_ENTER();
    o->iternext = listdir_next;
    return MP_OBJ_FROM_PTR(o);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_os_ilistdir_obj, 0, 1, mod_os_ilistdir);

STATIC mp_obj_t mod_os_listdir(size_t n_args, const mp_obj_t *args) {
    const char *path = ".";
    if (n_args > 0) {
        path = mp_obj_str_get_str(args[0]);
    }

    mp_raise_OSError(EPERM);
    return mp_const_none;

    mp_obj_listdir_t *o = m_new_obj(mp_obj_listdir_t);
 
    MP_THREAD_GIL_EXIT();
    MP_THREAD_GIL_ENTER();

    return MP_OBJ_FROM_PTR(o);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_os_listdir_obj, 0, 1, mod_os_listdir);

STATIC mp_obj_t mod_os_errno(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return MP_OBJ_NEW_SMALL_INT(errno);
    }

    errno = mp_obj_get_int(args[0]);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_os_errno_obj, 0, 1, mod_os_errno);

STATIC mp_obj_t mod_os_chdir(mp_obj_t path_in) {
    // TODO: Accept mode param
    char *path = mp_obj_str_get_str(path_in);
    char abspath[256] = {0};
    path = py_get_realpath(path, abspath, sizeof(abspath));

    MP_THREAD_GIL_EXIT();
    int r = chdir(path);
    MP_THREAD_GIL_ENTER();

    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_os_chdir_obj, mod_os_chdir);


STATIC mp_obj_t mod_os_getcwd() {
    // TODO: Accept mode param

    char buf[MICROPY_ALLOC_PATH_MAX + 1];
    const char *ret = getcwd(buf, sizeof(buf));
    if (ret == NULL) {
        mp_raise_OSError(errno);
    }
    return mp_obj_new_str(ret, strlen(ret));
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_os_getcwd_obj, mod_os_getcwd);

STATIC mp_obj_t mod_os_file_open(mp_obj_t path_in, mp_obj_t mode_in)
{
    const char *path = mp_obj_str_get_str(path_in);
    const char *mode = mp_obj_str_get_str(mode_in);

    if(path == NULL || mode == NULL) {
        mp_raise_OSError(EINVAL);
        return mp_const_none;
    }

    FILE *stream = fopen(path, mode);
    if (stream == NULL) {
        mp_raise_OSError(ENOENT);
        return mp_const_none;
    }
    return MP_OBJ_FROM_PTR(stream);
}
MP_DEFINE_CONST_FUN_OBJ_2(mod_os_file_open_obj, mod_os_file_open);

STATIC mp_obj_t mod_os_file_close(mp_obj_t stream_in)
{
    FILE *stream = (FILE *)MP_OBJ_TO_PTR(stream_in);

    int ret = -1;
    if (stream != NULL) {
        ret = fclose(stream);
    }
    else {
        ret = 0;
    }
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_os_file_close_obj, mod_os_file_close);

STATIC mp_obj_t mod_os_file_read(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 4) {
        mp_raise_OSError(EINVAL);
        return mp_const_false;
    }

    mp_int_t size = mp_obj_get_int(args[1]);
    mp_int_t nmemb = mp_obj_get_int(args[2]);
    FILE *stream = (FILE *)MP_OBJ_TO_PTR(args[3]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_WRITE);

    int ret = -1;
    if (stream != NULL) {
        ret = fread(bufinfo.buf, size, nmemb, stream);
    }
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_os_file_read_obj, 4, mod_os_file_read);

STATIC mp_obj_t mod_os_file_write(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 4) {
        mp_raise_OSError(EINVAL);
        return mp_const_false;
    }
    mp_int_t size = mp_obj_get_int(args[1]);
    mp_int_t nmemb = mp_obj_get_int(args[2]);
    FILE *stream = (FILE *)MP_OBJ_TO_PTR(args[3]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_READ);

    int ret = -1;
    if (stream != NULL) {
        ret = fwrite(bufinfo.buf, size, nmemb, stream);
    }
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_os_file_write_obj, 4, mod_os_file_write);

STATIC mp_obj_t mod_os_file_seek(mp_obj_t stream_in, mp_obj_t offset_in, mp_obj_t whence_in)
{
    FILE *stream = (FILE *)MP_OBJ_TO_PTR(stream_in);
    if (stream == NULL) {
        mp_raise_OSError(EINVAL);
        return mp_const_false;
    }

    mp_int_t offset = mp_obj_get_int(offset_in);
    mp_int_t whence = mp_obj_get_int(whence_in);

    int ret = fseek(stream, offset, whence);
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_3(mod_os_file_seek_obj, mod_os_file_seek);

STATIC mp_obj_t mod_os_file_tell(mp_obj_t stream_in)
{
    FILE *stream = (FILE *)MP_OBJ_TO_PTR(stream_in);
    if (stream == NULL) {
        mp_raise_OSError(EINVAL);
        return mp_const_false;
    }

    int ret = ftell(stream);
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_os_file_tell_obj, mod_os_file_tell);

STATIC mp_obj_t mod_os_file_rewind(mp_obj_t stream_in)
{
    FILE *stream = (FILE *)MP_OBJ_TO_PTR(stream_in);
    if (stream == NULL) {
        mp_raise_OSError(EINVAL);
        return mp_const_false;
    }

    rewind(stream);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_os_file_rewind_obj, mod_os_file_rewind);

STATIC mp_obj_t mod_os_file_getpos(mp_obj_t stream_in)
{
    FILE *stream = (FILE *)MP_OBJ_TO_PTR(stream_in);
    if (stream == NULL) {
        mp_raise_OSError(EINVAL);
        return mp_const_false;
    } else {
        fpos_t pos = 0;
        int ret = fgetpos(stream, &pos);
        if (ret == 0) {
            return mp_obj_new_int(pos);
        } else {
            return mp_obj_new_int(-1);
        }
    }
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_os_file_getpos_obj, mod_os_file_getpos);

STATIC mp_obj_t mod_os_file_setpos(mp_obj_t stream_in, mp_obj_t pos_in)
{
    FILE *stream = (FILE *)MP_OBJ_TO_PTR(stream_in);
    if (stream == NULL) {
        mp_raise_OSError(EINVAL);
        return mp_const_false;
    }

    mp_int_t pos = mp_obj_get_int(pos_in);
    int ret = fsetpos(stream, &pos);
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_2(mod_os_file_setpos_obj, mod_os_file_setpos);

/// \function sync()
/// Sync all filesystems.
STATIC mp_obj_t os_sync(void) {
    #if MICROPY_VFS_FAT
    for (mp_vfs_mount_t *vfs = MP_STATE_VM(vfs_mount_table); vfs != NULL; vfs = vfs->next) {
        // this assumes that vfs->obj is fs_user_mount_t with block device functions
        disk_ioctl(MP_OBJ_TO_PTR(vfs->obj), CTRL_SYNC, NULL);
    }
    #endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(mod_os_sync_obj, os_sync);

#if MICROPY_PY_OS_DUPTERM
STATIC mp_obj_t mp_uos_dupterm_notify(mp_obj_t obj_in) {
    (void)obj_in;
    for (;;) {
        int c = mp_uos_dupterm_rx_chr();
        if (c < 0) {
            break;
        }
        ringbuf_put(&stdin_ringbuf, c);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_uos_dupterm_notify_obj, mp_uos_dupterm_notify);
#endif

STATIC const mp_rom_map_elem_t mp_module_os_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uos) },
    { MP_ROM_QSTR(MP_QSTR_uname), MP_ROM_PTR(&os_uname_obj) },
    { MP_ROM_QSTR(MP_QSTR_urandom), MP_ROM_PTR(&os_urandom_obj) },

#if MICROPY_PY_OS_DUPTERM
    { MP_ROM_QSTR(MP_QSTR_dupterm), MP_ROM_PTR(&mp_uos_dupterm_obj) },
    { MP_ROM_QSTR(MP_QSTR_dupterm_notify), MP_ROM_PTR(&mp_uos_dupterm_notify_obj) },
#endif

    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&mod_os_ilistdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_listdir), MP_ROM_PTR(&mod_os_listdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&mod_os_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&mod_os_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&mod_os_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&mod_os_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&mod_os_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&mod_os_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&mod_os_stat_obj) },
    { MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&mod_os_statvfs_obj) },
    // { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&mod_os_mount_obj) },
    // { MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&mod_os_umount_obj) },
    { MP_ROM_QSTR(MP_QSTR_unlink), MP_ROM_PTR(&mod_os_remove_obj) },

#if MICROPY_VFS_FAT
    { MP_ROM_QSTR(MP_QSTR_VfsFat), MP_ROM_PTR(&mp_fat_vfs_type) },
#endif
#if MICROPY_VFS_LFS1
    { MP_ROM_QSTR(MP_QSTR_VfsLfs1), MP_ROM_PTR(&mp_type_vfs_lfs1) },
#endif
#if MICROPY_VFS_LFS2
    { MP_ROM_QSTR(MP_QSTR_VfsLfs2), MP_ROM_PTR(&mp_type_vfs_lfs2) },
#endif
    
    { MP_ROM_QSTR(MP_QSTR_sync), MP_ROM_PTR(&mod_os_sync_obj) },
    { MP_ROM_QSTR(MP_QSTR_sep), MP_ROM_QSTR(MP_QSTR__slash_) },
    { MP_ROM_QSTR(MP_QSTR_errno), MP_ROM_PTR(&mod_os_errno_obj) },
    { MP_ROM_QSTR(MP_QSTR_system), MP_ROM_PTR(&mod_os_system_obj) },
    { MP_ROM_QSTR(MP_QSTR_getenv), MP_ROM_PTR(&mod_os_getenv_obj) },
    { MP_ROM_QSTR(MP_QSTR_putenv), MP_ROM_PTR(&mod_os_putenv_obj) },
    { MP_ROM_QSTR(MP_QSTR_unsetenv), MP_ROM_PTR(&mod_os_unsetenv_obj) },

    // file operation
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mod_os_file_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mod_os_file_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mod_os_file_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mod_os_file_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_seek), MP_ROM_PTR(&mod_os_file_seek_obj) },
    { MP_ROM_QSTR(MP_QSTR_tell), MP_ROM_PTR(&mod_os_file_tell_obj) },
    { MP_ROM_QSTR(MP_QSTR_rewind), MP_ROM_PTR(&mod_os_file_rewind_obj) },
    { MP_ROM_QSTR(MP_QSTR_getpos), MP_ROM_PTR(&mod_os_file_getpos_obj) },
    { MP_ROM_QSTR(MP_QSTR_setpos), MP_ROM_PTR(&mod_os_file_setpos_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_os_globals, mp_module_os_globals_table);

const mp_obj_module_t mp_module_uos = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_os_globals,
};
