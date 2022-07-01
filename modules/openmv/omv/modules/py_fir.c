/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * FIR Python module.
 */
#include "py/runtime.h"
#include "py/objlist.h"

#include "omv_boardconfig.h"
#include "cambus.h"
#if (OMV_ENABLE_FIR_MLX90621 == 1)
#include "MLX90621_API.h"
#include "MLX90621_I2C_Driver.h"
#endif
#if (OMV_ENABLE_FIR_MLX90640 == 1)
#include "MLX90640_API.h"
#include "MLX90640_I2C_Driver.h"
#endif
#if (OMV_ENABLE_FIR_MLX90641 == 1)
#include "MLX90641_API.h"
#include "MLX90641_I2C_Driver.h"
#endif
#include "framebuffer.h"

#include "py_assert.h"
#include "py_helper.h"
#include "py_image.h"

#if (OMV_ENABLE_FIR_LEPTON == 1)
#include "py_fir_lepton.h"
#endif

#define MLX90621_ADDR                   0x50
#define MLX90621_WIDTH                  16
#define MLX90621_HEIGHT                 4
#define MLX90621_EEPROM_DATA_SIZE       256
#define MLX90621_FRAME_DATA_SIZE        66

#define MLX90640_ADDR                   0x33
#define MLX90640_WIDTH                  32
#define MLX90640_HEIGHT                 24
#define MLX90640_EEPROM_DATA_SIZE       832
#define MLX90640_FRAME_DATA_SIZE        834

#define MLX90641_ADDR                   0x33
#define MLX90641_WIDTH                  16
#define MLX90641_HEIGHT                 12
#define MLX90641_EEPROM_DATA_SIZE       832
#define MLX90641_FRAME_DATA_SIZE        242

#define AMG8833_ADDR                    0xD2
#define AMG8833_WIDTH                   8
#define AMG8833_HEIGHT                  8
#define AMG8833_RESET_REGISTER          0x01
#define AMG8833_THERMISTOR_REGISTER     0x0E
#define AMG8833_TEMPERATURE_REGISTER    0x80
#define AMG8833_INITIAL_RESET_VALUE     0x3F

#define LEPTON_ADDR                     0x54

#define AMG8833_12_TO_16(value) \
({ \
    __typeof__ (value) __value = (value); \
    if ((__value >> 11) & 1) { \
        __value |= 1 << 15; \
    } \
    __value & 0x87FF; \
})

static cambus_t fir_bus = {};
#if ((OMV_ENABLE_FIR_MLX90621 == 1) || (OMV_ENABLE_FIR_MLX90640 == 1) || (OMV_ENABLE_FIR_MLX90641 == 1))
static void *fir_mlx_data = NULL;
#endif

static enum {
    FIR_NONE,
#if (OMV_ENABLE_FIR_MLX90621 == 1)
    FIR_MLX90621,
#endif
#if (OMV_ENABLE_FIR_MLX90640 == 1)
    FIR_MLX90640,
#endif
#if (OMV_ENABLE_FIR_MLX90641 == 1)
    FIR_MLX90641,
#endif
#if (OMV_ENABLE_FIR_AMG8833 == 1)
    FIR_AMG8833,
#endif
#if (OMV_ENABLE_FIR_LEPTON == 1)
    FIR_LEPTON
#endif
} fir_sensor = FIR_NONE;

static int fir_width = 0;
static int fir_height = 0;
static int fir_ir_fresh_rate = 0;
static int fir_adc_resolution = 0;
static bool fir_transposed = false;

// img->w == data_w && img->h == data_h && img->bpp == IMAGE_BPP_GRAYSCALE
static void fir_fill_image_float_obj(image_t *img, mp_obj_t *data, float min, float max)
{
    float tmp = min;
    min = (min < max) ? min : max;
    max = (max > tmp) ? max : tmp;

    float diff = 255.f / (max - min);

    for (int y = 0; y < img->h; y++) {
        int row_offset = y * img->w;
        mp_obj_t *raw_row = data + row_offset;
        uint8_t *row_pointer = ((uint8_t *) img->data) + row_offset;

        for (int x = 0; x < img->w; x++) {
            float raw = mp_obj_get_float(raw_row[x]);

            if (raw < min) {
                raw = min;
            }

            if (raw > max) {
                raw = max;
            }

            int pixel = fast_roundf((raw - min) * diff);
            row_pointer[x] = __USAT(pixel, 8);
        }
    }
}

#if (OMV_ENABLE_FIR_MLX90621 == 1)
static void fir_MLX90621_get_frame(float *Ta, float *To)
{
    uint16_t *data = fb_alloc(MLX90621_FRAME_DATA_SIZE * sizeof(uint16_t), FB_ALLOC_NO_HINT);

    PY_ASSERT_TRUE_MSG(MLX90621_GetFrameData(data) >= 0,
                       "Failed to read the MLX90621 sensor data!");
    *Ta = MLX90621_GetTa(data, fir_mlx_data);
    MLX90621_CalculateTo(data, fir_mlx_data, 0.95f, *Ta - 8, To);

    fb_free();
}
#endif

#if (OMV_ENABLE_FIR_MLX90640 == 1)
static void fir_MLX90640_get_frame(float *Ta, float *To)
{
    uint16_t *data = fb_alloc(MLX90640_FRAME_DATA_SIZE * sizeof(uint16_t), FB_ALLOC_NO_HINT);

    // Calculate 1st sub-frame...
    PY_ASSERT_TRUE_MSG(MLX90640_GetFrameData(MLX90640_ADDR, data) >= 0,
                       "Failed to read the MLX90640 sensor data!");
    *Ta = MLX90640_GetTa(data, fir_mlx_data);
    MLX90640_CalculateTo(data, fir_mlx_data, 0.95f, *Ta - 8, To);

    // Calculate 2nd sub-frame...
    PY_ASSERT_TRUE_MSG(MLX90640_GetFrameData(MLX90640_ADDR, data) >= 0,
                       "Failed to read the MLX90640 sensor data!");
    *Ta = MLX90640_GetTa(data, fir_mlx_data);
    MLX90640_CalculateTo(data, fir_mlx_data, 0.95f, *Ta - 8, To);

    fb_free();
}
#endif

#if (OMV_ENABLE_FIR_MLX90641 == 1)
static void fir_MLX90641_get_frame(float *Ta, float *To)
{
    uint16_t *data = fb_alloc(MLX90641_FRAME_DATA_SIZE * sizeof(uint16_t), FB_ALLOC_NO_HINT);

    PY_ASSERT_TRUE_MSG(MLX90641_GetFrameData(MLX90641_ADDR, data) >= 0,
                       "Failed to read the MLX90641 sensor data!");
    *Ta = MLX90641_GetTa(data, fir_mlx_data);
    MLX90641_CalculateTo(data, fir_mlx_data, 0.95f, *Ta - 8, To);

    fb_free();
}
#endif

