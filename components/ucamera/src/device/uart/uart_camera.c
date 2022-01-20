/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "ucamera_service.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "stdatomic.h"
#include "uart_frame.h"
#include "lvgl.h"
#include "lvgl_helpers.h"
#include "axp192.h"
#include "i2c_manager.h"
#include "tft/tft.h"

#define TAG "UART_CAMERA"

#define HEAD_SIZE 2048
#define BODY_SZIE (1024 * 128 + 1)

//  #define USE_LVGL_DRAW_IMAGE
//  #define USE_TFT
ucamera_context_t uart_camera;

static QueueHandle_t jpeg_frame_queue = NULL;
static QueueHandle_t command_queue = NULL;
static atomic_uint_fast8_t capture_image;
static atomic_int_least32_t x;
static atomic_int_least32_t y;
static atomic_int_least32_t cmd_want;
static char token[33];

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

void set_capture_image(uint8_t show)
{
    atomic_store(&capture_image, show);
}

uint8_t get_capture_image(void)
{
    return (uint8_t)atomic_load(&capture_image);
}

#if defined(USE_LVGL_DRAW_IMAGE) || defined(USE_TFT)
static xSemaphoreHandle cap_sem;
#endif
#define CANVAS_WIDTH 320
#define CANVAS_HEIGHT 240

#ifdef USE_LVGL_DRAW_IMAGE
#define LV_TICK_PERIOD_MS 1
#define I2C_0 i2c_hal(0)
SemaphoreHandle_t xGuiSemaphore;

static void lv_tick_task(void *arg)
{
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}
static void guiTask(void *pvParameter)
{
    (void) pvParameter;
    // ESP_LOGE(TAG, "gui task entern");
    xGuiSemaphore = xSemaphoreCreateMutex();
    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    while (1) {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));

        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }

    vTaskDelete(NULL);
}

static void start_display(void)
{
   // ESP_LOGE(TAG, "start_display");
   xTaskCreatePinnedToCore(guiTask, "gui", 4096 * 10, NULL, 1, NULL, 0);
}

static void driver_init(void)
{
    axp192_init(I2C_0);
    axp192_ioctl(I2C_0, AXP192_GPIO0_SET_LEVEL, AXP192_HIGH);  //  M-Bus Power
    axp192_ioctl(I2C_0, AXP192_GPIO1_SET_LEVEL, AXP192_LOW);   //  Enable LED
    axp192_ioctl(I2C_0, AXP192_GPIO2_SET_LEVEL, AXP192_LOW);   //  Disable speaker
    axp192_ioctl(I2C_0, AXP192_LDO2_SET_VOLTAGE, 3300);        //  Set LDO2 LCD&TP voltage
    axp192_ioctl(I2C_0, AXP192_LDO2_ENABLE);                   //  Enable LDO2
    axp192_ioctl(I2C_0, AXP192_GPIO4_SET_LEVEL, AXP192_LOW);   //  LCD&TP Reset
    vTaskDelay(100 / portTICK_PERIOD_MS);
    axp192_ioctl(I2C_0, AXP192_GPIO4_SET_LEVEL, AXP192_HIGH);  //  LCD&TP Hold
    vTaskDelay(100 / portTICK_PERIOD_MS);
    lvgl_driver_init();
    start_display();
}

