/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef _HAL_API_H_
#define _HAL_API_H_

#include "basic_types.h"
#include "rtl8711b_vector.h"

typedef enum  _HAL_Status
{
  HAL_OK            = 0x00,
  HAL_BUSY          = 0x01,
  HAL_TIMEOUT       = 0x02,
  HAL_ERR_PARA      = 0x03,     // error with invaild parameters 
  HAL_ERR_MEM       = 0x04,     // error with memory allocation failed
  HAL_ERR_HW        = 0x05,     // error with hardware error

  HAL_ERR_UNKNOWN   = 0xee      // unknown error
  
} HAL_Status;


#endif //_HAL_API_H_