#if (OMV_ENABLE_FIR_AMG8833 == 1)
static void fir_AMG8833_get_frame(float *Ta, float *To)
{
    int16_t temp;
    int error = 0;
    error |= cambus_write_bytes(&fir_bus, AMG8833_ADDR, (uint8_t [1]){AMG8833_THERMISTOR_REGISTER}, 1, CAMBUS_XFER_NO_STOP);
    error |= cambus_read_bytes(&fir_bus, AMG8833_ADDR, (uint8_t *) &temp, sizeof(temp), CAMBUS_XFER_NO_FLAGS);
    PY_ASSERT_TRUE_MSG((error == 0), "Failed to read the AMG8833 sensor data!");

    *Ta = AMG8833_12_TO_16(temp) * 0.0625f;

    int16_t *data = fb_alloc(AMG8833_WIDTH * AMG8833_HEIGHT * sizeof(int16_t), FB_ALLOC_NO_HINT);
    error |= cambus_write_bytes(&fir_bus, AMG8833_ADDR, (uint8_t [1]){AMG8833_TEMPERATURE_REGISTER}, 1, CAMBUS_XFER_NO_STOP);
    error |= cambus_read_bytes(&fir_bus, AMG8833_ADDR, (uint8_t *) data, AMG8833_WIDTH * AMG8833_HEIGHT * 2, CAMBUS_XFER_NO_FLAGS);
    PY_ASSERT_TRUE_MSG((error == 0), "Failed to read the AMG8833 sensor data!");

    for (int i = 0, ii = AMG8833_WIDTH * AMG8833_HEIGHT; i < ii; i++) {
        To[i] = AMG8833_12_TO_16(data[i]) * 0.25f;
    }

    fb_free();
}
#endif

static mp_obj_t fir_get_ir(int w, int h, float Ta, float *To, bool mirror, bool flip, bool dst_transpose, bool src_transpose)
{
    mp_obj_list_t *list = (mp_obj_list_t *) mp_obj_new_list(w * h, NULL);
    float min = FLT_MAX;
    float max = -FLT_MAX;
    int w_1 = w - 1;
    int h_1 = h - 1;

    if (!src_transpose) {
        for (int y = 0; y < h; y++) {
            int y_dst = flip ? (h_1 - y) : y;
            float *raw_row = To + (y * w);
            mp_obj_t *list_row = list->items + (y_dst * w);
            mp_obj_t *t_list_row = list->items + y_dst;

            for (int x = 0; x < w; x++) {
                int x_dst = mirror ? (w_1 - x) : x;
                float raw = raw_row[x];

                if (raw < min) {
                    min = raw;
                }

                if (raw > max) {
                    max = raw;
                }

                mp_obj_t f = mp_obj_new_float(raw);

                if (!dst_transpose) {
                    list_row[x_dst] = f;
                } else {
                    t_list_row[x_dst * h] = f;
                }
            }
        }
    } else {
        for (int x = 0; x < w; x++) {
            int x_dst = mirror ? (w_1 - x) : x;
            float *raw_row = To + (x * h);
            mp_obj_t *t_list_row = list->items + (x_dst * h);
            mp_obj_t *list_row = list->items + x_dst;

            for (int y = 0; y < h; y++) {
                int y_dst = flip ? (h_1 - y) : y;
                float raw = raw_row[y];

                if (raw < min) {
                    min = raw;
                }

                if (raw > max) {
                    max = raw;
                }

                mp_obj_t f = mp_obj_new_float(raw);

                if (!dst_transpose) {
                    list_row[y_dst * w] = f;
                } else {
                    t_list_row[y_dst] = f;
                }
            }
        }
    }

    mp_obj_t tuple[4];
    tuple[0] = mp_obj_new_float(Ta);
    tuple[1] = MP_OBJ_FROM_PTR(list);
    tuple[2] = mp_obj_new_float(min);
    tuple[3] = mp_obj_new_float(max);
    return mp_obj_new_tuple(4, tuple);
}

static mp_obj_t py_fir_deinit()
{
    #if (OMV_ENABLE_FIR_LEPTON == 1)
    if (fir_sensor == FIR_LEPTON) {
        fir_lepton_deinit();
    }
    #endif

    if (fir_sensor != FIR_NONE) {
        cambus_deinit(&fir_bus);
        fir_sensor = FIR_NONE;
    }

    #if ((OMV_ENABLE_FIR_MLX90621 == 1) || (OMV_ENABLE_FIR_MLX90640 == 1) || (OMV_ENABLE_FIR_MLX90641 == 1))
    if (fir_mlx_data != NULL) {
        xfree(fir_mlx_data);
        fir_mlx_data = NULL;
    }
    #endif

    fir_width = 0;
    fir_height = 0;
    fir_ir_fresh_rate = 0;
    fir_adc_resolution = 0;
    fir_transposed = false;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_fir_deinit_obj, py_fir_deinit);

