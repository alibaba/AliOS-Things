#include <stdio.h>
#include <stdlib.h>
#include "gyroscope.h"
#include "hal_oled.h"
#include "mpu6050.h"

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
static int running = 1;

int gyroscope_init(void)
{
    MPU_Init();
    aos_task_new("gyroscope_task", gyroscope_task, NULL, 1000);
    printf("aos_task_new gyroscope_task \n");
    return 0;
}

void gyroscope_task(void)
{
    while (running)
    {
        OLED_Clear();
        MPU_Get_Accelerometer(&r_ax, &r_ay, &r_az);
        OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
        OLED_Icon_Draw(122, 24, &icon_skip_right, 0);
        OLED_DrawCircle(66, 32, 10, 1, 1);
        OLED_FillCircle(66 - r_ax / 250, 32 + r_ay / 500, 8, 1);
        OLED_Refresh_GRAM();
        aos_msleep(20);
    }
    running = 1;
}

int gyroscope_uninit(void)
{
    running = 0;

    while (!running)
    {
        aos_msleep(50);
    }

    aos_task_delete("gyroscope_task");
    printf("aos_task_delete gyroscope_task \n");
    return 0;
}