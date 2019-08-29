#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "be_module.h"
#include "jse_common.h"

#include <k_api.h>
#include "lvgl/lvgl.h"

static be_jse_symbol_t *gui_label(void)
{
    be_jse_symbol_t *arg0 = NULL;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        symbol_unlock(arg0);
        return new_int_symbol(-1);
    }

    lv_obj_t *scr_act = (lv_obj_t *)get_symbol_value_int(arg0);
    symbol_unlock(arg0);
    return new_int_symbol((int)lv_label_create(scr_act, NULL));
}

static be_jse_symbol_t *gui_label_set_text(void)
{
    int len               = 0;
    char *text            = NULL;
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;

    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0) || !arg1 || !symbol_is_string(arg1)) {
        symbol_unlock(arg0);
        symbol_unlock(arg1);
        return new_int_symbol(-1);
    }

    lv_obj_t *label = (lv_obj_t *)get_symbol_value_int(arg0);

    len  = symbol_str_len(arg1);
    if (NULL == (text = jse_calloc(1, sizeof(char) * (len + 1)))) {
        symbol_unlock(arg0);
        symbol_unlock(arg1);
        return new_int_symbol(-1);
    }

    symbol_to_str(arg1, text, len);
    // jse_debug("label:%p, text:%s\r\n", label, text);
    lv_label_set_text(label, text);

    symbol_unlock(arg0);
    symbol_unlock(arg1);
    return new_int_symbol(1);
}

static be_jse_symbol_t *gui_label_set_long_mode(void)
{
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;

    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0) || !arg1 || !symbol_is_string(arg1)) {
        symbol_unlock(arg0);
        symbol_unlock(arg1);
        return new_int_symbol(-1);
    }

    lv_obj_t *label = (lv_obj_t *)get_symbol_value_int(arg0);
    int mode        = (lv_obj_t *)get_symbol_value_int(arg1);

    // jse_debug("label:%p, mode:%d\r\n", label, mode);
    lv_label_set_long_mode(label, mode);

    symbol_unlock(arg0);
    symbol_unlock(arg1);
    return new_int_symbol(1);
}

be_jse_symbol_t *gui_label_handle_cb(be_jse_vm_ctx_t *execInfo,
                                     be_jse_symbol_t *var, const char *name)
{
    if (0 == strcmp(name, "label")) {
        return gui_label();
    }
    if (0 == strcmp(name, "labelSetText")) {
        return gui_label_set_text();
    }
    if (0 == strcmp(name, "labelSetLongMode")) {
        return gui_label_set_long_mode();
    }
    if (0 == strcmp(name, "LABEL_LONG_SCROLL")) {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        return new_int_symbol(LV_LABEL_LONG_SCROLL);
    }

    return NULL;
}