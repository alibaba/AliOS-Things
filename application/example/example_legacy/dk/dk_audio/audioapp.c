/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "aos/kernel.h"
#include "soc_init.h"
#include "isd9160.h"
#include "ulog/ulog.h"

#define AUDIO_FILE_SDCARD       "audio.data"
#define FIRMWARE_FILE_SDCARD    "isd9160_fw.bin"

#define VRCMD_WAKEUP_LED        GPIO_LED_1
#define VRCMD_INDICATOR_LED     GPIO_LED_2

#define RECORD_LED              GPIO_LED_1
#define PLAYBACK_LED            GPIO_LED_2
#define UPGRADE_LED             GPIO_LED_3

typedef enum {
    INDICATE_START,
    INDICATE_SUCCESS,
    INDICATE_FAILED,

    INDICATE_END
} INDICATE_ENUM;

typedef struct {
    int last_time;
    int interval;
} VRIndicator;

static int key_flag = 0;
static int stop_flag = 0;

static VRIndicator g_vri_data;

static const char *g_vrcmd_list[] = {
    "Xiao Te Xiao Te",
    "Photograph",
    "Scanning QR",
    "Playback music",
    "Pause music",
    "Previous music",
    "Next music",
    "Turn up volume",
    "Turn down volume",
    "Current temperature",
    "Current humidity",
    "Turn on light",
    "Turn off light",
};

