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

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "dirent.h"
#if MICROPY_VFS_FAT
#include "lib/oofatfs/diskio.h"
#include "lib/oofatfs/ff.h"
#endif
#include "esp_log.h"
#include "extmod/misc.h"
#include "extmod/vfs.h"
#include "genhdr/mpversion.h"
#include "py/mpconfig.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/mpthread.h"
#include "py/objstr.h"
#include "py/objtuple.h"
#include "py/runtime.h"

#if MICROPY_VFS_POSIX
#include "extmod/vfs_posix.h"
#endif

#define TAG "MOD_OS"

extern const mp_obj_type_t mp_fat_vfs_type;

STATIC const qstr os_uname_info_fields[] = { MP_QSTR_sysname, MP_QSTR_nodename, MP_QSTR_release, MP_QSTR_version,
                                             MP_QSTR_machine };
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_sysname_obj, MICROPY_PY_SYS_PLATFORM);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_nodename_obj, MICROPY_PY_SYS_NODE);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_release_obj, MICROPY_VERSION_STRING);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_version_obj, MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_machine_obj, MICROPY_HW_BOARD_NAME " with " MICROPY_HW_MCU_NAME);

STATIC MP_DEFINE_ATTRTUPLE(os_uname_info_obj, os_uname_info_fields, 5, (mp_obj_t)&os_uname_info_sysname_obj,
                           (mp_obj_t)&os_uname_info_nodename_obj, (mp_obj_t)&os_uname_info_release_obj,
                           (mp_obj_t)&os_uname_info_version_obj, (mp_obj_t)&os_uname_info_machine_obj);

STATIC mp_obj_t os_uname(void)
{
    return (mp_obj_t)&os_uname_info_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_uname_obj, os_uname);

STATIC mp_obj_t os_version(void)
{
    char version[40] = { 0 };
    sprintf(version, "%s-v%s", MICROPY_SW_VENDOR_NAME, SYSINFO_SYSTEM_VERSION);
    return mp_obj_new_str(version, strlen(version));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_version_obj, os_version);

STATIC mp_obj_t os_version_info(void)
{
    char version_info[80] = { 0 };
#ifdef MICROPY_HW_BOARD_TYPE
    sprintf(version_info, "%s-%s-%s-v%s-%s", MICROPY_SW_VENDOR_NAME, MICROPY_HW_MCU_NAME, MICROPY_HW_BOARD_TYPE,
            SYSINFO_SYSTEM_VERSION, MICROPY_BUILD_DATE);
#else
    sprintf(version_info, "%s-%s-v%s-%s", MICROPY_SW_VENDOR_NAME, MICROPY_HW_MCU_NAME, SYSINFO_SYSTEM_VERSION,
            MICROPY_BUILD_DATE);
#endif
    return mp_obj_new_str(version_info, strlen(version_info));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_version_info_obj, os_version_info);

STATIC mp_obj_t os_urandom(mp_obj_t num)
{
    mp_int_t n = mp_obj_get_int(num);
    vstr_t vstr;
    vstr_init_len(&vstr, n);
    mp_uint_t r = 0;
    for (int i = 0; i < n; i++) {
        if ((i & 3) == 0) {
            r = rand();
        }
        vstr.buf[i] = r;
        r >>= 8;
    }
    vstr.buf[n] = '\0';
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_urandom_obj, os_urandom);


#if MICROPY_PY_OS_DUPTERM
STATIC mp_obj_t os_dupterm_notify(mp_obj_t obj_in)
{
    (void)obj_in;
    for (;;) {
        int c = mp_uos_dupterm_rx_chr();
        if (c < 0) {
            break;
        }
        ringbuf_put(&stdin_ringbuf, c);
        mp_hal_wake_main_task_from_isr();
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_dupterm_notify_obj, os_dupterm_notify);
#endif

extern void aos_task_show_info();
STATIC mp_obj_t mod_os_tasklist(void)
{
    aos_task_show_info();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_os_tasklist_obj, mod_os_tasklist);

extern void aos_mm_show_info(void);
STATIC mp_obj_t mod_os_dumpsys_mm(void)
{
    aos_mm_show_info();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_os_dumpsys_mm_obj, mod_os_dumpsys_mm);

#include "esp_heap_caps.h"
STATIC mp_obj_t os_heap(void)
{
    printf("[free-defaut] %d\r\n", esp_get_free_heap_size());
    printf("[free-8bit-internal] %d\r\n", heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL));
#if CONFIG_SPIRAM_USE_MALLOC
    printf("[free-8bit-spiram] %d\r\n", heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM));
#endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_heap_obj, os_heap);

STATIC const mp_rom_map_elem_t os_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uos) },
    { MP_ROM_QSTR(MP_QSTR_uname), MP_ROM_PTR(&os_uname_obj) },
    { MP_ROM_QSTR(MP_QSTR_urandom), MP_ROM_PTR(&os_urandom_obj) },
    { MP_ROM_QSTR(MP_QSTR_version), MP_ROM_PTR(&os_version_obj) },
    { MP_ROM_QSTR(MP_QSTR_version_info), MP_ROM_PTR(&os_version_info_obj) },

#if MICROPY_PY_OS_DUPTERM
    { MP_ROM_QSTR(MP_QSTR_dupterm), MP_ROM_PTR(&mp_uos_dupterm_obj) },
    { MP_ROM_QSTR(MP_QSTR_dupterm_notify), MP_ROM_PTR(&os_dupterm_notify_obj) },
#endif

#if MICROPY_VFS
    { MP_ROM_QSTR(MP_QSTR_listdir), MP_ROM_PTR(&mp_vfs_listdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&mp_vfs_ilistdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&mp_vfs_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&mp_vfs_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&mp_vfs_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&mp_vfs_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&mp_vfs_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&mp_vfs_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&mp_vfs_stat_obj) },
    { MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&mp_vfs_statvfs_obj) },
    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&mp_vfs_mount_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&mp_vfs_umount_obj) },
#if MICROPY_VFS_POSIX
    { MP_ROM_QSTR(MP_QSTR_VfsPosix), MP_ROM_PTR(&mp_type_vfs_posix) },
#endif
#if MICROPY_VFS_FAT
    { MP_ROM_QSTR(MP_QSTR_VfsFat), MP_ROM_PTR(&mp_fat_vfs_type) },
#endif
#if MICROPY_VFS_LFS1
    { MP_ROM_QSTR(MP_QSTR_VfsLfs1), MP_ROM_PTR(&mp_type_vfs_lfs1) },
#endif
#if MICROPY_VFS_LFS2
    { MP_ROM_QSTR(MP_QSTR_VfsLfs2), MP_ROM_PTR(&mp_type_vfs_lfs2) },
#endif
#endif
    { MP_ROM_QSTR(MP_QSTR_heap), MP_ROM_PTR(&os_heap_obj) },
    { MP_ROM_QSTR(MP_QSTR_tasklist), MP_ROM_PTR(&mod_os_tasklist_obj) },
    { MP_ROM_QSTR(MP_QSTR_dumpsys_mm), MP_ROM_PTR(&mod_os_dumpsys_mm_obj) },
};

STATIC MP_DEFINE_CONST_DICT(os_module_globals, os_module_globals_table);

const mp_obj_module_t uos_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&os_module_globals,
};