mp_obj_t py_fir_init(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    py_fir_deinit();
    bool first_init = true;
    int type = py_helper_keyword_int(n_args, args, 0, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_type), -1);
    if (type == -1) {
        FIR_SCAN_RETRY:
        cambus_init(&fir_bus, FIR_I2C_ID, CAMBUS_SPEED_STANDARD);
        switch (cambus_scan(&fir_bus)) {
            #if (OMV_ENABLE_FIR_MLX90621 == 1)
                case (MLX90621_ADDR << 1): {
                    type = FIR_MLX90621;
                    break;
                }
            #endif
            #if (OMV_ENABLE_FIR_MLX90640 == 1)
                case (MLX90640_ADDR << 1): {
                    type = FIR_MLX90640;
                    break;
                }
            #endif
            #if (OMV_ENABLE_FIR_MLX90640 == 0) && (OMV_ENABLE_FIR_MLX90641 == 1)
                case (MLX90641_ADDR << 1): {
                    type = FIR_MLX90641;
                    break;
                }
            #endif
            #if (OMV_ENABLE_FIR_AMG8833 == 1)
                case AMG8833_ADDR: {
                    type = FIR_AMG8833;
                    break;
                }
            #endif
            #if (OMV_ENABLE_FIR_LEPTON == 1)
                case LEPTON_ADDR: {
                    type = FIR_LEPTON;
                    break;
                }
            #endif
            default: {
                if (first_init) {
                    first_init = false;
                    // Try to recover the bus once as it may be stuck from an interrupted run.
                    // This is mostly needed for the AMG8833
                    cambus_pulse_scl(&fir_bus);
                    goto FIR_SCAN_RETRY;
                } else {
                    mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Failed to detect any FIR sensor"));
                }
            }
        }
        cambus_deinit(&fir_bus);
    }

    // Initialize the detected sensor.
    first_init = true;
    switch (type) {
        case FIR_NONE: {
            return mp_const_none;
        }
        #if (OMV_ENABLE_FIR_MLX90621 == 1)
        case FIR_MLX90621: {
            // parse refresh rate and ADC resolution
            int ir_fresh_rate = py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_refresh), 64);
            int adc_resolution = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_resolution), 18);

            // sanitize values
            ir_fresh_rate = 14 - __CLZ(__RBIT((ir_fresh_rate > 512) ? 512 : ((ir_fresh_rate < 1) ? 1 : ir_fresh_rate)));
            adc_resolution = ((adc_resolution > 18) ? 18 : ((adc_resolution < 15) ? 15 : adc_resolution)) - 15;

            fir_mlx_data = xalloc(sizeof(paramsMLX90621));

            fir_sensor = FIR_MLX90621;
            FIR_MLX90621_RETRY:
            cambus_init(&fir_bus, FIR_I2C_ID, CAMBUS_SPEED_FULL); // The EEPROM must be read at <= 400KHz.
            MLX90621_I2CInit(&fir_bus);

            fb_alloc_mark();
            uint8_t *eeprom = fb_alloc(MLX90621_EEPROM_DATA_SIZE * sizeof(uint8_t), FB_ALLOC_NO_HINT);
            int error = MLX90621_DumpEE(eeprom);
            error |= MLX90621_Configure(eeprom);
            error |= MLX90621_SetRefreshRate(ir_fresh_rate);
            error |= MLX90621_SetResolution(adc_resolution);
            error |= MLX90621_ExtractParameters(eeprom, fir_mlx_data);
            fb_alloc_free_till_mark();

            if (error != 0) {
                if (first_init) {
                    first_init = false;
                    cambus_pulse_scl(&fir_bus);
                    goto FIR_MLX90621_RETRY;
                } else {
                    py_fir_deinit();
                    mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Failed to init the MLX90621!"));
                }
            }

            // Switch to FAST speed
            cambus_deinit(&fir_bus);
            cambus_init(&fir_bus, FIR_I2C_ID, CAMBUS_SPEED_FAST);
            fir_width = MLX90621_WIDTH;
            fir_height = MLX90621_HEIGHT;
            fir_ir_fresh_rate = ir_fresh_rate;
            fir_adc_resolution = adc_resolution;
            return mp_const_none;
        }
        #endif
        #if (OMV_ENABLE_FIR_MLX90640 == 1)
        case FIR_MLX90640: {
            // parse refresh rate and ADC resolution
            int ir_fresh_rate = py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_refresh), 32);
            int adc_resolution = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_resolution), 19);

            // sanitize values
            ir_fresh_rate = __CLZ(__RBIT((ir_fresh_rate > 64) ? 64 : ((ir_fresh_rate < 1) ? 1 : ir_fresh_rate))) + 1;
            adc_resolution = ((adc_resolution > 19) ? 19 : ((adc_resolution < 16) ? 16 : adc_resolution)) - 16;

            fir_mlx_data = xalloc(sizeof(paramsMLX90640));

            fir_sensor = FIR_MLX90640;
            FIR_MLX90640_RETRY:
            cambus_init(&fir_bus, FIR_I2C_ID, CAMBUS_SPEED_FULL); // The EEPROM must be read at <= 400KHz.
            MLX90640_I2CInit(&fir_bus);

            fb_alloc_mark();
            uint16_t *eeprom = fb_alloc(MLX90640_EEPROM_DATA_SIZE * sizeof(uint16_t), FB_ALLOC_NO_HINT);
            int error = MLX90640_DumpEE(MLX90640_ADDR, eeprom);
            error |= MLX90640_SetRefreshRate(MLX90640_ADDR, ir_fresh_rate);
            error |= MLX90640_SetResolution(MLX90640_ADDR, adc_resolution);
            error |= MLX90640_ExtractParameters(eeprom, fir_mlx_data);
            fb_alloc_free_till_mark();

            if (error != 0) {
                if (first_init) {
                    first_init = false;
                    cambus_pulse_scl(&fir_bus);
                    goto FIR_MLX90640_RETRY;
                } else {
                    py_fir_deinit();
                    mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Failed to init the MLX90640!"));
                }
            }

            // Switch to FAST speed
            cambus_deinit(&fir_bus);
            cambus_init(&fir_bus, FIR_I2C_ID, CAMBUS_SPEED_FAST);
            fir_width = MLX90640_WIDTH;
            fir_height = MLX90640_HEIGHT;
            fir_ir_fresh_rate = ir_fresh_rate;
            fir_adc_resolution = adc_resolution;
            return mp_const_none;
        }
        #endif
        #if (OMV_ENABLE_FIR_MLX90641 == 1)
        case FIR_MLX90641: {
            // parse refresh rate and ADC resolution
            int ir_fresh_rate = py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_refresh), 32);
            int adc_resolution = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_resolution), 19);

            // sanitize values
            ir_fresh_rate = __CLZ(__RBIT((ir_fresh_rate > 64) ? 64 : ((ir_fresh_rate < 1) ? 1 : ir_fresh_rate))) + 1;
            adc_resolution = ((adc_resolution > 19) ? 19 : ((adc_resolution < 16) ? 16 : adc_resolution)) - 16;

            fir_mlx_data = xalloc(sizeof(paramsMLX90641));

            fir_sensor = FIR_MLX90641;
            FIR_MLX90641_RETRY:
            cambus_init(&fir_bus, FIR_I2C_ID, CAMBUS_SPEED_FULL); // The EEPROM must be read at <= 400KHz.
            MLX90641_I2CInit(&fir_bus);

            fb_alloc_mark();
            uint16_t *eeprom = fb_alloc(MLX90641_EEPROM_DATA_SIZE * sizeof(uint16_t), FB_ALLOC_NO_HINT);
            int error = MLX90641_DumpEE(MLX90641_ADDR, eeprom);
            error |= MLX90641_SetRefreshRate(MLX90641_ADDR, ir_fresh_rate);
            error |= MLX90641_SetResolution(MLX90641_ADDR, adc_resolution);
            error |= MLX90641_ExtractParameters(eeprom, fir_mlx_data);
            fb_alloc_free_till_mark();

            if (error != 0) {
                if (first_init) {
                    first_init = false;
                    cambus_pulse_scl(&fir_bus);
                    goto FIR_MLX90641_RETRY;
                } else {
                    py_fir_deinit();
                    mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Failed to init the MLX90641!"));
                }
            }

            // Switch to FAST speed
            cambus_deinit(&fir_bus);
            cambus_init(&fir_bus, FIR_I2C_ID, CAMBUS_SPEED_FAST);
            fir_width = MLX90641_WIDTH;
            fir_height = MLX90641_HEIGHT;
            fir_ir_fresh_rate = ir_fresh_rate;
            fir_adc_resolution = adc_resolution;
            return mp_const_none;
        }
        #endif
        #if (OMV_ENABLE_FIR_AMG8833 == 1)
        case FIR_AMG8833: {
            fir_sensor = FIR_AMG8833;
            FIR_AMG8833_RETRY:
            cambus_init(&fir_bus, FIR_I2C_ID, CAMBUS_SPEED_STANDARD);

            int error = 0;
            error |= cambus_write_bytes(&fir_bus, AMG8833_ADDR, (uint8_t [1]){AMG8833_RESET_REGISTER}, 1, CAMBUS_XFER_SUSPEND);
            error |= cambus_write_bytes(&fir_bus, AMG8833_ADDR, (uint8_t [1]){AMG8833_INITIAL_RESET_VALUE}, 1, CAMBUS_XFER_NO_FLAGS);
            if (error != 0) {
                if (first_init) {
                    first_init = false;
                    cambus_pulse_scl(&fir_bus);
                    goto FIR_AMG8833_RETRY;
                } else {
                    py_fir_deinit();
                    mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Failed to init the AMG8833!"));
                }
            }

            fir_width = AMG8833_WIDTH;
            fir_height = AMG8833_HEIGHT;
            fir_ir_fresh_rate = 10;
            fir_adc_resolution = 12;
            return mp_const_none;
        }
        #endif
        #if (OMV_ENABLE_FIR_LEPTON == 1)
        case FIR_LEPTON: {
            fir_sensor = FIR_LEPTON;
            FIR_LEPTON_RETRY:
            cambus_init(&fir_bus, OMV_FIR_LEPTON_I2C_BUS, OMV_FIR_LEPTON_I2C_BUS_SPEED);

            int error = fir_lepton_init(&fir_bus, &fir_width, &fir_height, &fir_ir_fresh_rate, &fir_adc_resolution);

            if (error != 0) {
                if (first_init) {
                    first_init = false;
                    cambus_pulse_scl(&fir_bus);
                    goto FIR_LEPTON_RETRY;
                } else {
                    py_fir_deinit();
                    mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Failed to init the Lepton!"));
                }
            }

            return mp_const_none;
        }
        #endif
        default: {
            mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Invalid sensor type!"));
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_fir_init_obj, 0, py_fir_init);

