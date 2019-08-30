#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "be_module.h"
#include "jse_common.h"

#include <k_api.h>
#include "lvgl.h"

extern lv_style_t *_symbol_style_parser(be_jse_symbol_t *arg);

static be_jse_symbol_t *gui_bar(void)
{
    be_jse_symbol_t *arg0 = NULL;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        symbol_unlock(arg0);
        return new_int_symbol(-1);
    }

    lv_obj_t *scr_act = (lv_obj_t *)get_symbol_value_int(arg0);
    symbol_unlock(arg0);
    return new_int_symbol((int)lv_bar_create(scr_act, NULL));
}

static be_jse_symbol_t *gui_bar_set_value(void)
{
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;

    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0) || !arg1 || !symbol_is_int(arg1)) {
        symbol_unlock(arg0);
        symbol_unlock(arg1);
        return new_int_symbol(-1);
    }

    lv_obj_t *obj = (lv_obj_t *)get_symbol_value_int(arg0);
    int value     = get_symbol_value_int(arg1);
    // jse_debug("obj:%p, value:%d\r\n", obj, value);
    lv_bar_set_value(obj, value, LV_ANIM_OFF);

    symbol_unlock(arg0);
    symbol_unlock(arg1);
    return new_int_symbol(1);
}

static be_jse_symbol_t *gui_bar_set_style(void)
{
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    be_jse_symbol_t *arg2 = NULL;
    lv_style_t *style     = NULL;

    be_jse_handle_function(0, &arg0, &arg1, &arg2, NULL);
    if (!arg0 || !symbol_is_int(arg0) || !arg1 || !symbol_is_int(arg1) ||
        !arg2 || !symbol_is_object(arg2)) {
        symbol_unlock(arg0);
        symbol_unlock(arg1);
        symbol_unlock(arg2);
        return new_int_symbol(-1);
    }

    lv_obj_t *obj = (lv_obj_t *)get_symbol_value_int(arg0);
    int type      = get_symbol_value_int(arg1);

    if ((style = _symbol_style_parser(arg2)) == NULL) {
        symbol_unlock(arg0);
        symbol_unlock(arg1);
        symbol_unlock(arg2);
        return new_int_symbol(-1);
    }

    lv_bar_set_style(obj, type, style);

    symbol_unlock(arg0);
    symbol_unlock(arg1);
    symbol_unlock(arg2);
    return new_int_symbol(1);
}

be_jse_symbol_t *gui_bar_handle_cb(be_jse_vm_ctx_t *execInfo,
                                   be_jse_symbol_t *var, const char *name)
{
    if (0 == strcmp(name, "bar")) {
        return gui_bar();
    }

    if (0 == strcmp(name, "barSetValue")) {
        return gui_bar_set_value();
    }

    if (0 == strcmp(name, "barSetStyle")) {
        return gui_bar_set_style();
    }

    if (0 == strcmp(name, "BAR_STYLE_BG")) {
        return new_int_symbol(LV_BAR_STYLE_BG);
    }

    if (0 == strcmp(name, "BAR_STYLE_INDIC")) {
        return new_int_symbol(LV_BAR_STYLE_INDIC);
    }

    return NULL;
}