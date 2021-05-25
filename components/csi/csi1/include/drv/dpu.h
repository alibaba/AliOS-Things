/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     drv_dpu.h
 * @brief    header file for dpu driver
 * @version  V1.6
 * @date     27. May 2019
 * @model    dpu
 ******************************************************************************/

#ifndef _DRV_DPU_H_
#define _DRV_DPU_H_

#include <stdint.h>
#include <drv/common.h>

#ifdef __cplusplus
extern "C" {
#endif

/// definition for dpu handle.
typedef void *dpu_handle_t;
typedef uint32_t dpu_rgba32_t; /* rrggbbaa */

/*----- DPU interrupt event type -----*/
typedef enum {
    DPU_EVENT_VSYNC = 0,  ///< VSYNC event
    DPU_EVENT_HSYNC,      ///< HSYNC event
} dpu_event_e;

/*----- DPU extend error type -----*/
typedef enum {
    DPU_ERROR_GET_CONFIG_TYPE = (DRV_ERROR_SPECIFIC + 1),
    DPU_ERROR_SET_CONFIG_TYPE,
} dpu_errno_e;

/*----- DPU color mode type -----*/
typedef enum {
    DPU_COLORMODE_RGBA8888 = 0, ///< 32-bit RGBA mode
    DPU_COLORMODE_ABGR8888,     ///< 32-bit ABGR mode
    DPU_COLORMODE_ARGB8888,     ///< 32-bit ARGB mode
    DPU_COLORMODE_BGRA8888,     ///< 32-bit BGRA mode

    DPU_COLORMODE_RGB888,       ///< 24-bit RGB mode

    DPU_COLORMODE_RGB565,       ///< 16-bit RGB mode
    DPU_COLORMODE_RGBA5551,     ///< 16-bit RGBA mode
    DPU_COLORMODE_RGBA4444,     ///< 16-bit RGBA mode

    DPU_COLORMODE_RGBA3320,     ///< 8-bit RGBA mode

    DPU_COLORMODE_L1,           ///< 1-bit Grayscale mode from 0(black) to 1 (white)
    DPU_COLORMODE_L4,           ///< 4-bit Grayscale mode from 0(black) to 15 (white)
    DPU_COLORMODE_L8,           ///< 8-bit Grayscale mode from 0(black) to 255 (white)
    DPU_COLORMODE_PALETTE8,     ///< 8-bit Palatte(looked-up table) mode

    DPU_COLORMODE_YUYV,         ///< 32-bit/2-pixels YUV422 mode. (Y0 U0 Y1 V0)
    DPU_COLORMODE_YUY2,         ///< 32-bit/2-pixels YUV422 mode. (Y0 U0 Y1 V0)
    DPU_COLORMODE_YUV420SP,     ///< 48-bit/4-pixels YUV420 planar mode. (YYYY..UV..)
    DPU_COLORMODE_YUV420P,      ///< 48-bit/4-pixels YUV420 planar mode. (YYYY..U..V..)
} dpu_colormode_e;

/*----- DPU blend mode type -----*/
typedef enum {
    /* Blended Color = Cs * Fs + Cd * Fd
     * Cs: Color of source      (previous layer)
     * Cd: Color of destinaion  (current layer)
     * Fs: Factor of source     (previous layer)
     * Fd: Factor of destinaion (current layer)
     *
     * The factors are defined in below blend modes:
     * Sa = Source alpha        (pixel alpha in previous layer)
     * Da = Destinaion alpha    (pixel alpha in current layer)
     * Ga = Global alpha        (global alpha in layer configuration)
     */
    DPU_BLENDMODE_SIMPLE = 0,   ///< Sa * Sa + Da * (1 - Sa)
    DPU_BLENDMODE_CLEAR,        ///< 0
    DPU_BLENDMODE_SRC,          ///< Sa
    DPU_BLENDMODE_SRC_OVER,     ///< Sa + Da * (1 - Sa)
    DPU_BLENDMODE_DST_OVER,     ///< Sa * (1 - Da) + Da
    DPU_BLENDMODE_SRC_IN,       ///< Sa * Da
    DPU_BLENDMODE_DST_IN,       ///< Da * Sa
    DPU_BLENDMODE_SRC_OUT,      ///< Sa * (1 - Da)
    DPU_BLENDMODE_DST_OUT,      ///< Da * (1 - Sa)
    DPU_BLENDMODE_SRC_ATOP,     ///< Sa * Da + Da * (1 - Sa)
    DPU_BLENDMODE_DST_ATOP,     ///< Sa * (1 - Da) + Da * Sa
    DPU_BLENDMODE_ADD,          ///< Sa + Da
    DPU_BLENDMODE_XOR,          ///< Sa * (1 - Da) + Da * (1 - Sa)
    DPU_BLENDMODE_MIX,          ///< Sa * Ga + Da
} dpu_blendmode_e;

/*----- LCD signal active mode -----*/
typedef enum {
    DPU_SIGNAL_ACTIVE_LOW  = 0,
    DPU_SIGNAL_ACTIVE_HIGH = 1,
} dpu_signal_polarity_e;

/**
  \brief LCD display timing info
*/
typedef struct {
    uint32_t resolution_x;      ///< Horizontal valid data width
    uint32_t resolution_y;      ///< Vertical valid data width
    uint32_t hsync_width;       ///< HSync pulse width
    uint32_t vsync_width;       ///< VSync pluse width
    uint32_t front_porch_x;     ///< HSync front porch
    uint32_t front_porch_y;     ///< VSync front porch
    uint32_t back_porch_x;      ///< HSync back porch
    uint32_t back_porch_y;      ///< VSync back porch
} dpu_cfg_display_timing_t;

/**
  \brief LCD signal polarity config info
*/
typedef struct {
    dpu_signal_polarity_e vsync;        ///< vsync signal polarity
    dpu_signal_polarity_e hsync;        ///< hsync signal polarity
    dpu_signal_polarity_e deta_enable;  ///< deta_enable signal polarity
    dpu_signal_polarity_e pixel_clock;  ///< pixel_clock signal polarity
} dpu_cfg_display_signal_polarity_t;

/**
  \brief DPU layer config info
*/
typedef struct {
    void           *baseaddr;   ///< picture store address
    dpu_colormode_e colormode;  ///< color mode of the picture
    dpu_blendmode_e blendmode;  ///< blend mode with other layer(s)
    uint32_t        resx;       ///< resolution width of target display device
    uint32_t        resy;       ///< resolution height of target display device
    int32_t         startx;     ///< display horizontal position on target device
    int32_t         starty;     ///< display vertical position on target device
    uint32_t        sizex;      ///< display width on target device
    uint32_t        sizey;      ///< display height on target device
    uint8_t         alpha;      ///< display diaphaneity
} dpu_layer_cfg_t;

/**
  \brief DPU cursor info
*/
typedef struct {
    uint8_t width;
    uint8_t height;
    uint8_t *img_data;
} dpu_cursor_info_t;

///< Pointer to \ref dpu_event_cb_t : DPU Event call back.
typedef void (*dpu_event_cb_t)(dpu_handle_t handle, dpu_event_e event, void *user_data);

/**
  \brief       Initialize DPU Interface.
                Initialize the resources needed for the DPU interface
  \param[in]   idx  dpu index
  \param[in]   cb_event  event call back function \ref dpu_event_cb_t
  \return      pointer to dpu instance
*/
dpu_handle_t csi_dpu_initialize(int32_t idx, dpu_event_cb_t cb_event, void *user_data);

/**
  \brief       De-initialize DPU Interface.
                Stop operation and release the resources used by the interface
  \param[in]   handle  dpu handle to operate.
  \return      error code
*/
int32_t csi_dpu_uninitialize(dpu_handle_t handle);

/**
  \brief       Start DPU Interface.
  \param[in]   handle  dpu handle to operate.
  \return      error code
*/
int32_t csi_dpu_start(dpu_handle_t handle);

/**
  \brief       Stop DPU Interface.
  \param[in]   handle  dpu handle to operate.
  \return      error code
*/
int32_t csi_dpu_stop(dpu_handle_t handle);

/**
  \brief       Configurate display parameters.
  \param[in]   handle   dpu handle to operate.
  \param[in]   timing   the display's timing parameters.
  \param[in]   polarity the display's signal parameters.
  \return      error code
*/
int32_t csi_dpu_config_display(dpu_handle_t handle,
                               dpu_cfg_display_timing_t *timing,
                               dpu_cfg_display_signal_polarity_t *polarity);

/**
  \brief       Wait for display vsync.
  \param[in]   handle  dpu handle to operate.
  \return      error code
*/
int32_t csi_dpu_wait_vsync(dpu_handle_t handle);

/**
  \brief       Set background color.
  \param[in]   handle  dpu handle to operate.
  \param[in]   color   the background color.
  \return      error code
*/
int32_t csi_dpu_set_bgcolor(dpu_handle_t handle, dpu_rgba32_t color);

/**
  \brief       Set cursor image.
  \param[in]   handle  dpu handle to operate.
  \param[in]   img     cursor image address.
  \return      error code
*/
int32_t csi_dpu_cursor_set_img(dpu_handle_t handle, dpu_cursor_info_t *cursor);

/**
  \brief       Set cursor color palette.
  \param[in]   handle  dpu handle to operate.
  \param[in]   index   palette index.
  \param[in]   color   palette color of index.
  \return      error code
*/
int32_t csi_dpu_cursor_set_palette(dpu_handle_t handle, uint32_t index, dpu_rgba32_t color);

/**
  \brief       Enable/Disable cursor
  \param[in]   handle  dpu handle to operate.
  \param[in]   enable  set cursor enable or not. 0:disable; 1:enable.
  \return      error code
*/
int32_t csi_dpu_cursor_enable(dpu_handle_t handle, int32_t enable);

/**
  \brief       Set cursor position.
  \param[in]   handle  dpu handle to operate.
  \param[in]   x       cursor's x position.
  \param[in]   y       cursor's y position.
  \return      error code
*/
int32_t csi_dpu_cursor_set_position(dpu_handle_t handle, uint32_t x, uint32_t y);

/**
  \brief       Set layer's configuration parameters.
  \param[in]   handle    dpu handle to operate.
  \param[in]   layer_id  layer index number.
  \param[in]   config    layer configuration parameters.
  \return      error code
*/
int32_t csi_dpu_layer_set_config(dpu_handle_t handle, uint8_t layer_id, dpu_layer_cfg_t *config);

/**
  \brief       Get layer's configuration parameters.
  \param[in]   handle    dpu handle to operate.
  \param[in]   layer_id  layer index number.
  \param[out]  config    layer configuration parameters.
  \return      error code
*/
int32_t csi_dpu_layer_get_config(dpu_handle_t handle, uint8_t layer_id, dpu_layer_cfg_t *config);

/**
  \brief       Enable/Disable layer
  \param[in]   handle    dpu handle to operate.
  \param[in]   layer_id  layer index number.
  \param[in]   enable    Set layer enable or not. 0:disable; 1:enable.
  \return      error code
*/
int32_t csi_dpu_layer_enable(dpu_handle_t handle, uint8_t layer_id, int32_t enable);

/**
  \brief       Set layer's palette lookup table color
  \param[in]   handle    dpu handle to operate.
  \param[in]   layer_id  layer index number.
  \param[in]   index     the index of palette.
  \param[in]   color     the color of the specific palette index.
  \return      error code
*/
int32_t csi_dpu_set_palette(dpu_handle_t handle, uint8_t layer_id,
                            uint32_t index, dpu_rgba32_t color);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_DPU_H_ */
