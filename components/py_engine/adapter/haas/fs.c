#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

#define LOG_TAG "SYSTEM_FS"

extern const mp_obj_type_t system_fs_type;
extern mp_int_t amp_get_user_dir(char *dir);
// this is the actual C-structure for our new object

static mp_int_t check_fs_is_support()
{
    mp_int_t ret;
    void *fp;
    const char *string = "test if fs mount ok";
    char testfile[64] = { 0 };
    char root_dir[128] = { 0 };
#if 0
    amp_get_user_dir(root_dir);
    snprintf(testfile, sizeof(testfile), "%s/%s", root_dir,
             "testfile.txt");
    fp = amp_fopen(testfile, "w+");
    if (fp == NULL) {
        LOGE(LOG_TAG, "check_fs_is_support open fail\n");
        return 0;
    }

    ret = amp_fwrite((char *)string, 1, strlen(string), fp);
    if (ret <= 0) {
        LOGE(LOG_TAG, "check_fs_is_support write fail\n");
        amp_fclose(fp);
        return 0;
    }

    amp_fclose(fp);

    ret = amp_remove(testfile);
    if (ret) {
        LOGE(LOG_TAG, "check_fs_is_support sync fail\n");
        return 0;
    }
#endif
    return 1;
}

typedef struct {
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
} mp_fs_obj_t;

void fs_obj_print(const mp_print_t *print, mp_obj_t self_in,
                  mp_print_kind_t kind)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
    mp_fs_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t fs_obj_make_new(const mp_obj_type_t *type, size_t n_args,
                                size_t n_kw, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s;\n", __func__);
    mp_fs_obj_t *driver_obj = m_new_obj(mp_fs_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(MP_EINVAL);
    }

    driver_obj->Base.type = &system_fs_type;
    driver_obj->ModuleName = "fs";

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    mp_int_t ret = -1;
    if (n_args < 1) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__,
             n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_fs_obj_t *driver_obj = (mp_fs_obj_t *)self;
    if (driver_obj == NULL) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return mp_const_none;
    }
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(fs_obj_open, 1, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    mp_int_t ret = -1;
    if (n_args < 1) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__,
             n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_fs_obj_t *driver_obj = (mp_fs_obj_t *)self;
    if (driver_obj == NULL) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return mp_const_none;
    }
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(fs_obj_close, 1, obj_close);

STATIC mp_obj_t obj_issupport(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    mp_int_t ret = -1;
    if (n_args < 1) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__,
             n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_fs_obj_t *driver_obj = (mp_fs_obj_t *)self;
    if (driver_obj == NULL) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return mp_const_none;
    }

    ret = check_fs_is_support();
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(fs_obj_issupport, 1, obj_issupport);

STATIC mp_obj_t obj_read(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    mp_int_t ret = -1;
    void *fp = NULL;
    mp_int_t len = 0;
    mp_int_t size = 0;
    mp_int_t curpos = -1;
    char *path;
    char *buf = NULL;
    if (n_args < 2) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__,
             n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_fs_obj_t *driver_obj = (mp_fs_obj_t *)self;
    if (driver_obj == NULL) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return mp_const_none;
    }

    path = (char *)mp_obj_str_get_str(args[1]);
    LOGD(LOG_TAG, "%s: path = %s;\n", __func__, path);
#if 0
    fp   = amp_fopen(path, "r");
    if (fp == NULL) {
        LOGE(LOG_TAG, "jse_open failed\n");
        goto out;
    }

    amp_fseek(fp, 0, HAL_SEEK_END, &curpos);
    if (curpos < 0) {
        size = amp_ftell(fp);
    } else {
        size = curpos;
    }

    amp_fseek(fp, 0, HAL_SEEK_SET, &curpos);

    buf = (char *)amp_malloc(size + 1);
    if (!buf) {
        LOGE(LOG_TAG, "malloc failed\n");
        amp_fclose(fp);
        goto out;
    }

    len = amp_fread(buf, 1, size, fp);
    if (len > 0) {
        buf[len] = 0;
        LOGE(LOG_TAG, "read data: %s\n", buf);
    }
    amp_fclose(fp);

    mp_obj_base_t *content = MP_ROM_QSTR(buf);
out:
    amp_free(buf);
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return content;
#else
    return mp_const_none;
#endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(fs_obj_read, 2, obj_read);

STATIC mp_obj_t obj_write(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    mp_int_t ret = -1;
    void *fp = NULL;
    size_t str_len = 0;
    size_t nwrite = 0;
    char *path;
    char *content;
    char *flag;

    if (n_args < 4) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__,
             n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_fs_obj_t *driver_obj = (mp_fs_obj_t *)self;
    if (driver_obj == NULL) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return mp_const_none;
    }

    path = (char *)mp_obj_str_get_str(args[1]);
    flag = (char *)mp_obj_str_get_str(args[2]);
    LOGD(LOG_TAG, "%s:path = %s;flag = %s;\n", __func__, path, flag);
#if 0
	mp_buffer_info_t src;
    mp_get_buffer_raise(args[3], &src, MP_BUFFER_READ);
    LOGD(LOG_TAG, "%s:src.buf = %p;src.len = %d;\n", __func__, src.buf, src.len);
    content = (char*)src.buf;
    str_len = src.len;

    fp   = amp_fopen(path, flag);
    if (fp == NULL) {
        LOGE(LOG_TAG, "be_osal_open fail\n");
        return mp_const_none;
    }

    nwrite = amp_fwrite((char *)content, 1, str_len, fp);
    if (nwrite <= 0) {
        LOGE(LOG_TAG, "be_osal_write fail\n");
        amp_fclose(fp);
        return mp_const_none;
    }

    LOGD(LOG_TAG, "FS.write(%s,%s,%s);\n", path, content, flag);

    amp_fsync(fp);
    amp_fclose(fp);
#endif
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(fs_obj_write, 4, obj_write);

STATIC mp_obj_t obj_delete(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    mp_int_t ret = -1;
    char *path;
    if (n_args < 2) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__,
             n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_fs_obj_t *driver_obj = (mp_fs_obj_t *)self;
    if (driver_obj == NULL) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return mp_const_none;
    }
    path = (char *)mp_obj_str_get_str(args[1]);
    LOGD(LOG_TAG, "%s:path = %s;\n", __func__, path);
    // ret  = amp_remove(path);
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(fs_obj_delete, 2, obj_delete);

STATIC const mp_rom_map_elem_t fs_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_FS) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&fs_obj_open) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&fs_obj_close) },
    { MP_ROM_QSTR(MP_QSTR_issupport), MP_ROM_PTR(&fs_obj_issupport) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&fs_obj_read) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&fs_obj_write) },
    { MP_ROM_QSTR(MP_QSTR_delete), MP_ROM_PTR(&fs_obj_delete) },
};

STATIC MP_DEFINE_CONST_DICT(fs_locals_dict, fs_locals_dict_table);

const mp_obj_type_t system_fs_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_FS,
    .print = fs_obj_print,
    .make_new = fs_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&fs_locals_dict,
};
