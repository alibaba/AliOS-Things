/*
*********************************************************************************************************
*                                         BOARD SUPPORT PACKAGE
*
*                                            Renesas RX64M
*                                                on the
*                                      RSKRX64M Evaluation Board
*
*                                 (c) Copyright 2013, Micrium, Weston, FL
*                                          All Rights Reserved
*
* Filename      : bsp_int_vect_tbl.c
* Version       : V1.00
* Programmer(s) : JFD
*                 AL
*                 CM
*                 DC
*********************************************************************************************************
*/

#include  "bsp.h"
#include  "bsp_cfg.h"

#include  <bsp_int_vect_tbl.h>

#include "rx_platform.h"           // Located in the FIT BSP module
#include "r_sci_rx_if.h"        // The SCI module API interface file.
#include "r_byteq_if.h"         // The BYTEQ module API interface file.
#include "r_sci_rx_config.h"    // User configurable options for the SCI module
#include "r_sci_rx65n_private.h"
#include "r_sci_rx_private.h"


#pragma interrupt sci9_txi9_isr(vect=VECT(SCI9,TXI9))
void sci9_txi9_isr(void);
#pragma interrupt sci9_rxi9_isr(vect=VECT(SCI9,RXI9))
void sci9_rxi9_isr(void);

#pragma interrupt group_bl0_handler_isr(vect=VECT(ICU,GROUPBL0))
void group_bl0_handler_isr(void);
#pragma interrupt group_bl1_handler_isr(vect=VECT(ICU,GROUPBL1))
void group_bl1_handler_isr(void);

#pragma interrupt sci2_txi2_isr(vect=VECT(SCI2,TXI2))
#pragma interrupt sci2_rxi2_isr(vect=VECT(SCI2,RXI2))

#pragma interrupt sci5_txi5_isr(vect=VECT(SCI5,TXI5))
#pragma interrupt sci5_rxi5_isr(vect=VECT(SCI5,RXI5))

CPU_ISR sci5_txi5_isr(void)
{
	krhino_intrpt_enter();                                               /* Notify uC/OS-III or uCOS-II of ISR entry             */
    CPU_INT_GLOBAL_EN();                                        /* Reenable global interrupts                           */
    txi_handler(&ch5_ctrl);
    krhino_intrpt_exit();
}

CPU_ISR sci5_rxi5_isr(void)
{
	krhino_intrpt_enter();                                               /* Notify uC/OS-III or uCOS-II of ISR entry             */
    CPU_INT_GLOBAL_EN();                                        /* Reenable global interrupts                           */
    rxi_handler(&ch5_ctrl,5);
    krhino_intrpt_exit();
} /* End of function sci5_rxi5_isr() */


CPU_ISR sci2_txi2_isr(void)
{
	krhino_intrpt_enter();                                               /* Notify uC/OS-III or uCOS-II of ISR entry             */
    CPU_INT_GLOBAL_EN();                                        /* Reenable global interrupts                           */
    txi_handler(&ch2_ctrl);
    krhino_intrpt_exit();

}

CPU_ISR sci2_rxi2_isr(void)
{
	krhino_intrpt_enter();                                               /* Notify uC/OS-III or uCOS-II of ISR entry             */
    CPU_INT_GLOBAL_EN();                                        /* Reenable global interrupts                           */
    rxi_handler(&ch2_ctrl,2);
    krhino_intrpt_exit();
} /* End of function sci2_rxi2_isr() */
/*
*********************************************************************************************************
*                                       INTERRUPT VECTOR TABLE
*
* Note(s): 1) Could be in RAM if BSP_CFG_INT_VECT_RAM_EN is set to 1 in 'bsp_cfg.h'
*          2) In either case BSP_IntVectSet() must becalled to boint to BSP_IntVectTbl[]
*          3) INTB Interrupts are mapped directly correlated to the IVT Vector # they are connected
*             with, due to the amount of INTB IVT vectors presented. This has been done for ease of
*             access when selecting a INTB Interrupt. See Page 447+ of the TRM. Excess IVT Vector #'s can
*             be used for other INTB Interrupts (As a way to have multiple interrupts for a single event).
*          4) INTA Interrupts, however, have to be selected based on a "As Desired" basis due to the
*             limited amount of INTA IVT vectors present in the Vector Table.
*********************************************************************************************************
*/
CPU_ISR  OS_BSP_TickISR (void);
void cpu_switchint_handle(void);

