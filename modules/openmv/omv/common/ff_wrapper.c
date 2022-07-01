/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013-2016 Kwabena W. Agyeman <kwagyeman@openmv.io>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * File System Helper Functions
 *
 */
#include "imlib_config.h"
#if defined(IMLIB_ENABLE_IMAGE_FILE_IO)

#include <string.h>
#include "py/runtime.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"

#include "common.h"
#include "fb_alloc.h"
#include "ff_wrapper.h"
#define FF_MIN(x,y) (((x)<(y))?(x):(y))

NORETURN static void ff_fail(FIL *fp, FRESULT res)
{
    if (fp) f_close(fp);
    mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT(ffs_strerror(res)));
}

NORETURN static void ff_read_fail(FIL *fp)
{
    if (fp) f_close(fp);
    mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to read requested bytes!"));
}

NORETURN static void ff_write_fail(FIL *fp)
{
    if (fp) f_close(fp);
    mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to write requested bytes!"));
}

NORETURN static void ff_expect_fail(FIL *fp)
{
    if (fp) f_close(fp);
    mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Unexpected value read!"));
}

NORETURN void ff_unsupported_format(FIL *fp)
{
    if (fp) f_close(fp);
    mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Unsupported format!"));
}

NORETURN void ff_file_corrupted(FIL *fp)
{
    if (fp) f_close(fp);
    mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("File corrupted!"));
}

NORETURN void ff_not_equal(FIL *fp)
{
    if (fp) f_close(fp);
    mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Images not equal!"));
}

NORETURN void ff_no_intersection(FIL *fp)
{
    if (fp) f_close(fp);
    mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("No intersection!"));
}

void file_read_open(FIL *fp, const char *path)
{
    FRESULT res = f_open_helper(fp, path, FA_READ|FA_OPEN_EXISTING);
    if (res != FR_OK) ff_fail(fp, res);
}

void file_write_open(FIL *fp, const char *path)
{
    FRESULT res = f_open_helper(fp, path, FA_WRITE|FA_CREATE_ALWAYS);
    if (res != FR_OK) ff_fail(fp, res);
}

void file_close(FIL *fp)
{
    FRESULT res = f_close(fp);
    if (res != FR_OK) ff_fail(fp, res);
}

void file_seek(FIL *fp, UINT offset)
{
    FRESULT res = f_lseek(fp, offset);
    if (res != FR_OK) ff_fail(fp, res);
}

void file_truncate(FIL *fp)
{
    FRESULT res = f_truncate(fp);
    if (res != FR_OK) ff_fail(fp, res);
}

void file_sync(FIL *fp)
{
    FRESULT res = f_sync(fp);
    if (res != FR_OK) ff_fail(fp, res);
}

// These wrapper functions are used for backward compatibility with
// OpenMV code using vanilla FatFS. Note: Extracted from cc3200 ftp.c

STATIC FATFS *lookup_path(const TCHAR **path) {
    mp_vfs_mount_t *fs = mp_vfs_lookup_path(*path, path);
    if (fs == MP_VFS_NONE || fs == MP_VFS_ROOT) {
        return NULL;
    }
    // here we assume that the mounted device is FATFS
    return &((fs_user_mount_t*)MP_OBJ_TO_PTR(fs->obj))->fatfs;
}

FRESULT f_open_helper(FIL *fp, const TCHAR *path, BYTE mode) {
    FATFS *fs = lookup_path(&path);
    if (fs == NULL) {
        return FR_NO_PATH;
    }
    return f_open(fs, fp, path, mode);
}

FRESULT f_opendir_helper(FF_DIR *dp, const TCHAR *path) {
    FATFS *fs = lookup_path(&path);
    if (fs == NULL) {
        return FR_NO_PATH;
    }
    return f_opendir(fs, dp, path);
}

FRESULT f_stat_helper(const TCHAR *path, FILINFO *fno) {
    FATFS *fs = lookup_path(&path);
    if (fs == NULL) {
        return FR_NO_PATH;
    }
    return f_stat(fs, path, fno);
}

FRESULT f_mkdir_helper(const TCHAR *path) {
    FATFS *fs = lookup_path(&path);
    if (fs == NULL) {
        return FR_NO_PATH;
    }
    return f_mkdir(fs, path);
}

FRESULT f_unlink_helper(const TCHAR *path) {
    FATFS *fs = lookup_path(&path);
    if (fs == NULL) {
        return FR_NO_PATH;
    }
    return f_unlink(fs, path);
}

