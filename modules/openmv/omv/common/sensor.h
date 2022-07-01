/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Sensor abstraction layer.
 */
#ifndef __SENSOR_H__
#define __SENSOR_H__
#include <stdarg.h>
#include "cambus.h"
#include "imlib.h"

#define OV2640_SLV_ADDR     (0x60)
#define OV5640_SLV_ADDR     (0x78)
#define OV7725_SLV_ADDR     (0x42)
#define MT9V034_SLV_ADDR    (0xB8)
#define MT9M114_SLV_ADDR    (0x90)
#define LEPTON_SLV_ADDR     (0x54)
#define HM01B0_SLV_ADDR     (0x48)

// Chip ID Registers
#define OV5640_CHIP_ID      (0x300A)
#define OV_CHIP_ID          (0x0A)
#define ON_CHIP_ID          (0x00)
#define HIMAX_CHIP_ID       (0x0001)

// Chip ID Values
#define OV2640_ID           (0x26)
#define OV5640_ID           (0x56)
#define OV7670_ID           (0x76)
#define OV7690_ID           (0x76)
#define OV7725_ID           (0x77)
#define OV9650_ID           (0x96)
#define MT9V034_ID          (0x13)
#define MT9M114_ID          (0x81)
#define LEPTON_ID           (0x54)
#define HM01B0_ID           (0xB0)

typedef enum {
    FRAMESIZE_INVALID = 0,
    // C/SIF Resolutions
    FRAMESIZE_QQCIF,    // 88x72
    FRAMESIZE_QCIF,     // 176x144
    FRAMESIZE_CIF,      // 352x288
    FRAMESIZE_QQSIF,    // 88x60
    FRAMESIZE_QSIF,     // 176x120
    FRAMESIZE_SIF,      // 352x240
    // VGA Resolutions
    FRAMESIZE_QQQQVGA,  // 40x30
    FRAMESIZE_QQQVGA,   // 80x60
    FRAMESIZE_QQVGA,    // 160x120
    FRAMESIZE_QVGA,     // 320x240
    FRAMESIZE_VGA,      // 640x480
    FRAMESIZE_HQQQVGA,  // 60x40
    FRAMESIZE_HQQVGA,   // 120x80
    FRAMESIZE_HQVGA,    // 240x160
    // FFT Resolutions
    FRAMESIZE_64X32,    // 64x32
    FRAMESIZE_64X64,    // 64x64
    FRAMESIZE_128X64,   // 128x64
    FRAMESIZE_128X128,  // 128x128
    FRAMESIZE_320X320,  // 320x320
    // Other
    FRAMESIZE_LCD,      // 128x160
    FRAMESIZE_QQVGA2,   // 128x160
    FRAMESIZE_WVGA,     // 720x480
    FRAMESIZE_WVGA2,    // 752x480
    FRAMESIZE_SVGA,     // 800x600
    FRAMESIZE_XGA,      // 1024x768
    FRAMESIZE_SXGA,     // 1280x1024
    FRAMESIZE_UXGA,     // 1600x1200
    FRAMESIZE_HD,       // 1280x720
    FRAMESIZE_FHD,      // 1920x1080
    FRAMESIZE_QHD,      // 2560x1440
    FRAMESIZE_QXGA,     // 2048x1536
    FRAMESIZE_WQXGA,    // 2560x1600
    FRAMESIZE_WQXGA2,   // 2592x1944
} framesize_t;

typedef enum {
    GAINCEILING_2X,
    GAINCEILING_4X,
    GAINCEILING_8X,
    GAINCEILING_16X,
    GAINCEILING_32X,
    GAINCEILING_64X,
    GAINCEILING_128X,
} gainceiling_t;

typedef enum {
    SDE_NORMAL,
    SDE_NEGATIVE,
} sde_t;

typedef enum {
    ATTR_CONTRAST=0,
    ATTR_BRIGHTNESS,
    ATTR_SATURATION,
    ATTR_GAINCEILING,
} sensor_attr_t;

typedef enum {
    ACTIVE_LOW,
    ACTIVE_HIGH
} polarity_t;

