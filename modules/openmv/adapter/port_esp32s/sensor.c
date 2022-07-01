/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Sensor abstraction layer for nRF port.
 */
#include <string.h>
#include <stdint.h>
#include <imlib.h>
#include "py/mphal.h"
#include "framebuffer.h"
#include "unaligned_memcpy.h"
#include "omv_boardconfig.h"
// #include "sccb.h"
char unique_id[OMV_UNIQUE_ID_SIZE * 4];

const int omv_resolution[][2] = {
    {0, 0},
    // C/SIF Resolutions
    {88, 72},   /* QQCIF     */
    {176, 144}, /* QCIF      */
    {352, 288}, /* CIF       */
    {88, 60},   /* QQSIF     */
    {176, 120}, /* QSIF      */
    {352, 240}, /* SIF       */
    // VGA Resolutions
    {40, 30},   /* QQQQVGA   */
    {80, 60},   /* QQQVGA    */
    {96, 96},   /* 96X96    */
    {160, 120}, /* QQVGA     */
    {320, 240}, /* QVGA      */
    {640, 480}, /* VGA       */
    {60, 40},   /* HQQQVGA   */
    {120, 80},  /* HQQVGA    */
    {240, 160}, /* HQVGA     */
    // FFT Resolutions
    {64, 32},   /* 64x32     */
    {64, 64},   /* 64x64     */
    {128, 64},  /* 128x64    */
    {128, 128}, /* 128x128   */
    // Other
    {128, 160},   /* LCD       */
    {128, 160},   /* QQVGA2    */
    {720, 480},   /* WVGA      */
    {752, 480},   /* WVGA2     */
    {800, 600},   /* SVGA      */
    {1024, 768},  /* XGA       */
    {1280, 1024}, /* SXGA      */
    {1600, 1200}, /* UXGA      */
    {1280, 720},  /* HD        */
    {1920, 1080}, /* FHD       */
    {2560, 1440}, /* QHD       */
    {2048, 1536}, /* QXGA      */
    {2560, 1600}, /* WQXGA     */
    {2592, 1944}, /* WQXGA2    */
};

#include "esp32_sensors.c"
omv_sensor_t sensor = {0};
static bool need_reinit = true;

int sensor_init()
{
    int init_ret = 0;

    need_reinit = false;
    /* Reset the sesnor state */
    memset(&sensor, 0, sizeof(omv_sensor_t));

    // initialize the camera
    if (esp32_sensor_init(&sensor) != 0)
    {
        sensor.detected = 0;
    }

    // Set default snapshot function.
    sensor.snapshot = sensor_snapshot;

    // Set default color palette.
    sensor.color_palette = rainbow_table;
    sensor.pwdn_pol = ACTIVE_HIGH;
    sensor.reset_pol = ACTIVE_LOW;
    sensor.detected = true;

    // Disable VSYNC IRQ and callback
    sensor_set_vsync_callback(NULL);

    /* All good! */
    return 0;
}

int sensor_reset()
{
    framebuffer_reset_buffers();

    // Reset the sensor state
    sensor.sde = 0;
    sensor.pixformat = 0;
    sensor.framesize = 0;
    sensor.framerate = 0;
    sensor.gainceiling = 0;
    sensor.hmirror = false;
    sensor.vflip = false;
    sensor.transpose = false;

    sensor.auto_rotation = false;
    sensor.vsync_callback = NULL;

    // Reset default color palette.
    sensor.color_palette = rainbow_table;

    // Restore shutdown state on reset.
    sensor_shutdown(false);
    if (sensor.reset_pol == ACTIVE_HIGH)
        CAM_RST_HIGH();
    else
        CAM_RST_LOW();
    vTaskDelay(100);
    if (sensor.reset_pol == ACTIVE_HIGH)
        CAM_RST_LOW();
    else
        CAM_RST_HIGH();
    // Call sensor-specific reset function
    if (sensor.reset(&sensor) != 0)
    {
        return -1;
    }
    camera_subpart_deinit();
    need_reinit = true;

    mp_hal_delay_ms(500);
    sensor_init();

    return 0;
}
#define SCCB_FREQ 100000           /*!< I2C master frequency*/
#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0          /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                /*!< I2C ack value */
#define NACK_VAL 0x1               /*!< I2C nack value */