static void draw_image(uint8_t *buf, uint32_t len)
{
    static int init = 0;
    static lv_obj_t *canvas = NULL;
    static int8_t *cbuf = NULL;

    if (!buf) {
        return NULL;
    }
    if (init == 0) {
        printf("LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT): %d\n", LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT));
        cbuf = (int8_t *)malloc(LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT));

        canvas = lv_canvas_create(lv_scr_act());
        lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
        lv_obj_center(canvas);
        lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_GREY, 3), LV_OPA_COVER);
    }

    lv_img_dsc_t img;
    img.data = buf;
    img.data_size = len;
    img.header.always_zero = 0;
    img.header.w = CANVAS_WIDTH;
    img.header.h = CANVAS_HEIGHT;
    img.header.cf = LV_IMG_CF_TRUE_COLOR;
    lv_draw_img_dsc_t img_dsc;
    lv_draw_img_dsc_init(&img_dsc);
    lv_canvas_draw_img(canvas, 0, 0, &img, &img_dsc);

    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.radius = 0;
    rect_dsc.bg_opa = LV_OPA_0;
    //  rect_dsc.bg_grad_dir = LV_GRAD_DIR_HOR;
    rect_dsc.bg_color = lv_palette_main(LV_PALETTE_NONE);
    //  rect_dsc.bg_grad_color = lv_palette_main(LV_PALETTE_BLUE);
    rect_dsc.border_width = 2;
    //  rect_dsc.border_opa = LV_OPA_90;
    rect_dsc.border_color = lv_palette_main(LV_PALETTE_RED);
    rect_dsc.shadow_width = 5;
    rect_dsc.shadow_ofs_x = 0;
    rect_dsc.shadow_ofs_y = 0;
    lv_canvas_draw_rect(canvas, 70, 60, 100, 70, &rect_dsc);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_palette_main(LV_PALETTE_YELLOW);
    lv_canvas_draw_text(canvas, 40, 20, 100, &label_dsc, "Some text on text canvas");

    init = 1;
}
static lv_disp_draw_buf_t disp_buf;
static lv_disp_drv_t disp_drv;
static void lvgl_display_init(void)
{

    // lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    //  printf("(DISP_BUF_SIZE*sizeof(lv_color_t): %d\n", DISP_BUF_SIZE*sizeof(lv_color_t));
    //  lv_color_t *buf1 = (lv_color_t *)malloc(DISP_BUF_SIZE*sizeof(lv_color_t));
    lv_color_t *buf1 = (lv_color_t *)malloc(12800);
    assert(buf1 != NULL);

    // lv_color_t *buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    //  lv_color_t *buf2 = (lv_color_t *)malloc(DISP_BUF_SIZE*sizeof(lv_color_t));
    lv_color_t *buf2 = (lv_color_t *)malloc(12800);
    assert(buf2 != NULL);

    uint32_t size_in_px = 6400; // DISP_BUF_SIZE;

    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 240;
    disp_drv.flush_cb = disp_driver_flush;

    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);
}
#endif

#if defined(USE_LVGL_DRAW_IMAGE) || defined(USE_TFT)
static void jpeg_capture_task(void *arg)
{
    int32_t ret = 0;
    //  frame_buffer_t frame;
    //  frame.buf = NULL;
    //  frame.len = 0;
    ucamera_device_t *dev = (ucamera_device_t *)arg;
    printf("jpeg_capture_task start\n");
    while (1) {
        long long start_recv = aos_now_ms();
        ret = xQueueReceive(jpeg_frame_queue, &dev->frame, portMAX_DELAY);
        if (ret == pdTRUE) {
            long long end_recv = aos_now_ms();
            printf("xQueueReceive ok, excute time: %lld\n", end_recv - start_recv);
            long long prev = aos_now_ms();
#ifdef USE_LVGL_DRAW_IMAGE
            draw_image(dev->frame.buf, dev->frame.len);
            xSemaphoreGive(cap_sem);
#endif
#ifdef USE_TFT
            TFT_jpg_image(atomic_load(&x), atomic_load(&y), 0, NULL, dev->frame.buf, dev->frame.len);
            xSemaphoreGive(cap_sem);
#endif
            long long cur = aos_now_ms();
            printf("draw image done, excute time: %lld\n", (cur - prev));
            free(dev->frame.buf);
            dev->frame.buf = NULL;
        } else {
            printf("xQueueReceive fail\n");
        }
    }
}
#endif

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

#ifdef DISABLE_USE_UART_QUEUE
    atomic_store(&cmd_want, cmd_in);
    uart_frame_send(cmd_in, buff, size, true);
#else
    atomic_store(&cmd_want, -1);
    while (xQueueReceive(command_queue, cmd, 0) == pdTRUE) {
        free(cmd->buf);
    }
    atomic_store(&cmd_want, cmd_in);
    uart_frame_send(cmd_in, buff, size, true);
    if (xQueueReceive(command_queue, cmd, wait_time) == pdTRUE) {
        return cmd;
    }
