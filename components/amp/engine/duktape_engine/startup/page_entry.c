/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos_fs.h"
#include "aos_system.h"
#include "amp_config.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "board_mgr.h"
#include "be_inl.h"
#include "cJSON.h"
#include "startup/page_entry.h"
#include "render.h"


#define MOD_STR "APPENTRY"

//static dlist_t g_pages_list;


#ifdef JSE_ADVANCED_ADDON_UI
extern char g_app_path[128];

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

#endif

void set_active_page(void)
{


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
static duk_ret_t native_page_entry(duk_context *ctx)
{
    int i;
    int ret;
    page_options_t page_options;

    /* check paramters */
    if (!duk_is_object(ctx, 0))
    {
        // amp_warn("parameter must be object");
        duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "parameter must be object");
        return duk_throw(ctx);
    }

    memset(&page_options, 0, sizeof(page_options_t));

    /* get options object */
    duk_dup(ctx, -1);
    page_options.object = be_ref(ctx);

    /* find data */
    if (duk_get_prop_string(ctx, 0, "data"))
    {
        if (!duk_is_object(ctx, -1))
        {
            duk_pop(ctx);
            duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "data must be function");
            return duk_throw(ctx);
        }
        amp_debug(MOD_STR, "find Page#data");
        duk_dup(ctx, -1);
        page_options.data = be_ref(ctx);
        duk_pop(ctx);
    }

    /* find onShow() */
    if (duk_get_prop_string(ctx, 0, "onShow"))
    {
        if (!duk_is_function(ctx, -1))
        {
            duk_pop(ctx);
            duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "onShow must be function");
            return duk_throw(ctx);
        }
        amp_debug(MOD_STR, "find Page#onShow()");
        duk_dup(ctx, -1);
        page_options.on_show = be_ref(ctx);
        duk_pop(ctx);
    }

    /* find onUpdate() */
    if (duk_get_prop_string(ctx, 0, "onUpdate"))
    {
        if (!duk_is_function(ctx, -1))
        {
            duk_pop(ctx);
            duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "onUpdate must be function");
            return duk_throw(ctx);
        }
        amp_debug(MOD_STR, "find Page#onUpdate()");
        duk_dup(ctx, -1);
        page_options.on_update = be_ref(ctx);
        duk_pop(ctx);
    }

    /* find onExit() */
    if (duk_get_prop_string(ctx, 0, "onExit"))
    {
        if (!duk_is_function(ctx, -1))
        {
            duk_pop(ctx);
            duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "onExit must be function");
            return duk_throw(ctx);
        }
        amp_debug(MOD_STR, "find Page#onExit()");
        duk_dup(ctx, -1);
        page_options.on_exit = be_ref(ctx);
        duk_pop(ctx);
    }

    /* one-by-one insert into page list */
    ret = page_add_options(&page_options);

    // amp_task_schedule_call(page_entry, NULL);
    return 1;
}

void page_entry(void *para)
{
    int i = 0;
    page_options_t *options;
    duk_context *ctx = be_get_context();

    options = page_get_cur_options();
    if (options == NULL) {
        return;
    }

    /* onShow hook */
    be_push_ref(ctx, options->on_show);

    duk_push_object(ctx);

    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_debug("page", "%s : %d---------------------------------", __func__, __LINE__);
        //amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }

    duk_pop(ctx);
    duk_gc(ctx, 0);
}


void page_exit(void *para)
{
    int i = 0;
    page_options_t *options;
    duk_context *ctx = be_get_context();


    options = page_get_cur_options();
    if (options == NULL) {
        amp_debug("page", "%s : %d---------------------------------", __func__, __LINE__);
        return;
    }

    /* onShow hook */
    be_push_ref(ctx, options->on_exit);

    duk_push_object(ctx);

    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_debug("page", "%s : %d---------------------------------", __func__, __LINE__);
        //amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    duk_gc(ctx, 0);

    be_unref(ctx, options->object);

}


void page_update(void *para)
{
    int i = 0;
    page_options_t *options;
    duk_context *ctx = be_get_context();

    options = page_get_cur_options();
    if (options == NULL) {
        return;
    }

    /* onShow hook */
    be_push_ref(ctx, options->on_update);

    duk_push_object(ctx);

    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }

    duk_pop(ctx);
    duk_gc(ctx, 0);
    /* onExit hook */
    // be_push_ref(ctx, app_options.on_exit);

    // duk_push_object(ctx);

    // duk_pcall(ctx, 1);
    // duk_pop(ctx);
}

void page_entry_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    /* Page({}) */
    duk_push_c_function(ctx, native_page_entry, 1);
    duk_put_global_string(ctx, "Page");
}

