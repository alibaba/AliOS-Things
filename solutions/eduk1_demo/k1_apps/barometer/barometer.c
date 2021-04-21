#include "barometer.h"
#include "../../drivers/sensor/drv_baro_goertek_spl06.h"
#include <stdio.h>
#include <stdlib.h>

static char         pressure_str[16] = " ";
static char         altitude_str[16] = " ";
static char         Ctemp_str[7]     = " ";
static char         Ftemp_str[7]     = " ";
static spl06_data_t spl06_data       = {0};

static int running = 1;

MENU_COVER_TYP barometer_cover = {MENU_COVER_NONE};
MENU_TASK_TYP  barometer_tasks = {barometer_init, barometer_uninit};
MENU_TYP barometer = {"barometer", &barometer_cover, &barometer_tasks, NULL,
                      NULL};

static aos_task_t barometer_task_handle;

int barometer_init(void)
{
    LOGI(EDU_TAG, "spl06_init begin\n");
    spl06_init();
    LOGI(EDU_TAG, "spl06_init done\n");

    spl06_getdata(&spl06_data);

    aos_task_new_ext(&barometer_task_handle, "barometer_task", barometer_task, NULL, 1024, AOS_DEFAULT_APP_PRI);
    LOGI(EDU_TAG, "aos_task_new barometer_task\n");
    return 0;
}

void barometer_task()
{
    while (running) {
        OLED_Clear();

        OLED_Icon_Draw(14, 4, &icon_atmp_16_16, 0);
        sprintf(pressure_str, " %-10.3lfkPa", spl06_data.pressure / 10);
        LOGD(EDU_TAG, "%s\n", pressure_str);
        OLED_Show_String(32, 6, pressure_str, 12, 1);

        OLED_Icon_Draw(14, 23, &icon_asl_16_16, 0);
        sprintf(altitude_str, " %-12.2lfm", spl06_data.altitude);
        LOGD(EDU_TAG, "%s\n", altitude_str);
        OLED_Show_String(32, 25, altitude_str, 12, 1);

        OLED_Icon_Draw(14, 44, &icon_tempC_16_16, 0);
        sprintf(Ctemp_str, "%-5.2lf", spl06_data.Ctemp);
        LOGD(EDU_TAG, "%s\n", Ctemp_str);
        OLED_Show_String(30, 46, Ctemp_str, 12, 1);

        OLED_Icon_Draw(66, 44, &icon_tempF_16_16, 0);
        sprintf(Ftemp_str, "%-5.2lf", spl06_data.Ftemp);
        LOGD(EDU_TAG, "%s\n", Ftemp_str);
        OLED_Show_String(82, 46, Ftemp_str, 12, 1);

        OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
        OLED_Icon_Draw(122, 24, &icon_skip_right, 0);

        spl06_getdata(&spl06_data);

        OLED_Refresh_GRAM();

        aos_msleep(500);
    }

    running = 1;
}

int barometer_uninit(void)
{
    running = 0;

    while (!running) {
        aos_msleep(50);
    }

    aos_task_delete(&barometer_task_handle);
    LOGI(EDU_TAG, "aos_task_delete barometer_task\n");

    spl06_deinit();

    return 0;
}
