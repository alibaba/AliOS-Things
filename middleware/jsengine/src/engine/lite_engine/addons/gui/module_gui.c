/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "be_module.h"
#include "jse_common.h"

#include <k_api.h>
#include "lvgl/lvgl.h"

extern be_jse_symbol_t *gui_common_handle_cb(be_jse_vm_ctx_t *execInfo,
                                             be_jse_symbol_t *var,
                                             const char *name);

extern be_jse_symbol_t *gui_arc_handle_cb(be_jse_vm_ctx_t *execInfo,
                                          be_jse_symbol_t *var,
                                          const char *name);

extern be_jse_symbol_t *gui_obj_handle_cb(be_jse_vm_ctx_t *execInfo,
                                          be_jse_symbol_t *var,
                                          const char *name);

extern be_jse_symbol_t *gui_led_handle_cb(be_jse_vm_ctx_t *execInfo,
                                          be_jse_symbol_t *var,
                                          const char *name);

extern be_jse_symbol_t *gui_bar_handle_cb(be_jse_vm_ctx_t *execInfo,
                                          be_jse_symbol_t *var,
                                          const char *name);

extern be_jse_symbol_t *gui_label_handle_cb(be_jse_vm_ctx_t *execInfo,
                                            be_jse_symbol_t *var,
                                            const char *name);

extern be_jse_symbol_t *gui_canvas_handle_cb(be_jse_vm_ctx_t *execInfo,
                                             be_jse_symbol_t *var,
                                             const char *name);

static void littlevgl_refresh_task(void *arg)
{
    while (1) {
        /* this function is used to refresh the LCD */
        lv_task_handler();

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND / 10);
    }
}

void my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                   const lv_color_t *color_p)
{
    int32_t x = 0;
    int32_t y = 0;

    for (y = y1; y <= y2; y++) {
        ST7789H2_WriteLine(x1, y, (uint8_t *)color_p, (x2 - x1 + 1));
        color_p += (x2 - x1 + 1);
    }

    lv_flush_ready();
}

void my_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                  lv_color_t color)
{
    int32_t i = 0;
    int32_t j = 0;

    for (i = x1; i <= x2; i++) {
        for (j = y1; j <= y2; j++) {
            ST7789H2_WritePixel(i, j, color.full);
        }
    }
}

void my_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                 const lv_color_t *color_p)
{
    int32_t x = 0;
    int32_t y = 0;

    for (y = y1; y <= y2; y++) {
        ST7789H2_WriteLine(x1, y, (int16_t *)color_p, (x2 - x1 + 1));
        color_p += (x2 - x1 + 1);
    }
}

/* display driver */
lv_disp_drv_t dis_drv;
void lvgl_drv_register(void)
{
    lv_disp_drv_init(&dis_drv);

    dis_drv.disp_flush = my_disp_flush;
    dis_drv.disp_fill  = my_disp_fill;
    dis_drv.disp_map   = my_disp_map;
    lv_disp_drv_register(&dis_drv);
}

static be_jse_symbol_t *gui_init(void)
{
    be_jse_handle_function(0, NULL, NULL, NULL, NULL);
    /* init littlevGL */
    lv_init();

    /* init LCD */
    st7789_init();
    /* init LCD */
    st7789_init();

    /* register driver for littlevGL */
    lvgl_drv_register();

    /* create a task to refresh the LCD */
    aos_task_new("lv_refresh", littlevgl_refresh_task, NULL, 2048);

    return new_int_symbol(1);
}

static be_jse_symbol_t *gui_module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                             be_jse_symbol_t *var,
                                             const char *name)
{
    be_jse_symbol_t *handle = NULL;

    if (0 == strcmp(name, "init")) {
        return gui_init();
    }

    if ((handle = gui_common_handle_cb(execInfo, var, name)) != NULL) {
        return handle;
    }

    if ((handle = gui_arc_handle_cb(execInfo, var, name)) != NULL) {
        return handle;
    }

    if ((handle = gui_obj_handle_cb(execInfo, var, name)) != NULL) {
        return handle;
    }

#ifdef JSE_CORE_ADDON_GUI_LED
    if ((handle = gui_led_handle_cb(execInfo, var, name)) != NULL) {
        return handle;
    }
#endif
#ifdef JSE_CORE_ADDON_GUI_BAR
    if ((handle = gui_bar_handle_cb(execInfo, var, name)) != NULL) {
        return handle;
    }
#endif
#ifdef JSE_CORE_ADDON_GUI_LABEL
    if ((handle = gui_label_handle_cb(execInfo, var, name)) != NULL) {
        return handle;
    }
#endif
#ifdef JSE_CORE_ADDON_GUI_CANVAS
    if ((handle = gui_canvas_handle_cb(execInfo, var, name)) != NULL) {
        return handle;
    }
#endif
    return BE_JSE_FUNC_UNHANDLED;
}

void module_gui_register(void)
{
    be_jse_module_load("GUI", gui_module_handle_cb);
}
