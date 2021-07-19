#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "amp_config.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"
#include "ulog/ulog.h"
#include "oss_app.h"

#define MOD_STR "MOD_OSS"
#define MAX_URL_LENGTH 256

static aos_sem_t g_oss_task_sem = NULL;
static JSClassID js_oss_class_id;
static char oss_resp_url[MAX_URL_LENGTH];

typedef struct {
    int flag;   //0: upload; 1: download; 
    char *key;
    char *secret;
    char *endPoint;
    char *bucketName;
    char *objectName;
    char *filePath;
}  oss_task_param;

static void oss_task_handler(void *arg)
{
    char *url = NULL;
    oss_task_param *task_param = (oss_task_param *)arg;

    if(task_param->flag == 0) {
        url = oss_upload_file(task_param->key, task_param->secret, task_param->endPoint, task_param->bucketName, task_param->objectName, task_param->filePath);
        memset(oss_resp_url, 0, MAX_URL_LENGTH);
        strncpy(oss_resp_url, url, MAX_URL_LENGTH);
    } else {
        oss_download_file(task_param->key, task_param->secret, task_param->endPoint, task_param->bucketName, task_param->objectName, task_param->filePath);
    }
    aos_printf("oss task finished! \n");
    
    aos_sem_signal(&g_oss_task_sem);    
}

static JSValue native_oss_upload_file(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret = -1;

    aos_task_t oss_task;
    oss_task_param task_param;

    if (argc < 6)
    {
        amp_error(MOD_STR, "%s: args num is illegal :n_args = %d;\n", __func__, argc);
        return JS_NewInt32(ctx, 0);
    }

    task_param.flag = 0;
    task_param.key = (char *)JS_ToCString(ctx, argv[0]);
    task_param.secret = (char *)JS_ToCString(ctx, argv[1]);
    task_param.endPoint = (char *)JS_ToCString(ctx, argv[2]);
    task_param.bucketName = (char *)JS_ToCString(ctx, argv[3]);
    task_param.objectName = (char *)JS_ToCString(ctx, argv[4]);
    task_param.filePath = (char *)JS_ToCString(ctx, argv[5]);

    // amp_debug(MOD_STR, "key = %s;\n", task_param.key);
    // amp_debug(MOD_STR, "secret = %s;\n", task_param.secret);
    amp_debug(MOD_STR, "endPoint = %s;\n", task_param.endPoint);
    amp_debug(MOD_STR, "bucketName = %s;\n", task_param.bucketName);
    amp_debug(MOD_STR, "objectName = %s;\n", task_param.objectName);
    amp_debug(MOD_STR, "filePath = %s;\n", task_param.filePath);

    aos_task_new_ext(&oss_task, "amp oss task", oss_task_handler, &task_param, 1024 * 10, AOS_DEFAULT_APP_PRI);
    aos_sem_wait(&g_oss_task_sem, AOS_WAIT_FOREVER);

    JS_FreeCString(ctx, task_param.key);
    JS_FreeCString(ctx, task_param.secret);
    JS_FreeCString(ctx, task_param.endPoint);
    JS_FreeCString(ctx, task_param.bucketName);
    JS_FreeCString(ctx, task_param.objectName);
    JS_FreeCString(ctx, task_param.filePath);

    aos_printf("oss upload finished! %s\n", oss_resp_url);

    return JS_NewString(ctx, oss_resp_url);
}

static JSValue native_oss_download_file(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret = -1;
    char * url = NULL;
    aos_task_t oss_task;
    oss_task_param task_param;

    if (argc < 5)
    {
        amp_error(MOD_STR, "%s: args num is illegal :n_args = %d;\n", __func__, argc);
        return JS_NewInt32(ctx, 0);
    }

    task_param.flag = 1;
    task_param.key = (char *)JS_ToCString(ctx, argv[0]);
    task_param.secret = (char *)JS_ToCString(ctx, argv[1]);
    task_param.endPoint = (char *)JS_ToCString(ctx, argv[2]);
    task_param.bucketName = (char *)JS_ToCString(ctx, argv[3]);
    task_param.objectName = (char *)JS_ToCString(ctx, argv[4]);
    task_param.filePath = (char *)JS_ToCString(ctx, argv[5]);

    // amp_debug(MOD_STR, "key = %s;\n", task_param.key);
    // amp_debug(MOD_STR, "secret = %s;\n", task_param.secret);
    amp_debug(MOD_STR, "endPoint = %s;\n", task_param.endPoint);
    amp_debug(MOD_STR, "bucketName = %s;\n", task_param.bucketName);
    amp_debug(MOD_STR, "objectName = %s;\n", task_param.objectName);
    amp_debug(MOD_STR, "filePath = %s;\n", task_param.filePath);

    aos_task_new_ext(&oss_task, "amp oss task", oss_task_handler, &task_param, 1024 * 10, AOS_DEFAULT_APP_PRI);
    aos_sem_wait(&g_oss_task_sem, AOS_WAIT_FOREVER);

    JS_FreeCString(ctx, task_param.key);
    JS_FreeCString(ctx, task_param.secret);
    JS_FreeCString(ctx, task_param.endPoint);
    JS_FreeCString(ctx, task_param.bucketName);
    JS_FreeCString(ctx, task_param.objectName);
    JS_FreeCString(ctx, task_param.filePath);

    aos_printf("oss download finished!\n");

    return JS_NewInt32(ctx, 0);
}


static JSClassDef js_oss_class = {
    "oss",
};

static const JSCFunctionListEntry js_oss_funcs[] = {
    JS_CFUNC_DEF("uploadFile", 5, native_oss_upload_file ),
    JS_CFUNC_DEF("downloadFile", 5, native_oss_download_file ),
};

static int js_oss_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_oss_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_oss_class_id, &js_oss_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_oss_funcs,
                               countof(js_oss_funcs));
    JS_SetClassProto(ctx, js_oss_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_oss_funcs,
                                  countof(js_oss_funcs));
}

JSModuleDef *js_init_module_oss(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_oss_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_oss_funcs, countof(js_oss_funcs));
    return m;
}

void module_oss_register(void)
{
    amp_debug(MOD_STR, "module_oss_register");
    if (!g_oss_task_sem) {
        if (aos_sem_new(&g_oss_task_sem, 0) != 0) {
            amp_error(MOD_STR, "create amp oss sem fail");
        }
    }

    JSContext *ctx = js_get_context();

    js_init_module_oss(ctx, "oss");     
}


