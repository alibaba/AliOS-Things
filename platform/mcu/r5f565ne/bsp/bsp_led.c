/*
*********************************************************************************************************
*                                         BOARD SUPPORT PACKAGE
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
* Filename      : bsp_led.c
* Version       : V1.00
* Programmer(s) : JPB
*                 AA
*                 JJL
*                 JM
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDES
*********************************************************************************************************
*/

#include  <bsp_cfg.h>

#if BSP_CFG_LED_EN > 0u
#include  <lib_def.h>
#include  <bsp_led.h>
#include  <iodefine.h>


/*
*********************************************************************************************************
*                                         BSP_LED_Init()
*
* Description : This function is used to initialize the LEDs on the board.
*
* Arguments   : none
*
* Caller(s)   : none.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Init (void)
{
    PORT7.PDR.BIT.B0 = 1u;                                      /* Select port function.                                */
//    PORT0.PDR.BIT.B5 = 1u;
//    PORT2.PDR.BIT.B6 = 1u;
//    PORT2.PDR.BIT.B7 = 1u;

    BSP_LED_Off(0u);                                            /* Turn OFF all LEDs.                                   */
}


/*
*********************************************************************************************************
*                                            BSP_LED_On()
*
* Description : This function is used to control any or all the LEDs on the board.
*
* Arguments   : led    Is the number of the LED to control
*                      0    indicates that you want ALL the LEDs to be ON
*                      1    turns ON LED1 on the board
*                      .
*                      .
*                      3    turns ON LED3 on the board
*
* Caller(s)   : none.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_On (uint8_t  led)
{
    switch (led) {
        case 0u:
            BSP_LED0  = DEF_OFF;
            BSP_LED1  = DEF_OFF;
            BSP_LED2  = DEF_OFF;
            BSP_LED3  = DEF_OFF;
            break;

        case 1u:
            BSP_LED0  = DEF_OFF;
            break;

        case 2u:
            BSP_LED1  = DEF_OFF;
            break;

        case 3u:
            BSP_LED2  = DEF_OFF;
            break;

        case 4u:
            BSP_LED3  = DEF_OFF;
            break;

        default:
            break;
    }
}


/*
*********************************************************************************************************
*                                            BSP_LED_Off()
*
* Description : This function is used to control any or all the LEDs on the board.
*
* Arguments   : led    Is the number of the LED to turn OFF
*                      0    indicates that you want ALL the LEDs to be OFF
*                      1    turns OFF LED1 on the board
*                      .
*                      .
*                      3    turns OFF LED3 on the board
*
* Caller(s)   : none.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Off (uint8_t  led)
{
    switch (led) {
        case 0u:
            BSP_LED0  = DEF_ON;
            BSP_LED1  = DEF_ON;
            BSP_LED2  = DEF_ON;
            BSP_LED3  = DEF_ON;
            break;

        case 1u:
            BSP_LED0  = DEF_ON;
            break;

        case 2u:
            BSP_LED1  = DEF_ON;
            break;

        case 3u:
            BSP_LED2  = DEF_ON;
            break;

        case 4u:
            BSP_LED3  = DEF_ON;
            break;

        default:
            break;
    }
}


/*
*********************************************************************************************************
*                                          BSP_LED_Toggle()
*
* Description : This function is used to toggle the state of any or all the LEDs on the board.
*
* Arguments   : led    Is the number of the LED to toggle
*                      0    indicates that you want ALL the LEDs to toggle
*                      1    Toggle LED1 on the board
*                      .
*                      .
*                      3    Toggle LED3 on the board
*
* Caller(s)   : none.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Toggle (uint8_t  led)
{
    switch (led) {
        case 0u:
            BSP_LED0  = ~BSP_LED0;
            BSP_LED1  = ~BSP_LED1;
            BSP_LED2  = ~BSP_LED2;
            BSP_LED3  = ~BSP_LED3;
            break;

        case 1u:
            BSP_LED0  = ~BSP_LED0;
            break;

        case 2u:
            BSP_LED1  = ~BSP_LED1;
            break;

        case 3u:
            BSP_LED2  = ~BSP_LED2;
            break;

        case 4u:
            BSP_LED3  = ~BSP_LED3;
            break;

        default:
            break;
    }
}
#endif
