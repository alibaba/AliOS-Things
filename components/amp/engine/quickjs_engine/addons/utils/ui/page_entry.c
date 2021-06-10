/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifdef AMP_ADVANCED_ADDON_UI

#include "aos_fs.h"
#include "aos_system.h"
#include "amp_config.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "board_mgr.h"
#include "quickjs.h"
#include "cJSON.h"
#include "page_entry.h"
#include "render.h"
#include "quickjs_addon_common.h"

#define MOD_STR "APPENTRY"

//static dlist_t g_pages_list;
static JSClassID js_page_entry_class_id;



extern char g_app_path[128];
extern amp_app_desc_t g_app;

char app_file_path[128];

int search_js_page_entry(amp_app_desc_t *app)
{
    int i;
    int size;
    cJSON *root     = NULL;
    cJSON *item     = NULL;
    cJSON *temp     = NULL;
    void *json_data = NULL;

    void * js_app_fd = NULL;
    int file_len  = 0;
    int json_fd   = -1;

    page_desc_t *page;

    if (app == NULL) {
        return -1;
    }

    snprintf(app_file_path, 128, AMP_APP_MAIN_JSON);
    /* cannot find the index.js int current dir */
    if ((json_fd = aos_open(app_file_path, O_RDONLY)) < 0) {
        amp_error(MOD_STR, "cannot find the file :%s", app_file_path);
        return -1;
    }

    /* read package config file to json_data buffer */
    file_len = aos_lseek(json_fd, 0, SEEK_END);
    printf("%s %d\r\n", __func__, file_len);

    json_data = aos_calloc(1, sizeof(char) * (file_len + 1));
    if (NULL == json_data) {
        aos_close(json_fd);
        json_fd = -1;
        return -1;
    }
    aos_lseek(json_fd, 0, SEEK_SET);
    aos_read(json_fd, json_data, file_len);
    aos_close(json_fd);

    /* parser the package json data */
    root = cJSON_Parse(json_data);
    if (NULL == root) {
        aos_free(json_data);
        amp_error(MOD_STR, "cJSON_Parse failed");
        return -1;
    }

    item = cJSON_GetObjectItem(root, "pages");
    if (item == NULL)
    {
        return -1;
    }

    size = cJSON_GetArraySize(item);

    app->page = aos_malloc(sizeof(page_desc_t) * size);
    if (NULL == app->page)
    {
        return -1;
    }

    memset(app->page, 0, sizeof(page_desc_t) * size);
    app->num = size;
    app->cur_page = 0;


    for (i = 0; i < size; i++) {
        page = &app->page[i];
        page->index = i;
        temp = cJSON_GetArrayItem(item, i);
        if(NULL == temp )
        {
            continue;
        }

        //strncpy(page->path, temp->valuestring, 64);
        //printf("temp->valuestring == %s\n\r",temp->valuestring);
        snprintf(page->path, 64, "%s", temp->valuestring);
        snprintf(page->css_file, 128, "%s%s", temp->valuestring, ".css");
        snprintf(page->xml_file, 128, "%s%s", temp->valuestring, ".xml");
        snprintf(page->js_file, 128, "%s%s", temp->valuestring, ".js");

    }

    aos_free(json_data);
    cJSON_Delete(root);

    return 0;
}

void page_config_parse()
{
    memset(&g_app, 0, sizeof(g_app));

    search_js_page_entry(&g_app);
}



page_options_t* page_get_cur_options(void)
{
    int index;

    if (g_app.cur_page >=  g_app.num) {
        return NULL;
    }

    index = g_app.cur_page;

    return &(g_app.page[index].options);
}

static int page_add_options(page_options_t *options)
{
    int index;

    if (options ==  NULL) {
        return -1;
    }
    if (g_app.cur_page >=  g_app.num) {
        return -1;
    }

    index = g_app.cur_page;

    g_app.page[index].options.object    = options->object;
    g_app.page[index].options.data      = options->data;
    g_app.page[index].options.on_show   = options->on_show;
    g_app.page[index].options.on_update = options->on_update;
    g_app.page[index].options.on_exit   = options->on_exit;

    return 0;
}

