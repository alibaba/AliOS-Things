/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef UDISPLAY_CLI_API_H
#define UDISPLAY_CLI_API_H

/** @defgroup udisplay_cli_aos_api udisplay_cli
 *  @ingroup udisplay_aos_api
 * @{
 */

/**
 * udispaly test API
 * test pattern for rgb32
 *
 * @param[in]  color             color value for show
 * @param[in]  rotate            Rotate (set true or false, The rotation angle is determined by the drivers)
 *
 * @retrun 0 on success, otherwise will be failed.
 */
int32_t udisplay_pattern_rgb32(uint32_t color, bool rotate);

/**
 * udispaly test API
 * test pattern for rgb16
 *
 * @param[in]  color             color value for show
 * @param[in]  rotate            Rotate (set true or false, The rotation angle is determined by the drivers)
 *
 * @retrun 0 on success, otherwise will be failed.
 */
int32_t udisplay_pattern_rgb16(uint32_t color, bool rotate);


/**
 * @}
 */

#endif