typedef enum {
    IOCTL_SET_READOUT_WINDOW,
    IOCTL_GET_READOUT_WINDOW,
    IOCTL_SET_TRIGGERED_MODE,
    IOCTL_GET_TRIGGERED_MODE,
    IOCTL_TRIGGER_AUTO_FOCUS,
    IOCTL_PAUSE_AUTO_FOCUS,
    IOCTL_RESET_AUTO_FOCUS,
    IOCTL_WAIT_ON_AUTO_FOCUS,
    IOCTL_LEPTON_GET_WIDTH,
    IOCTL_LEPTON_GET_HEIGHT,
    IOCTL_LEPTON_GET_RADIOMETRY,
    IOCTL_LEPTON_GET_REFRESH,
    IOCTL_LEPTON_GET_RESOLUTION,
    IOCTL_LEPTON_RUN_COMMAND,
    IOCTL_LEPTON_SET_ATTRIBUTE,
    IOCTL_LEPTON_GET_ATTRIBUTE,
    IOCTL_LEPTON_GET_FPA_TEMPERATURE,
    IOCTL_LEPTON_GET_AUX_TEMPERATURE,
    IOCTL_LEPTON_SET_MEASUREMENT_MODE,
    IOCTL_LEPTON_GET_MEASUREMENT_MODE,
    IOCTL_LEPTON_SET_MEASUREMENT_RANGE,
    IOCTL_LEPTON_GET_MEASUREMENT_RANGE,
    IOCTL_HIMAX_MD_ENABLE,
    IOCTL_HIMAX_MD_CLEAR,
    IOCTL_HIMAX_MD_WINDOW,
    IOCTL_HIMAX_MD_THRESHOLD,
    IOCTL_HIMAX_OSC_ENABLE,
} ioctl_t;

#define SENSOR_HW_FLAGS_VSYNC        (0) // vertical sync polarity.
#define SENSOR_HW_FLAGS_HSYNC        (1) // horizontal sync polarity.
#define SENSOR_HW_FLAGS_PIXCK        (2) // pixel clock edge.
#define SENSOR_HW_FLAGS_FSYNC        (3) // hardware frame sync.
#define SENSOR_HW_FLAGS_JPEGE        (4) // hardware JPEG encoder.
#define SWNSOR_HW_FLAGS_RGB565_REV   (5) // byte reverse rgb565.
#define SENSOR_HW_FLAGS_GET(s, x)    ((s)->hw_flags &  (1<<x))
#define SENSOR_HW_FLAGS_SET(s, x, v) ((s)->hw_flags |= (v<<x))
#define SENSOR_HW_FLAGS_CLR(s, x)    ((s)->hw_flags &= ~(1<<x))

typedef void (*vsync_cb_t)(uint32_t vsync);

typedef struct _sensor sensor_t;
typedef struct _sensor {
    union {
    uint8_t  chip_id;           // Sensor ID.
    uint16_t chip_id_w;         // Sensor ID 16 bits.
    };
    uint8_t  slv_addr;          // Sensor I2C slave address.
    uint16_t gs_bpp;            // Grayscale bytes per pixel.
    uint32_t hw_flags;          // Hardware flags (clock polarities/hw capabilities)
    const uint16_t *color_palette;    // Color palette used for color lookup.

    vsync_cb_t vsync_callback;  // VSYNC callback.
    polarity_t pwdn_pol;        // PWDN polarity (TODO move to hw_flags)
    polarity_t reset_pol;       // Reset polarity (TODO move to hw_flags)

    // Sensor state
    sde_t sde;                  // Special digital effects
    pixformat_t pixformat;      // Pixel format
    framesize_t framesize;      // Frame size
    int framerate;              // Frame rate
    gainceiling_t gainceiling;  // AGC gainceiling
    bool hmirror;               // Horizontal Mirror
    bool vflip;                 // Vertical Flip
    bool transpose;             // Transpose Image
    bool auto_rotation;         // Rotate Image Automatically
    bool detected;              // Set to true when the sensor is initialized.

    cambus_t bus;               // SCCB/I2C bus.

    // Sensor function pointers
    int  (*reset)               (sensor_t *sensor);
    int  (*sleep)               (sensor_t *sensor, int enable);
    int  (*read_reg)            (sensor_t *sensor, uint16_t reg_addr);
    int  (*write_reg)           (sensor_t *sensor, uint16_t reg_addr, uint16_t reg_data);
    int  (*set_pixformat)       (sensor_t *sensor, pixformat_t pixformat);
    int  (*set_framesize)       (sensor_t *sensor, framesize_t framesize);
    int  (*set_framerate)       (sensor_t *sensor, int framerate);
    int  (*set_contrast)        (sensor_t *sensor, int level);
    int  (*set_brightness)      (sensor_t *sensor, int level);
    int  (*set_saturation)      (sensor_t *sensor, int level);
    int  (*set_gainceiling)     (sensor_t *sensor, gainceiling_t gainceiling);
    int  (*set_quality)         (sensor_t *sensor, int quality);
    int  (*set_colorbar)        (sensor_t *sensor, int enable);
    int  (*set_auto_gain)       (sensor_t *sensor, int enable, float gain_db, float gain_db_ceiling);
    int  (*get_gain_db)         (sensor_t *sensor, float *gain_db);
    int  (*set_auto_exposure)   (sensor_t *sensor, int enable, int exposure_us);
    int  (*get_exposure_us)     (sensor_t *sensor, int *exposure_us);
    int  (*set_auto_whitebal)   (sensor_t *sensor, int enable, float r_gain_db, float g_gain_db, float b_gain_db);
    int  (*get_rgb_gain_db)     (sensor_t *sensor, float *r_gain_db, float *g_gain_db, float *b_gain_db);
    int  (*set_hmirror)         (sensor_t *sensor, int enable);
    int  (*set_vflip)           (sensor_t *sensor, int enable);
    int  (*set_special_effect)  (sensor_t *sensor, sde_t sde);
    int  (*set_lens_correction) (sensor_t *sensor, int enable, int radi, int coef);
    int  (*ioctl)               (sensor_t *sensor, int request, va_list ap);
    int  (*snapshot)            (sensor_t *sensor, image_t *image, uint32_t flags);
} sensor_t;