/* App(Object options) entry */
static JSValue native_page_entry(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    int i;
    int ret;
    page_options_t page_options;
    JSValue irq_cb = argv[0];

    /* check paramters */
    if (!JS_IsObject(irq_cb))
    {
        return JS_ThrowTypeError(ctx, "not a object");
    }

    memset(&page_options, 0, sizeof(page_options_t));

    /* get options object */
    //duk_dup(ctx, -1);
    page_options.object = JS_DupValue(ctx, irq_cb);

    /* find data */
    if (JS_GetPropertyStr(ctx, argv[0], "data"))
    {
        amp_debug(MOD_STR, "find Page#data()");
        page_options.data = JS_DupValue(ctx, irq_cb);
    }

    /* find onShow() */
    //if (duk_get_prop_string(ctx, 0, "onShow"))
    if (JS_GetPropertyStr(ctx, argv[0], "onShow"))
    {
        amp_debug(MOD_STR, "find Page#onShow()");
        page_options.on_show = JS_DupValue(ctx, irq_cb);
    }

    /* find onUpdate() */
    if (JS_GetPropertyStr(ctx, argv[0], "onUpdate"))
    {
        amp_debug(MOD_STR, "find Page#onUpdate()");
        page_options.on_update = JS_DupValue(ctx, irq_cb);
    }

    /* find onExit() */
    if (JS_GetPropertyStr(ctx, argv[0], "onExit"))
    {
        amp_debug(MOD_STR, "find Page#onExit()");
        page_options.on_exit = JS_DupValue(ctx, irq_cb);
    }

    /* one-by-one insert into page list */
    ret = page_add_options(&page_options);

    // amp_task_schedule_call(page_entry, NULL);
    return 1;
}

void page_entry(void *para)
{
    page_options_t *options;
    JSContext *ctx = js_get_context();

    options = page_get_cur_options();
    if (options == NULL) {
        return;
    }
    /* onShow hook */
    uint32_t value = 0;
    JSValue val = JS_Call(ctx, options->on_show, JS_UNDEFINED, 1, &value);
    JS_FreeValue(ctx, val);

}


void page_exit(void *para)
{
    page_options_t *options;
    JSContext *ctx = js_get_context();

    options = page_get_cur_options();
    if (options == NULL) {
        amp_debug("page", "%s : %d---------------------------------", __func__, __LINE__);
        return;
    }

    /* onShow hook */
    uint32_t value = 0;
    JSValue val = JS_Call(ctx, options->on_exit, JS_UNDEFINED, 1, &value);
    JS_FreeValue(ctx, val);

    JS_FreeValue(ctx, options->object);
}


void page_update(void *para)
{
    page_options_t *options;
    JSContext *ctx = js_get_context();

    options = page_get_cur_options();
    if (options == NULL) {
        return;
    }

    /* onShow hook */
    uint32_t value = 0;
    JSValue val = JS_Call(ctx, options->on_update, JS_UNDEFINED, 1, &value);
    JS_FreeValue(ctx, val);
}



static JSClassDef js_page_entry_class = {
    "APPENTRY",
};

static int js_page_entry_init(JSContext *ctx)
{
    JSValue proto;

    JS_NewClassID(&js_page_entry_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_page_entry_class_id, &js_page_entry_class);
    proto = JS_NewObject(ctx);
    JS_SetClassProto(ctx, js_page_entry_class_id, proto);

    return;
}

void page_entry_register(void)
{
    amp_debug(MOD_STR, "module_page_entry_register");
    JSContext *ctx = js_get_context();
    aos_printf("module page_entry register\n");

    js_page_entry_init(ctx);

    QUICKJS_GLOBAL_FUNC("Page", native_page_entry);
}

#endif
