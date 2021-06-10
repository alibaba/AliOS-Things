
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "quickjs.h"
#include "amp_config.h"
#include "amp_defines.h"
#include "amp_list.h"
#include "quickjs-libc.h"
#include "aos_system.h"

#define MOD_STR "quickjs_module_load"
static dlist_t g_prebuild_module_list = AMP_DLIST_HEAD_INIT(g_prebuild_module_list);

extern JSValue js_load_binary_module(JSContext *ctx, const uint8_t *buf, size_t buf_len);

typedef struct {
    char *name;
    uint8_t *module_data;
    uint32_t module_len;
    dlist_t node;
} prebuild_module_info_t;

void quickjs_add_prebuild_module(char *module_name, uint8_t *module_data, uint32_t module_len)
{
    prebuild_module_info_t *module_info = NULL;
    module_info = aos_malloc(sizeof(prebuild_module_info_t));
    if (!module_info) {
        aos_printf("alloc gpio_module_info info fail!");
        return;
    }
    module_info->name = module_name;
    module_info->module_data = module_data;
    module_info->module_len  = module_len;
    dlist_add_tail(&module_info->node, &g_prebuild_module_list);
}

static uint8_t *find_prebuild_module(char *module_name, uint32_t *module_len)
{
    prebuild_module_info_t *module_info = NULL;
    dlist_for_each_entry(&g_prebuild_module_list, module_info, prebuild_module_info_t, node) {
        if(strcmp(module_info->name, module_name) == 0) {
            *module_len = module_info->module_len;
            return module_info->module_data;
        }
    }

    return NULL;
}

JSModuleDef * quickjs_module_loader(JSContext *ctx,
                                          const char *module_name, void *opaque)
{
    size_t buf_len = 0;
    uint8_t *buf = NULL;
    JSModuleDef *m = NULL;
    JSValue func_val = JS_UNDEFINED;
    char *path = NULL;
    int path_flag = 0;

    aos_printf("begin load module %s\n", module_name);

    if(strchr(module_name, '/') != NULL) {
        if(*module_name != '/') {
            buf_len = strlen(AMP_FS_ROOT_DIR) + 1 + strlen(module_name) + 1;
            path = aos_malloc(buf_len);
            memset(path, 0, buf_len);
            snprintf(path, buf_len, "%s/%s", AMP_FS_ROOT_DIR, module_name);
            path_flag = 1;
        } else {
            buf_len = strlen(module_name) + 1;
            path = module_name;
        }
        buf = js_load_file(ctx, &buf_len, path);
        if (!buf) {
            JS_ThrowReferenceError(ctx, "could not load module filename '%s'", module_name);
            if(path_flag)aos_free(path);
            return NULL;
        }

        /* compile the module */
        func_val = JS_Eval(ctx, (char *)buf, buf_len, module_name,
                        JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_COMPILE_ONLY);
        js_free(ctx, buf);
        if(path_flag)aos_free(path);
    } else {
        uint8_t *module_data = NULL;
        uint32_t module_len  = 0;

        module_data = find_prebuild_module(module_name, &module_len);
        if (module_data != NULL) {
            aos_printf("find binary module %s, len %d, begin load\n", module_name, module_len);
            func_val = js_load_binary_module(ctx, module_data, module_len);
        } else {
            amp_error(MOD_STR, "find module %s fail\n", module_name);
        }
    }

    if (JS_IsException(func_val)) {
        return NULL;
    }

    /* the module is already referenced, so we must free it */
    m = JS_VALUE_GET_PTR(func_val);
    JS_FreeValue(ctx, func_val);
    if (m)
        aos_printf("load module %s success\n", module_name);

    return m;
}