#include "driver/i2c.h"
int sensor_get_id()
{

    sensor_t *esp_sensor = esp_camera_sensor_get();

    return esp_sensor->id.PID;
}

bool sensor_is_detected()
{

    sensor_t *esp_sensor = esp_camera_sensor_get();
    if (esp_sensor)
        return sensor.detected;
    else
        return 0;
}

int sensor_sleep(int enable)
{
    if (sensor.sleep == NULL || sensor.sleep(&sensor, enable) != 0)
    {
        // Operation not supported
        return -1;
    }
    return 0;
}

int sensor_shutdown(int enable)
{
    int ret = 0;
    if (enable)
    {
        if (sensor.pwdn_pol == ACTIVE_HIGH)
        {
            CAM_PWDN_HIGH();
        }
        else
        {
            CAM_PWDN_LOW();
        }
    }
    else
    {
        if (sensor.pwdn_pol == ACTIVE_HIGH)
        {
            CAM_PWDN_LOW();
        }
        else
        {
            CAM_PWDN_HIGH();
        }
    }

    mp_hal_delay_ms(10);
    return ret;
}

int sensor_read_reg(uint16_t reg_addr)
{
    sensor_t *esp_sensor = esp_camera_sensor_get();

    return esp_sensor->get_reg(esp_sensor, reg_addr, ~0U);
}

int sensor_write_reg(uint16_t reg_addr, uint16_t reg_data)
{
    sensor_t *esp_sensor = esp_camera_sensor_get();

    return esp_sensor->set_reg(esp_sensor, reg_addr, ~0U, reg_data);
}

int sensor_set_pixformat(omv_pixformat_t pixformat)
{
    if (sensor.pixformat == pixformat)
    {
        // No change
        return 0;
    }

    // sensor_check_buffsize() will switch from OMV_PIXFORMAT_BAYER to OMV_PIXFORMAT_RGB565 to try to fit
    // the MAIN_FB() in RAM as a first step optimization. If the user tries to switch back to RGB565
    // and that would be bigger than the RAM buffer we would just switch back.
    //
    // So, just short-circuit doing any work.
    //
    // This code is explicitly here to allow users to set the omv_resolution to RGB565 and have it
    // switch to BAYER only once even though they are setting the omv_resolution to RGB565 repeatedly
    // in a loop. Only RGB565->BAYER has this problem and needs this fix because of sensor_check_buffsize().
    uint32_t size = framebuffer_get_buffer_size();
    if ((sensor.pixformat == OMV_PIXFORMAT_BAYER) && (pixformat == OMV_PIXFORMAT_RGB565) && (MAIN_FB()->u * MAIN_FB()->v * 2 > size) && (MAIN_FB()->u * MAIN_FB()->v * 1 <= size))
    {
        // No change
        return 0;
    }

    // Cropping and transposing (and thus auto rotation) don't work in JPEG mode.
    // if ((pixformat == OMV_PIXFORMAT_JPEG) && (cropped() || sensor.transpose || sensor.auto_rotation)) {
    //    return -1;
    //}

    if (sensor.set_pixformat == NULL || sensor.set_pixformat(&sensor, pixformat) != 0)
    {
        // Operation not supported
        return -1;
    }

    mp_hal_delay_ms(100); // wait for the camera to settle

    // Set pixel format
    sensor.pixformat = pixformat;

    // Skip the first frame.
    MAIN_FB()->bpp = -1;

    return 0;
}