#endif
    //  heap_caps_free(cmd);
    free(cmd);
    return NULL;
}

void frame_recv_callback(int cmd, const uint8_t *buf, int len)
{
    if (atomic_load(&cmd_want) == (cmd & 0x7f)) {
        uart_command_t recv;
        recv.buf = (uint8_t *)calloc(1, len); //  heap_caps_calloc(len, sizeof(uint8_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
        if (recv.buf == NULL) {
            return ;
        }
        recv.size = len;
        memcpy(recv.buf, buf, len);
        if (xQueueSend(command_queue, &recv, 0) != pdTRUE) {
            free(recv.buf);
        }
    }

    if (cmd == 0x00) {
        printf("receive frame from queue 0\n");
        if (get_capture_image() != 0) {
            frame_buffer_t frame;
            printf("receive frame from queue 1\n");
            frame.buf = (uint8_t *)calloc(1, len); //  heap_caps_calloc(len, sizeof(uint8_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
            if (frame.buf == NULL) {
                printf("allocate frame.buf fail\n");
                return;
            }

            memcpy(frame.buf, buf, len);
            frame.len = len;

            if (xQueueSend(jpeg_frame_queue, &frame, 0) != pdTRUE) {
                printf("xQueueSend fail\n");
                free(frame.buf);
            }
        }
    }
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
    if (jpeg_frame_queue == NULL) {
        jpeg_frame_queue = xQueueCreate(1, sizeof(frame_buffer_t));
        command_queue = xQueueCreate(2, sizeof(uart_command_t));
        atomic_store(&capture_image, 1);
        atomic_init(&x, 0);
        atomic_init(&y, 0);
        atomic_init(&cmd_want, -1);
#ifdef USE_LVGL_DRAW_IMAGE
        lv_init();
        driver_init();
        lvgl_display_init();
#endif
#if defined(USE_LVGL_DRAW_IMAGE) || defined(USE_TFT)
        cap_sem = xSemaphoreCreateBinary();
        xSemaphoreGive(cap_sem);
#endif
#if defined(USE_LVGL_DRAW_IMAGE) || defined(USE_TFT)
        xTaskCreatePinnedToCore(jpeg_capture_task, "jpeg show task", 4 * 1024, (void *)dev, 1, NULL, 0);
#endif
    } else {
        uart_update_pin(dev->params.uart.rx, dev->params.uart.tx);
    }

    return 0;
}

int32_t uart_camera_uninit(ucamera_device_t *dev)
{
    set_capture_image(0);
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
        if (jpeg_frame_queue != NULL)
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
#if defined(USE_LVGL_DRAW_IMAGE) || defined(USE_TFT)
    set_capture_image(1);
    if (xSemaphoreTake(cap_sem, portMAX_DELAY) == pdTRUE) {
        if (dev->frame.buf != NULL && dev->frame.len != 0)
            return &dev->frame;
        else
            return NULL;
    } else {
        return NULL;
    }
#else
#ifdef DISABLE_USE_UART_QUEUE
    while (1) {
        ret = uart_frame_recv(&buf, &len);
        if (ret < 0) {
            goto end;
        }

        cmd = buf[7];

        //  get one jpeg frame
        if (cmd == 0x00) {
            dev->frame.buf = (uint8_t *)calloc(1, len); //  heap_caps_calloc(len, sizeof(uint8_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
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
            //  printf("get jpeg frame success\n");
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
#else
    set_capture_image(1);
    ret = xQueueReceive(jpeg_frame_queue, &dev->frame, portMAX_DELAY);
    if (ret == pdPASS) {
        dev->frame.width = dev->frame_size.width;
        dev->frame.height = dev->frame_size.height;
        dev->frame.format = FRAME_FORMAT_JPEG;
        printf("[uart_camera_get_frame]get frame success\n");
        return &dev->frame;
    }
    return NULL;
#endif
#endif
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
