/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#ifndef _REG_ALIAS_H_
#define _REG_ALIAS_H_

#ifdef CHIP_DIE_8909
/////// reg name alias ///////
// hwp_sysCtrl
#define Reset_Cause         reset_cause
#define REG_DBG             reg_dbg
#define Sys_Rst_Set         sys_rst_set0
#define Sys_Rst_Clr         sys_rst_clr0
#define Cfg_Clk_Sys         cfg_clk_sys
#define Cfg_Clk_Mem_Bridge  cfg_clk_psram
#define Cfg_Clk_Uart        cfg_clk_uart
#define BB_Rst_Clr          sys_rst_clr1
#define BB_Rst_Set          sys_rst_set1

/////// macro alias ///////
#define SYS_CTRL_WATCHDOG_RESET_HAPPENED SYS_CTRL_WATCHDOG_RESET
#define SYS_CTRL_SOFT_RST                SYS_CTRL_SET_RST_GLOBAL
#define SYS_CTRL_SYS_FREQ_26M            SYS_CTRL_FREQ_26M

#endif


#endif
