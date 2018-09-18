/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <k_api.h>
#include <aos/aos.h>
#include "fatfs.h"
#include "st7789.h"
#include "gc0329.h"
#include "camera_demo.h"
#include "fatfs.h"

static uint8_t sd_on = 0;
static uint8_t keyB = 0;
uint16_t pBuffer[ST7789_WIDTH * ST7789_HEIGHT];
uint8_t pBuffer_t[3 * ST7789_WIDTH];
uint32_t counter = 0;

#define RGB565_TO_R(pixel)  ((pixel & 0x1f) << 3)
#define RGB565_TO_G(pixel)  ((pixel & 0x7e0) >> 3)
#define RGB565_TO_B(pixel)  ((pixel & 0xf800) >> 8)

int handing_shake()
{
    static sys_time_t last_time = 0;
    sys_time_t now_time = 0;
    int ret = 0;

    now_time = krhino_sys_time_get();
    if (now_time - last_time < 200) {
        ret = 1;
    }
    last_time = now_time;

    return ret;
}

void keyB_handle(void *arg)
{
    if (handing_shake()) {
        return;
    }
    keyB = 1;
}

void init_dcmi_dir(void)
{
    int ret;
    aos_dir_t *dp = (aos_dir_t *)aos_opendir("/sdcard/DCMI");
    if (!dp) {
        ret = aos_mkdir("/sdcard/DCMI");
        printf("aos_mkdir , ret = %d\n", ret);
    } else {
        ret = aos_closedir(dp);
        printf("aos_closedir , ret = %d\n", ret);
    }
}

static void get_images_index(uint32_t base_index)
{
    uint8_t str[30];
    uint32_t index = 0;
    struct stat st;
    int ret;

    for (index = base_index;; index++) {
        memset(str, 0, sizeof(str));
        sprintf((char *)str, "/sdcard/DCMI/image_%lu.bmp", index);
        ret = aos_stat((char *)str, &st);
        if (ret < 0) {
            counter = index;
            break;
        }
    }
}

static uint8_t SavePicture(void)
{
    uint8_t x, y;
    uint8_t nHeigh = ST7789_HEIGHT;
    uint8_t nWidth = ST7789_WIDTH;
    uint16_t tmp_data, bm;
    uint32_t pixel_index;
    BmpHead bmpheaher;
    InfoHead infoheader;
    uint8_t str[30];
    int fd, ret = HAL_OK;

    bm = 0x4d42; //"BM"
    bmpheaher.imageSize = nHeigh * nHeigh * 3 + 54;
    bmpheaher.blank = 0;
    bmpheaher.startPosition = 54;

    infoheader.Length = 40;
    infoheader.height = ST7789_HEIGHT;
    infoheader.width = ST7789_WIDTH;
    infoheader.colorPlane = 1;
    infoheader.bitColor = 24;
    infoheader.zipFormat = 0;
    infoheader.realSize = nHeigh * nHeigh * 4;
    infoheader.xPels = 0;
    infoheader.yPels = 0;
    infoheader.colorUse = 0;
    infoheader.colorImportant = 0;

    get_images_index(counter);
    sprintf((char *)str, "/sdcard/DCMI/image_%lu.bmp", counter);

    fd = aos_open((const char *)str, O_RDWR | O_CREAT | O_TRUNC);
    if (fd < 0) {
        printf("aos_open %s failed\n", str);
        return HAL_ERROR;

    }
    ret = aos_write(fd, &bm, sizeof(bm));
    if (ret != sizeof(bm)) {
        printf("aos_write bm failed\n");
        ret = HAL_ERROR;
        goto end;
    }

    ret = aos_write(fd, &bmpheaher, sizeof(bmpheaher));
    if (ret != sizeof(bmpheaher)) {
        printf("aos_write bmpheaher failed\n");
        ret = HAL_ERROR;
        goto end;
    }
    ret = aos_write(fd, &infoheader, sizeof(infoheader));
    if (ret != sizeof(infoheader)) {
        printf("aos_write infoheader failed\n");
        ret = HAL_ERROR;
        goto end;
    }

    for (x = 0; x < nHeigh; x++) {
        pixel_index = (nHeigh - 1 - x) * nWidth;
        for (y = 0; y < nWidth; y++) {
            tmp_data = ((pBuffer[pixel_index + y] >> 8) & 0xff) | ((pBuffer[pixel_index + y] << 8) & 0xff00);
            pBuffer_t[3 * y] = RGB565_TO_R(tmp_data);
            pBuffer_t[3 * y + 1] = RGB565_TO_G(tmp_data);
            pBuffer_t[3 * y + 2] = RGB565_TO_B(tmp_data);
        }
        ret = aos_write(fd, pBuffer_t, sizeof(pBuffer_t));
        if (ret != sizeof(pBuffer_t)) {
            //printf("aos_write nHeigh = %d  failed\n", x);
            ret = HAL_ERROR;
            goto end;
        }
    }
    ret = HAL_OK;
    //printf("image_%lu.bmp saved ok\n", counter);
    counter++;

end:
    aos_close(fd);
    if (ret != HAL_OK) {
        aos_unlink((const char *)str);
    } else {
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    }

    return ret;
}

int camera_to_sd(void)
{
    int times;
    int ret = HAL_ERROR;

    HAL_DCMI_Suspend(&hdcmi);
    times = 0;
    while (times++ < 10) {
        if (SavePicture() == HAL_OK) {
            ret = HAL_OK;
            break;
        } else {
            krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND / 10);
        }
    }
    HAL_DCMI_Resume(&hdcmi);

    return ret;
}

static void app_delayed_action(void *arg)
{
    int ret = 0;
    if (sd_on == 0) {
        return;
    }
    while (1) {
        if (keyB) {
            //printf("key3 pressed\n");
            ret = camera_to_sd();
            if (ret == HAL_OK) {
                hal_gpio_output_low(&brd_gpio_table[GPIO_GS_LED]);
                krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND * 2);
                hal_gpio_output_high(&brd_gpio_table[GPIO_GS_LED]);
            }
            keyB = 0;
        }
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND / 2);
    }
}

int application_start(int argc, char *argv[])
{
    int ret;

    LOG("camera application started.");
    /* init LCD */
    ret = fatfs_register();
    if (ret == 0) {
        sd_on = 1;
        init_dcmi_dir();
        get_images_index(counter);
    }
    LOG("sd_on = %d\n", sd_on);
    ret = hal_gpio_enable_irq(&brd_gpio_table[GPIO_KEY_3], IRQ_TRIGGER_RISING_EDGE, keyB_handle, NULL);
    if (ret != 0) {
        printf("hal_gpio_enable_irq key return failed.\n");
    }
    st7789_init();
    enable_camera_display(1);
    CameraDEMO_Init(pBuffer, sizeof(pBuffer));
    aos_post_delayed_action(500, app_delayed_action, NULL);
    aos_loop_run();

    return 0;
}

