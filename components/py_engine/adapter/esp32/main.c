/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "soc/cpu.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/spiram.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/spiram.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/spiram.h"
#endif

#include <fcntl.h>
#include <unistd.h>

#include "board_config.h"
#include "esp_log.h"
#include "esp_partition.h"
#include "esp_system.h"
#include "esp_task.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "modmachine.h"
#include "modnetwork.h"
#include "mpthreadport.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/persistentcode.h"
#include "py/repl.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "shared/readline/readline.h"
#include "shared/runtime/pyexec.h"
#include "uart.h"
#include "usb.h"
#include "usb_serial_jtag.h"
#include "wear_levelling.h"
#if MICROPY_BLUETOOTH_NIMBLE
#include "extmod/modbluetooth.h"
#endif

#if AOS_COMP_KV
#include "aos/kv.h"
#endif

// MicroPython runs as a task under FreeRTOS
#define MP_TASK_PRIORITY   (ESP_TASK_PRIO_MIN + 1)
#define MP_TASK_STACK_SIZE (16 * 1024)

// Set the margin for detecting stack overflow, depending on the CPU architecture.
#if CONFIG_IDF_TARGET_ESP32C3
#define MP_TASK_STACK_LIMIT_MARGIN (2048)
#else
#define MP_TASK_STACK_LIMIT_MARGIN (1024)
#endif

int vprintf_null(const char *format, va_list ap)
{
    // do nothing: this is used as a log target during raw repl mode
    return 0;
}
#include "extmod/vfs.h"
#include "extmod/vfs_posix.h"
// Try to mount the data on "/data" and chdir to it for the boot-up directory.
static int32_t mount_fs(char *mount_point_str)
{
    mp_obj_t mount_point = mp_obj_new_str(mount_point_str, strlen(mount_point_str));
    mp_obj_t bdev = mp_type_vfs_posix.make_new(&mp_type_vfs_posix, 0, 0, NULL);
    int32_t ret = mp_vfs_mount_and_chdir_protected(bdev, mount_point);
    if (ret != 0) {
        printf("mount_fs failed with mount_point: %s\n", mount_point_str);
        return -1;
    }
    return 0;
}

static char *is_mainpy_exist()
{
    // check whether main/pyamp/main.py
    FILE *fd = fopen(AMP_PY_ENTRY_HAAS, "r");
    if (fd != NULL) {
        printf(" ==== python file check %s ====\n", AMP_PY_ENTRY_HAAS);
        fclose(fd);
        return AMP_PY_ENTRY_HAAS;
    }

    fd = fopen(AMP_PY_ENTRY_ROOT, "r");
    if (fd != NULL) {
        printf(" ==== python file check %s ====\n", AMP_PY_ENTRY_ROOT);
        fclose(fd);
        return AMP_PY_ENTRY_ROOT;
    }

    return NULL;
}

void mp_task(void *pvParameter)
{
    volatile uint32_t sp = (uint32_t)get_sp();
    int ret;
#if MICROPY_PY_THREAD
    mp_thread_init(pxTaskGetStackStart(NULL), MP_TASK_STACK_SIZE / sizeof(uintptr_t));
#endif
#if CONFIG_USB_ENABLED
    usb_init();
#elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    usb_serial_jtag_init();
#else
    uart_init();
#endif
    machine_init();

// TODO: CONFIG_SPIRAM_SUPPORT is for 3.3 compatibility, remove after move to 4.0.
#if CONFIG_ESP32_SPIRAM_SUPPORT || CONFIG_SPIRAM_SUPPORT
    // Try to use the entire external SPIRAM directly for the heap
    size_t mp_task_heap_size;
    void *mp_task_heap = (void *)0x3f800000;
    switch (esp_spiram_get_chip_size()) {
    case ESP_SPIRAM_SIZE_16MBITS:
#if CONFIG_SPIRAM_USE_MALLOC
        mp_task_heap_size = 512 * 1024;
        mp_task_heap = malloc(mp_task_heap_size);
#else
        mp_task_heap_size = 2 * 1024 * 1024;
#endif
        break;
    case ESP_SPIRAM_SIZE_32MBITS:
    case ESP_SPIRAM_SIZE_64MBITS:
#if CONFIG_SPIRAM_USE_MALLOC
        mp_task_heap_size = 1 * 1024 * 1024;
        mp_task_heap = malloc(mp_task_heap_size);
#else
        mp_task_heap_size = 4 * 1024 * 1024;
#endif
        break;
    default:
        // No SPIRAM, fallback to normal allocation
        mp_task_heap_size = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
        mp_task_heap = malloc(mp_task_heap_size);
        break;
    }
#elif CONFIG_ESP32S2_SPIRAM_SUPPORT || CONFIG_ESP32S3_SPIRAM_SUPPORT
    // Try to use the entire external SPIRAM directly for the heap
    size_t mp_task_heap_size;
    size_t esp_spiram_size = esp_spiram_get_size();
    void *mp_task_heap = (void *)0x3ff80000 - esp_spiram_size;
    if (esp_spiram_size > 0) {
        mp_task_heap_size = esp_spiram_size;
    } else {
        // No SPIRAM, fallback to normal allocation
        mp_task_heap_size = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
        mp_task_heap = malloc(mp_task_heap_size);
    }
#else
    // Allocate the uPy heap using malloc and get the largest available region
    size_t mp_task_heap_size = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    mp_task_heap_size = 96 * 1024;
    void *mp_task_heap = malloc(mp_task_heap_size);
#endif

soft_reset:
    // initialise the stack pointer for the main thread
    mp_stack_set_top((void *)sp);
    mp_stack_set_limit(MP_TASK_STACK_SIZE - MP_TASK_STACK_LIMIT_MARGIN);
    gc_init(mp_task_heap, mp_task_heap + mp_task_heap_size);
    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));
    // add /data/pyamp to system path
    char *path = MP_FS_ROOT_DIR "/pyamp";
    mp_obj_list_append(mp_sys_path, mp_obj_new_str_via_qstr(path, strlen(path)));
    // add /data/lib/ to system path
    path = MP_FS_ROOT_DIR "/lib";
    mp_obj_list_append(mp_sys_path, mp_obj_new_str_via_qstr(path, strlen(path)));

    mp_obj_list_init(mp_sys_argv, 0);