static int is_recpb_version(const char *ver)
{
    int i;
    const char *recpb_version_table[] = {
        "v1.01",
        "v1.02",
    };
    const int recpb_version_item = sizeof(recpb_version_table) / sizeof(char *);

    for (i = 0; i < recpb_version_item; ++i) {
        if (strcmp(ver, recpb_version_table[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

static int is_vr_version(const char *ver)
{
    int i;
    const char *vr_version_table[] = {
        "v2.01",
    };
    const int vr_version_item = sizeof(vr_version_table) / sizeof(char *);

    for (i = 0; i < vr_version_item; ++i) {
        if (strcmp(ver, vr_version_table[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

static void blink_led(BOARD_GPIO led, INDICATE_ENUM indicate)
{
    switch (indicate) {
        case INDICATE_START:
            hal_gpio_output_low(&brd_gpio_table[led]);
            break;
        case INDICATE_SUCCESS:
            hal_gpio_output_high(&brd_gpio_table[led]);
            aos_msleep(200);
            hal_gpio_output_low(&brd_gpio_table[led]);
            aos_msleep(200);
            hal_gpio_output_high(&brd_gpio_table[led]);
            aos_msleep(200);
            hal_gpio_output_low(&brd_gpio_table[led]);
            aos_msleep(200);
            hal_gpio_output_high(&brd_gpio_table[led]);
            break;
        case INDICATE_FAILED:
            hal_gpio_output_high(&brd_gpio_table[led]);
            break;
        default:
            LOG("blink_led parameters is invalid.\n");
    }
}

static void isd9160_loop(void *arg)
{
    int ret = 0;

    isd9160_loop_once();
    if (key_flag == 1) {
        stop_flag = 0;
        blink_led(RECORD_LED, INDICATE_START);
        LOG("-->Begin to record voice, press button M to stop if finished!\n");
        ret = handle_record(AUDIO_FILE_SDCARD, &stop_flag);
        LOG("<--Recording Done, please press A to plackback. status:%d!\n", ret);
        if (ret == 0) {
            blink_led(RECORD_LED, INDICATE_SUCCESS);
        } else {
            blink_led(RECORD_LED, INDICATE_FAILED);
        }
        key_flag = 0;
    } else if (key_flag == 2) {
        blink_led(PLAYBACK_LED, INDICATE_START);
        LOG("-->Begin to replay voice!\n");
        ret = handle_playback(AUDIO_FILE_SDCARD);
        if (ret == 0) {
            blink_led(PLAYBACK_LED, INDICATE_SUCCESS);
            LOG("<---Replay done!\n");
        } else {
            blink_led(PLAYBACK_LED, INDICATE_FAILED);
        }
        key_flag = 0;
    } else if (key_flag == 3) {
        blink_led(UPGRADE_LED, INDICATE_START);
        LOG("-->Try to upgrade firmware, check version...\n");
        ret = handle_upgrade(FIRMWARE_FILE_SDCARD);
        if (ret == 0) {
            blink_led(UPGRADE_LED, INDICATE_SUCCESS);
            LOG("<--Firmware upgraded Done!\n");
        } else {
            blink_led(UPGRADE_LED, INDICATE_FAILED);
            LOG("<--Firmware upgraded Failed!\n");
        }
        key_flag = 0;
    }
    aos_post_delayed_action(1000, isd9160_loop, NULL);
}

int handing_shake()
{
    static sys_time_t last_time = 0;
    sys_time_t        now_time  = 0;
    int               ret       = 0;

    now_time = krhino_sys_time_get();
    if (now_time - last_time < 200) {
        ret = 1;
    }
    last_time = now_time;

    return ret;
}

void key1_handle(void *arg)
{
    if (handing_shake())
        return;
    if (key_flag != 0 && stop_flag == 0) {
        stop_flag = 1;
    }
    key_flag = 1;
}

void key2_handle(void *arg)
{
    if (handing_shake())
        return;
    key_flag = 2;
}

void key3_handle(void *arg)
{
    if (handing_shake())
        return;
    key_flag = 3;
}

static void wakeup_delayed_action(void *arg)
{
    hal_gpio_output_high(&brd_gpio_table[VRCMD_WAKEUP_LED]);
}

static void indicator_delayed_action(void *arg)
{
    hal_gpio_output_toggle(&brd_gpio_table[VRCMD_INDICATOR_LED]);
    if (--g_vri_data.last_time > 0) {
        aos_post_delayed_action(g_vri_data.interval, indicator_delayed_action, NULL);
    } else {
        aos_cancel_delayed_action(g_vri_data.interval, indicator_delayed_action, NULL);
    }
}

void isd9160_vr_handle(const char *sw_ver, int cmd_id)
{
    int vrcmd_num = sizeof(g_vrcmd_list) / sizeof(const char *);
    int ret = 0;

    if (cmd_id < 0 || cmd_id >= vrcmd_num) {
        LOG("Received unknow VRCMD %d.", cmd_id);
        return;
    }
    if (cmd_id == 0) {
        hal_gpio_output_low(&brd_gpio_table[VRCMD_WAKEUP_LED]);
        ret = aos_post_delayed_action(10000, wakeup_delayed_action, NULL);
    } else {
        g_vri_data.last_time = cmd_id * 2 - 1;
        g_vri_data.interval = 2000 / g_vri_data.last_time;
        aos_cancel_delayed_action(10000, wakeup_delayed_action, NULL);
        aos_post_delayed_action(2000, wakeup_delayed_action, NULL);
        hal_gpio_output_low(&brd_gpio_table[VRCMD_INDICATOR_LED]);
        aos_post_delayed_action(g_vri_data.interval, indicator_delayed_action, NULL);
    }
    LOG("Received VRCMD: %s\n", g_vrcmd_list[cmd_id]);
}

void isd9160_bootup(const char *sw_ver)
{
    int ret = 0;

    if (is_recpb_version(sw_ver)) {
        ret |= hal_gpio_enable_irq(&brd_gpio_table[GPIO_KEY_1],
                IRQ_TRIGGER_RISING_EDGE, key1_handle, NULL);
        ret |= hal_gpio_enable_irq(&brd_gpio_table[GPIO_KEY_2],
                IRQ_TRIGGER_RISING_EDGE, key2_handle, NULL);
        if (ret != 0) {
            LOG("hal_gpio_enable_irq key return failed.\n");
        }
        LOG("=============Recording/Playback Mode======================\n");
        LOG("--Press Button M to record voice and store into TF card!--\n");
        LOG("--Press Button A to playback which is from TF card!-----\n");
        LOG("--Press Button B to upgrade firmware!-----\n");
        LOG("----------------------------------------------------------\n");
    } else if (is_vr_version(sw_ver)) {
        ret |= hal_gpio_disable_irq(&brd_gpio_table[GPIO_KEY_1]);
        ret |= hal_gpio_disable_irq(&brd_gpio_table[GPIO_KEY_2]);
        if (ret != 0) {
            LOG("hal_gpio_enable_irq key return failed.\n");
        }
        register_vrcmd_callback(isd9160_vr_handle);
        LOG("=============Voice Recognition  Mode======================\n");
        LOG("--Wakeup Commands: Xiao Te Xiao Te\n");
        LOG("--Recognition Commands List:\n"
               "拍照, 扫一扫, 音乐播放, 音乐暂停, 上一首, 下一首, 音量增大,\n"
               "音量减小, 当前温度, 当前湿度, 请开灯, 请关灯--------------\n");
        LOG("--Press Button B to upgrade firmware!-----\n");
        LOG("----------------------------------------------------------\n");
    } else {
        LOG("Unknow version %s\n", sw_ver);
    }
}

int application_start(int argc, char *argv[])
{
    int ret = 0;

    LOG("application started.");
    ret = fatfs_register();
    if (ret != 0) {
        KIDS_A10_PRT("fatfs_register return failed.\n");
    }
    isd9160_i2c_init();
    audio_init();
    ret = hal_gpio_enable_irq(&brd_gpio_table[GPIO_KEY_3],
            IRQ_TRIGGER_RISING_EDGE, key3_handle, NULL);
    if (ret != 0) {
        LOG("hal_gpio_enable_irq key return failed.\n");
    }
    register_swver_callback(isd9160_bootup);
    aos_post_delayed_action(1000, isd9160_loop, NULL);
    aos_loop_run();

    return 0;
}
