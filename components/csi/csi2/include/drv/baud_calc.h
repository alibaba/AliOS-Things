/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file       drv/baud_calc.h
 * @brief      Header File for the PWM capture uart bandrate Driver
 * @version    V1.0
 * @date       9. Oct 2020
 * @model      baud_calc
 ******************************************************************************/

#ifndef _DRV_BAUD_CALC_H_
#define _DRV_BAUD_CALC_H_

#include <stdint.h>
#include <drv/common.h>
#include <soc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
  \brief       Baud rate calculation(Algorithm level)
  \param[in]   idx        PWM idx
  \param[in]   channel    Channel num
  \return      Error code(-1) or Baudare value
*/
int drv_calc_baud_adjust(uint32_t idx, uint32_t channel);

/**
  \brief       Baud rate calculation(Capture level)
  \param[in]   idx        PWM idx
  \param[in]   channel    Channel num
  \return      Error code(-1) or Baudare value
*/
int drv_calc_baud_original(uint32_t idx, uint32_t channel);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_BAUD_CALC_H_ */
