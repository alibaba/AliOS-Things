/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include "hal_oled.h"
#include "spl06.h"
#include "barometer.h"

static char pressure_str[16] = " ";
static char altitude_str[16] = " ";
static char Ctemp_str[7] = " ";
static char Ftemp_str[7] = " ";
static spl06_data_t spl06_data = {0};

MENU_COVER_TYP barometer_cover = {MENU_COVER_NONE};
MENU_TASK_TYP barometer_tasks = {
    barometer_init,
    barometer_uninit};
MENU_TYP barometer = {
    "barometer",
    &barometer_cover,
    &barometer_tasks,
    NULL,
    NULL};

int barometer_init(void)
{
    printf("spl06_init begin\n");
    spl06_init();
    printf("spl06_init done\n");

    spl06_getdata(&spl06_data);

    aos_task_new("barometer_task", barometer_task, NULL, 1000);
    printf("aos_task_new barometer_task \n");
    return 0;
}

int barometer_uninit(void)
{
    aos_task_delete("barometer_task");
    printf("aos_task_delete barometer_task \n");
    return 0;
}

void barometer_task()
{
    while (1)
    {
        OLED_Clear();

        OLED_Icon_Draw(14, 4, &icon_atmp_16_16, 0);
        sprintf(pressure_str, " %-12.2lfPa", spl06_data.pressure);
        printf("%s\n", pressure_str);
        OLED_Show_String(32, 6, pressure_str, 12, 1);

        OLED_Icon_Draw(14, 23, &icon_asl_16_16, 0);
        sprintf(altitude_str, " %-12.2lfm", spl06_data.altitude);
        printf("%s\n", altitude_str);
        OLED_Show_String(32, 25, altitude_str, 12, 1);

        OLED_Icon_Draw(14, 44, &icon_tempC_16_16, 0);
        sprintf(Ctemp_str, "%-5.2lf", spl06_data.Ctemp);
        printf("%s\n", Ctemp_str);
        OLED_Show_String(30, 46, Ctemp_str, 12, 1);

        OLED_Icon_Draw(66, 44, &icon_tempF_16_16, 0);
        sprintf(Ftemp_str, "%-5.2lf", spl06_data.Ftemp);
        printf("%s\n", Ftemp_str);
        OLED_Show_String(82, 46, Ftemp_str, 12, 1);

        OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
        OLED_Icon_Draw(122, 24, &icon_skip_right, 0);

        spl06_getdata(&spl06_data);

        OLED_Refresh_GRAM();

        aos_msleep(500);
    }
}
