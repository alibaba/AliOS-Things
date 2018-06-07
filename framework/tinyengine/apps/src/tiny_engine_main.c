/*
 * Copyright © 2018 alibaba. All rights reserved.
 */

#include <be_osal.h>
#include "be-debuger/be_debuger.h"
#include "be_jse_module.h"
#include "fs/module_fs.h"
#include "gpio/module_gpio.h"
#include "http/module_http.h"
#include "js-lite/module_js_lite.h"
#include "mqtt/module_mqtt.h"
#include "net/module_net.h"
#include "uart/module_uart.h"
#include "wifi/module_wifi.h"
#include "pwm/module_pwm.h"
#include "adc/module_adc.h"
#include "dac/module_dac.h"
#include "wdg/module_wdg.h"
#include "rtc/module_rtc.h"
#include "hw/module_hw.h"
#include "app-mgr/app_mgr.h"
#include "board-mgr/board_mgr.h"
#ifdef BE_OS_AOS
#include <aos/log.h>
#include "ali_crypto.h"
#ifdef AOS_ATCMD
#include <atparser.h>
#endif
#endif

#define MAIN_TAG ("Main")
#define BE_CLI_TAG ("BoneEngine")
#define BOARG_CONFIG_FILE_PATH "/spiffs/board_config.json"
#define CONFIG_LOGMACRO_DETAILS 1

static void tiny_engine_log_cb(const char *file, const char *tag, int level,
                               int line, const char *func, const char *message, ...)
{
    unsigned char msg[BE_LOG_MAX_MESSAGE] = {0};
    be_log_build(file, tag, level, line, func, message, (char *)msg,
                 sizeof(msg));
    printf("%s\r\n", msg);
}

void tiny_engine_load_addon(bool reload)
{
    bone_engine_set_log(tiny_engine_log_cb);
    module_js_lite_load();
    module_process_load();
    module_gpio_register();
#if !defined STM32L496xx && !defined CONFIG_MX108
    module_pwm_register();
    module_adc_register();
    module_dac_register();
    module_wdg_register();
    module_rtc_register();
#endif
#if defined STM32L496xx
    module_lcd_register();
#endif

    module_hw_register();
    module_mqtt_load();
    module_wifi_load();
    module_http_load();
    module_uart_register();
    module_fs_load();
    module_net_load();
    module_i2c_register();
}

static void on_recv_begin(size_t content_length)
{
    be_debug(BE_CLI_TAG, "on_recv_begin: content_length=%zu\r\n",
             content_length);
    app_mgr_set_boneflag(0);
    apppack_init(write_app_pack);
}

static void on_recv_update(unsigned char *data, size_t len)
{
    be_debug(BE_CLI_TAG, "on_recv_update: data=%d", (int)len);
    apppack_update(data, len);
}

static void on_recv_complete(unsigned char *data, size_t len)
{
    be_debug(BE_CLI_TAG, "on_recv_complete: data=%d", (int)len);
    apppack_update(data, len);
    apppack_final();
    app_mgr_set_boneflag(1);
    be_debug(BE_CLI_TAG, "upgrade success, reboot ...");
}

static void send_cli_wifi_reply()
{
    printf("#be-debuger-wifi-reply# \n");
}

static void wifi_event_cb(be_osal_input_event_t *event, void *data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    be_debug(MAIN_TAG, "wifi_event_cb type=%d\n\r", event->code);
    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    if (event->code && event->code < (CODE_WIFI_ON_GOT_IP + 1)) {
        char ip[16] = {0};
        be_osal_wifi_get_ip(ip);
        be_debug(MAIN_TAG, "got ip: %s", ip);
        send_cli_wifi_reply();
#ifndef STM32L496xx
        int status = be_debuger_http_start();
        if (status == 0) {
            be_debug(BE_CLI_TAG, "be-debuger ssdp start\r\n");
            be_debuger_ssdp_start();
        } else {
            be_debug(BE_CLI_TAG, "be_debuger_http_start error, status=%d\r\n",
                     status);
        }
#endif
    }
}

