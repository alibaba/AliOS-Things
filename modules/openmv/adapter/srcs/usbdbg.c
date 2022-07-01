/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * USB debugger.
 */
#include <string.h>
#include <stdio.h>
#include "py/nlr.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/lexer.h"
#include "py/parse.h"
#include "py/compile.h"
#include "py/runtime.h"
// #include "pendsv.h"

#include "imlib.h"
#include "usb_cdc.h"
#include "device/dcd.h"
#if MICROPY_PY_SENSOR
// #include "cambus.h"
#include "sensor.h"
#endif
#include "framebuffer.h"
#include "ff.h"
#include "usbdbg.h"
#include "omv_boardconfig.h"
#include "py_image.h"
#include "esp_log.h"

static int xfer_bytes;
static int xfer_length;
static enum usbdbg_cmd cmd;

static volatile bool script_ready;
static volatile bool script_running;
static vstr_t script_buf;
static mp_obj_t mp_const_ide_interrupt = MP_OBJ_NULL;

// These functions must be implemented in MicroPython CDC driver.
extern uint32_t usb_cdc_buf_len();
extern uint32_t usb_cdc_get_buf(uint8_t *buf, uint32_t len);

void usbdbg_init()
{
    cmd = USBDBG_NONE;
    script_ready = false;
    script_running = false;
    vstr_init(&script_buf, 100 * 1024);
    mp_const_ide_interrupt = mp_obj_new_exception_msg(&mp_type_Exception, "IDE interrupt");
}

void usbdbg_wait_for_command(uint32_t timeout)
{
    for (mp_uint_t ticks = mp_hal_ticks_ms(); ((mp_hal_ticks_ms() - ticks) < timeout) && (cmd != USBDBG_NONE); );
}


bool usbdbg_script_ready()
{
    return script_ready;
}

vstr_t *usbdbg_get_script()
{
    return &script_buf;
}

void usbdbg_set_script_running(bool running)
{
    script_running = running;
}

bool usbdbg_handle_script_status(void)
{
  bool _script_running = script_running;
  script_ready = script_running = false;
  return _script_running;
}

inline void usbdbg_set_irq_enabled(bool enabled)
{
    if (enabled) {
        dcd_int_enable(0);
    } else {
        dcd_int_disable(0);
    }
}

