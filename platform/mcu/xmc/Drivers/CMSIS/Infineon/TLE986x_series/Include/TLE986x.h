
/****************************************************************************************************//**
 * @file     TLE986x.h
 *
 * @brief    CMSIS Cortex-M3 Peripheral Access Layer Header File for
 *           Infineon Technologies ePower Family TLE986x
 *
 * @version  V2.10
 * @date     14. Jan 2016
 *
 * @note     Generated with SVDConv V2.78b 
 *
 * @note:    Generated for MISRA Compliance, this File is NOT CMSIS compliant.
 *******************************************************************************************************/



/** @addtogroup (null)
  * @{
  */

/** @addtogroup TLE986x
  * @{
  */

#ifndef TLE986x_H
#define TLE986x_H

#include <tle_device.h>

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
  Define Memory
 *----------------------------------------------------------------------------*/
#define ProgFlashStart  (0x11000000U)      /* Start Address of the flash      */
#define DataFlashSize   (0x1000U)          /* 4KB Data Flash                  */
#define DataFlashStart  (ProgFlashStart + ProgFlashSize)
#define NACStart        (DataFlashStart - 4U)
#define NADStart        (DataFlashStart - 2U)
#define RAMStart        (0x18000000UL)     /* Start Address of the SRAM       */

/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum {
/* -------------------  Cortex-M3 Processor Exceptions Numbers  ------------------- */
  Reset_IRQn                    = -15,              /*!<   1  Reset Vector, invoked on Power up and warm reset                 */
  NonMaskableInt_IRQn           = -14,              /*!<   2  Non maskable Interrupt, cannot be stopped or preempted           */
  HardFault_IRQn                = -13,              /*!<   3  Hard Fault, all classes of Fault                                 */
  MemoryManagement_IRQn         = -12,              /*!<   4  Memory Management, MPU mismatch, including Access Violation
                                                         and No Match                                                          */
  BusFault_IRQn                 = -11,              /*!<   5  Bus Fault, Pre-Fetch-, Memory Access Fault, other address/memory
                                                         related Fault                                                         */
  UsageFault_IRQn               = -10,              /*!<   6  Usage Fault, i.e. Undef Instruction, Illegal State Transition    */
  SVCall_IRQn                   =  -5,              /*!<  11  System Service Call via SVC instruction                          */
  DebugMonitor_IRQn             =  -4,              /*!<  12  Debug Monitor                                                    */
  PendSV_IRQn                   =  -2,              /*!<  14  Pendable request for system service                              */
  SysTick_IRQn                  =  -1,              /*!<  15  System Tick Timer                                                */
/* --------------------  TLE986x Specific Interrupt Numbers  -------------------- */
  GPT1_Int                      = 0,                /*!<  Interrupt node 0: GPT1 Block             */
  GPT2_Int                      = 1,                /*!<  Interrupt node 1: GPT2 Block             */
  ADC2_Tmr3_Int                 = 2,                /*!<  Interrupt node 2: ADC2, Timer3, BEMF     */
  ADC1_VREF5_Int                = 3,                /*!<  Interrupt node 3: ADC1, VREF5V           */
  CCU6_SR0_Int                  = 4,                /*!<  Interrupt node 4: CCU6 node0             */
  CCU6_SR1_Int                  = 5,                /*!<  Interrupt node 5: CCU6 node1             */
  CCU6_SR2_Int                  = 6,                /*!<  Interrupt node 6: CCU6 node2             */
  CCU6_SR3_Int                  = 7,                /*!<  Interrupt node 7: CCU6 node3             */
  SSC1_Int                      = 8,                /*!<  Interrupt node 8: SSC1                   */
  SSC2_Int                      = 9,                /*!<  Interrupt node 9: SSC2                   */
  UART1_LIN_Tmr2_Int            =10,                /*!<  Interrupt node10: UART1(ASC,LIN), Timer2 */
  UART2_Tmr21_EINT2_Int         =11,                /*!<  Interrupt node11: UART2, Timer21, EINT2  */
  EXINT0_MON_Int                =12,                /*!<  Interrupt node12: EINT0, MON             */
  EXINT1_Int                    =13,                /*!<  Interrupt node13: EINT1                  */
  BDRV_CP_Int                   =14,                /*!<  Interrupt node14: BDRV, Charge Pump      */
  DMA_Int                       =15,                /*!<  Interrupt node15: DMA                    */
} IRQn_Type;


/** @addtogroup Configuration_of_CMSIS
  * @{
  */


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* ----------------Configuration of the Cortex-M3 Processor and Core Peripherals---------------- */
#define __CM3_REV                 0x0000            /*!< Cortex-M3 Core Revision                                               */
#define __MPU_PRESENT                  0            /*!< MPU present or not                                                    */
#define __NVIC_PRIO_BITS               4            /*!< Number of Bits used for Priority Levels                               */
#define __Vendor_SysTickConfig         0            /*!< Set to 1 if different SysTick Config is used                          */
/** @} */ /* End of group Configuration_of_CMSIS */

#include <core_cm3.h>                               /*!< Cortex-M3 processor and core peripherals                              */
#include "system_TLE986x.h"                         /*!< TLE9869 System                                                        */


/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */


/** @addtogroup Device_Peripheral_Registers
  * @{
  */
#if defined(__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  /* TBD. pragmas for anonymous unions */
#else
  #warning Not supported compiler type
#endif


/* ================================================================================ */
/* ================                       SCU                      ================ */
/* ================================================================================ */


/**
  * @brief SCU (SCU)
  */

typedef struct {                                    /*!< (@ 0x50005000) SCU Structure                                          */
  
  union {
    __O  uint8_t   reg;                             /*!< (@ 0x50005000) NMI Clear Register, RESET_TYPE_3                       */
    
    struct {
      __O  uint8_t   NMIWDTC    :  1;               /*!< [0..0] Watchdog Timer NMI Clear                                       */
      __O  uint8_t   NMIPLLC    :  1;               /*!< [1..1] PLL Loss of Lock NMI Clear                                     */
      __O  uint8_t   NMINVMC    :  1;               /*!< [2..2] NVM Operation Complete NMI Clear                               */
      __O  uint8_t   NMIOTC     :  1;               /*!< [3..3] NMI OT Clear                                                   */
      __O  uint8_t   NMIOWDC    :  1;               /*!< [4..4] Oscillator Watchdog NMI Clear                                  */
      __O  uint8_t   NMIMAPC    :  1;               /*!< [5..5] NVM Map Error NMI Clear                                        */
      __O  uint8_t   NMIECCC    :  1;               /*!< [6..6] ECC Error NMI Clear                                            */
      __O  uint8_t   NMISUPC    :  1;               /*!< [7..7] Supply Prewarning NMI Clear                                    */
    } bit;                                          /*!< [8] BitSize                                                           */
  } NMICLR;
  __I  uint8_t   RESERVED0[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x50005004) Interrupt Request Register 0, RESET_TYPE_3             */
    
    struct {
      __I  uint8_t   EXINT0R    :  1;               /*!< [0..0] Interrupt Flag for External Interrupt 0x on rising edge        */
      __I  uint8_t   EXINT0F    :  1;               /*!< [1..1] Interrupt Flag for External Interrupt 0x on falling edge       */
      __I  uint8_t   EXINT1R    :  1;               /*!< [2..2] Interrupt Flag for External Interrupt 1x on rising edge        */
      __I  uint8_t   EXINT1F    :  1;               /*!< [3..3] Interrupt Flag for External Interrupt 1x on falling edge       */
      __I  uint8_t   EXINT2R    :  1;               /*!< [4..4] Interrupt Flag for External Interrupt 2x on rising edge        */
      __I  uint8_t   EXINT2F    :  1;               /*!< [5..5] Interrupt Flag for External Interrupt 2x on falling edge       */
      __I  uint8_t   MONR       :  1;               /*!< [6..6] Interrupt Flag for External Interrupt MON on rising edge       */
      __I  uint8_t   MONF       :  1;               /*!< [7..7] Interrupt Flag for External Interrupt MON on falling
                                                         edge                                                                  */
    } bit;                                          /*!< [8] BitSize                                                           */
  } IRCON0;
  __I  uint8_t   RESERVED1[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x50005008) Interrupt Request Register 1, RESET_TYPE_3             */
    
    struct {
      __I  uint8_t   EIR        :  1;               /*!< [0..0] Error Interrupt Flag for SSC1                                  */
      __I  uint8_t   TIR        :  1;               /*!< [1..1] Transmit Interrupt Flag for SSC1                               */
      __I  uint8_t   RIR        :  1;               /*!< [2..2] Receive Interrupt Flag for SSC1                                */
    } bit;                                          /*!< [3] BitSize                                                           */
  } IRCON1;
  __I  uint8_t   RESERVED2[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x5000500C) Interrupt Request Register 2, RESET_TYPE_3             */
    
    struct {
      __I  uint8_t   EIR        :  1;               /*!< [0..0] Error Interrupt Flag for SSC2                                  */
      __I  uint8_t   TIR        :  1;               /*!< [1..1] Transmit Interrupt Flag for SSC2                               */
      __I  uint8_t   RIR        :  1;               /*!< [2..2] Receive Interrupt Flag for SSC2                                */
    } bit;                                          /*!< [3] BitSize                                                           */
  } IRCON2;
  __I  uint8_t   RESERVED3[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x50005010) Interrupt Request Register 3, RESET_TYPE_3             */
    
    struct {
      __I  uint8_t   CCU6SR0    :  1;               /*!< [0..0] Interrupt Flag 0 for CCU6                                      */
           uint8_t              :  3;
      __I  uint8_t   CCU6SR1    :  1;               /*!< [4..4] Interrupt Flag 1 for CCU6                                      */
    } bit;                                          /*!< [5] BitSize                                                           */
  } IRCON3;
  __I  uint8_t   RESERVED4[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x50005014) Interrupt Request Register 4, RESET_TYPE_3             */
    
    struct {
      __I  uint8_t   CCU6SR2    :  1;               /*!< [0..0] Interrupt Flag 2 for CCU6                                      */
           uint8_t              :  3;
      __I  uint8_t   CCU6SR3    :  1;               /*!< [4..4] Interrupt Flag 3 for CCU6                                      */
    } bit;                                          /*!< [5] BitSize                                                           */
  } IRCON4;
  __I  uint8_t   RESERVED5[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x50005018) NMI Status Register, RESET_TYPE_4                      */
    
    struct {
      __I  uint8_t   FNMIWDT    :  1;               /*!< [0..0] Watchdog Timer NMI Flag                                        */
      __I  uint8_t   FNMIPLL    :  1;               /*!< [1..1] PLL NMI Flag                                                   */
      __I  uint8_t   FNMINVM    :  1;               /*!< [2..2] NVM Operation Complete NMI Flag                                */
      __I  uint8_t   FNMIOT     :  1;               /*!< [3..3] Over-temperature NMI Flag                                      */
      __I  uint8_t   FNMIOWD    :  1;               /*!< [4..4] Oscillator Watchdog or MI_CLK Watchdog NMI Flag                */
      __I  uint8_t   FNMIMAP    :  1;               /*!< [5..5] NVM Map Error NMI Flag                                         */
      __I  uint8_t   FNMIECC    :  1;               /*!< [6..6] ECC Error NMI Flag                                             */
      __I  uint8_t   FNMISUP    :  1;               /*!< [7..7] Supply Prewarning NMI Flag                                     */
    } bit;                                          /*!< [8] BitSize                                                           */
  } NMISR;
  __I  uint8_t   RESERVED6[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x5000501C) Interrupt Enable Register 0, RESET_TYPE_4              */
    
    struct {
           uint8_t              :  7;
      __IO uint8_t   EA         :  1;               /*!< [7..7] Global Interrupt Mask                                          */
    } bit;                                          /*!< [8] BitSize                                                           */
  } IEN0;
  __I  uint8_t   RESERVED7[7];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005024) NMI Control Register, RESET_TYPE_4                     */
    
    struct {
      __IO uint8_t   NMIWDT     :  1;               /*!< [0..0] Watchdog Timer NMI Enable                                      */
      __IO uint8_t   NMIPLL     :  1;               /*!< [1..1] PLL Loss of Lock NMI Enable                                    */
      __IO uint8_t   NMINVM     :  1;               /*!< [2..2] NVM Operation Complete NMI Enable                              */
      __IO uint8_t   NMIOT      :  1;               /*!< [3..3] NMI OT Enable                                                  */
      __IO uint8_t   NMIOWD     :  1;               /*!< [4..4] Oscillator Watchdog NMI Enable                                 */
      __IO uint8_t   NMIMAP     :  1;               /*!< [5..5] NVM Map Error NMI Enable                                       */
      __IO uint8_t   NMIECC     :  1;               /*!< [6..6] ECC Error NMI Enable                                           */
      __IO uint8_t   NMISUP     :  1;               /*!< [7..7] Supply Prewarning NMI Enable                                   */
    } bit;                                          /*!< [8] BitSize                                                           */
  } NMICON;
  __I  uint8_t   RESERVED8[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005028) External Interrupt Control Register 0, RESET_TYPE_3    */
    
    struct {
      __IO uint8_t   EXINT0     :  2;               /*!< [0..1] External Interrupt 0 Trigger Select                            */
      __IO uint8_t   EXINT1     :  2;               /*!< [2..3] External Interrupt 1 Trigger Select                            */
      __IO uint8_t   EXINT2     :  2;               /*!< [4..5] External Interrupt 2 Trigger Select                            */
      __IO uint8_t   MON        :  2;               /*!< [6..7] MON Input Trigger Select                                       */
    } bit;                                          /*!< [8] BitSize                                                           */
  } EXICON0;
  __I  uint8_t   RESERVED9[3];
  
  union {
    __O  uint8_t   reg;                             /*!< (@ 0x5000502C) Interrupt Request 0 Clear Register, RESET_TYPE_3       */
    
    struct {
      __O  uint8_t   EXINT0RC   :  1;               /*!< [0..0] Interrupt Flag for External Interrupt 0x on rising edge
                                                         clear                                                                 */
      __O  uint8_t   EXINT0FC   :  1;               /*!< [1..1] Interrupt Flag for External Interrupt 0x on falling edge
                                                         clear                                                                 */
      __O  uint8_t   EXINT1RC   :  1;               /*!< [2..2] Interrupt Flag for External Interrupt 1x on rising edge
                                                         clear                                                                 */
      __O  uint8_t   EXINT1FC   :  1;               /*!< [3..3] Interrupt Flag for External Interrupt 1x on falling edge
                                                         clear                                                                 */
      __O  uint8_t   EXINT2RC   :  1;               /*!< [4..4] Interrupt Flag for External Interrupt 2x on rising edge
                                                         clear                                                                 */
      __O  uint8_t   EXINT2FC   :  1;               /*!< [5..5] Interrupt Flag for External Interrupt 2x on falling edge
                                                         clear                                                                 */
      __O  uint8_t   MONRC      :  1;               /*!< [6..6] Interrupt Flag for External Interrupt MON on rising edge
                                                         clear                                                                 */
      __O  uint8_t   MONFC      :  1;               /*!< [7..7] Interrupt Flag for External Interrupt MON on falling
                                                         edge clear                                                            */
    } bit;                                          /*!< [8] BitSize                                                           */
  } IRCON0CLR;
  __I  uint8_t   RESERVED10[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005030) Peripheral Interrupt Enable Register 1, RESET_TYPE_3   */
    
    struct {
      __IO uint8_t   EIREN1     :  1;               /*!< [0..0] SSC 1 Error Interrupt Enable                                   */
      __IO uint8_t   TIREN1     :  1;               /*!< [1..1] SSC 1 Transmit Interrupt Enable                                */
      __IO uint8_t   RIREN1     :  1;               /*!< [2..2] SSC 1 Receive Interrupt Enable                                 */
           uint8_t              :  3;
      __IO uint8_t   RIEN1      :  1;               /*!< [6..6] UART 1 Receive Interrupt Enable                                */
      __IO uint8_t   TIEN1      :  1;               /*!< [7..7] UART 1 Transmit Interrupt Enable                               */
    } bit;                                          /*!< [8] BitSize                                                           */
  } MODIEN1;
  __I  uint8_t   RESERVED11[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005034) Peripheral Interrupt Enable Register 2, RESET_TYPE_3   */
    
    struct {
      __IO uint8_t   EIREN2     :  1;               /*!< [0..0] SSC 2 Error Interrupt Enable                                   */
      __IO uint8_t   TIREN2     :  1;               /*!< [1..1] SSC 2 Transmit Interrupt Enable                                */
      __IO uint8_t   RIREN2     :  1;               /*!< [2..2] SSC 2 Receive Interrupt Enable                                 */
           uint8_t              :  2;
      __IO uint8_t   EXINT2_EN  :  1;               /*!< [5..5] External Interrupt 2 Enable                                    */
      __IO uint8_t   RIEN2      :  1;               /*!< [6..6] UART 2 Receive Interrupt Enable                                */
      __IO uint8_t   TIEN2      :  1;               /*!< [7..7] UART 2 Transmit Interrupt Enable                               */
    } bit;                                          /*!< [8] BitSize                                                           */
  } MODIEN2;
  __I  uint8_t   RESERVED12[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005038) Peripheral Interrupt Enable Register 3, RESET_TYPE_3   */
    
    struct {
      __IO uint8_t   IE0        :  1;               /*!< [0..0] External Interrupt Enable                                      */
           uint8_t              :  3;
      __IO uint8_t   MONIE      :  1;               /*!< [4..4] MON Interrupt Enable                                           */
      __I  uint8_t   MONSTS     :  1;               /*!< [5..5] MON Input Status                                               */
    } bit;                                          /*!< [6] BitSize                                                           */
  } MODIEN3;
  __I  uint8_t   RESERVED13[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x5000503C) Peripheral Interrupt Enable Register 4, RESET_TYPE_3   */
    
    struct {
      __IO uint8_t   IE1        :  1;               /*!< [0..0] External Interrupt Enable                                      */
    } bit;                                          /*!< [1] BitSize                                                           */
  } MODIEN4;
  __I  uint8_t   RESERVED14[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005040) Power Mode Control Register 0, RESET_TYPE_3            */
    
    struct {
      __IO uint8_t   XTAL_ON    :  1;               /*!< [0..0] OSC_HP Operation in Power Down Mode                            */
      __IO uint8_t   SL         :  1;               /*!< [1..1] Sleep Mode Enable. Active High.                                */
      __IO uint8_t   PD         :  1;               /*!< [2..2] Power Down Mode Enable. Active High.                           */
      __IO uint8_t   SD         :  1;               /*!< [3..3] Slow Down Mode Enable. Active High.                            */
    } bit;                                          /*!< [4] BitSize                                                           */
  } PMCON0;
  __I  uint8_t   RESERVED15[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005044) PLL Control Register, RESET_TYPE_4                     */
    
    struct {
      __I  uint8_t   LOCK       :  1;               /*!< [0..0] PLL Lock Status Flag                                           */
      __IO uint8_t   RESLD      :  1;               /*!< [1..1] Restart Lock Detection                                         */
      __IO uint8_t   OSCDISC    :  1;               /*!< [2..2] Oscillator Disconnect                                          */
      __IO uint8_t   VCOBYP     :  1;               /*!< [3..3] PLL VCO Bypass Mode Select                                     */
      __IO uint8_t   NDIV       :  4;               /*!< [4..7] PLL N-Divider                                                  */
    } bit;                                          /*!< [8] BitSize                                                           */
  } PLL_CON;
  __I  uint8_t   RESERVED16[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005048) Clock Control Register 1, RESET_TYPE_4                 */
    
    struct {
      __IO uint8_t   CLKREL     :  4;               /*!< [0..3] Slow Down Clock Divider for fCCLK Generation                   */
      __IO uint8_t   K2DIV      :  2;               /*!< [4..5] PLL K2-Divider                                                 */
      __IO uint8_t   K1DIV      :  1;               /*!< [6..6] PLL K1-Divider                                                 */
      __IO uint8_t   VCOSEL     :  1;               /*!< [7..7] VCOSEL Setting                                                 */
    } bit;                                          /*!< [8] BitSize                                                           */
  } CMCON1;
  __I  uint8_t   RESERVED17[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x5000504C) Clock Control Register 2, RESET_TYPE_4                 */
    
    struct {
      __IO uint8_t   PBA0CLKREL :  1;               /*!< [0..0] PBA0 Clock Divider                                             */
    } bit;                                          /*!< [1] BitSize                                                           */
  } CMCON2;
  __I  uint8_t   RESERVED18[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005050) Watchdog Timer Control Register, RESET_TYPE_3          */
    
    struct {
      __IO uint8_t   WDTIN      :  1;               /*!< [0..0] Watchdog Timer Input Frequency Selection                       */
      __IO uint8_t   WDTRS      :  1;               /*!< [1..1] WDT Refresh Start                                              */
      __IO uint8_t   WDTEN      :  1;               /*!< [2..2] WDT Enable                                                     */
           uint8_t              :  1;
      __I  uint8_t   WDTPR      :  1;               /*!< [4..4] Watchdog Prewarning Mode Flag                                  */
      __IO uint8_t   WINBEN     :  1;               /*!< [5..5] Watchdog Window-Boundary Enable                                */
    } bit;                                          /*!< [6] BitSize                                                           */
  } WDTCON;
  __I  uint8_t   RESERVED19[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005054) Analog Peripheral Clock Control 1 Register, RESET_TYPE_4 */
    
    struct {
      __I  uint8_t   PLL_LOCK   :  1;               /*!< [0..0] PLL Lock Indicator                                             */
      __IO uint8_t   APCLK_SET  :  1;               /*!< [1..1] Set and Overtake Flag for Clock Settings                       */
      __IO uint8_t   T3CLK_SEL  :  1;               /*!< [2..2] Timer 3 Clock Selection                                        */
      __IO uint8_t   CLKWDT_IE  :  1;               /*!< [3..3] Clock Watchdog Interrupt Enable                                */
      __IO uint8_t   BGCLK_SEL  :  1;               /*!< [4..4] Bandgap Clock Selection                                        */
      __IO uint8_t   BGCLK_DIV  :  1;               /*!< [5..5] Bandgap Clock Divider                                          */
      __IO uint8_t   CPCLK_SEL  :  1;               /*!< [6..6] Charge Pump Clock Selection                                    */
      __IO uint8_t   CPCLK_DIV  :  1;               /*!< [7..7] Charge Pump Clock Divider                                      */
    } bit;                                          /*!< [8] BitSize                                                           */
  } APCLK_CTRL1;
  __I  uint8_t   RESERVED20[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005058) Analog Peripheral Clock Register 1, RESET_TYPE_4       */
    
    struct {
      __IO uint8_t   APCLK1FAC  :  2;               /*!< [0..1] Analog Module Clock Factor                                     */
      __O  uint8_t   APCLK1SCLR :  1;               /*!< [2..2] Analog Peripherals Clock Status Clear                          */
           uint8_t              :  1;
      __I  uint8_t   APCLK1STS  :  2;               /*!< [4..5] Analog Peripherals Clock Status                                */
      __I  uint8_t   APCLK3STS  :  1;               /*!< [6..6] Loss of Clock Status                                           */
      __O  uint8_t   APCLK3SCLR :  1;               /*!< [7..7] Analog Peripherals Clock Status Clear                          */
    } bit;                                          /*!< [8] BitSize                                                           */
  } APCLK1;
  __I  uint8_t   RESERVED21[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x5000505C) Analog Peripheral Clock Register 2, RESET_TYPE_4       */
    
    struct {
      __IO uint8_t   APCLK2FAC  :  5;               /*!< [0..4] Slow Down Clock Divider for TFILT_CLK Generation               */
      __I  uint8_t   APCLK2STS  :  2;               /*!< [5..6] Analog Peripherals Clock Status                                */
      __O  uint8_t   APCLK2SCLR :  1;               /*!< [7..7] Analog Peripherals Clock Status Clear                          */
    } bit;                                          /*!< [8] BitSize                                                           */
  } APCLK2;
  __I  uint8_t   RESERVED22[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005060) Peripheral Management Control Register 1, RESET_TYPE_3 */
    
    struct {
      __IO uint8_t   ADC1_DIS   :  1;               /*!< [0..0] ADC1 Disable Request. Active high.                             */
      __IO uint8_t   SSC1_DIS   :  1;               /*!< [1..1] SSC1 Disable Request. Active high.                             */
      __IO uint8_t   CCU6_DIS   :  1;               /*!< [2..2] CCU6 Disable Request. Active high.                             */
      __IO uint8_t   T2_DIS     :  1;               /*!< [3..3] T2 Disable Request. Active high.                               */
      __IO uint8_t   GPT12_DIS  :  1;               /*!< [4..4] General Purpose Timer 12 Disable Request. Active high.         */
    } bit;                                          /*!< [5] BitSize                                                           */
  } PMCON1;
  __I  uint8_t   RESERVED23[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005064) Peripheral Management Control Register 2, RESET_TYPE_3 */
    
    struct {
           uint8_t              :  1;
      __IO uint8_t   SSC2_DIS   :  1;               /*!< [1..1] SSC2 Disable Request. Active high.                             */
           uint8_t              :  1;
      __IO uint8_t   T21_DIS    :  1;               /*!< [3..3] T21 Disable Request. Active high.                              */
           uint8_t              :  1;
      __IO uint8_t   T3_DIS     :  1;               /*!< [5..5] T3 Disable Request. Active high.                               */
    } bit;                                          /*!< [6] BitSize                                                           */
  } PMCON2;
  __I  uint8_t   RESERVED24[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005068) Reset Control Register, RESET_TYPE_3                   */
    
    struct {
      __IO uint8_t   LOCKUP     :  1;               /*!< [0..0] Lockup Flag                                                    */
           uint8_t              :  6;
      __IO uint8_t   LOCKUP_EN  :  1;               /*!< [7..7] Lockup Reset Enable Flag                                       */
    } bit;                                          /*!< [8] BitSize                                                           */
  } RSTCON;
  __I  uint8_t   RESERVED25[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x5000506C) Analog Peripheral Clock Control 2 Register, RESET_TYPE_4 */
    
    struct {
           uint8_t              :  2;
      __IO uint8_t   T3CLK_DIV  :  2;               /*!< [2..3] Timer 3 Clock Divider                                          */
    } bit;                                          /*!< [4] BitSize                                                           */
  } APCLK_CTRL2;
  __I  uint8_t   RESERVED26[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005070) System Control Register 0, RESET_TYPE_4                */
    
    struct {
           uint8_t              :  4;
      __IO uint8_t   NVMCLKFAC  :  2;               /*!< [4..5] NVM Access Clock Factor                                        */
      __IO uint8_t   SYSCLKSEL  :  2;               /*!< [6..7] System Clock Select                                            */
    } bit;                                          /*!< [8] BitSize                                                           */
  } SYSCON0;
  __I  uint8_t   RESERVED27[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x50005074) System Startup Status Register                         */
    
    struct {
      __I  uint8_t   INIT_FAIL  :  1;               /*!< [0..0] Initialization at startup failed                               */
      __I  uint8_t   MRAMINITSTS:  1;               /*!< [1..1] Map RAM Initialization Status                                  */
      __I  uint8_t   PG100TP_CHKS_ERR:  1;          /*!< [2..2] 100 TP Page Checksum Error                                     */
    } bit;                                          /*!< [3] BitSize                                                           */
  } SYS_STRTUP_STS;
  __I  uint8_t   RESERVED28[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005078) Watchdog Timer Reload Register, RESET_TYPE_3           */
    
    struct {
      __IO uint8_t   WDTREL     :  8;               /*!< [0..7] Watchdog Timer Reload Value                                    */
    } bit;                                          /*!< [8] BitSize                                                           */
  } WDTREL;
  __I  uint8_t   RESERVED29[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x5000507C) Watchdog Window-Boundary Count, RESET_TYPE_3           */
    
    struct {
      __IO uint8_t   WDTWINB    :  8;               /*!< [0..7] Watchdog Window-Boundary Count Value                           */
    } bit;                                          /*!< [8] BitSize                                                           */
  } WDTWINB;
  __I  uint8_t   RESERVED30[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x50005080) Watchdog Timer, Low Byte, RESET_TYPE_3                 */
    
    struct {
      __I  uint8_t   WDT        :  8;               /*!< [0..7] Watchdog Timer Current Value                                   */
    } bit;                                          /*!< [8] BitSize                                                           */
  } WDTL;
  __I  uint8_t   RESERVED31[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x50005084) Watchdog Timer, High Byte, RESET_TYPE_3                */
    
    struct {
      __I  uint8_t   WDT        :  8;               /*!< [0..7] Watchdog Timer Current Value                                   */
    } bit;                                          /*!< [8] BitSize                                                           */
  } WDTH;
  __I  uint8_t   RESERVED32[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005088) Baud Rate Control Register 1, RESET_TYPE_3             */
    
    struct {
      __IO uint8_t   R          :  1;               /*!< [0..0] Baud Rate Generator Run Control Bit                            */
      __IO uint8_t   BRPRE      :  3;               /*!< [1..3] Prescaler Bit                                                  */
    } bit;                                          /*!< [4] BitSize                                                           */
  } BCON1;
  __I  uint8_t   RESERVED33[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x5000508C) Baud Rate Timer/Reload Register, Low Byte 1,
                                                         RESET_TYPE_3                                                          */
    
    struct {
      __IO uint8_t   FD_SEL     :  5;               /*!< [0..4] Fractional Divider Selection                                   */
      __IO uint8_t   BR_VALUE   :  3;               /*!< [5..7] Baud Rate Timer/Reload Value                                   */
    } bit;                                          /*!< [8] BitSize                                                           */
  } BGL1;
  __I  uint8_t   RESERVED34[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005090) Baud Rate Timer/Reload Register, High Byte, RESET_TYPE_3 */
    
    struct {
      __IO uint8_t   BR_VALUE   :  8;               /*!< [0..7] Baud Rate Timer/Reload Value                                   */
    } bit;                                          /*!< [8] BitSize                                                           */
  } BGH1;
  __I  uint8_t   RESERVED35[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005094) LIN Status Register, RESET_TYPE_3                      */
    
    struct {
      __IO uint8_t   BRDIS      :  1;               /*!< [0..0] Baud Rate Detection Disable                                    */
      __IO uint8_t   BGSEL      :  2;               /*!< [1..2] Baud Rate Select for Detection                                 */
      __I  uint8_t   BRK        :  1;               /*!< [3..3] Break Field Flag                                               */
      __I  uint8_t   EOFSYN     :  1;               /*!< [4..4] End of SYN Byte Interrupt Flag                                 */
      __I  uint8_t   ERRSYN     :  1;               /*!< [5..5] SYN Byte Error Interrupt Flag                                  */
      __IO uint8_t   SYNEN      :  1;               /*!< [6..6] End of SYN Byte and SYN Byte Error Interrupts Enable           */
    } bit;                                          /*!< [7] BitSize                                                           */
  } LINST;
  __I  uint8_t   RESERVED36[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005098) Baud Rate Control Register 2, RESET_TYPE_3             */
    
    struct {
      __IO uint8_t   R          :  1;               /*!< [0..0] Baud Rate Generator Run Control Bit                            */
      __IO uint8_t   BRPRE      :  3;               /*!< [1..3] Prescaler Bit                                                  */
    } bit;                                          /*!< [4] BitSize                                                           */
  } BCON2;
  __I  uint8_t   RESERVED37[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x5000509C) Baud Rate Timer/Reload Register, Low Byte 2,
                                                         RESET_TYPE_3                                                          */
    
    struct {
      __IO uint8_t   FD_SEL     :  5;               /*!< [0..4] Fractional Divider Selection                                   */
      __IO uint8_t   BR_VALUE   :  3;               /*!< [5..7] Baud Rate Timer/Reload Value                                   */
    } bit;                                          /*!< [8] BitSize                                                           */
  } BGL2;
  __I  uint8_t   RESERVED38[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050A0) Baud Rate Timer/Reload Register, High Byte, RESET_TYPE_3 */
    
    struct {
      __IO uint8_t   BR_VALUE   :  8;               /*!< [0..7] Baud Rate Timer/Reload Value                                   */
    } bit;                                          /*!< [8] BitSize                                                           */
  } BGH2;
  __I  uint8_t   RESERVED39[3];
  
  union {
    __O  uint8_t   reg;                             /*!< (@ 0x500050A4) LIN Status Clear Register, RESET_TYPE_3                */
    
    struct {
           uint8_t              :  3;
      __O  uint8_t   BRKC       :  1;               /*!< [3..3] Break Field Flag Clear                                         */
      __O  uint8_t   EOFSYNC    :  1;               /*!< [4..4] End of SYN Byte Interrupt Flag Clear                           */
      __O  uint8_t   ERRSYNC    :  1;               /*!< [5..5] SYN Byte Error Interrupt Flag                                  */
    } bit;                                          /*!< [6] BitSize                                                           */
  } LINSCLR;
  __I  uint8_t   RESERVED40[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x500050A8) Identity Register, RESET_TYPE_3                        */
    
    struct {
      __I  uint8_t   VERID      :  3;               /*!< [0..2] Version ID                                                     */
      __I  uint8_t   PRODID     :  5;               /*!< [3..7] Product ID                                                     */
    } bit;                                          /*!< [8] BitSize                                                           */
  } ID;
  __I  uint8_t   RESERVED41[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050AC) Password Register, RESET_TYPE_3                        */
    
    struct {
      __IO uint8_t   MODE       :  2;               /*!< [0..1] Bit-Protection Scheme Control Bit                              */
      __I  uint8_t   PROTECT_S  :  1;               /*!< [2..2] Bit-Protection Signal Status Bit                               */
      __O  uint8_t   PASS       :  5;               /*!< [3..7] Password Bits                                                  */
    } bit;                                          /*!< [8] BitSize                                                           */
  } PASSWD;
  __I  uint8_t   RESERVED42[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050B0) OSC Control Register, RESET_TYPE_4                     */
    
    struct {
      __IO uint8_t   OSCSS      :  2;               /*!< [0..1] Oscillator Source Select                                       */
      __IO uint8_t   OSCWDTRST  :  1;               /*!< [2..2] Oscillator Watchdog Reset                                      */
      __I  uint8_t   OSC2L      :  1;               /*!< [3..3] OSC-Too-Low Condition Flag                                     */
      __IO uint8_t   XPD        :  1;               /*!< [4..4] XTAL (OSC_HP) Power Down Control                               */
           uint8_t              :  2;
      __IO uint8_t   OSCTRIM_8  :  1;               /*!< [7..7] OSC_PLL Trim Configuration Bit [8]                             */
    } bit;                                          /*!< [8] BitSize                                                           */
  } OSC_CON;
  __I  uint8_t   RESERVED43[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050B4) Clock Output Control Register, RESET_TYPE_4            */
    
    struct {
      __IO uint8_t   COREL      :  4;               /*!< [0..3] Clock Output Divider                                           */
      __IO uint8_t   COUTS0     :  1;               /*!< [4..4] Clock Out Source Select Bit 0                                  */
      __IO uint8_t   TLEN       :  1;               /*!< [5..5] Toggle Latch Enable                                            */
      __IO uint8_t   COUTS1     :  1;               /*!< [6..6] Clock Out Source Select Bit 1                                  */
      __IO uint8_t   EN         :  1;               /*!< [7..7] CLKOUT Enable                                                  */
    } bit;                                          /*!< [8] BitSize                                                           */
  } COCON;
  __I  uint8_t   RESERVED44[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050B8) Peripheral Input Select Register, RESET_TYPE_3         */
    
    struct {
      __IO uint8_t   EXINT0IS   :  2;               /*!< [0..1] External Interrupt 0 Input Select                              */
      __IO uint8_t   EXINT1IS   :  2;               /*!< [2..3] External Interrupt 1 Input Select                              */
      __IO uint8_t   EXINT2IS   :  2;               /*!< [4..5] External Interrupt 2 Input Select                              */
      __IO uint8_t   URIOS1     :  1;               /*!< [6..6] UART1 Input/Output Select                                      */
      __IO uint8_t   U_TX_CONDIS:  1;               /*!< [7..7] UART1 TxD Connection Disable                                   */
    } bit;                                          /*!< [8] BitSize                                                           */
  } MODPISEL;
  __I  uint8_t   RESERVED45[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050BC) Peripheral Input Select Register 1, RESET_TYPE_3       */
    
    struct {
           uint8_t              :  6;
      __IO uint8_t   T2EXCON    :  1;               /*!< [6..6] Timer 2 External Input Control                                 */
      __IO uint8_t   T21EXCON   :  1;               /*!< [7..7] Timer 21 External Input Control                                */
    } bit;                                          /*!< [8] BitSize                                                           */
  } MODPISEL1;
  __I  uint8_t   RESERVED46[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050C0) Peripheral Input Select Register 2, RESET_TYPE_3       */
    
    struct {
      __IO uint8_t   T2IS       :  2;               /*!< [0..1] Timer 2 Input Select                                           */
      __IO uint8_t   T21IS      :  2;               /*!< [2..3] Timer 21 Input Select                                          */
      __IO uint8_t   T2EXIS     :  2;               /*!< [4..5] Timer 2 External Input Select                                  */
      __IO uint8_t   T21EXIS    :  2;               /*!< [6..7] Timer 21 External Input Select                                 */
    } bit;                                          /*!< [8] BitSize                                                           */
  } MODPISEL2;
  __I  uint8_t   RESERVED47[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050C4) Peripheral Input Select Register, RESET_TYPE_3         */
    
    struct {
           uint8_t              :  6;
      __IO uint8_t   URIOS2     :  1;               /*!< [6..6] UART2 Input/Output Select                                      */
    } bit;                                          /*!< [7] BitSize                                                           */
  } MODPISEL3;
  __I  uint8_t   RESERVED48[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050C8) Module Suspend Control Register 1, RESET_TYPE_3        */
    
    struct {
      __IO uint8_t   WDTSUSP    :  1;               /*!< [0..0] SCU Watchdog Timer Debug Suspend Bit                           */
      __IO uint8_t   T12SUSP    :  1;               /*!< [1..1] Timer 12 Debug Suspend Bit                                     */
      __IO uint8_t   T13SUSP    :  1;               /*!< [2..2] Timer 13 Debug Suspend Bit                                     */
      __IO uint8_t   T2_SUSP    :  1;               /*!< [3..3] Timer2 Debug Suspend Bit                                       */
      __IO uint8_t   GPT12_SUSP :  1;               /*!< [4..4] GPT12 Debug Suspend Bit                                        */
           uint8_t              :  1;
      __IO uint8_t   T21_SUSP   :  1;               /*!< [6..6] Timer21 Debug Suspend Bit                                      */
    } bit;                                          /*!< [7] BitSize                                                           */
  } MODSUSP1;
  __I  uint8_t   RESERVED49[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050CC) Module Suspend Control Register 2, RESET_TYPE_3        */
    
    struct {
      __IO uint8_t   T3_SUSP    :  1;               /*!< [0..0] Measurement Unit Debug Suspend Bit                             */
      __IO uint8_t   MU_SUSP    :  1;               /*!< [1..1] Measurement Unit Debug Suspend Bit                             */
      __IO uint8_t   ADC1_SUSP  :  1;               /*!< [2..2] ADC1 Unit Debug Suspend Bit                                    */
    } bit;                                          /*!< [3] BitSize                                                           */
  } MODSUSP2;
  __I  uint8_t   RESERVED50[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050D0) GPT12 Peripheral Input Select Register, RESET_TYPE_3   */
    
    struct {
      __IO uint8_t   GPT12      :  4;               /*!< [0..3] GPT12 TIN3B / TIN4D Input Select                               */
      __IO uint8_t   TRIG_CONF  :  1;               /*!< [4..4] CCU6 Trigger Configuration.                                    */
      __IO uint8_t   T3_GPT12_SEL:  1;              /*!< [5..5] CCU6_INT_SEL.                                                  */
    } bit;                                          /*!< [6] BitSize                                                           */
  } GPT12PISEL;
  __I  uint8_t   RESERVED51[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050D4) Error Detection and Correction Control Register,
                                                         RESET_TYPE_3                                                          */
    
    struct {
      __IO uint8_t   RIE        :  1;               /*!< [0..0] RAM Double Bit ECC Error Interrupt Enable                      */
           uint8_t              :  1;
      __IO uint8_t   NVMIE      :  1;               /*!< [2..2] NVM Double Bit ECC Error Interrupt Enable                      */
    } bit;                                          /*!< [3] BitSize                                                           */
  } EDCCON;
  __I  uint8_t   RESERVED52[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x500050D8) Error Detection and Correction Status Register,
                                                         RESET_TYPE_4                                                          */
    
    struct {
      __I  uint8_t   RDBE       :  1;               /*!< [0..0] RAM Double Bit Error                                           */
           uint8_t              :  1;
      __I  uint8_t   NVMDBE     :  1;               /*!< [2..2] NVM Double Bit Error                                           */
           uint8_t              :  1;
      __I  uint8_t   RSBE       :  1;               /*!< [4..4] RAM Single Bit Error                                           */
    } bit;                                          /*!< [5] BitSize                                                           */
  } EDCSTAT;
  __I  uint8_t   RESERVED53[3];
  
  union {
    __IO uint8_t   reg;                           /*!< (@ 0x500050DC) Memory Status Register                                 */
  
    struct {
	  __IO uint8_t   SECTORINFO:  6;              /*!< [0..5] Sector Information                                             */
	  __IO uint8_t   SASTATUS :  2;               /*!< [6..7] Service Algorithm Status                                       */
    } bit0;                                        /*!< [8] BitSize                                                           */
    struct {
      __IO uint8_t   NVMPROP  :  1;               /*!< [0..0] NVM Program Operation Status Bit                               */
      __IO uint8_t   EMPROP   :  1;               /*!< [1..1] Emergency Program Operation Status Bit                         */
    } bit1;                                        /*!< [2] BitSize                                                           */
  } MEMSTAT;
  __I  uint8_t   RESERVED54[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050E0) NVM Protection Status Register, RESET_TYPE_4           */
    
    struct {
      __IO uint8_t   NVMPROTSTSL_0:  1;             /*!< [0..0] NVM Protection Status Register Low Flags                       */
      __IO uint8_t   NVMPROTSTSL_1:  1;             /*!< [1..1] NVM Protection Status Register Low Flags                       */
      __IO uint8_t   NVMPROTSTSL_2:  1;             /*!< [2..2] NVM Protection Status Register Low Flags                       */
      __IO uint8_t   NVMPROTSTSL_3:  1;             /*!< [3..3] NVM Protection Status Register Low Flags                       */
    } bit;                                          /*!< [4] BitSize                                                           */
  } NVM_PROT_STS;
  __I  uint8_t   RESERVED55[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x500050E4) Memory Access Status Register, RESET_TYPE_3            */
    
    struct {
      __I  uint8_t   NVM_PROT_ERR:  1;              /*!< [0..0] NVM Access Protection                                          */
      __I  uint8_t   NVM_ADDR_ERR:  1;              /*!< [1..1] NVM Address Protection                                         */
      __I  uint8_t   NVM_SFR_PROT_ERR:  1;          /*!< [2..2] NVM SFR Access Protection                                      */
      __I  uint8_t   NVM_SFR_ADDR_ERR:  1;          /*!< [3..3] NVM SFR Address Protection                                     */
      __I  uint8_t   ROM_PROT_ERR:  1;              /*!< [4..4] ROM Access Protection                                          */
      __I  uint8_t   ROM_ADDR_ERR:  1;              /*!< [5..5] ROM Address Protection                                         */
      __I  uint8_t   RAM_PROT_ERR:  1;              /*!< [6..6] RAM Access Protection                                          */
      __I  uint8_t   RAM_ADDR_ERR:  1;              /*!< [7..7] RAM Address Protection                                         */
    } bit;                                          /*!< [8] BitSize                                                           */
  } MEM_ACC_STS;
  __I  uint8_t   RESERVED56[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050E8) Port Output Control Register, RESET_TYPE_3             */
    
    struct {
      __IO uint8_t   PDM0       :  3;               /*!< [0..2] P0.0 Port Driver Mode                                          */
           uint8_t              :  1;
      __IO uint8_t   PDM1       :  3;               /*!< [4..6] P0.1 Port Driver Mode                                          */
    } bit;                                          /*!< [7] BitSize                                                           */
  } P0_POCON0;
  __I  uint8_t   RESERVED57[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050EC) Port Output Control Register, RESET_TYPE_3             */
    
    struct {
      __IO uint8_t   PDM2       :  3;               /*!< [0..2] P0.2 Port Driver Mode                                          */
           uint8_t              :  1;
      __IO uint8_t   PDM3       :  3;               /*!< [4..6] P0.3 Port Driver Mode                                          */
    } bit;                                          /*!< [7] BitSize                                                           */
  } P0_POCON1;
  __I  uint8_t   RESERVED58[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050F0) Port Output Control Register, RESET_TYPE_3             */
    
    struct {
      __IO uint8_t   PDM4       :  3;               /*!< [0..2] P0.4 Port Driver Mode                                          */
    } bit;                                          /*!< [3] BitSize                                                           */
  } P0_POCON2;
  __I  uint8_t   RESERVED59[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050F4) Temperature Compensation Control Register, RESET_TYPE_3 */
    
    struct {
      __IO uint8_t   TCC        :  2;               /*!< [0..1] Temperature Compensation Control                               */
    } bit;                                          /*!< [2] BitSize                                                           */
  } TCCR;
  __I  uint8_t   RESERVED60[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050F8) Port Output Control Register, RESET_TYPE_3             */
    
    struct {
      __IO uint8_t   PDM0       :  3;               /*!< [0..2] P1.0 Port Driver Mode                                          */
           uint8_t              :  1;
      __IO uint8_t   PDM1       :  3;               /*!< [4..6] P1.1 Port Driver Mode                                          */
    } bit;                                          /*!< [7] BitSize                                                           */
  } P1_POCON0;
  __I  uint8_t   RESERVED61[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500050FC) Port Output Control Register, RESET_TYPE_3             */
    
    struct {
      __IO uint8_t   PDM2       :  3;               /*!< [0..2] P1.2 Port Driver Mode                                          */
           uint8_t              :  1;
      __IO uint8_t   PDM3       :  3;               /*!< [4..6] P1.3 Port Driver Mode                                          */
    } bit;                                          /*!< [7] BitSize                                                           */
  } P1_POCON1;
  __I  uint8_t   RESERVED62[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005100) Port Output Control Register, RESET_TYPE_3             */
    
    struct {
      __IO uint8_t   PDM4       :  3;               /*!< [0..2] P1.4 Port Driver Mode                                          */
    } bit;                                          /*!< [3] BitSize                                                           */
  } P1_POCON2;
  __I  uint8_t   RESERVED63[11];
  
  union {
    __O  uint8_t   reg;                             /*!< (@ 0x5000510C) Error Detection and Correction Status Clear Register,
                                                         RESET_TYPE_3                                                          */
    
    struct {
      __O  uint8_t   RDBEC      :  1;               /*!< [0..0] RAM Double Bit Error Clear                                     */
           uint8_t              :  1;
      __O  uint8_t   NVMDBEC    :  1;               /*!< [2..2] NVM Double Bit Error Clear                                     */
           uint8_t              :  1;
      __O  uint8_t   RSBEC      :  1;               /*!< [4..4] RAM Single Bit Error Clear                                     */
    } bit;                                          /*!< [5] BitSize                                                           */
  } EDCSCLR;
  __I  uint8_t   RESERVED64[55];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005144) DMA Interrupt Enable Register 1, RESET_TYPE_3          */
    
    struct {
      __IO uint8_t   CH1IE      :  1;               /*!< [0..0] DMA Channel 1 Interrupt Enable                                 */
      __IO uint8_t   CH2IE      :  1;               /*!< [1..1] DMA Channel 2 Interrupt Enable                                 */
      __IO uint8_t   CH3IE      :  1;               /*!< [2..2] DMA Channel 3 Interrupt Enable                                 */
      __IO uint8_t   CH4IE      :  1;               /*!< [3..3] DMA Channel 4 Interrupt Enable                                 */
      __IO uint8_t   CH5IE      :  1;               /*!< [4..4] DMA Channel 5 Interrupt Enable                                 */
      __IO uint8_t   CH6IE      :  1;               /*!< [5..5] DMA Channel 6 Interrupt Enable                                 */
      __IO uint8_t   CH7IE      :  1;               /*!< [6..6] DMA Channel 7 Interrupt Enable                                 */
      __IO uint8_t   CH8IE      :  1;               /*!< [7..7] DMA Channel 8 Interrupt Enable                                 */
    } bit;                                          /*!< [8] BitSize                                                           */
  } DMAIEN1;
  __I  uint8_t   RESERVED65[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005148) DMA Interrupt Enable Register 2, RESET_TYPE_3          */
    
    struct {
      __IO uint8_t   TRERRIE    :  1;               /*!< [0..0] DMA Transfer Error Interrupt Enable                            */
      __IO uint8_t   TRSEQ1RDYIE:  1;               /*!< [1..1] DMA Transfer Sequence 1 Ready Interrupt Enable                 */
      __IO uint8_t   TRSEQ2RDYIE:  1;               /*!< [2..2] DMA Transfer Sequence 2 Ready Interrupt Enable                 */
      __IO uint8_t   SSCTXIE    :  1;               /*!< [3..3] DMA SSC Transmit Transfer Interrupt Enable                     */
      __IO uint8_t   SSCRXIE    :  1;               /*!< [4..4] DMA SSC Receive Transfer Interrupt Enable                      */
      __IO uint8_t   GPT12IE    :  1;               /*!< [5..5] DMA GPT12 triggered Transfer Interrupt Enable                  */
    } bit;                                          /*!< [6] BitSize                                                           */
  } DMAIEN2;
  __I  uint8_t   RESERVED66[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x5000514C) DMA Source Selection Register, RESET_TYPE_3            */
    
    struct {
      __IO uint8_t   SSCTXSRCSEL:  1;               /*!< [0..0] SSCx Transmit Source Select                                    */
      __IO uint8_t   SSCRXSRCSEL:  1;               /*!< [1..1] SSCx Receive Source Select                                     */
      __IO uint8_t   T12ZM_DMAEN:  1;               /*!< [2..2] CC6_T12_ZM (Zero Match) DMA muxer, DMA channel 10              */
      __IO uint8_t   T12PM_DMAEN:  1;               /*!< [3..3] CC6_T12_PM (Period Match) DMA muxer, DMA channel 11            */
           uint8_t              :  1;
      __I  uint8_t   SSCTX      :  1;               /*!< [5..5] SSC Transmit Request                                           */
      __I  uint8_t   SSCRX      :  1;               /*!< [6..6] SSC Receive Request                                            */
      __I  uint8_t   GPT12_T3   :  1;               /*!< [7..7] GPT12 Transmit Request                                         */
    } bit;                                          /*!< [8] BitSize                                                           */
  } DMASRCSEL;
  __I  uint8_t   RESERVED67[7];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x50005154) DMA Interrupt Control Register 1, RESET_TYPE_3         */
    
    struct {
      __I  uint8_t   CH1        :  1;               /*!< [0..0] DMA ADC Channel 1 Interrupt Status (DMA channel 4)             */
      __I  uint8_t   CH2        :  1;               /*!< [1..1] DMA ADC Channel 2 Interrupt Status (DMA channel 5)             */
      __I  uint8_t   CH3        :  1;               /*!< [2..2] DMA ADC Channel 3 Interrupt Status (DMA channel 6)             */
      __I  uint8_t   CH4        :  1;               /*!< [3..3] DMA ADC Channel 4 Interrupt Status (DMA channel 7)             */
      __I  uint8_t   CH5        :  1;               /*!< [4..4] DMA ADC Channel 5 Interrupt Status (DMA channel 8)             */
      __I  uint8_t   CH6        :  1;               /*!< [5..5] DMA ADC Channel 6 Interrupt Status (DMA channel 9)             */
      __I  uint8_t   CH7        :  1;               /*!< [6..6] DMA ADC Channel 7 Interrupt Status (DMA channel 10)            */
      __I  uint8_t   CH8        :  1;               /*!< [7..7] DMA ADC Channel 8 Interrupt Status (DMA channel 11)            */
    } bit;                                          /*!< [8] BitSize                                                           */
  } DMAIRC1;
  __I  uint8_t   RESERVED68[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x50005158) ADC1 Interrupt Control Register 2, RESET_TYPE_3        */
    
    struct {
      __I  uint8_t   STRDY      :  1;               /*!< [0..0] DMA Single Transfer Ready                                      */
      __I  uint8_t   TRSEQ1DY   :  1;               /*!< [1..1] DMA Transfer Sequence 1 Ready (DMA channel 0)                  */
      __I  uint8_t   TRSEQ2DY   :  1;               /*!< [2..2] DMA Transfer Sequence 2 Ready (DMA channel 1)                  */
      __I  uint8_t   SSC1       :  1;               /*!< [3..3] DMA SSC1 Transfer Ready (DMA channel 2)                        */
      __I  uint8_t   SSC2       :  1;               /*!< [4..4] DMA SSC2 Transfer Ready (DMA channel 3)                        */
      __I  uint8_t   GPT12      :  1;               /*!< [5..5] DMA GPT12 Transfer Ready (DMA channel 12)                      */
    } bit;                                          /*!< [6] BitSize                                                           */
  } DMAIRC2;
  __I  uint8_t   RESERVED69[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x5000515C) General Purpose Timer 12 Interrupt Enable Register
                                                         , RESET_TYPE_3                                                        */
    
    struct {
      __IO uint8_t   T2IE       :  1;               /*!< [0..0] General Purpose Timer 12 T2 Interrupt Enable                   */
      __IO uint8_t   T3IE       :  1;               /*!< [1..1] General Purpose Timer 12 T3 Interrupt Enable                   */
      __IO uint8_t   T4IE       :  1;               /*!< [2..2] General Purpose Timer 12 T4 Interrupt Enable                   */
      __IO uint8_t   T5IE       :  1;               /*!< [3..3] General Purpose Timer 12 T5 Interrupt Enable                   */
      __IO uint8_t   T6IE       :  1;               /*!< [4..4] General Purpose Timer 12 T6 Interrupt Enable                   */
      __IO uint8_t   CRIE       :  1;               /*!< [5..5] General Purpose Timer 12 Capture and Reload Interrupt
                                                         Enable                                                                */
    } bit;                                          /*!< [6] BitSize                                                           */
  } GPT12IEN;
  __I  uint8_t   RESERVED70[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x50005160) Timer and Counter Control/Status Register, RESET_TYPE_3 */
    
    struct {
      __I  uint8_t   T2         :  1;               /*!< [0..0] GPT Module 1 Timer 2 Interrupt Status                          */
      __I  uint8_t   T3         :  1;               /*!< [1..1] GPT Module 1 Timer3 Interrupt Status                           */
      __I  uint8_t   T4         :  1;               /*!< [2..2] GPT Module 1 Timer4 Interrupt Status                           */
      __I  uint8_t   T5         :  1;               /*!< [3..3] GPT Module 2 Timer5 Interrupt Status                           */
      __I  uint8_t   T6         :  1;               /*!< [4..4] GPT Module 2Timer6 Interrupt Status                            */
      __I  uint8_t   CR         :  1;               /*!< [5..5] GPT Module 2 Capture Reload Interrupt Status                   */
    } bit;                                          /*!< [6] BitSize                                                           */
  } GPT12IRC;
  __I  uint8_t   RESERVED71[3];
  
  union {
    __O  uint8_t   reg;                             /*!< (@ 0x50005164) Timer and Counter Control/Status Clear Register,
                                                         RESET_TYPE_3                                                          */
    
    struct {
      __O  uint8_t   T2C        :  1;               /*!< [0..0] GPT Module 1 Timer 2 Interrupt Status Clear                    */
      __O  uint8_t   T3C        :  1;               /*!< [1..1] GPT Module 1 Timer3 Interrupt Status Clear                     */
      __O  uint8_t   T4C        :  1;               /*!< [2..2] GPT Module 1 Timer4 Interrupt Status Clear                     */
      __O  uint8_t   T5C        :  1;               /*!< [3..3] GPT Module 2 Timer5 Interrupt Status Clear                     */
      __O  uint8_t   T6C        :  1;               /*!< [4..4] GPT Module 2Timer6 Interrupt Status Clear                      */
      __O  uint8_t   CRC        :  1;               /*!< [5..5] GPT Module 2 Capture Reload Interrupt Status Clear             */
    } bit;                                          /*!< [6] BitSize                                                           */
  } GPT12ICLR;
  __I  uint8_t   RESERVED72[19];
  
  union {
    __O  uint8_t   reg;                             /*!< (@ 0x50005178) Interrupt Request 1 Clear Register, RESET_TYPE_3       */
    
    struct {
      __O  uint8_t   EIRC       :  1;               /*!< [0..0] Error Interrupt Flag for SSC1 Clear                            */
      __O  uint8_t   TIRC       :  1;               /*!< [1..1] Transmit Interrupt Flag for SSC1 Clear                         */
      __O  uint8_t   RIRC       :  1;               /*!< [2..2] Receive Interrupt Flag for SSC1 Clear                          */
    } bit;                                          /*!< [3] BitSize                                                           */
  } IRCON1CLR;
  __I  uint8_t   RESERVED73[3];
  
  union {
    __O  uint8_t   reg;                             /*!< (@ 0x5000517C) Interrupt Request 2 Clear Register, RESET_TYPE_3       */
    
    struct {
      __O  uint8_t   EIRC       :  1;               /*!< [0..0] Error Interrupt Flag for SSC2 Clear                            */
      __O  uint8_t   TIRC       :  1;               /*!< [1..1] Transmit Interrupt Flag for SSC2 Clear                         */
      __O  uint8_t   RIRC       :  1;               /*!< [2..2] Receive Interrupt Flag for SSC2 Clear                          */
    } bit;                                          /*!< [3] BitSize                                                           */
  } IRCON2CLR;
  __I  uint8_t   RESERVED74[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50005180) DMA Source Selection Register 2, RESET_TYPE_3          */
    
    struct {
      __IO uint8_t   GPT12_DMAEN:  2;               /*!< [0..1] GPT12 T3 DMA muxer, DMA channel 12                             */
    } bit;                                          /*!< [2] BitSize                                                           */
  } DMASRCSEL2;
  __I  uint8_t   RESERVED75[3];
  
  union {
    __O  uint8_t   reg;                             /*!< (@ 0x50005184) DMA Interrupt Control 1 Clear Register, RESET_TYPE_3   */
    
    struct {
      __O  uint8_t   CH1C       :  1;               /*!< [0..0] DMA ADC Channel 1 Interrupt Status (DMA channel 4) Clear       */
      __O  uint8_t   CH2C       :  1;               /*!< [1..1] DMA ADC Channel 2 Interrupt Status (DMA channel 5) Clear       */
      __O  uint8_t   CH3C       :  1;               /*!< [2..2] DMA ADC Channel 3 Interrupt Status (DMA channel 6) Clear       */
      __O  uint8_t   CH4C       :  1;               /*!< [3..3] DMA ADC Channel 4 Interrupt Status (DMA channel 7) Clear       */
      __O  uint8_t   CH5C       :  1;               /*!< [4..4] DMA ADC Channel 5 Interrupt Status (DMA channel 8) Clear       */
      __O  uint8_t   CH6C       :  1;               /*!< [5..5] DMA ADC Channel 6 Interrupt Status (DMA channel 9) Clear       */
      __O  uint8_t   CH7C       :  1;               /*!< [6..6] DMA ADC Channel 7 Interrupt Status (DMA channel 10) Clear      */
      __O  uint8_t   CH8C       :  1;               /*!< [7..7] DMA ADC Channel 8 Interrupt Status (DMA channel 11) Clear      */
    } bit;                                          /*!< [8] BitSize                                                           */
  } DMAIRC1CLR;
  __I  uint8_t   RESERVED76[3];
  
  union {
    __O  uint8_t   reg;                             /*!< (@ 0x50005188) ADC1 Interrupt Control 2 Clear Register, RESET_TYPE_3  */
    
    struct {
           uint8_t              :  1;
      __O  uint8_t   TRSEQ1DYC  :  1;               /*!< [1..1] DMA Transfer Sequence 1 Ready (DMA channel 0) Clear            */
      __O  uint8_t   TRSEQ2DYC  :  1;               /*!< [2..2] DMA Transfer Sequence 2 Ready (DMA channel 1) Clear            */
      __O  uint8_t   SSC1C      :  1;               /*!< [3..3] DMA SSC1 Transfer Ready (DMA channel 2) Clear                  */
      __O  uint8_t   SSC2C      :  1;               /*!< [4..4] DMA SSC2 Transfer Ready (DMA channel 3) Clear                  */
      __O  uint8_t   GPT12C     :  1;               /*!< [5..5] DMA GPT12 Transfer Ready (DMA channel 12) Clear                */
    } bit;                                          /*!< [6] BitSize                                                           */
  } DMAIRC2CLR;
  __I  uint8_t   RESERVED77[7];
  
  union {
    __O  uint8_t   reg;                             /*!< (@ 0x50005190) Interrupt Request 3 Clear Register, RESET_TYPE_3       */
    
    struct {
      __O  uint8_t   CCU6SR0C   :  1;               /*!< [0..0] Interrupt Flag 0 for CCU6 Clear                                */
           uint8_t              :  3;
      __O  uint8_t   CCU6SR1C   :  1;               /*!< [4..4] Interrupt Flag 1 for CCU6 Clear                                */
    } bit;                                          /*!< [5] BitSize                                                           */
  } IRCON3CLR;
  __I  uint8_t   RESERVED78[3];
  
  union {
    __O  uint8_t   reg;                             /*!< (@ 0x50005194) Interrupt Request 4 Clear Register, RESET_TYPE_3       */
    
    struct {
      __O  uint8_t   CCU6SR2C   :  1;               /*!< [0..0] Interrupt Flag 2 for CCU6 Clear                                */
           uint8_t              :  3;
      __O  uint8_t   CCU6SR3C   :  1;               /*!< [4..4] Interrupt Flag 3 for CCU6 Clear                                */
    } bit;                                          /*!< [5] BitSize                                                           */
  } IRCON4CLR;
  __I  uint8_t   RESERVED79[3];
  
  union {
    __O  uint8_t   reg;                             /*!< (@ 0x50005198) DMA Source Selection Clear Register, RESET_TYPE_3      */
    
    struct {
           uint8_t              :  5;
      __O  uint8_t   SSCTXC     :  1;               /*!< [5..5] SSC Transmit Request Clear                                     */
      __O  uint8_t   SSCRXC     :  1;               /*!< [6..6] SSC Receive Request Clear                                      */
      __O  uint8_t   GPT12_T3C  :  1;               /*!< [7..7] GPT12 Transmit Request Clear                                   */
    } bit;                                          /*!< [8] BitSize                                                           */
  } DMASRCCLR;
} SCU_Type;