#if 1
    printf("mount fs\r\n");
    ret = mount_fs("/");
    if (ret != 0) {
        printf(" !!!!!!!! %s, %d, faild to mount fs !!!!!!!!\n", __func__, __LINE__);
    }
    MP_STATE_VM(vfs_cur) = MP_STATE_VM(vfs_mount_table);
#endif

    readline_init0();

    // initialise peripherals
    machine_pins_init();
#if MICROPY_PY_MACHINE_I2S
    machine_i2s_init0();
#endif

    // run boot-up scripts
    // pyexec_frozen_module("_boot.py");
    // do not delete this message, haas studio need use this message judge python status
    printf(" ==== python execute bootpy ====\n");
    pyexec_file_if_exists("boot.py");
    if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL) {
        char *path = is_mainpy_exist();
        if (path != NULL) {
            // do not delete this message, haas studio need use this message judge python status
            printf(" ==== python execute from %s ====\n", path);
            int ret = pyexec_file_if_exists(path);
            if (ret & PYEXEC_FORCED_EXIT) {
                goto soft_reset_exit;
            }
        }
    }
    uint32_t esp_get_free_heap_size(void);
    printf("%s-%d %d\r\n", __func__, __LINE__, esp_get_free_heap_size());
    for (;;) {
        if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
            vprintf_like_t vprintf_log = esp_log_set_vprintf(vprintf_null);
            if (pyexec_raw_repl() != 0) {
                break;
            }
            esp_log_set_vprintf(vprintf_log);
        } else {
            if (pyexec_friendly_repl() != 0) {
                break;
            }
        }
    }

soft_reset_exit:

#if MICROPY_BLUETOOTH_NIMBLE
    mp_bluetooth_deinit();
#endif

    machine_timer_deinit_all();

#if MICROPY_PY_THREAD
    mp_thread_deinit();
#endif

    gc_sweep_all();

    mp_hal_stdout_tx_str("MPY: soft reboot\r\n");

    // deinitialise peripherals
    machine_pins_deinit();
    machine_deinit();
    usocket_events_deinit();

    mp_deinit();
    fflush(stdout);
    goto soft_reset;
}

void mount_fat(void)
{
    int ret;

    static wl_handle_t s_test_wl_handle;
    esp_vfs_fat_mount_config_t mount_config = { .format_if_mount_failed = true, .max_files = 5 };
    ret = esp_vfs_fat_spiflash_mount("", NULL, &mount_config, &s_test_wl_handle);
    if (ret != 0) {
        printf("%s mount fail\r\n", __func__);
        return;
    }
}

static void queue_handler_task(void *p)
{
    py_task_init();
    while (1) {
        /* loop for asynchronous operation */
        if (py_task_yield(200) == 1) {
            printf("pyengine task yield exit! \r\n");
            break;
        }
    }
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

#if AOS_COMP_KV
    ret = aos_kv_init();
    if (ret) {
        printf("kv init fail . ret is %d\r\n", ret);
    }
#endif

    ulog_init();

    littlefs_register();

    TaskHandle_t mp_queue_task_handle;
    ret = xTaskCreatePinnedToCore(queue_handler_task, "queue_handler", 1024 * 8 / sizeof(StackType_t), NULL,
                                  ESP_TASK_PRIO_MIN + 3, &mp_queue_task_handle, MP_TASK_COREID);
    if (ret != pdPASS) {
        printf("queue_handler_task creat failed!\r\n");
        return;
    }

    ret = xTaskCreatePinnedToCore(mp_task, "mp_task", MP_TASK_STACK_SIZE / sizeof(StackType_t), NULL, MP_TASK_PRIORITY,
                                  &mp_main_task_handle, MP_TASK_COREID);
    if (ret != pdPASS) {
        printf("mp_task task creat failed!\r\n");
        return;
    }
}

void nlr_jump_fail(void *val)
{
    printf("NLR jump failed, val=%p\n", val);
    esp_restart();
}

// modussl_mbedtls uses this function but it's not enabled in ESP IDF
void mbedtls_debug_set_threshold(int threshold)
{
    (void)threshold;
}

void *esp_native_code_commit(void *buf, size_t len, void *reloc)
{
    len = (len + 3) & ~3;
    uint32_t *p = heap_caps_malloc(len, MALLOC_CAP_EXEC);
    if (p == NULL) {
        m_malloc_fail(len);
    }
    if (reloc) {
        mp_native_relocate(reloc, buf, (uintptr_t)p);
    }
    memcpy(p, buf, len);
    return p;
}