int sensor_set_framesize(omv_framesize_t framesize)
{
    if (sensor.framesize == framesize)
    {
        // No change
        return 0;
    }

    // Call the sensor specific function
    if (sensor.set_framesize == NULL || sensor.set_framesize(&sensor, framesize) != 0)
    {
        // Operation not supported
        return -1;
    }

    mp_hal_delay_ms(100); // wait for the camera to settle

    // Set framebuffer size
    sensor.framesize = framesize;

    // Skip the first frame.
    MAIN_FB()->bpp = -1;

    // Set MAIN FB x offset, y offset, width, height, backup width, and backup height.
    MAIN_FB()->x = 0;
    MAIN_FB()->y = 0;
    MAIN_FB()->w = MAIN_FB()->u = omv_resolution[framesize][0];
    MAIN_FB()->h = MAIN_FB()->v = omv_resolution[framesize][1];

    return 0;
}

int sensor_set_framerate(int framerate)
{
    if (sensor.framerate == framerate)
    {
        // No change
        return 0;
    }

    // Call the sensor specific function
    if (sensor.set_framerate == NULL || sensor.set_framerate(&sensor, framerate) != 0)
    {
        // Operation not supported
        return -1;
    }

    return 0;
}

int sensor_set_windowing(int x, int y, int w, int h)
{
    // py_sensor_set_windowing ensures this the window is at least 8x8
    // and that it is fully inside the sensor output framesize window.
    if (sensor.pixformat == OMV_PIXFORMAT_JPEG)
    {
        return -1;
    }

    // We force everything to be a multiple of 2 so that when you switch between
    // grayscale/rgb565/bayer/jpeg the frame doesn't need to move around for bayer to work.
    MAIN_FB()->x = (x / 2) * 2;
    MAIN_FB()->y = (y / 2) * 2;
    MAIN_FB()->w = MAIN_FB()->u = (w / 2) * 2;
    MAIN_FB()->h = MAIN_FB()->v = (h / 2) * 2;

    return 0;
}

int sensor_set_contrast(int level)
{
    if (sensor.set_contrast != NULL)
    {
        return sensor.set_contrast(&sensor, level);
    }
    return -1;
}

int sensor_set_brightness(int level)
{
    if (sensor.set_brightness != NULL)
    {
        return sensor.set_brightness(&sensor, level);
    }
    return -1;
}

int sensor_set_saturation(int level)
{
    if (sensor.set_saturation != NULL)
    {
        return sensor.set_saturation(&sensor, level);
    }
    return -1;
}

int sensor_set_gainceiling(gainceiling_t gainceiling)
{
    if (sensor.gainceiling == gainceiling)
    {
        /* no change */
        return 0;
    }

    /* call the sensor specific function */
    if (sensor.set_gainceiling == NULL || sensor.set_gainceiling(&sensor, gainceiling) != 0)
    {
        /* operation not supported */
        return -1;
    }

    sensor.gainceiling = gainceiling;
    return 0;
}

int sensor_set_quality(int qs)
{
    /* call the sensor specific function */
    if (sensor.set_quality == NULL || sensor.set_quality(&sensor, qs) != 0)
    {
        /* operation not supported */
        return -1;
    }
    return 0;
}

int sensor_set_colorbar(int enable)
{
    /* call the sensor specific function */
    if (sensor.set_colorbar == NULL || sensor.set_colorbar(&sensor, enable) != 0)
    {
        /* operation not supported */
        return -1;
    }
    return 0;
}

int sensor_set_auto_gain(int enable, float gain_db, float gain_db_ceiling)
{
    /* call the sensor specific function */
    if (sensor.set_auto_gain == NULL || sensor.set_auto_gain(&sensor, enable, gain_db, gain_db_ceiling) != 0)
    {
        /* operation not supported */
        return -1;
    }
    return 0;
}

int sensor_get_gain_db(float *gain_db)
{
    /* call the sensor specific function */
    if (sensor.get_gain_db == NULL || sensor.get_gain_db(&sensor, gain_db) != 0)
    {
        /* operation not supported */
        return -1;
    }
    return 0;
}

int sensor_set_auto_exposure(int enable, int exposure_us)
{
    /* call the sensor specific function */
    if (sensor.set_auto_exposure == NULL || sensor.set_auto_exposure(&sensor, enable, exposure_us) != 0)
    {
        /* operation not supported */
        return -1;
    }
    return 0;
}