/* ================================================================================ */
/* ================                      PORT                      ================ */
/* ================================================================================ */


/**
  * @brief PORT (PORT)
  */

typedef struct {                                    /*!< (@ 0x48028000) PORT Structure                                         */
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48028000) Port 0 Data Register                                   */
    
    struct {
      __IO uint8_t   P0         :  1;               /*!< [0..0] Port 0 Pin 0 Data Value                                        */
      __IO uint8_t   P1         :  1;               /*!< [1..1] Port 0 Pin 1 Data Value                                        */
      __IO uint8_t   P2         :  1;               /*!< [2..2] Port 0 Pin 2 Data Value                                        */
      __IO uint8_t   P3         :  1;               /*!< [3..3] Port 0 Pin 3 Data Value                                        */
      __IO uint8_t   P4         :  1;               /*!< [4..4] Port 0 Pin 4 Data Value                                        */
    } bit;                                          /*!< [5] BitSize                                                           */
  } P0_DATA;
  __I  uint8_t   RESERVED0[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48028004) Port 0 Direction Register                              */
    
    struct {
      __IO uint8_t   P0         :  1;               /*!< [0..0] Port 0 Pin 0 Direction Control                                 */
      __IO uint8_t   P1         :  1;               /*!< [1..1] Port 0 Pin 1 Direction Control                                 */
      __IO uint8_t   P2         :  1;               /*!< [2..2] Port 0 Pin 2 Direction Control                                 */
      __IO uint8_t   P3         :  1;               /*!< [3..3] Port 0 Pin 3 Direction Control                                 */
      __IO uint8_t   P4         :  1;               /*!< [4..4] Port 0 Pin 4 Direction Control                                 */
    } bit;                                          /*!< [5] BitSize                                                           */
  } P0_DIR;
  __I  uint8_t   RESERVED1[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48028008) Port 1 Data Register                                   */
    
    struct {
      __IO uint8_t   P0         :  1;               /*!< [0..0] Port 1 Pin 0 Data Value                                        */
      __IO uint8_t   P1         :  1;               /*!< [1..1] Port 1 Pin 1 Data Value                                        */
      __IO uint8_t   P2         :  1;               /*!< [2..2] Port 1 Pin 2 Data Value                                        */
      __IO uint8_t   P3         :  1;               /*!< [3..3] Port 1 Pin 3 Data Value                                        */
      __IO uint8_t   P4         :  1;               /*!< [4..4] Port 1 Pin 4 Data Value                                        */
    } bit;                                          /*!< [5] BitSize                                                           */
  } P1_DATA;
  __I  uint8_t   RESERVED2[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x4802800C) Port 1 Direction Register                              */
    
    struct {
      __IO uint8_t   P0         :  1;               /*!< [0..0] Port 1 Pin 0 Direction Control                                 */
      __IO uint8_t   P1         :  1;               /*!< [1..1] Port 1 Pin 1 Direction Control                                 */
      __IO uint8_t   P2         :  1;               /*!< [2..2] Port 1 Pin 2 Direction Control                                 */
      __IO uint8_t   P3         :  1;               /*!< [3..3] Port 1 Pin 3 Direction Control                                 */
      __IO uint8_t   P4         :  1;               /*!< [4..4] Port 1 Pin 4 Direction Control                                 */
    } bit;                                          /*!< [5] BitSize                                                           */
  } P1_DIR;
  __I  uint8_t   RESERVED3[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x48028010) Port 2 Data Register                                   */
    
    struct {
      __I  uint8_t   P0         :  1;               /*!< [0..0] Port 2 Pin 0 Data Value                                        */
           uint8_t              :  1;
      __I  uint8_t   P2         :  1;               /*!< [2..2] Port 2 Pin 2 Data Value                                        */
      __I  uint8_t   P3         :  1;               /*!< [3..3] Port 2 Pin 3 Data Value                                        */
      __I  uint8_t   P4         :  1;               /*!< [4..4] Port 2 Pin 4 Data Value                                        */
      __I  uint8_t   P5         :  1;               /*!< [5..5] Port 2 Pin 5 Data Value                                        */
    } bit;                                          /*!< [6] BitSize                                                           */
  } P2_DATA;
  __I  uint8_t   RESERVED4[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48028014) Port 2 Direction Register                              */
    
    struct {
      __IO uint8_t   P0         :  1;               /*!< [0..0] Port 2 Pin 0 Driver Control                                    */
           uint8_t              :  1;
      __IO uint8_t   P2         :  1;               /*!< [2..2] Port 2 Pin 2 Driver Control                                    */
      __IO uint8_t   P3         :  1;               /*!< [3..3] Port 2 Pin 3 Driver Control                                    */
      __IO uint8_t   P4         :  1;               /*!< [4..4] Port 2 Pin 4 Driver Control                                    */
      __IO uint8_t   P5         :  1;               /*!< [5..5] Port 2 Pin 5 Driver Control                                    */
    } bit;                                          /*!< [6] BitSize                                                           */
  } P2_DIR;
  __I  uint8_t   RESERVED5[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48028018) Port 0 Pull-Up/Pull-Down Select Register               */
    
    struct {
      __IO uint8_t   P0         :  1;               /*!< [0..0] Pull-Up/Pull-Down Select Port 0 Bit 0                          */
      __IO uint8_t   P1         :  1;               /*!< [1..1] Pull-Up/Pull-Down Select Port 0 Bit 1                          */
      __IO uint8_t   P2         :  1;               /*!< [2..2] Pull-Up/Pull-Down Select Port 0 Bit 2                          */
      __IO uint8_t   P3         :  1;               /*!< [3..3] Pull-Up/Pull-Down Select Port 0 Bit 3                          */
      __IO uint8_t   P4         :  1;               /*!< [4..4] Pull-Up/Pull-Down Select Port 0 Bit 4                          */
    } bit;                                          /*!< [5] BitSize                                                           */
  } P0_PUDSEL;
  __I  uint8_t   RESERVED6[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x4802801C) Port 0 Pull-Up/Pull-Down Enable Register               */
    
    struct {
      __IO uint8_t   P0         :  1;               /*!< [0..0] Pull-Up/Pull-Down Enable at Port 0 Bit 0                       */
      __IO uint8_t   P1         :  1;               /*!< [1..1] Pull-Up/Pull-Down Enable at Port 0 Bit 1                       */
      __IO uint8_t   P2         :  1;               /*!< [2..2] Pull-Up/Pull-Down Enable at Port 0 Bit 2                       */
      __IO uint8_t   P3         :  1;               /*!< [3..3] Pull-Up/Pull-Down Enable at Port 0 Bit 3                       */
      __IO uint8_t   P4         :  1;               /*!< [4..4] Pull-Up/Pull-Down Enable at Port 0 Bit 4                       */
    } bit;                                          /*!< [5] BitSize                                                           */
  } P0_PUDEN;
  __I  uint8_t   RESERVED7[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48028020) Port 1 Pull-Up/Pull-Down Select Register               */
    
    struct {
      __IO uint8_t   P0         :  1;               /*!< [0..0] Pull-Up/Pull-Down Select Port 1 Bit 0                          */
      __IO uint8_t   P1         :  1;               /*!< [1..1] Pull-Up/Pull-Down Select Port 1 Bit 1                          */
      __IO uint8_t   P2         :  1;               /*!< [2..2] Pull-Up/Pull-Down Select Port 1 Bit 2                          */
      __IO uint8_t   P3         :  1;               /*!< [3..3] Pull-Up/Pull-Down Select Port 1 Bit 3                          */
      __IO uint8_t   P4         :  1;               /*!< [4..4] Pull-Up/Pull-Down Select Port 1 Bit 4                          */
    } bit;                                          /*!< [5] BitSize                                                           */
  } P1_PUDSEL;
  __I  uint8_t   RESERVED8[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48028024) Port 1 Pull-Up/Pull-Down Enable Register               */
    
    struct {
      __IO uint8_t   P0         :  1;               /*!< [0..0] Pull-Up/Pull-Down Enable at Port 1 Bit 0                       */
      __IO uint8_t   P1         :  1;               /*!< [1..1] Pull-Up/Pull-Down Enable at Port 1 Bit 1                       */
      __IO uint8_t   P2         :  1;               /*!< [2..2] Pull-Up/Pull-Down Enable at Port 1 Bit 2                       */
      __IO uint8_t   P3         :  1;               /*!< [3..3] Pull-Up/Pull-Down Enable at Port 1 Bit 3                       */
      __IO uint8_t   P4         :  1;               /*!< [4..4] Pull-Up/Pull-Down Enable at Port 1 Bit 4                       */
    } bit;                                          /*!< [5] BitSize                                                           */
  } P1_PUDEN;
  __I  uint8_t   RESERVED9[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48028028) Port 2 Pull-Up/Pull-Down Select Register               */
    
    struct {
      __IO uint8_t   P0         :  1;               /*!< [0..0] Pull-Up/Pull-Down Select Port 2 Bit 0                          */
           uint8_t              :  1;
      __IO uint8_t   P2         :  1;               /*!< [2..2] Pull-Up/Pull-Down Select Port 2 Bit 2                          */
      __IO uint8_t   P3         :  1;               /*!< [3..3] Pull-Up/Pull-Down Select Port 2 Bit 3                          */
      __IO uint8_t   P4         :  1;               /*!< [4..4] Pull-Up/Pull-Down Select Port 2 Bit 4                          */
      __IO uint8_t   P5         :  1;               /*!< [5..5] Pull-Up/Pull-Down Select Port 2 Bit 5                          */
    } bit;                                          /*!< [6] BitSize                                                           */
  } P2_PUDSEL;
  __I  uint8_t   RESERVED10[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x4802802C) Port 2 Pull-Up/Pull-Down Enable Register               */
    
    struct {
      __IO uint8_t   P0         :  1;               /*!< [0..0] Pull-Up/Pull-Down Enable at Port 2 Bit 0                       */
           uint8_t              :  1;
      __IO uint8_t   P2         :  1;               /*!< [2..2] Pull-Up/Pull-Down Enable at Port 2 Bit 2                       */
      __IO uint8_t   P3         :  1;               /*!< [3..3] Pull-Up/Pull-Down Enable at Port 2 Bit 3                       */
      __IO uint8_t   P4         :  1;               /*!< [4..4] Pull-Up/Pull-Down Enable at Port 2 Bit 4                       */
      __IO uint8_t   P5         :  1;               /*!< [5..5] Pull-Up/Pull-Down Enable at Port 2 Bit 5                       */
    } bit;                                          /*!< [6] BitSize                                                           */
  } P2_PUDEN;
  __I  uint8_t   RESERVED11[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48028030) Port 0 Alternate Select Register                       */
    
    struct {
      __IO uint8_t   P0         :  1;               /*!< [0..0] Alternate Select Port 0 Bit 0                                  */
      __IO uint8_t   P1         :  1;               /*!< [1..1] Alternate Select Port 0 Bit 1                                  */
      __IO uint8_t   P2         :  1;               /*!< [2..2] Alternate Select Port 0 Bit 2                                  */
      __IO uint8_t   P3         :  1;               /*!< [3..3] Alternate Select Port 0 Bit 3                                  */
      __IO uint8_t   P4         :  1;               /*!< [4..4] Alternate Select Port 0 Bit 4                                  */
    } bit;                                          /*!< [5] BitSize                                                           */
  } P0_ALTSEL0;
  __I  uint8_t   RESERVED12[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48028034) Port 0 Alternate Select Register                       */
    
    struct {
      __IO uint8_t   P0         :  1;               /*!< [0..0] Alternate Select Port 0 Bit 0                                  */
      __IO uint8_t   P1         :  1;               /*!< [1..1] Alternate Select Port 0 Bit 1                                  */
      __IO uint8_t   P2         :  1;               /*!< [2..2] Alternate Select Port 0 Bit 2                                  */
      __IO uint8_t   P3         :  1;               /*!< [3..3] Alternate Select Port 0 Bit 3                                  */
      __IO uint8_t   P4         :  1;               /*!< [4..4] Alternate Select Port 0 Bit 4                                  */
    } bit;                                          /*!< [5] BitSize                                                           */
  } P0_ALTSEL1;
  __I  uint8_t   RESERVED13[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48028038) Port 1 Alternate Select Register                       */
    
    struct {
      __IO uint8_t   P0         :  1;               /*!< [0..0] Alternate Select Port 1 Bit 0                                  */
      __IO uint8_t   P1         :  1;               /*!< [1..1] Alternate Select Port 1 Bit 1                                  */
      __IO uint8_t   P2         :  1;               /*!< [2..2] Alternate Select Port 1 Bit 2                                  */
      __IO uint8_t   P3         :  1;               /*!< [3..3] Alternate Select Port 1 Bit 3                                  */
      __IO uint8_t   P4         :  1;               /*!< [4..4] Alternate Select Port 1 Bit 4                                  */
    } bit;                                          /*!< [5] BitSize                                                           */
  } P1_ALTSEL0;
  __I  uint8_t   RESERVED14[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x4802803C) Port 1 Alternate Select Register                       */
    
    struct {
      __IO uint8_t   P0         :  1;               /*!< [0..0] Alternate Select Port 1 Bit 0                                  */
      __IO uint8_t   P1         :  1;               /*!< [1..1] Alternate Select Port 1 Bit 1                                  */
      __IO uint8_t   P2         :  1;               /*!< [2..2] Alternate Select Port 1 Bit 2                                  */
      __IO uint8_t   P3         :  1;               /*!< [3..3] Alternate Select Port 1 Bit 3                                  */
      __IO uint8_t   P4         :  1;               /*!< [4..4] Alternate Select Port 1 Bit 4                                  */
    } bit;                                          /*!< [5] BitSize                                                           */
  } P1_ALTSEL1;
  __I  uint8_t   RESERVED15[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48028040) Port 0 Open Drain Control Register                     */
    
    struct {
      __IO uint8_t   P0         :  1;               /*!< [0..0] Port 0 Pin 0 Open Drain Mode                                   */
      __IO uint8_t   P1         :  1;               /*!< [1..1] Port 0 Pin 1 Open Drain Mode                                   */
      __IO uint8_t   P2         :  1;               /*!< [2..2] Port 0 Pin 2 Open Drain Mode                                   */
      __IO uint8_t   P3         :  1;               /*!< [3..3] Port 0 Pin 3 Open Drain Mode                                   */
      __IO uint8_t   P4         :  1;               /*!< [4..4] Port 0 Pin 4 Open Drain Mode                                   */
    } bit;                                          /*!< [5] BitSize                                                           */
  } P0_OD;
  __I  uint8_t   RESERVED16[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48028044) Port 1 Open Drain Control Register                     */
    
    struct {
      __IO uint8_t   P0         :  1;               /*!< [0..0] Port 1 Pin 0 Open Drain Mode                                   */
      __IO uint8_t   P1         :  1;               /*!< [1..1] Port 1 Pin 1 Open Drain Mode                                   */
      __IO uint8_t   P2         :  1;               /*!< [2..2] Port 1 Pin 2 Open Drain Mode                                   */
      __IO uint8_t   P3         :  1;               /*!< [3..3] Port 1 Pin 3 Open Drain Mode                                   */
      __IO uint8_t   P4         :  1;               /*!< [4..4] Port 1 Pin 4 Open Drain Mode                                   */
    } bit;                                          /*!< [5] BitSize                                                           */
  } P1_OD;
} PORT_Type;


