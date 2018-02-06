
/*
*********************************************************************************************************
*
*                                         BOARD SUPPORT PACKAGE
*
*                                 (c) Copyright 2014, Micrium, Weston, FL
*                                          All Rights Reserved
*
* Filename      : bsp_cfg.h
* Version       : V1.00
* Programmer(s) : JJL
*                 JM
*********************************************************************************************************
*/

#ifndef  BSP_CFG_H_
#define  BSP_CFG_H_


/*
*********************************************************************************************************
*                                            MODULES ENABLE
*********************************************************************************************************
*/

#define  BSP_CFG_CAN_EN                     0    /* Enable (1) or Disable (0) os/CAN                   */
#define  BSP_CFG_CAN_OPEN_EN                0    /* Enable (1) or Disable (0) os/CANopen               */
#define  BSP_CFG_FS_EN                      0    /* Enable (1) or Disable (0) os/FS                    */
#define  BSP_CFG_GUI_EN                     0    /* Enable (1) or Disable (0) os/GUI                   */
#define  BSP_CFG_MBM_EN                     0    /* Enable (1) or Disable (0) os/Modbus-Master         */
#define  BSP_CFG_MBS_EN                     0    /* Enable (1) or Disable (0) os/Modbus-Slave          */
#define  BSP_CFG_NET_EN                     0    /* Enable (1) or Disable (0) os/TCP-IP                */
#define  BSP_CFG_OS2_EN                     0    /* Enable (1) or Disable (0) os/OS-II                 */
#define  BSP_CFG_OS3_EN                     1    /* Enable (1) or Disable (0) os/OS-III                */
#define  BSP_CFG_USBD_EN                    0    /* Enable (1) or Disable (0) os/USB-D                 */
#define  BSP_CFG_USBH_EN                    0    /* Enable (1) or Disable (0) os/USB-H                 */


/*
*********************************************************************************************************
*                                        INTERRUPT VECTOR TABLE
*********************************************************************************************************
*/

#define  BSP_CFG_INT_VECT_TBL_RAM_EN        1    /* Enable (1) vector table in RAM or in ROM (0)       */
#define  BSP_CFG_INT_VECT_TBL_SIZE        256    /* Max. number of entries in the interrupt vector tbl */


/*
*********************************************************************************************************
*                                          CLOCK MANAGEMENT
*********************************************************************************************************
*/

#define  BSP_CFG_SYS_EXT_CLK_FREQ    12000000uL


/*
*********************************************************************************************************
*                                        BOARD SPECIFIC BSPs
*********************************************************************************************************
*/

#define  BSP_CFG_SER_EN                     0    /* Enable (1) or Disable (0) Serial port BSP          */

#define  BSP_CFG_LED_EN                     1    /* Enable (1) or Disable (0) LEDs                     */

#define  BSP_CFG_GRAPH_LCD_EN               0    /* Enable (1) or Disable (0) Graphical LCD display    */

#define  BSP_CFG_MISC_EN                    0    /* Enable (1) or Disable (0) Miscellaneous            */

#define  BSP_CFG_I2C_EN                     0    /* Enable (1) or Disable (0) i2c                      */


/*
*********************************************************************************************************
*                                     INTERRUPT PRIORITY LEVELS
*********************************************************************************************************
*/

#define  BSP_CFG_OS_TICK_IPL                3

#if      BSP_CFG_SER_EN > 0
#define  BSP_CFG_SER_IPL                    4
#endif
#endif