int sensor_get_exposure_us(int *exposure_us)
{
    /* call the sensor specific function */
    if (sensor.get_exposure_us == NULL || sensor.get_exposure_us(&sensor, exposure_us) != 0)
    {
        /* operation not supported */
        return -1;
    }
    return 0;
}

int sensor_set_auto_whitebal(int enable, float r_gain_db, float g_gain_db, float b_gain_db)
{
    /* call the sensor specific function */

    if (sensor.set_auto_whitebal == NULL || sensor.set_auto_whitebal(&sensor, enable, r_gain_db, g_gain_db, b_gain_db) != 0)
    {
        /* operation not supported */
        return -1;
    }
    return 0;
}

int sensor_get_rgb_gain_db(float *r_gain_db, float *g_gain_db, float *b_gain_db)
{
    /* call the sensor specific function */
    if (sensor.get_rgb_gain_db == NULL || sensor.get_rgb_gain_db(&sensor, r_gain_db, g_gain_db, b_gain_db) != 0)
    {
        /* operation not supported */
        return -1;
    }
    return 0;
}

int sensor_set_hmirror(int enable)
{
    if (sensor.hmirror == ((bool)enable))
    {
        /* no change */
        return 0;
    }

    // /* call the sensor specific function */
    // if (sensor.set_hmirror == NULL || sensor.set_hmirror(&sensor, enable) != 0)
    // {
    //     /* operation not supported */
    //     return -1;
    // }
    sensor.hmirror = enable;
    sensor_t *esp_sensor = esp_camera_sensor_get();
    if (esp_sensor) // if get sensor succeed
    {
        esp_sensor->set_hmirror(esp_sensor, enable);
    }
    else
    {
        mp_hal_delay_ms(100); // wait for the camera to settle
        esp_sensor = esp_camera_sensor_get();
        if (esp_sensor) // if get sensor succeed
        {
            esp_sensor->set_hmirror(esp_sensor, enable);
        }
    }
    mp_hal_delay_ms(100); // wait for the camera to settle
    return 0;
}

bool sensor_get_hmirror()
{
    return sensor.hmirror;
}

int sensor_set_vflip(int enable)
{
    if (sensor.vflip == ((bool)enable))
    {
        /* no change */
        return 0;
    }

    /* call the sensor specific function */
    // if (sensor.set_vflip == NULL || sensor.set_vflip(&sensor, enable) != 0)
    // {
    //     /* operation not supported */
    //     return -1;
    // }
    sensor.vflip = enable;
    sensor_t *esp_sensor = esp_camera_sensor_get();
    if (esp_sensor) // if get sensor succeed
    {
        esp_sensor->set_vflip(esp_sensor, enable);
    }
    else
    {
        mp_hal_delay_ms(100); // wait for the camera to settle
        esp_sensor = esp_camera_sensor_get();
        if (esp_sensor) // if get sensor succeed
        {
            esp_sensor->set_vflip(esp_sensor, enable);
        }
    }

    mp_hal_delay_ms(100); // wait for the camera to settle
    return 0;
}

bool sensor_get_vflip()
{
    return sensor.vflip;
}

int sensor_set_transpose(bool enable)
{
    if (sensor.pixformat == OMV_PIXFORMAT_JPEG)
    {
        return -1;
    }

    sensor.transpose = enable;
    return 0;
}

bool sensor_get_transpose()
{
    return sensor.transpose;
}

int sensor_set_auto_rotation(bool enable)
{
    if (sensor.pixformat == OMV_PIXFORMAT_JPEG)
    {
        return -1;
    }

    sensor.auto_rotation = enable;
    return 0;
}

bool sensor_get_auto_rotation()
{
    return sensor.auto_rotation;
}

int sensor_set_framebuffers(int count)
{
    return framebuffer_set_buffers(count);
}

int sensor_set_special_effect(sde_t sde)
{
    if (sensor.sde == sde)
    {
        /* no change */
        return 0;
    }

    /* call the sensor specific function */
    if (sensor.set_special_effect == NULL || sensor.set_special_effect(&sensor, sde) != 0)
    {
        /* operation not supported */
        return -1;
    }

    sensor.sde = sde;
    return 0;
}