/* ================================================================================ */
/* ================                     GPT12E                     ================ */
/* ================================================================================ */


/**
  * @brief GPT12E (GPT12E)
  */

typedef struct {                                    /*!< (@ 0x40010000) GPT12E Structure                                       */
  
  union {
    __I  uint16_t  reg;                             /*!< (@ 0x40010000) Module Identification Register                         */
    
    struct {
      __I  uint16_t  MOD_REV    :  8;               /*!< [0..7] Module Revision Number                                         */
      __I  uint16_t  MOD_TYPE   :  8;               /*!< [8..15] Module Identification Number                                  */
    } bit;                                          /*!< [16] BitSize                                                          */
  } ID;
  __I  uint16_t  RESERVED0;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x40010004) Port Input Select Register                             */
    
    struct {
      __IO uint16_t  IST2IN     :  1;               /*!< [0..0] Input Select for T2IN                                          */
      __IO uint16_t  IST2EUD    :  1;               /*!< [1..1] Input Select for T2EUD                                         */
      __IO uint16_t  IST3IN     :  2;               /*!< [2..3] Input Select for T3IN                                          */
      __IO uint16_t  IST3EUD    :  2;               /*!< [4..5] Input Select for T3EUD                                         */
      __IO uint16_t  IST4IN     :  2;               /*!< [6..7] Input Select for T4IN                                          */
      __IO uint16_t  IST4EUD    :  2;               /*!< [8..9] Input Select for T4EUD                                         */
      __IO uint16_t  IST5IN     :  1;               /*!< [10..10] Input Select for T5IN                                        */
      __IO uint16_t  IST5EUD    :  1;               /*!< [11..11] Input Select for T5EUD                                       */
      __IO uint16_t  IST6IN     :  1;               /*!< [12..12] Input Select for T6IN                                        */
      __IO uint16_t  IST6EUD    :  1;               /*!< [13..13] Input Select for T6EUD                                       */
      __IO uint16_t  ISCAPIN    :  2;               /*!< [14..15] Input Select for CAPIN                                       */
    } bit;                                          /*!< [16] BitSize                                                          */
  } PISEL;
  __I  uint16_t  RESERVED1;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x40010008) Timer T2 Control Register                              */
    
    struct {
      __IO uint16_t  T2I        :  3;               /*!< [0..2] Timer T2 Input Parameter Selection                             */
      __IO uint16_t  T2M        :  3;               /*!< [3..5] Timer T2 Mode Control (Basic Operating Mode)                   */
      __IO uint16_t  T2R        :  1;               /*!< [6..6] Timer T2 Run Bit                                               */
      __IO uint16_t  T2UD       :  1;               /*!< [7..7] Timer T2 Up/Down Control                                       */
      __IO uint16_t  T2UDE      :  1;               /*!< [8..8] Timer T2 External Up/Down Enable                               */
      __IO uint16_t  T2RC       :  1;               /*!< [9..9] Timer T2 Remote Control                                        */
           uint16_t             :  2;
      __IO uint16_t  T2IRDIS    :  1;               /*!< [12..12] Timer T2 Interrupt Disable                                   */
      __IO uint16_t  T2EDGE     :  1;               /*!< [13..13] Timer T2 Edge Detection                                      */
      __IO uint16_t  T2CHDIR    :  1;               /*!< [14..14] Timer T2 Count Direction Change                              */
      __I  uint16_t  T2RDIR     :  1;               /*!< [15..15] Timer T2 Rotation Direction                                  */
    } bit;                                          /*!< [16] BitSize                                                          */
  } T2CON;
  __I  uint16_t  RESERVED2;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4001000C) Timer T3 Control Register                              */
    
    struct {
      __IO uint16_t  T3I        :  3;               /*!< [0..2] Timer T3 Input Parameter Selection                             */
      __IO uint16_t  T3M        :  3;               /*!< [3..5] Timer T3 Mode Control                                          */
      __IO uint16_t  T3R        :  1;               /*!< [6..6] Timer T3 Run Bit                                               */
      __IO uint16_t  T3UD       :  1;               /*!< [7..7] Timer T3 Up/Down Control                                       */
      __IO uint16_t  T3UDE      :  1;               /*!< [8..8] Timer T3 External Up/Down Enable                               */
      __IO uint16_t  T3OE       :  1;               /*!< [9..9] Overflow/Underflow Output Enable                               */
      __IO uint16_t  T3OTL      :  1;               /*!< [10..10] Timer T3 Overflow Toggle Latch                               */
      __IO uint16_t  BPS1       :  2;               /*!< [11..12] GPT1 Block Prescaler Control                                 */
      __IO uint16_t  T3EDGE     :  1;               /*!< [13..13] Timer T3 Edge Detection Flag                                 */
      __IO uint16_t  T3CHDIR    :  1;               /*!< [14..14] Timer T3 Count Direction Change Flag                         */
      __I  uint16_t  T3RDIR     :  1;               /*!< [15..15] Timer T3 Rotation Direction Flag                             */
    } bit;                                          /*!< [16] BitSize                                                          */
  } T3CON;
  __I  uint16_t  RESERVED3;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x40010010) Timer T4 Control Register                              */
    
    struct {
      __IO uint16_t  T4I        :  3;               /*!< [0..2] Timer T4 Input Parameter Selection                             */
      __IO uint16_t  T4M        :  3;               /*!< [3..5] Timer T4 Mode Control (Basic Operating Mode)                   */
      __IO uint16_t  T4R        :  1;               /*!< [6..6] Timer T4 Run Bit                                               */
      __IO uint16_t  T4UD       :  1;               /*!< [7..7] Timer T4 Up/Down Control                                       */
      __IO uint16_t  T4UDE      :  1;               /*!< [8..8] Timer T4 External Up/Down Enable                               */
      __IO uint16_t  T4RC       :  1;               /*!< [9..9] Timer T4 Remote Control                                        */
      __IO uint16_t  CLRT2EN    :  1;               /*!< [10..10] Clear Timer T2 Enable                                        */
      __IO uint16_t  CLRT3EN    :  1;               /*!< [11..11] Clear Timer T3 Enable                                        */
      __IO uint16_t  T4IRDIS    :  1;               /*!< [12..12] Timer T4 Interrupt Disable                                   */
      __IO uint16_t  T4EDGE     :  1;               /*!< [13..13] Timer T4 Edge Detection                                      */
      __IO uint16_t  T4CHDIR    :  1;               /*!< [14..14] Timer T4 Count Direction Change                              */
      __I  uint16_t  T4RDIR     :  1;               /*!< [15..15] Timer T4 Rotation Direction                                  */
    } bit;                                          /*!< [16] BitSize                                                          */
  } T4CON;
  __I  uint16_t  RESERVED4;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x40010014) Timer 5 Control Register                               */
    
    struct {
      __IO uint16_t  T5I        :  3;               /*!< [0..2] Timer T5 Input Parameter Selection                             */
      __IO uint16_t  T5M        :  2;               /*!< [3..4] Timer T5 Mode Control (Basic Operating Mode)                   */
           uint16_t             :  1;
      __IO uint16_t  T5R        :  1;               /*!< [6..6] Timer T5 Run Bit                                               */
      __IO uint16_t  T5UD       :  1;               /*!< [7..7] Timer T5 Up/Down Control                                       */
      __IO uint16_t  T5UDE      :  1;               /*!< [8..8] Timer T5 External Up/Down Enable                               */
      __IO uint16_t  T5RC       :  1;               /*!< [9..9] Timer T5 Remote Control                                        */
      __IO uint16_t  CT3        :  1;               /*!< [10..10] Timer T3 Capture Trigger Enable                              */
           uint16_t             :  1;
      __IO uint16_t  CI         :  2;               /*!< [12..13] Register CAPREL Capture Trigger Selection                    */
      __IO uint16_t  T5CLR      :  1;               /*!< [14..14] Timer T5 Clear Enable Bit                                    */
      __IO uint16_t  T5SC       :  1;               /*!< [15..15] Timer T5 Capture Mode Enable                                 */
    } bit;                                          /*!< [16] BitSize                                                          */
  } T5CON;
  __I  uint16_t  RESERVED5;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x40010018) Timer T6 Control Register                              */
    
    struct {
      __IO uint16_t  T6I        :  3;               /*!< [0..2] Timer T6 Input Parameter Selection                             */
      __IO uint16_t  T6M        :  3;               /*!< [3..5] Timer T6 Mode Control (Basic Operating Mode)                   */
      __IO uint16_t  T6R        :  1;               /*!< [6..6] Timer T6 Run Bit                                               */
      __IO uint16_t  T6UD       :  1;               /*!< [7..7] Timer T6 Up/Down Control                                       */
      __IO uint16_t  T6UDE      :  1;               /*!< [8..8] Timer T6 External Up/Down Enable                               */
      __IO uint16_t  T6OE       :  1;               /*!< [9..9] Overflow/Underflow Output Enable                               */
      __IO uint16_t  T6OTL      :  1;               /*!< [10..10] Timer T6 Overflow Toggle Latch                               */
      __IO uint16_t  BPS2       :  2;               /*!< [11..12] GPT2 Block Prescaler Control                                 */
           uint16_t             :  1;
      __IO uint16_t  T6CLR      :  1;               /*!< [14..14] Timer T6 Clear Enable Bit                                    */
      __IO uint16_t  T6SR       :  1;               /*!< [15..15] Timer T6 Reload Mode Enable                                  */
    } bit;                                          /*!< [16] BitSize                                                          */
  } T6CON;
  __I  uint16_t  RESERVED6;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4001001C) Capture/Reload Register                                */
    
    struct {
      __IO uint16_t  CAPREL     : 16;               /*!< [0..15] Current reload value or Captured value                        */
    } bit;                                          /*!< [16] BitSize                                                          */
  } CAPREL;
  __I  uint16_t  RESERVED7;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x40010020) Timer T2 Count Register                                */
    
    struct {
      __IO uint16_t  T2         : 16;               /*!< [0..15] Timer T2 Current Value                                        */
    } bit;                                          /*!< [16] BitSize                                                          */
  } T2;
  __I  uint16_t  RESERVED8;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x40010024) Timer T3 Count Register                                */
    
    struct {
      __IO uint16_t  T3         : 16;               /*!< [0..15] Timer T3 Current Value                                        */
    } bit;                                          /*!< [16] BitSize                                                          */
  } T3;
  __I  uint16_t  RESERVED9;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x40010028) Timer T4 Count Register                                */
    
    struct {
      __IO uint16_t  T4         : 16;               /*!< [0..15] Timer T4 Current Value                                        */
    } bit;                                          /*!< [16] BitSize                                                          */
  } T4;
  __I  uint16_t  RESERVED10;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4001002C) Timer 5 Count Register                                 */
    
    struct {
      __IO uint16_t  T5         : 16;               /*!< [0..15] Timer T5 Current Value                                        */
    } bit;                                          /*!< [16] BitSize                                                          */
  } T5;
  __I  uint16_t  RESERVED11;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x40010030) Timer T6 Count Register                                */
    
    struct {
      __IO uint16_t  T6         : 16;               /*!< [0..15] Timer T6 Current Value                                        */
    } bit;                                          /*!< [16] BitSize                                                          */
  } T6;
} GPT12E_Type;


/* ================================================================================ */
/* ================                     TIMER2x                    ================ */
/* ================================================================================ */


/**
  * @brief TIMER2x (TIMER2x)
  */

typedef struct {                                    /*!< (@ 0x48004000) TIMER2 Structure                                       */
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48004000) Timer 2 Control Register                               */
    
    struct {
      __IO uint8_t   CP_RL2     :  1;               /*!< [0..0] Capture/Reload Select                                          */
      __IO uint8_t   C_T2       :  1;               /*!< [1..1] Timer or Counter Select                                        */
      __IO uint8_t   TR2        :  1;               /*!< [2..2] Timer 2 Start/Stop Control                                     */
      __IO uint8_t   EXEN2      :  1;               /*!< [3..3] Timer 2 External Enable Control                                */
           uint8_t              :  2;
      __I  uint8_t   EXF2       :  1;               /*!< [6..6] Timer 2 External Flag                                          */
      __I  uint8_t   TF2        :  1;               /*!< [7..7] Timer 2 Overflow/Underflow Flag                                */
    } bit;                                          /*!< [8] BitSize                                                           */
  } T2CON;
  __I  uint8_t   RESERVED0[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48004004) Timer 2 Mode Register                                  */
    
    struct {
      __IO uint8_t   DCEN       :  1;               /*!< [0..0] Up/Down Counter Enable                                         */
      __IO uint8_t   T2PRE      :  3;               /*!< [1..3] Timer 2 Prescaler Bit                                          */
      __IO uint8_t   PREN       :  1;               /*!< [4..4] Prescaler Enable                                               */
      __IO uint8_t   EDGESEL    :  1;               /*!< [5..5] Edge Select in Capture Mode/Reload Mode                        */
      __IO uint8_t   T2RHEN     :  1;               /*!< [6..6] Timer 2 External Start Enable                                  */
      __IO uint8_t   T2REGS     :  1;               /*!< [7..7] Edge Select for Timer 2 External Start                         */
    } bit;                                          /*!< [8] BitSize                                                           */
  } T2MOD;
  __I  uint8_t   RESERVED1[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48004008) Timer 2 Reload/Capture Register, Low Byte              */
    
    struct {
      __IO uint8_t   RC2        :  8;               /*!< [0..7] Reload/Capture Value                                           */
    } bit;                                          /*!< [8] BitSize                                                           */
  } RC2L;
  __I  uint8_t   RESERVED2[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x4800400C) Timer 2 Reload/Capture Register, High Byte             */
    
    struct {
      __IO uint8_t   RC2        :  8;               /*!< [0..7] Reload/Capture Value                                           */
    } bit;                                          /*!< [8] BitSize                                                           */
  } RC2H;
  __I  uint8_t   RESERVED3[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48004010) Timer 2, Low Byte                                      */
    
    struct {
      __IO uint8_t   T2L        :  8;               /*!< [0..7] Timer 2 Value                                                  */
    } bit;                                          /*!< [8] BitSize                                                           */
  } T2L;
  __I  uint8_t   RESERVED4[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48004014) Timer 2, High Byte                                     */
    
    struct {
      __IO uint8_t   T2H        :  8;               /*!< [0..7] Timer 2 Value                                                  */
    } bit;                                          /*!< [8] BitSize                                                           */
  } T2H;
  __I  uint8_t   RESERVED5[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48004018) Timer 2 Control Register 1                             */
    
    struct {
      __IO uint8_t   EXF2EN     :  1;               /*!< [0..0] External Interrupt Enable                                      */
      __IO uint8_t   TF2EN      :  1;               /*!< [1..1] Overflow/Underflow Interrupt Enable                            */
    } bit;                                          /*!< [2] BitSize                                                           */
  } T2CON1;
  __I  uint8_t   RESERVED6[3];
  
  union {
    __O  uint8_t   reg;                             /*!< (@ 0x4800401C) Timer 2 Interrupt Clear Register                       */
    
    struct {
           uint8_t              :  6;
      __O  uint8_t   EXF2CLR    :  1;               /*!< [6..6] External Interrupt Clear Flag                                  */
      __O  uint8_t   TF2CLR     :  1;               /*!< [7..7] Overflow/Underflow Interrupt Clear Flag                        */
    } bit;                                          /*!< [8] BitSize                                                           */
  } T2ICLR;
} TIMER2x_Type;


/* ================================================================================ */
/* ================                      CCU6                      ================ */
/* ================================================================================ */


/**
  * @brief CCU6 (CCU6)
  */

typedef struct {                                    /*!< (@ 0x4000C000) CCU6 Structure                                         */
  
  union {
    __I  uint16_t  reg;                             /*!< (@ 0x4000C000) Capture/Compare Register for Channel CC63              */
    
    struct {
      __I  uint16_t  CCV        : 16;               /*!< [0..15] Channel CC63 Compare Value                                    */
    } bit;                                          /*!< [16] BitSize                                                          */
  } CC63R;
  __I  uint16_t  RESERVED0;
  
  union {
    __O  uint16_t  reg;                             /*!< (@ 0x4000C004) Timer Control Register 4                               */
    
    struct {
      __O  uint16_t  T12RR      :  1;               /*!< [0..0] Timer T12 Run Reset                                            */
      __O  uint16_t  T12RS      :  1;               /*!< [1..1] Timer T12 Run Set                                              */
      __O  uint16_t  T12RES     :  1;               /*!< [2..2] Timer T12 Reset                                                */
      __O  uint16_t  DTRES      :  1;               /*!< [3..3] Dead-Time Counter Reset                                        */
           uint16_t             :  1;
      __O  uint16_t  T12CNT     :  1;               /*!< [5..5] Timer T12 Count Event                                          */
      __O  uint16_t  T12STR     :  1;               /*!< [6..6] Timer T12 Shadow Transfer Request                              */
      __O  uint16_t  T12STD     :  1;               /*!< [7..7] Timer T12 Shadow Transfer Disable                              */
      __O  uint16_t  T13RR      :  1;               /*!< [8..8] Timer T13 Run Reset                                            */
      __O  uint16_t  T13RS      :  1;               /*!< [9..9] Timer T13 Run Set                                              */
      __O  uint16_t  T13RES     :  1;               /*!< [10..10] Timer T13 Reset                                              */
           uint16_t             :  2;
      __O  uint16_t  T13CNT     :  1;               /*!< [13..13] Timer T13 Count Event                                        */
      __O  uint16_t  T13STR     :  1;               /*!< [14..14] Timer T13 Shadow Transfer Request                            */
      __O  uint16_t  T13STD     :  1;               /*!< [15..15] Timer T13 Shadow Transfer Disable                            */
    } bit;                                          /*!< [16] BitSize                                                          */
  } TCTR4;
  __I  uint16_t  RESERVED1;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C008) Multi-Channel Mode Output Shadow Register              */
    
    struct {
      __IO uint16_t  MCMPS      :  6;               /*!< [0..5] Multi-Channel PWM Pattern Shadow                               */
           uint16_t             :  1;
      __O  uint16_t  STRMCM     :  1;               /*!< [7..7] Shadow Transfer Request for MCMPS                              */
      __IO uint16_t  EXPHS      :  3;               /*!< [8..10] Expected Hall Pattern Shadow                                  */
      __IO uint16_t  CURHS      :  3;               /*!< [11..13] Current Hall Pattern Shadow                                  */
           uint16_t             :  1;
      __O  uint16_t  STRHP      :  1;               /*!< [15..15] Shadow Transfer Request for the Hall Pattern                 */
    } bit;                                          /*!< [16] BitSize                                                          */
  } MCMOUTS;
  __I  uint16_t  RESERVED2;
  
  union {
    __O  uint16_t  reg;                             /*!< (@ 0x4000C00C) Capture/Compare Interrupt Status Reset Register        */
    
    struct {
      __O  uint16_t  RCC60R     :  1;               /*!< [0..0] Reset Capture, Compare-Match Rising Edge Flag                  */
      __O  uint16_t  RCC60F     :  1;               /*!< [1..1] Reset Capture, Compare-Match Falling Edge Flag                 */
      __O  uint16_t  RCC61R     :  1;               /*!< [2..2] Reset Capture, Compare-Match Rising Edge Flag                  */
      __O  uint16_t  RCC61F     :  1;               /*!< [3..3] Reset Capture, Compare-Match Falling Edge Flag                 */
      __O  uint16_t  RCC62R     :  1;               /*!< [4..4] Reset Capture, Compare-Match Rising Edge Flag                  */
      __O  uint16_t  RCC62F     :  1;               /*!< [5..5] Reset Capture, Compare-Match Falling Edge Flag                 */
      __O  uint16_t  RT12OM     :  1;               /*!< [6..6] Reset Timer T12 One-Match Flag                                 */
      __O  uint16_t  RT12PM     :  1;               /*!< [7..7] Reset Timer T12 Period-Match Flag                              */
      __O  uint16_t  RT13CM     :  1;               /*!< [8..8] Reset Timer T13 Compare-Match Flag                             */
      __O  uint16_t  RT13PM     :  1;               /*!< [9..9] Reset Timer T13 Period-Match Flag                              */
      __O  uint16_t  RTRPF      :  1;               /*!< [10..10] Reset Trap Flag                                              */
           uint16_t             :  1;
      __O  uint16_t  RCHE       :  1;               /*!< [12..12] Reset Correct Hall Event Flag                                */
      __O  uint16_t  RWHE       :  1;               /*!< [13..13] Reset Wrong Hall Event Flag                                  */
      __O  uint16_t  RIDLE      :  1;               /*!< [14..14] Reset IDLE Flag                                              */
      __O  uint16_t  RSTR       :  1;               /*!< [15..15] Reset STR Flag                                               */
    } bit;                                          /*!< [16] BitSize                                                          */
  } ISR;
  __I  uint16_t  RESERVED3;
  
  union {
    __O  uint16_t  reg;                             /*!< (@ 0x4000C010) Compare State Modification Register                    */
    
    struct {
      __O  uint16_t  MCC60S     :  1;               /*!< [0..0] Capture/Compare Status Modification Bits (Set) (x = 0,
                                                         1, 2, 3)                                                              */
      __O  uint16_t  MCC61S     :  1;               /*!< [1..1] Capture/Compare Status Modification Bits (Set) (x = 0,
                                                         1, 2, 3)                                                              */
      __O  uint16_t  MCC62S     :  1;               /*!< [2..2] Capture/Compare Status Modification Bits (Set) (x = 0,
                                                         1, 2, 3)                                                              */
           uint16_t             :  3;
      __O  uint16_t  MCC63S     :  1;               /*!< [6..6] Capture/Compare Status Modification Bits (Set) (x = 0,
                                                         1, 2, 3)                                                              */
           uint16_t             :  1;
      __O  uint16_t  MCC60R     :  1;               /*!< [8..8] Capture/Compare Status Modification Bits (Reset) (x =
                                                         0, 1, 2, 3)                                                           */
      __O  uint16_t  MCC61R     :  1;               /*!< [9..9] Capture/Compare Status Modification Bits (Reset) (x =
                                                         0, 1, 2, 3)                                                           */
      __O  uint16_t  MCC62R     :  1;               /*!< [10..10] Capture/Compare Status Modification Bits (Reset) (x
                                                         = 0, 1, 2, 3)                                                         */
           uint16_t             :  3;
      __O  uint16_t  MCC63R     :  1;               /*!< [14..14] Capture/Compare Status Modification Bits (Reset) (x
                                                         = 0, 1, 2, 3)                                                         */
    } bit;                                          /*!< [15] BitSize                                                          */
  } CMPMODIF;
  __I  uint16_t  RESERVED4;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C014) Capture/Compare Shadow Register for Channel CC60       */
    
    struct {
      __IO uint16_t  CCS        : 16;               /*!< [0..15] Shadow Register for Channel x Capture/Compare Value           */
    } bit;                                          /*!< [16] BitSize                                                          */
  } CC60SR;
  __I  uint16_t  RESERVED5;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C018) Capture/Compare Shadow Register for Channel CC61       */
    
    struct {
      __IO uint16_t  CCS        : 16;               /*!< [0..15] Shadow Register for Channel x Capture/Compare Value           */
    } bit;                                          /*!< [16] BitSize                                                          */
  } CC61SR;
  __I  uint16_t  RESERVED6;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C01C) Capture/Compare Shadow Register for Channel CC62       */
    
    struct {
      __IO uint16_t  CCS        : 16;               /*!< [0..15] Shadow Register for Channel x Capture/Compare Value           */
    } bit;                                          /*!< [16] BitSize                                                          */
  } CC62SR;
  __I  uint16_t  RESERVED7;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C020) Capture/Compare Shadow Register for Channel CC63       */
    
    struct {
      __IO uint16_t  CCS        : 16;               /*!< [0..15] Shadow Register for Channel CC63 Compare Value                */
    } bit;                                          /*!< [16] BitSize                                                          */
  } CC63SR;
  __I  uint16_t  RESERVED8;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C024) Timer T12 Period Register                              */
    
    struct {
      __IO uint16_t  T12PV      : 16;               /*!< [0..15] T12 Period Value                                              */
    } bit;                                          /*!< [16] BitSize                                                          */
  } T12PR;
  __I  uint16_t  RESERVED9;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C028) Timer T13 Period Register                              */
    
    struct {
      __IO uint16_t  T13PV      : 16;               /*!< [0..15] T13 Period Value                                              */
    } bit;                                          /*!< [16] BitSize                                                          */
  } T13PR;
  __I  uint16_t  RESERVED10;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C02C) Timer T12 Dead-Time Control Register                   */
    
    struct {
      __IO uint16_t  DTM        :  8;               /*!< [0..7] Dead-Time                                                      */
      __IO uint16_t  DTE0       :  1;               /*!< [8..8] Dead-Time Enable Bits                                          */
      __IO uint16_t  DTE1       :  1;               /*!< [9..9] Dead-Time Enable Bits                                          */
      __IO uint16_t  DTE2       :  1;               /*!< [10..10] Dead-Time Enable Bits                                        */
           uint16_t             :  1;
      __I  uint16_t  DTR0       :  1;               /*!< [12..12] Dead-Time Run Indication Bits                                */
      __I  uint16_t  DTR1       :  1;               /*!< [13..13] Dead-Time Run Indication Bits                                */
      __I  uint16_t  DTR2       :  1;               /*!< [14..14] Dead-Time Run Indication Bits                                */
    } bit;                                          /*!< [15] BitSize                                                          */
  } T12DTC;
  __I  uint16_t  RESERVED11;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C030) Timer Control Register 0                               */
    
    struct {
      __IO uint16_t  T12CLK     :  3;               /*!< [0..2] Timer T12 Input Clock Select                                   */
      __IO uint16_t  T12PRE     :  1;               /*!< [3..3] Timer T12 Prescaler Bit                                        */
      __I  uint16_t  T12R       :  1;               /*!< [4..4] Timer T12 Run Bit                                              */
      __I  uint16_t  STE12      :  1;               /*!< [5..5] Timer T12 Shadow Transfer Enable                               */
      __I  uint16_t  CDIR       :  1;               /*!< [6..6] Count Direction of Timer T12                                   */
      __IO uint16_t  CTM        :  1;               /*!< [7..7] T12 Operating Mode                                             */
      __IO uint16_t  T13CLK     :  3;               /*!< [8..10] Timer T13 Input Clock Select                                  */
      __IO uint16_t  T13PRE     :  1;               /*!< [11..11] Timer T13 Prescaler Bit                                      */
      __I  uint16_t  T13R       :  1;               /*!< [12..12] Timer T13 Run Bit                                            */
      __I  uint16_t  STE13      :  1;               /*!< [13..13] Timer T13 Shadow Transfer Enable                             */
    } bit;                                          /*!< [14] BitSize                                                          */
  } TCTR0;
  __I  uint16_t  RESERVED12;
  
  union {
    __I  uint16_t  reg;                             /*!< (@ 0x4000C034) Capture/Compare Register for Channel CC60              */
    
    struct {
      __I  uint16_t  CCV        : 16;               /*!< [0..15] Channel x Capture/Compare Value                               */
    } bit;                                          /*!< [16] BitSize                                                          */
  } CC60R;
  __I  uint16_t  RESERVED13;
  
  union {
    __I  uint16_t  reg;                             /*!< (@ 0x4000C038) Capture/Compare Register for Channel CC61              */
    
    struct {
      __I  uint16_t  CCV        : 16;               /*!< [0..15] Channel x Capture/Compare Value                               */
    } bit;                                          /*!< [16] BitSize                                                          */
  } CC61R;
  __I  uint16_t  RESERVED14;
  
  union {
    __I  uint16_t  reg;                             /*!< (@ 0x4000C03C) Capture/Compare Register for Channel CC62              */
    
    struct {
      __I  uint16_t  CCV        : 16;               /*!< [0..15] Channel x Capture/Compare Value                               */
    } bit;                                          /*!< [16] BitSize                                                          */
  } CC62R;
  __I  uint16_t  RESERVED15;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C040) Capture/Compare T12 Mode Select Register               */
    
    struct {
      __IO uint16_t  MSEL60     :  4;               /*!< [0..3] Capture/Compare Mode Selection                                 */
      __IO uint16_t  MSEL61     :  4;               /*!< [4..7] Capture/Compare Mode Selection                                 */
      __IO uint16_t  MSEL62     :  4;               /*!< [8..11] Capture/Compare Mode Selection                                */
      __IO uint16_t  HSYNC      :  3;               /*!< [12..14] Hall Synchronization                                         */
      __IO uint16_t  DBYP       :  1;               /*!< [15..15] Delay Bypass                                                 */
    } bit;                                          /*!< [16] BitSize                                                          */
  } T12MSEL;
  __I  uint16_t  RESERVED16;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C044) Capture/Compare Interrupt Enable Register              */
    
    struct {
      __IO uint16_t  ENCC60R    :  1;               /*!< [0..0] Capture, Compare-Match Rising Edge Interrupt Enable for
                                                         Channel 0                                                             */
      __IO uint16_t  ENCC60F    :  1;               /*!< [1..1] Capture, Compare-Match Falling Edge Interrupt Enable
                                                         for Channel 0                                                         */
      __IO uint16_t  ENCC61R    :  1;               /*!< [2..2] Capture, Compare-Match Rising Edge Interrupt Enable for
                                                         Channel 1                                                             */
      __IO uint16_t  ENCC61F    :  1;               /*!< [3..3] Capture, Compare-Match Falling Edge Interrupt Enable
                                                         for Channel 1                                                         */
      __IO uint16_t  ENCC62R    :  1;               /*!< [4..4] Capture, Compare-Match Rising Edge Interrupt Enable for
                                                         Channel 2                                                             */
      __IO uint16_t  ENCC62F    :  1;               /*!< [5..5] Capture, Compare-Match Falling Edge Interrupt Enable
                                                         for Channel 2                                                         */
      __IO uint16_t  ENT12OM    :  1;               /*!< [6..6] Enable Interrupt for T12 One-Match                             */
      __IO uint16_t  ENT12PM    :  1;               /*!< [7..7] Enable Interrupt for T12 Period-Match                          */
      __IO uint16_t  ENT13CM    :  1;               /*!< [8..8] Enable Interrupt for T13 Compare-Match                         */
      __IO uint16_t  ENT13PM    :  1;               /*!< [9..9] Enable Interrupt for T13 Period-Match                          */
      __IO uint16_t  ENTRPF     :  1;               /*!< [10..10] Enable Interrupt for Trap Flag                               */
           uint16_t             :  1;
      __IO uint16_t  ENCHE      :  1;               /*!< [12..12] Enable Interrupt for Correct Hall Event                      */
      __IO uint16_t  ENWHE      :  1;               /*!< [13..13] Enable Interrupt for Wrong Hall Event                        */
      __IO uint16_t  ENIDLE     :  1;               /*!< [14..14] Enable Idle                                                  */
      __IO uint16_t  ENSTR      :  1;               /*!< [15..15] Enable Multi-Channel Mode Shadow Transfer Interrupt          */
    } bit;                                          /*!< [16] BitSize                                                          */
  } IEN;
  __I  uint16_t  RESERVED17;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C048) Capture/Compare Interrupt Node Pointer Register        */
    
    struct {
      __IO uint16_t  INPCC60    :  2;               /*!< [0..1] Interrupt Node Pointer for Channel 0 Interrupts                */
      __IO uint16_t  INPCC61    :  2;               /*!< [2..3] Interrupt Node Pointer for Channel 1 Interrupts                */
      __IO uint16_t  INPCC62    :  2;               /*!< [4..5] Interrupt Node Pointer for Channel 2 Interrupts                */
      __IO uint16_t  INPCHE     :  2;               /*!< [6..7] Interrupt Node Pointer for the CHE Interrupt                   */
      __IO uint16_t  INPERR     :  2;               /*!< [8..9] Interrupt Node Pointer for Error Interrupts                    */
      __IO uint16_t  INPT12     :  2;               /*!< [10..11] Interrupt Node Pointer for Timer T12 Interrupts              */
      __IO uint16_t  INPT13     :  2;               /*!< [12..13] Interrupt Node Pointer for Timer T13 Interrupts              */
    } bit;                                          /*!< [14] BitSize                                                          */
  } INP;
  __I  uint16_t  RESERVED18;
  
  union {
    __O  uint16_t  reg;                             /*!< (@ 0x4000C04C) Capture/Compare Interrupt Status Set Register          */
    
    struct {
      __O  uint16_t  SCC60R     :  1;               /*!< [0..0] Set Capture, Compare-Match Rising Edge Flag                    */
      __O  uint16_t  SCC60F     :  1;               /*!< [1..1] Set Capture, Compare-Match Falling Edge Flag                   */
      __O  uint16_t  SCC61R     :  1;               /*!< [2..2] Set Capture, Compare-Match Rising Edge Flag                    */
      __O  uint16_t  SCC61F     :  1;               /*!< [3..3] Set Capture, Compare-Match Falling Edge Flag                   */
      __O  uint16_t  SCC62R     :  1;               /*!< [4..4] Set Capture, Compare-Match Rising Edge Flag                    */
      __O  uint16_t  SCC62F     :  1;               /*!< [5..5] Set Capture, Compare-Match Falling Edge Flag                   */
      __O  uint16_t  ST12OM     :  1;               /*!< [6..6] Set Timer T12 One-Match Flag                                   */
      __O  uint16_t  ST12PM     :  1;               /*!< [7..7] Set Timer T12 Period-Match Flag                                */
      __O  uint16_t  ST13CM     :  1;               /*!< [8..8] Set Timer T13 Compare-Match Flag                               */
      __O  uint16_t  ST13PM     :  1;               /*!< [9..9] Set Timer T13 Period-Match Flag                                */
      __O  uint16_t  STRPF      :  1;               /*!< [10..10] Set Trap Flag                                                */
      __O  uint16_t  SWHC       :  1;               /*!< [11..11] Software Hall Compare                                        */
      __O  uint16_t  SCHE       :  1;               /*!< [12..12] Set Correct Hall Event Flag                                  */
      __O  uint16_t  SWHE       :  1;               /*!< [13..13] Set Wrong Hall Event Flag                                    */
      __O  uint16_t  SIDLE      :  1;               /*!< [14..14] Set IDLE Flag                                                */
      __O  uint16_t  SSTR       :  1;               /*!< [15..15] Set STR Flag                                                 */
    } bit;                                          /*!< [16] BitSize                                                          */
  } ISS;
  __I  uint16_t  RESERVED19;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C050) Passive State Level Register                           */
    
    struct {
      __IO uint16_t  PSL        :  6;               /*!< [0..5] Compare Outputs Passive State Level                            */
           uint16_t             :  1;
      __IO uint16_t  PSL63      :  1;               /*!< [7..7] Passive State Level of Output COUT63                           */
    } bit;                                          /*!< [8] BitSize                                                           */
  } PSLR;
  __I  uint16_t  RESERVED20;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C054) Multi-Channel Mode Control Register                    */
    
    struct {
      __IO uint16_t  SWSEL      :  3;               /*!< [0..2] Switching Selection                                            */
           uint16_t             :  1;
      __IO uint16_t  SWSYN      :  2;               /*!< [4..5] Switching Synchronization                                      */
           uint16_t             :  2;
      __IO uint16_t  STE12U     :  1;               /*!< [8..8] Shadow Transfer Enable for T12 Upcounting                      */
      __IO uint16_t  STE12D     :  1;               /*!< [9..9] Shadow Transfer Enable for T12 Downcounting                    */
      __IO uint16_t  STE13U     :  1;               /*!< [10..10] Shadow Transfer Enable for T13 Upcounting                    */
    } bit;                                          /*!< [11] BitSize                                                          */
  } MCMCTR;
  __I  uint16_t  RESERVED21;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C058) Timer Control Register 2                               */
    
    struct {
      __IO uint16_t  T12SSC     :  1;               /*!< [0..0] Timer T12 Single Shot Control                                  */
      __IO uint16_t  T13SSC     :  1;               /*!< [1..1] Timer T13 Single Shot Control                                  */
      __IO uint16_t  T13TEC     :  3;               /*!< [2..4] T13 Trigger Event Control                                      */
      __IO uint16_t  T13TED     :  2;               /*!< [5..6] Timer T13 Trigger Event Direction                              */
           uint16_t             :  1;
      __IO uint16_t  T12RSEL    :  2;               /*!< [8..9] Timer T12 External Run Selection                               */
      __IO uint16_t  T13RSEL    :  2;               /*!< [10..11] Timer T13 External Run Selection                             */
    } bit;                                          /*!< [12] BitSize                                                          */
  } TCTR2;
  __I  uint16_t  RESERVED22;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C05C) Modulation Control Register                            */
    
    struct {
      __IO uint16_t  T12MODEN   :  6;               /*!< [0..5] T12 Modulation Enable                                          */
           uint16_t             :  1;
      __IO uint16_t  MCMEN      :  1;               /*!< [7..7] Multi-Channel Mode Enable                                      */
      __IO uint16_t  T13MODEN   :  6;               /*!< [8..13] T13 Modulation Enable                                         */
           uint16_t             :  1;
      __IO uint16_t  ECT13O     :  1;               /*!< [15..15] Enable Compare Timer T13 Output                              */
    } bit;                                          /*!< [16] BitSize                                                          */
  } MODCTR;
  __I  uint16_t  RESERVED23;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C060) Trap Control Register                                  */
    
    struct {
      __IO uint16_t  TRPM0      :  1;               /*!< [0..0] Trap Mode Control Bits 1, 0                                    */
      __IO uint16_t  TRPM1      :  1;               /*!< [1..1] Trap Mode Control Bits 1, 0                                    */
      __IO uint16_t  TRPM2      :  1;               /*!< [2..2] Trap Mode Control Bit 2                                        */
           uint16_t             :  5;
      __IO uint16_t  TRPEN      :  6;               /*!< [8..13] Trap Enable Control                                           */
      __IO uint16_t  TRPEN13    :  1;               /*!< [14..14] Trap Enable Control for Timer T13                            */
      __IO uint16_t  TRPPEN     :  1;               /*!< [15..15] Trap Pin Enable                                              */
    } bit;                                          /*!< [16] BitSize                                                          */
  } TRPCTR;
  __I  uint16_t  RESERVED24;
  
  union {
    __I  uint16_t  reg;                             /*!< (@ 0x4000C064) Multi-Channel Mode Output Register                     */
    
    struct {
      __I  uint16_t  MCMP       :  6;               /*!< [0..5] Multi-Channel PWM Pattern                                      */
      __I  uint16_t  R          :  1;               /*!< [6..6] Reminder Flag                                                  */
           uint16_t             :  1;
      __I  uint16_t  EXPH       :  3;               /*!< [8..10] Expected Hall Pattern                                         */
      __I  uint16_t  CURH       :  3;               /*!< [11..13] Current Hall Pattern                                         */
    } bit;                                          /*!< [14] BitSize                                                          */
  } MCMOUT;
  __I  uint16_t  RESERVED25;
  
  union {
    __I  uint16_t  reg;                             /*!< (@ 0x4000C068) Capture/Compare Interrupt Status Register              */
    
    struct {
      __I  uint16_t  ICC60R     :  1;               /*!< [0..0] Capture, Compare-Match Rising Edge Flag (x = 0, 1, 2)          */
      __I  uint16_t  ICC60F     :  1;               /*!< [1..1] Capture, Compare-Match Falling Edge Flag (x = 0, 1, 2)         */
      __I  uint16_t  ICC61R     :  1;               /*!< [2..2] Capture, Compare-Match Rising Edge Flag (x = 0, 1, 2)          */
      __I  uint16_t  ICC61F     :  1;               /*!< [3..3] Capture, Compare-Match Falling Edge Flag (x = 0, 1, 2)         */
      __I  uint16_t  ICC62R     :  1;               /*!< [4..4] Capture, Compare-Match Rising Edge Flag (x = 0, 1, 2)          */
      __I  uint16_t  ICC62F     :  1;               /*!< [5..5] Capture, Compare-Match Falling Edge Flag (x = 0, 1, 2)         */
      __I  uint16_t  T12OM      :  1;               /*!< [6..6] Timer T12 One-Match Flag                                       */
      __I  uint16_t  T12PM      :  1;               /*!< [7..7] Timer T12 Period-Match Flag                                    */
      __I  uint16_t  T13CM      :  1;               /*!< [8..8] Timer T13 Compare-Match Flag                                   */
      __I  uint16_t  T13PM      :  1;               /*!< [9..9] Timer T13 Period-Match Flag                                    */
      __I  uint16_t  TRPF       :  1;               /*!< [10..10] Trap Flag                                                    */
      __I  uint16_t  TRPS       :  1;               /*!< [11..11] Trap State                                                   */
      __I  uint16_t  CHE        :  1;               /*!< [12..12] Correct Hall Event                                           */
      __I  uint16_t  WHE        :  1;               /*!< [13..13] Wrong Hall Event                                             */
      __I  uint16_t  IDLE       :  1;               /*!< [14..14] IDLE State                                                   */
      __I  uint16_t  STR        :  1;               /*!< [15..15] Multi-Channel Mode Shadow Transfer Request                   */
    } bit;                                          /*!< [16] BitSize                                                          */
  } IS;
  __I  uint16_t  RESERVED26;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C06C) Port Input Select Register 0                           */
    
    struct {
      __IO uint16_t  ISCC60     :  2;               /*!< [0..1] Input Select for CC60                                          */
      __IO uint16_t  ISCC61     :  2;               /*!< [2..3] Input Select for CC61                                          */
      __IO uint16_t  ISCC62     :  2;               /*!< [4..5] Input Select for CC62                                          */
      __IO uint16_t  ISTRP      :  2;               /*!< [6..7] Input Select for CTRAP                                         */
      __IO uint16_t  ISPOS0     :  2;               /*!< [8..9] Input Select for CCPOS0                                        */
      __IO uint16_t  ISPOS1     :  2;               /*!< [10..11] Input Select for CCPOS1                                      */
      __IO uint16_t  ISPOS2     :  2;               /*!< [12..13] Input Select for CCPOS2                                      */
      __IO uint16_t  IST12HR    :  2;               /*!< [14..15] Input Select for T12HR                                       */
    } bit;                                          /*!< [16] BitSize                                                          */
  } PISEL0;
  __I  uint16_t  RESERVED27[3];
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C074) Port Input Select Register 2                           */
    
    struct {
      __IO uint16_t  IST13HR    :  2;               /*!< [0..1] Input Select for T13HR                                         */
      __IO uint16_t  ISCNT12    :  2;               /*!< [2..3] Input Select for T12 Counting Input                            */
      __IO uint16_t  ISCNT13    :  2;               /*!< [4..5] Input Select for T13 Counting Input                            */
      __IO uint16_t  T12EXT     :  1;               /*!< [6..6] Extension for T12HR Inputs                                     */
      __IO uint16_t  T13EXT     :  1;               /*!< [7..7] Extension for T13HR Inputs                                     */
    } bit;                                          /*!< [8] BitSize                                                           */
  } PISEL2;
  __I  uint16_t  RESERVED28;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C078) Timer T12 Counter Register                             */
    
    struct {
      __IO uint16_t  T12CV      : 16;               /*!< [0..15] Timer T12 Counter Value                                       */
    } bit;                                          /*!< [16] BitSize                                                          */
  } T12;
  __I  uint16_t  RESERVED29;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C07C) Timer T13 Counter Register                             */
    
    struct {
      __IO uint16_t  T13CV      : 16;               /*!< [0..15] Timer T13 Counter Value                                       */
    } bit;                                          /*!< [16] BitSize                                                          */
  } T13;
  __I  uint16_t  RESERVED30;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x4000C080) Compare State Register                                 */
    
    struct {
      __I  uint16_t  CC60ST     :  1;               /*!< [0..0] Capture/Compare State Bits (x = 0, 1, 2, 3)                    */
      __I  uint16_t  CC61ST     :  1;               /*!< [1..1] Capture/Compare State Bits (x = 0, 1, 2, 3)                    */
      __I  uint16_t  CC62ST     :  1;               /*!< [2..2] Capture/Compare State Bits (x = 0, 1, 2, 3)                    */
      __I  uint16_t  CCPOS0     :  1;               /*!< [3..3] Sampled Hall Pattern Bits (x = 0, 1, 2)                        */
      __I  uint16_t  CCPOS1     :  1;               /*!< [4..4] Sampled Hall Pattern Bits (x = 0, 1, 2)                        */
      __I  uint16_t  CCPOS2     :  1;               /*!< [5..5] Sampled Hall Pattern Bits (x = 0, 1, 2)                        */
      __I  uint16_t  CC63ST     :  1;               /*!< [6..6] Capture/Compare State Bits (x = 0, 1, 2, 3)                    */
           uint16_t             :  1;
      __IO uint16_t  CC60PS     :  1;               /*!< [8..8] Passive State Select for Compare Outputs                       */
      __IO uint16_t  COUT60PS   :  1;               /*!< [9..9] Passive State Select for Compare Outputs                       */
      __IO uint16_t  CC61PS     :  1;               /*!< [10..10] Passive State Select for Compare Outputs                     */
      __IO uint16_t  COUT61PS   :  1;               /*!< [11..11] Passive State Select for Compare Outputs                     */
      __IO uint16_t  CC62PS     :  1;               /*!< [12..12] Passive State Select for Compare Outputs                     */
      __IO uint16_t  COUT62PS   :  1;               /*!< [13..13] Passive State Select for Compare Outputs                     */
      __IO uint16_t  COUT63PS   :  1;               /*!< [14..14] Passive State Select for Compare Outputs                     */
      __IO uint16_t  T13IM      :  1;               /*!< [15..15] T13 Inverted Modulation                                      */
    } bit;                                          /*!< [16] BitSize                                                          */
  } CMPSTAT;
} CCU6_Type;


