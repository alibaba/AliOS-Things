/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_config.h"
#include "aos_fs.h"
#include "aos/vfs.h"
#include "amp_defines.h"
#include "aos_system.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "FS"

static JSClassID js_fs_class_id;

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
static JSValue native_fs_is_support(JSContext *ctx, JSValueConst this_val,
                                    int argc, JSValueConst *argv)
{
    int ret;
    ret = check_fs_is_support();
    return JS_NewInt32(ctx, ret);
}

/*****************************************************************************
 * Function:    native_fs_read
 * Description: js native addon for FILE.read(filepath)
 *            read the file content
 * Called by:   js api
 * Input:       filepath : string
 * Output:      a String object which the file content is
 *****************************************************************************/
static JSValue native_fs_read(JSContext *ctx, JSValueConst this_val,
                              int argc, JSValueConst *argv)
{
    int fp   = -1;
    int len  = 0;
    int size = 0;
    const char *path;
    char *buf = NULL;
    JSValue response = JS_UNDEFINED;

    if (!JS_IsString(argv[0])) {
        amp_error(MOD_STR, "parameter must be [string]\n");
        goto out;
    }

    path = JS_ToCString(ctx, argv[0]);
    if (path == NULL) {
        amp_error(MOD_STR, "get path fail!");
        goto out;
    }

    fp = aos_open(path, O_RDONLY);
    if (fp < 0) {
        amp_warn(MOD_STR, "be_osal_open failed\n");
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
    if (path != NULL) {
        JS_FreeCString(ctx, path);
    }
    if (len > 0) {
        response = JS_NewStringLen(ctx, buf, len);
    }
    if (buf != NULL) {
        aos_free(buf);
    }
    return response;
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
static JSValue native_fs_write(JSContext *ctx, JSValueConst this_val,
                               int argc, JSValueConst *argv)
{
    int ret = -1;
    int fp = -1;
    size_t str_len = 0;
    size_t nwrite  = 0;
    const char *path;
    const char *content;
    const char *flag, *flag_s;
    JSValue obj;
    int   aos_flag = 0, m = 0;

    path = JS_ToCString(ctx, argv[0]);
    if (path == NULL) {
        amp_error(MOD_STR, "get fs path fail!");
        goto out;
    }

    content = JS_ToCStringLen(ctx, &str_len, argv[1]);
    if (content == NULL) {
        amp_error(MOD_STR, "get fs content fail!");
        goto out;
    }

    /* get option */
    obj = JS_GetPropertyStr(ctx, argv[2], "flag");
    if (JS_IsException(obj)) {
        amp_error(MOD_STR, "Parameter must be an object like {flag: string}");
        goto out;
    }

    flag   = JS_ToCString(ctx, obj);
    flag_s = flag;
    if (flag == NULL) {
        amp_error(MOD_STR, "get fs flag fail!");
        goto out;
    }

    /* transfor flag to aos_flag */
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

    fp = aos_open(path, aos_flag);
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

    amp_debug(MOD_STR, "FS.write(%s,%s,%s);\n", path, content, flag_s);

    aos_sync(fp);
    aos_close(fp);

    ret = (nwrite == str_len) ? 0 : -1;

out:
    if (path != NULL) {
        JS_FreeCString(ctx, path);
    }
    if (content != NULL) {
        JS_FreeCString(ctx, content);
    }
    if (flag_s != NULL) {
        JS_FreeCString(ctx, flag_s);
    }
    return JS_NewInt32(ctx, ret);
}

/*****************************************************************************
 * Function:    native_fs_delete
 * Description: js native addon for FILE.delete(filepath)
 *            delete the file
 * Called by:   js api
 * Input:       filepath : string
 * Output:      0 delete ok ;other delete fail
 *****************************************************************************/
static JSValue native_fs_delete(JSContext *ctx, JSValueConst this_val,
                                int argc, JSValueConst *argv)
{
    int ret = -1;
    const char *path;

    if (!JS_IsString(argv[0])) {
        amp_warn(MOD_STR, "parameter must be [string]\n");
        goto out;
    }

    path = JS_ToCString(ctx, argv[0]);
    if (path == NULL) {
        amp_error(MOD_STR, "get path fail!");
        goto out;
    }

    ret  = aos_remove(path);

out:
    if (path != NULL) {
        JS_FreeCString(ctx, path);
    }
    return JS_NewInt32(ctx, ret);
}

/*****************************************************************************
 * Function:    native_fs_totalsize
 * Description: js native addon for FILE.totalsize()
 *            get file system total size in bytes
 * Called by:   js api
 * Output:      file system total size in bytes; negative if failed.
 *****************************************************************************/
static JSValue native_fs_totalsize(JSContext *ctx, JSValueConst this_val,
                                   int argc, JSValueConst *argv)
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
    return JS_NewInt32(ctx, ret);
}

/*****************************************************************************
 * Function:    native_fs_usedsize
 * Description: js native addon for FILE.usedsize()
 *            get file system used size in bytes
 * Called by:   js api
 * Output:      file system used size in bytes; negative if failed.
 *****************************************************************************/
static JSValue native_fs_usedsize(JSContext *ctx, JSValueConst this_val,
                                  int argc, JSValueConst *argv)
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
    return JS_NewInt32(ctx, ret);
}

/*****************************************************************************
 * Function:    native_fs_freesize
 * Description: js native addon for FILE.freesize()
 *            get file system free size in bytes
 * Called by:   js api
 * Output:      file system free size in bytes; negative if failed.
 *****************************************************************************/
static JSValue native_fs_freesize(JSContext *ctx, JSValueConst this_val,
                                  int argc, JSValueConst *argv)
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
    return JS_NewInt32(ctx, ret);
}

static JSClassDef js_fs_class = {
    "FS",
};

static const JSCFunctionListEntry js_fs_funcs[] = {
    JS_CFUNC_DEF("read", 1, native_fs_read),
    JS_CFUNC_DEF("write", 2, native_fs_write),
    JS_CFUNC_DEF("delete", 1, native_fs_delete),
    JS_CFUNC_DEF("totalSize", 1, native_fs_totalsize),
    JS_CFUNC_DEF("usedSize", 1, native_fs_usedsize),
    JS_CFUNC_DEF("freeSize", 1, native_fs_freesize),
};

static int js_fs_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_fs_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_fs_class_id, &js_fs_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_fs_funcs,
                               countof(js_fs_funcs));
    JS_SetClassProto(ctx, js_fs_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_fs_funcs,
                                  countof(js_fs_funcs));
}

JSModuleDef *js_init_module_fs(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_fs_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_fs_funcs, countof(js_fs_funcs));
    return m;
}

void module_fs_register(void)
{
    amp_debug(MOD_STR, "module_fs_register");
    JSContext *ctx = js_get_context();
    aos_printf("module fs register\n");
    js_init_module_fs(ctx, "FS");
}
