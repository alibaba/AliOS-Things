#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "be_module.h"
#include "jse_common.h"

#include <k_api.h>
#include "lvgl.h"

static be_jse_symbol_t *gui_scr_act(void)
{
    be_jse_handle_function(0, NULL, NULL, NULL, NULL);
    return new_int_symbol((int)lv_scr_act());
}

be_jse_symbol_t *gui_arc_handle_cb(be_jse_vm_ctx_t *execInfo,
                                   be_jse_symbol_t *var, const char *name)
{
    if (0 == strcmp(name, "scrAct")) {
        return gui_scr_act();
    }
    return NULL;
}