/*open js file and read content to bone_engint*/
const char *load_js_module(const char *moduleName)
{
    int fd;
    int len;
    int size;
    char path[64] = {0};
    char *data = NULL;

    if (moduleName == NULL) {
        return NULL;
    }

    snprintf(path, sizeof(path), "%s/", BE_FS_ROOT_DIR);
    if (moduleName[0] == '.') {
        if (moduleName[1] == '/') {
            strcat(path, moduleName + 2);
        }
    } else {
        strcat(path, moduleName);
    }

    fd = be_osal_open(path, O_RDONLY);
    if (fd < 0) {
        strcat(path, ".js");
        fd = be_osal_open(path, O_RDONLY);
    }

    if (fd < 0) {
        be_warn(MAIN_TAG, "load %s fail \r\n", moduleName);
        return NULL;
    }

    size = be_osal_lseek(fd, 0, SEEK_END);
    be_osal_lseek(fd, 0, SEEK_SET);
    be_debug(MAIN_TAG, "%s file size = %d \r\n", path, size);

    data = malloc(size + 1);
    data[0] = 0;
    len = be_osal_read(fd, data, size);
    if (len > 0) {
        data[len] = 0;
    }

    be_osal_close(fd);
    return data;
}

/*mount app spiffs partion */
int tiny_engine_spiffs_init(void)
{
    int ret = -1;

    be_debug(MAIN_TAG, "tiny_engine_spiffs_init enter\n\r");
    ret = be_osal_spiffs_init();
    if (ret != 0) {
        be_debug(MAIN_TAG, "be_osal_spiffs_init fail %d\n\r", ret);
        return -1;
    }
    be_debug(MAIN_TAG, "tiny_engine_spiffs_init ok\n\r");
    return 0;
}



/*initlize wifi module*/
static void be_wifi_init(void)
{
#ifdef BE_OS_AOS
#if AOS_ATCMD
    at.set_mode(ASYN);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX, AT_RECV_FAIL_POSTFIX,
            AT_SEND_DELIMITER, 1000);
#endif
#ifdef WITH_SAL
    sal_init();
#endif
#endif
    be_osal_register_event_filter(EV_WIFI, wifi_event_cb, NULL);
    be_osal_wifi_init();
}

int init()
{
    int size = 4;
    int BoneEngineEnable = 1;
    const char *default_js_app = "index.js";
    char *data = NULL;

    be_set_log(tiny_engine_log_cb);
    tiny_engine_spiffs_init();
#ifdef BE_OS_AOS
    aos_set_log_level(AOS_LL_WARN);
#endif
    board_mgr_init(BOARG_CONFIG_FILE_PATH);
    be_wifi_init();

    cli_cmd_register_app();
    cli_cmd_register_js();
    cli_cmd_register_file();
    cli_cmd_register_ext();
    cli_cmd_register_dev();

    bone_engine_init();
    tiny_engine_load_addon(false);

    be_debuger_init(on_recv_begin, on_recv_update, on_recv_complete);
    be_debuger_uart_start();
    bone_engine_loadmodule_register(load_js_module);

    if (be_osal_kv_get(BoneFlag, &BoneEngineEnable, &size) != 0) {
        BoneEngineEnable = 1;
    }

    be_debug(MAIN_TAG, "BoneEngine Enable=%d ", BoneEngineEnable);
    if (BoneEngineEnable) {
        data = (char *)load_js_module(default_js_app);
        if (data) {
            be_debug(MAIN_TAG, "Run Js With TinyEngine...");
            bone_engine_start(data);
            free(data);
        }
    }

    return 0;
}

/*TinyEngine Entrance*/
int tiny_engine_start()
{
    be_debug(MAIN_TAG, "tiny_engine_start enter:\r\n");
#ifdef BE_OS_AOS
    ali_crypto_init();
#endif
    init();
    return 0;
}
