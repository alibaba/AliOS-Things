#include "shakeshake.h"
#include "../../drivers/sensor/drv_acc_gyro_inv_mpu6050.h"
#include <stdio.h>
#include <stdlib.h>

#define SHAKE_Z_THRESHOLD 5000
#define SHAKE_Y_THRESHOLD 4000

MENU_COVER_TYP shakeshake_cover = {MENU_COVER_NONE};
MENU_TASK_TYP  shakeshake_tasks = {shakeshake_init, shakeshake_uninit};
MENU_TYP shakeshake = {"shakeshake", &shakeshake_cover, &shakeshake_tasks, NULL,
                       NULL};

static aos_task_t shakeshake_task_handle;

char       showstr[10] = "0";
short      ax          = 0, ay, az;
short      ay_pre = 0, az_pre = 0;
short      y_change = 0, z_change = 0;
short      rand_value = 0;
static int running    = 1;

int shakeshake_init(void)
{
    MPU_Init();

    OLED_Clear();
    OLED_Icon_Draw(50, 0, &icon_shakeshake_32_32_v2, 1);
    OLED_Show_String(32, 40, "Shake me !", 16, 1);
    OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
    OLED_Icon_Draw(122, 24, &icon_skip_right, 0);
    OLED_Refresh_GRAM();

    aos_task_new_ext(&shakeshake_task_handle, "shakeshake_task", shakeshake_task, NULL, 1024, AOS_DEFAULT_APP_PRI);
    LOGI(EDU_TAG, "aos_task_new shakeshake_task\n");
    return 0;
}

void shakeshake_task()
{
    while (running) {
        MPU_Get_Accelerometer(&ax, &ay, &az);

        y_change = (ay >= ay_pre) ? (ay - ay_pre) : (ay_pre - ay);
        z_change = (az >= az_pre) ? (az - az_pre) : (az_pre - az);

        if ((y_change > SHAKE_Y_THRESHOLD) && (z_change > SHAKE_Z_THRESHOLD)) {
            rand_value = rand() % 100;
            itoa(rand_value, showstr, 10);

            OLED_Clear();
            OLED_Icon_Draw(50, 0, &icon_shakeshake_32_32_v2, 1);
            OLED_Show_String(54, 36, showstr, 24, 1);
            OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
            OLED_Icon_Draw(122, 24, &icon_skip_right, 0);
            OLED_Refresh_GRAM();
        }

        az_pre = az;
        ay_pre = ay;
        aos_msleep(50);
    }

    running = 1;
}

int shakeshake_uninit(void)
{
    running = 0;

    while (!running) {
        aos_msleep(50);
    }

    MPU_Deinit();
    aos_task_delete(&shakeshake_task_handle);
    LOGI(EDU_TAG, "aos_task_delete shakeshake_task\n");
    return 0;
}
