/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "be_module.h"
#include "jse_common.h"

#include <k_api.h>
#include "lvgl.h"

/* display driver */
lv_disp_drv_t disp_drv;

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

void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                   lv_color_t *color_p)
{
    int32_t x = 0;
    int32_t y = 0;

    for (y = area->y1; y <= area->y2; y++) {
        hal_lcd_draw_line(area->x1, y, (uint8_t *)color_p,
                           (area->x2 - area->x1 + 1));
        color_p += (area->x2 - area->x1 + 1);
    }

    lv_disp_flush_ready(&disp_drv);
}

/* display driver */
void lvgl_disp_drv_init(void)
{
    static lv_disp_buf_t disp_buf_2;
    /* A buffer for 10 rows */
    static lv_color_t buf2_1[LV_HOR_RES_MAX * 10];
    /* An other buffer for 10 rows */
    static lv_color_t buf2_2[LV_HOR_RES_MAX * 10];
    /* Initialize the display buffer */
    lv_disp_buf_init(&disp_buf_2, buf2_1, buf2_2, LV_HOR_RES_MAX * 10);

    /*-----------------------------------
     * Register the display in LittlevGL
     *----------------------------------*/

    lv_disp_drv_init(&disp_drv); /* Basic initialization */

    /* Set up the functions to access to your display */

    /* Set the resolution of the display */
    disp_drv.hor_res = 240;
    disp_drv.ver_res = 240;

    /* Used to copy the buffer's content to the display */
    disp_drv.flush_cb = my_disp_flush;

    /* Set a display buffer */
    disp_drv.buffer = &disp_buf_2;

    /* Finally register the driver */
    lv_disp_drv_register(&disp_drv);
}

static be_jse_symbol_t *gui_init(void)
{
    be_jse_handle_function(0, NULL, NULL, NULL, NULL);
    /* init littlevGL */
    lv_init();

    /* init LCD */
    hal_lcd_init(NULL);

    /* register driver for littlevGL */
    lvgl_disp_drv_init();

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
