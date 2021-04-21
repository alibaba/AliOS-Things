#include "humiture.h"
#include "../../drivers/sensor/drv_temp_humi_si_si7006.h"
#include "../menu.h"

MENU_COVER_TYP humiture_cover = {MENU_COVER_NONE};
MENU_TASK_TYP  humiture_tasks = {humiture_init, humiture_uninit};
MENU_TYP humiture = {"humiture", &humiture_cover, &humiture_tasks, NULL, NULL};

static int running = 1;

static aos_task_t humiture_task_handle;

int humiture_init(void)
{
    LOGI(EDU_TAG, "si7006_init begin\n");
    si7006_init();
    LOGI(EDU_TAG, "si7006_init done\n");
    OLED_Clear();
    OLED_Refresh_GRAM();
    running = 1;
    aos_task_new_ext(&humiture_task_handle, "humiture_task", humiture_task, NULL, 1024, AOS_DEFAULT_APP_PRI);
    LOGI(EDU_TAG, "aos_task_delete humiture_task\n");
    return 0;
}

void humiture_task(void)
{
    float         temp, hump;
    uint8_t       temp_str[10];
    uint8_t       hump_str[10];
    unsigned char c = 0;

    while (running) {
        si7006_getTempHumidity(&hump, &temp);
        sprintf(temp_str, "T:%5.1fC", temp);
        sprintf(hump_str, "H:%5.1f%%", hump);
        LOGD(EDU_TAG, "%s %s", temp_str, hump_str);
        OLED_Icon_Draw(14, 4, &icon_thermometer_24_24, 0);
        OLED_Icon_Draw(14, 36, &icon_hygrometer_24_24, 0);

        OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
        OLED_Icon_Draw(122, 24, &icon_skip_right, 0);

        OLED_Show_String(42, 8, temp_str, 16, 1);
        OLED_Show_String(42, 40, hump_str, 16, 1);

        OLED_Refresh_GRAM();
        aos_msleep(500);
    }
    running = 1;
}

int humiture_uninit(void)
{
    running = 0;

    while (!running) {
        aos_msleep(50);
    }

    si7006_deinit();

    aos_task_delete(&humiture_task_handle);
    LOGI(EDU_TAG, "aos_task_delete humiture_task\n");
    return 0;
}
