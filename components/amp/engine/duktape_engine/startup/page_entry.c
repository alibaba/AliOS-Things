/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_fs.h"
#include "amp_system.h"
#include "amp_config.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "board_mgr.h"
#include "be_inl.h"
#include "cJSON.h"
#include "startup/page_entry.h"

#define MOD_STR "APPENTRY"

static dlist_t g_pages_list;
static page_entry_t *active_page_node;

void page_list_init(void)
{
    dlist_init(&g_pages_list);
}

/* insert page route into list */
void page_list_add(const char *route)
{
    page_entry_t *page_entry = amp_malloc(sizeof(page_entry_t));
    memset(page_entry, 0, sizeof(page_entry_t));
    page_entry->page = strdup(route); /* don't forget to free */
    dlist_add_tail(&page_entry->node, &g_pages_list);
}

/* insert page options into list one-by-one */
static int page_list_add_options(page_options_t *options)
{
    dlist_t *temp;
    page_entry_t *page_node;
    dlist_for_each_entry_safe(&g_pages_list, temp, page_node, page_entry_t, node)
    {
        amp_debug(MOD_STR, "page route: %s, options: %p", page_node->page, page_node->options);
        if (page_node->options)
        {
            continue;
        }

        page_node->options = options;
        return 0;
    }

    amp_error(MOD_STR, "pages options already have");
    return -1;
}

/* display all page list */
void page_list_dump(void)
{
    dlist_t *temp;
    page_entry_t *page_node;

    dlist_for_each_entry_safe(&g_pages_list, temp, page_node, page_entry_t, node)
    {
        amp_debug(MOD_STR, "page route: %s", page_node->page);
    }
}

/* free all list */
void page_list_free(void)
{
    dlist_t *temp;
    page_entry_t *page_node;

    dlist_for_each_entry_safe(&g_pages_list, temp, page_node, page_entry_t, node)
    {
        dlist_del(&page_node->node);
        amp_free(page_node->page);
        amp_free(page_node);
    }
}

void set_active_page(void)
{
}

/* App(Object options) entry */
static duk_ret_t native_page_entry(duk_context *ctx)
{
    int i;
    page_options_t *page_options = NULL;

    /* check paramters */
    if (!duk_is_object(ctx, 0))
    {
        // amp_warn("parameter must be object");
        duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "parameter must be object");
        return duk_throw(ctx);
    }

    if ((page_options = (page_options_t *)amp_malloc(sizeof(page_options_t))) == NULL)
    {
        amp_error(MOD_STR, "memory overflow");
        return 1;
    }

    memset(page_options, 0, sizeof(page_options_t));

    /* get options object */
    duk_dup(ctx, -1);
    page_options->object = be_ref(ctx);

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
        page_options->data = be_ref(ctx);
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
        page_options->on_show = be_ref(ctx);
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
        page_options->on_update = be_ref(ctx);
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
        page_options->on_exit = be_ref(ctx);
        duk_pop(ctx);
    }

    /* one-by-one insert into page list */
    page_list_add_options(page_options);

    // amp_task_schedule_call(page_entry, NULL);
    return 1;
}

void page_entry(page_options_t *options)
{
    int i = 0;

    duk_context *ctx = be_get_context();
    /* onShow hook */
    be_push_ref(ctx, options->on_show);

    duk_push_object(ctx);

    if (duk_pcall_prop(ctx, options->on_show, 1) != DUK_EXEC_SUCCESS) {
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