int sensor_set_lens_correction(int enable, int radi, int coef)
{
    /* call the sensor specific function */
    if (sensor.set_lens_correction == NULL || sensor.set_lens_correction(&sensor, enable, radi, coef) != 0)
    {
        /* operation not supported */
        return -1;
    }

    return 0;
}

int sensor_ioctl(int request, ... /* arg */)
{
    int ret = -1;

    if (sensor.ioctl != NULL)
    {
        va_list ap;
        va_start(ap, request);
        /* call the sensor specific function */
        ret = sensor.ioctl(&sensor, request, ap);
        va_end(ap);
    }

    return ret;
}

int sensor_set_vsync_callback(vsync_cb_t vsync_cb)
{
    sensor.vsync_callback = vsync_cb;
    if (sensor.vsync_callback == NULL)
    {
        // Disable VSYNC EXTI IRQ
    }
    else
    {
        // Enable VSYNC EXTI IRQ
    }
    return 0;
}

int sensor_set_color_palette(const uint16_t *color_palette)
{
    sensor.color_palette = color_palette;
    return 0;
}

const uint16_t *sensor_get_color_palette()
{
    return sensor.color_palette;
}

void VsyncExtiCallback()
{
    if (sensor.vsync_callback != NULL)
    {
        // sensor.vsync_callback(HAL_GPIO_ReadPin(CAM_VSYNC_PORT, CAM_VSYNC_PIN));
    }
}

// To make the user experience better we automatically shrink the size of the MAIN_FB() to fit
// within the RAM we have onboard the system.
void sensor_check_buffsize()
{
    if (MAIN_FB()->n_buffers != 1)
    {
        framebuffer_set_buffers(1);
    }

    uint32_t size = framebuffer_get_buffer_size();
    uint32_t bpp;

    switch (sensor.pixformat)
    {
    case OMV_PIXFORMAT_GRAYSCALE:
    case OMV_PIXFORMAT_BAYER:
        bpp = 1;
        break;
    case OMV_PIXFORMAT_RGB565:
    case OMV_PIXFORMAT_YUV422:
        bpp = 2;
        break;
    // If the pixformat is NULL/JPEG there we can't do anything to check if it fits before hand.
    default:
        return;
    }

    // MAIN_FB() fits, we are done.
    if ((MAIN_FB()->u * MAIN_FB()->v * bpp) <= size)
    {
        return;
    }

    if (sensor.pixformat == OMV_PIXFORMAT_RGB565)
    {
        // Switch to bayer for the quick 2x savings.
        sensor_set_pixformat(OMV_PIXFORMAT_BAYER);
        bpp = 1;

        // MAIN_FB() fits, we are done (bpp is 1).
        if (MAIN_FB()->u * MAIN_FB()->v <= size)
        {
            return;
        }
    }

    int window_w = MAIN_FB()->u;
    int window_h = MAIN_FB()->v;

    // We need to shrink the frame buffer. We can do this by cropping. So, we will subtract columns
    // and rows from the frame buffer until it fits within the frame buffer.
    int max = IM_MAX(window_w, window_h);
    int min = IM_MIN(window_w, window_h);
    float aspect_ratio = max / ((float)min);
    float r = aspect_ratio, best_r = r;
    int c = 1, best_c = c;
    float best_err = FLT_MAX;

    // Find the width/height ratio that's within 1% of the aspect ratio with a loop limit.
    for (int i = 100; i; i--)
    {
        float err = fast_fabsf(r - fast_roundf(r));

        if (err <= best_err)
        {
            best_err = err;
            best_r = r;
            best_c = c;
        }

        if (best_err <= 0.01f)
        {
            break;
        }

        r += aspect_ratio;
        c += 1;
    }

    // Select the larger geometry to map the aspect ratio to.
    int u_sub, v_sub;

    if (window_w > window_h)
    {
        u_sub = fast_roundf(best_r);
        v_sub = best_c;
    }
    else
    {
        u_sub = best_c;
        v_sub = fast_roundf(best_r);
    }

    // Crop the frame buffer while keeping the aspect ratio and keeping the width/height even.
    while (((MAIN_FB()->u * MAIN_FB()->v * bpp) > size) || (MAIN_FB()->u % 2) || (MAIN_FB()->v % 2))
    {
        MAIN_FB()->u -= u_sub;
        MAIN_FB()->v -= v_sub;
    }

    // Center the new window using the previous offset and keep the offset even.
    MAIN_FB()->x += (window_w - MAIN_FB()->u) / 2;
    MAIN_FB()->y += (window_h - MAIN_FB()->v) / 2;

    if (MAIN_FB()->x % 2)
    {
        MAIN_FB()->x -= 1;
    }

    if (MAIN_FB()->y % 2)
    {
        MAIN_FB()->y -= 1;
    }
}