static mp_obj_t py_fir_type()
{
    if (fir_sensor == FIR_NONE) return mp_const_none;
    return mp_obj_new_int(fir_sensor);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_fir_type_obj, py_fir_type);

static mp_obj_t py_fir_width()
{
    if (fir_sensor == FIR_NONE) return mp_const_none;
    return mp_obj_new_int(fir_width);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_fir_width_obj, py_fir_width);

static mp_obj_t py_fir_height()
{
    if (fir_sensor == FIR_NONE) return mp_const_none;
    return mp_obj_new_int(fir_height);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_fir_height_obj, py_fir_height);

static mp_obj_t py_fir_refresh()
{
    #if (OMV_ENABLE_FIR_MLX90621 == 1)
    const int mlx_90621_refresh_rates[16] = {512, 512, 512, 512, 512, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1, 0};
    #endif
    #if (OMV_ENABLE_FIR_MLX90640 == 1) || (OMV_ENABLE_FIR_MLX90641 == 1)
    const int mlx_90640_1_refresh_rates[8] = {0, 1, 2, 4, 8, 16, 32, 64};
    #endif
    switch (fir_sensor) {
        case FIR_NONE:
            return mp_const_none;
        #if (OMV_ENABLE_FIR_MLX90621 == 1)
        case FIR_MLX90621:
            return mp_obj_new_int(mlx_90621_refresh_rates[fir_ir_fresh_rate]);
        #endif
        #if (OMV_ENABLE_FIR_MLX90640 == 1) || (OMV_ENABLE_FIR_MLX90640 == 1)
        #if (OMV_ENABLE_FIR_MLX90640 == 1)
        case FIR_MLX90640:
        #endif
        #if (OMV_ENABLE_FIR_MLX90641 == 1)
        case FIR_MLX90641:
        #endif
            return mp_obj_new_int(mlx_90640_1_refresh_rates[fir_ir_fresh_rate]);
        #endif
        #if (OMV_ENABLE_FIR_AMG8833 == 1)
        case FIR_AMG8833:
            return mp_obj_new_int(fir_ir_fresh_rate);
        #endif
        #if (OMV_ENABLE_FIR_LEPTON == 1)
        case FIR_LEPTON:
            return mp_obj_new_int(fir_ir_fresh_rate);
        #endif
        default:
            return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_fir_refresh_obj, py_fir_refresh);

static mp_obj_t py_fir_resolution()
{
    switch (fir_sensor) {
        case FIR_NONE:
            return mp_const_none;
        #if (OMV_ENABLE_FIR_MLX90621 == 1)
        case FIR_MLX90621:
            return mp_obj_new_int(fir_adc_resolution + 15);
        #endif
        #if (OMV_ENABLE_FIR_MLX90640 == 1) || (OMV_ENABLE_FIR_MLX90640 == 1)
        #if (OMV_ENABLE_FIR_MLX90640 == 1)
        case FIR_MLX90640:
        #endif
        #if (OMV_ENABLE_FIR_MLX90641 == 1)
        case FIR_MLX90641:
        #endif
            return mp_obj_new_int(fir_adc_resolution + 16);
        #endif
        #if (OMV_ENABLE_FIR_AMG8833 == 1)
        case FIR_AMG8833:
            return mp_obj_new_int(fir_adc_resolution);
        #endif
        #if (OMV_ENABLE_FIR_LEPTON == 1)
        case FIR_LEPTON:
            return mp_obj_new_int(fir_adc_resolution);
        #endif
        default:
            return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_fir_resolution_obj, py_fir_resolution);

#if (OMV_ENABLE_FIR_LEPTON == 1)
static mp_obj_t py_fir_radiometric()
{
    if (fir_sensor == FIR_LEPTON) {
        return fir_lepton_get_radiometry();
    }

    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_fir_radiometric_obj, py_fir_radiometric);

#if defined(OMV_FIR_LEPTON_VSYNC_PRESENT)
static mp_obj_t py_fir_register_vsync_cb(mp_obj_t cb)
{
    if (fir_sensor == FIR_LEPTON) {
        fir_lepton_register_vsync_cb(cb);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_fir_register_vsync_cb_obj, py_fir_register_vsync_cb);
#endif

static mp_obj_t py_fir_register_frame_cb(mp_obj_t cb)
{
    if (fir_sensor == FIR_LEPTON) {
        fir_lepton_register_frame_cb(cb);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_fir_register_frame_cb_obj, py_fir_register_frame_cb);

static mp_obj_t py_fir_get_frame_available()
{
    if (fir_sensor == FIR_LEPTON) {
        return fir_lepton_get_frame_available();
    }

    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_fir_get_frame_available_obj, py_fir_get_frame_available);

static mp_obj_t py_fir_trigger_ffc(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    if (fir_sensor == FIR_LEPTON) {
        fir_lepton_trigger_ffc(n_args, args, kw_args);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_fir_trigger_ffc_obj, 0, py_fir_trigger_ffc);
#endif

mp_obj_t py_fir_read_ta()
{
    switch(fir_sensor) {
        case FIR_NONE: {
            return mp_const_none;
        }
        #if (OMV_ENABLE_FIR_MLX90621 == 1)
        case FIR_MLX90621: {
            fb_alloc_mark();
            uint16_t *data = fb_alloc(MLX90621_FRAME_DATA_SIZE * sizeof(uint16_t), FB_ALLOC_NO_HINT);
            PY_ASSERT_TRUE_MSG(MLX90621_GetFrameData(data) >= 0,
                               "Failed to read the MLX90640 sensor data!");
            mp_obj_t result = mp_obj_new_float(MLX90621_GetTa(data, fir_mlx_data));
            fb_alloc_free_till_mark();
            return result;
        }
        #endif
        #if (OMV_ENABLE_FIR_MLX90640 == 1)
        case FIR_MLX90640: {
            fb_alloc_mark();
            uint16_t *data = fb_alloc(MLX90640_FRAME_DATA_SIZE * sizeof(uint16_t), FB_ALLOC_NO_HINT);
            PY_ASSERT_TRUE_MSG(MLX90640_GetFrameData(MLX90640_ADDR, data) >= 0,
                               "Failed to read the MLX90640 sensor data!");
            mp_obj_t result = mp_obj_new_float(MLX90640_GetTa(data, fir_mlx_data));
            fb_alloc_free_till_mark();
            return result;
        }
        #endif
        #if (OMV_ENABLE_FIR_MLX90641 == 1)
        case FIR_MLX90641: {
            fb_alloc_mark();
            uint16_t *data = fb_alloc(MLX90641_FRAME_DATA_SIZE * sizeof(uint16_t), FB_ALLOC_NO_HINT);
            PY_ASSERT_TRUE_MSG(MLX90641_GetFrameData(MLX90641_ADDR, data) >= 0,
                               "Failed to read the MLX90641 sensor data!");
            mp_obj_t result = mp_obj_new_float(MLX90641_GetTa(data, fir_mlx_data));
            fb_alloc_free_till_mark();
            return result;
        }
        #endif
        #if (OMV_ENABLE_FIR_AMG8833 == 1)
        case FIR_AMG8833: {
            int16_t temp;
            int error = 0;
            error |= cambus_write_bytes(&fir_bus, AMG8833_ADDR, (uint8_t [1]){AMG8833_THERMISTOR_REGISTER}, 1, CAMBUS_XFER_NO_STOP);
            error |= cambus_read_bytes(&fir_bus, AMG8833_ADDR, (uint8_t *) &temp, sizeof(temp), CAMBUS_XFER_NO_FLAGS);
            PY_ASSERT_TRUE_MSG((error == 0), "Failed to read the AMG8833 sensor data!");
            return mp_obj_new_float(AMG8833_12_TO_16(temp) * 0.0625f);
        }
        #endif
        #if (OMV_ENABLE_FIR_LEPTON == 1)
        case FIR_LEPTON: {
            return fir_lepton_read_ta();
        }
        #endif
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_fir_read_ta_obj, py_fir_read_ta);

mp_obj_t py_fir_read_ir(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    bool arg_hmirror = py_helper_keyword_int(n_args, args, 0, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_hmirror), false);
    bool arg_vflip = py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_vflip), false);
    fir_transposed = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_transpose), false);
    #if (OMV_ENABLE_FIR_LEPTON == 1)
    int arg_timeout = py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_timeout), -1);
    #endif

    switch(fir_sensor) {
        case FIR_NONE: {
            return mp_const_none;
        }
        #if (OMV_ENABLE_FIR_MLX90621 == 1)
        case FIR_MLX90621: {
            fb_alloc_mark();
            float Ta, *To = fb_alloc(MLX90621_WIDTH * MLX90621_HEIGHT * sizeof(float), FB_ALLOC_NO_HINT);
            fir_MLX90621_get_frame(&Ta, To);
            mp_obj_t result = fir_get_ir(MLX90621_WIDTH, MLX90621_HEIGHT, Ta, To,
                                         arg_hmirror ^ true, arg_vflip, fir_transposed, true);
            fb_alloc_free_till_mark();
            return result;
        }
        #endif
        #if (OMV_ENABLE_FIR_MLX90640 == 1)
        case FIR_MLX90640: {
            fb_alloc_mark();
            float Ta, *To = fb_alloc(MLX90640_WIDTH * MLX90640_HEIGHT * sizeof(float), FB_ALLOC_NO_HINT);
            fir_MLX90640_get_frame(&Ta, To);
            mp_obj_t result = fir_get_ir(MLX90640_WIDTH, MLX90640_HEIGHT, Ta, To,
                                         arg_hmirror ^ true, arg_vflip, fir_transposed, false);
            fb_alloc_free_till_mark();
            return result;
        }
        #endif
        #if (OMV_ENABLE_FIR_MLX90641 == 1)
        case FIR_MLX90641: {
            fb_alloc_mark();
            float Ta, *To = fb_alloc(MLX90641_WIDTH * MLX90641_HEIGHT * sizeof(float), FB_ALLOC_NO_HINT);
            fir_MLX90641_get_frame(&Ta, To);
            mp_obj_t result = fir_get_ir(MLX90641_WIDTH, MLX90641_HEIGHT, Ta, To,
                                         arg_hmirror ^ true, arg_vflip, fir_transposed, false);
            fb_alloc_free_till_mark();
            return result;
        }
        #endif
        #if (OMV_ENABLE_FIR_AMG8833 == 1)
        case FIR_AMG8833: {
            fb_alloc_mark();
            float Ta, *To = fb_alloc(AMG8833_WIDTH * AMG8833_HEIGHT * sizeof(float), FB_ALLOC_NO_HINT);
            fir_AMG8833_get_frame(&Ta, To);
            mp_obj_t result = fir_get_ir(AMG8833_WIDTH, AMG8833_HEIGHT, Ta, To,
                                         arg_hmirror ^ true, arg_vflip, fir_transposed, true);
            fb_alloc_free_till_mark();
            return result;
        }
        #endif
        #if (OMV_ENABLE_FIR_LEPTON == 1)
        case FIR_LEPTON: {
            return fir_lepton_read_ir(fir_width, fir_height, arg_hmirror, arg_vflip, fir_transposed, arg_timeout);
        }
        #endif
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_fir_read_ir_obj, 0, py_fir_read_ir);

