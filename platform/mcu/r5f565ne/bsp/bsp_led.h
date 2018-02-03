/*
*********************************************************************************************************
*                                           BOARD SUPPORT PACKAGE
*
*                            (c) Copyright 2014; Micrium, Inc.; Weston, FL
*
*               All rights reserved. Protected by international copyright laws.
*
*               BSP is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Filename      : bsp_led.h
* Version       : V1.00
* Programmer(s) : DC
*********************************************************************************************************
*/

#ifndef  BSP_LED_H_
#define  BSP_LED_H_

#include "bsp.h"
/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/

#define  BSP_LED0                   PORT7.PODR.BIT.B0
#define  BSP_LED1                   PORT0.PODR.BIT.B5
#define  BSP_LED2                   PORT2.PODR.BIT.B6
#define  BSP_LED3                   PORT2.PODR.BIT.B7


/*
*********************************************************************************************************
*                                            PROTOTYPES
*********************************************************************************************************
*/

void    BSP_LED_Init    (void);
void    BSP_LED_On      (uint8_t  led);
void    BSP_LED_Off     (uint8_t  led);
void    BSP_LED_Toggle  (uint8_t  led);

#endif
