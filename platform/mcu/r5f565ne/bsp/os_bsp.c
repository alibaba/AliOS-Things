/*
*********************************************************************************************************
*                                         BOARD SUPPORT PACKAGE
*
*                            (c) Copyright 2014; Micrium, Inc.; Weston, FL
*                    All rights reserved.  Protected by international copyright laws.
*
*                                      Renesas RX Specific code
*
* File      : os_bsp.c
* By        : PC
*             JM
*
* LICENSING TERMS:
* ---------------
*             BSP is provided in source form to registered licensees ONLY.  It is
*             illegal to distribute this source code to any third party unless you receive
*             written permission by an authorized Micrium representative.  Knowledge of
*             the source code may NOT be used to develop a similar product.
*
*             Please help us continue to provide the Embedded community with the finest
*             software available.  Your honesty is greatly appreciated.
*
*             You can contact us at www.micrium.com.
*********************************************************************************************************
*/

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *bsp_tick_c__c = "$Id: $";
#endif


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <bsp.h>

#include  "bsp_cfg.h"
#include  <bsp_sys.h>
#include  <bsp_int_vect_tbl.h>
#include  <iodefine.h>
#include  <machine.h>

CPU_ISR  OS_BSP_TickISR (void);

/*
*********************************************************************************************************
*                                          OS_BSP_TickInit()
*
* Description : Initialize the kernel's tick interrupt.
*
* Argument(s) : none.
*
* Caller(s)   : Application.
*
* Return(s)   : none.
*
* Note(s)     : (1) This function MUST be called after OSStart() & after processor initialization.
*********************************************************************************************************
*/

void  OS_BSP_TickInit (void)
{
    uint16_t  cmcor;

    
    BSP_IntVectSet(VECT_CMT0_CMI0, (CPU_FNCT_VOID)OS_BSP_TickISR);

#if (OS_VERSION >= 30000u)
    cmcor = BSP_SysPerClkFreqGet() / (32u * RHINO_CONFIG_TICKS_PER_SECOND);
#else
    cmcor = BSP_SysPerClkFreqGet() / (32u * RHINO_CONFIG_TICKS_PER_SECOND);
#endif

    MSTP(CMT0)          = 0u;                                   /* Enable CMT0 module.                                  */

    CMT.CMSTR0.BIT.STR0 = 0u;                                   /* Stop timer channel 0.                                */
    CMT0.CMCR.BIT.CKS   = 1u;                                   /* Set peripheral clock divider.                        */

    CMT0.CMCOR          = (cmcor - 1u);                         /* Set compare-match value.                             */
    CMT0.CMCNT          = 0u;                                   /* Clear counter register.                              */

    IR(CMT0,  CMI0)     = 0u;                                   /* Clear any pending ISR.                               */
    IPR(CMT0, CMI0)     = BSP_CFG_OS_TICK_IPL;                  /* Set interrupt priority.                              */
    IEN(CMT0, CMI0)     = 1u;                                   /* Enable interrupt source.                             */

    CMT0.CMCR.BIT.CMIE  = 1u;                                   /* Enable interrupt.                                    */

    CMT.CMSTR0.BIT.STR0 = 1u;                                   /* Start timer.                                         */
}


/*
*********************************************************************************************************
*                                         OS_BSP_TickISR()
*
* Description : Kernel's tick interrupt handler
*
* Argument(s) : none.
*
* Caller(s)   : tick interrupt.
*
* Return(s)   : none.
*********************************************************************************************************
*/

#if      __RENESAS__
#pragma  interrupt  OS_BSP_TickISR
#endif

CPU_ISR  OS_BSP_TickISR (void)
{
	krhino_intrpt_enter();                                               /* Notify uC/OS-III or uCOS-II of ISR entry             */
    CPU_INT_GLOBAL_EN();                                        /* Reenable global interrupts                           */
    krhino_tick_proc();
    krhino_intrpt_exit();                                                /* Notify uC/OS-III or uCOS-II of ISR exit              */
}

