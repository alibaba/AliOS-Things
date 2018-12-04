/**
  ******************************************************************************
  * @file    nano_util.h
  * @author  MCU China FAE team
  * @version 1.0
  * @date    23/11/2018
  * @brief   utility of nano kernel certification
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NANO_UTIL_H__
#define __NANO_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Exported macros -----------------------------------------------------------*/
#define PRINTF_RAW(...) {nano_util_Send(__VA_ARGS__);}while(0)
#define PRINTF          PRINTF_RAW

#ifdef __cplusplus
}
#endif

#endif 