/* ================================================================================ */
/* ================                      UARTx                     ================ */
/* ================================================================================ */


/**
  * @brief UARTx (UARTx)
  */

typedef struct {                                    /*!< (@ 0x48022000) UART2 Structure                                        */
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48022000) Serial Channel Control Register                        */
    
    struct {
      __IO uint8_t   RI         :  1;               /*!< [0..0] Receive Interrupt Flag                                         */
      __IO uint8_t   TI         :  1;               /*!< [1..1] Transmit Interrupt Flag                                        */
      __IO uint8_t   RB8        :  1;               /*!< [2..2] Serial Port Receiver Bit 9                                     */
      __IO uint8_t   TB8        :  1;               /*!< [3..3] Serial Port Transmitter Bit 9                                  */
      __IO uint8_t   REN        :  1;               /*!< [4..4] Enable Receiver of Serial Port                                 */
      __IO uint8_t   SM2        :  1;               /*!< [5..5] Enable Serial Port Multiprocessor Communication in Modes
                                                         2 and 3                                                               */
      __IO uint8_t   SM1        :  1;               /*!< [6..6] Serial Port Operating Mode Selection                           */
      __IO uint8_t   SM0        :  1;               /*!< [7..7] Serial Port Operating Mode Selection                           */
    } bit;                                          /*!< [8] BitSize                                                           */
  } SCON;
  __I  uint8_t   RESERVED0[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x48022004) Serial Data Buffer                                     */
    
    struct {
      __IO uint8_t   VAL        :  8;               /*!< [0..7] Serial Interface Buffer Register                               */
    } bit;                                          /*!< [8] BitSize                                                           */
  } SBUF;
  __I  uint8_t   RESERVED1[3];
  
  union {
    __O  uint8_t   reg;                             /*!< (@ 0x48022008) Serial Channel Control Clear Register                  */
    
    struct {
      __O  uint8_t   RICLR      :  1;               /*!< [0..0] Receive Interrupt Clear Flag                                   */
      __O  uint8_t   TICLR      :  1;               /*!< [1..1] Transmit Interrupt Clear Flag                                  */
    } bit;                                          /*!< [2] BitSize                                                           */
  } SCONCLR;
} UART_Type;



/* ================================================================================ */
/* ================                      SSCx                      ================ */
/* ================================================================================ */


/**
  * @brief SSCx (SSCx)
  */

typedef struct {                                    /*!< (@ 0x48026000) SSC2 Structure                                         */
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x48026000) Port Input Select Register, RESET_TYPE_3               */
    
    struct {
      __IO uint16_t  MIS_0      :  1;               /*!< [0..0] Master Mode Receiver Input Select: 0b0=see (SSC1)., 0b1=see
                                                         (SSC1)., 0b0=Receiver input (Port A: P1.2) is selected (SSC2).,
                                                          0b1=Receiver input (Port B: P2.5) is selected (SSC2).,               */
      __IO uint16_t  SIS        :  1;               /*!< [1..1] Slave Mode Receiver Input Select: 0b0=Receiver input
                                                         (Port A: P0.2) is selected (SSC1)., 0b1=Receiver input (Port
                                                          B: P0.2) is selected (SSC1)., 0b0=Receiver input (Port A: P1.1)
                                                          is selected (SSC2)., 0b1=Receiver input (Port B: P1.1) is selected
                                                          (SSC2).,                                                             */
      __IO uint16_t  CIS        :  1;               /*!< [2..2] Slave Mode Clock Input Select: 0b0=Clock input (Port
                                                         A: P0.3) is selected (SSC1)., 0b1=Clock input (Port B: P0.3)
                                                          is selected (SSC1)., 0b0=Clock input (Port A: P1.0) is selected
                                                          (SSC2)., 0b1=Clock input (Port B: P1.0) is selected (SSC2).,
                                                                                                                               */
      __IO uint16_t  MIS_1      :  1;               /*!< [3..3] Master Mode Receiver Input Select: 0b0=see (SSC1)., 0b1=see
                                                         (SSC1)., 0b0=n/a (SSC2)., 0b1=n/a (SSC2).,                            */
    } bit;                                          /*!< [4] BitSize                                                           */
  } PISEL;
  __I  uint16_t  RESERVED0;
  
	union {
	  __IO uint16_t  reg;                           /*!< (@ 0x48026004) Control Register                                       */
	  
	  struct {
	    __I  uint16_t  BC       :  4;               /*!< [0..3] Bit Count Field                                                */
	         uint16_t           :  4;
	    __IO uint16_t  TE       :  1;               /*!< [8..8] Transmit Error Flag                                            */
	    __IO uint16_t  RE       :  1;               /*!< [9..9] Receive Error Flag                                             */
	    __IO uint16_t  PE       :  1;               /*!< [10..10] Phase Error Flag                                             */
	    __IO uint16_t  BE       :  1;               /*!< [11..11] Baud Rate Error Flag                                         */
	    __I  uint16_t  BSY      :  1;               /*!< [12..12] Busy Flag                                                    */
	         uint16_t           :  1;
	    __IO uint16_t  MS       :  1;               /*!< [14..14] Master Select Bit                                            */
	    __IO uint16_t  EN       :  1;               /*!< [15..15] Enable Bit = 1                                               */
	  } RUN_bit;                                    /*!< [16] BitSize                                                          */
	
	  struct {
	    __IO uint16_t  BM       :  4;               /*!< [0..3] Data Width Selection                                           */
	    __IO uint16_t  HB       :  1;               /*!< [4..4] Heading Control                                                */
	    __IO uint16_t  PH       :  1;               /*!< [5..5] Clock Phase Control                                            */
	    __IO uint16_t  PO       :  1;               /*!< [6..6] Clock Polarity Control                                         */
	    __IO uint16_t  LB       :  1;               /*!< [7..7] Loop Back Control                                              */
	    __IO uint16_t  TEN      :  1;               /*!< [8..8] Transmit Error Enable                                          */
	    __IO uint16_t  REN      :  1;               /*!< [9..9] Receive Error Enable                                           */
	    __IO uint16_t  PEN      :  1;               /*!< [10..10] Phase Error Enable                                           */
	    __IO uint16_t  BEN      :  1;               /*!< [11..11] Baud Rate Error Enable                                       */
	    __IO uint16_t  AREN     :  1;               /*!< [12..12] Automatic Reset Enable                                       */
	         uint16_t           :  1;
	    __IO uint16_t  MS       :  1;               /*!< [14..14] Master Select                                                */
	    __IO uint16_t  EN       :  1;               /*!< [15..15] Enable Bit = 0                                               */
	  } PRG_bit;                                    /*!< [16] BitSize                                                          */
	} CON;
  __I  uint16_t  RESERVED1;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x48026008) Transmitter Buffer Register                            */
    
    struct {
      __IO uint16_t  TB_VALUE   : 16;               /*!< [0..15] Transmit Data Register Value                                  */
    } bit;                                          /*!< [16] BitSize                                                          */
  } TB;
  __I  uint16_t  RESERVED2;
  
  union {
    __I  uint16_t  reg;                             /*!< (@ 0x4802600C) Receiver Buffer Register                               */
    
    struct {
      __I  uint16_t  RB_VALUE   : 16;               /*!< [0..15] Receive Data Register Value                                   */
    } bit;                                          /*!< [16] BitSize                                                          */
  } RB;
  __I  uint16_t  RESERVED3;
  
  union {
    __IO uint16_t  reg;                             /*!< (@ 0x48026010) Baud Rate Timer Reload Register                        */
    
    struct {
      __IO uint16_t  BR_VALUE   : 16;               /*!< [0..15] Baud Rate Timer/Reload Register Value                         */
    } bit;                                          /*!< [16] BitSize                                                          */
  } BR;
  __I  uint16_t  RESERVED4;
  
  union {
    __O  uint16_t  reg;                             /*!< (@ 0x48026014) Interrupt Status Register Clear                        */
    
    struct {
           uint16_t             :  8;
      __O  uint16_t  TECLR      :  1;               /*!< [8..8] Transmit Error Flag Clear                                      */
      __O  uint16_t  RECLR      :  1;               /*!< [9..9] Receive Error Flag Clear                                       */
      __O  uint16_t  PECLR      :  1;               /*!< [10..10] Phase Error Flag Clear                                       */
      __O  uint16_t  BECLR      :  1;               /*!< [11..11] Baud Rate Error Flag Clear                                   */
    } bit;                                          /*!< [12] BitSize                                                          */
  } ISRCLR;
} SSC_Type;


/* ================================================================================ */
/* ================                       PMU                      ================ */
/* ================================================================================ */


/**
  * @brief PMU (PMU)
  */

typedef struct {                                    /*!< (@ 0x50004000) PMU Structure                                          */
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x50004000) Main wake status register                              */
    
    struct {
      __I  uint8_t   LIN_WAKE   :  1;               /*!< [0..0] Wake-up via LIN- Message                                       */
      __I  uint8_t   MON_WAKE   :  1;               /*!< [1..1] Wake-up via MON                                                */
      __I  uint8_t   GPIO0      :  1;               /*!< [2..2] Wake-up via GPIO0 which is a logical OR combination of
                                                         all Wake_STS_GPIO0 bits                                               */
      __I  uint8_t   GPIO1      :  1;               /*!< [3..3] Wake-up via GPIO1 which is a logical OR combination of
                                                         all Wake_STS_GPIO1 bits                                               */
      __I  uint8_t   CYC_WAKE   :  1;               /*!< [4..4] Wake-up caused by Cyclic Wake                                  */
      __I  uint8_t   FAIL       :  1;               /*!< [5..5] Wake-up after VDDEXT Fail                                      */
    } bit;                                          /*!< [6] BitSize                                                           */
  } WAKE_STATUS;
  __I  uint8_t   RESERVED0[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50004004) Voltage Reg Status Register                            */
    
    struct {
      __I  uint8_t   PMU_1V5_OVERVOLT:  1;          /*!< [0..0] Overvoltage at VDDC regulator                                  */
      __I  uint8_t   PMU_1V5_OVERLOAD:  1;          /*!< [1..1] Overload at VDDC regulator                                     */
      __IO uint8_t   PMU_1V5_FAIL_EN:  1;           /*!< [2..2] Enabling of VDDC status information as interrupt source        */
           uint8_t              :  1;
      __I  uint8_t   PMU_5V_OVERVOLT:  1;           /*!< [4..4] Overvoltage at VDDP regulator                                  */
      __I  uint8_t   PMU_5V_OVERLOAD:  1;           /*!< [5..5] Overload at VDDP regulator                                     */
      __IO uint8_t   PMU_5V_FAIL_EN:  1;            /*!< [6..6] Enabling of VDDP status information as interrupt source        */
    } bit;                                          /*!< [7] BitSize                                                           */
  } PMU_SUPPLY_STS;
  __I  uint8_t   RESERVED1[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50004008) VDDEXT Control                                         */
    
    struct {
      __IO uint8_t   ENABLE     :  1;               /*!< [0..0] VDDEXT Supply Enable                                           */
      __IO uint8_t   CYC_EN     :  1;               /*!< [1..1] VDDEXT Supply for Cyclic Sense Enable                          */
      __IO uint8_t   FAIL_EN    :  1;               /*!< [2..2] Enabling of VDDEXT Supply status information as interrupt
                                                         source                                                                */
      __IO uint8_t   SHORT      :  1;               /*!< [3..3] VDDEXT Supply Shorted Output                                   */
      __IO uint8_t   OVERVOLT   :  1;               /*!< [4..4] VDDEXT Supply Overvoltage                                      */
      __IO uint8_t   OVERLOAD   :  1;               /*!< [5..5] VDDEXT Supply Overload                                         */
      __I  uint8_t   OK         :  1;               /*!< [6..6] VDDEXT Supply works inside its specified range 2               */
      __I  uint8_t   STABLE     :  1;               /*!< [7..7] VDDEXT Supply works inside its specified range 1               */
    } bit;                                          /*!< [8] BitSize                                                           */
  } VDDEXT_CTRL;
  __I  uint8_t   RESERVED2[7];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50004010) Reset Status Hard Register                             */
    
    struct {
      __IO uint8_t   SYS_FAIL   :  1;               /*!< [0..0] Flag which indicates a reset caused by a System Fail
                                                         reported in the corresponding Fail Register                           */
      __IO uint8_t   PMU_WAKE   :  1;               /*!< [1..1] Flag which indicates a reset caused by Stop-Exit               */
      __IO uint8_t   PMU_SleepEX:  1;               /*!< [2..2] Flag which indicates a reset caused by Sleep-Exit              */
      __IO uint8_t   PMU_LPR    :  1;               /*!< [3..3] Low Priority Resets (see PMU_RESET_STS2)                       */
      __IO uint8_t   PMU_ClkWDT :  1;               /*!< [4..4] Clock Watchdog (CLKWDT) Reset Flag                             */
      __IO uint8_t   PMU_ExtWDT :  1;               /*!< [5..5] External Watchdog (WDT1) Reset Flag                            */
      __IO uint8_t   PMU_PIN    :  1;               /*!< [6..6] PIN-Reset Flag                                                 */
      __IO uint8_t   PMU_1V5DidPOR:  1;             /*!< [7..7] Power-On Reset Flag                                            */
    } bit;                                          /*!< [8] BitSize                                                           */
  } PMU_RESET_STS1;
  __I  uint8_t   RESERVED3[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50004014) Reset Status Soft Register                             */
    
    struct {
      __IO uint8_t   PMU_IntWDT :  1;               /*!< [0..0] Internal Watchdog Reset Flag                                   */
      __IO uint8_t   PMU_SOFT   :  1;               /*!< [1..1] Soft-Reset Flag                                                */
      __IO uint8_t   LOCKUP     :  1;               /*!< [2..2] Lockup-Reset Flag                                              */
    } bit;                                          /*!< [3] BitSize                                                           */
  } PMU_RESET_STS2;
  __I  uint8_t   RESERVED4[11];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50004020) PMU Settings Register                                  */
    
    struct {
      __IO uint8_t   WAKE_W_RST :  1;               /*!< [0..0] Wake-up with reset execution                                   */
      __IO uint8_t   EN_0V9_N   :  1;               /*!< [1..1] Disables the reduction of the VDDC regulator output to
                                                         0.9 V during Stop-Mode                                                */
      __IO uint8_t   CYC_WAKE_EN:  1;               /*!< [2..2] Enabling Cyclic Wake                                           */
      __IO uint8_t   CYC_SENSE_EN:  1;              /*!< [3..3] Enabling Cyclic Sense                                          */
           uint8_t              :  3;
      __IO uint8_t   EN_VDDEXT_OC_OFF_N:  1;        /*!< [7..7] Disabling VDDEXT Shutdown in Overload Condition                */
    } bit;                                          /*!< [8] BitSize                                                           */
  } CNF_PMU_SETTINGS;
  __I  uint8_t   RESERVED5[7];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50004028) Dead Time in Cyclic Sense Register                     */
    
    struct {
      __IO uint8_t   M03        :  4;               /*!< [0..3] Mantissa                                                       */
      __IO uint8_t   E01        :  2;               /*!< [4..5] Exponent                                                       */
           uint8_t              :  1;
      __IO uint8_t   OSC_100kHz_EN:  1;             /*!< [7..7] 100 kHz Oscillator Enable                                      */
    } bit;                                          /*!< [8] BitSize                                                           */
  } CNF_CYC_SENSE;
  __I  uint8_t   RESERVED6[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x5000402C) Dead Time in Cyclic Wake Register                      */
    
    struct {
      __IO uint8_t   M03        :  4;               /*!< [0..3] Mantissa                                                       */
      __IO uint8_t   E01        :  2;               /*!< [4..5] Exponent                                                       */
    } bit;                                          /*!< [6] BitSize                                                           */
  } CNF_CYC_WAKE;
  __I  uint8_t   RESERVED7[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50004030) Sample Delay in Cyclic Sense Register                  */
    
    struct {
      __IO uint8_t   M03        :  4;               /*!< [0..3] Mantissa                                                       */
    } bit;                                          /*!< [4] BitSize                                                           */
  } CNF_CYC_SAMPLE_DEL;
  __I  uint8_t   RESERVED8[31];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50004050) LIN Wake Enable                                        */
    
    struct {
           uint8_t              :  7;
      __IO uint8_t   LIN_EN     :  1;               /*!< [7..7] Lin Wake enable                                                */
    } bit;                                          /*!< [8] BitSize                                                           */
  } LIN_WAKE_EN;
  __I  uint8_t   RESERVED9[27];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x5000406C) Reset Blind Time Register                              */
    
    struct {
      __IO uint8_t   RST_TFB    :  2;               /*!< [0..1] Reset Pin Blind Time Selection Bits                            */
    } bit;                                          /*!< [2] BitSize                                                           */
  } CNF_RST_TFB;
  __I  uint8_t   RESERVED10[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50004070) System Fail Status Register                            */
    
    struct {
      __IO uint8_t   SUPP_SHORT :  1;               /*!< [0..0] Supply Short                                                   */
      __IO uint8_t   SUPP_TMOUT :  1;               /*!< [1..1] Supply Time Out                                                */
      __IO uint8_t   PMU_1V5_OVL:  1;               /*!< [2..2] VDDC Overload Flag                                             */
      __IO uint8_t   PMU_5V_OVL :  1;               /*!< [3..3] VDDP Overload Flag                                             */
           uint8_t              :  1;
      __IO uint8_t   SYS_OT     :  1;               /*!< [5..5] System Overtemperature Indication Flag                         */
      __IO uint8_t   WDT1_SEQ_FAIL:  1;             /*!< [6..6] External Watchdog (WDT1) Sequential Fail                       */
    } bit;                                          /*!< [7] BitSize                                                           */
  } SYS_FAIL_STS;
  __I  uint8_t   RESERVED11[15];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50004080) Wake Status Fail Register                              */
    
    struct {
      __IO uint8_t   SUPPFAIL   :  1;               /*!< [0..0] Stop-Exit due to overvoltage at the VDDEXT Supply              */
           uint8_t              :  1;
      __IO uint8_t   VDDEXTSHORT:  1;               /*!< [2..2] Stop-Exit due to short circuit at the VDDEXT Supply            */
    } bit;                                          /*!< [3] BitSize                                                           */
  } WAKE_STS_FAIL;
  __I  uint8_t   RESERVED12[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x50004084) Wake Source MON Input Register                         */
    
    struct {
      __I  uint8_t   WAKE_STS   :  1;               /*!< [0..0] Status of MON                                                  */
    } bit;                                          /*!< [1] BitSize                                                           */
  } WAKE_STS_MON;
  __I  uint8_t   RESERVED13[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x50004088) Wake Status GPIO 0 Register                            */
    
    struct {
      __I  uint8_t   GPIO0_STS_0:  1;               /*!< [0..0] Status of GPIO0_0                                              */
      __I  uint8_t   GPIO0_STS_1:  1;               /*!< [1..1] Status of GPIO0_1                                              */
      __I  uint8_t   GPIO0_STS_2:  1;               /*!< [2..2] Status of GPIO0_2                                              */
      __I  uint8_t   GPIO0_STS_3:  1;               /*!< [3..3] Status of GPIO0_3                                              */
      __I  uint8_t   GPIO0_STS_4:  1;               /*!< [4..4] Status of GPIO0_4                                              */
    } bit;                                          /*!< [5] BitSize                                                           */
  } WAKE_STS_GPIO0;
  __I  uint8_t   RESERVED14[3];
  
  union {
    __I  uint8_t   reg;                             /*!< (@ 0x5000408C) Wake Status GPIO 1 Register                            */
    
    struct {
      __I  uint8_t   GPIO1_STS_0:  1;               /*!< [0..0] Wake GPIO1_0                                                   */
      __I  uint8_t   GPIO1_STS_1:  1;               /*!< [1..1] Wake GPIO1_1                                                   */
      __I  uint8_t   GPIO1_STS_2:  1;               /*!< [2..2] Wake GPIO1_2                                                   */
      __I  uint8_t   GPIO1_STS_3:  1;               /*!< [3..3] Wake GPIO1_3                                                   */
      __I  uint8_t   GPIO1_STS_4:  1;               /*!< [4..4] Wake GPIO1_4                                                   */
    } bit;                                          /*!< [5] BitSize                                                           */
  } WAKE_STS_GPIO1;
  __I  uint8_t   RESERVED15[31];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500040AC) PMU Wake-up Timing Register                            */
    
    struct {
      __IO uint8_t   CNF_LIN_FT :  1;               /*!< [0..0] Wake-up Filter time for LIN WAKE                               */
      __IO uint8_t   CNF_MON_FT :  1;               /*!< [1..1] Wake-up Filter time for Monitoring Inputs                      */
      __IO uint8_t   CNF_GPIO_FT:  2;               /*!< [2..3] Wake-up Filter time for General Purpose IO                     */
    } bit;                                          /*!< [4] BitSize                                                           */
  } CNF_WAKE_FILTER;
  __I  uint8_t   RESERVED16[19];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500040C0) General Purpose User DATA0                             */
    
    struct {
      __IO uint8_t   DATA0      :  8;               /*!< [0..7] DATA0 Storage Byte                                             */
    } bit;                                          /*!< [8] BitSize                                                           */
  } GPUDATA00;
  __I  uint8_t   RESERVED17[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500040C4) General Purpose User DATA1                             */
    
    struct {
      __IO uint8_t   DATA1      :  8;               /*!< [0..7] DATA1 Storage Byte                                             */
    } bit;                                          /*!< [8] BitSize                                                           */
  } GPUDATA01;
  __I  uint8_t   RESERVED18[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500040C8) General Purpose User DATA2                             */
    
    struct {
      __IO uint8_t   DATA2      :  8;               /*!< [0..7] DATA2 Storage Byte                                             */
    } bit;                                          /*!< [8] BitSize                                                           */
  } GPUDATA02;
  __I  uint8_t   RESERVED19[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500040CC) General Purpose User DATA3                             */
    
    struct {
      __IO uint8_t   DATA3      :  8;               /*!< [0..7] DATA3 Storage Byte                                             */
    } bit;                                          /*!< [8] BitSize                                                           */
  } GPUDATA03;
  __I  uint8_t   RESERVED20[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500040D0) General Purpose User DATA4                             */
    
    struct {
      __IO uint8_t   DATA4      :  8;               /*!< [0..7] DATA4 Storage Byte                                             */
    } bit;                                          /*!< [8] BitSize                                                           */
  } GPUDATA04;
  __I  uint8_t   RESERVED21[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500040D4) General Purpose User DATA5                             */
    
    struct {
      __IO uint8_t   DATA5      :  8;               /*!< [0..7] DATA5 Storage Byte                                             */
    } bit;                                          /*!< [8] BitSize                                                           */
  } GPUDATA05;
  __I  uint8_t   RESERVED22[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500040D8) Wake Configuration GPIO Port 0 Rising Edge Register    */
    
    struct {
      __IO uint8_t   GPIO0_RI_0 :  1;               /*!< [0..0] Port 0_0 Wake-up on Rising Edge enable                         */
      __IO uint8_t   GPIO0_RI_1 :  1;               /*!< [1..1] Port 0_1 Wake-up on Rising Edge enable                         */
      __IO uint8_t   GPIO0_RI_2 :  1;               /*!< [2..2] Port 0_2 Wake-up on Rising Edge enable                         */
      __IO uint8_t   GPIO0_RI_3 :  1;               /*!< [3..3] Port 0_3 Wake-up on Rising Edge enable                         */
      __IO uint8_t   GPIO0_RI_4 :  1;               /*!< [4..4] Port 0_4 Wake-up on Rising Edge enable                         */
    } bit;                                          /*!< [5] BitSize                                                           */
  } WAKE_CONF_GPIO0_RISE;
  __I  uint8_t   RESERVED23[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500040DC) Wake Configuration GPIO Port 0 Falling Edge Register   */
    
    struct {
      __IO uint8_t   GPIO0_FA_0 :  1;               /*!< [0..0] Port 0_0 Wake-up on Falling Edge enable                        */
      __IO uint8_t   GPIO0_FA_1 :  1;               /*!< [1..1] Port 0_1 Wake-up on Falling Edge enable                        */
      __IO uint8_t   GPIO0_FA_2 :  1;               /*!< [2..2] Port 0_2 Wake-up on Falling Edge enable                        */
      __IO uint8_t   GPIO0_FA_3 :  1;               /*!< [3..3] Port 0_3 Wake-up on Falling Edge enable                        */
      __IO uint8_t   GPIO0_FA_4 :  1;               /*!< [4..4] Port 0_4 Wake-up on Falling Edge enable                        */
    } bit;                                          /*!< [5] BitSize                                                           */
  } WAKE_CONF_GPIO0_FALL;
  __I  uint8_t   RESERVED24[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500040E0) Wake Port 0 Cycle Enabled Register                     */
    
    struct {
      __IO uint8_t   GPIO0_CYC_0:  1;               /*!< [0..0] GPIO0_0 input for cycle sense enable                           */
      __IO uint8_t   GPIO0_CYC_1:  1;               /*!< [1..1] GPIO0_1 input for cycle sense enable                           */
      __IO uint8_t   GPIO0_CYC_2:  1;               /*!< [2..2] GPIO0_2 input for cycle sense enable                           */
      __IO uint8_t   GPIO0_CYC_3:  1;               /*!< [3..3] GPIO0_3 input for cycle sense enable                           */
      __IO uint8_t   GPIO0_CYC_4:  1;               /*!< [4..4] GPIO0_4 input for cycle sense enable                           */
    } bit;                                          /*!< [5] BitSize                                                           */
  } WAKE_CONF_GPIO0_CYC;
  __I  uint8_t   RESERVED25[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500040E4) Wake Configuration GPIO Port 1 Rising Edge Register    */
    
    struct {
      __IO uint8_t   GPIO1_RI_0 :  1;               /*!< [0..0] Port 1_0 Wake-up on Rising Edge enable                         */
      __IO uint8_t   GPIO1_RI_1 :  1;               /*!< [1..1] Port 1_1 Wake-up on Rising Edge enable                         */
      __IO uint8_t   GPIO1_RI_2 :  1;               /*!< [2..2] Port 1_2 Wake-up on Rising Edge enable                         */
      __IO uint8_t   GPIO1_RI_3 :  1;               /*!< [3..3] Port 1_3 Wake-up on Rising Edge enable                         */
      __IO uint8_t   GPIO1_RI_4 :  1;               /*!< [4..4] Port 1_4 Wake-up on Rising Edge enable                         */
    } bit;                                          /*!< [5] BitSize                                                           */
  } WAKE_CONF_GPIO1_RISE;
  __I  uint8_t   RESERVED26[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500040E8) Wake Configuration GPIO Port 1 Falling Edge Register   */
    
    struct {
      __IO uint8_t   GPIO1_FA_0 :  1;               /*!< [0..0] Port 1_0 Wake-up on Falling Edge enable                        */
      __IO uint8_t   GPIO1_FA_1 :  1;               /*!< [1..1] Port 1_1 Wake-up on Falling Edge enable                        */
      __IO uint8_t   GPIO1_FA_2 :  1;               /*!< [2..2] Port 1_2 Wake-up on Falling Edge enable                        */
      __IO uint8_t   GPIO1_FA_3 :  1;               /*!< [3..3] Port 1_3 Wake-up on Falling Edge enable                        */
      __IO uint8_t   GPIO1_FA_4 :  1;               /*!< [4..4] Port 1_4 Wake-up on Falling Edge enable                        */
    } bit;                                          /*!< [5] BitSize                                                           */
  } WAKE_CONF_GPIO1_FALL;
  __I  uint8_t   RESERVED27[3];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500040EC) Wake Port 1 Cycle Enabled Register                     */
    
    struct {
      __IO uint8_t   GPIO1_CYC_0:  1;               /*!< [0..0] GPIO1_0 input for cycle sense enable                           */
      __IO uint8_t   GPIO1_CYC_1:  1;               /*!< [1..1] GPIO1_1 input for cycle sense enable                           */
      __IO uint8_t   GPIO1_CYC_2:  1;               /*!< [2..2] GPIO1_2 input for cycle sense enable                           */
      __IO uint8_t   GPIO1_CYC_3:  1;               /*!< [3..3] GPIO1_3 input for cycle sense enable                           */
      __IO uint8_t   GPIO1_CYC_4:  1;               /*!< [4..4] GPIO1_4 input for cycle sense enable                           */
    } bit;                                          /*!< [5] BitSize                                                           */
  } WAKE_CONF_GPIO1_CYC;
  __I  uint8_t   RESERVED28[487];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x500042D4) System Startup Config                                  */
    
    struct {
      __IO uint8_t   MBIST_EN   :  1;               /*!< [0..0] System Startup Configuration Bit for RAM MBIST at Sleep
                                                         Mode exit                                                             */
    } bit;                                          /*!< [1] BitSize                                                           */
  } SystemStartConfig;
} PMU_Type;


