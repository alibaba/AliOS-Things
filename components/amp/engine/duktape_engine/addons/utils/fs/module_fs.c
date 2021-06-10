/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_config.h"
#include "aos_fs.h"
#include "amp_defines.h"
#include "aos_system.h"
#include "be_inl.h"

#define MOD_STR "FS"
static int check_fs_is_support()
{
    int ret;
	int fp;
    const char *string = "test if fs mount ok";
    char testfile[64]  = {0};

    snprintf(testfile, sizeof(testfile), AMP_FS_ROOT_DIR"/%s", "testfile.txt");
    fp = aos_open(testfile, O_RDWR | O_CREAT | O_TRUNC);
    if (fp < 0) {
        amp_warn(MOD_STR, "check_fs_is_support open fail\n");
        return 0;
    }

    ret = aos_write(fp, (char *)string, strlen(string));
    if (ret <= 0) {
        amp_warn(MOD_STR, "check_fs_is_support write fail\n");
        aos_close(fp);
        return 0;
    }

    aos_close(fp);

    ret = aos_remove(testfile);
    if (ret) {
        amp_warn(MOD_STR, "check_fs_is_support sync fail\n");
        return 0;
    }

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
    int fp   = -1;
    int len  = 0;
    int size = 0;
    const char *path;
    char *buf = NULL;

    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "invalid parameter\n");
        goto out;
    }

    path = duk_get_string(ctx, 0);
    fp   = aos_open(path, O_RDONLY);
    if (fp < 0) {
        amp_warn(MOD_STR, "jse_open failed\n");
        goto out;
    }

    size = aos_lseek(fp, 0, HAL_SEEK_END);

    aos_lseek(fp, 0, HAL_SEEK_SET);

    buf = (char *)aos_malloc(size + 1);
    if (!buf) {
        amp_warn(MOD_STR, "malloc failed\n");
        aos_close(fp);
        goto out;
    }

    len = aos_read(fp, buf, size);
    if (len > 0) {
        buf[len] = 0;
        amp_debug(MOD_STR, "read data: %s\n", buf);
    }
    aos_close(fp);

out:
    if (len > 0)
        duk_push_string(ctx, buf);
    else
        duk_push_undefined(ctx);
    aos_free(buf);
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
    int ret = -1;
    int fp = -1;
    size_t str_len = 0;
    size_t nwrite  = 0;
    const char *path;
    const char *content;
    const char *flag;
    int   aos_flag = 0, m = 0;

    if (!duk_is_string(ctx, 0) || !duk_is_string(ctx, 1) || !duk_is_object(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be [string, string, object]\n");
        goto out;
    }

    path = duk_get_string(ctx, 0);
    content = duk_get_lstring(ctx, 1, &str_len);

 /* get device certificate */
    duk_get_prop_string(ctx, 2, "flag");

    if (!duk_is_string(ctx, -1))
    {
        amp_warn(MOD_STR, "Parameter must be an object like {flag: string}");
        ret = -1;
        goto out;
    }

    flag  = duk_get_string(ctx, -1);

    switch (flag[0]) {
        case 'r':
            m = O_RDONLY;
            aos_flag = 0;
            break;

        case 'w':
            m = O_WRONLY;
            aos_flag = O_CREAT | O_TRUNC;
            break;

        case 'a':
            m = O_WRONLY;
            aos_flag = O_CREAT | O_APPEND;
            break;
         default:
            return -1;
            goto out;
    }

    while (*++flag) {
        switch (*flag) {
            case '+':
                m = (m & ~O_ACCMODE) | O_RDWR;
                break;
            default:
                break;
        }
    }
    aos_flag = m | aos_flag;

    fp   = aos_open(path, aos_flag);
    if (fp < 0) {
        amp_error(MOD_STR, "be_osal_open fail\n");
        goto out;
    }

    nwrite = aos_write(fp, (char *)content, str_len);
    if (nwrite <= 0) {
        amp_error(MOD_STR, "be_osal_write fail\n");
        aos_close(fp);
        goto out;
    }

    amp_debug(MOD_STR, "FS.write(%s,%s,%s);\n", path, content, flag);

    aos_sync(fp);
    aos_close(fp);

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
        amp_warn(MOD_STR, "invalid parameter\n");
        goto out;
    }

    path = duk_get_string(ctx, 0);
    ret  = aos_remove(path);

out:
    duk_push_int(ctx, ret);
    return 1;
}

/*****************************************************************************
 * Function:    native_fs_totalsize
 * Description: js native addon for FILE.totalsize()
 *            get file system total size in bytes
 * Called by:   js api
 * Output:      file system total size in bytes; negative if failed.
 *****************************************************************************/
static duk_ret_t native_fs_totalsize(duk_context *ctx)
{
    int ret = -1;
    int size;
    struct aos_statfs buf;

    aos_statfs(AMP_FS_ROOT_DIR, &buf);

    size = buf.f_bsize * buf.f_blocks;
    if (size < 0) {
        amp_error(MOD_STR, "get fs totalsize fail");
        goto out;
    }
    ret = size;
out:
    duk_push_int(ctx, ret);
    return 1;
}

/*****************************************************************************
 * Function:    native_fs_usedsize
 * Description: js native addon for FILE.usedsize()
 *            get file system used size in bytes
 * Called by:   js api
 * Output:      file system used size in bytes; negative if failed.
 *****************************************************************************/
static duk_ret_t native_fs_usedsize(duk_context *ctx)
{
    int ret = -1;
    int size;
    struct aos_statfs buf;

    aos_statfs(AMP_FS_ROOT_DIR, &buf);

    size = buf.f_bsize * buf.f_blocks - buf.f_bsize * buf.f_bavail;
    if (size < 0) {
        amp_error(MOD_STR, "get fs usedsize fail");
        goto out;
    }
    ret = size;
out:
    duk_push_int(ctx, ret);
    return 1;
}

/*****************************************************************************
 * Function:    native_fs_freesize
 * Description: js native addon for FILE.freesize()
 *            get file system free size in bytes
 * Called by:   js api
 * Output:      file system free size in bytes; negative if failed.
 *****************************************************************************/
static duk_ret_t native_fs_freesize(duk_context *ctx)
{
    int ret = -1;
    int size;
    struct aos_statfs buf;

    aos_statfs(AMP_FS_ROOT_DIR, &buf);

    size = buf.f_bsize * buf.f_bavail;
    if (size < 0) {
        amp_error(MOD_STR, "get fs freesize fail");
        goto out;
    }
    ret = size;
out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_fs_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("issupport", native_fs_is_support, 0);
    AMP_ADD_FUNCTION("read",      native_fs_read, 1);
    AMP_ADD_FUNCTION("write",     native_fs_write, 3);
    AMP_ADD_FUNCTION("delete",    native_fs_delete, 1);
    AMP_ADD_FUNCTION("totalsize", native_fs_totalsize, 0);
    AMP_ADD_FUNCTION("usedsize",  native_fs_usedsize, 0);
    AMP_ADD_FUNCTION("freesize",  native_fs_freesize, 0);

    duk_put_prop_string(ctx, -2, "FS");
}
