#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "be_module.h"
#include "jse_common.h"

#include <k_api.h>
#include "lvgl/lvgl.h"

extern be_jse_symbol_t *gui_set_style(void);

static be_jse_symbol_t *gui_led(void)
{
    be_jse_symbol_t *arg0 = NULL;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        symbol_unlock(arg0);
        return new_int_symbol(-1);
    }

    lv_obj_t *scr_act = (lv_obj_t *)get_symbol_value_int(arg0);
    symbol_unlock(arg0);
    return new_int_symbol((int)lv_led_create(scr_act, NULL));
}

static be_jse_symbol_t *gui_led_set(void)
{
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;

    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0) || !arg1 || !symbol_is_int(arg1)) {
        symbol_unlock(arg0);
        symbol_unlock(arg1);
        return new_int_symbol(-1);
    }

    lv_obj_t *led = (lv_obj_t *)get_symbol_value_int(arg0);
    int status    = get_symbol_value_int(arg1);
    status == 0 ? lv_led_off(led) : lv_led_on(led);

    symbol_unlock(arg0);
    symbol_unlock(arg1);
    return new_int_symbol(1);
}

be_jse_symbol_t *gui_led_handle_cb(be_jse_vm_ctx_t *execInfo,
                                   be_jse_symbol_t *var, const char *name)
{
    if (0 == strcmp(name, "led")) {
        return gui_led();
    }

    if (0 == strcmp(name, "ledSetStyle")) {
        return gui_set_style();
    }

    if (0 == strcmp(name, "ledSet")) {
        return gui_led_set();
    }

    return NULL;
}