/* ================================================================================ */
/* ================                      SCUPM                     ================ */
/* ================================================================================ */


/**
  * @brief SCUPM (SCUPM)
  */

typedef struct {                                    /*!< (@ 0x50006000) SCUPM Structure                                        */
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x50006000) Analog Module Clock Frequency Status Register          */
    
    struct {
      __I  uint32_t  AMCLK1_FREQ:  6;               /*!< [0..5] Current frequency of Analog Module Clock System Clock
                                                         (MI_CLK)                                                              */
           uint32_t             :  2;
      __I  uint32_t  AMCLK2_FREQ:  6;               /*!< [8..13] Current frequency of Analog Module Clock 2 (TFILT_CLK)        */
    } bit;                                          /*!< [14] BitSize                                                          */
  } AMCLK_FREQ_STS;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x50006004) Analog Module Clock Control                            */
    
    struct {
      __IO uint32_t  CLKWDT_PD_N:  1;               /*!< [0..0] Clock Watchdog Powerdown                                       */
    } bit;                                          /*!< [1] BitSize                                                           */
  } AMCLK_CTRL;
  __I  uint32_t  RESERVED0;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x5000600C) Analog Module Clock Limit Register                     */
    
    struct {
      __IO uint32_t  AMCLK1_UP_TH:  6;              /*!< [0..5] Analog Module Clock 1 (MI_CLK) Upper Limit Threshold           */
      __IO uint32_t  AMCLK1_UP_HYS:  2;             /*!< [6..7] Analog Module Clock 1 (MI_CLK) Upper Hysteresis                */
      __IO uint32_t  AMCLK1_LOW_TH:  6;             /*!< [8..13] Analog Module Clock 1 (MI_CLK) Lower Limit Threshold          */
      __IO uint32_t  AMCLK1_LOW_HYS:  2;            /*!< [14..15] Analog Module Clock 1 (MI_CLK) Lower Hysteresis              */
      __IO uint32_t  AMCLK2_UP_TH:  6;              /*!< [16..21] Analog Module Clock 2 (TFILT_CLK) Upper Limit Threshold      */
      __IO uint32_t  AMCLK2_UP_HYS:  2;             /*!< [22..23] Analog Module Clock 2 (TFILT_CLK) Upper Hysteresis           */
      __IO uint32_t  AMCLK2_LOW_TH:  6;             /*!< [24..29] Analog Module Clock 2 (TFILT_CLK) Lower Limit Threshold      */
      __IO uint32_t  AMCLK2_LOW_HYS:  2;            /*!< [30..31] Analog Module Clock 2 (TFILT_CLK) Lower Hysteresis           */
    } bit;                                          /*!< [32] BitSize                                                          */
  } AMCLK_TH_HYS;
  __I  uint32_t  RESERVED1;
  
  union {
    __O  uint32_t  reg;                             /*!< (@ 0x50006014) System Interrupt Status Clear                          */
    
    struct {
      __O  uint32_t  LIN_OC_ICLR:  1;               /*!< [0..0] LIN Overcurrent interrupt status                               */
      __O  uint32_t  LIN_OT_ICLR:  1;               /*!< [1..1] LIN Overtemperature interrupt status                           */
      __O  uint32_t  LIN_TMOUT_ICLR:  1;            /*!< [2..2] LIN TXD timeout                                                */
           uint32_t             :  3;
      __O  uint32_t  PMU_OTWARN_ICLR:  1;           /*!< [6..6] PMU Regulator Overtemperature Prewarning (ADC2, Channel
                                                         9) interrupt status                                                   */
      __O  uint32_t  PMU_OT_ICLR:  1;               /*!< [7..7] PMU Regulator Overtemperature Shutdown (ADC2, Channel
                                                         9) interrupt status                                                   */
      __O  uint32_t  SYS_OTWARN_ICLR:  1;           /*!< [8..8] System Overtemperature Prewarning (ADC2, Channel 8) interrupt
                                                         status                                                                */
      __O  uint32_t  SYS_OT_ICLR:  1;               /*!< [9..9] System Overtemperature Shutdown (ADC2, Channel 8) interrupt
                                                         status                                                                */
      __O  uint32_t  REFBG_LOTHWARN_ICLR:  1;       /*!< [10..10] 8 Bit ADC2 Reference Undervoltage (ADC2, Channel 5)
                                                         interrupt status                                                      */
      __O  uint32_t  REFBG_UPTHWARN_ICLR:  1;       /*!< [11..11] 8 Bit ADC2 Reference Overvoltage (ADC2, Channel 5)
                                                         interrupt status                                                      */
      __O  uint32_t  VREF5V_LOWTH_ICLR:  1;         /*!< [12..12] VREF5V ADC1 Reference Undervoltage (ADC2, Channel 4)
                                                         Interrupt Status                                                      */
      __O  uint32_t  VREF5V_UPTH_ICLR:  1;          /*!< [13..13] VREF5V ADC1 Reference Overvoltage (ADC2, Channel 4)
                                                         Interrupt Status                                                      */
      __O  uint32_t  VREF5V_OVL_ICLR:  1;           /*!< [14..14] VREF5V Overload Interrupt Status                             */
      __O  uint32_t  ADC2_ESM_ICLR:  1;             /*!< [15..15] ADC2 Exceptional Sequence Measurement Interrupt Status       */
    } bit;                                          /*!< [16] BitSize                                                          */
  } SYS_ISCLR;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x50006018) System Interrupt Status                                */
    
    struct {
      __I  uint32_t  LIN_OC_IS  :  1;               /*!< [0..0] LIN Overcurrent interrupt status                               */
      __I  uint32_t  LIN_OT_IS  :  1;               /*!< [1..1] LIN Overtemperature interrupt status                           */
      __I  uint32_t  LIN_TMOUT_IS:  1;              /*!< [2..2] LIN TXD timeout                                                */
           uint32_t             :  3;
      __I  uint32_t  PMU_OTWARN_IS:  1;             /*!< [6..6] PMU Regulator Overtemperature Prewarning (ADC2, Channel
                                                         9) interrupt status                                                   */
      __I  uint32_t  PMU_OT_IS  :  1;               /*!< [7..7] PMU Regulator Overtemperature Shutdown (ADC2, Channel
                                                         9) interrupt status                                                   */
      __I  uint32_t  SYS_OTWARN_IS:  1;             /*!< [8..8] System Overtemperature Prewarning (ADC2, Channel 8) interrupt
                                                         status                                                                */
      __I  uint32_t  SYS_OT_IS  :  1;               /*!< [9..9] System Overtemperature Shutdown (ADC2, Channel 8) interrupt
                                                         status                                                                */
      __I  uint32_t  REFBG_LOTHWARN_IS:  1;         /*!< [10..10] 8 Bit ADC2 Reference Undervoltage (ADC2, Channel 5)
                                                         interrupt status                                                      */
      __I  uint32_t  REFBG_UPTHWARN_IS:  1;         /*!< [11..11] 8 Bit ADC2 Reference Overvoltage (ADC2, Channel 5)
                                                         interrupt status                                                      */
      __I  uint32_t  VREF5V_LOWTH_IS:  1;           /*!< [12..12] VREF5V ADC1 Reference Undervoltage (ADC2, Channel 4)
                                                         Interrupt Status                                                      */
      __I  uint32_t  VREF5V_UPTH_IS:  1;            /*!< [13..13] VREF5V ADC1 Reference Overvoltage (ADC2, Channel 4)
                                                         Interrupt Status                                                      */
      __I  uint32_t  VREF5V_OVL_IS:  1;             /*!< [14..14] VREF5V Overload Interrupt Status                             */
      __I  uint32_t  ADC2_ESM_IS:  1;               /*!< [15..15] ADC2 Exceptional Sequence Measurement Interrupt Status       */
    } bit;                                          /*!< [16] BitSize                                                          */
  } SYS_IS;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x5000601C) System Supply Interrupt Status                         */
    
    struct {
      __I  uint32_t  VBAT_UV_IS :  1;               /*!< [0..0] VBAT Undervoltage Interrupt Status (ADC2 channel 0)            */
      __I  uint32_t  VS_UV_IS   :  1;               /*!< [1..1] VS Undervoltage Interrupt Status (ADC2 channel 1)              */
      __I  uint32_t  VDD5V_UV_IS:  1;               /*!< [2..2] VDDP Undervoltage Interrupt Status (ADC2 channel 2)            */
      __I  uint32_t  VDD1V5_UV_IS:  1;              /*!< [3..3] VDDC Undervoltage Interrupt Status (ADC2 channel 3)            */
      __I  uint32_t  VBAT_OV_IS :  1;               /*!< [4..4] VBAT Overvoltage Interrupt Status (ADC2 channel 0)             */
      __I  uint32_t  VS_OV_IS   :  1;               /*!< [5..5] VS Overvoltage Interrupt Status (ADC2 channel 1)               */
      __I  uint32_t  VDD5V_OV_IS:  1;               /*!< [6..6] VDDP Overvoltage Interrupt Status (ADC2 channel 2)             */
      __I  uint32_t  VDD1V5_OV_IS:  1;              /*!< [7..7] VDDC Overvoltage Interrupt Status (ADC2 channel 3)             */
           uint32_t             :  8;
      __I  uint32_t  VBAT_UV_STS:  1;               /*!< [16..16] VBAT Undervoltage Status                                     */
      __I  uint32_t  VS_UV_STS  :  1;               /*!< [17..17] VS Undervoltage Status                                       */
      __I  uint32_t  VDD5V_UV_STS:  1;              /*!< [18..18] VDDP Undervoltage Status                                     */
      __I  uint32_t  VDD1V5_UV_STS:  1;             /*!< [19..19] VDDC Undervoltage Status                                     */
      __I  uint32_t  VBAT_OV_STS:  1;               /*!< [20..20] VBAT Overvoltage Status                                      */
      __I  uint32_t  VS_OV_STS  :  1;               /*!< [21..21] VS Overvoltage Interrupt Status                              */
      __I  uint32_t  VDD5V_OV_STS:  1;              /*!< [22..22] VDDP Overvoltage Status                                      */
      __I  uint32_t  VDD1V5_OV_STS:  1;             /*!< [23..23] VDDC Overvoltage Status                                      */
    } bit;                                          /*!< [24] BitSize                                                          */
  } SYS_SUPPLY_IRQ_STS;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x50006020) System Supply Interrupt Control                        */
    
    struct {
      __IO uint32_t  VBAT_UV_IE :  1;               /*!< [0..0] VBAT Undervoltage Interrupt Enable                             */
      __IO uint32_t  VS_UV_IE   :  1;               /*!< [1..1] VS Undervoltage Interrupt Enable                               */
      __IO uint32_t  VDD5V_UV_IE:  1;               /*!< [2..2] VDD5V Undervoltage Interrupt Enable                            */
      __IO uint32_t  VDD1V5_UV_IE:  1;              /*!< [3..3] VDD1V5 Undervoltage Interrupt Enable                           */
      __IO uint32_t  VBAT_OV_IE :  1;               /*!< [4..4] VBAT Overvoltage Interrupt Enable                              */
      __IO uint32_t  VS_OV_IE   :  1;               /*!< [5..5] VS Overvoltage Interrupt Enable                                */
      __IO uint32_t  VDD5V_OV_IE:  1;               /*!< [6..6] VDD5V Overvoltage Interrupt Enable                             */
      __IO uint32_t  VDD1V5_OV_IE:  1;              /*!< [7..7] VDD1V5 Overvoltage Interrupt Enable                            */
    } bit;                                          /*!< [8] BitSize                                                           */
  } SYS_SUPPLY_IRQ_CTRL;
  
  union {
    __O  uint32_t  reg;                             /*!< (@ 0x50006024) System Supply Interrupt Status Clear                   */
    
    struct {
      __O  uint32_t  VBAT_UV_ICLR:  1;              /*!< [0..0] VBAT Undervoltage Interrupt Status (ADC2 channel 0)            */
      __O  uint32_t  VS_UV_ICLR :  1;               /*!< [1..1] VS Undervoltage Interrupt Status (ADC2 channel 1)              */
      __O  uint32_t  VDD5V_UV_ICLR:  1;             /*!< [2..2] VDDP Undervoltage Interrupt Status (ADC2 channel 2)            */
      __O  uint32_t  VDD1V5_UV_ICLR:  1;            /*!< [3..3] VDDC Undervoltage Interrupt Status (ADC2 channel 3)            */
      __O  uint32_t  VBAT_OV_ICLR:  1;              /*!< [4..4] VBAT Overvoltage Interrupt Status (ADC2 channel 0)             */
      __O  uint32_t  VS_OV_ICLR :  1;               /*!< [5..5] VS Overvoltage Interrupt Status (ADC2 channel 1)               */
      __O  uint32_t  VDD5V_OV_ICLR:  1;             /*!< [6..6] VDDP Overvoltage Interrupt Status (ADC2 channel 2)             */
      __O  uint32_t  VDD1V5_OV_ICLR:  1;            /*!< [7..7] VDDC Overvoltage Interrupt Status (ADC2 channel 3)             */
           uint32_t             :  8;
      __O  uint32_t  VBAT_UV_SCLR:  1;              /*!< [16..16] VBAT Undervoltage Status                                     */
      __O  uint32_t  VS_UV_SCLR :  1;               /*!< [17..17] VS Undervoltage Status                                       */
      __O  uint32_t  VDD5V_UV_SCLR:  1;             /*!< [18..18] VDDP Undervoltage Status                                     */
      __O  uint32_t  VDD1V5_UV_SCLR:  1;            /*!< [19..19] VDDC Undervoltage Status                                     */
      __O  uint32_t  VBAT_OV_SCLR:  1;              /*!< [20..20] VBAT Overvoltage Status                                      */
      __O  uint32_t  VS_OV_SCLR :  1;               /*!< [21..21] VS Overvoltage Interrupt Status                              */
      __O  uint32_t  VDD5V_OV_SCLR:  1;             /*!< [22..22] VDDP Overvoltage Status                                      */
      __O  uint32_t  VDD1V5_OV_SCLR:  1;            /*!< [23..23] VDDC Overvoltage Status                                      */
    } bit;                                          /*!< [24] BitSize                                                          */
  } SYS_SUPPLY_IRQ_CLR;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x50006028) System Interrupt Control                               */
    
    struct {
      __IO uint32_t  LIN_OC_IE  :  1;               /*!< [0..0] LIN Overcurrent Interrupt Enable                               */
      __IO uint32_t  LIN_OT_IE  :  1;               /*!< [1..1] LIN Overtemperature Interrupt Enable                           */
      __IO uint32_t  LIN_TMOUT_IE:  1;              /*!< [2..2] LIN TXD timeout Interrupt Enable                               */
           uint32_t             :  3;
      __IO uint32_t  PMU_OTWARN_IE:  1;             /*!< [6..6] PMU Regulator Overtemperature Warning Interrupt Enable         */
      __IO uint32_t  PMU_OT_IE  :  1;               /*!< [7..7] PMU Regulator Overtemperature Shutdown Interrupt Enable
                                                         (leads to shutdown of System)                                         */
      __IO uint32_t  SYS_OTWARN_IE:  1;             /*!< [8..8] System Overtemperature Warning Interrupt Enable                */
      __IO uint32_t  SYS_OT_IE  :  1;               /*!< [9..9] System Overtemperature Shutdown Interrupt Enable (leads
                                                         to shutdown of System)                                                */
      __IO uint32_t  REFBG_LOTHWARN_IE:  1;         /*!< [10..10] Reference Voltage Undervoltage Interrupt Enable              */
      __IO uint32_t  REFBG_UPTHWARN_IE:  1;         /*!< [11..11] Reference Voltage Overvoltage Interrupt Enable               */
      __IO uint32_t  VREF5V_LOWTH_IE:  1;           /*!< [12..12] VREF5V ADC1 Reference Undervoltage (ADC2, Channel 4)
                                                         Interrupt Enable                                                      */
      __IO uint32_t  VREF5V_UPTH_IE:  1;            /*!< [13..13] VREF5V ADC1 Reference Overvoltage (ADC2, Channel 4)
                                                         Interrupt Enable                                                      */
      __IO uint32_t  VREF5V_OVL_IE:  1;             /*!< [14..14] VREF5V Overload Interrupt Enable                             */
      __IO uint32_t  ADC2_ESM_IE:  1;               /*!< [15..15] ADC2 Exceptional Sequence Measurement Interrupt Enable       */
    } bit;                                          /*!< [16] BitSize                                                          */
  } SYS_IRQ_CTRL;
  __I  uint32_t  RESERVED2;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x50006030) Power Control Unit Control Status                      */
    
    struct {
           uint32_t             :  1;
      __IO uint32_t  CLKWDT_SD_DIS:  1;             /*!< [1..1] Power Modules Clock Watchdog Shutdown Disable                  */
           uint32_t             :  5;
      __IO uint32_t  FAIL_PS_DIS:  1;               /*!< [7..7] Disable LIN, BDRV and CP because of Overtemperature            */
      __IO uint32_t  LIN_VS_UV_SD_DIS:  1;          /*!< [8..8] LIN Module VS Undervoltage Transmitter Shutdown                */
           uint32_t             : 5;
      __IO uint32_t  SYS_VSD_OV_SLM_DIS:  1;        /*!< [14..14] VSD Overvoltage Shutdown for Peripherals Disable             */
           uint32_t             : 9;
      __IO uint32_t  SYS_OT_PS_DIS:  1;             /*!< [24..24] System Overtemperature Power Switches Shutdown Disable       */
      __IO uint32_t  CLKLOSS_SD_DIS:  1;            /*!< [25..25] Power Switches Loss of Clock Shutdown Disable (AMCLK3)       */
      __IO uint32_t  CLKWDT_RES_SD_DIS:  1;         /*!< [26..26] Clock Watchdog Reset Disable                                 */
    } bit;                                          /*!< [26] BitSize                                                          */
  } PCU_CTRL_STS;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x50006034) WDT1 Watchdog Control                                  */
    
    struct {
      __IO uint32_t  WDP_SEL    :  6;               /*!< [0..5] Watchdog Period Selection and trigger                          */
      __IO uint32_t  SOWCONF    :  2;               /*!< [6..7] Short Open Window Configuration                                */
    } bit;                                          /*!< [8] BitSize                                                           */
  } WDT1_TRIG;
  __I  uint32_t  RESERVED3[7];
  
  union {
    __O  uint32_t  reg;                             /*!< (@ 0x50006054) Bridge Driver Interrrupt Status Clear                  */
    
    struct {
      __O  uint32_t  LS1_DS_ICLR:  1;               /*!< [0..0] Bridge Driver Low Side 1 Pre-Driver short Interrupt Status     */
      __O  uint32_t  LS2_DS_ICLR:  1;               /*!< [1..1] Bridge Driver Low Side 2 Pre-Driver short Interrupt Status     */
      __O  uint32_t  HS1_DS_ICLR:  1;               /*!< [2..2] Bridge Driver High Side 1 Pre-Driver short Interrupt
                                                         Status                                                                */
      __O  uint32_t  HS2_DS_ICLR:  1;               /*!< [3..3] Bridge Driver High Side 2 Pre-Driver short Interrupt
                                                         Status                                                                */
           uint32_t             :  6;
      __O  uint32_t  LS1_OC_ICLR:  1;               /*!< [10..10] External Low Side 1 FET Over-current Status                  */
      __O  uint32_t  LS2_OC_ICLR:  1;               /*!< [11..11] External Low Side 2 FET Over-current Status                  */
      __O  uint32_t  HS1_OC_ICLR:  1;               /*!< [12..12] External High 1 FET Over-current Status                      */
      __O  uint32_t  HS2_OC_ICLR:  1;               /*!< [13..13] External High Side 2 FET Over-current Status                 */
           uint32_t             :  2;
      __O  uint32_t  VCP_LOWTH2_ICLR:  1;           /*!< [16..16] Warning for VCP Lower Threshold 2 Measurement (VCP_LOW
                                                         Signal from CP) Interrupt Status                                      */
      __O  uint32_t  VCP_LOWTH1_ICLR:  1;           /*!< [17..17] Warning for VCP Lower Threshold 1 Measurement (ADC2
                                                         channel 7) Interrupt Status                                           */
      __O  uint32_t  VCP_UPTH_ICLR:  1;             /*!< [18..18] Warning for VCP Upper Threshold Measurement (ADC2 channel
                                                         7) Interrupt Status                                                   */
      __O  uint32_t  VSD_LOWTH_ICLR:  1;            /*!< [19..19] Warning for VSD Lower Threshold Measurement (ADC2 channel
                                                         6) Interrupt Status                                                   */
      __O  uint32_t  VSD_UPTH_ICLR:  1;             /*!< [20..20] Warning for VSD Upper Threshold Measurement (ADC2 channel
                                                         6) Interrupt Status                                                   */
           uint32_t             :  3;
      __O  uint32_t  VCP_LOWTH2_SCLR:  1;           /*!< [24..24] Warning for VCP Lower Threshold 2 Measurement (VCP_LOW
                                                         Signal from CP) Status                                                */
      __O  uint32_t  VCP_LOWTH1_SCLR:  1;           /*!< [25..25] Warning for VCP Lower Threshold 1 Measurement (ADC2
                                                         channel 7) Status                                                     */
      __O  uint32_t  VCP_UPTH_SCLR:  1;             /*!< [26..26] Warning for VCP Upper Threshold Measurement (ADC2 channel
                                                         7) Status                                                             */
      __O  uint32_t  VSD_LOWTH_SCLR:  1;            /*!< [27..27] Warning for VSD Lower Threshold Measurement (ADC2 channel
                                                         6) Status                                                             */
      __O  uint32_t  VSD_UPTH_SCLR:  1;             /*!< [28..28] Warning for VSD Upper Threshold Measurement (ADC2 channel
                                                         6) Status                                                             */
    } bit;                                          /*!< [29] BitSize                                                          */
  } BDRV_ISCLR;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x50006058) Bridge Driver Interrrupt Status                        */
    
    struct {
      __I  uint32_t  LS1_DS_IS  :  1;               /*!< [0..0] Bridge Driver Low Side 1 Pre-Driver short Interrupt Status     */
      __I  uint32_t  LS2_DS_IS  :  1;               /*!< [1..1] Bridge Driver Low Side 2 Pre-Driver short Interrupt Status     */
      __I  uint32_t  HS1_DS_IS  :  1;               /*!< [2..2] Bridge Driver High Side 1 Pre-Driver short Interrupt
                                                         Status                                                                */
      __I  uint32_t  HS2_DS_IS  :  1;               /*!< [3..3] Bridge Driver High Side 2 Pre-Driver short Interrupt
                                                         Status                                                                */
           uint32_t             :  6;
      __I  uint32_t  LS1_OC_IS  :  1;               /*!< [10..10] External Low Side 1 FET Over-current Status                  */
      __I  uint32_t  LS2_OC_IS  :  1;               /*!< [11..11] External Low Side 2 FET Over-current Status                  */
      __I  uint32_t  HS1_OC_IS  :  1;               /*!< [12..12] External High 1 FET Over-current Status                      */
      __I  uint32_t  HS2_OC_IS  :  1;               /*!< [13..13] External High Side 2 FET Over-current Status                 */
           uint32_t             :  2;
      __I  uint32_t  VCP_LOWTH2_IS:  1;             /*!< [16..16] Warning for VCP Lower Threshold 2 Measurement (VCP_LOW
                                                         Signal from CP) Interrupt Status                                      */
      __I  uint32_t  VCP_LOWTH1_IS:  1;             /*!< [17..17] Warning for VCP Lower Threshold 1 Measurement (ADC2
                                                         channel 7) Interrupt Status                                           */
      __I  uint32_t  VCP_UPTH_IS:  1;               /*!< [18..18] Warning for VCP Upper Threshold Measurement (ADC2 channel
                                                         7) Interrupt Status                                                   */
      __I  uint32_t  VSD_LOWTH_IS:  1;              /*!< [19..19] Warning for VSD Lower Threshold Measurement (ADC2 channel
                                                         6) Interrupt Status                                                   */
      __I  uint32_t  VSD_UPTH_IS:  1;               /*!< [20..20] Warning for VSD Upper Threshold Measurement (ADC2 channel
                                                         6) Interrupt Status                                                   */
           uint32_t             :  3;
      __I  uint32_t  VCP_LOWTH2_STS:  1;            /*!< [24..24] Warning for VCP Lower Threshold 2 Measurement (VCP_LOW
                                                         Signal from CP) Status                                                */
      __I  uint32_t  VCP_LOWTH1_STS:  1;            /*!< [25..25] Warning for VCP Lower Threshold 1 Measurement (ADC2
                                                         channel 7) Status                                                     */
      __I  uint32_t  VCP_UPTH_STS:  1;              /*!< [26..26] Warning for VCP Upper Threshold Measurement (ADC2 channel
                                                         7) Status                                                             */
      __I  uint32_t  VSD_LOWTH_STS:  1;             /*!< [27..27] Warning for VSD Lower Threshold Measurement (ADC2 channel
                                                         6) Status                                                             */
      __I  uint32_t  VSD_UPTH_STS:  1;              /*!< [28..28] Warning for VSD Upper Threshold Measurement (ADC2 channel
                                                         6) Status                                                             */
    } bit;                                          /*!< [29] BitSize                                                          */
  } BDRV_IS;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x5000605C) Bridge Driver Interrupt Control                        */
    
    struct {
      __IO uint32_t  LS1_DS_IE  :  1;               /*!< [0..0] Bridge Driver Low Side 1 Pre-Driver Short Interrupt Enable     */
      __IO uint32_t  LS2_DS_IE  :  1;               /*!< [1..1] Bridge Driver Low Side 2 Pre-Driver Short Interrupt Enable     */
      __IO uint32_t  HS1_DS_IE  :  1;               /*!< [2..2] Bridge Driver High Side 1 Pre-Driver Short Interrupt
                                                         Enable                                                                */
      __IO uint32_t  HS2_DS_IE  :  1;               /*!< [3..3] Bridge Driver High Side 2 Pre-Driver Short Interrupt
                                                         Enable                                                                */
           uint32_t             :  6;
      __IO uint32_t  LS1_OC_IE  :  1;               /*!< [10..10] External Low Side 1 FET Over-current Interrupt Enable        */
      __IO uint32_t  LS2_OC_IE  :  1;               /*!< [11..11] External Low Side 2 FET Over-current Interrupt Enable        */
      __IO uint32_t  HS1_OC_IE  :  1;               /*!< [12..12] External High Side 1 FET Over-current Interrupt Enable       */
      __IO uint32_t  HS2_OC_IE  :  1;               /*!< [13..13] External High Side 2 FET Over-current Interrupt Enable       */
           uint32_t             :  2;
      __IO uint32_t  VCP_LOWTH2_IE:  1;             /*!< [16..16] VCP Measurement Lower Threshold 2 Interrupt Enable           */
      __IO uint32_t  VCP_LOWTH1_IE:  1;             /*!< [17..17] VCP Measurement Lower Threshold 1 Interrupt Enable           */
      __IO uint32_t  VCP_UPTH_IE:  1;               /*!< [18..18] VCP Measurement Upper Threshold Interrupt Enable             */
      __IO uint32_t  VSD_LOWTH_IE:  1;              /*!< [19..19] VSD Measurement Lower Threshold Interrupt Enable             */
      __IO uint32_t  VSD_UPTH_IE:  1;               /*!< [20..20] VSD Measurement Upper Threshold Interrupt Enable             */
    } bit;                                          /*!< [21] BitSize                                                          */
  } BDRV_IRQ_CTRL;
  __I  uint32_t  RESERVED4[3];
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x5000606C) System Tick Calibration Register                       */
    
    struct {
      __IO uint32_t  STCALIB    : 26;               /*!< [0..25] System Tick Calibration                                       */
    } bit;                                          /*!< [26] BitSize                                                          */
  } STCALIB;
  __I  uint32_t  RESERVED5[4];
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x50006080) Bus Fault Status                                       */
    
    struct {
      __I  uint32_t  DBFSTS     :  1;               /*!< [0..0] Data Bus Fault Status Valid Flag                               */
      __I  uint32_t  SBFSTS     :  1;               /*!< [1..1] System Bus Fault Status Valid Flag                             */
    } bit;                                          /*!< [2] BitSize                                                           */
  } BFSTS;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x50006084) Data Bus Fault Address Register                        */
    
    struct {
      __I  uint32_t  DBFA       : 32;               /*!< [0..31] Reserved                                                      */
    } bit;                                          /*!< [32] BitSize                                                          */
  } DBFA;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x50006088) System Bus Fault Address Register                      */
    
    struct {
      __I  uint32_t  SBFA       : 32;               /*!< [0..31] Reserved                                                      */
    } bit;                                          /*!< [32] BitSize                                                          */
  } SBFA;
  
  union {
    __O  uint32_t  reg;                             /*!< (@ 0x5000608C) Bus Fault Status Clear Register                        */
    
    struct {
      __O  uint32_t  DBFSTSCLR  :  1;               /*!< [0..0] Data Bus Fault Status Clear Flag                               */
      __O  uint32_t  SBFSTSCLR  :  1;               /*!< [1..1] System Bus Fault Status Clear Flag                             */
    } bit;                                          /*!< [2] BitSize                                                           */
  } BFSTS_CLR;
} SCUPM_Type;


/* ================================================================================ */
/* ================                       CPU                      ================ */
/* ================================================================================ */


/**
  * @brief CPU (CPU)
  */