void usbdbg_data_in(void *buffer, int length)
{
    ESP_LOGD("usbdbg", "usbdbg_data_in(%d)", cmd);
    switch (cmd) {
        case USBDBG_FW_VERSION: {
            uint32_t *ver_buf = buffer;
            ver_buf[0] = FIRMWARE_VERSION_MAJOR;
            ver_buf[1] = FIRMWARE_VERSION_MINOR;
            ver_buf[2] = FIRMWARE_VERSION_PATCH;
            cmd = USBDBG_NONE;
            break;
        }

        case USBDBG_TX_BUF_LEN: {
            uint32_t tx_buf_len = usb_cdc_buf_len();
            memcpy(buffer, &tx_buf_len, sizeof(tx_buf_len));
            cmd = USBDBG_NONE;
            break;
        }

        case USBDBG_SENSOR_ID: {
            int sensor_id = 0x00;
            // #if MICROPY_PY_SENSOR
            if (sensor_is_detected() == 1) {
                
                sensor_id = sensor_get_id();
            }
            else
            sensor_id = 0xff;
            // #endif
            memcpy(buffer, &sensor_id, 4);
            cmd = USBDBG_NONE;
            break;
        }

        case USBDBG_TX_BUF: {
            xfer_bytes += usb_cdc_get_buf(buffer, length);
            if (xfer_bytes == xfer_length) {
                cmd = USBDBG_NONE;
            }
            break;
        }

        case USBDBG_FRAME_SIZE:
            // Return 0 if FB is locked or not ready.
            ((uint32_t*)buffer)[0] = 0;
                        // ESP_LOGI("111","JPEG_FB()->w %d JPEG_FB()->w %d JPEG_FB()->w %d",JPEG_FB()->w,JPEG_FB()->h,JPEG_FB()->size);

            // Try to lock FB. If header size == 0 frame is not ready
            if (mutex_try_lock(&JPEG_FB()->lock, MUTEX_TID_IDE)) {
                // If header size == 0 frame is not ready
                if (JPEG_FB()->size == 0) {
                    // unlock FB
                    mutex_unlock(&JPEG_FB()->lock, MUTEX_TID_IDE);
                } else {
                    // Return header w, h and size/bpp
                    ((uint32_t*)buffer)[0] = JPEG_FB()->w;
                    ((uint32_t*)buffer)[1] = JPEG_FB()->h;
                    ((uint32_t*)buffer)[2] = JPEG_FB()->size;
                }
            }
            cmd = USBDBG_NONE;
            break;

        case USBDBG_FRAME_DUMP:
        // ESP_LOGI("222","JPEG_FB()->w %d JPEG_FB()->w %d JPEG_FB()->w %d",JPEG_FB()->w,JPEG_FB()->h,JPEG_FB()->size);

            if (xfer_bytes < xfer_length) {
                memcpy(buffer, JPEG_FB()->pixels+xfer_bytes, length);
                xfer_bytes += length;
                if (xfer_bytes == xfer_length) {
                    cmd = USBDBG_NONE;
                    JPEG_FB()->w = 0; JPEG_FB()->h = 0; JPEG_FB()->size = 0;
                    mutex_unlock(&JPEG_FB()->lock, MUTEX_TID_IDE);
                }
            }
            // if (xfer_bytes < xfer_length) {
            //     memcpy(buffer, JPEG_FB()->pixels+xfer_bytes, length);
            //     xfer_bytes += length;
            //     if (xfer_bytes == xfer_length) {
            //         cmd = USBDBG_NONE;
            //         mutex_unlock(&JPEG_FB()->lock, MUTEX_TID_IDE);
            //     }
            // }
            break;

        case USBDBG_ARCH_STR: {
            unsigned int uid[3] = {
            #if (OMV_UNIQUE_ID_SIZE == 2)
                0U,
            #else
                *((unsigned int *) (OMV_UNIQUE_ID_ADDR + 8)),
            #endif
                *((unsigned int *) (OMV_UNIQUE_ID_ADDR + 4)),
                *((unsigned int *) (OMV_UNIQUE_ID_ADDR + 0)),
            };
            snprintf((char *) buffer, 64, "%s [%s:%08X%08X%08X]",
                    OMV_ARCH_STR, OMV_BOARD_TYPE, uid[0], uid[1], uid[2]);
            cmd = USBDBG_NONE;
            break;
        }

        case USBDBG_SCRIPT_RUNNING: {
            uint32_t *buf = buffer;
            buf[0] = (uint32_t) script_running;
            cmd = USBDBG_NONE;
            break;
        }
        default: /* error */
            break;
    }
}

void usbdbg_data_out(void *buffer, int length)
{
    ESP_LOGD("usbdbg", "usbdbg_data_out(%d)", cmd);
    fflush(stdout);
    switch (cmd) {
        case USBDBG_FB_ENABLE: {
            uint32_t enable = *((int32_t*)buffer);
            JPEG_FB()->enabled = enable;
            if (enable == 0) {
                // When disabling framebuffer, the IDE might still be holding FB lock.
                // If the IDE is not the current lock owner, this operation is ignored.
                mutex_unlock(&JPEG_FB()->lock, MUTEX_TID_IDE);
            }
            cmd = USBDBG_NONE;
            break;
        }

        case USBDBG_SCRIPT_EXEC:
            // check if GC is locked before allocating memory for vstr. If GC was locked
            // at least once before the script is fully uploaded xfer_bytes will be less
            // than the total length (xfer_length) and the script will Not be executed.
            ESP_LOGD("usbdbg", "USBDBG_SCRIPT_EXEC");
            //if (!script_running && !gc_is_locked()) {
            if(!script_running) {
                vstr_add_strn(&script_buf, buffer, length);
                xfer_bytes += length;
                if (xfer_bytes == xfer_length) {
                    // Set script ready flag
                    script_ready = true;

                    // Set script running flag
                    script_running = true;

                    // Disable IDE IRQ (re-enabled by pyexec or main).
                    usbdbg_set_irq_enabled(false);

                    // Clear interrupt traceback
                    mp_obj_exception_clear_traceback(mp_const_ide_interrupt);
                    //MICROPY_WRAP_MP_SCHED_EXCEPTION(mp_const_ide_interrupt);
                    mp_sched_exception(mp_const_ide_interrupt);
                }
            }
            break;

        case USBDBG_TEMPLATE_SAVE: {
            #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
            image_t image;
            framebuffer_initialize_image(&image);

            // null terminate the path
            length = (length == 64) ? 63:length;
            ((char*)buffer)[length] = 0;

            rectangle_t *roi = (rectangle_t*)buffer;
            char *path = (char*)buffer+sizeof(rectangle_t);

            imlib_save_image(&image, path, roi, 50);

            // raise a flash IRQ to flush image
            //NVIC->STIR = FLASH_IRQn;
            #endif  //IMLIB_ENABLE_IMAGE_FILE_IO
            break;
        }

        case USBDBG_DESCRIPTOR_SAVE: {
            #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)\
                && defined(IMLIB_ENABLE_KEYPOINTS)
            image_t image;
            framebuffer_initialize_image(&image);

            // null terminate the path
            length = (length == 64) ? 63:length;
            ((char*)buffer)[length] = 0;

            rectangle_t *roi = (rectangle_t*)buffer;
            char *path = (char*)buffer+sizeof(rectangle_t);

            py_image_descriptor_from_roi(&image, path, roi);
            #endif  //IMLIB_ENABLE_IMAGE_FILE_IO && IMLIB_ENABLE_KEYPOINTS
            cmd = USBDBG_NONE;
            break;
        }

        case USBDBG_ATTR_WRITE: {
            #if MICROPY_PY_SENSOR
            /* write sensor attribute */
            int32_t attr= *((int32_t*)buffer);
            int32_t val = *((int32_t*)buffer+1);
            switch (attr) {
                case ATTR_CONTRAST:
                    sensor_set_contrast(val);
                    break;
                case ATTR_BRIGHTNESS:
                    sensor_set_brightness(val);
                    break;
                case ATTR_SATURATION:
                    sensor_set_saturation(val);
                    break;
                case ATTR_GAINCEILING:
                    sensor_set_gainceiling(val);
                    break;
                default:
                    break;
            }
            #endif
            cmd = USBDBG_NONE;
            break;
        }

        default: /* error */
            break;
    }
}