// This is the default snapshot function, which can be replaced in sensor_init functions.
int sensor_snapshot(omv_sensor_t *sensor, image_t *image, uint32_t flags)
{
    // Compress the framebuffer for the IDE preview, only if it's not the first frame,
    // the framebuffer is enabled and the image sensor does not support JPEG encoding.
    // Note: This doesn't run unless the IDE is connected and the framebuffer is enabled.
    // extern bool is_dbg_mode_enabled(void);
    if (need_reinit)
    {
        camera_subpart_reinit(&camera_config);
        need_reinit = false;
    }
    // if (is_dbg_mode_enabled())
    //     framebuffer_update_jpeg_buffer();
    framebuffer_free_current_buffer();
    vbuffer_t *buffer = framebuffer_get_tail(FB_NO_FLAGS);

    if (!buffer)
    {
        ESP_LOGI(TAG, "no buffer get failed");
        return -1;
    }

    ESP_LOGD(TAG, "Taking picture...");
    camera_fb_t *pic = esp_camera_fb_get();
    if (pic == NULL) {
        ESP_LOGE(TAG, "can not get fb");
        return -1;
    }
    memcpy(buffer->data, pic->buf, pic->len);
    esp_camera_fb_return(pic);
    // use pic->buf to access the image
    // ESP_LOGI(TAG, "Picture taken! Its size was: %zu bytes jpeg size %d", pic->len, jpeg_framebuffer->size);

    // Fix the BPP.
    switch (sensor->pixformat)
    {
    case OMV_PIXFORMAT_GRAYSCALE:
        MAIN_FB()->bpp = 1;
        break;
    case OMV_PIXFORMAT_YUV422:
    case OMV_PIXFORMAT_RGB565:
    {
        MAIN_FB()->bpp = 2;
        /*
        for (int i = 0; i < pic->len; i += 2)
        {
            uint8_t temp = 0;
            temp = buffer->data[i];
            buffer->data[i] = buffer->data[i + 1];
            buffer->data[i + 1] = temp;
        }*/
        /*
            for(int i = 0; i < pic->len; i+=2){
                uint8_t temp = 0;
                temp = buffer->data[i];
                buffer->data[i] = buffer->data[i+1];
                buffer->data[i+1] = temp;
            }
            //*/
        break;
    }
    case OMV_PIXFORMAT_BAYER:
        MAIN_FB()->bpp = 3;
        break;
    case OMV_PIXFORMAT_JPEG:
        MAIN_FB()->bpp = pic->len;
        break;
    default:
        MAIN_FB()->bpp = -1;
        break;
    }

    // Set the user image.
    if (image != NULL)
    {
        image->w = MAIN_FB()->w;
        image->h = MAIN_FB()->h;
        image->bpp = MAIN_FB()->bpp;
        image->pixels = buffer->data;
        // ESP_LOGI(TAG, "w=%d, h=%d, bpp=%d", image->w, image->h, image->bpp);
    }

    return 0;
}

void sensor_set_ir_led(bool enable)
{
    // if(enable)
    //   CAM_IR_LED_HIGH();
    // else
    //   CAM_IR_LED_LOW();
    printf("%s IR LED by extgpio.\n", enable ? "Enabling" : "Disabling");
}