typedef struct {                                    /*!< (@ 0xE000E000) CPU Structure                                          */
  __I  uint32_t  RESERVED0;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0xE000E004) Interrupt Controller Type                              */
    
    struct {
      __I  uint32_t  INTLINESNUM:  5;               /*!< [0..4] Interrupt Lines                                                */
    } bit;                                          /*!< [5] BitSize                                                           */
  } ICT;
  __I  uint32_t  RESERVED1[2];
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000E010) SysTick Control and Status                             */
    
    struct {
      __IO uint32_t  ENABLE     :  1;               /*!< [0..0] Enable                                                         */
      __IO uint32_t  TICKINT    :  1;               /*!< [1..1] TICKINT                                                        */
      __IO uint32_t  CLKSOURCE  :  1;               /*!< [2..2] CLK Source                                                     */
           uint32_t             : 13;
      __IO uint32_t  COUNTFLAG  :  1;               /*!< [16..16] Count Flag                                                   */
    } bit;                                          /*!< [17] BitSize                                                          */
  } SYSTICK_CS;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000E014) SysTick Reload Value                                   */
    
    struct {
      __IO uint32_t  RELOAD     : 24;               /*!< [0..23] Reload                                                        */
    } bit;                                          /*!< [24] BitSize                                                          */
  } SYSTICK_RL;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000E018) SysTick Current Value                                  */
    
    struct {
      __IO uint32_t  CURRENT    : 24;               /*!< [0..23] Current                                                       */
    } bit;                                          /*!< [24] BitSize                                                          */
  } SYSTICK_CUR;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0xE000E01C) SysTick Calibration Value                              */
    
    struct {
      __I  uint32_t  TENMS      : 24;               /*!< [0..23] Tenms                                                         */
           uint32_t             :  6;
      __I  uint32_t  SKEW       :  1;               /*!< [30..30] Skew                                                         */
      __I  uint32_t  NOREF      :  1;               /*!< [31..31] No Reference Clock                                           */
    } bit;                                          /*!< [32] BitSize                                                          */
  } SYSTICK_CAL;
  __I  uint32_t  RESERVED2[56];
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000E100) Interrupt Set-Enable                                   */
    
    struct {
      __IO uint32_t  Int_GPT1   :  1;               /*!< [0..0] Interrupt Set for GPT1                                         */
      __IO uint32_t  Int_GPT2   :  1;               /*!< [1..1] Interrupt Set for GPT2                                         */
      __IO uint32_t  Int_ADC2   :  1;               /*!< [2..2] Interrupt Set for MU, ADC2                                     */
      __IO uint32_t  Int_ADC1   :  1;               /*!< [3..3] Interrupt Set for ADC1                                         */
      __IO uint32_t  Int_CCU6SR0:  1;               /*!< [4..4] Interrupt Set for CCU6 SR0                                     */
      __IO uint32_t  Int_CCU6SR1:  1;               /*!< [5..5] Interrupt Set for CCU6 SR1                                     */
      __IO uint32_t  Int_CCU6SR2:  1;               /*!< [6..6] Interrupt Set for CCU6 SR2                                     */
      __IO uint32_t  Int_CCU6SR3:  1;               /*!< [7..7] Interrupt Set for CCU6 SR3                                     */
      __IO uint32_t  Int_SSC1   :  1;               /*!< [8..8] Interrupt Set for SSC1                                         */
      __IO uint32_t  Int_SSC2   :  1;               /*!< [9..9] Interrupt Set for SSC2                                         */
      __IO uint32_t  Int_UART1  :  1;               /*!< [10..10] Interrupt Set for UART1                                      */
      __IO uint32_t  Int_UART2  :  1;               /*!< [11..11] Interrupt Set for UART2                                      */
      __IO uint32_t  Int_EXINT0 :  1;               /*!< [12..12] Interrupt Set for External Int 0                             */
      __IO uint32_t  Int_EXINT1 :  1;               /*!< [13..13] Interrupt Set for External Int 1                             */
      __IO uint32_t  Int_BDRV   :  1;               /*!< [14..14] Interrupt Set for Bridge Driver                              */
      __IO uint32_t  Int_DMA    :  1;               /*!< [15..15] Interrupt Set for DMA                                        */
    } bit;                                          /*!< [16] BitSize                                                          */
  } NVIC_ISER0;
  __I  uint32_t  RESERVED3[31];
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000E180) Interrupt Clear-Enable                                 */
    
    struct {
      __IO uint32_t  Int_GPT1   :  1;               /*!< [0..0] Interrupt Clear for GPT1                                       */
      __IO uint32_t  Int_GPT2   :  1;               /*!< [1..1] Interrupt Clear for GPT2                                       */
      __IO uint32_t  Int_ADC2   :  1;               /*!< [2..2] Interrupt Clear for MU, ADC2                                   */
      __IO uint32_t  Int_ADC1   :  1;               /*!< [3..3] Interrupt Clear for ADC1                                       */
      __IO uint32_t  Int_CCU6SR0:  1;               /*!< [4..4] Interrupt Clear for CCU6 SR0                                   */
      __IO uint32_t  Int_CCU6SR1:  1;               /*!< [5..5] Interrupt Clear for CCU6 SR1                                   */
      __IO uint32_t  Int_CCU6SR2:  1;               /*!< [6..6] Interrupt Clear for CCU6 SR2                                   */
      __IO uint32_t  Int_CCU6SR3:  1;               /*!< [7..7] Interrupt Clear for CCU6 SR3                                   */
      __IO uint32_t  Int_SSC1   :  1;               /*!< [8..8] Interrupt Clear for SSC1                                       */
      __IO uint32_t  Int_SSC2   :  1;               /*!< [9..9] Interrupt Clear for SSC2                                       */
      __IO uint32_t  Int_UART1  :  1;               /*!< [10..10] Interrupt Clear for UART1                                    */
      __IO uint32_t  Int_UART2  :  1;               /*!< [11..11] Interrupt Clear for UART2                                    */
      __IO uint32_t  Int_EXINT0 :  1;               /*!< [12..12] Interrupt Clear for External Int 0                           */
      __IO uint32_t  Int_EXINT1 :  1;               /*!< [13..13] Interrupt Clear for External Int 1                           */
      __IO uint32_t  Int_BDRV   :  1;               /*!< [14..14] Interrupt Clear for Bridge Driver                            */
      __IO uint32_t  Int_DMA    :  1;               /*!< [15..15] Interrupt Clr for DMA                                        */
    } bit;                                          /*!< [16] BitSize                                                          */
  } NVIC_ICER0;
  __I  uint32_t  RESERVED4[31];
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000E200) Interrupt Set-Pending                                  */
    
    struct {
      __IO uint32_t  Int_GPT1   :  1;               /*!< [0..0] Interrupt Set Pending for GPT1                                 */
      __IO uint32_t  Int_GPT2   :  1;               /*!< [1..1] Interrupt Set Pending for GPT2                                 */
      __IO uint32_t  Int_ADC2   :  1;               /*!< [2..2] Interrupt Set Pending for MU, ADC2                             */
      __IO uint32_t  Int_ADC1   :  1;               /*!< [3..3] Interrupt Set Pending for ADC1                                 */
      __IO uint32_t  Int_CCU6SR0:  1;               /*!< [4..4] Interrupt Set Pending for CCU6 SR0                             */
      __IO uint32_t  Int_CCU6SR1:  1;               /*!< [5..5] Interrupt Set Pending for CCU6 SR1                             */
      __IO uint32_t  Int_CCU6SR2:  1;               /*!< [6..6] Interrupt Set Pending for CCU6 SR2                             */
      __IO uint32_t  Int_CCU6SR3:  1;               /*!< [7..7] Interrupt Set Pending for CCU6 SR3                             */
      __IO uint32_t  Int_SSC1   :  1;               /*!< [8..8] Interrupt Set Pending for SSC1                                 */
      __IO uint32_t  Int_SSC2   :  1;               /*!< [9..9] Interrupt Set Pending for SSC2                                 */
      __IO uint32_t  Int_UART1  :  1;               /*!< [10..10] Interrupt Set Pending for UART1                              */
      __IO uint32_t  Int_UART2  :  1;               /*!< [11..11] Interrupt Set Pending for UART2                              */
      __IO uint32_t  Int_EXINT0 :  1;               /*!< [12..12] Interrupt Set Pending for External Int 0                     */
      __IO uint32_t  Int_EXINT1 :  1;               /*!< [13..13] Interrupt Set Pending for External Int 1                     */
      __IO uint32_t  Int_BDRV   :  1;               /*!< [14..14] Interrupt Set Pending for Bridge Driver                      */
      __IO uint32_t  Int_DMA    :  1;               /*!< [15..15] Interrupt Set Pend for DMA                                   */
    } bit;                                          /*!< [16] BitSize                                                          */
  } NVIC_ISPR0;
  __I  uint32_t  RESERVED5[6];
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000E21C) System Handler Priority Register 2                     */
    
    struct {
      __IO uint32_t  PRI_8      :  8;               /*!< [0..7] Reserved for Priority of System Handler 8                      */
      __IO uint32_t  PRI_9      :  8;               /*!< [8..15] Reserved for Priority of System Handler 9                     */
      __IO uint32_t  PRI_10     :  8;               /*!< [16..23] Reserved for Priority of System Handler 10                   */
      __IO uint32_t  PRI_11     :  8;               /*!< [24..31] Priority of System Handler 11, SVCall                        */
    } bit;                                          /*!< [32] BitSize                                                          */
  } SHPR2;
  __I  uint32_t  RESERVED6[24];
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000E280) Interrupt Clear-Pending                                */
    
    struct {
      __IO uint32_t  Int_GPT1   :  1;               /*!< [0..0] Interrupt Clear Pending for GPT1                               */
      __IO uint32_t  Int_GPT2   :  1;               /*!< [1..1] Interrupt Clear Pending for GPT2                               */
      __IO uint32_t  Int_ADC2   :  1;               /*!< [2..2] Interrupt Clear Pending for MU, ADC2                           */
      __IO uint32_t  Int_ADC1   :  1;               /*!< [3..3] Interrupt Clear Pending for ADC1                               */
      __IO uint32_t  Int_CCU6SR0:  1;               /*!< [4..4] Interrupt Clear Pending for CCU6 SR0                           */
      __IO uint32_t  Int_CCU6SR1:  1;               /*!< [5..5] Interrupt Clear Pending for CCU6 SR1                           */
      __IO uint32_t  Int_CCU6SR2:  1;               /*!< [6..6] Interrupt Clear Pending for CCU6 SR2                           */
      __IO uint32_t  Int_CCU6SR3:  1;               /*!< [7..7] Interrupt Clear Pending for CCU6 SR3                           */
      __IO uint32_t  Int_SSC1   :  1;               /*!< [8..8] Interrupt Clear Pending for SSC1                               */
      __IO uint32_t  Int_SSC2   :  1;               /*!< [9..9] Interrupt Clear Pending for SSC2                               */
      __IO uint32_t  Int_UART1  :  1;               /*!< [10..10] Interrupt Clear Pending for UART1                            */
      __IO uint32_t  Int_UART2  :  1;               /*!< [11..11] Interrupt Clear Pending for UART2                            */
      __IO uint32_t  Int_EXINT0 :  1;               /*!< [12..12] Interrupt Clear Pending for External Int 0                   */
      __IO uint32_t  Int_EXINT1 :  1;               /*!< [13..13] Interrupt Clear Pending for External Int 1                   */
      __IO uint32_t  Int_BDRV   :  1;               /*!< [14..14] Interrupt Clear Pending for Bridge Driver                    */
      __IO uint32_t  Int_DMA    :  1;               /*!< [15..15] Interrupt Clr Pend for DMA                                   */
    } bit;                                          /*!< [16] BitSize                                                          */
  } NVIC_ICPR0;
  __I  uint32_t  RESERVED7[31];
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0xE000E300) Active Bit Register Interrupt Active Flags             */
    
    struct {
      __I  uint32_t  Int_GPT1   :  1;               /*!< [0..0] Interrupt Active for GPT1                                      */
      __I  uint32_t  Int_GPT2   :  1;               /*!< [1..1] Interrupt Active for GPT2                                      */
      __I  uint32_t  Int_ADC2   :  1;               /*!< [2..2] Interrupt Active for MU, ADC2                                  */
      __I  uint32_t  Int_ADC1   :  1;               /*!< [3..3] Interrupt Active for ADC1                                      */
      __I  uint32_t  Int_CCU6SR0:  1;               /*!< [4..4] Interrupt Active for CCU6 SR0                                  */
      __I  uint32_t  Int_CCU6SR1:  1;               /*!< [5..5] Interrupt Active for CCU6 SR1                                  */
      __I  uint32_t  Int_CCU6SR2:  1;               /*!< [6..6] Interrupt Active for CCU6 SR2                                  */
      __I  uint32_t  Int_CCU6SR3:  1;               /*!< [7..7] Interrupt Active for CCU6 SR3                                  */
      __I  uint32_t  Int_SSC1   :  1;               /*!< [8..8] Interrupt Active for SSC1                                      */
      __I  uint32_t  Int_SSC2   :  1;               /*!< [9..9] Interrupt Active for SSC2                                      */
      __I  uint32_t  Int_UART1  :  1;               /*!< [10..10] Interrupt Active for UART1                                   */
      __I  uint32_t  Int_UART2  :  1;               /*!< [11..11] Interrupt Active for UART2                                   */
      __I  uint32_t  Int_EXINT0 :  1;               /*!< [12..12] Interrupt Active for External Int 0                          */
      __I  uint32_t  Int_EXINT1 :  1;               /*!< [13..13] Interrupt Active for External Int 1                          */
      __I  uint32_t  Int_BDRV   :  1;               /*!< [14..14] Interrupt Active for Bridge Driver                           */
      __I  uint32_t  Int_DMA    :  1;               /*!< [15..15] Interrupt Active for DMA                                     */
    } bit;                                          /*!< [16] BitSize                                                          */
  } NVIC_IABR0;
  __I  uint32_t  RESERVED8[63];
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000E400) Interrupt Priority                                     */
    
    struct {
      __IO uint32_t  PRI_GPT1   :  8;               /*!< [0..7] Priority for GPT1                                              */
      __IO uint32_t  PRI_GPT2   :  8;               /*!< [8..15] Priority for GPT2                                             */
      __IO uint32_t  PRI_ADC2   :  8;               /*!< [16..23] Priority for MU, ADC2                                        */
      __IO uint32_t  PRI_ADC1   :  8;               /*!< [24..31] Priority for ADC1                                            */
    } bit;                                          /*!< [32] BitSize                                                          */
  } NVIC_IPR0;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000E404) Interrupt Priority                                     */
    
    struct {
      __IO uint32_t  PRI_CCU6SR0:  8;               /*!< [0..7] Priority for CCU6 SR0                                          */
      __IO uint32_t  PRI_CCU6SR1:  8;               /*!< [8..15] Priority for CCU6 SR1                                         */
      __IO uint32_t  PRI_CCU6SR2:  8;               /*!< [16..23] Priority for CCU6 SR2                                        */
      __IO uint32_t  PRI_CCU6SR3:  8;               /*!< [24..31] Priority for CCU6 SR3                                        */
    } bit;                                          /*!< [32] BitSize                                                          */
  } NVIC_IPR1;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000E408) Interrupt Priority                                     */
    
    struct {
      __IO uint32_t  PRI_SSC1   :  8;               /*!< [0..7] Priority for SSC1                                              */
      __IO uint32_t  PRI_SSC2   :  8;               /*!< [8..15] Priority for SSC2                                             */
      __IO uint32_t  PRI_UART1  :  8;               /*!< [16..23] Priority for UART1                                           */
      __IO uint32_t  PRI_UART2  :  8;               /*!< [24..31] Priority for UART2                                           */
    } bit;                                          /*!< [32] BitSize                                                          */
  } NVIC_IPR2;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000E40C) Interrupt Priority                                     */
    
    struct {
      __IO uint32_t  PRI_EXINT0 :  8;               /*!< [0..7] Priority for Ext. Int 0                                        */
      __IO uint32_t  PRI_EXINT1 :  8;               /*!< [8..15] Priority for Ext. Int 1                                       */
      __IO uint32_t  PRI_BDRV   :  8;               /*!< [16..23] Priority for Bridge Driver                                   */
      __IO uint32_t  PRI_DMA    :  8;               /*!< [24..31] Priority for DMA                                             */
    } bit;                                          /*!< [32] BitSize                                                          */
  } NVIC_IPR3;
  __I  uint32_t  RESERVED9[572];
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0xE000ED00) CPU ID Base Register                                   */
    
    struct {
      __I  uint32_t  REVISION   :  4;               /*!< [0..3] Revision Number                                                */
      __I  uint32_t  PARTNO     : 12;               /*!< [4..15] Part Number                                                   */
      __I  uint32_t  ARCHITECTURE:  4;              /*!< [16..19] Architecture                                                 */
      __I  uint32_t  VARIANT    :  4;               /*!< [20..23] Variant Number                                               */
      __I  uint32_t  IMPLEMENTER:  8;               /*!< [24..31] Implementer Code                                             */
    } bit;                                          /*!< [32] BitSize                                                          */
  } CPUID;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000ED04) Interrupt Control State Register                       */
    
    struct {
      __I  uint32_t  VECTACTIVE :  9;               /*!< [0..8] VECTACTIVE                                                     */
           uint32_t             :  2;
      __I  uint32_t  RETTOBASE  :  1;               /*!< [11..11] RETTOBASE                                                    */
      __I  uint32_t  VECTPENDING:  9;               /*!< [12..20] VECTPENDING                                                  */
           uint32_t             :  1;
      __I  uint32_t  ISRPENDING :  1;               /*!< [22..22] ISRPENDING                                                   */
      __I  uint32_t  ISRPREEMPT :  1;               /*!< [23..23] ISRPREEMPT                                                   */
           uint32_t             :  1;
      __O  uint32_t  PENDSTCLR  :  1;               /*!< [25..25] PENDSTCLR                                                    */
      __IO uint32_t  PENDSTSET  :  1;               /*!< [26..26] PENDSTSET                                                    */
      __O  uint32_t  PENDSVCLR  :  1;               /*!< [27..27] PENDSVCLR                                                    */
      __IO uint32_t  PENDSVSET  :  1;               /*!< [28..28] PENDSVSET                                                    */
           uint32_t             :  2;
      __IO uint32_t  NMIPENDSET :  1;               /*!< [31..31] NMI PendSet                                                  */
    } bit;                                          /*!< [32] BitSize                                                          */
  } ICSR;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000ED08) Vector Table Offset Register                           */
    
    struct {
           uint32_t             :  7;
      __IO uint32_t  TBLOFF     : 25;               /*!< [7..31] Vector Table Offset                                           */
    } bit;                                          /*!< [32] BitSize                                                          */
  } VTOR;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000ED0C) Application Interrupt/Reset Control Register           */
    
    struct {
      __O  uint32_t  VECTRESET  :  1;               /*!< [0..0] VECTRESET                                                      */
      __O  uint32_t  VECTCLRACTIVE:  1;             /*!< [1..1] VECTCLRACTIVE                                                  */
      __IO uint32_t  SYSRESETREQ:  1;               /*!< [2..2] System Reset Request                                           */
           uint32_t             :  5;
      __IO uint32_t  PRIGROUP   :  3;               /*!< [8..10] Priority Grouping                                             */
           uint32_t             :  4;
      __I  uint32_t  ENDIANNESS :  1;               /*!< [15..15] Memory System Endianness                                     */
      __IO uint32_t  VECTKEY    : 16;               /*!< [16..31] Vector Key                                                   */
    } bit;                                          /*!< [32] BitSize                                                          */
  } AIRCR;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000ED10) System Control Register                                */
    
    struct {
           uint32_t             :  1;
      __IO uint32_t  SLEEPONEXIT:  1;               /*!< [1..1] Sleep on Exit                                                  */
      __IO uint32_t  SLEEPDEEP  :  1;               /*!< [2..2] Sleep Deep                                                     */
           uint32_t             :  1;
      __IO uint32_t  SEVONPEND  :  1;               /*!< [4..4] SEVONPEND                                                      */
    } bit;                                          /*!< [5] BitSize                                                           */
  } SCR;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000ED14) Configuration Control Register                         */
    
    struct {
      __IO uint32_t  NONBASETHRDENA:  1;            /*!< [0..0] NONBASETHRDENA                                                 */
      __IO uint32_t  USERSETMPEND:  1;              /*!< [1..1] USERSETMPEND                                                   */
           uint32_t             :  1;
      __IO uint32_t  UNALIGN_TRP:  1;               /*!< [3..3] UNALIGN_TRP                                                    */
      __IO uint32_t  DIV_0_TRP  :  1;               /*!< [4..4] DIV_0_TRP                                                      */
           uint32_t             :  3;
      __IO uint32_t  BFHFMIGN   :  1;               /*!< [8..8] BFHFMIGN                                                       */
      __IO uint32_t  STKALIGN   :  1;               /*!< [9..9] STKALIGN                                                       */
    } bit;                                          /*!< [10] BitSize                                                          */
  } CCR;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000ED18) System Handler Priority Register 1                     */
    
    struct {
      __IO uint32_t  PRI_4      :  8;               /*!< [0..7] Priority of System Handler 4, MemManage                        */
      __IO uint32_t  PRI_5      :  8;               /*!< [8..15] Priority of System Handler 5, BusFault                        */
      __IO uint32_t  PRI_6      :  8;               /*!< [16..23] Priority of System Handler 6, UsageFault                     */
      __IO uint32_t  PRI_7      :  8;               /*!< [24..31] Reserved for Priority of System Handler 7                    */
    } bit;                                          /*!< [32] BitSize                                                          */
  } SHPR1;
  __I  uint32_t  RESERVED10;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000ED20) System Handler Priority Register 3                     */
    
    struct {
      __IO uint32_t  PRI_12     :  8;               /*!< [0..7] Priority of System Handler 12, DebugMonitor                    */
      __IO uint32_t  PRI_13     :  8;               /*!< [8..15] Reserved for Priority of System Handler 13                    */
      __IO uint32_t  PRI_14     :  8;               /*!< [16..23] Priority of System Handler 14, PendSV                        */
      __IO uint32_t  PRI_15     :  8;               /*!< [24..31] Priority of System Handler 15, SysTick                       */
    } bit;                                          /*!< [32] BitSize                                                          */
  } SHPR3;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000ED24) System Handler Control and State Register              */
    
    struct {
      __IO uint32_t  MEMFAULTACT:  1;               /*!< [0..0] MEMFAULTACT                                                    */
      __IO uint32_t  BUSFAULTACT:  1;               /*!< [1..1] BUSFAULTACT                                                    */
           uint32_t             :  1;
      __IO uint32_t  USGFAULTACT:  1;               /*!< [3..3] USGFAULTACT                                                    */
           uint32_t             :  3;
      __IO uint32_t  SVCALLACT  :  1;               /*!< [7..7] SVCALLACT                                                      */
      __IO uint32_t  MONITORACT :  1;               /*!< [8..8] MONITORACT                                                     */
           uint32_t             :  1;
      __IO uint32_t  PENDSVACT  :  1;               /*!< [10..10] PENDSVACT                                                    */
      __IO uint32_t  SYSTICKACT :  1;               /*!< [11..11] SYSTICKACT                                                   */
      __IO uint32_t  USGFAULTPENDED:  1;            /*!< [12..12] USGFAULTPENDED                                               */
      __IO uint32_t  MEMFAULTPENDED:  1;            /*!< [13..13] MEMFAULTPENDED                                               */
      __IO uint32_t  BUSFAULTPENDED:  1;            /*!< [14..14] BUSFAULTPENDED                                               */
      __IO uint32_t  SVCALLPENDED:  1;              /*!< [15..15] SVCALLPENDED                                                 */
      __IO uint32_t  MEMFAULTENA:  1;               /*!< [16..16] MEMFAULTENA                                                  */
      __IO uint32_t  BUSFAULTENA:  1;               /*!< [17..17] BUSFAULTENA                                                  */
      __IO uint32_t  USGFAULTENA:  1;               /*!< [18..18] USGFAULTENA                                                  */
    } bit;                                          /*!< [19] BitSize                                                          */
  } SHCSR;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000ED28) Configurable Fault Status Register                     */
    
    struct {
      __IO uint32_t  IACCVIOL   :  1;               /*!< [0..0] IACCVIOL                                                       */
      __IO uint32_t  DACCVIOL   :  1;               /*!< [1..1] DACCVIOL                                                       */
           uint32_t             :  1;
      __IO uint32_t  MUNSTKERR  :  1;               /*!< [3..3] MUNSTKERR                                                      */
      __IO uint32_t  MSTERR     :  1;               /*!< [4..4] MSTERR                                                         */
      __IO uint32_t  MLSPERR    :  1;               /*!< [5..5] MLSPERR                                                        */
           uint32_t             :  1;
      __IO uint32_t  MMARVALID  :  1;               /*!< [7..7] MMARVALID                                                      */
      __IO uint32_t  IBUSERR    :  1;               /*!< [8..8] IBUSERR                                                        */
      __IO uint32_t  PRECISERR  :  1;               /*!< [9..9] PRECISERR                                                      */
      __IO uint32_t  IMPRECISERR:  1;               /*!< [10..10] IMPRECISERR                                                  */
      __IO uint32_t  UNSTKERR   :  1;               /*!< [11..11] UNSTKERR                                                     */
      __IO uint32_t  STKERR     :  1;               /*!< [12..12] STKERR                                                       */
      __IO uint32_t  LSPERR     :  1;               /*!< [13..13] LSPERR                                                       */
           uint32_t             :  1;
      __IO uint32_t  BFARVALID  :  1;               /*!< [15..15] BFAR Valid                                                   */
      __IO uint32_t  UNDEFINSTR :  1;               /*!< [16..16] Undefined Instruction                                        */
      __IO uint32_t  INVSTATE   :  1;               /*!< [17..17] INVSTATE                                                     */
      __IO uint32_t  INVPC      :  1;               /*!< [18..18] INVPC                                                        */
      __IO uint32_t  NOCP       :  1;               /*!< [19..19] No CP                                                        */
           uint32_t             :  4;
      __IO uint32_t  UNALIGNED  :  1;               /*!< [24..24] Unaligned                                                    */
      __IO uint32_t  DIVBYZERO  :  1;               /*!< [25..25] Divide by Zero                                               */
    } bit;                                          /*!< [26] BitSize                                                          */
  } CFSR;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000ED2C) Hard Fault Status Register                             */
    
    struct {
           uint32_t             :  1;
      __IO uint32_t  VECTTBL    :  1;               /*!< [1..1] VECTTBL                                                        */
           uint32_t             : 28;
      __IO uint32_t  FORCED     :  1;               /*!< [30..30] Forced                                                       */
      __IO uint32_t  DEBUGEVT   :  1;               /*!< [31..31] Debug Event                                                  */
    } bit;                                          /*!< [32] BitSize                                                          */
  } HFSR;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000ED30) Debug Fault Status Register                            */
    
    struct {
      __IO uint32_t  HALTED     :  1;               /*!< [0..0] HALTED                                                         */
      __IO uint32_t  BKPT       :  1;               /*!< [1..1] BKPT                                                           */
      __IO uint32_t  DWTTRAP    :  1;               /*!< [2..2] DWTTRAP                                                        */
      __IO uint32_t  VCATCH     :  1;               /*!< [3..3] Vector Catch                                                   */
      __IO uint32_t  EXTERNAL   :  1;               /*!< [4..4] External                                                       */
    } bit;                                          /*!< [5] BitSize                                                           */
  } DFSR;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000ED34) MemManage Fault Status Register                        */
    
    struct {
      __IO uint32_t  ADDRESS    : 32;               /*!< [0..31] Data Address for an MPU Fault                                 */
    } bit;                                          /*!< [32] BitSize                                                          */
  } MMFAR;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000ED38) Bus Fault Status Register                              */
    
    struct {
      __IO uint32_t  ADDRESS    : 32;               /*!< [0..31] Data Address for a precise BusFault                           */
    } bit;                                          /*!< [32] BitSize                                                          */
  } BFAR;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0xE000ED3C) Auxiliary Fault Status Register                        */
    
    struct {
      __IO uint32_t  CP0        :  2;               /*!< [0..1] Access Privileges for Coprocessor 0 (n= 0-7, 10, 11)           */
      __IO uint32_t  CP1        :  2;               /*!< [2..3] Access Privileges for Coprocessor 1 (n= 0-7, 10, 11)           */
      __IO uint32_t  CP2        :  2;               /*!< [4..5] Access Privileges for Coprocessor 2 (n= 0-7, 10, 11)           */
      __IO uint32_t  CP3        :  2;               /*!< [6..7] Access Privileges for Coprocessor 3 (n= 0-7, 10, 11)           */
      __IO uint32_t  CP4        :  2;               /*!< [8..9] Access Privileges for Coprocessor 4 (n= 0-7, 10, 11)           */
      __IO uint32_t  CP5        :  2;               /*!< [10..11] Access Privileges for Coprocessor 5 (n= 0-7, 10, 11)         */
      __IO uint32_t  CP6        :  2;               /*!< [12..13] Access Privileges for Coprocessor 6 (n= 0-7, 10, 11)         */
      __IO uint32_t  CP7        :  2;               /*!< [14..15] Access Privileges for Coprocessor 7 (n= 0-7, 10, 11)         */
           uint32_t             :  4;
      __IO uint32_t  CP10       :  2;               /*!< [20..21] Access Privileges for Coprocessor 10 (n= 0-7, 10, 11)        */
      __IO uint32_t  CP11       :  2;               /*!< [22..23] Access Privileges for Coprocessor 11 (n= 0-7, 10, 11)        */
    } bit;                                          /*!< [24] BitSize                                                          */
  } AFSR;
} CPU_Type;


/* ================================================================================ */
/* ================                       DMA                      ================ */
/* ================================================================================ */


/**
  * @brief DMA (DMA)
  */

typedef struct {                                    /*!< (@ 0x50014000) DMA Structure                                          */
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x50014000) DMA Status                                             */
    
    struct {
      __I  uint32_t  MASTER_ENABLE:  1;             /*!< [0..0] Enable Status of the Controller                                */
           uint32_t             :  3;
      __I  uint32_t  STATE      :  4;               /*!< [4..7] Current State of the Control State Machine                     */
           uint32_t             :  8;
      __I  uint32_t  CHNLS_MINUS1:  5;              /*!< [16..20] Available Channels minus 1                                   */
    } bit;                                          /*!< [21] BitSize                                                          */
  } STATUS;
  
  union {
    __O  uint32_t  reg;                             /*!< (@ 0x50014004) DMA Configuration                                      */
    
    struct {
      __O  uint32_t  MASTER_ENABLE:  1;             /*!< [0..0] Enable for the Controller                                      */
           uint32_t             :  4;
      __O  uint32_t  CHN1_PROT_CTRL:  3;            /*!< [5..7] CHN1_PROT_CTRL                                                 */
    } bit;                                          /*!< [8] BitSize                                                           */
  } CFG;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x50014008) Channel Control Data Base Pointer                      */
    
    struct {
           uint32_t             :  9;
      __IO uint32_t  CTRL_BASE_PTR: 23;             /*!< [9..31] CTRL_BASE_PTR                                                 */
    } bit;                                          /*!< [32] BitSize                                                          */
  } CTRL_BASE_PTR;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x5001400C) Channel Alternate Control Data Base Pointer            */
    
    struct {
      __I  uint32_t  ALT_CTRL_BASE_PTR: 32;         /*!< [0..31] Base Address of the Alternate Data Structure                  */
    } bit;                                          /*!< [32] BitSize                                                          */
  } ALT_CTRL_BASE_PTR;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x50014010) Channel Wait on Request Status                         */
    
    struct {
      __I  uint32_t  WAITONREQ_STATUS: 14;          /*!< [0..13] Channel Wait on Request Status                                */
    } bit;                                          /*!< [14] BitSize                                                          */
  } WAITONREQ_STATUS;
  
  union {
    __O  uint32_t  reg;                             /*!< (@ 0x50014014) Channel Software Request                               */
    
    struct {
      __O  uint32_t  CHNL_SW_REQUEST: 14;           /*!< [0..13] CHNL_SW_REQUEST                                               */
    } bit;                                          /*!< [14] BitSize                                                          */
  } CHNL_SW_REQUEST;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x50014018) Channel Useburst Set                                   */
    
    struct {
      __IO uint32_t  CHNL_USEBURST_SET: 14;         /*!< [0..13] CHNL_USEBURST_SET: 0b0=on read: DMA channel n responds
                                                         to requests that it receives on dma_req[C] or dma_sreq[C]. The
                                                          controller performs 2, or single, bus transfers., 0b1=on read:
                                                          DMA channel n does not respond to requests that it receives
                                                          on dma_req[C] or dma_sreq[C]. The controller only reponds to
                                                          dma_req[C] requests and performs 2 transfers., 0b0=on write:
                                                          No effect. Use the CHNL_USEBURST_CLR Register to set bit [C]
                                                          to 0., 0b1=on write: Disables dma_sreq[C] from generating DMA
                                                          requests. The controller                                             */
    } bit;                                          /*!< [14] BitSize                                                          */
  } CHNL_USEBURST_SET;
  
  union {
    __O  uint32_t  reg;                             /*!< (@ 0x5001401C) Channel Useburst Clear                                 */
    
    struct {
      __O  uint32_t  CHNL_USEBURST_CLR: 14;         /*!< [0..13] CHNL_USEBURST_CLR                                             */
    } bit;                                          /*!< [14] BitSize                                                          */
  } CHNL_USEBURST_CLR;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x50014020) Channel Request Mask Set                               */
    
    struct {
      __IO uint32_t  CHNL_REQ_MASK_SET: 14;         /*!< [0..13] CHNL_REQ_MASK_SET: 0b0=on read: External requests are
                                                         enabled for channel C., 0b1=on read: External requests are disabled
                                                          for channel C., 0b0=on write: No effect. Use the CHNL_REQ_MASK_CLR
                                                          Register to enable DMA requests., 0b1=on write: Disables dma_req[C]
                                                          and dma_sreq[C] from generating DMA requests.,                       */
    } bit;                                          /*!< [14] BitSize                                                          */
  } CHNL_REQ_MASK_SET;
  
  union {
    __O  uint32_t  reg;                             /*!< (@ 0x50014024) Channel Request Mask Clear                             */
    
    struct {
      __O  uint32_t  CHNL_REQ_MASK_CLR: 14;         /*!< [0..13] CHNL_REQ_MASK_CLR                                             */
    } bit;                                          /*!< [14] BitSize                                                          */
  } CHNL_REQ_MASK_CLR;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x50014028) Channel Enable Set                                     */
    
    struct {
      __IO uint32_t  CHNL_ENABLE_SET: 14;           /*!< [0..13] CHNL_ENABLE_SET: 0b0=on read: Channel C is disabled.,
                                                         0b1=on read: Channel C is enabled., 0b0=on write: No effect.
                                                          Use the CHNL_ENABLE_CLR Register to disable a channel., 0b1=on
                                                          write: Enables channel C.,                                           */
    } bit;                                          /*!< [14] BitSize                                                          */
  } CHNL_ENABLE_SET;
  
  union {
    __O  uint32_t  reg;                             /*!< (@ 0x5001402C) Channel Enable Clear                                   */
    
    struct {
      __O  uint32_t  CHNL_ENABLE_CLR: 14;           /*!< [0..13] CHNL_ENABLE_CLR                                               */
    } bit;                                          /*!< [14] BitSize                                                          */
  } CHNL_ENABLE_CLR;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x50014030) Channel Primary-Alternate Set                          */
    
    struct {
      __IO uint32_t  CHNL_PRI_ALT_SET: 14;          /*!< [0..13] CHNL_PRI_ALT_SET: 0b0=on read: DMA channel C is using
                                                         the primary data structure., 0b1=on read: DMA channel C is using
                                                          the alternate data structure., 0b0=on write: No effect. Use
                                                          the CHNL_PRI_ALT_CLR Register to set bit [C] to 0., 0b1=on write:
                                                          Selects the alternate data structure for channel C.,                 */
    } bit;                                          /*!< [14] BitSize                                                          */
  } CHNL_PRI_ALT_SET;
  
  union {
    __O  uint32_t  reg;                             /*!< (@ 0x50014034) Channel Primary-Alternate Clear                        */
    
    struct {
      __O  uint32_t  CHNL_PRI_ALT_CLR: 14;          /*!< [0..13] CHNL_PRI_ALT_CLR                                              */
    } bit;                                          /*!< [14] BitSize                                                          */
  } CHNL_PRI_ALT_CLR;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x50014038) Channel Priority Set                                   */
    
    struct {
      __IO uint32_t  CHNL_PRIORITY_SET: 14;         /*!< [0..13] CHNL_PRIORITY_SET: 0b0=on read: DMA channel C is using
                                                         the default priority level., 0b1=on read: DMA channel C is using
                                                          a high priority level., 0b0=on write: No effect. Use the CHNL_ENABLE_CLR
                                                          Register to set channel C to the default priority level., 0b1=on
                                                          write: Channel C uses the high priority level.,                      */
    } bit;                                          /*!< [14] BitSize                                                          */
  } CHNL_PRIORITY_SET;
  
  union {
    __O  uint32_t  reg;                             /*!< (@ 0x5001403C) Channel Priority Clear                                 */
    
    struct {
      __O  uint32_t  CHNL_PRIORITY_CLR: 14;         /*!< [0..13] CHNL_PRIORITY_CLR                                             */
    } bit;                                          /*!< [14] BitSize                                                          */
  } CHNL_PRIORITY_CLR;
  __I  uint32_t  RESERVED0[3];
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x5001404C) Bus Error Clear                                        */
    
    struct {
      __IO uint32_t  ERR_CLR    :  1;               /*!< [0..0] ERR_CLR: 0b0=on read: dma_err is LOW., 0b1=on read: dma_err
                                                         is HIGH., 0b0=on write: No effect, status of dma_err is unchanged.,
                                                          0b1=on write: Sets dma_err LOW.,                                     */
    } bit;                                          /*!< [1] BitSize                                                           */
  } ERR_CLR;
} DMA_Type;


/* ================================================================================ */
/* ================                     TIMER3                     ================ */
/* ================================================================================ */


/**
  * @brief TIMER3 (TIMER3)
  */

typedef struct {                                    /*!< (@ 0x48006000) TIMER3 Structure                                       */
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x48006000) T3 Trigger Control                                     */
    
    struct {
      __IO uint32_t  T3_TRIGG_INP_SEL:  3;          /*!< [0..2] Timer 3 Trigger Input Event Selection (only in mode3b)         */
           uint32_t             :  1;
      __IO uint32_t  T3_RES_CONF:  2;               /*!< [4..5] Timer 3 Trigger Reset Selection for Mode 1b                    */
      __IO uint32_t  RETRIG     :  1;               /*!< [6..6] Retrigger Condition (in mode 1b) for CCU6-T12 ZM and
                                                         CCU6 PM                                                               */
    } bit;                                          /*!< [7] BitSize                                                           */
  } T3_TRIGG_CTRL;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x48006004) Timer 3 Compare Value                                  */
    
    struct {
      __IO uint32_t  LO         :  8;               /*!< [0..7] Timer 3 Compare Value Low Byte                                 */
      __IO uint32_t  HI         :  8;               /*!< [8..15] Timer 3 Compare Value High Byte                               */
    } bit;                                          /*!< [16] BitSize                                                          */
  } CMP;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x48006008) Timer 3                                                */
    
    struct {
      __IO uint32_t  LO         :  8;               /*!< [0..7] Timer 3 Low Register or Preload Value                          */
      __IO uint32_t  HI         :  8;               /*!< [8..15] Timer 3 High Register or Preload Value                        */
    } bit;                                          /*!< [16] BitSize                                                          */
  } CNT;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4800600C) Timer 3 Control Register                               */
    
    struct {
      __IO uint32_t  T3_PD_N    :  1;               /*!< [0..0] Timer 3 Power Down                                             */
      __IO uint32_t  T3_RD_REQ  :  1;               /*!< [1..1] Timer 3 Value Read Request                                     */
      __IO uint32_t  T3_RD_REQ_CONF:  1;            /*!< [2..2] Timer 3 Read Mode                                              */
      __IO uint32_t  CNT_RDY    :  1;               /*!< [3..3] Timer 3 Count Ready                                            */
      __IO uint32_t  TR3H       :  1;               /*!< [4..4] Timer 3 Run Control (High Byte Timer)                          */
      __I  uint32_t  T3H_OVF_STS:  1;               /*!< [5..5] Timer 3 Overflow Flag (High Byte Timer)                        */
      __IO uint32_t  TR3L       :  1;               /*!< [6..6] Timer 3 Run Control (Low Byte Timer)                           */
      __I  uint32_t  T3L_OVF_STS:  1;               /*!< [7..7] Timer 3 Overflow Flag (Low Byte Timer)                         */
      __IO uint32_t  T3L_OVF_IE :  1;               /*!< [8..8] Timer 3 Overflow Interrupt Enable (Low Byte Timer)             */
      __IO uint32_t  T3H_OVF_IE :  1;               /*!< [9..9] Timer 3 Overflow Interrupt Enable (High Byte Timer)            */
    } bit;                                          /*!< [10] BitSize                                                          */
  } CTRL;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x48006010) Timer 3 Mode Configuration Register                    */
    
    struct {
      __IO uint32_t  T3M        :  2;               /*!< [0..1] Mode Select Bits                                               */
           uint32_t             :  4;
      __IO uint32_t  T3_SUBM    :  2;               /*!< [6..7] Sub-Mode Select Bits                                           */
    } bit;                                          /*!< [8] BitSize                                                           */
  } MODE_CONF;
  
  union {
    __O  uint32_t  reg;                             /*!< (@ 0x48006014) Timer 3 Interrupt Status Clear Register                */
    
    struct {
           uint32_t             :  5;
      __O  uint32_t  T3H_OVF_ICLR:  1;              /*!< [5..5] Timer 3 Overflow Flag (High Byte Timer) Interrupt Clear        */
           uint32_t             :  1;
      __O  uint32_t  T3L_OVF_ICLR:  1;              /*!< [7..7] Timer 3 Overflow Flag (Low Byte Timer) Interrupt Clear         */
    } bit;                                          /*!< [8] BitSize                                                           */
  } ISRCLR;
} TIMER3_Type;