mp_obj_t py_fir_draw_ir(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *dst_img = py_helper_arg_to_image_mutable(args[0]);

    image_t src_img;
    src_img.bpp = IMAGE_BPP_GRAYSCALE;

    size_t len;
    mp_obj_t *items, *arg_to;
    mp_obj_get_array(args[1], &len, &items);

    if (len == 3) {
        src_img.w = mp_obj_get_int(items[0]);
        src_img.h = mp_obj_get_int(items[1]);
        mp_obj_get_array_fixed_n(items[2], src_img.w * src_img.h, &arg_to);
    } else if (fir_sensor != FIR_NONE) {
        src_img.w = fir_transposed ? fir_height : fir_width;
        src_img.h = fir_transposed ? fir_width : fir_height;
        // Handle if the user passed an array of the array.
        if (len == 1) {
            mp_obj_get_array_fixed_n(*items, src_img.w * src_img.h, &arg_to);
        } else {
            mp_obj_get_array_fixed_n(args[1], src_img.w * src_img.h, &arg_to);
        }
    } else {
        mp_raise_msg(&mp_type_TypeError, MP_ERROR_TEXT("Invalid IR array!"));
    }

    int arg_x_off = 0;
    int arg_y_off = 0;
    uint offset = 2;
    if (n_args > 2) {
        if (MP_OBJ_IS_TYPE(args[2], &mp_type_tuple) || MP_OBJ_IS_TYPE(args[2], &mp_type_list)) {
            mp_obj_t *arg_vec;
            mp_obj_get_array_fixed_n(args[2], 2, &arg_vec);
            arg_x_off = mp_obj_get_int(arg_vec[0]);
            arg_y_off = mp_obj_get_int(arg_vec[1]);
            offset = 3;
        } else if (n_args > 3) {
            arg_x_off = mp_obj_get_int(args[2]);
            arg_y_off = mp_obj_get_int(args[3]);
            offset = 4;
        } else if (n_args > 2) {
            mp_raise_msg(&mp_type_TypeError, MP_ERROR_TEXT("Expected x and y offset!"));
        }
    }

    float arg_x_scale = 1.f;
    bool got_x_scale = py_helper_keyword_float_maybe(n_args, args,
            offset + 0, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_scale), &arg_x_scale);

    float arg_y_scale = 1.f;
    bool got_y_scale = py_helper_keyword_float_maybe(n_args, args,
            offset + 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_scale), &arg_y_scale);

    rectangle_t arg_roi;
    py_helper_keyword_rectangle_roi(&src_img, n_args, args, offset + 2, kw_args, &arg_roi);

    float tmp_x_scale = dst_img->w / ((float) arg_roi.w);
    float tmp_y_scale = dst_img->h / ((float) arg_roi.h);
    float tmp_scale = IM_MIN(tmp_x_scale, tmp_y_scale);

    if (n_args == 2) {
        arg_x_off = fast_floorf((dst_img->w - (arg_roi.w * tmp_scale)) / 2.f);
        arg_y_off = fast_floorf((dst_img->h - (arg_roi.h * tmp_scale)) / 2.f);
    }

    if (!got_x_scale) {
        arg_x_scale = tmp_scale;
    }

    if (!got_y_scale) {
        arg_y_scale = tmp_scale;
    }

    int arg_rgb_channel = py_helper_keyword_int(n_args, args, offset + 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_rgb_channel), -1);
    if ((arg_rgb_channel < -1) || (2 < arg_rgb_channel)) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("-1 <= rgb_channel <= 2!"));
    }

    int arg_alpha = py_helper_keyword_int(n_args, args, offset + 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_alpha), 128);
    if ((arg_alpha < 0) || (256 < arg_alpha)) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("0 <= alpha <= 256!"));
    }

    const uint16_t *color_palette = py_helper_keyword_color_palette(n_args, args, offset + 5, kw_args, rainbow_table);
    const uint8_t *alpha_palette = py_helper_keyword_alpha_palette(n_args, args, offset + 6, kw_args, NULL);

    image_hint_t hint = py_helper_keyword_int(n_args, args, offset + 7, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_hint), 0);

    int arg_x_size;
    bool got_x_size = py_helper_keyword_int_maybe(n_args, args, offset + 8, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_size), &arg_x_size);

    int arg_y_size;
    bool got_y_size = py_helper_keyword_int_maybe(n_args, args, offset + 9, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_size), &arg_y_size);

    if (got_x_scale && got_x_size) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Choose either x_scale or x_size not both!"));
    }

    if (got_y_scale && got_y_size) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Choose either y_scale or y_size not both!"));
    }

    if (got_x_size) {
        arg_x_scale = arg_x_size / ((float) arg_roi.w);
    }

    if (got_y_size) {
        arg_y_scale = arg_y_size / ((float) arg_roi.h);
    }

    if ((!got_x_scale) && (!got_x_size) && got_y_size) {
        arg_x_scale = arg_y_scale;
    }

    if ((!got_y_scale) && (!got_y_size) && got_x_size) {
        arg_y_scale = arg_x_scale;
    }

    mp_obj_t scale_obj = py_helper_keyword_object(n_args, args, offset + 10, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_scale), NULL);
    float min = FLT_MAX, max = -FLT_MAX;

    if (scale_obj) {
        mp_obj_t *arg_scale;
        mp_obj_get_array_fixed_n(scale_obj, 2, &arg_scale);
        min = mp_obj_get_float(arg_scale[0]);
        max = mp_obj_get_float(arg_scale[1]);
    } else {
        for (int i = 0, ii = src_img.w * src_img.h; i < ii; i++) {
            float temp = mp_obj_get_float(arg_to[i]);
            if (temp < min) {
                min = temp;
            }
            if (temp > max) {
                max = temp;
            }
        }
    }

    fb_alloc_mark();

    src_img.data = fb_alloc(src_img.w * src_img.h * sizeof(uint8_t), FB_ALLOC_NO_HINT);
    fir_fill_image_float_obj(&src_img, arg_to, min, max);

    imlib_draw_image(dst_img, &src_img, arg_x_off, arg_y_off, arg_x_scale, arg_y_scale, &arg_roi,
                     arg_rgb_channel, arg_alpha, color_palette, alpha_palette, hint, NULL, NULL);

    fb_alloc_free_till_mark();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_fir_draw_ir_obj, 2, py_fir_draw_ir);

