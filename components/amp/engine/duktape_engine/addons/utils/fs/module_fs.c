/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_fs.h"
#include "amp_defines.h"
#include "amp_system.h"
#include "wrappers_defs.h"
#include "be_inl.h"

#define MOD_STR "FS"
static int check_fs_is_support()
{
    int ret;
	void *fp;
    const char *string = "test if fs mount ok";
    char testfile[64]  = {0};
    char root_dir[128] = {0};

    HAL_Get_User_Dir(root_dir);
    snprintf(testfile, sizeof(testfile), "%s/%s", root_dir,
             "testfile.txt");
    fp = HAL_Fopen(testfile, "w+");
    if (fp == NULL) {
        amp_warn(MOD_STR, "check_fs_is_support open fail\n");
        return 0;
    }

    ret = HAL_Fwrite((char *)string, 1, strlen(string), fp);
    if (ret <= 0) {
        amp_warn(MOD_STR, "check_fs_is_support write fail\n");
        return 0;
    }

    HAL_Fclose(fp);

    ret = HAL_Funlink(testfile);
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
    void *fp   = NULL;
    int len  = 0;
    int size = 0;
    int32_t curpos = -1;
    const char *path;
    char *buf = NULL;

    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "invalid parameter\n");
        goto out;
    }

    path = duk_get_string(ctx, 0);
    fp   = HAL_Fopen(path, "r");
    if (fp == NULL) {
        amp_warn(MOD_STR, "jse_open failed\n");
        goto out;
    }

    HAL_Fseek(fp, 0, HAL_SEEK_END, &curpos);
    if (curpos < 0) {
        size = HAL_Ftell(fp);
    } else {
        size = curpos;
    }

    HAL_Fseek(fp, 0, SEEK_SET, &curpos);

    buf = (char *)amp_malloc(size + 1);
    if (!buf) {
        amp_warn(MOD_STR, "malloc failed\n");
        goto out;
    }

    len = HAL_Fread(buf, 1, size, fp);
    if (len > 0) {
        buf[len] = 0;
        amp_debug(MOD_STR, "read data: %s\n", buf);
    }
    HAL_Fclose(fp);

out:
    if (len > 0)
        duk_push_string(ctx, buf);
    else
        duk_push_undefined(ctx);
    amp_free(buf);
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
    void *fp = NULL;
    size_t str_len = 0;
    size_t nwrite  = 0;
    const char *path;
    const char *content;
    const char *flag;

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

    fp   = HAL_Fopen(path, flag);
    if (fp == NULL) {
        amp_error(MOD_STR, "be_osal_open fail\n");
        goto out;
    }

    nwrite = HAL_Fwrite((char *)content, 1, str_len, fp);
    if (nwrite <= 0) {
        amp_error(MOD_STR, "be_osal_write fail\n");
        HAL_Fclose(fp);
        goto out;
    }

    amp_debug(MOD_STR, "FS.write(%s,%s,%s);\n", path, content, flag);

    HAL_Fsync(fp);
    HAL_Fclose(fp);

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
    ret  = HAL_Funlink(path);

out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_fs_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_fs_is_support, 0);
    duk_put_prop_string(ctx, -2, "issupport");

    duk_push_c_function(ctx, native_fs_read, 1);
    duk_put_prop_string(ctx, -2, "read");

    duk_push_c_function(ctx, native_fs_write, 3);
    duk_put_prop_string(ctx, -2, "write");

    duk_push_c_function(ctx, native_fs_delete, 1);
    duk_put_prop_string(ctx, -2, "delete");

    duk_put_prop_string(ctx, -2, "FS");
}