#if     BSP_CFG_INT_VECT_TBL_RAM_EN > 0u
static  CPU_FNCT_VOID  BSP_IntVectTbl[] =
#else
const   CPU_FNCT_VOID  BSP_IntVectTbl[] =
#endif
{
    (CPU_FNCT_VOID)BSP_IntHandler_000,                          /*  00, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_001,                          /*  01, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_002,                          /*  02, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_003,                          /*  03, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_004,                          /*  04, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_005,                          /*  05, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_006,                          /*  06, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_007,                          /*  07, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_008,                          /*  08, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_009,                          /*  09, Reserved                                        */

    (CPU_FNCT_VOID)BSP_IntHandler_010,                          /*  10, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_011,                          /*  11, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_012,                          /*  12, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_013,                          /*  13, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_014,                          /*  14, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_015,                          /*  15, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_016,                          /*  16, BSC_BUSERR                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_017,                          /*  17, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_018,                          /*  18, RAM_RAMERR                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_019,                          /*  19, Reserved                                        */

    (CPU_FNCT_VOID)BSP_IntHandler_020,                          /*  20, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_021,                          /*  21, FCU_FIFERR                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_022,                          /*  22, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_023,                          /*  23, FCU_FRDYI                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_024,                          /*  24, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_025,                          /*  25, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_026,                          /*  26, Reserved                                        */
    (CPU_FNCT_VOID)cpu_switchint_handle,                                  /*  27, ICU_SWINT  - uC/OS-xx Context Switch            */
    (CPU_FNCT_VOID)OS_BSP_TickISR,                              /*  28, CMTU0_CMI0 - uC/OS-xx Tick Interrupt Handler    */
    (CPU_FNCT_VOID)BSP_IntHandler_029,                          /*  29, CMTU0_CMI1                                      */

    (CPU_FNCT_VOID)BSP_IntHandler_030,                          /*  30, CMTU1_CMI2                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_031,                          /*  31, CMTU1_CMI3                                      */

#if ((BSP_CFG_USBD_EN > 0u) || (BSP_CFG_USBH_EN > 0u))
#if BSP_CFG_USBD_EN > 0u 
    (CPU_FNCT_VOID)USBD_BSP_IntHandlerD0FIFO2,                  /*  32, USBD_HS_D0FIFO2                                 */
    (CPU_FNCT_VOID)USBD_BSP_IntHandlerD1FIFO2,                  /*  33, USBD_HS_D1FIFO2                                 */
    (CPU_FNCT_VOID)USBD_BSP_IntHandlerD0FIFO0,                  /*  34, USBD0_D0FIFO0                                   */
    (CPU_FNCT_VOID)USBD_BSP_IntHandlerD1FIFO0,                  /*  35, USBD0_D1FIFO0                                   */
#endif

#if BSP_CFG_USBH_EN > 0u
    (CPU_FNCT_VOID)USBH_BSP_ISR_Handler,                        /*  32, USBH_HS_D0FIFO2                                 */
    (CPU_FNCT_VOID)USBH_BSP_ISR_Handler,                        /*  33, USBH_HS_D1FIFO2                                 */
    (CPU_FNCT_VOID)USBH_RX600_BSP_IntHandler,                   /*  34, USBH0_D0FIFO0                                   */
    (CPU_FNCT_VOID)USBH_RX600_BSP_IntHandler,                   /*  35, USBH0_D1FIFO0                                   */
#endif

#else
    (CPU_FNCT_VOID)BSP_IntHandler_032,                          /*      32, ------------------------------------------- */
    (CPU_FNCT_VOID)BSP_IntHandler_033,                          /*      33, ------------------------------------------- */
    (CPU_FNCT_VOID)BSP_IntHandler_034,                          /*      34, ------------------------------------------- */
    (CPU_FNCT_VOID)BSP_IntHandler_035,                          /*      35, ------------------------------------------- */
#endif
    (CPU_FNCT_VOID)BSP_IntHandler_036,                          /*  36, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_037,                          /*  37, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_038,                          /*  38, RSPI0_SPRI0                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_039,                          /*  39, RSPI0_SPTI0                                     */

    (CPU_FNCT_VOID)BSP_IntHandler_040,                          /*  40, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_041,                          /*  41, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_042,                          /*  42, QSPI_SPRI                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_043,                          /*  43, QSPI_SPTI                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_044,                          /*  44, SDHI_SBFAI                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_045,                          /*  45, MMC_MBFAI                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_046,                          /*  46, SSI0_SSITXI0                                    */
    (CPU_FNCT_VOID)BSP_IntHandler_047,                          /*  47, SSI0_SSIRXI0                                    */
    (CPU_FNCT_VOID)BSP_IntHandler_048,                          /*  48, SSI1_SSIRTI1                                    */
    (CPU_FNCT_VOID)BSP_IntHandler_049,                          /*  49, Reserved                                        */

    (CPU_FNCT_VOID)BSP_IntHandler_050,                          /*  50, SRC_IDEI                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_051,                          /*  51, SRC_ODFI                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_052,                          /*  52, RIIC0_RXI0                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_053,                          /*  53, RIIC0_TXI0                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_054,                          /*  54, RIIC2_RXI2                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_055,                          /*  55, RIIC2_TXI2                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_056,                          /*  56, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_057,                          /*  57, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_058,                          /*  58, SCI0_RXI0                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_059,                          /*  59, SCI0_TXI0                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_060,                          /*  60, SCI1_RXI1                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_061,                          /*  61, SCI1_TXI1                                       */
    (CPU_FNCT_VOID)sci2_rxi2_isr,                          /*  62, SCI2_RXI2                                       */
    (CPU_FNCT_VOID)sci2_txi2_isr,                          /*  63, SCI2_TXI2                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_064,                          /*  64, ICU_IRQ0                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_065,                          /*  65, ICU_IRQ1                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_066,                          /*  66, ICU_IRQ2                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_067,                          /*  67, ICU_IRQ3                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_068,                          /*  68, ICU_IRQ4                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_069,                          /*  69, ICU_IRQ5                                        */

    (CPU_FNCT_VOID)BSP_IntHandler_070,                          /*  70, ICU_IRQ6                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_071,                          /*  71, ICU_IRQ7                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_072,                          /*  72, ICU_IRQ8                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_073,                          /*  73, ICU_IRQ9                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_074,                          /*  74, ICU_IRQ10                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_075,                          /*  75, ICU_IRQ11                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_076,                          /*  76, ICU_IRQ12                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_077,                          /*  77, ICU_IRQ13                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_078,                          /*  78, ICU_IRQ14                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_079,                          /*  79, ICU_IRQ15                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_080,                          /*  80, SCI3_RXI3                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_081,                          /*  81, SCI3_TXI3                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_082,                          /*  82, SCI4_RXI4                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_083,                          /*  83, SCI4_TXI4                                       */
    (CPU_FNCT_VOID)sci5_rxi5_isr,                          		/*  84, SCI5_RXI5                                       */
    (CPU_FNCT_VOID)sci5_txi5_isr,                          		/*  85, SCI5_TXI5                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_086,                          /*  86, SCI6_RXI6                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_087,                          /*  87, SCI6_TXI6                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_088,                          /*  88, LVD1_LVD1                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_089,                          /*  89, LVD2_LVD2                                       */
    
#if ((BSP_CFG_USBD_EN > 0u) || (BSP_CFG_USBH_EN > 0u))
#if BSP_CFG_USBD_EN > 0u
    (CPU_FNCT_VOID)USBD_BSP_IntHandlerUSBR0,                    /*  90, USBD_USBR0                                      */
#endif

#if BSP_CFG_USBH_EN > 0u
    (CPU_FNCT_VOID)USBH_RX600_BSP_IntHandler,                   /*  90, USBH_USBR0                                      */
#endif

#else
    (CPU_FNCT_VOID)BSP_IntHandler_090,                          /*      90, ------------------------------------------- */
#endif
    (CPU_FNCT_VOID)BSP_IntHandler_091,                          /*  91, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_092,                          /*  92, RTC_ALM                                         */
    (CPU_FNCT_VOID)BSP_IntHandler_093,                          /*  93, RTC_PRD                                         */
    
#if ((BSP_CFG_USBD_EN > 0u) || (BSP_CFG_USBH_EN > 0u))
#if BSP_CFG_USBD_EN > 0u
    (CPU_FNCT_VOID)USBD_BSP_IntHandlerUSBHSR,                   /*  94, USBD_HS_USBHSR                                  */
#endif

#if BSP_CFG_USBH_EN > 0u
    (CPU_FNCT_VOID)USBH_BSP_ISR_Handler,                        /*  94, USBH_HS_USBHSR                                  */
#endif

#else
    (CPU_FNCT_VOID)BSP_IntHandler_094,                          /*      94, ------------------------------------------- */
#endif
    (CPU_FNCT_VOID)BSP_IntHandler_095,                          /*  95, IWDT_IWUNI                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_096,                          /*  96, WDT_WUNI                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_097,                          /*  97, PDC_PCDFI                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_098,                          /*  98, SCI7_RXI7                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_099,                          /*  99, SCI7_TXI7                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_100,                          /* 100, SCIF8_RXIF8                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_101,                          /* 101, SCIF8_TXIF8                                     */
    (CPU_FNCT_VOID)sci9_rxi9_isr,                          		/* 102, SCIF9_RXIF9                                     */
    (CPU_FNCT_VOID)sci9_txi9_isr,                          		/* 103, SCIF9_TXIF9                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_104,                          /* 104, SCIF10_RXIF10                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_105,                          /* 105, SCIF10_TXIF10                                   */
#if BSP_CFG_CAN_EN > 0u
    (CPU_FNCT_VOID)RX600_CAN_NSHandler,                         /* 106, CAN[0-3] ERR INT = ICU_GROUPBE0                 */
#else
    (CPU_FNCT_VOID)BSP_IntHandler_106,                          /*      106, ------------------------------------------ */
#endif
    (CPU_FNCT_VOID)BSP_IntHandler_107,                          /* 107, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_108,                          /* 108, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_109,                          /* 109, Reserved                                        */

    (CPU_FNCT_VOID)group_bl0_handler_isr,                          /* 110, ICU_GROUPBL0                                    */
#if BSP_CFG_FS_EN > 0u
    (CPU_FNCT_VOID)FSDev_SD_Card_BSP_ISR,                       /* 111, ICU_GROUPBL1: SD MMC                            */
#else
    (CPU_FNCT_VOID)group_bl1_handler_isr,                          /*      111, ------------------------------------------ */
#endif
    (CPU_FNCT_VOID)BSP_IntHandler_112,                          /* 112, ICU_GROUPAL0                                    */

#if BSP_CFG_NET_EN > 0u
    (CPU_FNCT_VOID)NetDev_ISR,                                  /* 113, ICU_GROUPAL1                                    */
#else
    (CPU_FNCT_VOID)BSP_IntHandler_113,                          /*      113, ------------------------------------------ */
#endif
    (CPU_FNCT_VOID)BSP_IntHandler_114,                          /* 114, SCIF11_RXIF11                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_115,                          /* 115, SCIF11_TXIF11                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_116,                          /* 116, SCI12_RXI12                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_117,                          /* 117, SCI12_TXI12                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_118,                          /* 118, Reserved                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_119,                          /* 119, Reserved                                        */

    (CPU_FNCT_VOID)BSP_IntHandler_120,                          /* 120, DMAC_DMAC0I                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_121,                          /* 121, DMAC_DMAC1I                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_122,                          /* 122, DMAC_DMAC2I                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_123,                          /* 123, DMAC_DMAC3I                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_124,                          /* 124, DMAC_DMAC74I                                    */
    (CPU_FNCT_VOID)BSP_IntHandler_125,                          /* 125, OST_OST                                         */
    (CPU_FNCT_VOID)BSP_IntHandler_126,                          /* 126, EXDMAC_EXDMAC0I                                 */
    (CPU_FNCT_VOID)BSP_IntHandler_127,                          /* 127, EXDMAC_EXDMAC1I                                 */
                                                                /* -- "INTERRUPT B" SECTION INTERRUPTS. SEE NOTE (3) -- */
#if BSP_CFG_USBH_EN > 0u
    (CPU_FNCT_VOID)USBH_RX600_BSP_IntHandler,                   /* 128, USBH_USBI0  USBI0                               */
#else
    (CPU_FNCT_VOID)BSP_IntHandler_128,                          /* 128, PERIB: CMT2_CMI2                                */
#endif
    (CPU_FNCT_VOID)BSP_IntHandler_129,                          /* 129, PERIB: CMT3_CMI3                                */

    (CPU_FNCT_VOID)BSP_IntHandler_130,                          /* 130, PERIB: TMR0_CMIA0                               */
    (CPU_FNCT_VOID)BSP_IntHandler_131,                          /* 131, PERIB: TMR0_CMIB0                               */
    (CPU_FNCT_VOID)BSP_IntHandler_132,                          /* 132, PERIB: TMR0_OVI0                                */
    (CPU_FNCT_VOID)BSP_IntHandler_133,                          /* 133, PERIB: TMR1_CMIA1                               */
    (CPU_FNCT_VOID)BSP_IntHandler_134,                          /* 134, PERIB: TMR1_CMIB1                               */
    (CPU_FNCT_VOID)BSP_IntHandler_135,                          /* 135, PERIB: TMR1_OVI1                                */
    (CPU_FNCT_VOID)BSP_IntHandler_136,                          /* 136, PERIB: TMR2_CMIA2                               */
    (CPU_FNCT_VOID)BSP_IntHandler_137,                          /* 137, PERIB: TMR2_CMIB2                               */
    (CPU_FNCT_VOID)BSP_IntHandler_138,                          /* 138, PERIB: TMR2_OVI2                                */
    (CPU_FNCT_VOID)BSP_IntHandler_139,                          /* 139, PERIB: TMR3_CMIA3                               */

    (CPU_FNCT_VOID)BSP_IntHandler_140,                          /* 140, PERIB: TMR3_CMIB3                               */
    (CPU_FNCT_VOID)BSP_IntHandler_141,                          /* 141, PERIB: TMR3_OVI3                                */
    (CPU_FNCT_VOID)BSP_IntHandler_142,                          /* 142, PERIB: TPU0_TGI0A                               */
    (CPU_FNCT_VOID)BSP_IntHandler_143,                          /* 143, PERIB: TPU0_TGI0B                               */
    (CPU_FNCT_VOID)BSP_IntHandler_144,                          /* 144, PERIB: TPU0_TGI0C                               */
    (CPU_FNCT_VOID)BSP_IntHandler_145,                          /* 145, PERIB: TPU0_TGI0D                               */
    (CPU_FNCT_VOID)BSP_IntHandler_146,                          /* 146, PERIB: TPU0_TCI0V                               */
    (CPU_FNCT_VOID)BSP_IntHandler_147,                          /* 147, PERIB: TPU1_TGI1A                               */
    (CPU_FNCT_VOID)BSP_IntHandler_148,                          /* 148, PERIB: TPU1_TGI1B                               */
    (CPU_FNCT_VOID)BSP_IntHandler_149,                          /* 149, PERIB: TPU1_TCI1V                               */

    (CPU_FNCT_VOID)BSP_IntHandler_150,                          /* 150, PERIB: TPU1_TCI1U                               */
    (CPU_FNCT_VOID)BSP_IntHandler_151,                          /* 151, PERIB: TPU2_TGI2A                               */
    (CPU_FNCT_VOID)BSP_IntHandler_152,                          /* 152, PERIB: TPU2_TGI2B                               */
    (CPU_FNCT_VOID)BSP_IntHandler_153,                          /* 153, PERIB: TPU2_TCI2V                               */
    (CPU_FNCT_VOID)BSP_IntHandler_154,                          /* 154, PERIB: TPU2_TCI2U                               */
    (CPU_FNCT_VOID)BSP_IntHandler_155,                          /* 155, PERIB: TPU3_TGI3A                               */
    (CPU_FNCT_VOID)BSP_IntHandler_156,                          /* 156, PERIB: TPU3_TGI3B                               */
    (CPU_FNCT_VOID)BSP_IntHandler_157,                          /* 157, PERIB: TPU3_TGI3C                               */
    (CPU_FNCT_VOID)BSP_IntHandler_158,                          /* 158, PERIB: TPU3_TGI3D                               */
    (CPU_FNCT_VOID)BSP_IntHandler_159,                          /* 159, PERIB: TPU3_TCI3V                               */

    (CPU_FNCT_VOID)BSP_IntHandler_160,                          /* 160, PERIB: TPU4_TGI4A                               */
    (CPU_FNCT_VOID)BSP_IntHandler_161,                          /* 161, PERIB: TPU4_TGI4B                               */
    (CPU_FNCT_VOID)BSP_IntHandler_162,                          /* 162, PERIB: TPU4_TCI4V                               */
    (CPU_FNCT_VOID)BSP_IntHandler_163,                          /* 163, PERIB: TPU4_TCI4U                               */
    (CPU_FNCT_VOID)BSP_IntHandler_164,                          /* 164, PERIB: TPU5_TGI5A                               */
    (CPU_FNCT_VOID)BSP_IntHandler_165,                          /* 165, PERIB: TPU5_TGI5B                               */
    (CPU_FNCT_VOID)BSP_IntHandler_166,                          /* 166, PERIB: TPU5_TCI5V                               */
    (CPU_FNCT_VOID)BSP_IntHandler_167,                          /* 167, PERIB: TPU5_TCI5U                               */
    (CPU_FNCT_VOID)BSP_IntHandler_168,                          /* 168, PERIB: CMTW0_IC0I0                              */
    (CPU_FNCT_VOID)BSP_IntHandler_169,                          /* 169, PERIB: CMTW0_IC1I0                              */

    (CPU_FNCT_VOID)BSP_IntHandler_170,                          /* 170, PERIB: CMTW0_OC0I0                              */
    (CPU_FNCT_VOID)BSP_IntHandler_171,                          /* 171, PERIB: CMTW0_OC1I0                              */
    (CPU_FNCT_VOID)BSP_IntHandler_172,                          /* 172, PERIB: CMTW1_IC0I1                              */
    (CPU_FNCT_VOID)BSP_IntHandler_173,                          /* 173, PERIB: CMTW1_IC1I1                              */
    (CPU_FNCT_VOID)BSP_IntHandler_174,                          /* 174, PERIB: CMTW1_OC1I0                              */
    (CPU_FNCT_VOID)BSP_IntHandler_175,                          /* 175, PERIB: CMTW1_OC1I1                              */
    (CPU_FNCT_VOID)BSP_IntHandler_176,                          /* 176, PERIB: RTC_CUP                                  */
#if BSP_CFG_CAN_EN > 0u
    (CPU_FNCT_VOID)RX600_CAN0_RxHandler,                        /* 177, PERIB: CAN0_RXF0                                */
    (CPU_FNCT_VOID)RX600_CAN0_TxHandler,                        /* 178, PERIB: CAN0_TXF0                                */
    (CPU_FNCT_VOID)RX600_CAN0_RxHandler,                        /* 179, PERIB: CAN0_RXM0                                */
    (CPU_FNCT_VOID)RX600_CAN0_TxHandler,                        /* 180, PERIB: CAN0_TXM0                                */
#else
    (CPU_FNCT_VOID)BSP_IntHandler_177,                          /*      177, ------------------------------------------ */
    (CPU_FNCT_VOID)BSP_IntHandler_178,                          /*      178, ------------------------------------------ */
    (CPU_FNCT_VOID)BSP_IntHandler_179,                          /*      179, ------------------------------------------ */
    (CPU_FNCT_VOID)BSP_IntHandler_180,                          /*      180, ------------------------------------------ */
#endif
    (CPU_FNCT_VOID)BSP_IntHandler_181,                          /* 181, PERIB: CAN1_RXF1                                */
    (CPU_FNCT_VOID)BSP_IntHandler_182,                          /* 182, PERIB: CAN1_TXF1                                */
    (CPU_FNCT_VOID)BSP_IntHandler_183,                          /* 183, PERIB: CAN1_RXM1                                */
    (CPU_FNCT_VOID)BSP_IntHandler_184,                          /* 184, PERIB: CAN1_TXM1                                */
    (CPU_FNCT_VOID)BSP_IntHandler_185,                          /* 185, PERIB: CAN2_RXF2                                */
    (CPU_FNCT_VOID)BSP_IntHandler_186,                          /* 186, PERIB: CAN2_TXF2                                */
    (CPU_FNCT_VOID)BSP_IntHandler_187,                          /* 187, PERIB: CAN2_RXM2                                */
    (CPU_FNCT_VOID)BSP_IntHandler_188,                          /* 188, PERIB: CAN2_TXM2                                */
    (CPU_FNCT_VOID)BSP_IntHandler_189,                          /* 189, PERIB: USB0_USBI0                               */

    (CPU_FNCT_VOID)BSP_IntHandler_190,                          /* 190, PERIB: S12AD_S12ADI                             */
    (CPU_FNCT_VOID)BSP_IntHandler_191,                          /* 191, PERIB: S12AD_S12GBADI1                          */
    (CPU_FNCT_VOID)BSP_IntHandler_192,                          /* 192, PERIB: S12AD1_S12ADI1                           */
    (CPU_FNCT_VOID)BSP_IntHandler_193,                          /* 193, PERIB: S12AD1_S12GBADI1                         */
    (CPU_FNCT_VOID)BSP_IntHandler_194,                          /* 194, PERIB: DES_DESEND                               */
    (CPU_FNCT_VOID)BSP_IntHandler_195,                          /* 195, PERIB: SHA_SHADEND                              */
    (CPU_FNCT_VOID)BSP_IntHandler_196,                          /* 196, PERIB: SHA_SHAEND                               */
    (CPU_FNCT_VOID)BSP_IntHandler_197,                          /* 197, PERIB: RNG_RNGEND                               */
    (CPU_FNCT_VOID)BSP_IntHandler_198,                          /* 198, PERIB: ELC_ELSR18I                              */
    (CPU_FNCT_VOID)BSP_IntHandler_199,                          /* 199, PERIB: ELC_ELSR19I                              */

    (CPU_FNCT_VOID)BSP_IntHandler_200,                          /* 200, PERIB: INTB200                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_201,                          /* 201, PERIB: INTB201                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_202,                          /* 202, PERIB: INTB202                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_203,                          /* 203, PERIB: INTB203                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_204,                          /* 204, PERIB: INTB204                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_205,                          /* 205, PERIB: INTB205                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_206,                          /* 206, PERIB: INTB206                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_207,                          /* 207, PERIB: INTB207                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_208,                          /* 208, PERIB: INTB208                                  */
                                                                /* -- "INTERRUPT A" SECTION INTERRUPTS. SEE NOTE (4) -- */
    (CPU_FNCT_VOID)BSP_IntHandler_209,                          /* 209, PERIA: INTA209                                  */

    (CPU_FNCT_VOID)BSP_IntHandler_210,                          /* 210, PERIA: INTA210                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_211,                          /* 211, PERIA: INTA211                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_212,                          /* 212, PERIA: INTA212                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_213,                          /* 213, PERIA: INTA213                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_214,                          /* 214, PERIA: INTA214                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_215,                          /* 215, PERIA: INTA215                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_216,                          /* 216, PERIA: INTA216                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_217,                          /* 217, PERIA: INTA217                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_218,                          /* 218, PERIA: INTA218                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_219,                          /* 219, PERIA: INTA219                                  */

    (CPU_FNCT_VOID)BSP_IntHandler_220,                          /* 220, PERIA: INTA220                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_221,                          /* 221, PERIA: INTA221                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_222,                          /* 222, PERIA: INTA222                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_223,                          /* 223, PERIA: INTA223                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_224,                          /* 224, PERIA: INTA224                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_225,                          /* 225, PERIA: INTA225                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_226,                          /* 226, PERIA: INTA226                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_227,                          /* 227, PERIA: INTA227                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_228,                          /* 228, PERIA: INTA228                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_229,                          /* 229, PERIA: INTA229                                  */

    (CPU_FNCT_VOID)BSP_IntHandler_230,                          /* 230, PERIA: INTA230                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_231,                          /* 231, PERIA: INTA231                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_232,                          /* 232, PERIA: INTA232                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_233,                          /* 233, PERIA: INTA233                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_234,                          /* 234, PERIA: INTA234                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_235,                          /* 235, PERIA: INTA235                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_236,                          /* 236, PERIA: INTA236                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_237,                          /* 237, PERIA: INTA237                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_238,                          /* 238, PERIA: INTA238                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_239,                          /* 239, PERIA: INTA239                                  */

    (CPU_FNCT_VOID)BSP_IntHandler_240,                          /* 240, PERIA: INTA240                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_241,                          /* 241, PERIA: INTA241                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_242,                          /* 242, PERIA: INTA242                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_243,                          /* 243, PERIA: INTA243                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_244,                          /* 244, PERIA: INTA244                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_245,                          /* 245, PERIA: INTA245                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_246,                          /* 246, PERIA: INTA246                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_247,                          /* 247, PERIA: INTA247                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_248,                          /* 248, PERIA: INTA248                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_249,                          /* 249, PERIA: INTA249                                  */

    (CPU_FNCT_VOID)BSP_IntHandler_250,                          /* 250, PERIA: INTA250                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_251,                          /* 251, PERIA: INTA251                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_252,                          /* 252, PERIA: INTA252                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_253,                          /* 253, PERIA: INTA253                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_254,                          /* 254, PERIA: INTA254                                  */
    (CPU_FNCT_VOID)BSP_IntHandler_255,                          /* 255, PERIA: INTA255                                  */
};