mp_obj_t py_fir_snapshot(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    if (fir_sensor == FIR_NONE) return mp_const_none;

    bool arg_hmirror = py_helper_keyword_int(n_args, args, 0, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_hmirror), false);
    bool arg_vflip = py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_vflip), false);
    bool arg_transpose = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_transpose), false);

    image_t src_img;
    src_img.bpp = IMAGE_BPP_GRAYSCALE;
    src_img.w = arg_transpose ? fir_height : fir_width;
    src_img.h = arg_transpose ? fir_width : fir_height;

    float arg_x_scale = 1.f;
    bool got_x_scale = py_helper_keyword_float_maybe(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_scale), &arg_x_scale);

    float arg_y_scale = 1.f;
    bool got_y_scale = py_helper_keyword_float_maybe(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_scale), &arg_y_scale);

    rectangle_t arg_roi;
    py_helper_keyword_rectangle_roi(&src_img, n_args, args, 5, kw_args, &arg_roi);

    int arg_rgb_channel = py_helper_keyword_int(n_args, args, 6, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_rgb_channel), -1);
    if ((arg_rgb_channel < -1) || (2 < arg_rgb_channel)) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("-1 <= rgb_channel <= 2!"));
    }

    int arg_alpha = py_helper_keyword_int(n_args, args, 7, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_alpha), 128);
    if ((arg_alpha < 0) || (256 < arg_alpha)) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("0 <= alpha <= 256!"));
    }

    const uint16_t *color_palette = py_helper_keyword_color_palette(n_args, args, 8, kw_args, rainbow_table);
    const uint8_t *alpha_palette = py_helper_keyword_alpha_palette(n_args, args, 9, kw_args, NULL);

    image_hint_t hint = py_helper_keyword_int(n_args, args, 10, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_hint), 0);

    int arg_x_size;
    bool got_x_size = py_helper_keyword_int_maybe(n_args, args, 11, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_size), &arg_x_size);

    int arg_y_size;
    bool got_y_size = py_helper_keyword_int_maybe(n_args, args, 12, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_size), &arg_y_size);

    if (got_x_scale && got_x_size) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Choose either x_scale or x_size not both!"));
    }

    if (got_y_scale && got_y_size) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Choose either y_scale or y_size not both!"));
    }

    if (got_x_size) {
        arg_x_scale = arg_x_size / ((float) arg_roi.w);
    }

    if (got_y_size) {
        arg_y_scale = arg_y_size / ((float) arg_roi.h);
    }

    if ((!got_x_scale) && (!got_x_size) && got_y_size) {
        arg_x_scale = arg_y_scale;
    }

    if ((!got_y_scale) && (!got_y_size) && got_x_size) {
        arg_y_scale = arg_x_scale;
    }

    mp_obj_t scale_obj = py_helper_keyword_object(n_args, args, 13, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_scale), NULL);
    float min, max;

    if (scale_obj) {
        mp_obj_t *arg_scale;
        mp_obj_get_array_fixed_n(scale_obj, 2, &arg_scale);
        min = mp_obj_get_float(arg_scale[0]);
        max = mp_obj_get_float(arg_scale[1]);
    }

    int arg_pixformat = py_helper_keyword_int(n_args, args, 14, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_pixformat), PIXFORMAT_RGB565);
    if ((arg_pixformat != PIXFORMAT_GRAYSCALE) && (arg_pixformat != PIXFORMAT_RGB565)) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Invalid pixformat!"));
    }

    mp_obj_t copy_to_fb_obj = py_helper_keyword_object(n_args, args, 15, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_copy_to_fb), NULL);
    bool copy_to_fb = false;
    image_t *arg_other = NULL;

    if (copy_to_fb_obj) {
        if (mp_obj_is_integer(copy_to_fb_obj)) {
            copy_to_fb = mp_obj_get_int(copy_to_fb_obj);
        } else {
            arg_other = py_helper_arg_to_image_mutable_bayer(copy_to_fb_obj);
        }
    }

    if (copy_to_fb) {
        framebuffer_update_jpeg_buffer();
    }

    image_t dst_img;
    dst_img.w = fast_floorf(arg_roi.w * arg_x_scale);
    dst_img.h = fast_floorf(arg_roi.h * arg_y_scale);
    dst_img.bpp = (arg_pixformat == PIXFORMAT_RGB565) ? IMAGE_BPP_RGB565 : IMAGE_BPP_GRAYSCALE;

    if (copy_to_fb) {
        py_helper_set_to_framebuffer(&dst_img);
    } else if (arg_other) {
        bool fb = py_helper_is_equal_to_framebuffer(arg_other);
        size_t size = fb ? framebuffer_get_buffer_size() : image_size(arg_other);
        PY_ASSERT_TRUE_MSG((image_size(&dst_img) <= size),
                "The new image won't fit in the target frame buffer!");
        arg_other->w = dst_img.w;
        arg_other->h = dst_img.h;
        arg_other->bpp = dst_img.bpp;
        dst_img.data = arg_other->data;
        py_helper_update_framebuffer(&dst_img);
    } else {
        dst_img.data = xalloc(image_size(&dst_img));
    }

    #if (OMV_ENABLE_FIR_LEPTON == 1)
    int arg_timeout = py_helper_keyword_int(n_args, args, 16, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_timeout), -1);
    #endif

    fb_alloc_mark();
    src_img.data = fb_alloc(src_img.w * src_img.h * sizeof(uint8_t), FB_ALLOC_NO_HINT);

    switch(fir_sensor) {
        #if (OMV_ENABLE_FIR_MLX90621 == 1)
        case FIR_MLX90621: {
            float Ta, *To = fb_alloc(MLX90621_WIDTH * MLX90621_HEIGHT * sizeof(float), FB_ALLOC_NO_HINT);
            fir_MLX90621_get_frame(&Ta, To);

            if (!scale_obj) {
                fast_get_min_max(To, MLX90621_WIDTH * MLX90621_HEIGHT, &min, &max);
            }

            imlib_fill_image_from_float(&src_img, MLX90621_WIDTH, MLX90621_HEIGHT, To, min, max,
                                        arg_hmirror ^ true, arg_vflip, arg_transpose, true);
            fb_free();
            break;
        }
        #endif
        #if (OMV_ENABLE_FIR_MLX90640 == 1)
        case FIR_MLX90640: {
            float Ta, *To = fb_alloc(MLX90640_WIDTH * MLX90640_HEIGHT * sizeof(float), FB_ALLOC_NO_HINT);
            fir_MLX90640_get_frame(&Ta, To);

            if (!scale_obj) {
                fast_get_min_max(To, MLX90640_WIDTH * MLX90640_HEIGHT, &min, &max);
            }

            imlib_fill_image_from_float(&src_img, MLX90640_WIDTH, MLX90640_HEIGHT, To, min, max,
                                        arg_hmirror ^ true, arg_vflip, arg_transpose, false);
            fb_free();
            break;
        }
        #endif
        #if (OMV_ENABLE_FIR_MLX90641 == 1)
        case FIR_MLX90641: {
            float Ta, *To = fb_alloc(MLX90641_WIDTH * MLX90641_HEIGHT * sizeof(float), FB_ALLOC_NO_HINT);
            fir_MLX90641_get_frame(&Ta, To);

            if (!scale_obj) {
                fast_get_min_max(To, MLX90641_WIDTH * MLX90641_HEIGHT, &min, &max);
            }

            imlib_fill_image_from_float(&src_img, MLX90641_WIDTH, MLX90641_HEIGHT, To, min, max,
                                        arg_hmirror ^ true, arg_vflip, arg_transpose, false);
            fb_free();
            break;
        }
        #endif
        #if (OMV_ENABLE_FIR_AMG8833 == 1)
        case FIR_AMG8833: {
            float Ta, *To = fb_alloc(AMG8833_WIDTH * AMG8833_HEIGHT * sizeof(float), FB_ALLOC_NO_HINT);
            fir_AMG8833_get_frame(&Ta, To);

            if (!scale_obj) {
                fast_get_min_max(To, AMG8833_WIDTH * AMG8833_HEIGHT, &min, &max);
            }

            imlib_fill_image_from_float(&src_img, AMG8833_WIDTH, AMG8833_HEIGHT, To, min, max,
                                        arg_hmirror ^ true, arg_vflip, arg_transpose, true);
            fb_free();
            break;
        }
        #endif
        #if (OMV_ENABLE_FIR_LEPTON == 1)
        case FIR_LEPTON: {
            fir_lepton_fill_image(&src_img, fir_width, fir_height, !scale_obj, min, max,
                                  arg_hmirror, arg_vflip, arg_transpose, arg_timeout);
            break;
        }
        #endif
        default: {
            break;
        }
    }

    imlib_draw_image(&dst_img, &src_img, 0, 0, arg_x_scale, arg_y_scale,
            &arg_roi, arg_rgb_channel, arg_alpha, color_palette, alpha_palette,
            (hint & (~IMAGE_HINT_CENTER)) | IMAGE_HINT_BLACK_BACKGROUND, NULL, NULL);

    fb_alloc_free_till_mark();

    return py_image_from_struct(&dst_img);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_fir_snapshot_obj, 0, py_fir_snapshot);

