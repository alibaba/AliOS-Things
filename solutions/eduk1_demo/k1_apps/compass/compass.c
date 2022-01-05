#include "compass.h"
#include "drv_mag_qst_qmc5883l.h"
#include "drv_mag_qst_qmc6310.h"
#include <stdio.h>
#include <stdlib.h>

#define COMPASS_CENTER_X 38
#define COMPASS_CENTER_Y 32

// 严格限制长度
static char    code_str[3]   = " "; // ES
static char    number_str[4] = " "; // 329
static int     heading       = 0;
static uint8_t arror_len     = 16;

static int running = 1;

MENU_COVER_TYP compass_cover = {MENU_COVER_NONE};
MENU_TASK_TYP  compass_tasks = {compass_init, compass_uninit};
MENU_TYP compass = {"compass", &compass_cover, &compass_tasks, NULL, NULL};

static aos_task_t compass_task_handle;

int compass_init(void)
{
    if (g_haasboard_is_k1c) {
        qmc6310_init();
        LOGI(EDU_TAG, "qmc6310_init done\n");
    } else {
        qmc5883l_init();
        LOGI(EDU_TAG, "qmc5883l_init done\n");
    }

    OLED_Clear();
    OLED_Refresh_GRAM();
    aos_task_new_ext(&compass_task_handle, "compass_task", compass_task, NULL, 1024 * 4, AOS_DEFAULT_APP_PRI);
    LOGI(EDU_TAG, "aos_task_new compass_task\n");
    return 0;
}

void compass_task()
{
    while (running) {
        if (g_haasboard_is_k1c) {
            heading = qmc6310_readHeading();
            LOGD(EDU_TAG, "heading %d\n", heading);
        } else {
            heading = qmc5883l_readHeading();
            LOGD(EDU_TAG, "heading %d\n", heading);
        }
        OLED_Clear();
        OLED_Icon_Draw(COMPASS_CENTER_X - 27, COMPASS_CENTER_Y - 27,
                       &icon_compass_55_55, 0);
        OLED_Icon_Draw(72, 4, &icon_compass_arror_24_24, 0);
        format_compass_str(number_str, code_str, (-heading), &arror_len);
        OLED_DrawLine_ByAngle(COMPASS_CENTER_X, COMPASS_CENTER_Y,
                              (-heading - 90), arror_len, 1);
        OLED_Show_String(96, 4, code_str, 24, 1);
        OLED_Show_String(78, 36, number_str, 24, 1);
        OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
        OLED_Icon_Draw(122, 24, &icon_skip_right, 0);
        OLED_Refresh_GRAM();

        aos_msleep(30);
    }

    running = 1;
}

int compass_uninit(void)
{
    running = 0;

    while (!running) {
        aos_msleep(50);
    }
    if (g_haasboard_is_k1c) {
        qmc6310_deinit();
    } else {
        qmc5883l_deinit();
    }

    aos_task_delete(&compass_task_handle);
    LOGI(EDU_TAG, "aos_task_delete compass_task\n");
    return 0;
}

void format_compass_str(char *number_str,
                        char *code_str,
                        int heading,
                        uint8_t *arror_len)
{
    heading = (heading % 360 < 0) ? (heading % 360 + 360) : (heading % 360);
    sprintf(number_str, "%3d", heading);
    if ((heading > 0 && heading <= 23) || (heading > 337 && heading <= 360)) {
        sprintf(code_str, " N", heading);
        *arror_len = 16;
    } else if (heading > 292 && heading <= 337) {
        sprintf(code_str, "WN", heading);
        *arror_len = 20;
    } else if (heading > 246 && heading <= 292) {
        sprintf(code_str, " W", heading);
        *arror_len = 16;
    } else if (heading > 202 && heading <= 246) {
        sprintf(code_str, "WS", heading);
        *arror_len = 20;
    } else if (heading > 158 && heading <= 202) {
        sprintf(code_str, " S", heading);
        *arror_len = 16;
    } else if (heading > 112 && heading <= 158) {
        sprintf(code_str, "ES", heading);
        *arror_len = 20;
    } else if (heading > 67 && heading <= 112) {
        sprintf(code_str, " E", heading);
        *arror_len = 16;
    } else if (heading > 23 && heading <= 67) {
        sprintf(code_str, "EN", heading);
        *arror_len = 20;
    }
}
