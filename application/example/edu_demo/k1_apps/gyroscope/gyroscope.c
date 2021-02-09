/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include "gyroscope.h"
#include "hal_oled.h"
#include "mpu6050.h"
// #define USE_WHILE_1

MENU_COVER_TYP gyroscope_cover = {MENU_COVER_NONE};
MENU_TASK_TYP gyroscope_tasks = {
    gyroscope_init,
    gyroscope_uninit};
MENU_TYP gyroscope = {
    "gyroscope",
    &gyroscope_cover,
    &gyroscope_tasks,
    NULL,
    NULL};

static short r_ax = 0, r_ay = 0, r_az = 0;
#ifdef USE_WHILE_1
static short running = 0;
#endif

int gyroscope_init(void)
{
    MPU_Init();
#ifdef USE_WHILE_1
    running = 1;
    gyroscope_task();
#else
    aos_task_new("gyroscope_task", gyroscope_task, NULL, 1000);
#endif
    printf("aos_task_new gyroscope_task \n");

    return 0;
}

// TODO 直接 while (1) 会导致接收不到按键？ 我上层打印不出来 等会看看驱动
void gyroscope_task(void)
{
    while (1)
    {
        OLED_Clear();
        MPU_Get_Accelerometer(&r_ax, &r_ay, &r_az);
        OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
        OLED_Icon_Draw(122, 24, &icon_skip_right, 0);
        OLED_DrawCircle(66, 32, 10, 1, 1);
        OLED_FillCircle(66 - r_ax / 250, 32 + r_ay / 500, 8, 1);
        OLED_Refresh_GRAM();
#ifdef USE_WHILE_1
        if (running == 0)
            break;
#endif
        aos_msleep(20);
    }
}

int gyroscope_uninit(void)
{
#ifdef USE_WHILE_1
    running = 0;
#else
    aos_task_delete("gyroscope_task");
#endif
    printf("aos_task_delete gyroscope_task \n");
    return 0;
}