// Resolution table
extern const int resolution[][2];

// Initialize the sensor hardware and probe the image sensor.
int sensor_init();

// Initialize the sensor state.
void sensor_init0();

// Reset the sensor to its default state.
int sensor_reset();

// Return sensor PID.
int sensor_get_id();

// Return true if the sensor was detected and initialized.
bool sensor_is_detected();

// Sleep mode.
int sensor_sleep(int enable);

// Shutdown mode.
int sensor_shutdown(int enable);

// Read a sensor register.
int sensor_read_reg(uint16_t reg_addr);

// Write a sensor register.
int sensor_write_reg(uint16_t reg_addr, uint16_t reg_data);

// Set the sensor pixel format.
int sensor_set_pixformat(pixformat_t pixformat);

// Set the sensor frame size.
int sensor_set_framesize(framesize_t framesize);

// Set the sensor frame rate.
int sensor_set_framerate(int framerate);

// Set window size.
int sensor_set_windowing(int x, int y, int w, int h);

// Set the sensor contrast level (from -3 to +3).
int sensor_set_contrast(int level);

// Set the sensor brightness level (from -3 to +3).
int sensor_set_brightness(int level);

// Set the sensor saturation level (from -3 to +3).
int sensor_set_saturation(int level);

// Set the sensor AGC gain ceiling.
// Note: This function has no effect when AGC (Automatic Gain Control) is disabled.
int sensor_set_gainceiling(gainceiling_t gainceiling);

// Set the quantization scale factor, controls JPEG quality (quality 0-255).
int sensor_set_quality(int qs);

// Enable/disable the colorbar mode.
int sensor_set_colorbar(int enable);

// Enable auto gain or set value manually.
int sensor_set_auto_gain(int enable, float gain_db, float gain_db_ceiling);

// Get the gain value.
int sensor_get_gain_db(float *gain_db);

// Enable auto exposure or set value manually.
int sensor_set_auto_exposure(int enable, int exposure_us);

// Get the exposure value.
int sensor_get_exposure_us(int *get_exposure_us);

// Enable auto white balance or set value manually.
int sensor_set_auto_whitebal(int enable, float r_gain_db, float g_gain_db, float b_gain_db);

// Get the rgb gain values.
int sensor_get_rgb_gain_db(float *r_gain_db, float *g_gain_db, float *b_gain_db);

// Enable/disable the hmirror mode.
int sensor_set_hmirror(int enable);

// Get hmirror status.
bool sensor_get_hmirror();

// Enable/disable the vflip mode.
int sensor_set_vflip(int enable);

// Get vflip status.
bool sensor_get_vflip();

// Enable/disable the transpose mode.
int sensor_set_transpose(bool enable);

// Get transpose mode state.
bool sensor_get_transpose();

// Enable/disable the auto rotation mode.
int sensor_set_auto_rotation(bool enable);

// Get transpose mode state.
bool sensor_get_auto_rotation();

// Set the number of virtual frame buffers.
int sensor_set_framebuffers(int count);

// Set special digital effects (SDE).
int sensor_set_special_effect(sde_t sde);

// Set lens shading correction
int sensor_set_lens_correction(int enable, int radi, int coef);

// IOCTL function
int sensor_ioctl(int request, ...);

// Set vsync callback function.
int sensor_set_vsync_callback(vsync_cb_t vsync_cb);

// Set color palette
int sensor_set_color_palette(const uint16_t *color_palette);

// Get color palette
const uint16_t *sensor_get_color_palette();

// Default snapshot function.
int sensor_snapshot(sensor_t *sensor, image_t *image, uint32_t flags);

#endif /* __SENSOR_H__ */