/*
*********************************************************************************************************
*                                            BSP_IntVectSet()
*
* Description : Assign ISR handler.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               isr         Handler to assign.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void   BSP_IntVectSet          (uint16_t     int_id,
                                CPU_FNCT_VOID  isr)
{
#if  BSP_CFG_INT_VECT_TBL_RAM_EN > 0u
    CPSR_ALLOC();


    CPU_INT_VECT_TBL_BASE_SET((uint32_t)&BSP_IntVectTbl[0]);
    if (int_id < BSP_CFG_INT_VECT_TBL_SIZE) {
        RHINO_CPU_INTRPT_DISABLE();
        BSP_IntVectTbl[int_id] = isr;
        RHINO_CPU_INTRPT_ENABLE();
    }
#else
    (void)&int_id;
    (void)&isr;
    CPU_INT_VECT_TBL_BASE_SET((CPU_INT32U)&BSP_IntVectTbl[0]);
#endif
}


/*
*********************************************************************************************************
*                                          BSP_IntVectTblInit()
*
* Description : Sets up the interrupt vector table base address of the CPU.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntVectTblInit (void)
{
    CPU_INT_VECT_TBL_BASE_SET((uint32_t)&BSP_IntVectTbl[0u]);
}


/*
*********************************************************************************************************
*                                        BSP_IntHandler_xxx()
*
* Description : Dummy interrupt handlers.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntHandler_000 (void)
{
    while (1u);
}


void  BSP_IntHandler_001 (void)
{
    while (1u);
}


void  BSP_IntHandler_002 (void)
{
    while (1u);
}


void  BSP_IntHandler_003 (void)
{
    while (1u);
}


void  BSP_IntHandler_004 (void)
{
    while (1u);
}


void  BSP_IntHandler_005 (void)
{
    while (1u);
}


void  BSP_IntHandler_006 (void)
{
    while (1u);
}


void  BSP_IntHandler_007 (void)
{
    while (1u);
}


void  BSP_IntHandler_008 (void)
{
    while (1u);
}


void  BSP_IntHandler_009 (void)
{
    while (1u);
}


void  BSP_IntHandler_010 (void)
{
    while (1u);
}


void  BSP_IntHandler_011 (void)
{
    while (1u);
}


void  BSP_IntHandler_012 (void)
{
    while (1u);
}


void  BSP_IntHandler_013 (void)
{
    while (1u);
}


void  BSP_IntHandler_014 (void)
{
    while (1u);
}


void  BSP_IntHandler_015 (void)
{
    while (1u);
}


void  BSP_IntHandler_016 (void)
{
    while (1u);
}


void  BSP_IntHandler_017 (void)
{
    while (1u);
}


void  BSP_IntHandler_018 (void)
{
    while (1u);
}


void  BSP_IntHandler_019 (void)
{
    while (1u);
}


void  BSP_IntHandler_020 (void)
{
    while (1u);
}


void  BSP_IntHandler_021 (void)
{
    while (1u);
}


void  BSP_IntHandler_022 (void)
{
    while (1u);
}


void  BSP_IntHandler_023 (void)
{
    while (1u);
}


void  BSP_IntHandler_024 (void)
{
    while (1u);
}


void  BSP_IntHandler_025 (void)
{
    while (1u);
}


void  BSP_IntHandler_026 (void)
{
    while (1u);
}


void  BSP_IntHandler_027 (void)
{
    while (1u);
}


void  BSP_IntHandler_028 (void)
{
    while (1u);
}


void  BSP_IntHandler_029 (void)
{
    while (1u);
}


void  BSP_IntHandler_030 (void)
{
    while (1u);
}


void  BSP_IntHandler_031 (void)
{
    while (1u);
}


void  BSP_IntHandler_032 (void)
{
    while (1u);
}


void  BSP_IntHandler_033 (void)
{
    while (1u);
}


void  BSP_IntHandler_034 (void)
{
    while (1u);
}


void  BSP_IntHandler_035 (void)
{
    while (1u);
}


void  BSP_IntHandler_036 (void)
{
    while (1u);
}


void  BSP_IntHandler_037 (void)
{
    while (1u);
}


void  BSP_IntHandler_038 (void)
{
    while (1u);
}


void  BSP_IntHandler_039 (void)
{
    while (1u);
}


void  BSP_IntHandler_040 (void)
{
    while (1u);
}


void  BSP_IntHandler_041 (void)
{
    while (1u);
}


void  BSP_IntHandler_042 (void)
{
    while (1u);
}


void  BSP_IntHandler_043 (void)
{
    while (1u);
}


void  BSP_IntHandler_044 (void)
{
    while (1u);
}


void  BSP_IntHandler_045 (void)
{
    while (1u);
}


void  BSP_IntHandler_046 (void)
{
    while (1u);
}


void  BSP_IntHandler_047 (void)
{
    while (1u);
}


void  BSP_IntHandler_048 (void)
{
    while (1u);
}


void  BSP_IntHandler_049 (void)
{
    while (1u);
}


void  BSP_IntHandler_050 (void)
{
    while (1u);
}


void  BSP_IntHandler_051 (void)
{
    while (1u);
}


void  BSP_IntHandler_052 (void)
{
    while (1u);
}


void  BSP_IntHandler_053 (void)
{
    while (1u);
}


void  BSP_IntHandler_054 (void)
{
    while (1u);
}


void  BSP_IntHandler_055 (void)
{
    while (1u);
}


void  BSP_IntHandler_056 (void)
{
    while (1u);
}


void  BSP_IntHandler_057 (void)
{
    while (1u);
}


void  BSP_IntHandler_058 (void)
{
    while (1u);
}


void  BSP_IntHandler_059 (void)
{
    while (1u);
}


void  BSP_IntHandler_060 (void)
{
    while (1u);
}


void  BSP_IntHandler_061 (void)
{
    while (1u);
}


void  BSP_IntHandler_062 (void)
{
    while (1u);
}


void  BSP_IntHandler_063 (void)
{
    while (1u);
}


void  BSP_IntHandler_064 (void)
{
    while (1u);
}


void  BSP_IntHandler_065 (void)
{
    while (1u);
}


void  BSP_IntHandler_066 (void)
{
    while (1u);
}


void  BSP_IntHandler_067 (void)
{
    while (1u);
}


void  BSP_IntHandler_068 (void)
{
    while (1u);
}


void  BSP_IntHandler_069 (void)
{
    while (1u);
}


void  BSP_IntHandler_070 (void)
{
    while (1u);
}


void  BSP_IntHandler_071 (void)
{
    while (1u);
}


void  BSP_IntHandler_072 (void)
{
    while (1u);
}


void  BSP_IntHandler_073 (void)
{
    while (1u);
}


void  BSP_IntHandler_074 (void)
{
    while (1u);
}


void  BSP_IntHandler_075 (void)
{
    while (1u);
}


void  BSP_IntHandler_076 (void)
{
    while (1u);
}


void  BSP_IntHandler_077 (void)
{
    while (1u);
}


void  BSP_IntHandler_078 (void)
{
    while (1u);
}


void  BSP_IntHandler_079 (void)
{
    while (1u);
}


void  BSP_IntHandler_080 (void)
{
    while (1u);
}


void  BSP_IntHandler_081 (void)
{
    while (1u);
}


void  BSP_IntHandler_082 (void)
{
    while (1u);
}


void  BSP_IntHandler_083 (void)
{
    while (1u);
}


void  BSP_IntHandler_084 (void)
{
    while (1u);
}


void  BSP_IntHandler_085 (void)
{
    //while (1u);
	txi_handler(&ch5_ctrl);
}


void  BSP_IntHandler_086 (void)
{
    while (1u);
}


void  BSP_IntHandler_087 (void)
{
    while (1u);
}


void  BSP_IntHandler_088 (void)
{
    while (1u);
}


void  BSP_IntHandler_089 (void)
{
    while (1u);
}


void  BSP_IntHandler_090 (void)
{
    while (1u);
}


void  BSP_IntHandler_091 (void)
{
    while (1u);
}


void  BSP_IntHandler_092 (void)
{
    while (1u);
}


void  BSP_IntHandler_093 (void)
{
    while (1u);
}


void  BSP_IntHandler_094 (void)
{
    while (1u);
}


void  BSP_IntHandler_095 (void)
{
    while (1u);
}


void  BSP_IntHandler_096 (void)
{
    while (1u);
}


void  BSP_IntHandler_097 (void)
{
    while (1u);
}


void  BSP_IntHandler_098 (void)
{
    while (1u);
}


void  BSP_IntHandler_099 (void)
{
    while (1u);
}


void  BSP_IntHandler_100 (void)
{
    while (1u);
}


void  BSP_IntHandler_101 (void)
{
    while (1u);
}


void  BSP_IntHandler_102 (void)
{
    while (1u);
}


void  BSP_IntHandler_103 (void)
{
    while (1u);
}


void  BSP_IntHandler_104 (void)
{
    while (1u);
}


void  BSP_IntHandler_105 (void)
{
    while (1u);
}


void  BSP_IntHandler_106 (void)
{
    while (1u);
}


void  BSP_IntHandler_107 (void)
{
    while (1u);
}


void  BSP_IntHandler_108 (void)
{
    while (1u);
}


void  BSP_IntHandler_109 (void)
{
    while (1u);
}


void  BSP_IntHandler_110 (void)
{
    while (1u);
}


void  BSP_IntHandler_111 (void)
{
    while (1u);
}


void  BSP_IntHandler_112 (void)
{
    while (1u);
}


void  BSP_IntHandler_113 (void)
{
    while (1u);
}


void  BSP_IntHandler_114 (void)
{
    while (1u);
}


void  BSP_IntHandler_115 (void)
{
    while (1u);
}


void  BSP_IntHandler_116 (void)
{
    while (1u);
}


void  BSP_IntHandler_117 (void)
{
    while (1u);
}


void  BSP_IntHandler_118 (void)
{
    while (1u);
}


void  BSP_IntHandler_119 (void)
{
    while (1u);
}


void  BSP_IntHandler_120 (void)
{
    while (1u);
}


void  BSP_IntHandler_121 (void)
{
    while (1u);
}


void  BSP_IntHandler_122 (void)
{
    while (1u);
}


void  BSP_IntHandler_123 (void)
{
    while (1u);
}


void  BSP_IntHandler_124 (void)
{
    while (1u);
}


void  BSP_IntHandler_125 (void)
{
    while (1u);
}


void  BSP_IntHandler_126 (void)
{
    while (1u);
}


void  BSP_IntHandler_127 (void)
{
    while (1u);
}


void  BSP_IntHandler_128 (void)
{
    while (1u);
}


void  BSP_IntHandler_129 (void)
{
    while (1u);
}


void  BSP_IntHandler_130 (void)
{
    while (1u);
}


void  BSP_IntHandler_131 (void)
{
    while (1u);
}


void  BSP_IntHandler_132 (void)
{
    while (1u);
}


void  BSP_IntHandler_133 (void)
{
    while (1u);
}


void  BSP_IntHandler_134 (void)
{
    while (1u);
}


void  BSP_IntHandler_135 (void)
{
    while (1u);
}


void  BSP_IntHandler_136 (void)
{
    while (1u);
}


void  BSP_IntHandler_137 (void)
{
    while (1u);
}


void  BSP_IntHandler_138 (void)
{
    while (1u);
}


void  BSP_IntHandler_139 (void)
{
    while (1u);
}


void  BSP_IntHandler_140 (void)
{
    while (1u);
}


void  BSP_IntHandler_141 (void)
{
    while (1u);
}


void  BSP_IntHandler_142 (void)
{
    while (1u);
}


void  BSP_IntHandler_143 (void)
{
    while (1u);
}


void  BSP_IntHandler_144 (void)
{
    while (1u);
}


void  BSP_IntHandler_145 (void)
{
    while (1u);
}


void  BSP_IntHandler_146 (void)
{
    while (1u);
}


void  BSP_IntHandler_147 (void)
{
    while (1u);
}


void  BSP_IntHandler_148 (void)
{
    while (1u);
}


void  BSP_IntHandler_149 (void)
{
    while (1u);
}


void  BSP_IntHandler_150 (void)
{
    while (1u);
}


void  BSP_IntHandler_151 (void)
{
    while (1u);
}


void  BSP_IntHandler_152 (void)
{
    while (1u);
}


void  BSP_IntHandler_153 (void)
{
    while (1u);
}


void  BSP_IntHandler_154 (void)
{
    while (1u);
}


void  BSP_IntHandler_155 (void)
{
    while (1u);
}


void  BSP_IntHandler_156 (void)
{
    while (1u);
}


void  BSP_IntHandler_157 (void)
{
    while (1u);
}


void  BSP_IntHandler_158 (void)
{
    while (1u);
}


void  BSP_IntHandler_159 (void)
{
    while (1u);
}


void  BSP_IntHandler_160 (void)
{
    while (1u);
}


void  BSP_IntHandler_161 (void)
{
    while (1u);
}


void  BSP_IntHandler_162 (void)
{
    while (1u);
}


void  BSP_IntHandler_163 (void)
{
    while (1u);
}


void  BSP_IntHandler_164 (void)
{
    while (1u);
}


void  BSP_IntHandler_165 (void)
{
    while (1u);
}


void  BSP_IntHandler_166 (void)
{
    while (1u);
}


void  BSP_IntHandler_167 (void)
{
    while (1u);
}


void  BSP_IntHandler_168 (void)
{
    while (1u);
}


void  BSP_IntHandler_169 (void)
{
    while (1u);
}


void  BSP_IntHandler_170 (void)
{
    while (1u);
}


void  BSP_IntHandler_171 (void)
{
    while (1u);
}


void  BSP_IntHandler_172 (void)
{
    while (1u);
}


void  BSP_IntHandler_173 (void)
{
    while (1u);
}


void  BSP_IntHandler_174 (void)
{
    while (1u);
}


void  BSP_IntHandler_175 (void)
{
    while (1u);
}


void  BSP_IntHandler_176 (void)
{
    while (1u);
}


void  BSP_IntHandler_177 (void)
{
    while (1u);
}


void  BSP_IntHandler_178 (void)
{
    while (1u);
}


void  BSP_IntHandler_179 (void)
{
    while (1u);
}


void  BSP_IntHandler_180 (void)
{
    while (1u);
}


void  BSP_IntHandler_181 (void)
{
    while (1u);
}


void  BSP_IntHandler_182 (void)
{
    while (1u);
}


void  BSP_IntHandler_183 (void)
{
    while (1u);
}


void  BSP_IntHandler_184 (void)
{
    while (1u);
}


void  BSP_IntHandler_185 (void)
{
    while (1u);
}


void  BSP_IntHandler_186 (void)
{
    while (1u);
}


void  BSP_IntHandler_187 (void)
{
    while (1u);
}


void  BSP_IntHandler_188 (void)
{
    while (1u);
}


void  BSP_IntHandler_189 (void)
{
    while (1u);
}


void  BSP_IntHandler_190 (void)
{
    while (1u);
}


void  BSP_IntHandler_191 (void)
{
    while (1u);
}


void  BSP_IntHandler_192 (void)
{
    while (1u);
}


void  BSP_IntHandler_193 (void)
{
    while (1u);
}


void  BSP_IntHandler_194 (void)
{
    while (1u);
}


void  BSP_IntHandler_195 (void)
{
    while (1u);
}


void  BSP_IntHandler_196 (void)
{
    while (1u);
}


void  BSP_IntHandler_197 (void)
{
    while (1u);
}


void  BSP_IntHandler_198 (void)
{
    while (1u);
}


void  BSP_IntHandler_199 (void)
{
    while (1u);
}


void  BSP_IntHandler_200 (void)
{
    while (1u);
}


void  BSP_IntHandler_201 (void)
{
    while (1u);
}


void  BSP_IntHandler_202 (void)
{
    while (1u);
}


void  BSP_IntHandler_203 (void)
{
    while (1u);
}


void  BSP_IntHandler_204 (void)
{
    while (1u);
}


void  BSP_IntHandler_205 (void)
{
    while (1u);
}


void  BSP_IntHandler_206 (void)
{
    while (1u);
}


void  BSP_IntHandler_207 (void)
{
    while (1u);
}


void  BSP_IntHandler_208 (void)
{
    while (1u);
}


void  BSP_IntHandler_209 (void)
{
    while (1u);
}


void  BSP_IntHandler_210 (void)
{
    while (1u);
}


void  BSP_IntHandler_211 (void)
{
    while (1u);
}


void  BSP_IntHandler_212 (void)
{
    while (1u);
}


void  BSP_IntHandler_213 (void)
{
    while (1u);
}


void  BSP_IntHandler_214 (void)
{
    while (1u);
}


void  BSP_IntHandler_215 (void)
{
    while (1u);
}


void  BSP_IntHandler_216 (void)
{
    while (1u);
}


void  BSP_IntHandler_217 (void)
{
    while (1u);
}


void  BSP_IntHandler_218 (void)
{
    while (1u);
}


void  BSP_IntHandler_219 (void)
{
    while (1u);
}


void  BSP_IntHandler_220 (void)
{
    while (1u);
}


void  BSP_IntHandler_221 (void)
{
    while (1u);
}


void  BSP_IntHandler_222 (void)
{
    while (1u);
}


void  BSP_IntHandler_223 (void)
{
    while (1u);
}


void  BSP_IntHandler_224 (void)
{
    while (1u);
}


void  BSP_IntHandler_225 (void)
{
    while (1u);
}


void  BSP_IntHandler_226 (void)
{
    while (1u);
}


void  BSP_IntHandler_227 (void)
{
    while (1u);
}


void  BSP_IntHandler_228 (void)
{
    while (1u);
}


void  BSP_IntHandler_229 (void)
{
    while (1u);
}


void  BSP_IntHandler_230 (void)
{
    while (1u);
}


void  BSP_IntHandler_231 (void)
{
    while (1u);
}


void  BSP_IntHandler_232 (void)
{
    while (1u);
}


void  BSP_IntHandler_233 (void)
{
    while (1u);
}


void  BSP_IntHandler_234 (void)
{
    while (1u);
}


void  BSP_IntHandler_235 (void)
{
    while (1u);
}


void  BSP_IntHandler_236 (void)
{
    while (1u);
}


void  BSP_IntHandler_237 (void)
{
    while (1u);
}


void  BSP_IntHandler_238 (void)
{
    while (1u);
}


void  BSP_IntHandler_239 (void)
{
    while (1u);
}


void  BSP_IntHandler_240 (void)
{
    while (1u);
}


void  BSP_IntHandler_241 (void)
{
    while (1u);
}


void  BSP_IntHandler_242 (void)
{
    while (1u);
}


void  BSP_IntHandler_243 (void)
{
    while (1u);
}


void  BSP_IntHandler_244 (void)
{
    while (1u);
}


void  BSP_IntHandler_245 (void)
{
    while (1u);
}


void  BSP_IntHandler_246 (void)
{
    while (1u);
}


void  BSP_IntHandler_247 (void)
{
    while (1u);
}


void  BSP_IntHandler_248 (void)
{
    while (1u);
}


void  BSP_IntHandler_249 (void)
{
    while (1u);
}


void  BSP_IntHandler_250 (void)
{
    while (1u);
}


void  BSP_IntHandler_251 (void)
{
    while (1u);
}


void  BSP_IntHandler_252 (void)
{
    while (1u);
}


void  BSP_IntHandler_253 (void)
{
    while (1u);
}


void  BSP_IntHandler_254 (void)
{
    while (1u);
}


void  BSP_IntHandler_255 (void)
{
    while (1u);
}
