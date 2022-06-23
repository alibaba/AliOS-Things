/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "ucamera_service.h"

#include "stdatomic.h"
#include "uart_frame.h"

#define TAG "UART_CAMERA"

#define HEAD_SIZE 2048
#define BODY_SZIE (1024 * 128 + 1)

//  #define USE_LVGL_DRAW_IMAGE
//  #define USE_TFT

typedef enum {
    kWifiSta,
    kUart,
    kTiming
} CamMode_t;

typedef struct _JpegFrame_t {
    uint8_t *buf;
    uint32_t size;
} JpegFrame_t;

typedef struct _uart_ommand_t {
    uint8_t cmd;
    uint8_t *buf;
    uint32_t size;
} uart_command_t;

int g_init_flag = 0;

ucamera_context_t uart_camera;

static atomic_uint_fast8_t capture_image;
static atomic_int_least32_t x;
static atomic_int_least32_t y;
static atomic_int_least32_t cmd_want;
static char token[33];

void set_capture_image(uint8_t show)
{
    atomic_store(&capture_image, show);
}

uint8_t get_capture_image(void)
{
    return (uint8_t)atomic_load(&capture_image);
}


#define CANVAS_WIDTH 320
#define CANVAS_HEIGHT 240

uart_command_t *send_cmd(uint8_t cmd_in, uint8_t *buff, uint32_t size, uint32_t wait_time)
{
    if (wait_time == 0) {
        uart_frame_send(cmd_in, buff, size, true);
        return NULL;
    }

    uart_command_t *cmd = malloc(sizeof(uart_command_t)); //  heap_caps_malloc(sizeof(uart_command_t), MALLOC_CAP_DEFAULT | MALLOC_CAP_8BIT);
    if (cmd == NULL) {
        return NULL;
    }

    atomic_store(&cmd_want, cmd_in);
    uart_frame_send(cmd_in, buff, size, true);
    free(cmd);
    return NULL;
}

static void uart_camera_delete(ucamera_device_t *dev)
{
    free(dev);
}

static bool uart_camera_device_available(void)
{
    return true;
}

int32_t uart_camera_init(ucamera_device_t *dev)
{
    aos_status_t status = -1;

    if (g_init_flag == 0) {
        atomic_store(&capture_image, 1);
        atomic_init(&x, 0);
        atomic_init(&y, 0);
        atomic_init(&cmd_want, -1);
        g_init_flag = 1;
    }
    return 0;
}

int32_t uart_camera_uninit(ucamera_device_t *dev)
{
    set_capture_image(0);
    uart_frame_uninit();
    return 0;
}

int32_t uart_camera_connect(ucamera_device_t *dev)
{
    return 0;
}

void uart_camera_disconnect(ucamera_device_t *dev)
{
}