/* ================================================================================ */
/* ================                       LIN                      ================ */
/* ================================================================================ */


/**
  * @brief LIN (LIN)
  */

typedef struct {                                    /*!< (@ 0x4801E000) LIN Structure                                          */
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801E000) LIN Transceiver Control and Status                     */
    
    struct {
           uint32_t             :  1;
      __IO uint32_t  MODE       :  2;               /*!< [1..2] LIN transceiver power mode control                             */
      __I  uint32_t  M_SM_ERR   :  1;               /*!< [3..3] LIN Transceiver Mode or Slope Mode Error                       */
      __I  uint32_t  OT_STS     :  1;               /*!< [4..4] LIN Receiver Overtemperature Status                            */
      __I  uint32_t  OC_STS     :  1;               /*!< [5..5] LIN Receiver Overcurrent Status                                */
      __I  uint32_t  TXD_TMOUT_STS:  1;             /*!< [6..6] LIN TXD time-out status                                        */
           uint32_t             :  2;
      __IO uint32_t  TXD        :  1;               /*!< [9..9] LIN Transmitter switch on (only used when LIN_HV_MODE
                                                         is set)                                                               */
      __I  uint32_t  RXD        :  1;               /*!< [10..10] Output Signal of Receiver                                    */
      __IO uint32_t  SM         :  2;               /*!< [11..12] LIN Transmitter Slope mode control                           */
      __I  uint32_t  FB_SM1     :  1;               /*!< [13..13] Feedback Signal 1 for Slope Mode Setting                     */
      __I  uint32_t  FB_SM2     :  1;               /*!< [14..14] Feedback Signal 2 for Slope Mode Setting                     */
      __I  uint32_t  FB_SM3     :  1;               /*!< [15..15] Feedback Signal 3 for Slope Mode Setting                     */
      __I  uint32_t  MODE_FB    :  3;               /*!< [16..18] Feedback Signals for LIN Transmitter Mode Settings           */
           uint32_t             :  2;
      __IO uint32_t  HV_MODE    :  1;               /*!< [21..21] LIN Transceiver High Voltage Input - Output Mode             */
           uint32_t             :  2;
      __O  uint32_t  M_SM_ERR_CLR:  1;              /*!< [24..24] LIN Transceiver Mode or Slope Mode Error Clear               */
    } bit;                                          /*!< [25] BitSize                                                          */
  } CTRL_STS;
} LIN_Type;


/* ================================================================================ */
/* ================                       MF                       ================ */
/* ================================================================================ */


/**
  * @brief MF (MF)
  */

typedef struct {                                    /*!< (@ 0x48018000) MF Structure                                           */
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x48018000) Port 2 ADC Selection Control Register                  */
    
    struct {
           uint32_t             : 10;
      __IO uint32_t  ADC1_CH1_SEL:  1;              /*!< [10..10] ADC1 Channel 1 Input Selection                               */
    } bit;                                          /*!< [11] BitSize                                                          */
  } P2_ADCSEL_CTRL;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x48018004) Supply Sense Control Register                          */
    
    struct {
      __IO uint32_t  VMON_SEN_PD_N:  1;             /*!< [0..0] Monitoring Input Attenuator enable                             */
           uint32_t             :  3;
      __IO uint32_t  VMON_SEN_HRESO_5V:  1;         /*!< [4..4] Monitoring Input Attenuator High Impedance Output Control      */
      __IO uint32_t  VMON_SEN_SEL_INRANGE:  1;      /*!< [5..5] Monitoring Input Attenuator Select Inputrange                  */
    } bit;                                          /*!< [6] BitSize                                                           */
  } VMON_SEN_CTRL;
  __I  uint32_t  RESERVED0[2];
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x48018010) Temperature Sensor Control Register                    */
    
    struct {
           uint32_t             :  4;
      __I  uint32_t  PMU_OTWARN_STS:  1;            /*!< [4..4] PMU Regulator Overtemperature Warning (MU) Status              */
      __I  uint32_t  PMU_OT_STS :  1;               /*!< [5..5] PMU Regulator Overtemperature (MU) Status                      */
      __I  uint32_t  SYS_OTWARN_STS:  1;            /*!< [6..6] System Overtemperature Warning (MU) Status                     */
      __I  uint32_t  SYS_OT_STS :  1;               /*!< [7..7] System Overtemperature (MU) Status                             */
    } bit;                                          /*!< [8] BitSize                                                           */
  } TEMPSENSE_CTRL;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x48018014) Reference 1 Status Register                            */
    
    struct {
           uint32_t             :  4;
      __I  uint32_t  REFBG_LOTHWARN_STS:  1;        /*!< [4..4] Status for Undervoltage Threshold Measurement of internal
                                                         VAREF                                                                 */
      __I  uint32_t  REFBG_UPTHWARN_STS:  1;        /*!< [5..5] Status for Overvoltage Threshold Measurement of internal
                                                         VAREF                                                                 */
    } bit;                                          /*!< [6] BitSize                                                           */
  } REF1_STS;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x48018018) Reference 2 Control Register                           */
    
    struct {
      __IO uint32_t  VREF5V_PD_N:  1;               /*!< [0..0] ADC1 Bit Reference Voltage Generation Power Down Bit           */
      __I  uint32_t  VREF5V_OVL_STS:  1;            /*!< [1..1] ADC1 Bit Reference Voltage Generation Over Load Bit            */
      __I  uint32_t  VREF5V_UV_STS:  1;             /*!< [2..2] ADC1 Bit Reference Voltage Generation Undervoltage Bit         */
      __I  uint32_t  VREF5V_OV_STS:  1;             /*!< [3..3] ADC1 Bit Reference Voltage Generation Overvoltage Bit          */
    } bit;                                          /*!< [4] BitSize                                                           */
  } REF2_CTRL;
} MF_Type;


/* ================================================================================ */
/* ================                      ADC2                      ================ */
/* ================================================================================ */


/**
  * @brief ADC2 (ADC2)
  */

typedef struct {                                    /*!< (@ 0x4801C000) ADC2 Structure                                         */
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C000) ADC2 Control and Status Register                       */
    
    struct {
           uint32_t             : 16;
      __IO uint32_t  VBAT_RANGE :  1;               /*!< [16..16] ADC2 Channel 0 Range Selection                               */
      __IO uint32_t  VS_RANGE   :  1;               /*!< [17..17] ADC2 Channel 1 Range Selection                               */
    } bit;                                          /*!< [18] BitSize                                                          */
  } CTRL_STS;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C004) Sequencer Feedback Register                            */
    
    struct {
      __I  uint32_t  SQ_FB      :  4;               /*!< [0..3] Current Sequence that caused software mode                     */
           uint32_t             :  4;
      __I  uint32_t  SQ_STOP    :  1;               /*!< [8..8] ADC2 Sequencer Stop Signal for DPP                             */
      __I  uint32_t  EIM_ACTIVE :  1;               /*!< [9..9] ADC2 EIM active                                                */
      __I  uint32_t  ESM_ACTIVE :  1;               /*!< [10..10] ADC2 ESM active                                              */
      __I  uint32_t  SQx        :  4;               /*!< [11..14] Current Active Sequencer                                     */
           uint32_t             :  1;
      __I  uint32_t  CHx        :  5;               /*!< [16..20] Current ADC2 Channel                                         */
    } bit;                                          /*!< [21] BitSize                                                          */
  } SQ_FB;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C008) Channel Settings Bits for Exceptional Interrupt
                                                         Measurement                                                           */
    
    struct {
      __IO uint32_t  CHx        :  5;               /*!< [0..4] Channel set for exceptional interrupt measurement (EIM)        */
           uint32_t             :  3;
      __IO uint32_t  REP        :  3;               /*!< [8..10] Repeat count for exceptional interrupt measurement (EIM)      */
      __IO uint32_t  EN         :  1;               /*!< [11..11] Exceptional interrupt measurement (EIM) Trigger Event
                                                         enable                                                                */
      __IO uint32_t  SEL        :  1;               /*!< [12..12] Exceptional interrupt measurement (EIM) Trigger Trigger
                                                         select                                                                */
    } bit;                                          /*!< [13] BitSize                                                          */
  } CHx_EIM;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C00C) Channel Settings Bits for Exceptional Sequence
                                                         Measurement                                                           */
    
    struct {
      __IO uint32_t  ESM_0      :  6;               /*!< [0..5] Channel Sequence for Exceptional Sequence Measurement
                                                         (ESM)                                                                 */
      __IO uint32_t  ESM_1      :  4;               /*!< [6..9] Channel Sequence for Exceptional Sequence Measurement
                                                         (ESM)                                                                 */
      __IO uint32_t  SEL        :  1;               /*!< [10..10] Exceptional Sequence Measurement Trigger Select              */
           uint32_t             :  5;
      __IO uint32_t  EN         :  1;               /*!< [16..16] Enable for Exceptional Sequence Measurement Trigger
                                                         Event                                                                 */
      __I  uint32_t  STS        :  1;               /*!< [17..17] Exceptional Sequence Measurement is finished                 */
    } bit;                                          /*!< [18] BitSize                                                          */
  } CHx_ESM;
  __I  uint32_t  RESERVED0;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C014) Measurement Unit Control Register 1                    */
    
    struct {
      __IO uint32_t  CALIB_EN   :  6;               /*!< [0..5] Calibration Enable for Channels 0 to 5                         */
    } bit;                                          /*!< [6] BitSize                                                           */
  } CTRL1;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C018) Measurement Unit Control Register 2                    */
    
    struct {
      __IO uint32_t  MCM_PD_N   :  1;               /*!< [0..0] Power Down Signal for MCM                                      */
      __IO uint32_t  TS_SD_SEL_CONF:  1;            /*!< [1..1] Temperature Sensor Control Configuration                       */
      __IO uint32_t  TSENSE_SD_SEL:  1;             /*!< [2..2] TSENSE select for channel 9                                    */
           uint32_t             :  4;
      __I  uint32_t  MCM_RDY    :  1;               /*!< [7..7] Ready Signal for MCM after Power On or Reset                   */
      __IO uint32_t  SAMPLE_TIME_int:  4;           /*!< [8..11] Sample time of ADC2                                           */
           uint32_t             :  4;
      __IO uint32_t  SEL_TS_COUNT:  4;              /*!< [16..19] Time for Automatic Muxing of SEL_TS                          */
    } bit;                                          /*!< [20] BitSize                                                          */
  } CTRL2;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C01C) Measurement Unit Control Register 4                    */
    
    struct {
      __IO uint32_t  FILT_OUT_SEL_5_0:  6;          /*!< [0..5] Output Filter Selection for Channels 0 to 5                    */
           uint32_t             :  2;
      __IO uint32_t  FILT_OUT_SEL_9_6:  4;          /*!< [8..11] Output Filter Selection for Channels 6 to 9                   */
    } bit;                                          /*!< [12] BitSize                                                          */
  } CTRL4;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C020) Measurement Channel Enable Bits for Cycle 1-4          */
    
    struct {
      __IO uint32_t  SQ1        :  6;               /*!< [0..5] Sequence 1 channel enable                                      */
           uint32_t             :  2;
      __IO uint32_t  SQ2        :  6;               /*!< [8..13] Sequence 2 channel enable                                     */
           uint32_t             :  2;
      __IO uint32_t  SQ3        :  6;               /*!< [16..21] Sequence 3 channel enable                                    */
           uint32_t             :  2;
      __IO uint32_t  SQ4        :  6;               /*!< [24..29] Sequence 4 channel enable                                    */
    } bit;                                          /*!< [30] BitSize                                                          */
  } SQ1_4;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C024) Measurement Channel Enable Bits for Cycle 5 -
                                                         8                                                                     */
    
    struct {
      __IO uint32_t  SQ5        :  6;               /*!< [0..5] Sequence 5 channel enable                                      */
           uint32_t             :  2;
      __IO uint32_t  SQ6        :  6;               /*!< [8..13] Sequence 6 channel enable                                     */
           uint32_t             :  2;
      __IO uint32_t  SQ7        :  6;               /*!< [16..21] Sequence 7 channel enable                                    */
           uint32_t             :  2;
      __IO uint32_t  SQ8        :  6;               /*!< [24..29] Sequence 8 channel enable                                    */
    } bit;                                          /*!< [30] BitSize                                                          */
  } SQ5_8;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C028) Measurement Channel Enable Bits for Cycle 9 -
                                                         10                                                                    */
    
    struct {
      __IO uint32_t  SQ9        :  6;               /*!< [0..5] Sequence 9 channel enable                                      */
           uint32_t             :  2;
      __IO uint32_t  SQ10       :  6;               /*!< [8..13] Sequence 10 channel enable                                    */
    } bit;                                          /*!< [14] BitSize                                                          */
  } SQ9_10;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C02C) Measurement Channel Enable Bits for Cycle 1 -
                                                         8                                                                     */
    
    struct {
      __I  uint32_t  SQ1_int    :  4;               /*!< [0..3] Sequence 1 channel enable                                      */
      __I  uint32_t  SQ2_int    :  4;               /*!< [4..7] Sequence 2 channel enable                                      */
      __I  uint32_t  SQ3_int    :  4;               /*!< [8..11] Sequence 3 channel enable                                     */
      __I  uint32_t  SQ4_int    :  4;               /*!< [12..15] Sequence 4 channel enable                                    */
      __I  uint32_t  SQ5_int    :  4;               /*!< [16..19] Sequence 5 channel enable                                    */
      __I  uint32_t  SQ6_int    :  4;               /*!< [20..23] Sequence 6 channel enable                                    */
      __I  uint32_t  SQ7_int    :  4;               /*!< [24..27] Sequence 7 channel enable                                    */
      __I  uint32_t  SQ8_int    :  4;               /*!< [28..31] Sequence 8 channel enable                                    */
    } bit;                                          /*!< [32] BitSize                                                          */
  } SQ1_8_int;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C030) Measurement Channel Enable Bits for Cycle 9 and
                                                         10                                                                    */
    
    struct {
      __I  uint32_t  SQ9_int    :  4;               /*!< [0..3] Sequence 9 channel enable                                      */
      __I  uint32_t  SQ10_int   :  4;               /*!< [4..7] Sequence 10 channel enable                                     */
    } bit;                                          /*!< [8] BitSize                                                           */
  } SQ9_10_int;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C034) Calibration for Channel 0 & 1                          */
    
    struct {
      __IO uint32_t  OFFS_CH0   :  8;               /*!< [0..7] Offset Calibration for channel 0                               */
      __IO uint32_t  GAIN_CH0   :  8;               /*!< [8..15] Gain Calibration for channel 0                                */
      __IO uint32_t  OFFS_CH1   :  8;               /*!< [16..23] Offset Calibration for channel 1                             */
      __IO uint32_t  GAIN_CH1   :  8;               /*!< [24..31] Gain Calibration for channel 1                               */
    } bit;                                          /*!< [32] BitSize                                                          */
  } CAL_CH0_1;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C038) Calibration for Channel 2 & 3                          */
    
    struct {
      __IO uint32_t  OFFS_CH2   :  8;               /*!< [0..7] Offset Calibration for channel 2                               */
      __IO uint32_t  GAIN_CH2   :  8;               /*!< [8..15] Gain Calibration for channel 2                                */
      __IO uint32_t  OFFS_CH3   :  8;               /*!< [16..23] Offset Calibration for channel 3                             */
      __IO uint32_t  GAIN_CH3   :  8;               /*!< [24..31] Gain Calibration for channel 3                               */
    } bit;                                          /*!< [32] BitSize                                                          */
  } CAL_CH2_3;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C03C) Calibration for Channel 4 & 5                          */
    
    struct {
      __IO uint32_t  OFFS_CH4   :  8;               /*!< [0..7] Offset Calibration for channel 4                               */
      __IO uint32_t  GAIN_CH4   :  8;               /*!< [8..15] Gain Calibration for channel 4                                */
      __IO uint32_t  OFFS_CH5   :  8;               /*!< [16..23] Offset Calibration for channel 5                             */
      __IO uint32_t  GAIN_CH5   :  8;               /*!< [24..31] Gain Calibration for channel 5                               */
    } bit;                                          /*!< [32] BitSize                                                          */
  } CAL_CH4_5;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C040) Calibration for Channel 6 & 7                          */
    
    struct {
      __I  uint32_t  OFFS_CH6   :  8;               /*!< [0..7] Offset Calibration for channel 6                               */
      __I  uint32_t  GAIN_CH6   :  8;               /*!< [8..15] Gain Calibration for channel 6                                */
      __I  uint32_t  OFFS_CH7   :  8;               /*!< [16..23] Offset Calibration for channel 7                             */
      __I  uint32_t  GAIN_CH7   :  8;               /*!< [24..31] Gain Calibration for channel 7                               */
    } bit;                                          /*!< [32] BitSize                                                          */
  } CAL_CH6_7;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C044) Calibration for Channel 8 & 9                          */
    
    struct {
      __I  uint32_t  OFFS_CH8   :  8;               /*!< [0..7] Offset Calibration for channel 8                               */
      __I  uint32_t  GAIN_CH8   :  8;               /*!< [8..15] Gain Calibration for channel 8                                */
      __I  uint32_t  OFFS_CH9   :  8;               /*!< [16..23] Offset Calibration for channel 9                             */
      __I  uint32_t  GAIN_CH9   :  8;               /*!< [24..31] Gain Calibration for channel 9                               */
    } bit;                                          /*!< [32] BitSize                                                          */
  } CAL_CH8_9;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C048) Filter Coefficients ADC Channel 0-5                    */
    
    struct {
      __IO uint32_t  CH0        :  2;               /*!< [0..1] Filter Coefficients ADC channel 0                              */
      __IO uint32_t  CH1        :  2;               /*!< [2..3] Filter Coefficients ADC channel 1                              */
      __IO uint32_t  CH2        :  2;               /*!< [4..5] Filter Coefficients ADC channel 2                              */
      __IO uint32_t  CH3        :  2;               /*!< [6..7] Filter Coefficients ADC channel 3                              */
      __IO uint32_t  CH4        :  2;               /*!< [8..9] Filter Coefficients ADC channel 4                              */
      __IO uint32_t  CH5        :  2;               /*!< [10..11] Filter Coefficients ADC channel 5                            */
    } bit;                                          /*!< [12] BitSize                                                          */
  } FILTCOEFF0_5;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C04C) Filter Coefficents ADC Channel 6-9                     */
    
    struct {
      __I  uint32_t  CH6        :  2;               /*!< [0..1] Filter Coefficients ADC channel 6                              */
      __I  uint32_t  CH7        :  2;               /*!< [2..3] Filter Coefficients ADC channel 7                              */
      __I  uint32_t  CH8        :  2;               /*!< [4..5] Filter Coefficients ADC channel 8                              */
      __I  uint32_t  CH9        :  2;               /*!< [6..7] Filter Coefficients ADC channel 9                              */
    } bit;                                          /*!< [8] BitSize                                                           */
  } FILTCOEFF6_9;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C050) ADC or Filter Output Channel 0                         */
    
    struct {
      __I  uint32_t  OUT_CH0    : 10;               /*!< [0..9] ADC or filter output value channel 0                           */
    } bit;                                          /*!< [10] BitSize                                                          */
  } FILT_OUT0;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C054) ADC or Filter Output Channel 1                         */
    
    struct {
      __I  uint32_t  OUT_CH1    : 10;               /*!< [0..9] ADC or filter output value channel 1                           */
    } bit;                                          /*!< [10] BitSize                                                          */
  } FILT_OUT1;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C058) ADC or Filter Output Channel 2                         */
    
    struct {
      __I  uint32_t  OUT_CH2    : 10;               /*!< [0..9] ADC or filter output value channel 2                           */
    } bit;                                          /*!< [10] BitSize                                                          */
  } FILT_OUT2;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C05C) ADC or Filter Output Channel 3                         */
    
    struct {
      __I  uint32_t  OUT_CH3    : 10;               /*!< [0..9] ADC or filter output value channel 3                           */
    } bit;                                          /*!< [10] BitSize                                                          */
  } FILT_OUT3;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C060) ADC or Filter Output Channel 4                         */
    
    struct {
      __I  uint32_t  OUT_CH4    : 10;               /*!< [0..9] ADC or filter output value channel 4                           */
    } bit;                                          /*!< [10] BitSize                                                          */
  } FILT_OUT4;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C064) ADC or Filter Output Channel 5                         */
    
    struct {
      __I  uint32_t  OUT_CH5    : 10;               /*!< [0..9] ADC or filter output value channel 5                           */
    } bit;                                          /*!< [10] BitSize                                                          */
  } FILT_OUT5;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C068) ADC or Filter Output Channel 6                         */
    
    struct {
      __I  uint32_t  OUT_CH6    : 10;               /*!< [0..9] ADC or filter output value channel 6                           */
    } bit;                                          /*!< [10] BitSize                                                          */
  } FILT_OUT6;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C06C) ADC or Filter Output Channel 7                         */
    
    struct {
      __I  uint32_t  OUT_CH7    : 10;               /*!< [0..9] ADC or filter output value channel 7                           */
    } bit;                                          /*!< [10] BitSize                                                          */
  } FILT_OUT7;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C070) ADC or Filter Output Channel 8                         */
    
    struct {
      __I  uint32_t  OUT_CH8    : 10;               /*!< [0..9] ADC or filter output value channel 8                           */
    } bit;                                          /*!< [10] BitSize                                                          */
  } FILT_OUT8;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C074) ADC or Filter Output Channel 9                         */
    
    struct {
      __I  uint32_t  OUT_CH9    : 10;               /*!< [0..9] ADC or filter output value channel 9                           */
    } bit;                                          /*!< [10] BitSize                                                          */
  } FILT_OUT9;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C078) Upper Threshold Filter Enable                          */
    
    struct {
      __IO uint32_t  Ch0_EN     :  1;               /*!< [0..0] Upper threshold IIR filter enable ch 0                         */
      __IO uint32_t  Ch1_EN     :  1;               /*!< [1..1] Upper threshold IIR filter enable ch 1                         */
      __IO uint32_t  Ch2_EN     :  1;               /*!< [2..2] Upper threshold IIR filter enable ch 2                         */
      __IO uint32_t  Ch3_EN     :  1;               /*!< [3..3] Upper threshold IIR filter enable ch 3                         */
      __IO uint32_t  Ch4_EN     :  1;               /*!< [4..4] Upper threshold IIR filter enable ch 4                         */
      __IO uint32_t  Ch5_EN     :  1;               /*!< [5..5] Upper threshold IIR filter enable ch 5                         */
    } bit;                                          /*!< [6] BitSize                                                           */
  } FILT_UP_CTRL;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C07C) Lower Threshold Filter Enable                          */
    
    struct {
      __IO uint32_t  Ch0_EN     :  1;               /*!< [0..0] Lower threshold IIR filter enable ch 0                         */
      __IO uint32_t  Ch1_EN     :  1;               /*!< [1..1] Lower threshold IIR filter enable ch 1                         */
      __IO uint32_t  Ch2_EN     :  1;               /*!< [2..2] Lower threshold IIR filter enable ch 2                         */
      __IO uint32_t  Ch3_EN     :  1;               /*!< [3..3] Lower threshold IIR filter enable ch 3                         */
      __IO uint32_t  Ch4_EN     :  1;               /*!< [4..4] Lower threshold IIR filter enable ch 4                         */
      __IO uint32_t  Ch5_EN     :  1;               /*!< [5..5] Lower threshold IIR filter enable ch 5                         */
    } bit;                                          /*!< [6] BitSize                                                           */
  } FILT_LO_CTRL;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C080) Lower Comparator Trigger Level Channel 0 -3            */
    
    struct {
      __IO uint32_t  CH0        :  8;               /*!< [0..7] Channel 0 lower trigger level                                  */
      __IO uint32_t  CH1        :  8;               /*!< [8..15] Channel 1 lower trigger level                                 */
      __IO uint32_t  CH2        :  8;               /*!< [16..23] Channel 2 lower trigger level                                */
      __IO uint32_t  CH3        :  8;               /*!< [24..31] Channel 3 lower trigger level                                */
    } bit;                                          /*!< [32] BitSize                                                          */
  } TH0_3_LOWER;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C084) Lower Comparator Trigger Level Channel 4 & 5           */
    
    struct {
      __IO uint32_t  CH4        :  8;               /*!< [0..7] Channel 4 lower trigger level                                  */
      __IO uint32_t  CH5        :  8;               /*!< [8..15] Channel 5 lower trigger level                                 */
    } bit;                                          /*!< [16] BitSize                                                          */
  } TH4_5_LOWER;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C088) Lower Comparator Trigger Level Channel 6 -9            */
    
    struct {
      __IO uint32_t  CH6        :  8;               /*!< [0..7] Channel 6 lower trigger level                                  */
      __I  uint32_t  CH7        :  8;               /*!< [8..15] Channel 7 lower trigger level                                 */
      __IO uint32_t  CH8        :  8;               /*!< [16..23] Channel 8 lower trigger level                                */
      __IO uint32_t  CH9        :  8;               /*!< [24..31] Channel 9 lower trigger level                                */
    } bit;                                          /*!< [32] BitSize                                                          */
  } TH6_9_LOWER;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C08C) Upper Comparator Trigger Level Channel 0-3             */
    
    struct {
      __IO uint32_t  CH0        :  8;               /*!< [0..7] Channel 0 upper trigger level                                  */
      __IO uint32_t  CH1        :  8;               /*!< [8..15] Channel 1 upper trigger level                                 */
      __IO uint32_t  CH2        :  8;               /*!< [16..23] Channel 2 upper trigger level                                */
      __IO uint32_t  CH3        :  8;               /*!< [24..31] Channel 3 upper trigger level                                */
    } bit;                                          /*!< [32] BitSize                                                          */
  } TH0_3_UPPER;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C090) Upper Comparator Trigger Level Channel 4 -5            */
    
    struct {
      __IO uint32_t  CH4        :  8;               /*!< [0..7] Channel 4 upper trigger level                                  */
      __IO uint32_t  CH5        :  8;               /*!< [8..15] Channel 5 upper trigger level                                 */
    } bit;                                          /*!< [16] BitSize                                                          */
  } TH4_5_UPPER;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C094) Upper Comparator Trigger Level Channel 6 -9            */
    
    struct {
      __I  uint32_t  CH6        :  8;               /*!< [0..7] Channel 6 upper trigger level                                  */
      __I  uint32_t  CH7        :  8;               /*!< [8..15] Channel 7 upper trigger level                                 */
      __I  uint32_t  CH8        :  8;               /*!< [16..23] Channel 8 upper trigger level                                */
      __I  uint32_t  CH9        :  8;               /*!< [24..31] Channel 9 upper trigger level                                */
    } bit;                                          /*!< [32] BitSize                                                          */
  } TH6_9_UPPER;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C098) Lower Counter Trigger Level Channel 0 - 3              */
    
    struct {
      __IO uint32_t  CNT_LO_CH0 :  3;               /*!< [0..2] Lower timer trigger threshold channel 0                        */
      __IO uint32_t  HYST_LO_CH0:  2;               /*!< [3..4] Channel 0 lower hysteresis                                     */
           uint32_t             :  3;
      __IO uint32_t  CNT_LO_CH1 :  3;               /*!< [8..10] Lower timer trigger threshold channel 1                       */
      __IO uint32_t  HYST_LO_CH1:  2;               /*!< [11..12] Channel 1 lower hysteresis                                   */
           uint32_t             :  3;
      __IO uint32_t  CNT_LO_CH2 :  3;               /*!< [16..18] Lower timer trigger threshold channel 2                      */
      __IO uint32_t  HYST_LO_CH2:  2;               /*!< [19..20] Channel 2 lower hysteresis                                   */
           uint32_t             :  3;
      __IO uint32_t  CNT_LO_CH3 :  3;               /*!< [24..26] Lower timer trigger threshold channel 3                      */
      __IO uint32_t  HYST_LO_CH3:  2;               /*!< [27..28] Channel 3 lower hysteresis                                   */
    } bit;                                          /*!< [29] BitSize                                                          */
  } CNT0_3_LOWER;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C09C) Lower Counter Trigger Level Channel 4 & 5              */
    
    struct {
      __IO uint32_t  CNT_LO_CH4 :  3;               /*!< [0..2] Lower timer trigger threshold channel 4                        */
      __IO uint32_t  HYST_LO_CH4:  2;               /*!< [3..4] Channel 4 lower hysteresis                                     */
           uint32_t             :  3;
      __IO uint32_t  CNT_LO_CH5 :  3;               /*!< [8..10] Lower timer trigger threshold channel 5                       */
      __IO uint32_t  HYST_LO_CH5:  2;               /*!< [11..12] Channel 5 lower hysteresis                                   */
    } bit;                                          /*!< [13] BitSize                                                          */
  } CNT4_5_LOWER;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C0A0) Lower Counter Trigger Level Channel 6 - 9              */
    
    struct {
      __I  uint32_t  CNT_LO_CH6 :  3;               /*!< [0..2] Lower timer trigger threshold channel 6                        */
      __I  uint32_t  HYST_LO_CH6:  2;               /*!< [3..4] Channel 6 lower hysteresis                                     */
           uint32_t             :  3;
      __I  uint32_t  CNT_LO_CH7 :  3;               /*!< [8..10] Lower timer trigger threshold channel 7                       */
      __I  uint32_t  HYST_LO_CH7:  2;               /*!< [11..12] Channel 7 lower hysteresis                                   */
           uint32_t             :  3;
      __I  uint32_t  CNT_LO_CH8 :  3;               /*!< [16..18] Lower timer trigger threshold channel 8                      */
      __I  uint32_t  HYST_LO_CH8:  2;               /*!< [19..20] Channel 8 lower hysteresis                                   */
           uint32_t             :  3;
      __I  uint32_t  CNT_LO_CH9 :  3;               /*!< [24..26] Lower timer trigger threshold channel 9                      */
      __I  uint32_t  HYST_LO_CH9:  2;               /*!< [27..28] Channel 9 lower hysteresis                                   */
    } bit;                                          /*!< [29] BitSize                                                          */
  } CNT6_9_LOWER;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C0A4) Upper Counter Trigger Level Channel 0 - 3              */
    
    struct {
      __IO uint32_t  CNT_UP_CH0 :  3;               /*!< [0..2] Upper timer trigger threshold channel 0                        */
      __IO uint32_t  HYST_UP_CH0:  2;               /*!< [3..4] Channel 0 upper hysteresis                                     */
           uint32_t             :  3;
      __IO uint32_t  CNT_UP_CH1 :  3;               /*!< [8..10] Upper timer trigger threshold channel 1                       */
      __IO uint32_t  HYST_UP_CH1:  2;               /*!< [11..12] Channel 1 upper hysteresis                                   */
           uint32_t             :  3;
      __IO uint32_t  CNT_UP_CH2 :  3;               /*!< [16..18] Upper timer trigger threshold channel 2                      */
      __IO uint32_t  HYST_UP_CH2:  2;               /*!< [19..20] Channel 2 upper hysteresis                                   */
           uint32_t             :  3;
      __IO uint32_t  CNT_UP_CH3 :  3;               /*!< [24..26] Upper timer trigger threshold channel 3                      */
      __IO uint32_t  HYST_UP_CH3:  2;               /*!< [27..28] Channel 3 upper hysteresis                                   */
    } bit;                                          /*!< [29] BitSize                                                          */
  } CNT0_3_UPPER;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C0A8) Upper Counter Trigger Level Channel 4 & 5              */
    
    struct {
      __IO uint32_t  CNT_UP_CH4 :  3;               /*!< [0..2] Upper timer trigger threshold channel 4                        */
      __IO uint32_t  HYST_UP_CH4:  2;               /*!< [3..4] Channel 4 upper hysteresis                                     */
           uint32_t             :  3;
      __IO uint32_t  CNT_UP_CH5 :  3;               /*!< [8..10] Upper timer trigger threshold channel 5                       */
      __IO uint32_t  HYST_UP_CH5:  2;               /*!< [11..12] Channel 5 upper hysteresis                                   */
    } bit;                                          /*!< [13] BitSize                                                          */
  } CNT4_5_UPPER;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C0AC) Upper Counter Trigger Level Channel 6 -9               */
    
    struct {
      __I  uint32_t  CNT_UP_CH6 :  3;               /*!< [0..2] Upper timer trigger threshold channel 6                        */
      __I  uint32_t  HYST_UP_CH6:  2;               /*!< [3..4] Channel 6 upper hysteresis                                     */
           uint32_t             :  3;
      __I  uint32_t  CNT_UP_CH7 :  3;               /*!< [8..10] Upper timer trigger threshold channel 7                       */
      __I  uint32_t  HYST_UP_CH7:  2;               /*!< [11..12] Channel 7 upper hysteresis                                   */
           uint32_t             :  3;
      __I  uint32_t  CNT_UP_CH8 :  3;               /*!< [16..18] Upper timer trigger threshold channel 8                      */
      __I  uint32_t  HYST_UP_CH8:  2;               /*!< [19..20] Channel 8 upper hysteresis                                   */
           uint32_t             :  3;
      __I  uint32_t  CNT_UP_CH9 :  3;               /*!< [24..26] Upper timer trigger threshold channel 9                      */
      __I  uint32_t  HYST_UP_CH9:  2;               /*!< [27..28] Channel 9 upper hysteresis                                   */
    } bit;                                          /*!< [29] BitSize                                                          */
  } CNT6_9_UPPER;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801C0B0) Overvoltage Measurement Mode of Ch 0-5                 */
    
    struct {
      __IO uint32_t  Ch0        :  2;               /*!< [0..1] Measurement mode ch 0                                          */
      __IO uint32_t  Ch1        :  2;               /*!< [2..3] Measurement mode ch 1                                          */
      __IO uint32_t  Ch2        :  2;               /*!< [4..5] Measurement mode ch 2                                          */
      __IO uint32_t  Ch3        :  2;               /*!< [6..7] Measurement mode ch 3                                          */
      __IO uint32_t  Ch4        :  2;               /*!< [8..9] Measurement mode ch 4                                          */
      __IO uint32_t  Ch5        :  2;               /*!< [10..11] Measurement mode ch 5                                        */
    } bit;                                          /*!< [12] BitSize                                                          */
  } MMODE0_5;
  __I  uint32_t  RESERVED1[2];
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x4801C0BC) ADC2 HV Status Register                                */
    
    struct {
           uint32_t             :  1;
      __I  uint32_t  READY      :  1;               /*!< [1..1] HVADC Ready bit                                                */
    } bit;                                          /*!< [2] BitSize                                                           */
  } HV_STS;
} ADC2_Type;


/* ================================================================================ */
/* ================                      ADC1                      ================ */
/* ================================================================================ */


/**
  * @brief ADC1 (ADC1)
  */

