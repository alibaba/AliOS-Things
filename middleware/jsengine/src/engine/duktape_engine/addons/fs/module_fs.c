/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* #define LOG_NDEBUG 0 */
#include "hal/log.h"
#include "be_port_osal.h"
#include "bone_engine_inl.h"
#include "hal/system.h"

static int convert_fs_mode_to_oflag(const char *mode)
{
    if (strcmp(mode, "w") == 0) return O_WRONLY | O_CREAT | O_TRUNC;
    if (strcmp(mode, "r") == 0) return O_RDONLY;
    if (strcmp(mode, "w+") == 0) return O_RDWR | O_CREAT | O_TRUNC;
    if (strcmp(mode, "r+") == 0) return O_RDWR;
    if (strcmp(mode, "a") == 0) return O_RDWR | O_CREAT | O_APPEND;
    if (strcmp(mode, "a+") == 0) return O_RDWR | O_CREAT | O_APPEND;

    return O_WRONLY | O_CREAT;
}

static int check_fs_is_support()
{
    int ret, fd;
    const char *string = "test if fs mount ok";
    char testfile[64]  = {0};

    snprintf(testfile, sizeof(testfile), "%s/%s", BE_FS_ROOT_DIR,
             "testfile.txt");
    fd = be_open(testfile, O_RDWR | O_CREAT | O_TRUNC);
    if (fd <= 0) {
        jse_warn("check_fs_is_support open fail\n");
        return 0;
    }

    ret = be_write(fd, string, strlen(string));
    if (ret <= 0) {
        jse_warn("check_fs_is_support write fail\n");
        return 0;
    }

    ret = be_unlink(testfile);
    if (ret) {
        jse_warn("check_fs_is_support sync fail\n");
        return 0;
    }

    be_close(fd);
    return 1;
}

/*****************************************************************************
 * Function:    native_fs_issupport
 * Description: js native addon for FILE.issupport()
 *            check if the js FILE object is support
 * Called by:   js api
 * Input:       none
 * Output:      1 :support ,0 :not support
 *****************************************************************************/
static duk_ret_t native_fs_is_support(duk_context *ctx)
{
    int ret;
    ret = check_fs_is_support();
    duk_push_int(ctx, ret);
    return 1;
}

/*****************************************************************************
 * Function:    native_fs_read
 * Description: js native addon for FILE.read(filepath)
 *            read the file content
 * Called by:   js api
 * Input:       filepath : string
 * Output:      a String object which the file content is
 *****************************************************************************/
static duk_ret_t native_fs_read(duk_context *ctx)
{
    int fd   = 0;
    int len  = 0;
    int size = 0;
    const char *path;
    char *buf = NULL;

    if (!duk_is_string(ctx, 0)) {
        jse_warn("invalid parameter\n");
        goto out;
    }

    path = duk_get_string(ctx, 0);
    fd   = be_open(path, O_RDONLY);
    if (fd < 0) {
        jse_warn("be_open failed, fd: %d\n", fd);
        goto out;
    }

    size = be_lseek(fd, 0, SEEK_END);
    be_lseek(fd, 0, SEEK_SET);

    buf = (char *)jse_malloc(size + 1);
    if (!buf) {
        jse_warn("malloc failed\n");
        goto out;
    }

    len = be_read(fd, buf, size);
    if (len > 0) {
        buf[len] = 0;
        jse_debug("read data: %s\n", buf);
    }
    be_close(fd);

out:
    if (len > 0)
        duk_push_string(ctx, buf);
    else
        duk_push_string(ctx, "");
    jse_free(buf);
    return 1;
}

/*****************************************************************************
 * Function:    native_fs_write
 * Description: js native addon for FILE.write(filepath,buffer,mode)
 *            write the content to the file
 * Called by:   js api
 * Input:       filepath : string ;
 *            buffer: the content buffer which is a String object
 *            mode: could be "w","r","w+","r+","a","a+"
 * Output:      0 write ok ;other write fail
 *****************************************************************************/
static duk_ret_t native_fs_write(duk_context *ctx)
{
    int fd;
    size_t str_len = 0;
    size_t nwrite  = 0;
    const char *path;
    const char *str;
    const char *mode;

    if (!duk_is_string(ctx, 0) || !duk_is_string(ctx, 1) ||
        !duk_is_string(ctx, 2)) {
        jse_warn("invalid parameter\n");
        goto out;
    }

    path = duk_get_string(ctx, 0);
    mode = duk_get_string(ctx, 2);
    fd   = be_open(path, convert_fs_mode_to_oflag(mode));
    if (fd < 0) {
        jse_error("be_osal_open fail\n");
        goto out;
    }

    str    = duk_get_lstring(ctx, 1, &str_len);
    nwrite = be_write(fd, str, str_len);
    if (nwrite <= 0) {
        jse_error("be_osal_write fail\n");
        be_close(fd);
        goto out;
    }

    jse_debug("FS.write(%s,%s,%s);\n", path, str, mode);

    be_sync(fd);
    be_close(fd);

out:
    duk_push_int(ctx, nwrite == str_len ? 0 : -1);
    return 1;
}

/*****************************************************************************
 * Function:    native_fs_delete
 * Description: js native addon for FILE.delete(filepath)
 *            delete the file
 * Called by:   js api
 * Input:       filepath : string
 * Output:      0 delete ok ;other delete fail
 *****************************************************************************/
static duk_ret_t native_fs_delete(duk_context *ctx)
{
    int ret = -1;
    const char *path;

    if (!duk_is_string(ctx, 0)) {
        jse_warn("invalid parameter\n");
        goto out;
    }

    path = duk_get_string(ctx, 0);
    ret  = be_unlink(path);

out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_fs_register(void)
{
    duk_context *ctx = bone_engine_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_fs_is_support, 0);
    duk_put_prop_string(ctx, -2, "issupport");

    duk_push_c_function(ctx, native_fs_read, 1);
    duk_put_prop_string(ctx, -2, "read");

    duk_push_c_function(ctx, native_fs_write, 3);
    duk_put_prop_string(ctx, -2, "write");

    duk_push_c_function(ctx, native_fs_delete, 1);
    duk_put_prop_string(ctx, -2, "delete");

    duk_put_global_string(ctx, "FS");
}
