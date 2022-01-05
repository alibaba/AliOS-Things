/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_log.h"
#include "lightduer_types.h"
#include "lightduer_dev_info.h"

static const char* TAG="ESP_DEV_INFO";

#define FIRMWARE_VERSION "1.0.0.0"

const duer_system_static_info_t g_system_static_info = {
    .os_version         = "FreeRTOS V8.2.0",
    .sw_version         = "ESP32_LYRAT_"FIRMWARE_VERSION,
    .brand              = "ESPRESSIF",
    .hardware_version   = "ESP32",
    .equipment_type     = "ESP32_LYRAT",
    .ram_size           = 5120,
    .rom_size           = 4096,
};

static int get_firmware_version(char *firmware_version)
{
    strncpy(firmware_version, FIRMWARE_VERSION, FIRMWARE_VERSION_LEN);
    return 0;
}

static struct DevInfoOps dev_info_ops = {
    .get_firmware_version = get_firmware_version,
};

int duer_init_device_info(void)
{
    int ret = 0;

    ret = duer_register_device_info_ops(&dev_info_ops);
    if (ret != 0) {
        ESP_LOGE(TAG, "Dev Info: Register dev ops failed");
    }
    return ret;
}
