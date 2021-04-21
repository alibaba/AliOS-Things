#include "gyroscope.h"
#include "../../drivers/sensor/drv_acc_gyro_inv_mpu6050.h"
#include <stdio.h>
#include <stdlib.h>

MENU_COVER_TYP gyroscope_cover = {MENU_COVER_NONE};
MENU_TASK_TYP  gyroscope_tasks = {gyroscope_init, gyroscope_uninit};
MENU_TYP gyroscope = {"gyroscope", &gyroscope_cover, &gyroscope_tasks, NULL,
                      NULL};

static aos_task_t gyroscope_task_handle;

static short r_ax = 0, r_ay = 0, r_az = 0;
static int   running = 1;

typedef struct {
    int x;
    int y;
    int z;
} node_t;

typedef struct {
    node_t *start_node;
    node_t *end_node;
} line_t;

node_t node_list[] = {{0, 0, 0},      {20, 20, 20},   {-20, 20, 20},
                      {20, -20, 20},  {20, 20, -20},  {-20, -20, 20},
                      {-20, 20, -20}, {20, -20, -20}, {-20, -20, -20}
};

line_t line_list[] = {
    {&node_list[1], &node_list[2]}, {&node_list[1], &node_list[3]},
    {&node_list[3], &node_list[2]}, {&node_list[3], &node_list[4]},
    {&node_list[1], &node_list[2]}, {&node_list[1], &node_list[2]},
    {&node_list[1], &node_list[2]}, {&node_list[1], &node_list[2]},
    {&node_list[1], &node_list[2]}, {&node_list[1], &node_list[2]},
    {&node_list[1], &node_list[2]}, {&node_list[1], &node_list[2]},
};

int gyroscope_init(void)
{
    LOGI(EDU_TAG, "MPU_Init start\n");
    MPU_Init();
    LOGI(EDU_TAG, "MPU_Init done\n");
    aos_task_new_ext(&gyroscope_task_handle, "gyroscope_task", gyroscope_task, NULL, 1024, AOS_DEFAULT_APP_PRI);
    LOGI(EDU_TAG, "aos_task_new gyroscope_task\n");
    return 0;
}

void gyroscope_task(void)
{
    while (running) {
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

    while (!running) {
        aos_msleep(50);
    }
    LOGI(EDU_TAG, "MPU_Deinit start\n");
    MPU_Deinit();
    LOGI(EDU_TAG, "MPU_Deinit done\n");
    aos_task_delete(&gyroscope_task_handle);
    LOGI(EDU_TAG, "aos_task_delete gyroscope_task\n");
    return 0;
}
