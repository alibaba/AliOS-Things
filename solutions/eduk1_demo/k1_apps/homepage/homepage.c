#include "homepage.h"
#include "../menu.h"
#include "aos/kernel.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "posix/timer.h"
#include "../../build_version.h"
#include "netmgr.h"
#include "aos/vfs.h"

#include <drivers/u_ld.h>
#include <vfsdev/adc_dev.h>
#include <drivers/char/u_device.h>

static int battery_level = 0;
extern int bt_connected;
extern int ip_got_finished;
extern char eduk1_ip_addr[IPADDR_STR_LEN];

MENU_COVER_TYP homepage_cover = {MENU_COVER_NONE};
MENU_TASK_TYP  homepage_tasks = {homepage_init, homepage_uninit};
MENU_TYP homepage = {"homepage", &homepage_cover, &homepage_tasks, NULL, NULL};

static aos_task_t homepage_task_handle;

int homepage_init(void)
{
    OLED_Clear();
    OLED_Refresh_GRAM();
    aos_task_new_ext(&homepage_task_handle, "homepage_task", homepage_task, NULL, 1024, AOS_DEFAULT_APP_PRI);
    LOGI(EDU_TAG, "aos_task_new homepage_task\n");
    return 0;
}

static int get_battery(int *level, uint32_t *volage)
{
    int32_t  ret       = 0;
    uint32_t test_sum  = 0;
    uint32_t test_avrg = 0;
    uint32_t test_min  = 3300;
    uint32_t test_max  = 0;
    int32_t  fd        = 0;
    int32_t index      = 1;
    int sampling_cycle = 100;
    char name[16] = {0};
    io_adc_arg_t adc_arg;

    snprintf(name, sizeof(name), "/dev/adc%d", index);
    fd = open(name, 0);

    if (fd >= 0) {
        ret = ioctl(fd, IOC_ADC_START, sampling_cycle);
        usleep(1000);
        adc_arg.value = 0;
        adc_arg.timeout = 500000; // in unit of us

        for (int32_t i = 0; i < 10; i++) {
            ret = ioctl(fd, IOC_ADC_GET_VALUE, (unsigned long)&adc_arg);
            test_sum += adc_arg.value;

            /* the min sampling voltage */
            if (test_min >= adc_arg.value) {
                test_min = adc_arg.value;
            }
            /* the max sampling voltage */
            if (test_max <= adc_arg.value) {
                test_max = adc_arg.value;
            }
        }
        usleep(1000);
        ret = ioctl(fd, IOC_ADC_STOP, 0);
        close(fd);

        test_avrg = (test_sum - test_min - test_max) >> 3;
        LOGD(EDU_TAG, "the samping volage is:%dmv\n", test_avrg);
        test_avrg *= 3.208;
        *volage = test_avrg;

        if (test_avrg > 4100) {
            *level = 4;
        } else if ((test_avrg > 3980) && (test_avrg < 4100)) {
            *level = 3;
        } else if ((test_avrg > 3850) && (test_avrg < 3980)) {
            *level = 2;
        } else if ((test_avrg > 3700) && (test_avrg < 3850)) {
            *level = 1;
        } else if (test_avrg < 3700) {
            *level = 0;
        }
    }
    return 0;

}

void homepage_task(void)
{
    unsigned char   c = 0;
    struct tm *info;
    struct timespec tv;
    uint8_t         image_version[22];
    uint8_t         tmp[22];
    netmgr_ifconfig_info_t ifconfig;
    netmgr_hdl_t hdl;
    uint32_t volage;
    int ret = 0;

    while (1) {
        OLED_Clear();
        /* 获取 GMT 时间 */
        clock_gettime(CLOCK_REALTIME, &tv);
        info = gmtime(&tv);

        snprintf(tmp, 20, "%2d:%02d", (info->tm_hour + 8) % 24, info->tm_min);
        OLED_Show_String(2, 12 * 0, tmp, 12, 1);
        if (ip_got_finished)
        {
            OLED_Icon_Draw(86, 0, &icon_wifi_on_12_12, 0);
            snprintf(image_version, 20, "IP: %s", eduk1_ip_addr);
            OLED_Show_String(20, (12 + 4) * 3, image_version, 12, 1);
        }else
        {
            //snprintf(image_version, 20, "IP: ?.?.?.?", ifconfig.ip_addr);
            //OLED_Show_String(20, (12 + 4) * 3, image_version, 12, 1);
        }

        if (bt_connected)
        {
            OLED_Icon_Draw(97, 0, &icon_bt_on_12_12, 0);
        }

        OLED_Show_String(40, (12 + 4) * 1, "HaaS EDU", 12, 1);
        snprintf(image_version, 21, "VER: %s", BUILD_VERSION);
        OLED_Show_String(33, (12 + 4) * 2, image_version, 12, 1);

        OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
        OLED_Icon_Draw(122, 24, &icon_skip_right, 0);

        if (0 == get_battery(&battery_level, &volage)) {
            LOGD(EDU_TAG, "get_battery success:%d mv,level :%d \n", volage, battery_level);
            OLED_Icon_Draw(110, 0, &icon_battery_20_12[battery_level], 0);
        } else {
            LOGE(EDU_TAG, "get_battery fail\n");
        }

        OLED_Refresh_GRAM();
        aos_msleep(1000);
    }
}

int homepage_uninit(void)
{
    aos_task_delete(&homepage_task_handle);
    LOGI(EDU_TAG, "aos_task_delete homepage_task\n");
    return 0;
}