FRESULT f_rename_helper(const TCHAR *path_old, const TCHAR *path_new) {
    FATFS *fs_old = lookup_path(&path_old);
    if (fs_old == NULL) {
        return FR_NO_PATH;
    }
    FATFS *fs_new = lookup_path(&path_new);
    if (fs_new == NULL) {
        return FR_NO_PATH;
    }
    if (fs_old != fs_new) {
        return FR_NO_PATH;
    }
    return f_rename(fs_new, path_old, path_new);
}
// When a sector boundary is encountered while writing a file and there are
// more than 512 bytes left to write FatFs will detect that it can bypass
// its internal write buffer and pass the data buffer passed to it directly
// to the disk write function. However, the disk write function needs the
// buffer to be aligned to a 4-byte boundary. FatFs doesn't know this and
// will pass an unaligned buffer if we don't fix the issue. To fix this problem
// we use a temporary buffer to fix the alignment and to speed everything up.

// We use this temporary buffer for both reads and writes. The buffer allows us
// to do multi-block reads and writes which signifcantly speed things up.

static uint32_t file_buffer_offset = 0;
static uint8_t *file_buffer_pointer = 0;
static uint32_t file_buffer_size = 0;
static uint32_t file_buffer_index = 0;

void file_buffer_init0()
{
    file_buffer_offset = 0;
    file_buffer_pointer = 0;
    file_buffer_size = 0;
    file_buffer_index = 0;
}

OMV_ATTR_ALWAYS_INLINE static void file_fill(FIL *fp)
{
    if (file_buffer_index == file_buffer_size) {
        file_buffer_pointer -= file_buffer_offset;
        file_buffer_size += file_buffer_offset;
        file_buffer_offset = 0;
        file_buffer_index = 0;
        uint32_t file_remaining = f_size(fp) - f_tell(fp);
        uint32_t can_do = FF_MIN(file_buffer_size, file_remaining);
        UINT bytes;
        FRESULT res = f_read(fp, file_buffer_pointer, can_do, &bytes);
        if (res != FR_OK) ff_fail(fp, res);
        if (bytes != can_do) ff_read_fail(fp);
    }
}

OMV_ATTR_ALWAYS_INLINE static void file_flush(FIL *fp)
{
    if (file_buffer_index == file_buffer_size) {
        UINT bytes;
        FRESULT res = f_write(fp, file_buffer_pointer, file_buffer_index, &bytes);
        if (res != FR_OK) ff_fail(fp, res);
        if (bytes != file_buffer_index) ff_write_fail(fp);
        file_buffer_pointer -= file_buffer_offset;
        file_buffer_size += file_buffer_offset;
        file_buffer_offset = 0;
        file_buffer_index = 0;
    }
}

uint32_t file_tell_w_buf(FIL *fp)
{
    if (fp->flag & FA_READ) {
        return f_tell(fp) - file_buffer_size + file_buffer_index;
    } else {
        return f_tell(fp) + file_buffer_index;
    }
}

uint32_t file_size_w_buf(FIL *fp)
{
    if (fp->flag & FA_READ) {
        return f_size(fp);
    } else {
        return f_size(fp) + file_buffer_index;
    }
}

void file_buffer_on(FIL *fp)
{
    file_buffer_offset = f_tell(fp) % 4;
    file_buffer_pointer = fb_alloc_all(&file_buffer_size, FB_ALLOC_PREFER_SIZE) + file_buffer_offset;
    if (!file_buffer_size) {
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("No memory!"));
    }
    file_buffer_size -= file_buffer_offset;
    file_buffer_index = 0;
    if (fp->flag & FA_READ) {
        uint32_t file_remaining = f_size(fp) - f_tell(fp);
        uint32_t can_do = FF_MIN(file_buffer_size, file_remaining);
        UINT bytes;
        FRESULT res = f_read(fp, file_buffer_pointer, can_do, &bytes);
        if (res != FR_OK) ff_fail(fp, res);
        if (bytes != can_do) ff_read_fail(fp);
    }
}

void file_buffer_off(FIL *fp)
{
    if ((fp->flag & FA_WRITE) && file_buffer_index) {
        UINT bytes;
        FRESULT res = f_write(fp, file_buffer_pointer, file_buffer_index, &bytes);
        if (res != FR_OK) ff_fail(fp, res);
        if (bytes != file_buffer_index) ff_write_fail(fp);
    }
    file_buffer_pointer = 0;
    fb_free();
}

void read_byte(FIL *fp, uint8_t *value)
{
    if (file_buffer_pointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // via massive reads. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(*value); i++) {
            file_fill(fp);
            ((uint8_t *) value)[i] = file_buffer_pointer[file_buffer_index++];
        }
    } else {
        UINT bytes;
        FRESULT res = f_read(fp, value, sizeof(*value), &bytes);
        if (res != FR_OK) ff_fail(fp, res);
        if (bytes != sizeof(*value)) ff_read_fail(fp);
    }
}

void read_byte_expect(FIL *fp, uint8_t value)
{
    uint8_t compare;
    read_byte(fp, &compare);
    if (value != compare) ff_expect_fail(fp);
}