int32_t uart_camera_config(ucamera_device_t *dev)
{
    uint32_t data = 0;
    const char *ssid = NULL;
    const char *pwd = NULL;
    uint16_t ssid_len = 0;
    uint16_t pwd_len = 0;
    uint8_t *out_buf = NULL;

    if (!dev)
        return -1;

    switch (dev->cmd) {
    case UCAMERA_CMD_SET_UART_PARAMS:
        uart_frame_init(dev->params.uart.rx, dev->params.uart.tx, dev->params.uart.bandrate);
        break;
    case UCAMERA_CMD_SET_FRAME_SIZE:
        send_cmd(1, (uint8_t *)&dev->frame_size.id, 2, 0);
        break;
    case UCAMERA_CMD_SET_MODE:
        if (dev->params.uart.mode == CAM_UART_MODE) {
            data = 5;
            send_cmd(1, (uint8_t *)&data, 2, 0);
            data = CAM_UART_MODE;
            send_cmd(26, (uint8_t *)&data, 1, 0);
        } else if (dev->params.uart.mode == CAM_TIMING_MODE) {
            data = 9;
            send_cmd(1, (uint8_t *)&data, 2, 0);
            data = CAM_TIMING_MODE;
            send_cmd(26, (uint8_t *)&data, 1, 0);
        }
        break;
    case UCAMERA_CMD_SET_LED_BRIGHTNESS:
        data = dev->params.uart.led_brightness;
        if (data > 1024) {
            data = 1024;
        }
        send_cmd(40, (uint8_t *)&data, 2, 0);
        break;
    case UCAMERA_CMD_SET_WIFI_SSID_PWD:
        ssid = dev->params.uart.wifi_ssid;
        pwd = dev->params.uart.wifi_pwd;
        ssid_len = strlen(ssid) + 1;
        pwd_len = strlen(pwd) + 1;
        if ((ssid_len + pwd_len) > 98) {
            return -1;
        }
        out_buf = (uint8_t *)malloc(ssid_len + pwd_len + 2);
        out_buf[0] = ssid_len;
        out_buf[ssid_len] = '\0';
        memcpy(out_buf + 1, ssid, ssid_len);
        memcpy(out_buf + ssid_len + 2, pwd, pwd_len);
        out_buf[ssid_len + pwd_len + 1] = '\0';
        send_cmd(31, out_buf, ssid_len + pwd_len + 2, 0);
        free(out_buf);
        break;
    case UCAMERA_CMD_SET_UPLOAD_TIME:
        data = dev->params.uart.upload_time;
        if (data < 5) {
            data = 5;
        }
        if (data > 65536) {
            data = 65536;
        }
        send_cmd(35, (uint8_t *)&data, 2, 0);
        break;
    default:
        break;
    }

    //  save config
    send_cmd(28, NULL, 0, 0);

    return 0;
}

frame_buffer_t *uart_camera_get_frame(ucamera_device_t *dev)
{
    int32_t ret = 0;
    int32_t cmd;
    uint8_t *buf = NULL;
    uint32_t len;

    while (1) {
        ret = uart_frame_recv(&buf, &len);
        if (ret < 0) {
            goto end;
        }

        cmd = buf[7];

        //  get one jpeg frame
        if (cmd == 0x00) {
            //printf("get one jpeg frame, len:%d\n", len);
            dev->frame.buf = (uint8_t *)calloc(1, len);
            if (dev->frame.buf == NULL) {
                printf("allocate frame.buf fail\n");
                return NULL;
            }

            memcpy(dev->frame.buf, &buf[8], len);
            dev->frame.len = len;
            dev->frame.width = dev->frame_size.width;
            dev->frame.height = dev->frame_size.height;
            dev->frame.format = FRAME_FORMAT_JPEG;
            free(buf);
            //printf("get jpeg frame success\n");
            return &dev->frame;
        } else {
            if (buf)
                free(buf);
            continue;
        }
    }

end:
    if (buf) {
        free(buf);
    }
    return NULL;

}

static ucamera_device_t *uart_camera_device_create(int devindex)
{
    ucamera_device_t *dev = NULL;

    /* Initialize all variables that we clean on shutdown */
    dev = (ucamera_device_t *) calloc(1, sizeof(ucamera_device_t));
    if (!dev) {
        LOGE(TAG, "malloc camera device fail\n");
        return 0;
    }
    dev->is_dummy = false;
    dev->frame_size.width = UART_CAMERA_FRAME_WIDTH;
    dev->frame_size.height = UART_CAMERA_FRAME_HEIGHT;

    /* Set the function pointers */
    dev->camera_init = uart_camera_init;
    dev->camera_uninit = uart_camera_uninit;
    dev->camera_connect = uart_camera_connect;
    dev->camera_disconnect = uart_camera_disconnect;
    dev->camera_config = uart_camera_config;
    dev->camera_get_frame = uart_camera_get_frame;
    dev->camera_free = uart_camera_delete;

    LOG("create camera device successfully\n");
    return dev;
}

ucamera_context_t uart_camera = {
    UART_CAMERA_NAME, "UART Camera", CAM_UART_TYPE,
    uart_camera_device_available, uart_camera_device_create
};
