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
*                                      BOARD SUPPORT PACKAGE (BSP)
*
*                                            RENESAS RX64M
*                                               on the
*                                     RSKRX64M DEVELOPMENT BOARD
*
* Filename      : bsp.h
* Version       : V1.00
* Programmer(s) : JPB
*                 JJL
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               INCLUDES
*********************************************************************************************************
*/

#include  "bsp_cfg.h"

#include  <bsp_sys.h>

#if BSP_CFG_LED_EN         > 0u
#include  <bsp_led.h>
#endif

#if BSP_CFG_CAN_EN         > 0u
#include  <can_bsp.h>
#endif

#if  BSP_CFG_CAN_OPEN_EN   > 0u
#endif

#if  BSP_CFG_FS_EN         > 0u
#endif


#if BSP_CFG_GRAPH_LCD_EN   > 0u
#include  <bsp_glcd.h>
#endif

#if  BSP_CFG_GUI_EN        > 0u
#endif


#if  BSP_CFG_MBM_EN        > 0u
#endif


#if  BSP_CFG_MBS_EN        > 0u
#endif


#if  BSP_CFG_NET_EN        > 0u
#endif


#if  BSP_CFG_OS2_EN        > 0u
#endif


#if  BSP_CFG_OS3_EN        > 0u
#endif


#if  BSP_CFG_USBD_EN       > 0u
#endif


#if  BSP_CFG_USBH_EN       > 0u
#endif


#if BSP_CFG_MISC_EN        > 0u
#include  <bsp_misc.h>
#endif

#include  <iodefine.h>

extern void  OS_BSP_TickInit (void);

/*
*********************************************************************************************************
*                                             BSP_Init()
*
* Description : This function should be called by the application code before using any functions in this
*               module.
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

void  BSP_Init (void)
{
    BSP_SysInit();                                              /* Initialize System Services, Clocks, etc.             */

    OS_BSP_TickInit();                                          /* Init uC/OS-xx periodic time source                   */

#if BSP_CFG_LED_EN         > 0u
    BSP_LED_Init();                                             /* Initialize LEDs                                      */
#endif


#if BSP_CFG_GRAPH_LCD_EN   > 0u
    BSP_GraphLCD_Init();                                        /* Initialize the on-board graphics LCD display         */
#endif


#if  BSP_CFG_CAN_EN        > 0u
#endif


#if  BSP_CFG_CAN_OPEN_EN   > 0u
#endif


#if  BSP_CFG_FS_EN         > 0u
#endif


#if  BSP_CFG_GUI_EN        > 0u
#endif


#if  BSP_CFG_MBM_EN        > 0u
#endif


#if  BSP_CFG_MBS_EN        > 0u
#endif


#if  BSP_CFG_NET_EN        > 0u
#endif


#if  BSP_CFG_OS2_EN        > 0u
#endif


#if  BSP_CFG_OS3_EN        > 0u
#endif


#if  BSP_CFG_USBD_EN       > 0u
#endif


#if  BSP_CFG_USBH_EN       > 0u
#endif


#if BSP_CFG_MISC_EN        > 0u
    BSP_MiscInit();                                             /* Initialize Miscellaneous Services                    */
#endif
}

void   BSP_IntVectSet  (int16_t     int_id,void*  isr);
void   cpu_switchint_handle(void);
void   os_pre_init(void);

void BSP_Pre_Init(void)
{
    set_ipl(0u);		//IPL_BOUNDARY

	BSP_IntVectSet(27u, cpu_switchint_handle);             /* Setup kernel context switch                          */

	os_pre_init();
}