void read_byte_ignore(FIL *fp)
{
    uint8_t trash;
    read_byte(fp, &trash);
}

void read_word(FIL *fp, uint16_t *value)
{
    if (file_buffer_pointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // via massive reads. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(*value); i++) {
            file_fill(fp);
            ((uint8_t *) value)[i] = file_buffer_pointer[file_buffer_index++];
        }
    } else {
        UINT bytes;
        FRESULT res = f_read(fp, value, sizeof(*value), &bytes);
        if (res != FR_OK) ff_fail(fp, res);
        if (bytes != sizeof(*value)) ff_read_fail(fp);
    }
}

void read_word_expect(FIL *fp, uint16_t value)
{
    uint16_t compare;
    read_word(fp, &compare);
    if (value != compare) ff_expect_fail(fp);
}

void read_word_ignore(FIL *fp)
{
    uint16_t trash;
    read_word(fp, &trash);
}

void read_long(FIL *fp, uint32_t *value)
{
    if (file_buffer_pointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // via massive reads. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(*value); i++) {
            file_fill(fp);
            ((uint8_t *) value)[i] = file_buffer_pointer[file_buffer_index++];
        }
    } else {
        UINT bytes;
        FRESULT res = f_read(fp, value, sizeof(*value), &bytes);
        if (res != FR_OK) ff_fail(fp, res);
        if (bytes != sizeof(*value)) ff_read_fail(fp);
    }
}

void read_long_expect(FIL *fp, uint32_t value)
{
    uint32_t compare;
    read_long(fp, &compare);
    if (value != compare) ff_expect_fail(fp);
}

void read_long_ignore(FIL *fp)
{
    uint32_t trash;
    read_long(fp, &trash);
}

void read_data(FIL *fp, void *data, UINT size)
{
    if (file_buffer_pointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // via massive reads. So much so that the time wasted by
        // all these operations does not cost us.
        while (size) {
            file_fill(fp);
            uint32_t file_buffer_space_left = file_buffer_size - file_buffer_index;
            uint32_t can_do = FF_MIN(size, file_buffer_space_left);
            memcpy(data, file_buffer_pointer+file_buffer_index, can_do);
            file_buffer_index += can_do;
            data += can_do;
            size -= can_do;
        }
    } else {
        UINT bytes;
        FRESULT res = f_read(fp, data, size, &bytes);
        if (res != FR_OK) ff_fail(fp, res);
        if (bytes != size) ff_read_fail(fp);
    }
}

void write_byte(FIL *fp, uint8_t value)
{
    if (file_buffer_pointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // before a write to the SD card. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(value); i++) {
            file_buffer_pointer[file_buffer_index++] = ((uint8_t *) &value)[i];
            file_flush(fp);
        }
    } else {
        UINT bytes;
        FRESULT res = f_write(fp, &value, sizeof(value), &bytes);
        if (res != FR_OK) ff_fail(fp, res);
        if (bytes != sizeof(value)) ff_write_fail(fp);
    }
}

void write_word(FIL *fp, uint16_t value)
{
    if (file_buffer_pointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // before a write to the SD card. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(value); i++) {
            file_buffer_pointer[file_buffer_index++] = ((uint8_t *) &value)[i];
            file_flush(fp);
        }
    } else {
        UINT bytes;
        FRESULT res = f_write(fp, &value, sizeof(value), &bytes);
        if (res != FR_OK) ff_fail(fp, res);
        if (bytes != sizeof(value)) ff_write_fail(fp);
    }
}

void write_long(FIL *fp, uint32_t value)
{
    if (file_buffer_pointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // before a write to the SD card. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(value); i++) {
            file_buffer_pointer[file_buffer_index++] = ((uint8_t *) &value)[i];
            file_flush(fp);
        }
    } else {
        UINT bytes;
        FRESULT res = f_write(fp, &value, sizeof(value), &bytes);
        if (res != FR_OK) ff_fail(fp, res);
        if (bytes != sizeof(value)) ff_write_fail(fp);
    }
}

void write_data(FIL *fp, const void *data, UINT size)
{
    if (file_buffer_pointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // before a write to the SD card. So much so that the time wasted by
        // all these operations does not cost us.
        while (size) {
            uint32_t file_buffer_space_left = file_buffer_size - file_buffer_index;
            uint32_t can_do = FF_MIN(size, file_buffer_space_left);
            memcpy(file_buffer_pointer+file_buffer_index, data, can_do);
            file_buffer_index += can_do;
            data += can_do;
            size -= can_do;
            file_flush(fp);
        }
    } else {
        UINT bytes;
        FRESULT res = f_write(fp, data, size, &bytes);
        if (res != FR_OK) ff_fail(fp, res);
        if (bytes != size) ff_write_fail(fp);
    }
}
#endif //IMLIB_ENABLE_IMAGE_FILE_IO