STATIC const mp_rom_map_elem_t globals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_fir)                    },
    { MP_ROM_QSTR(MP_QSTR_FIR_NONE),            MP_ROM_INT(FIR_NONE)                        },
#if (OMV_ENABLE_FIR_MLX90621 == 1)
    { MP_ROM_QSTR(MP_QSTR_FIR_SHIELD),          MP_ROM_INT(FIR_MLX90621)                    },
    { MP_ROM_QSTR(MP_QSTR_FIR_MLX90621),        MP_ROM_INT(FIR_MLX90621)                    },
#endif
#if (OMV_ENABLE_FIR_MLX90640 == 1)
    { MP_ROM_QSTR(MP_QSTR_FIR_MLX90640),        MP_ROM_INT(FIR_MLX90640)                    },
#endif
#if (OMV_ENABLE_FIR_MLX90641 == 1)
    { MP_ROM_QSTR(MP_QSTR_FIR_MLX90641),        MP_ROM_INT(FIR_MLX90641)                    },
#endif
#if (OMV_ENABLE_FIR_AMG8833 == 1)
    { MP_ROM_QSTR(MP_QSTR_FIR_AMG8833),         MP_ROM_INT(FIR_AMG8833)                     },
#endif
#if (OMV_ENABLE_FIR_LEPTON == 1)
    { MP_ROM_QSTR(MP_QSTR_FIR_LEPTON),          MP_ROM_INT(FIR_LEPTON)                      },