typedef struct {                                    /*!< (@ 0x40004000) ADC1 Structure                                         */
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40004000) ADC1 Control and Status Register                       */
    
    struct {
      __IO uint32_t  PD_N       :  1;               /*!< [0..0] ADC1 Power Down Signal                                         */
           uint32_t             :  1;
      __IO uint32_t  SOC        :  1;               /*!< [2..2] ADC1 Start of Conversion (software mode)                       */
      __I  uint32_t  EOC        :  1;               /*!< [3..3] ADC1 End of Conversion (software mode)                         */
      __IO uint32_t  IN_MUX_SEL :  3;               /*!< [4..6] Channel for software mode                                      */
    } bit;                                          /*!< [7] BitSize                                                           */
  } CTRL_STS;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40004004) Global Control Register                                */
    
    struct {
      __IO uint32_t  DIVA       :  6;               /*!< [0..5] Divide Factor for the Analog internal clock: 0x00=Fadci
                                                         = Fadc, 0x01=Fadci = Fadc/2, 0x02=Fadci = Fadc/3, 0x02=...,
                                                          0x3F=Fadci = Fadc/64,                                                */
           uint32_t             :  2;
      __IO uint32_t  ANON       :  2;               /*!< [8..9] Analog Part Switched On                                        */
    } bit;                                          /*!< [10] BitSize                                                          */
  } GLOBCTR;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40004008) Channel Settings Bits for Exceptional Interrupt
                                                         Measurement                                                           */
    
    struct {
      __IO uint32_t  CHx        :  3;               /*!< [0..2] Channel set for exceptional interrupt measurement (EIM)        */
           uint32_t             :  1;
      __IO uint32_t  REP        :  3;               /*!< [4..6] Repeat count for exceptional interrupt measurement (EIM)       */
           uint32_t             :  9;
      __IO uint32_t  TRIG_SEL   :  3;               /*!< [16..18] Trigger selection for exceptional interrupt measurement
                                                         (EIM)                                                                 */
    } bit;                                          /*!< [19] BitSize                                                          */
  } CHx_EIM;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4000400C) Channel Settings Bits for Exceptional Sequence
                                                         Measurement                                                           */
    
    struct {
      __IO uint32_t  ESM_0      :  8;               /*!< [0..7] Channel Sequence for Exceptional Sequence Measurement
                                                         (ESM)                                                                 */
           uint32_t             :  8;
      __IO uint32_t  TRIG_SEL   :  3;               /*!< [16..18] Trigger selection for exceptional interrupt measurement
                                                         (ESM)                                                                 */
    } bit;                                          /*!< [19] BitSize                                                          */
  } CHx_ESM;
  __I  uint32_t  RESERVED0[2];
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40004018) Measurement Channel Enable Bits for Cycle 1 -
                                                         4                                                                     */
    
    struct {
      __IO uint32_t  SQ1        :  8;               /*!< [0..7] Sequence 1 channel enable                                      */
      __IO uint32_t  SQ2        :  8;               /*!< [8..15] Sequence 2 channel enable                                     */
      __IO uint32_t  SQ3        :  8;               /*!< [16..23] Sequence 3 channel enable                                    */
      __IO uint32_t  SQ4        :  8;               /*!< [24..31] Sequence 4 channel enable                                    */
    } bit;                                          /*!< [32] BitSize                                                          */
  } SQ1_4;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4000401C) Measurement Channel Enable Bits for Cycle 5 -
                                                         8                                                                     */
    
    struct {
      __IO uint32_t  SQ5        :  8;               /*!< [0..7] Sequence 5 channel enable                                      */
      __IO uint32_t  SQ6        :  8;               /*!< [8..15] Sequence 6 channel enable                                     */
      __IO uint32_t  SQ7        :  8;               /*!< [16..23] Sequence 7 channel enable                                    */
      __IO uint32_t  SQ8        :  8;               /*!< [24..31] Sequence 8 channel enable                                    */
    } bit;                                          /*!< [32] BitSize                                                          */
  } SQ5_8;
  __I  uint32_t  RESERVED1;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40004024) Measurement Channel Data Width Selection               */
    
    struct {
      __IO uint32_t  ch0        :  1;               /*!< [0..0] Data Width channel 0                                           */
      __IO uint32_t  ch1        :  1;               /*!< [1..1] Data Width channel 1                                           */
      __IO uint32_t  ch2        :  1;               /*!< [2..2] Data Width channel 2                                           */
      __IO uint32_t  ch3        :  1;               /*!< [3..3] Data Width channel 3                                           */
      __IO uint32_t  ch4        :  1;               /*!< [4..4] Data Width channel 4                                           */
      __IO uint32_t  ch5        :  1;               /*!< [5..5] Data Width channel 5                                           */
      __IO uint32_t  ch6        :  1;               /*!< [6..6] Data Width channel 6                                           */
      __IO uint32_t  ch7        :  1;               /*!< [7..7] Data Width channel 7                                           */
    } bit;                                          /*!< [8] BitSize                                                           */
  } DWSEL;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40004028) Measurement Channel Sample Time Control 0 - 3          */
    
    struct {
      __IO uint32_t  ch0        :  8;               /*!< [0..7] Sample Time Control for Channel 0                              */
      __IO uint32_t  ch1        :  8;               /*!< [8..15] Sample Time Control for Channel 1                             */
      __IO uint32_t  ch2        :  8;               /*!< [16..23] Sample Time Control for Channel 2                            */
      __IO uint32_t  ch3        :  8;               /*!< [24..31] Sample Time Control for Channel 3                            */
    } bit;                                          /*!< [32] BitSize                                                          */
  } STC_0_3;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4000402C) Measurement Channel Sample Time Control 4 - 7          */
    
    struct {
      __IO uint32_t  ch4        :  8;               /*!< [0..7] Sample Time Control for Channel 4                              */
      __IO uint32_t  ch5        :  8;               /*!< [8..15] Sample Time Control for Channel 5                             */
      __IO uint32_t  ch6        :  8;               /*!< [16..23] Sample Time Control for Channel 6                            */
      __IO uint32_t  ch7        :  8;               /*!< [24..31] Sample Time Control for Channel 7                            */
    } bit;                                          /*!< [32] BitSize                                                          */
  } STC_4_7;
  __I  uint32_t  RESERVED2[4];
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40004040) ADC1 Output Channel EIM                                */
    
    struct {
      __I  uint32_t  OUT_CH_EIM : 12;               /*!< [0..11] ADC1 output result value EIM                                  */
           uint32_t             :  4;
      __IO uint32_t  WFR8       :  1;               /*!< [16..16] Wait for Read Mode                                           */
      __I  uint32_t  VF8        :  1;               /*!< [17..17] Valid Flag                                                   */
      __I  uint32_t  OF8        :  1;               /*!< [18..18] Overrun Flag                                                 */
    } bit;                                          /*!< [19] BitSize                                                          */
  } RES_OUT_EIM;
  __I  uint32_t  RESERVED3[3];
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40004050) Sequencer Feedback Register                            */
    
    struct {
           uint32_t             :  8;
      __IO uint32_t  SQ_RUN     :  1;               /*!< [8..8] ADC1 Sequencer RUN                                             */
      __I  uint32_t  EIM_ACTIVE :  1;               /*!< [9..9] ADC1 EIM active                                                */
      __I  uint32_t  ESM_ACTIVE :  1;               /*!< [10..10] ADC1 ESM active                                              */
      __I  uint32_t  SQx        :  3;               /*!< [11..13] Current Active Sequence in Sequencer Mode                    */
           uint32_t             :  2;
      __I  uint32_t  CHx        :  3;               /*!< [16..18] Current Channel                                              */
    } bit;                                          /*!< [19] BitSize                                                          */
  } SQ_FB;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40004054) ADC1 Output Channel 7                                  */
    
    struct {
      __I  uint32_t  OUT_CH7    : 12;               /*!< [0..11] ADC1 output result value channel 7                            */
           uint32_t             :  4;
      __IO uint32_t  WFR7       :  1;               /*!< [16..16] Wait for Read Mode                                           */
      __I  uint32_t  VF7        :  1;               /*!< [17..17] Valid Flag                                                   */
      __I  uint32_t  OF7        :  1;               /*!< [18..18] Overrun Flag                                                 */
    } bit;                                          /*!< [19] BitSize                                                          */
  } RES_OUT7;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40004058) ADC1 Output Channel 6                                  */
    
    struct {
      __I  uint32_t  OUT_CH6    : 12;               /*!< [0..11] ADC1 output result value channel 6                            */
           uint32_t             :  4;
      __IO uint32_t  WFR6       :  1;               /*!< [16..16] Wait for Read Mode                                           */
      __I  uint32_t  VF6        :  1;               /*!< [17..17] Valid Flag                                                   */
      __I  uint32_t  OF6        :  1;               /*!< [18..18] Overrun Flag                                                 */
    } bit;                                          /*!< [19] BitSize                                                          */
  } RES_OUT6;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4000405C) ADC1 Output Channel 5                                  */
    
    struct {
      __I  uint32_t  OUT_CH5    : 12;               /*!< [0..11] ADC1 output result value channel 5                            */
           uint32_t             :  4;
      __IO uint32_t  WFR5       :  1;               /*!< [16..16] Wait for Read Mode                                           */
      __I  uint32_t  VF5        :  1;               /*!< [17..17] Valid Flag                                                   */
      __I  uint32_t  OF5        :  1;               /*!< [18..18] Overrun Flag                                                 */
    } bit;                                          /*!< [19] BitSize                                                          */
  } RES_OUT5;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40004060) ADC1 Output Channel 4                                  */
    
    struct {
      __I  uint32_t  OUT_CH4    : 12;               /*!< [0..11] ADC1 output result value channel 4                            */
           uint32_t             :  4;
      __IO uint32_t  WFR4       :  1;               /*!< [16..16] Wait for Read Mode                                           */
      __I  uint32_t  VF4        :  1;               /*!< [17..17] Valid Flag                                                   */
      __I  uint32_t  OF4        :  1;               /*!< [18..18] Overrun Flag                                                 */
    } bit;                                          /*!< [19] BitSize                                                          */
  } RES_OUT4;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40004064) ADC1 Output Channel 3                                  */
    
    struct {
      __I  uint32_t  OUT_CH3    : 12;               /*!< [0..11] ADC1 output result value channel 3                            */
           uint32_t             :  4;
      __IO uint32_t  WFR3       :  1;               /*!< [16..16] Wait for Read Mode                                           */
      __I  uint32_t  VF3        :  1;               /*!< [17..17] Valid Flag                                                   */
      __I  uint32_t  OF3        :  1;               /*!< [18..18] Overrun Flag                                                 */
    } bit;                                          /*!< [19] BitSize                                                          */
  } RES_OUT3;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40004068) ADC1 Output Channel 2                                  */
    
    struct {
      __I  uint32_t  OUT_CH2    : 12;               /*!< [0..11] ADC1 output result value channel 2                            */
           uint32_t             :  4;
      __IO uint32_t  WFR2       :  1;               /*!< [16..16] Wait for Read Mode                                           */
      __I  uint32_t  VF2        :  1;               /*!< [17..17] Valid Flag                                                   */
      __I  uint32_t  OF2        :  1;               /*!< [18..18] Overrun Flag                                                 */
    } bit;                                          /*!< [19] BitSize                                                          */
  } RES_OUT2;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4000406C) ADC1 Output Channel 1                                  */
    
    struct {
      __I  uint32_t  OUT_CH1    : 12;               /*!< [0..11] ADC1 output result value channel 1                            */
           uint32_t             :  4;
      __IO uint32_t  WFR1       :  1;               /*!< [16..16] Wait for Read Mode                                           */
      __I  uint32_t  VF1        :  1;               /*!< [17..17] Valid Flag                                                   */
      __I  uint32_t  OF1        :  1;               /*!< [18..18] Overrun Flag                                                 */
    } bit;                                          /*!< [19] BitSize                                                          */
  } RES_OUT1;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40004070) ADC1 Output Channel 0                                  */
    
    struct {
      __I  uint32_t  OUT_CH0    : 12;               /*!< [0..11] ADC1 output reset value channel 0                             */
           uint32_t             :  4;
      __IO uint32_t  WFR0       :  1;               /*!< [16..16] Wait for Read Mode                                           */
      __I  uint32_t  VF0        :  1;               /*!< [17..17] Valid Flag                                                   */
      __I  uint32_t  OF0        :  1;               /*!< [18..18] Overrun Flag                                                 */
    } bit;                                          /*!< [19] BitSize                                                          */
  } RES_OUT0;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x40004074) Global Status Register                                 */
    
    struct {
      __I  uint32_t  BUSY       :  1;               /*!< [0..0] Analog Part Busy                                               */
      __I  uint32_t  SAMPLE     :  1;               /*!< [1..1] Sample Phase Indication                                        */
           uint32_t             :  1;
      __I  uint32_t  CHNR       :  3;               /*!< [3..5] Channel Number                                                 */
           uint32_t             :  2;
      __I  uint32_t  ANON_ST    :  2;               /*!< [8..9] Analog Part Switched On                                        */
    } bit;                                          /*!< [10] BitSize                                                          */
  } GLOBSTR;
  
  union {
    __I  uint32_t  reg;                             /*!< (@ 0x40004078) ADC1 Interrupt Status Register                         */
    
    struct {
      __I  uint32_t  CH0_STS    :  1;               /*!< [0..0] ADC1 Channel 0 Interrupt Status                                */
      __I  uint32_t  CH1_STS    :  1;               /*!< [1..1] ADC1 Channel 1 Interrupt Status                                */
      __I  uint32_t  CH2_STS    :  1;               /*!< [2..2] ADC1 Channel 2 Interrupt Status                                */
      __I  uint32_t  CH3_STS    :  1;               /*!< [3..3] ADC1 Channel 3 Interrupt Status                                */
      __I  uint32_t  CH4_STS    :  1;               /*!< [4..4] ADC1 Channel 4 Interrupt Status                                */
      __I  uint32_t  CH5_STS    :  1;               /*!< [5..5] ADC1 Channel 5 Interrupt Status                                */
      __I  uint32_t  CH6_STS    :  1;               /*!< [6..6] ADC1 Channel 6 Interrupt Status                                */
      __I  uint32_t  CH7_STS    :  1;               /*!< [7..7] ADC1 Channel 7 Interrupt Status                                */
      __I  uint32_t  EIM_STS    :  1;               /*!< [8..8] Exceptional Interrupt Measurement (EIM) Status                 */
      __I  uint32_t  ESM_STS    :  1;               /*!< [9..9] Exceptional Sequence Measurement (ESM) Status                  */
    } bit;                                          /*!< [10] BitSize                                                          */
  } IS;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4000407C) ADC1 Interrupt Enable Register                         */
    
    struct {
      __IO uint32_t  CH0_IE     :  1;               /*!< [0..0] ADC1 Channel 0 Interrupt Enable                                */
      __IO uint32_t  CH1_IE     :  1;               /*!< [1..1] ADC1 Channel 1 Interrupt Enable                                */
      __IO uint32_t  CH2_IE     :  1;               /*!< [2..2] ADC1 Channel 2 Interrupt Enable                                */
      __IO uint32_t  CH3_IE     :  1;               /*!< [3..3] ADC1 Channel 3 Interrupt Enable                                */
      __IO uint32_t  CH4_IE     :  1;               /*!< [4..4] ADC1 Channel 4 Interrupt Enable                                */
      __IO uint32_t  CH5_IE     :  1;               /*!< [5..5] ADC1 Channel 5 Interrupt Enable                                */
      __IO uint32_t  CH6_IE     :  1;               /*!< [6..6] ADC1 Channel 6 Interrupt Enable                                */
      __IO uint32_t  CH7_IE     :  1;               /*!< [7..7] ADC1 Channel 7 Interrupt Enable                                */
      __IO uint32_t  EIM_IE     :  1;               /*!< [8..8] Exceptional Interrupt Measurement (EIM) Interrupt Enable       */
      __IO uint32_t  ESM_IE     :  1;               /*!< [9..9] Exceptional Sequence Measurement (ESM) Interrupt Enable        */
    } bit;                                          /*!< [10] BitSize                                                          */
  } IE;
  
  union {
    __O  uint32_t  reg;                             /*!< (@ 0x40004080) ADC1 Interrupt Status Clear Register                   */
    
    struct {
      __O  uint32_t  CH0_ICLR   :  1;               /*!< [0..0] ADC1 Channel 0 Interrupt Status Clear                          */
      __O  uint32_t  CH1_ICLR   :  1;               /*!< [1..1] ADC1 Channel 1 Interrupt Status Clear                          */
      __O  uint32_t  CH2_ICLR   :  1;               /*!< [2..2] ADC1 Channel 2 Interrupt Status Clear                          */
      __O  uint32_t  CH3_ICLR   :  1;               /*!< [3..3] ADC1 Channel 3 Interrupt Status Clear                          */
      __O  uint32_t  CH4_ICLR   :  1;               /*!< [4..4] ADC1 Channel 4 Interrupt Status Clear                          */
      __O  uint32_t  CH5_ICLR   :  1;               /*!< [5..5] ADC1 Channel 5 Interrupt Status Clear                          */
      __O  uint32_t  CH6_ICLR   :  1;               /*!< [6..6] ADC1 Channel 6 Interrupt Status Clear                          */
      __O  uint32_t  CH7_ICLR   :  1;               /*!< [7..7] ADC1 Channel 7 Interrupt Status Clear                          */
      __O  uint32_t  EIM_ICLR   :  1;               /*!< [8..8] Exceptional Interrupt Measurement (EIM) Status Clear           */
      __O  uint32_t  ESM_ICLR   :  1;               /*!< [9..9] Exceptional Sequence Measurement (ESM) Status Clear            */
    } bit;                                          /*!< [10] BitSize                                                          */
  } ICLR;
} ADC1_Type;


/* ================================================================================ */
/* ================                      BDRV                      ================ */
/* ================================================================================ */


/**
  * @brief BDRV (BDRV)
  */

typedef struct {                                    /*!< (@ 0x40034000) BDRV Structure                                         */
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40034000) H-Bridge Driver Control 1                              */
    
    struct {
      __IO uint32_t  LS1_EN     :  1;               /*!< [0..0] Low Side Driver 1 Enable                                       */
      __IO uint32_t  LS1_PWM    :  1;               /*!< [1..1] Low Side Driver 1 PWM Enable                                   */
      __IO uint32_t  LS1_ON     :  1;               /*!< [2..2] Low Side Driver 1 On                                           */
      __IO uint32_t  LS1_DCS_EN :  1;               /*!< [3..3] Low Side Driver 1 Diagnosis Current Source Enable              */
      __I  uint32_t  LS1_DS_STS :  1;               /*!< [4..4] Low Side Driver 1 Drain Source Monitoring Status in OFF-State  */
      __I  uint32_t  LS1_SUPERR_STS:  1;            /*!< [5..5] Low Side Driver 1 Supply Error Status                          */
      __I  uint32_t  LS1_OC_STS :  1;               /*!< [6..6] External Low Side 1 FET Over-current Status                    */
      __IO uint32_t  LS1_OC_DIS :  1;               /*!< [7..7] Low Side Driver 1 Overcurrent Shutdown Disable                 */
      __IO uint32_t  LS2_EN     :  1;               /*!< [8..8] Low Side Driver 2 Enable                                       */
      __IO uint32_t  LS2_PWM    :  1;               /*!< [9..9] Low Side Driver 2 PWM Enable                                   */
      __IO uint32_t  LS2_ON     :  1;               /*!< [10..10] Low Side Driver 2 On                                         */
      __IO uint32_t  LS2_DCS_EN :  1;               /*!< [11..11] Low Side Driver 2 Diagnosis Current Source Enable            */
      __I  uint32_t  LS2_DS_STS :  1;               /*!< [12..12] Low Side Driver 2 Drain Source Monitoring Status in
                                                         OFF-State                                                             */
      __I  uint32_t  LS2_SUPERR_STS:  1;            /*!< [13..13] Low Side Driver 2 Supply Error Status                        */
      __I  uint32_t  LS2_OC_STS :  1;               /*!< [14..14] External Low Side 2 FET Over-current Status                  */
      __IO uint32_t  LS2_OC_DIS :  1;               /*!< [15..15] Low Side Driver Overcurrent Shutdown Disable                 */
      __IO uint32_t  HS1_EN     :  1;               /*!< [16..16] High Side Driver 1 Enable                                    */
      __IO uint32_t  HS1_PWM    :  1;               /*!< [17..17] High Side Driver 1 PWM Enable                                */
      __IO uint32_t  HS1_ON     :  1;               /*!< [18..18] High Side Driver 1 On                                        */
      __IO uint32_t  HS1_DCS_EN :  1;               /*!< [19..19] High Side Driver 1 Diagnosis Current Source Enable           */
      __I  uint32_t  HS1_DS_STS :  1;               /*!< [20..20] High Side Driver 1 Drain Source Monitoring Status in
                                                         OFF-State                                                             */
      __I  uint32_t  HS1_SUPERR_STS:  1;            /*!< [21..21] High Side Driver 1 Supply Error Status                       */
      __I  uint32_t  HS1_OC_STS :  1;               /*!< [22..22] External High Side 1 FET Over-current Status                 */
      __IO uint32_t  HS1_OC_DIS :  1;               /*!< [23..23] High Side Driver Overcurrent Shutdown Disable                */
      __IO uint32_t  HS2_EN     :  1;               /*!< [24..24] High Side Driver 2 Enable                                    */
      __IO uint32_t  HS2_PWM    :  1;               /*!< [25..25] High Side Driver 2 PWM Enable                                */
      __IO uint32_t  HS2_ON     :  1;               /*!< [26..26] High Side Driver 2 On                                        */
      __IO uint32_t  HS2_DCS_EN :  1;               /*!< [27..27] High Side Driver 2 Diagnosis Current Source Enable           */
      __I  uint32_t  HS2_DS_STS :  1;               /*!< [28..28] High Side Driver 2 Drain Source Monitoring Status in
                                                         OFF-State                                                             */
      __I  uint32_t  HS2_SUPERR_STS:  1;            /*!< [29..29] High Side Driver 2 Supply Error Status                       */
      __I  uint32_t  HS2_OC_STS :  1;               /*!< [30..30] External High Side 2 FET Over-current Status                 */
      __IO uint32_t  HS2_OC_DIS :  1;               /*!< [31..31] High Side Driver Overcurrent Shutdown Disable                */
    } bit;                                          /*!< [32] BitSize                                                          */
  } CTRL1;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40034004) H-Bridge Driver Control 2                              */
    
    struct {
           uint32_t             : 16;
      __I  uint32_t  DLY_DIAG_TIM: 10;              /*!< [16..25] Ext. power on/off diag timer result register                 */
      __O  uint32_t  DLY_DIAG_SCLR:  1;             /*!< [26..26] Ext. power diag timer valid flag clear                       */
      __I  uint32_t  DLY_DIAG_STS:  1;              /*!< [27..27] Ext. power diag timer valid flag                             */
      __IO uint32_t  DLY_DIAG_CHSEL:  3;            /*!< [28..30] Ext. power on/off timer channel select                       */
      __IO uint32_t  DLY_DIAG_DIRSEL:  1;           /*!< [31..31] Ext. power diag timer on / off select                        */
    } bit;                                          /*!< [32] BitSize                                                          */
  } CTRL2;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40034008) H-Bridge Driver Control 3                              */
    
    struct {
      __IO uint32_t  ICHARGE_TRIM:  5;              /*!< [0..4] Trimming of the internal driver charge current                 */
           uint32_t             :  1;
      __IO uint32_t  ICHARGEDIV2_N:  1;             /*!< [6..6] ICHARGE Current divide by 2 not                                */
      __IO uint32_t  ON_SEQ_EN  :  1;               /*!< [7..7] Turn On SlewrateSequencer enable                               */
      __IO uint32_t  IDISCHARGE_TRIM:  5;           /*!< [8..12] Trimming of the internal driver dis-charge current            */
           uint32_t             :  1;
      __IO uint32_t  IDISCHARGEDIV2_N:  1;          /*!< [14..14] IDISCHARGE Current divide by 2 not                           */
      __IO uint32_t  OFF_SEQ_EN :  1;               /*!< [15..15] Turn Off Slewrate Sequencer enable                           */
      __IO uint32_t  DSMONVTH   :  3;               /*!< [16..18] Voltage Threshold for Drain-Source Monitoring of external
                                                         FETs                                                                  */
           uint32_t             :  5;
      __IO uint32_t  DRV_CCP_TIMSEL:  2;            /*!< [24..25] minimum cross conduction protection time setting             */
      __IO uint32_t  DRV_CCP_DIS:  1;               /*!< [26..26] Dynamic cross conduction protection Disable                  */
           uint32_t             :  3;
      __O  uint32_t  SEQ_ERR_SCLR:  1;              /*!< [30..30] Driver Sequence Error Status Clear                           */
      __I  uint32_t  SEQ_ERR_STS:  1;               /*!< [31..31] Driver Sequence Error Status                                 */
    } bit;                                          /*!< [32] BitSize                                                          */
  } CTRL3;
  __I  uint32_t  RESERVED0;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40034010) Turn on Slewrate Sequencer Control                     */
    
    struct {
      __IO uint32_t  DRV_OFF_t_4:  3;               /*!< [0..2] Slew rate sequencer off phase 4 time                           */
      __IO uint32_t  DRV_OFF_I_4:  5;               /*!< [3..7] Slew rate sequencer off phase 4 current                        */
      __IO uint32_t  DRV_OFF_t_3:  3;               /*!< [8..10] Slew rate sequencer off phase 3 time                          */
      __IO uint32_t  DRV_OFF_I_3:  5;               /*!< [11..15] Slew rate sequencer off phase 3 current                      */
      __IO uint32_t  DRV_OFF_t_2:  3;               /*!< [16..18] Slew rate sequencer off phase 2 time                         */
      __IO uint32_t  DRV_OFF_I_2:  5;               /*!< [19..23] Slew rate sequencer off phase 2 current                      */
      __IO uint32_t  DRV_OFF_t_1:  3;               /*!< [24..26] Slew rate sequencer off phase 1 time                         */
      __IO uint32_t  DRV_OFF_I_1:  5;               /*!< [27..31] Slew rate sequencer off phase 1 current                      */
    } bit;                                          /*!< [32] BitSize                                                          */
  } OFF_SEQ_CTRL;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40034014) Turn off Slewrate Sequencer Control                    */
    
    struct {
      __IO uint32_t  DRV_ON_t_4 :  3;               /*!< [0..2] Slew rate sequencer on phase 4 time                            */
      __IO uint32_t  DRV_ON_I_4 :  5;               /*!< [3..7] Slew rate sequencer on phase 4 current                         */
      __IO uint32_t  DRV_ON_t_3 :  3;               /*!< [8..10] Slew rate sequencer on phase 3 time                           */
      __IO uint32_t  DRV_ON_I_3 :  5;               /*!< [11..15] Slew rate sequencer on phase 3 current                       */
      __IO uint32_t  DRV_ON_t_2 :  3;               /*!< [16..18] Slew rate sequencer on phase 2 time                          */
      __IO uint32_t  DRV_ON_I_2 :  5;               /*!< [19..23] Slew rate sequencer on phase 2 current                       */
      __IO uint32_t  DRV_ON_t_1 :  3;               /*!< [24..26] Slew rate sequencer on phase 1 time                          */
      __IO uint32_t  DRV_ON_I_1 :  5;               /*!< [27..31] Slew rate sequencer on phase 1 current                       */
    } bit;                                          /*!< [32] BitSize                                                          */
  } ON_SEQ_CTRL;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40034018) Trimming of Driver                                     */
    
    struct {
      __IO uint32_t  LS_HS_BT_TFILT_SEL:  2;        /*!< [0..1] Blanking Time for Drain-Source Monitoring of Low / High
                                                         Side Drivers                                                          */
           uint32_t             :  6;
      __IO uint32_t  LSDRV_DS_TFILT_SEL:  2;        /*!< [8..9] Filter Time for Drain-Source Monitoring of Low Side Drivers    */
      __IO uint32_t  LS1DRV_FDISCHG_DIS:  1;        /*!< [10..10] Low Side 1 Predriver in overcurrent situation disable        */
      __IO uint32_t  LS2DRV_FDISCHG_DIS:  1;        /*!< [11..11] Low Side 2 Predriver in overcurrent situation disable        */
           uint32_t             :  1;
      __IO uint32_t  LS1DRV_OCSDN_DIS:  1;          /*!< [13..13] Low Side 1 Predriver in overcurrent situation disable        */
      __IO uint32_t  LS2DRV_OCSDN_DIS:  1;          /*!< [14..14] Low Side 2 Predriver in overcurrent situation disable        */
           uint32_t             :  1;
      __IO uint32_t  HSDRV_DS_TFILT_SEL:  2;        /*!< [16..17] Filter Time for Drain-Source Monitoring of High Side
                                                         Drivers                                                               */
      __IO uint32_t  HS1DRV_FDISCHG_DIS:  1;        /*!< [18..18] High Side 1 Predriver in overcurrent situation disable       */
      __IO uint32_t  HS2DRV_FDISCHG_DIS:  1;        /*!< [19..19] High Side 2 Predriver in overcurrent situation disable       */
           uint32_t             :  1;
      __IO uint32_t  HS1DRV_OCSDN_DIS:  1;          /*!< [21..21] High Side 1 Predriver in overcurrent situation disable       */
      __IO uint32_t  HS2DRV_OCSDN_DIS:  1;          /*!< [22..22] High Side 2 Predriver in overcurrent situation disable       */
           uint32_t             :  1;
      __IO uint32_t  CPLOW_TFILT_SEL:  2;           /*!< [24..25] Filter Time for Charge Pump Voltage Low Diagnosis            */
    } bit;                                          /*!< [26] BitSize                                                          */
  } TRIM_DRVx;
  __I  uint32_t  RESERVED1;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40034020) Charge Pump Control and Status Register                */
    
    struct {
      __IO uint32_t  CP_EN      :  1;               /*!< [0..0] Charge Pump Enable                                             */
           uint32_t             :  1;
      __IO uint32_t  CP_RDY_EN  :  1;               /*!< [2..2] Bridge Driver on Charge Pump Ready Enable                      */
           uint32_t             :  2;
      __I  uint32_t  VCP_LOTH2_STS:  1;             /*!< [5..5] Charge Pump Low Status                                         */
           uint32_t             :  2;
      __IO uint32_t  VCP_LOWTH2 :  3;               /*!< [8..10] Charge Pump Output Voltage Lower Threshold Detection
                                                         Level                                                                 */
           uint32_t             :  5;
      __IO uint32_t  DRVx_VCPLO_DIS:  1;            /*!< [16..16] Driver On Charge Pump Low Voltage Disable                    */
      __I  uint32_t  VCP_LOTH1_STS:  1;             /*!< [17..17] Charge Pump MU Low Status                                    */
      __IO uint32_t  DRVx_VCPUP_DIS:  1;            /*!< [18..18] Driver On Charge Pump Upper Voltage Disable                  */
      __I  uint32_t  VCP_UPTH_STS:  1;              /*!< [19..19] Charge Pump MU High Status                                   */
      __IO uint32_t  DRVx_VSDLO_DIS:  1;            /*!< [20..20] Driver On VSD Lower Voltage Disable                          */
      __I  uint32_t  VSD_LOTH_STS:  1;              /*!< [21..21] Driver Supply MU Low Status                                  */
      __IO uint32_t  DRVx_VSDUP_DIS:  1;            /*!< [22..22] Driver On VSD Upper Voltage Disable                          */
      __I  uint32_t  VSD_UPTH_STS:  1;              /*!< [23..23] Driver Supply MU High Status                                 */
      __IO uint32_t  CPLOPWRM_EN:  1;               /*!< [24..24] Charge Pump Low Power Mode Enable                            */
      __IO uint32_t  VCP9V_SET  :  1;               /*!< [25..25] Charge Pump 9 V Output Voltage Set                           */
      __IO uint32_t  VTHVCP9V_TRIM:  2;             /*!< [26..27] Charge Pump Output Voltage 9V Trimming                       */
    } bit;                                          /*!< [28] BitSize                                                          */
  } CP_CTRL_STS;
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x40034024) Charge Pump Clock Control Register                     */
    
    struct {
      __IO uint32_t  DITH_LOWER :  5;               /*!< [0..4] CP_CLK lower frequency boundary during dithering               */
           uint32_t             :  3;
      __IO uint32_t  DITH_UPPER :  5;               /*!< [8..12] CP_CLK upper frequency boundary during dithering              */
      __IO uint32_t  F_CP       :  2;               /*!< [13..14] MSB of CP_CLK divider                                        */
      __IO uint32_t  CPCLK_EN   :  1;               /*!< [15..15] Charge Pump Clock Enable                                     */
    } bit;                                          /*!< [16] BitSize                                                          */
  } CP_CLK_CTRL;
} BDRV_Type;


/* ================================================================================ */
/* ================                       MON                      ================ */
/* ================================================================================ */


/**
  * @brief MON (MON)
  */

typedef struct {                                    /*!< (@ 0x50004000) MON Structure                                          */
  __I  uint32_t  RESERVED0[13];
  
  union {
    __IO uint8_t   reg;                             /*!< (@ 0x50004034) Settings Monitor 1                                     */
    
    struct {
      __IO uint8_t   EN         :  1;               /*!< [0..0] MON Enable                                                     */
      __IO uint8_t   FALL       :  1;               /*!< [1..1] MON Wake-up on Falling Edge Enable                             */
      __IO uint8_t   RISE       :  1;               /*!< [2..2] MON Wake-up on Rising Edge Enable                              */
      __IO uint8_t   CYC        :  1;               /*!< [3..3] MON for Cycle Sense Enable                                     */
      __IO uint8_t   PD         :  1;               /*!< [4..4] Pull-Down Current Source for MON Input Enable                  */
      __IO uint8_t   PU         :  1;               /*!< [5..5] Pull-Up Current Source for MON Input Enable                    */
           uint8_t              :  1;
      __I  uint8_t   STS        :  1;               /*!< [7..7] MON Status Input                                               */
    } bit;                                          /*!< [8] BitSize                                                           */
  } CNF;
} MON_Type;


/* ================================================================================ */
/* ================                       CSA                      ================ */
/* ================================================================================ */


/**
  * @brief CSA (CSA)
  */

typedef struct {                                    /*!< (@ 0x48018000) CSA Structure                                          */
  __I  uint32_t  RESERVED0[3];
  
  union {
    __IO uint32_t  reg;                             /*!< (@ 0x4801800C) Current Sense Amplifier Control Register               */
    
    struct {
      __IO uint32_t  EN         :  1;               /*!< [0..0] OPA enable                                                     */
      __IO uint32_t  GAIN       :  4;               /*!< [1..4] OPA gain setting                                               */
           uint32_t             :  3;
      __IO uint32_t  VZERO      :  1;               /*!< [8..8] Current Sense Output Voltage Level to Ground                   */
    } bit;                                          /*!< [9] BitSize                                                           */
  } CTRL;
} CSA_Type;


/* --------------------  End of section using anonymous unions  ------------------- */
#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)
  /* leave anonymous unions enabled */
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  /* TBD. pragmas for anonymous unions */
#else
  #warning Not supported compiler type
#endif



/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */

#define SCU_BASE                        0x50005000UL
#define PORT_BASE                       0x48028000UL
#define GPT12E_BASE                     0x40010000UL
#define TIMER2_BASE                     0x48004000UL
#define TIMER21_BASE                    0x48005000UL
#define CCU6_BASE                       0x4000C000UL
#define UART1_BASE                      0x48020000UL
#define UART2_BASE                      0x48022000UL
#define SSC1_BASE                       0x48024000UL
#define SSC2_BASE                       0x48026000UL
#define PMU_BASE                        0x50004000UL
#define SCUPM_BASE                      0x50006000UL
#define CPU_BASE                        0xE000E000UL
#define DMA_BASE                        0x50014000UL
#define TIMER3_BASE                     0x48006000UL
#define LIN_BASE                        0x4801E000UL
#define MF_BASE                         0x48018000UL
#define ADC2_BASE                       0x4801C000UL
#define ADC1_BASE                       0x40004000UL
#define BDRV_BASE                       0x40034000UL
#define MON_BASE                        0x50004000UL
#define CSA_BASE                        0x48018000UL


/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */

#define SCU                             ((SCU_Type                *) SCU_BASE)
#define PORT                            ((PORT_Type               *) PORT_BASE)
#define GPT12E                          ((GPT12E_Type             *) GPT12E_BASE)
#define TIMER2                          ((TIMER2x_Type            *) TIMER2_BASE)
#define TIMER21                         ((TIMER2x_Type            *) TIMER21_BASE)
#define CCU6                            ((CCU6_Type               *) CCU6_BASE)
#define UART1                           ((UART_Type               *) UART1_BASE)
#define UART2                           ((UART_Type               *) UART2_BASE)
#define SSC1                            ((SSC_Type                *) SSC1_BASE)
#define SSC2                            ((SSC_Type                *) SSC2_BASE)
#define PMU                             ((PMU_Type                *) PMU_BASE)
#define SCUPM                           ((SCUPM_Type              *) SCUPM_BASE)
#define CPU                             ((CPU_Type                *) CPU_BASE)
#define DMA                             ((DMA_Type                *) DMA_BASE)
#define TIMER3                          ((TIMER3_Type             *) TIMER3_BASE)
#define LIN                             ((LIN_Type                *) LIN_BASE)
#define MF                              ((MF_Type                 *) MF_BASE)
#define ADC2                            ((ADC2_Type               *) ADC2_BASE)
#define ADC1                            ((ADC1_Type               *) ADC1_BASE)
#define BDRV                            ((BDRV_Type               *) BDRV_BASE)
#define MON                             ((MON_Type                *) MON_BASE)
#define CSA                             ((CSA_Type                *) CSA_BASE)


/** @} */ /* End of group Device_Peripheral_Registers */
/** @} */ /* End of group TLE986x */
/** @} */ /* End of group (null) */

#ifdef __cplusplus
}
#endif


#endif  /* TLE986x_H */

