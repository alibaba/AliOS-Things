/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef UDISPLAY_API_H
#define UDISPLAY_API_H

#include <aos/kernel.h>
#include <stdbool.h>
#include <fb_define.h>
#include <string.h>

/* Here is Macro and struct definition*/
typedef struct _udisplay_context_t {
    uint8_t *framebuffer;
    uint32_t framebuffer_size;
    fb_var_screeninfo_t var;
    int32_t fd;
    uint8_t fb_id;
    bool swap;
    aos_mutex_t mutex;
} udisplay_context_t;

#define FB_PATH  "/dev/fb0"

#define UDISPLAY_STEP_DEBUG printf("[%s][%d]excute to here\n", \
                                        __func__, __LINE__);


/** @defgroup udisplay_aos_api udisplay
 * @{
 */

/**
 * Init the udisplay module.
 *
 * @retrun 0 on success, otherwise will be failed.
 */
int32_t udisplay_init(void);

/**
 * udisplay show rect function.
 *
 * @param[in]  buf           Graphic Framebuffer
 * @param[in]  x             x coordinate
 * @param[in]  y             y coordinate
 * @param[in]  w             Graphic Wide
 * @param[in]  h             Graphic High
 * @param[in]  rotate        Rotate (set true or false, The rotation angle is determined by the drivers)
 *
 * @return  0 on success, negative error on failure.
 */
int32_t udisplay_show_rect(uint8_t *buf, uint32_t x, \
                    uint32_t y, uint32_t w, uint32_t h, bool rotate);


/**
 * udispaly get framebuffer from drivers.
 *
 * @retrun buffer on success, otherwise will return NULL.
 */
uint8_t *udisplay_get_framebuffer(void);

/**
 * udispaly show (the buffer get from udisplay_get_framebuffer function).
 *
 * @retrun 0 on success, otherwise will be failed.
 */
int32_t udisplay_show(void);


/**
 * udispaly set bright
 * before it need enable backlight
 * the framebuffer need get from drivers
 *
 * @param[in]  brightness             brightness value
 *
 * @retrun 0 on success, otherwise will be failed.
 */
int32_t udisplay_set_brightness(int32_t brightness);

/**
 * udispaly get bright value
 * before it need enable backlight
 * the framebuffer need get from drivers
 *
 * @retrun bright value on success, otherwise will be failed.
 */
int32_t udisplay_get_brightness(void);

/**
 * udispaly enable backlight
 * the framebuffer need get from drivers
 *
 * @retrun bright value on success, otherwise will be failed.
 */
int32_t udisplay_enable_backlight(void);

/**
 * udispaly disable backlight
 * the framebuffer need get from drivers
 *
 * @retrun bright value on success, otherwise will be failed.
 */
int32_t udisplay_disable_backlight(void);

/**
 * @}
 */

#endif