#endif
    { MP_ROM_QSTR(MP_QSTR_PALETTE_RAINBOW),     MP_ROM_INT(COLOR_PALETTE_RAINBOW)           },
    { MP_ROM_QSTR(MP_QSTR_PALETTE_IRONBOW),     MP_ROM_INT(COLOR_PALETTE_IRONBOW)           },
    { MP_ROM_QSTR(MP_QSTR_GRAYSCALE),           MP_ROM_INT(PIXFORMAT_GRAYSCALE)             },
    { MP_ROM_QSTR(MP_QSTR_RGB565),              MP_ROM_INT(PIXFORMAT_RGB565)                },
    { MP_ROM_QSTR(MP_QSTR_init),                MP_ROM_PTR(&py_fir_init_obj)                },
    { MP_ROM_QSTR(MP_QSTR_deinit),              MP_ROM_PTR(&py_fir_deinit_obj)              },
    { MP_ROM_QSTR(MP_QSTR_type),                MP_ROM_PTR(&py_fir_type_obj)                },
    { MP_ROM_QSTR(MP_QSTR_width),               MP_ROM_PTR(&py_fir_width_obj)               },
    { MP_ROM_QSTR(MP_QSTR_height),              MP_ROM_PTR(&py_fir_height_obj)              },
    { MP_ROM_QSTR(MP_QSTR_refresh),             MP_ROM_PTR(&py_fir_refresh_obj)             },
    { MP_ROM_QSTR(MP_QSTR_resolution),          MP_ROM_PTR(&py_fir_resolution_obj)          },
#if (OMV_ENABLE_FIR_LEPTON == 1)
    { MP_ROM_QSTR(MP_QSTR_radiometric),         MP_ROM_PTR(&py_fir_radiometric_obj)         },
#if defined(OMV_FIR_LEPTON_VSYNC_PRESENT)
    { MP_ROM_QSTR(MP_QSTR_register_vsync_cb),   MP_ROM_PTR(&py_fir_register_vsync_cb_obj)   },
#else
    { MP_ROM_QSTR(MP_QSTR_register_vsync_cb),   MP_ROM_PTR(&py_func_unavailable_obj)        },
#endif
    { MP_ROM_QSTR(MP_QSTR_register_frame_cb),   MP_ROM_PTR(&py_fir_register_frame_cb_obj)   },
    { MP_ROM_QSTR(MP_QSTR_get_frame_available), MP_ROM_PTR(&py_fir_get_frame_available_obj) },
    { MP_ROM_QSTR(MP_QSTR_trigger_ffc),         MP_ROM_PTR(&py_fir_trigger_ffc_obj)         },
#else
    { MP_ROM_QSTR(MP_QSTR_radiometric),         MP_ROM_PTR(&py_func_unavailable_obj)        },
    { MP_ROM_QSTR(MP_QSTR_register_vsync_cb),   MP_ROM_PTR(&py_func_unavailable_obj)        },
    { MP_ROM_QSTR(MP_QSTR_register_frame_cb),   MP_ROM_PTR(&py_func_unavailable_obj)        },
    { MP_ROM_QSTR(MP_QSTR_get_frame_available), MP_ROM_PTR(&py_func_unavailable_obj)        },
    { MP_ROM_QSTR(MP_QSTR_trigger_ffc),         MP_ROM_PTR(&py_func_unavailable_obj)        },
#endif
    { MP_ROM_QSTR(MP_QSTR_read_ta),             MP_ROM_PTR(&py_fir_read_ta_obj)             },
    { MP_ROM_QSTR(MP_QSTR_read_ir),             MP_ROM_PTR(&py_fir_read_ir_obj)             },
    { MP_ROM_QSTR(MP_QSTR_draw_ir),             MP_ROM_PTR(&py_fir_draw_ir_obj)             },
    { MP_ROM_QSTR(MP_QSTR_snapshot),            MP_ROM_PTR(&py_fir_snapshot_obj)            }
};

STATIC MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);

const mp_obj_module_t fir_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_t) &globals_dict,
};

void py_fir_init0()
{
    py_fir_deinit();
}