void usbdbg_control(void *buffer, uint8_t request, uint32_t length)
{
    cmd = (enum usbdbg_cmd) request;
    ESP_LOGD("usbdbg", "usbdbg_control(%d)\r\n", cmd);
    switch (cmd) {
        case USBDBG_FW_VERSION:
            xfer_bytes = 0;
            xfer_length = length;
            break;

        case USBDBG_FRAME_SIZE:
            xfer_bytes = 0;
            xfer_length = length;
            break;

        case USBDBG_FRAME_DUMP:
            xfer_bytes = 0;
            xfer_length = length;
            break;

        case USBDBG_ARCH_STR:
            xfer_bytes = 0;
            xfer_length = length;
            break;

        case USBDBG_SCRIPT_EXEC:
            xfer_bytes = 0;
            xfer_length = length;
            vstr_reset(&script_buf);
            break;

        case USBDBG_SCRIPT_STOP:
            if (script_running) {
                // Set script running flag
                script_running = false;

                // Disable IDE IRQ (re-enabled by pyexec or main).
                usbdbg_set_irq_enabled(false);

                // interrupt running code by raising an exception
                mp_obj_exception_clear_traceback(mp_const_ide_interrupt);
                mp_sched_exception(mp_const_ide_interrupt);
            }
            cmd = USBDBG_NONE;
            break;

        case USBDBG_SCRIPT_SAVE:
            // TODO: save running script
            cmd = USBDBG_NONE;
            break;

        case USBDBG_SCRIPT_RUNNING:
            xfer_bytes = 0;
            xfer_length =length;
            break;

        case USBDBG_TEMPLATE_SAVE:
        case USBDBG_DESCRIPTOR_SAVE:
            /* save template */
            xfer_bytes = 0;
            xfer_length =length;
            break;

        case USBDBG_ATTR_WRITE:
            xfer_bytes = 0;
            xfer_length =length;
            break;

        case USBDBG_SYS_RESET:
            //NVIC_SystemReset();
            esp_restart();
            break;

        case USBDBG_SYS_RESET_TO_BL:{
            #if defined(RTC_BASE)
            RTC_HandleTypeDef RTCHandle;
            RTCHandle.Instance = RTC;
            HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR0, 0xDF59);
            #endif
            //NVIC_SystemReset();
            esp_restart();
            break;
        }
        
        case USBDBG_FB_ENABLE: {
            xfer_bytes = 0;
            xfer_length = length;
            break;
        }

        case USBDBG_TX_BUF:
        case USBDBG_TX_BUF_LEN:
            xfer_bytes = 0;
            xfer_length = length;
            break;

        case USBDBG_SENSOR_ID:
            xfer_bytes = 0;
            xfer_length = length;
            break;

        default: /* error */
            cmd = USBDBG_NONE;
            break;
    }
}
