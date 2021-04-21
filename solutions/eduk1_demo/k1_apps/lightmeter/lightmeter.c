#include "lightmeter.h"
#include "../../drivers/sensor/drv_als_ps_ir_liteon_ap3216c.h"
#include "aos/kernel.h"
#include <stdio.h>
#include <stdlib.h>

MENU_COVER_TYP lightmeter_cover      = {MENU_COVER_NONE};
MENU_TASK_TYP  lightmeter_tasks      = {lightmeter_init, lightmeter_uninit};
MENU_LIST_TYP  lightmeter_child_list = {NULL, 0};
MENU_TYP lightmeter = {"lightmeter", &lightmeter_cover, &lightmeter_tasks, NULL,
                       &lightmeter_child_list};

static aos_task_t lightmeter_task_handle;

static int running = 1;

int lightmeter_init(void)
{
    LOGI(EDU_TAG, "lightmeter_init begin\n");
    ap3216c_init();
    LOGI(EDU_TAG, "lightmeter_init done\n");

    OLED_Clear();
    OLED_Refresh_GRAM();
    aos_task_new_ext(&lightmeter_task_handle, "lightmeter_task", lightmeter_task, NULL, 2048, AOS_DEFAULT_APP_PRI);
    LOGI(EDU_TAG, "aos_task_new lightmeter_task\n");
    return 0;
}

void lightmeter_task(void)
{
    uint16_t tmp[3];
    uint8_t  als[20];
    uint8_t  ps[20];
    uint8_t  ir[20];

    while (running) {
        tmp[0] = ap3216c_read_ambient_light();
        tmp[1] = ap3216c_read_ir_data();
        tmp[2] = ap3216c_read_ps_data();

        sprintf(als, "ALS: %d", tmp[0]);
        sprintf(ir, "IR : %d", tmp[1]);

        OLED_Clear();
        OLED_Icon_Draw(20, 14, &icon_lighter_32_32, 0);
        OLED_Show_String(64, 6, als, 12, 1);
        OLED_Show_String(64, 20, ir, 12, 1);

        if ((tmp[2] >> 15) & 1)
            OLED_Show_String(64, 36, "near !", 16, 1);
        else
            OLED_Show_String(64, 40, "far !", 16, 1);

        OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
        OLED_Icon_Draw(122, 24, &icon_skip_right, 0);
        OLED_Refresh_GRAM();
        /* wait for 112.5ms at least according to ap3216c's datasheet */
        aos_msleep(150);
    }
    running = 1;
}

int lightmeter_uninit(void)
{
    running = 0;

    while (!running) {
        aos_msleep(50);
    }
    ap3216c_deinit();
    aos_task_delete(&lightmeter_task_handle);
    LOGI(EDU_TAG, "aos_task_delete lightmeter_task\n");
    return 0;
}
