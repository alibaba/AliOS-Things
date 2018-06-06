/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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


#ifndef _SYS_CTRL_H_
#define _SYS_CTRL_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'sys_ctrl'."
#endif


#include "globals.h"

// =============================================================================
//  MACROS
// =============================================================================

// ============================================================================
// CPU_ID_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
/// CPU IDs
    XCPU                                        = 0x00000000,
    BCPU                                        = 0x00000001
} CPU_ID_T;


// ============================================================================
// SYS_CLKS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
/// System side System clocks
    SYS_XCPU                                    = 0x00000000,
    SYS_XCPU_INT                                = 0x00000001,
    SYS_PCLK_CONF                               = 0x00000002,
    SYS_PCLK_DATA                               = 0x00000003,
    SYS_AMBA                                    = 0x00000004,
    SYS_DMA                                     = 0x00000005,
    SYS_GOUDA                                   = 0x00000006,
    SYS_EBC                                     = 0x00000007,
    SYS_IFC_CH0                                 = 0x00000008,
    SYS_IFC_CH1                                 = 0x00000009,
    SYS_IFC_CH2                                 = 0x0000000A,
    SYS_IFC_CH3                                 = 0x0000000B,
    SYS_IFC_CH4                                 = 0x0000000C,
    SYS_IFC_CH5                                 = 0x0000000D,
    SYS_IFC_CH6                                 = 0x0000000E,
    SYS_IFC_CH7                                 = 0x0000000F,
    SYS_IFC_DBG                                 = 0x00000010,
    SYS_A2A                                     = 0x00000011,
    SYS_INTSRAM                                 = 0x00000012,
    SYS_DEBUG_UART                              = 0x00000013,
    SYS_DBGHST                                  = 0x00000014,
    SYS_VOC_AHB                                 = 0x00000015,
    SYS_MAILBOX                                 = 0x00000016,
    SYS_IFC2_CH0                                = 0x00000017,
    SYS_IFC2_CH1                                = 0x00000018,
    SYS_AIF                                     = 0x00000019,
/// System side divided clock (either divided by module or by sys_ctrl)
    SYSD_SCI                                    = 0x0000001A,
    SYSD_RF_SPI                                 = 0x0000001B,
    SYSD_OSC                                    = 0x0000001C,
    SYSD_PWM                                    = 0x0000001D,
/// divided BCK clock for DAI
    SYSD_DAIS_208K                              = 0x0000001E,
/// the following don't have an auto enable
    SYS_GPIO                                    = 0x0000001F,
    SYS_IRQ                                     = 0x00000020,
    SYS_TCU                                     = 0x00000021,
    SYS_TIMER                                   = 0x00000022,
/// the following are sharing their enable
    SYS_SCI                                     = 0x00000023,
/// keep last
    SYS_NOGATE                                  = 0x00000024
} SYS_CLKS_T;

#define NB_SYS_CLK_XCPU                         (2)
#define NB_SYS_CLK_AEN                          (31)
#define SYSCLK_PAGE_1                           (31)
#define NB_SYS_CLK_EN                           (35)
#define NB_SYS_CLK                              (37)

// ============================================================================
// PER_CLKS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
/// System side System clocks for Peripherals
    PER_UART                                    = 0x00000000,
    PER_UART2                                   = 0x00000001,
    PER_I2C                                     = 0x00000002,
    PER_I2C2                                    = 0x00000003,
    PER_I2C3                                    = 0x00000004,
    PER_SDMMC                                   = 0x00000005,
    PER_SDMMC2                                  = 0x00000006,
    PER_CAMERA                                  = 0x00000007,
/// System side divided clock (either divided by module or by sys_ctrl)
    PERD_SPI1                                   = 0x00000008,
    PERD_SPI2                                   = 0x00000009,
    PERD_SPI3                                   = 0x0000000A,
    PER_USBC                                    = 0x0000000B,
    PER_SPY                                     = 0x0000000C,
    PER_TEST                                    = 0x0000000D
} PER_CLKS_T;

#define NB_PER_CLK_AEN                          (11)
#define NB_PER_CLK_EN                           (14)
#define NB_PER_CLK                              (14)

// ============================================================================
// BB_CLKS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
/// Baseband side System clocks
    BB_BCPU                                     = 0x00000000,
    BB_BCPU_INT                                 = 0x00000001,
    BB_AMBA                                     = 0x00000002,
    BB_PCLK_CONF                                = 0x00000003,
    BB_PCLK_DATA                                = 0x00000004,
    BB_ROM_AHB                                  = 0x00000005,
    BB_MAILBOX                                  = 0x00000006,
    BB_EXCOR                                    = 0x00000007,
    BB_IFC_CH2                                  = 0x00000008,
    BB_IFC_CH3                                  = 0x00000009,
    BB_SRAM                                     = 0x0000000A,
    BB_A2A                                      = 0x0000000B,
    BB_ITLV                                     = 0x0000000C,
    BB_VITERBI                                  = 0x0000000D,
    BB_CIPHER                                   = 0x0000000E,
    BB_RF_IF                                    = 0x0000000F,
    BB_COPRO                                    = 0x00000010,
    BB_CP2_REG                                  = 0x00000011,
    BB_XCOR                                     = 0x00000012,
    BB_EVITAC                                   = 0x00000013,
    BB_CORDIC                                   = 0x00000014,
    BB_COM_REGS                                 = 0x00000015,
    BB2_SDMMC                                   = 0x00000016,
    BB_CHOLK                                    = 0x00000017,
/// the following don't have an auto enable
    BB_IRQ                                      = 0x00000018,
    BB_A53                                      = 0x00000019,
    BB_TIMER                                    = 0x0000001A,
    BBD_OSC                                     = 0x0000001B
} BB_CLKS_T;

#define NB_BB_CLK_AEN                           (24)
#define NB_BB_CLK_EN                            (28)
#define NB_BB_CLK                               (28)
/// Other clocks
/// clocks with auto enble
/// the debug host clock auto enable is not used in host mode, only in uart mode
#define OC_HOST_UART                            (0)

// ============================================================================
// OTHER_CLKS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    OC_DEBUG_UART                               = 0x00000000,
    OC_BCK                                      = 0x00000001,
    OC_UART                                     = 0x00000002,
    OC_UART2                                    = 0x00000003,
    OC_RF_RX                                    = 0x00000004,
    OC_RF_TX                                    = 0x00000005,
    OC_MEM_BRIDGE                               = 0x00000006,
    OC_VOC_CORE                                 = 0x00000007,
    OC_VOC_AHB                                  = 0x00000008,
    OC_VOC_DAI                                  = 0x00000009,
    OC_VOC                                      = 0x0000000A,
    OC_LPS                                      = 0x0000000B,
    OC_GPIO                                     = 0x0000000C,
    OC_USBPHY                                   = 0x0000000D,
    OC_PIX                                      = 0x0000000E,
    OC_RESERVE                                  = 0x0000000F,
    OC_MEM_CLK_OUT                              = 0x00000010,
    OC_DBG                                      = 0x00000011,
    OC_TCU                                      = 0x00000012,
    OC_SYS_SPIFLASH                             = 0x00000013,
    OC_SPIFLASH                                 = 0x00000014
} OTHER_CLKS_T;

#define NB_OTHER_CLK_AEN                        (10)
#define NB_OTHER_CLK_EN                         (21)
#define NB_OTHER_CLK                            (21)

// ============================================================================
// RESETS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
/// System side resets
    RST_XCPU                                    = 0x00000000,
    RST_SYS_IRQ                                 = 0x00000001,
    RST_SYS_A2A                                 = 0x00000002,
    RST_ABB                                     = 0x00000003,
    RST_DMA                                     = 0x00000004,
    RST_GOUDA                                   = 0x00000005,
    RST_TIMER                                   = 0x00000006,
    RST_TCU                                     = 0x00000007,
    RST_GPIO                                    = 0x00000008,
    RST_KEYPAD                                  = 0x00000009,
    RST_PWM                                     = 0x0000000A,
    RST_UART                                    = 0x0000000B,
    RST_UART2                                   = 0x0000000C,
    RST_SPI1                                    = 0x0000000D,
    RST_SPI2                                    = 0x0000000E,
    RST_SPI3                                    = 0x0000000F,
    RST_RF_SPI                                  = 0x00000010,
    RST_SCI                                     = 0x00000011,
    RST_SPY                                     = 0x00000012,
    RST_I2C                                     = 0x00000013,
    RST_I2C2                                    = 0x00000014,
    RST_I2C3                                    = 0x00000015,
    RST_MEM_BRIDGE                              = 0x00000016,
    RST_SPIFLASH                                = 0x00000017,
    RST_EXT_APB                                 = 0x00000018,
    RST_SDMMC                                   = 0x00000019,
    RST_CAMERA                                  = 0x0000001A,
    RST_USBC                                    = 0x0000001B,
    RST_USBPHY                                  = 0x0000001C,
    RST_DBG                                     = 0x0000001D,
    RST_VOC                                     = 0x0000001E,
    RST_AIF                                     = 0x0000001F,
    RST_IFC2                                    = 0x00000020,
    RST_MAILBOX                                 = 0x00000021,
    RST_SDMMC2                                  = 0x00000022,
    RST_DAI                                     = 0x00000023,
    RST_BCPU                                    = 0x00000024,
    RST_BB_IRQ                                  = 0x00000025,
    RST_BB_A2A                                  = 0x00000026,
    RST_BB_IFC                                  = 0x00000027,
    RST_BB_SRAM                                 = 0x00000028,
    RST_ITLV                                    = 0x00000029,
    RST_VITERBI                                 = 0x0000002A,
    RST_CIPHER                                  = 0x0000002B,
    RST_XCOR                                    = 0x0000002C,
    RST_COPRO                                   = 0x0000002D,
    RST_RF_IF                                   = 0x0000002E,
    RST_ROM_BB                                  = 0x0000002F,
    RST_MAILBOX_BB                              = 0x00000030,
    RST_EXCOR                                   = 0x00000031,
    RST_EVITAC                                  = 0x00000032,
    RST_CORDIC                                  = 0x00000033,
    RST_TCU_BB                                  = 0x00000034,
    RST_BB2_SDMMC                               = 0x00000035,
    RST_CHOLK                                   = 0x00000036,
    RST_A53                                     = 0x00000037,
    RST_BB_TIMER                                = 0x00000038,
    RST_BB_FULL                                 = 0x00000039,
    RST_SYS_FULL                                = 0x0000003A
} RESETS_T;

/// Next Register
#define RST_PAGE_1                              (30)
#define NB_SRST                                 (36)
/// Baseband side resets
#define BOUND_BRST_FIRST                        (36)
#define BOUND_BRST_AFTER                        (57)
/// The following reset does not have register
#define NR_RST_REG                              (58)
#define NB_RST                                  (59)
#define NB_BRST                                 (BOUND_BRST_AFTER-BOUND_BRST_FIRST)

// ============================================================================
// RESET_OTHERS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
/// Reset Other : resync on corresponding clock other
    RSTO_DBG_HOST                               = 0x00000000,
    RSTO_GPADC                                  = 0x00000001,
    RSTO_BCK                                    = 0x00000002,
    RSTO_UART                                   = 0x00000003,
    RSTO_UART2                                  = 0x00000004,
    RSTO_RF_RX                                  = 0x00000005,
    RSTO_RF_TX                                  = 0x00000006,
    RSTO_MEM_BRIDGE                             = 0x00000007,
    RSTO_DBG                                    = 0x00000008,
    RSTO_VOC_DAI                                = 0x00000009,
    RSTO_VOC                                    = 0x0000000A,
    RSTO_LPS                                    = 0x0000000B,
    RSTO_GPIO                                   = 0x0000000C,
    RSTO_WDTIMER                                = 0x0000000D,
    RSTO_USBPHY                                 = 0x0000000E,
    RSTO_TCU                                    = 0x0000000F,
    RSTO_BBTIMER                                = 0x00000010,
    RSTO_NANDMC                                 = 0x00000011
} RESET_OTHERS_T;

#define BOUND_RSTO_RF_FIRST                     (5)
#define BOUND_RSTO_RF_AFTER                     (7)
#define NB_RSTO                                 (18)
/// For REG_DBG protect lock/unlock value
#define SYS_CTRL_PROTECT_LOCK                   (0XA50000)
#define SYS_CTRL_PROTECT_UNLOCK                 (0XA50001)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SYS_CTRL_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_SYS_CTRL_BASE          0x01A00000

typedef volatile struct
{
    /// <strong>This register is used to Lock and Unlock the protected
    /// registers.</strong>
    REG32                          REG_DBG;                      //0x00000000
    /// This register is protected.
    REG32                          Sys_Rst_Set;                  //0x00000004
    REG32                          Sys_Rst_Clr;                  //0x00000008
    /// This register is protected.
    REG32                          BB_Rst_Set;                   //0x0000000C
    REG32                          BB_Rst_Clr;                   //0x00000010
    REG32                          Clk_Sys_Mode;                 //0x00000014
    REG32                          Clk_Sys_Enable;               //0x00000018
    /// This register is protected.
    REG32                          Clk_Sys_Disable;              //0x0000001C
    REG32                          Clk_Per_Mode;                 //0x00000020
    REG32                          Clk_Per_Enable;               //0x00000024
    /// This register is protected.
    REG32                          Clk_Per_Disable;              //0x00000028
    REG32                          Clk_BB_Mode;                  //0x0000002C
    REG32                          Clk_BB_Enable;                //0x00000030
    /// This register is protected.
    REG32                          Clk_BB_Disable;               //0x00000034
    REG32                          Clk_Other_Mode;               //0x00000038
    REG32                          Clk_Other_Enable;             //0x0000003C
    /// This register is protected.
    REG32                          Clk_Other_Disable;            //0x00000040
    /// Register protected by Write_Unlocked_H.
    REG32                          Pll_Ctrl;                     //0x00000044
    /// This register is protected.
    REG32                          Sel_Clock;                    //0x00000048
    REG32                          Cfg_Clk_Sys;                  //0x0000004C
    REG32                          Cfg_Clk_Mem_Bridge;           //0x00000050
    REG32                          Cfg_Clk_Voc;                  //0x00000054
    REG32                          Cfg_Clk_Out;                  //0x00000058
    REG32                          Cfg_Clk_BB;                   //0x0000005C
    REG32                          Cfg_Clk_AudioBCK_Div;         //0x00000060
    REG32                          Cfg_Aif_Tx_Stb_Div;           //0x00000064
    REG32                          Cfg_Clk_Uart[3];              //0x00000068
    REG32                          Cfg_Clk_PWM;                  //0x00000074
    REG32                          Cfg_Clk_Auxclk;               //0x00000078
    REG32 Reserved_0000007C;                    //0x0000007C
    REG32                          Cfg_Clk_Camera_Out;           //0x00000080
    /// This register is protected.
    REG32                          Cfg_AHB;                      //0x00000084
    /// This register is protected. Used to unsplit masters manualy.
    REG32                          Ctrl_AHB;                     //0x00000088
    REG32                          XCpu_Dbg_BKP;                 //0x0000008C
    REG32                          XCpu_Dbg_Addr;                //0x00000090
    REG32                          BCpu_Dbg_BKP;                 //0x00000094
    REG32                          BCpu_Dbg_Addr;                //0x00000098
    REG32                          Cfg_Cpus_Cache_Ram_Disable;   //0x0000009C
    REG32                          Reset_Cause;                  //0x000000A0
    /// This register is protected.
    REG32                          WakeUp;                       //0x000000A4
    /// This register is protected.
    REG32                          Ignore_Charger_REG;           //0x000000A8
    REG32                          Cfg_Clk_SpiFlash;             //0x000000AC
    REG32 Reserved_000000B0;                    //0x000000B0
    REG32                          Clk_Sys_Enable2;              //0x000000B4
    /// This register is protected.
    REG32                          Clk_Sys_Disable2;             //0x000000B8
    REG32 Reserved_000000BC[5];                 //0x000000BC
    REG32                          CFG_XTAL_DIV;                 //0x000000D0
    REG32 Reserved_000000D4[10];                //0x000000D4
    /// This register is reserved.
    REG32                          Cfg_Rsd;                      //0x000000FC
} HWP_SYS_CTRL_T;

#define hwp_sysCtrl                ((HWP_SYS_CTRL_T*) KSEG1(REG_SYS_CTRL_BASE))


//REG_DBG
#define SYS_CTRL_SCRATCH(n)        (((n)&0xFFFF)<<0)
#define SYS_CTRL_WRITE_UNLOCK_STATUS (1<<30)
#define SYS_CTRL_WRITE_UNLOCK      (1<<31)

//Sys_Rst_Set
#define SYS_CTRL_SET_RST_XCPU      (1<<0)
#define SYS_CTRL_SET_RST_SYS_IRQ   (1<<1)
#define SYS_CTRL_SET_RST_SYS_A2A   (1<<2)
#define SYS_CTRL_SET_RST_ABB       (1<<3)
#define SYS_CTRL_SET_RST_DMA       (1<<4)
#define SYS_CTRL_SET_RST_GOUDA     (1<<5)
#define SYS_CTRL_SET_RST_TIMER     (1<<6)
#define SYS_CTRL_SET_RST_TCU       (1<<7)
#define SYS_CTRL_SET_RST_GPIO      (1<<8)
#define SYS_CTRL_SET_RST_KEYPAD    (1<<9)
#define SYS_CTRL_SET_RST_PWM       (1<<10)
#define SYS_CTRL_SET_RST_UART      (1<<11)
#define SYS_CTRL_SET_RST_UART2     (1<<12)
#define SYS_CTRL_SET_RST_SPI1      (1<<13)
#define SYS_CTRL_SET_RST_SPI2      (1<<14)
#define SYS_CTRL_SET_RST_SPI3      (1<<15)
#define SYS_CTRL_SET_RST_RF_SPI    (1<<16)
#define SYS_CTRL_SET_RST_SCI       (1<<17)
#define SYS_CTRL_SET_RST_SPY       (1<<18)
#define SYS_CTRL_SET_RST_I2C       (1<<19)
#define SYS_CTRL_SET_RST_I2C2      (1<<20)
#define SYS_CTRL_SET_RST_I2C3      (1<<21)
#define SYS_CTRL_SET_RST_MEM_BRIDGE (1<<22)
#define SYS_CTRL_SET_RST_SPIFLASH  (1<<23)
#define SYS_CTRL_SET_RST_EXT_APB   (1<<24)
#define SYS_CTRL_SET_RST_SDMMC     (1<<25)
#define SYS_CTRL_SET_RST_CAMERA    (1<<26)
#define SYS_CTRL_SET_RST_USBC      (1<<27)
#define SYS_CTRL_SET_RST_USBPHY    (1<<28)
#define SYS_CTRL_SET_RST_DBG       (1<<29)
#define SYS_CTRL_SET_RST_OUT       (1<<30)
#define SYS_CTRL_SOFT_RST          (1<<31)
#define SYS_CTRL_SET_SYS_RST(n)    (((n)&0x3FFFFFFF)<<0)
#define SYS_CTRL_SET_SYS_RST_MASK  (0x3FFFFFFF<<0)
#define SYS_CTRL_SET_SYS_RST_SHIFT (0)

//Sys_Rst_Clr
#define SYS_CTRL_CLR_RST_XCPU      (1<<0)
#define SYS_CTRL_CLR_RST_SYS_IRQ   (1<<1)
#define SYS_CTRL_CLR_RST_SYS_A2A   (1<<2)
#define SYS_CTRL_CLR_RST_ABB       (1<<3)
#define SYS_CTRL_CLR_RST_DMA       (1<<4)
#define SYS_CTRL_CLR_RST_GOUDA     (1<<5)
#define SYS_CTRL_CLR_RST_TIMER     (1<<6)
#define SYS_CTRL_CLR_RST_TCU       (1<<7)
#define SYS_CTRL_CLR_RST_GPIO      (1<<8)
#define SYS_CTRL_CLR_RST_KEYPAD    (1<<9)
#define SYS_CTRL_CLR_RST_PWM       (1<<10)
#define SYS_CTRL_CLR_RST_UART      (1<<11)
#define SYS_CTRL_CLR_RST_UART2     (1<<12)
#define SYS_CTRL_CLR_RST_SPI1      (1<<13)
#define SYS_CTRL_CLR_RST_SPI2      (1<<14)
#define SYS_CTRL_CLR_RST_SPI3      (1<<15)
#define SYS_CTRL_CLR_RST_RF_SPI    (1<<16)
#define SYS_CTRL_CLR_RST_SCI       (1<<17)
#define SYS_CTRL_CLR_RST_SPY       (1<<18)
#define SYS_CTRL_CLR_RST_I2C       (1<<19)
#define SYS_CTRL_CLR_RST_I2C2      (1<<20)
#define SYS_CTRL_CLR_RST_I2C3      (1<<21)
#define SYS_CTRL_CLR_RST_MEM_BRIDGE (1<<22)
#define SYS_CTRL_CLR_RST_SPIFLASH  (1<<23)
#define SYS_CTRL_CLR_RST_EXT_APB   (1<<24)
#define SYS_CTRL_CLR_RST_SDMMC     (1<<25)
#define SYS_CTRL_CLR_RST_CAMERA    (1<<26)
#define SYS_CTRL_CLR_RST_USBC      (1<<27)
#define SYS_CTRL_CLR_RST_USBPHY    (1<<28)
#define SYS_CTRL_CLR_RST_DBG       (1<<29)
#define SYS_CTRL_CLR_RST_OUT       (1<<30)
#define SYS_CTRL_CLR_SYS_RST(n)    (((n)&0x3FFFFFFF)<<0)
#define SYS_CTRL_CLR_SYS_RST_MASK  (0x3FFFFFFF<<0)
#define SYS_CTRL_CLR_SYS_RST_SHIFT (0)

//BB_Rst_Set
#define SYS_CTRL_SET_RST_VOC       (1<<0)
#define SYS_CTRL_SET_RST_AIF       (1<<1)
#define SYS_CTRL_SET_RST_IFC2      (1<<2)
#define SYS_CTRL_SET_RST_MAILBOX   (1<<3)
#define SYS_CTRL_SET_RST_SDMMC2    (1<<4)
#define SYS_CTRL_SET_RST_DAI       (1<<5)
#define SYS_CTRL_SET_RST_BCPU      (1<<6)
#define SYS_CTRL_SET_RST_BB_IRQ    (1<<7)
#define SYS_CTRL_SET_RST_BB_A2A    (1<<8)
#define SYS_CTRL_SET_RST_BB_IFC    (1<<9)
#define SYS_CTRL_SET_RST_BB_SRAM   (1<<10)
#define SYS_CTRL_SET_RST_ITLV      (1<<11)
#define SYS_CTRL_SET_RST_VITERBI   (1<<12)
#define SYS_CTRL_SET_RST_CIPHER    (1<<13)
#define SYS_CTRL_SET_RST_XCOR      (1<<14)
#define SYS_CTRL_SET_RST_COPRO     (1<<15)
#define SYS_CTRL_SET_RST_RF_IF     (1<<16)
#define SYS_CTRL_SET_RST_ROM_BB    (1<<17)
#define SYS_CTRL_SET_RST_MAILBOX_BB (1<<18)
#define SYS_CTRL_SET_RST_EXCOR     (1<<19)
#define SYS_CTRL_SET_RST_EVITAC    (1<<20)
#define SYS_CTRL_SET_RST_CORDIC    (1<<21)
#define SYS_CTRL_SET_RST_TCU_BB    (1<<22)
#define SYS_CTRL_SET_RST_BB2_SDMMC (1<<23)
#define SYS_CTRL_SET_RST_CHOLK     (1<<24)
#define SYS_CTRL_SET_RST_A53       (1<<25)
#define SYS_CTRL_SET_RST_BB_TIMER  (1<<26)
#define SYS_CTRL_SET_RST_BB_FULL   (1<<31)
#define SYS_CTRL_SET_SYS_MSB_RST(n) (((n)&0x3F)<<0)
#define SYS_CTRL_SET_SYS_MSB_RST_MASK (0x3F<<0)
#define SYS_CTRL_SET_SYS_MSB_RST_SHIFT (0)
#define SYS_CTRL_SET_BB_RST(n)     (((n)&0x1FFFFF)<<6)
#define SYS_CTRL_SET_BB_RST_MASK   (0x1FFFFF<<6)
#define SYS_CTRL_SET_BB_RST_SHIFT  (6)

//BB_Rst_Clr
#define SYS_CTRL_CLR_RST_VOC       (1<<0)
#define SYS_CTRL_CLR_RST_AIF       (1<<1)
#define SYS_CTRL_CLR_RST_IFC2      (1<<2)
#define SYS_CTRL_CLR_RST_MAILBOX   (1<<3)
#define SYS_CTRL_CLR_RST_SDMMC2    (1<<4)
#define SYS_CTRL_CLR_RST_DAI       (1<<5)
#define SYS_CTRL_CLR_RST_BCPU      (1<<6)
#define SYS_CTRL_CLR_RST_BB_IRQ    (1<<7)
#define SYS_CTRL_CLR_RST_BB_A2A    (1<<8)
#define SYS_CTRL_CLR_RST_BB_IFC    (1<<9)
#define SYS_CTRL_CLR_RST_BB_SRAM   (1<<10)
#define SYS_CTRL_CLR_RST_ITLV      (1<<11)
#define SYS_CTRL_CLR_RST_VITERBI   (1<<12)
#define SYS_CTRL_CLR_RST_CIPHER    (1<<13)
#define SYS_CTRL_CLR_RST_XCOR      (1<<14)
#define SYS_CTRL_CLR_RST_COPRO     (1<<15)
#define SYS_CTRL_CLR_RST_RF_IF     (1<<16)
#define SYS_CTRL_CLR_RST_ROM_BB    (1<<17)
#define SYS_CTRL_CLR_RST_MAILBOX_BB (1<<18)
#define SYS_CTRL_CLR_RST_EXCOR     (1<<19)
#define SYS_CTRL_CLR_RST_EVITAC    (1<<20)
#define SYS_CTRL_CLR_RST_CORDIC    (1<<21)
#define SYS_CTRL_CLR_RST_TCU_BB    (1<<22)
#define SYS_CTRL_CLR_RST_BB2_SDMMC (1<<23)
#define SYS_CTRL_CLR_RST_CHOLK     (1<<24)
#define SYS_CTRL_CLR_RST_A53       (1<<25)
#define SYS_CTRL_CLR_RST_BB_TIMER  (1<<26)
#define SYS_CTRL_CLR_RST_BB_FULL   (1<<31)
#define SYS_CTRL_CLR_SYS_MSB_RST(n) (((n)&0x3F)<<0)
#define SYS_CTRL_CLR_SYS_MSB_RST_MASK (0x3F<<0)
#define SYS_CTRL_CLR_SYS_MSB_RST_SHIFT (0)
#define SYS_CTRL_CLR_BB_RST(n)     (((n)&0x1FFFFF)<<6)
#define SYS_CTRL_CLR_BB_RST_MASK   (0x1FFFFF<<6)
#define SYS_CTRL_CLR_BB_RST_SHIFT  (6)

//Clk_Sys_Mode
#define SYS_CTRL_MODE_SYS_XCPU     (1<<0)
#define SYS_CTRL_MODE_SYS_XCPU_INT_AUTOMATIC (0<<1)
#define SYS_CTRL_MODE_SYS_XCPU_INT_MANUAL (1<<1)
#define SYS_CTRL_MODE_SYS_PCLK_CONF_AUTOMATIC (0<<2)
#define SYS_CTRL_MODE_SYS_PCLK_CONF_MANUAL (1<<2)
#define SYS_CTRL_MODE_SYS_PCLK_DATA_AUTOMATIC (0<<3)
#define SYS_CTRL_MODE_SYS_PCLK_DATA_MANUAL (1<<3)
#define SYS_CTRL_MODE_SYS_AMBA_AUTOMATIC (0<<4)
#define SYS_CTRL_MODE_SYS_AMBA_MANUAL (1<<4)
#define SYS_CTRL_MODE_SYS_DMA_AUTOMATIC (0<<5)
#define SYS_CTRL_MODE_SYS_DMA_MANUAL (1<<5)
#define SYS_CTRL_MODE_SYS_GOUDA_AUTOMATIC (0<<6)
#define SYS_CTRL_MODE_SYS_GOUDA_MANUAL (1<<6)
#define SYS_CTRL_MODE_SYS_EBC_AUTOMATIC (0<<7)
#define SYS_CTRL_MODE_SYS_EBC_MANUAL (1<<7)
#define SYS_CTRL_MODE_SYS_IFC_CH0_AUTOMATIC (0<<8)
#define SYS_CTRL_MODE_SYS_IFC_CH0_MANUAL (1<<8)
#define SYS_CTRL_MODE_SYS_IFC_CH1_AUTOMATIC (0<<9)
#define SYS_CTRL_MODE_SYS_IFC_CH1_MANUAL (1<<9)
#define SYS_CTRL_MODE_SYS_IFC_CH2_AUTOMATIC (0<<10)
#define SYS_CTRL_MODE_SYS_IFC_CH2_MANUAL (1<<10)
#define SYS_CTRL_MODE_SYS_IFC_CH3_AUTOMATIC (0<<11)
#define SYS_CTRL_MODE_SYS_IFC_CH3_MANUAL (1<<11)
#define SYS_CTRL_MODE_SYS_IFC_CH4_AUTOMATIC (0<<12)
#define SYS_CTRL_MODE_SYS_IFC_CH4_MANUAL (1<<12)
#define SYS_CTRL_MODE_SYS_IFC_CH5_AUTOMATIC (0<<13)
#define SYS_CTRL_MODE_SYS_IFC_CH5_MANUAL (1<<13)
#define SYS_CTRL_MODE_SYS_IFC_CH6_AUTOMATIC (0<<14)
#define SYS_CTRL_MODE_SYS_IFC_CH6_MANUAL (1<<14)
#define SYS_CTRL_MODE_SYS_IFC_CH7_AUTOMATIC (0<<15)
#define SYS_CTRL_MODE_SYS_IFC_CH7_MANUAL (1<<15)
#define SYS_CTRL_MODE_SYS_IFC_DBG_AUTOMATIC (0<<16)
#define SYS_CTRL_MODE_SYS_IFC_DBG_MANUAL (1<<16)
#define SYS_CTRL_MODE_SYS_A2A_AUTOMATIC (0<<17)
#define SYS_CTRL_MODE_SYS_A2A_MANUAL (1<<17)
#define SYS_CTRL_MODE_SYS_INTSRAM_AUTOMATIC (0<<18)
#define SYS_CTRL_MODE_SYS_INTSRAM_MANUAL (1<<18)
#define SYS_CTRL_MODE_SYS_DEBUG_UART_AUTOMATIC (0<<19)
#define SYS_CTRL_MODE_SYS_DEBUG_UART_MANUAL (1<<19)
#define SYS_CTRL_MODE_SYS_DBGHST_AUTOMATIC (0<<20)
#define SYS_CTRL_MODE_SYS_DBGHST_MANUAL (1<<20)
#define SYS_CTRL_MODE_SYS_VOC_AHB_AUTOMATIC (0<<21)
#define SYS_CTRL_MODE_SYS_VOC_AHB_MANUAL (1<<21)
#define SYS_CTRL_MODE_SYS_MAILBOX_AUTOMATIC (0<<22)
#define SYS_CTRL_MODE_SYS_MAILBOX_MANUAL (1<<22)
#define SYS_CTRL_MODE_SYS_IFC2_CH0_AUTOMATIC (0<<23)
#define SYS_CTRL_MODE_SYS_IFC2_CH0_MANUAL (1<<23)
#define SYS_CTRL_MODE_SYS_IFC2_CH1_AUTOMATIC (0<<24)
#define SYS_CTRL_MODE_SYS_IFC2_CH1_MANUAL (1<<24)
#define SYS_CTRL_MODE_SYS_AIF_AUTOMATIC (0<<25)
#define SYS_CTRL_MODE_SYS_AIF_MANUAL (1<<25)
#define SYS_CTRL_MODE_SYSD_SCI_AUTOMATIC (0<<26)
#define SYS_CTRL_MODE_SYSD_SCI_MANUAL (1<<26)
#define SYS_CTRL_MODE_SYSD_RF_SPI_AUTOMATIC (0<<27)
#define SYS_CTRL_MODE_SYSD_RF_SPI_MANUAL (1<<27)
#define SYS_CTRL_MODE_SYSD_OSC_AUTOMATIC (0<<28)
#define SYS_CTRL_MODE_SYSD_OSC_MANUAL (1<<28)
#define SYS_CTRL_MODE_SYSD_PWM_AUTOMATIC (0<<29)
#define SYS_CTRL_MODE_SYSD_PWM_MANUAL (1<<29)
#define SYS_CTRL_MODE_SYSD_DAIS_208K_AUTOMATIC (0<<30)
#define SYS_CTRL_MODE_SYSD_DAIS_208K_MANUAL (1<<30)
#define SYS_CTRL_MODE_CLK_SYS(n)   (((n)&0x3FFFFFFF)<<1)
#define SYS_CTRL_MODE_CLK_SYS_MASK (0x3FFFFFFF<<1)
#define SYS_CTRL_MODE_CLK_SYS_SHIFT (1)

//Clk_Sys_Enable
#define SYS_CTRL_ENABLE_SYS_XCPU   (1<<0)
#define SYS_CTRL_ENABLE_SYS_XCPU_INT (1<<1)
#define SYS_CTRL_ENABLE_SYS_PCLK_CONF (1<<2)
#define SYS_CTRL_ENABLE_SYS_PCLK_DATA (1<<3)
#define SYS_CTRL_ENABLE_SYS_AMBA   (1<<4)
#define SYS_CTRL_ENABLE_SYS_DMA    (1<<5)
#define SYS_CTRL_ENABLE_SYS_GOUDA  (1<<6)
#define SYS_CTRL_ENABLE_SYS_EBC    (1<<7)
#define SYS_CTRL_ENABLE_SYS_IFC_CH0 (1<<8)
#define SYS_CTRL_ENABLE_SYS_IFC_CH1 (1<<9)
#define SYS_CTRL_ENABLE_SYS_IFC_CH2 (1<<10)
#define SYS_CTRL_ENABLE_SYS_IFC_CH3 (1<<11)
#define SYS_CTRL_ENABLE_SYS_IFC_CH4 (1<<12)
#define SYS_CTRL_ENABLE_SYS_IFC_CH5 (1<<13)
#define SYS_CTRL_ENABLE_SYS_IFC_CH6 (1<<14)
#define SYS_CTRL_ENABLE_SYS_IFC_CH7 (1<<15)
#define SYS_CTRL_ENABLE_SYS_IFC_DBG (1<<16)
#define SYS_CTRL_ENABLE_SYS_A2A    (1<<17)
#define SYS_CTRL_ENABLE_SYS_INTSRAM (1<<18)
#define SYS_CTRL_ENABLE_SYS_DEBUG_UART (1<<19)
#define SYS_CTRL_ENABLE_SYS_DBGHST (1<<20)
#define SYS_CTRL_ENABLE_SYS_VOC_AHB (1<<21)
#define SYS_CTRL_ENABLE_SYS_MAILBOX (1<<22)
#define SYS_CTRL_ENABLE_SYS_IFC2_CH0 (1<<23)
#define SYS_CTRL_ENABLE_SYS_IFC2_CH1 (1<<24)
#define SYS_CTRL_ENABLE_SYS_AIF    (1<<25)
#define SYS_CTRL_ENABLE_SYSD_SCI   (1<<26)
#define SYS_CTRL_ENABLE_SYSD_RF_SPI (1<<27)
#define SYS_CTRL_ENABLE_SYSD_OSC   (1<<28)
#define SYS_CTRL_ENABLE_SYSD_PWM   (1<<29)
#define SYS_CTRL_ENABLE_SYSD_DAIS_208K (1<<30)
#define SYS_CTRL_ENABLE_CLK_SYS(n) (((n)&0x7FFFFFFF)<<0)
#define SYS_CTRL_ENABLE_CLK_SYS_MASK (0x7FFFFFFF<<0)
#define SYS_CTRL_ENABLE_CLK_SYS_SHIFT (0)

//Clk_Sys_Disable
#define SYS_CTRL_DISABLE_SYS_XCPU  (1<<0)
#define SYS_CTRL_DISABLE_SYS_XCPU_INT (1<<1)
#define SYS_CTRL_DISABLE_SYS_PCLK_CONF (1<<2)
#define SYS_CTRL_DISABLE_SYS_PCLK_DATA (1<<3)
#define SYS_CTRL_DISABLE_SYS_AMBA  (1<<4)
#define SYS_CTRL_DISABLE_SYS_DMA   (1<<5)
#define SYS_CTRL_DISABLE_SYS_GOUDA (1<<6)
#define SYS_CTRL_DISABLE_SYS_EBC   (1<<7)
#define SYS_CTRL_DISABLE_SYS_IFC_CH0 (1<<8)
#define SYS_CTRL_DISABLE_SYS_IFC_CH1 (1<<9)
#define SYS_CTRL_DISABLE_SYS_IFC_CH2 (1<<10)
#define SYS_CTRL_DISABLE_SYS_IFC_CH3 (1<<11)
#define SYS_CTRL_DISABLE_SYS_IFC_CH4 (1<<12)
#define SYS_CTRL_DISABLE_SYS_IFC_CH5 (1<<13)
#define SYS_CTRL_DISABLE_SYS_IFC_CH6 (1<<14)
#define SYS_CTRL_DISABLE_SYS_IFC_CH7 (1<<15)
#define SYS_CTRL_DISABLE_SYS_IFC_DBG (1<<16)
#define SYS_CTRL_DISABLE_SYS_A2A   (1<<17)
#define SYS_CTRL_DISABLE_SYS_INTSRAM (1<<18)
#define SYS_CTRL_DISABLE_SYS_DEBUG_UART (1<<19)
#define SYS_CTRL_DISABLE_SYS_DBGHST (1<<20)
#define SYS_CTRL_DISABLE_SYS_VOC_AHB (1<<21)
#define SYS_CTRL_DISABLE_SYS_MAILBOX (1<<22)
#define SYS_CTRL_DISABLE_SYS_IFC2_CH0 (1<<23)
#define SYS_CTRL_DISABLE_SYS_IFC2_CH1 (1<<24)
#define SYS_CTRL_DISABLE_SYS_AIF   (1<<25)
#define SYS_CTRL_DISABLE_SYSD_SCI  (1<<26)
#define SYS_CTRL_DISABLE_SYSD_RF_SPI (1<<27)
#define SYS_CTRL_DISABLE_SYSD_OSC  (1<<28)
#define SYS_CTRL_DISABLE_SYSD_PWM  (1<<29)
#define SYS_CTRL_DISABLE_SYSD_DAIS_208K (1<<30)
#define SYS_CTRL_DISABLE_CLK_SYS(n) (((n)&0x7FFFFFFF)<<0)
#define SYS_CTRL_DISABLE_CLK_SYS_MASK (0x7FFFFFFF<<0)
#define SYS_CTRL_DISABLE_CLK_SYS_SHIFT (0)

//Clk_Per_Mode
#define SYS_CTRL_MODE_PER_UART_AUTOMATIC (0<<0)
#define SYS_CTRL_MODE_PER_UART_MANUAL (1<<0)
#define SYS_CTRL_MODE_PER_UART2_AUTOMATIC (0<<1)
#define SYS_CTRL_MODE_PER_UART2_MANUAL (1<<1)
#define SYS_CTRL_MODE_PER_I2C_AUTOMATIC (0<<2)
#define SYS_CTRL_MODE_PER_I2C_MANUAL (1<<2)
#define SYS_CTRL_MODE_PER_I2C2_AUTOMATIC (0<<3)
#define SYS_CTRL_MODE_PER_I2C2_MANUAL (1<<3)
#define SYS_CTRL_MODE_PER_I2C3_AUTOMATIC (0<<4)
#define SYS_CTRL_MODE_PER_I2C3_MANUAL (1<<4)
#define SYS_CTRL_MODE_PER_SDMMC_AUTOMATIC (0<<5)
#define SYS_CTRL_MODE_PER_SDMMC_MANUAL (1<<5)
#define SYS_CTRL_MODE_PER_SDMMC2_AUTOMATIC (0<<6)
#define SYS_CTRL_MODE_PER_SDMMC2_MANUAL (1<<6)
#define SYS_CTRL_MODE_PER_CAMERA_AUTOMATIC (0<<7)
#define SYS_CTRL_MODE_PER_CAMERA_MANUAL (1<<7)
#define SYS_CTRL_MODE_PERD_SPI1_AUTOMATIC (0<<8)
#define SYS_CTRL_MODE_PERD_SPI1_MANUAL (1<<8)
#define SYS_CTRL_MODE_PERD_SPI2_AUTOMATIC (0<<9)
#define SYS_CTRL_MODE_PERD_SPI2_MANUAL (1<<9)
#define SYS_CTRL_MODE_PERD_SPI3_AUTOMATIC (0<<10)
#define SYS_CTRL_MODE_PERD_SPI3_MANUAL (1<<10)
#define SYS_CTRL_MODE_CLK_PER(n)   (((n)&0x7FF)<<0)
#define SYS_CTRL_MODE_CLK_PER_MASK (0x7FF<<0)
#define SYS_CTRL_MODE_CLK_PER_SHIFT (0)

//Clk_Per_Enable
#define SYS_CTRL_ENABLE_PER_UART   (1<<0)
#define SYS_CTRL_ENABLE_PER_UART2  (1<<1)
#define SYS_CTRL_ENABLE_PER_I2C    (1<<2)
#define SYS_CTRL_ENABLE_PER_I2C2   (1<<3)
#define SYS_CTRL_ENABLE_PER_I2C3   (1<<4)
#define SYS_CTRL_ENABLE_PER_SDMMC  (1<<5)
#define SYS_CTRL_ENABLE_PER_SDMMC2 (1<<6)
#define SYS_CTRL_ENABLE_PER_CAMERA (1<<7)
#define SYS_CTRL_ENABLE_PERD_SPI1  (1<<8)
#define SYS_CTRL_ENABLE_PERD_SPI2  (1<<9)
#define SYS_CTRL_ENABLE_PERD_SPI3  (1<<10)
#define SYS_CTRL_ENABLE_PER_USBC   (1<<11)
#define SYS_CTRL_ENABLE_PER_SPY    (1<<12)
#define SYS_CTRL_ENABLE_PER_TEST   (1<<13)
#define SYS_CTRL_ENABLE_CLK_PER(n) (((n)&0x3FFF)<<0)
#define SYS_CTRL_ENABLE_CLK_PER_MASK (0x3FFF<<0)
#define SYS_CTRL_ENABLE_CLK_PER_SHIFT (0)

//Clk_Per_Disable
#define SYS_CTRL_DISABLE_PER_UART  (1<<0)
#define SYS_CTRL_DISABLE_PER_UART2 (1<<1)
#define SYS_CTRL_DISABLE_PER_I2C   (1<<2)
#define SYS_CTRL_DISABLE_PER_I2C2  (1<<3)
#define SYS_CTRL_DISABLE_PER_I2C3  (1<<4)
#define SYS_CTRL_DISABLE_PER_SDMMC (1<<5)
#define SYS_CTRL_DISABLE_PER_SDMMC2 (1<<6)
#define SYS_CTRL_DISABLE_PER_CAMERA (1<<7)
#define SYS_CTRL_DISABLE_PERD_SPI1 (1<<8)
#define SYS_CTRL_DISABLE_PERD_SPI2 (1<<9)
#define SYS_CTRL_DISABLE_PERD_SPI3 (1<<10)
#define SYS_CTRL_DISABLE_PER_USBC  (1<<11)
#define SYS_CTRL_DISABLE_PER_SPY   (1<<12)
#define SYS_CTRL_DISABLE_PER_TEST  (1<<13)
#define SYS_CTRL_DISABLE_CLK_PER(n) (((n)&0x3FFF)<<0)
#define SYS_CTRL_DISABLE_CLK_PER_MASK (0x3FFF<<0)
#define SYS_CTRL_DISABLE_CLK_PER_SHIFT (0)

//Clk_BB_Mode
#define SYS_CTRL_MODE_BB_BCPU      (1<<0)
#define SYS_CTRL_MODE_BB_BCPU_INT_AUTOMATIC (0<<1)
#define SYS_CTRL_MODE_BB_BCPU_INT_MANUAL (1<<1)
#define SYS_CTRL_MODE_BB_AMBA_AUTOMATIC (0<<2)
#define SYS_CTRL_MODE_BB_AMBA_MANUAL (1<<2)
#define SYS_CTRL_MODE_BB_PCLK_CONF_AUTOMATIC (0<<3)
#define SYS_CTRL_MODE_BB_PCLK_CONF_MANUAL (1<<3)
#define SYS_CTRL_MODE_BB_PCLK_DATA_AUTOMATIC (0<<4)
#define SYS_CTRL_MODE_BB_PCLK_DATA_MANUAL (1<<4)
#define SYS_CTRL_MODE_BB_ROM_AHB_AUTOMATIC (0<<5)
#define SYS_CTRL_MODE_BB_ROM_AHB_MANUAL (1<<5)
#define SYS_CTRL_MODE_BB_MAILBOX_AUTOMATIC (0<<6)
#define SYS_CTRL_MODE_BB_MAILBOX_MANUAL (1<<6)
#define SYS_CTRL_MODE_BB_EXCOR_AUTOMATIC (0<<7)
#define SYS_CTRL_MODE_BB_EXCOR_MANUAL (1<<7)
#define SYS_CTRL_MODE_BB_IFC_CH2_AUTOMATIC (0<<8)
#define SYS_CTRL_MODE_BB_IFC_CH2_MANUAL (1<<8)
#define SYS_CTRL_MODE_BB_IFC_CH3_AUTOMATIC (0<<9)
#define SYS_CTRL_MODE_BB_IFC_CH3_MANUAL (1<<9)
#define SYS_CTRL_MODE_BB_SRAM_AUTOMATIC (0<<10)
#define SYS_CTRL_MODE_BB_SRAM_MANUAL (1<<10)
#define SYS_CTRL_MODE_BB_A2A_AUTOMATIC (0<<11)
#define SYS_CTRL_MODE_BB_A2A_MANUAL (1<<11)
#define SYS_CTRL_MODE_BB_ITLV_AUTOMATIC (0<<12)
#define SYS_CTRL_MODE_BB_ITLV_MANUAL (1<<12)
#define SYS_CTRL_MODE_BB_VITERBI_AUTOMATIC (0<<13)
#define SYS_CTRL_MODE_BB_VITERBI_MANUAL (1<<13)
#define SYS_CTRL_MODE_BB_CIPHER_AUTOMATIC (0<<14)
#define SYS_CTRL_MODE_BB_CIPHER_MANUAL (1<<14)
#define SYS_CTRL_MODE_BB_RF_IF_AUTOMATIC (0<<15)
#define SYS_CTRL_MODE_BB_RF_IF_MANUAL (1<<15)
#define SYS_CTRL_MODE_BB_COPRO_AUTOMATIC (0<<16)
#define SYS_CTRL_MODE_BB_COPRO_MANUAL (1<<16)
#define SYS_CTRL_MODE_BB_CP2_REG_AUTOMATIC (0<<17)
#define SYS_CTRL_MODE_BB_CP2_REG_MANUAL (1<<17)
#define SYS_CTRL_MODE_BB_XCOR_AUTOMATIC (0<<18)
#define SYS_CTRL_MODE_BB_XCOR_MANUAL (1<<18)
#define SYS_CTRL_MODE_BB_EVITAC_AUTOMATIC (0<<19)
#define SYS_CTRL_MODE_BB_EVITAC_MANUAL (1<<19)
#define SYS_CTRL_MODE_BB_CORDIC_AUTOMATIC (0<<20)
#define SYS_CTRL_MODE_BB_CORDIC_MANUAL (1<<20)
#define SYS_CTRL_MODE_BB_COM_REGS_AUTOMATIC (0<<21)
#define SYS_CTRL_MODE_BB_COM_REGS_MANUAL (1<<21)
#define SYS_CTRL_MODE_BB2_SDMMC_AUTOMATIC (0<<22)
#define SYS_CTRL_MODE_BB2_SDMMC_MANUAL (1<<22)
#define SYS_CTRL_MODE_BB_CHOLK_AUTOMATIC (0<<23)
#define SYS_CTRL_MODE_BB_CHOLK_MANUAL (1<<23)
#define SYS_CTRL_MODE_CLK_BB(n)    (((n)&0x7FFFFF)<<1)
#define SYS_CTRL_MODE_CLK_BB_MASK  (0x7FFFFF<<1)
#define SYS_CTRL_MODE_CLK_BB_SHIFT (1)

//Clk_BB_Enable
#define SYS_CTRL_ENABLE_BB_BCPU    (1<<0)
#define SYS_CTRL_ENABLE_BB_BCPU_INT (1<<1)
#define SYS_CTRL_ENABLE_BB_AMBA    (1<<2)
#define SYS_CTRL_ENABLE_BB_PCLK_CONF (1<<3)
#define SYS_CTRL_ENABLE_BB_PCLK_DATA (1<<4)
#define SYS_CTRL_ENABLE_BB_ROM_AHB (1<<5)
#define SYS_CTRL_ENABLE_BB_MAILBOX (1<<6)
#define SYS_CTRL_ENABLE_BB_EXCOR   (1<<7)
#define SYS_CTRL_ENABLE_BB_IFC_CH2 (1<<8)
#define SYS_CTRL_ENABLE_BB_IFC_CH3 (1<<9)
#define SYS_CTRL_ENABLE_BB_SRAM    (1<<10)
#define SYS_CTRL_ENABLE_BB_A2A     (1<<11)
#define SYS_CTRL_ENABLE_BB_ITLV    (1<<12)
#define SYS_CTRL_ENABLE_BB_VITERBI (1<<13)
#define SYS_CTRL_ENABLE_BB_CIPHER  (1<<14)
#define SYS_CTRL_ENABLE_BB_RF_IF   (1<<15)
#define SYS_CTRL_ENABLE_BB_COPRO   (1<<16)
#define SYS_CTRL_ENABLE_BB_CP2_REG (1<<17)
#define SYS_CTRL_ENABLE_BB_XCOR    (1<<18)
#define SYS_CTRL_ENABLE_BB_EVITAC  (1<<19)
#define SYS_CTRL_ENABLE_BB_CORDIC  (1<<20)
#define SYS_CTRL_ENABLE_BB_COM_REGS (1<<21)
#define SYS_CTRL_ENABLE_BB2_SDMMC  (1<<22)
#define SYS_CTRL_ENABLE_BB_CHOLK   (1<<23)
#define SYS_CTRL_ENABLE_BB_IRQ     (1<<24)
#define SYS_CTRL_ENABLE_BB_A53     (1<<25)
#define SYS_CTRL_ENABLE_BB_TIMER   (1<<26)
#define SYS_CTRL_ENABLE_BBD_OSC    (1<<27)
#define SYS_CTRL_ENABLE_CLK_BB(n)  (((n)&0xFFFFFFF)<<0)
#define SYS_CTRL_ENABLE_CLK_BB_MASK (0xFFFFFFF<<0)
#define SYS_CTRL_ENABLE_CLK_BB_SHIFT (0)

//Clk_BB_Disable
#define SYS_CTRL_DISABLE_BB_BCPU   (1<<0)
#define SYS_CTRL_DISABLE_BB_BCPU_INT (1<<1)
#define SYS_CTRL_DISABLE_BB_AMBA   (1<<2)
#define SYS_CTRL_DISABLE_BB_PCLK_CONF (1<<3)
#define SYS_CTRL_DISABLE_BB_PCLK_DATA (1<<4)
#define SYS_CTRL_DISABLE_BB_ROM_AHB (1<<5)
#define SYS_CTRL_DISABLE_BB_MAILBOX (1<<6)
#define SYS_CTRL_DISABLE_BB_EXCOR  (1<<7)
#define SYS_CTRL_DISABLE_BB_IFC_CH2 (1<<8)
#define SYS_CTRL_DISABLE_BB_IFC_CH3 (1<<9)
#define SYS_CTRL_DISABLE_BB_SRAM   (1<<10)
#define SYS_CTRL_DISABLE_BB_A2A    (1<<11)
#define SYS_CTRL_DISABLE_BB_ITLV   (1<<12)
#define SYS_CTRL_DISABLE_BB_VITERBI (1<<13)
#define SYS_CTRL_DISABLE_BB_CIPHER (1<<14)
#define SYS_CTRL_DISABLE_BB_RF_IF  (1<<15)
#define SYS_CTRL_DISABLE_BB_COPRO  (1<<16)
#define SYS_CTRL_DISABLE_BB_CP2_REG (1<<17)
#define SYS_CTRL_DISABLE_BB_XCOR   (1<<18)
#define SYS_CTRL_DISABLE_BB_EVITAC (1<<19)
#define SYS_CTRL_DISABLE_BB_CORDIC (1<<20)
#define SYS_CTRL_DISABLE_BB_COM_REGS (1<<21)
#define SYS_CTRL_DISABLE_BB2_SDMMC (1<<22)
#define SYS_CTRL_DISABLE_BB_CHOLK  (1<<23)
#define SYS_CTRL_DISABLE_BB_IRQ    (1<<24)
#define SYS_CTRL_DISABLE_BB_A53    (1<<25)
#define SYS_CTRL_DISABLE_BB_TIMER  (1<<26)
#define SYS_CTRL_DISABLE_BBD_OSC   (1<<27)
#define SYS_CTRL_DISABLE_CLK_BB(n) (((n)&0xFFFFFFF)<<0)
#define SYS_CTRL_DISABLE_CLK_BB_MASK (0xFFFFFFF<<0)
#define SYS_CTRL_DISABLE_CLK_BB_SHIFT (0)

//Clk_Other_Mode
#define SYS_CTRL_MODE_OC_DEBUG_UART_AUTOMATIC (0<<0)
#define SYS_CTRL_MODE_OC_DEBUG_UART_MANUAL (1<<0)
#define SYS_CTRL_MODE_OC_BCK_AUTOMATIC (0<<1)
#define SYS_CTRL_MODE_OC_BCK_MANUAL (1<<1)
#define SYS_CTRL_MODE_OC_UART_AUTOMATIC (0<<2)
#define SYS_CTRL_MODE_OC_UART_MANUAL (1<<2)
#define SYS_CTRL_MODE_OC_UART2_AUTOMATIC (0<<3)
#define SYS_CTRL_MODE_OC_UART2_MANUAL (1<<3)
#define SYS_CTRL_MODE_OC_RF_RX_AUTOMATIC (0<<4)
#define SYS_CTRL_MODE_OC_RF_RX_MANUAL (1<<4)
#define SYS_CTRL_MODE_OC_RF_TX_AUTOMATIC (0<<5)
#define SYS_CTRL_MODE_OC_RF_TX_MANUAL (1<<5)
#define SYS_CTRL_MODE_OC_MEM_BRIDGE_AUTOMATIC (0<<6)
#define SYS_CTRL_MODE_OC_MEM_BRIDGE_MANUAL (1<<6)
#define SYS_CTRL_MODE_OC_VOC_CORE_AUTOMATIC (0<<7)
#define SYS_CTRL_MODE_OC_VOC_CORE_MANUAL (1<<7)
#define SYS_CTRL_MODE_OC_VOC_AHB_AUTOMATIC (0<<8)
#define SYS_CTRL_MODE_OC_VOC_AHB_MANUAL (1<<8)
#define SYS_CTRL_MODE_OC_VOC_DAI_AUTOMATIC (0<<9)
#define SYS_CTRL_MODE_OC_VOC_DAI_MANUAL (1<<9)
#define SYS_CTRL_MODE_CLK_OTHER(n) (((n)&0x3FF)<<0)
#define SYS_CTRL_MODE_CLK_OTHER_MASK (0x3FF<<0)
#define SYS_CTRL_MODE_CLK_OTHER_SHIFT (0)

//Clk_Other_Enable
#define SYS_CTRL_ENABLE_OC_DEBUG_UART (1<<0)
#define SYS_CTRL_ENABLE_OC_BCK     (1<<1)
#define SYS_CTRL_ENABLE_OC_UART    (1<<2)
#define SYS_CTRL_ENABLE_OC_UART2   (1<<3)
#define SYS_CTRL_ENABLE_OC_RF_RX   (1<<4)
#define SYS_CTRL_ENABLE_OC_RF_TX   (1<<5)
#define SYS_CTRL_ENABLE_OC_MEM_BRIDGE (1<<6)
#define SYS_CTRL_ENABLE_OC_VOC_CORE (1<<7)
#define SYS_CTRL_ENABLE_OC_VOC_AHB (1<<8)
#define SYS_CTRL_ENABLE_OC_VOC_DAI (1<<9)
#define SYS_CTRL_ENABLE_OC_VOC     (1<<10)
#define SYS_CTRL_ENABLE_OC_LPS     (1<<11)
#define SYS_CTRL_ENABLE_OC_GPIO    (1<<12)
#define SYS_CTRL_ENABLE_OC_USBPHY  (1<<13)
#define SYS_CTRL_ENABLE_OC_PIX     (1<<14)
#define SYS_CTRL_ENABLE_OC_RESERVE (1<<15)
#define SYS_CTRL_ENABLE_OC_MEM_CLK_OUT (1<<16)
#define SYS_CTRL_ENABLE_OC_DBG     (1<<17)
#define SYS_CTRL_ENABLE_OC_TCU     (1<<18)
#define SYS_CTRL_ENABLE_OC_SYS_SPIFLASH (1<<19)
#define SYS_CTRL_ENABLE_OC_SPIFLASH (1<<20)
#define SYS_CTRL_ENABLE_CLK_OTHER(n) (((n)&0x1FFFFF)<<0)
#define SYS_CTRL_ENABLE_CLK_OTHER_MASK (0x1FFFFF<<0)
#define SYS_CTRL_ENABLE_CLK_OTHER_SHIFT (0)

//Clk_Other_Disable
#define SYS_CTRL_DISABLE_OC_DEBUG_UART (1<<0)
#define SYS_CTRL_DISABLE_OC_BCK    (1<<1)
#define SYS_CTRL_DISABLE_OC_UART   (1<<2)
#define SYS_CTRL_DISABLE_OC_UART2  (1<<3)
#define SYS_CTRL_DISABLE_OC_RF_RX  (1<<4)
#define SYS_CTRL_DISABLE_OC_RF_TX  (1<<5)
#define SYS_CTRL_DISABLE_OC_MEM_BRIDGE (1<<6)
#define SYS_CTRL_DISABLE_OC_VOC_CORE (1<<7)
#define SYS_CTRL_DISABLE_OC_VOC_AHB (1<<8)
#define SYS_CTRL_DISABLE_OC_VOC_DAI (1<<9)
#define SYS_CTRL_DISABLE_OC_VOC    (1<<10)
#define SYS_CTRL_DISABLE_OC_LPS    (1<<11)
#define SYS_CTRL_DISABLE_OC_GPIO   (1<<12)
#define SYS_CTRL_DISABLE_OC_USBPHY (1<<13)
#define SYS_CTRL_DISABLE_OC_PIX    (1<<14)
#define SYS_CTRL_DISABLE_OC_RESERVE (1<<15)
#define SYS_CTRL_DISABLE_OC_MEM_CLK_OUT (1<<16)
#define SYS_CTRL_DISABLE_OC_DBG    (1<<17)
#define SYS_CTRL_DISABLE_OC_TCU    (1<<18)
#define SYS_CTRL_DISABLE_OC_SYS_SPIFLASH (1<<19)
#define SYS_CTRL_DISABLE_OC_SPIFLASH (1<<20)
#define SYS_CTRL_DISABLE_CLK_OTHER(n) (((n)&0x1FFFFF)<<0)
#define SYS_CTRL_DISABLE_CLK_OTHER_MASK (0x1FFFFF<<0)
#define SYS_CTRL_DISABLE_CLK_OTHER_SHIFT (0)

//Pll_Ctrl
#define SYS_CTRL_PLL_ENABLE        (1<<0)
#define SYS_CTRL_PLL_ENABLE_MASK   (1<<0)
#define SYS_CTRL_PLL_ENABLE_SHIFT  (0)
#define SYS_CTRL_PLL_ENABLE_POWER_DOWN (0<<0)
#define SYS_CTRL_PLL_ENABLE_ENABLE (1<<0)
#define SYS_CTRL_PLL_LOCK_RESET    (1<<4)
#define SYS_CTRL_PLL_LOCK_RESET_MASK (1<<4)
#define SYS_CTRL_PLL_LOCK_RESET_SHIFT (4)
#define SYS_CTRL_PLL_LOCK_RESET_RESET (0<<4)
#define SYS_CTRL_PLL_LOCK_RESET_NO_RESET (1<<4)
#define SYS_CTRL_PLL_CLK_FAST_ENABLE_MASK (1<<12)
#define SYS_CTRL_PLL_CLK_FAST_ENABLE_SHIFT (12)
#define SYS_CTRL_PLL_CLK_FAST_ENABLE_DISABLE (0<<12)
#define SYS_CTRL_PLL_CLK_FAST_ENABLE_ENABLE (1<<12)

//Sel_Clock
#define SYS_CTRL_SLOW_SEL_RF_OSCILLATOR (1<<0)
#define SYS_CTRL_SLOW_SEL_RF_RF    (0<<0)
#define SYS_CTRL_SYS_SEL_FAST_SLOW (1<<1)
#define SYS_CTRL_SYS_SEL_FAST_FAST (0<<1)
#define SYS_CTRL_TCU_13M_SEL_13M   (0<<2)
#define SYS_CTRL_TCU_13M_SEL_26M   (1<<2)
#define SYS_CTRL_PLL_DISABLE_LPS_DISABLE (1<<3)
#define SYS_CTRL_PLL_DISABLE_LPS_ENABLE (0<<3)
#define SYS_CTRL_RF_DETECTED_OK    (1<<4)
#define SYS_CTRL_RF_DETECTED_NO    (0<<4)
#define SYS_CTRL_RF_DETECT_BYPASS  (1<<5)
#define SYS_CTRL_RF_DETECT_RESET   (1<<6)
#define SYS_CTRL_RF_SELECTED_L     (1<<7)
#define SYS_CTRL_PLL_LOCKED        (1<<8)
#define SYS_CTRL_PLL_LOCKED_MASK   (1<<8)
#define SYS_CTRL_PLL_LOCKED_SHIFT  (8)
#define SYS_CTRL_PLL_LOCKED_LOCKED (1<<8)
#define SYS_CTRL_PLL_LOCKED_NOT_LOCKED (0<<8)
#define SYS_CTRL_FAST_SELECTED_L   (1<<9)
#define SYS_CTRL_FAST_SELECTED_L_MASK (1<<9)
#define SYS_CTRL_FAST_SELECTED_L_SHIFT (9)
#define SYS_CTRL_SPIFLASH_SEL_FAST (1<<10)
#define SYS_CTRL_SPIFLASH_SEL_FAST_SLOW (1<<10)
#define SYS_CTRL_SPIFLASH_SEL_FAST_FAST (0<<10)
#define SYS_CTRL_MEM_BRIDGE_SEL_FAST  (1<<11)
#define SYS_CTRL_MEM_BRIDGE_SEL_FAST_SLOW (1<<11)
#define SYS_CTRL_MEM_BRIDGE_SEL_FAST_FAST (0<<11)
#define SYS_CTRL_BB_SEL_FAST       (1<<12)
#define SYS_CTRL_BB_SEL_FAST_SLOW       (1<<12)
#define SYS_CTRL_BB_SEL_FAST_FAST       (0<<12)
#define SYS_CTRL_PLL_BYPASS_LOCK   (1<<13)
#define SYS_CTRL_VOC_SEL_FAST      (1<<14)
#define SYS_CTRL_VOC_SEL_FAST_SLOW      (1<<14)
#define SYS_CTRL_VOC_SEL_FAST_FAST      (0<<14)
//Cfg_Clk_Sys
#define SYS_CTRL_SYS_FREQ(n)       (((n)&15)<<0)
#define SYS_CTRL_SYS_FREQ_MASK     (15<<0)
#define SYS_CTRL_SYS_FREQ_SHIFT    (0)
#define SYS_CTRL_SYS_FREQ_312M     (13<<0)
#define SYS_CTRL_SYS_FREQ_250M     (12<<0)
#define SYS_CTRL_SYS_FREQ_208M     (11<<0)
#define SYS_CTRL_SYS_FREQ_178M     (10<<0)
#define SYS_CTRL_SYS_FREQ_156M     (9<<0)
#define SYS_CTRL_SYS_FREQ_139M     (8<<0)
#define SYS_CTRL_SYS_FREQ_125M     (7<<0)
#define SYS_CTRL_SYS_FREQ_113M     (6<<0)
#define SYS_CTRL_SYS_FREQ_104M     (5<<0)
#define SYS_CTRL_SYS_FREQ_89M      (4<<0)
#define SYS_CTRL_SYS_FREQ_78M      (3<<0)
#define SYS_CTRL_SYS_FREQ_52M      (2<<0)
#define SYS_CTRL_SYS_FREQ_39M      (1<<0)
#define SYS_CTRL_SYS_FREQ_26M      (0<<0)

//Cfg_Clk_Mem_Bridge
#define SYS_CTRL_MEM_FREQ(n)       (((n)&15)<<0)
#define SYS_CTRL_MEM_FREQ_MASK     (15<<0)
#define SYS_CTRL_MEM_FREQ_SHIFT    (0)
#define SYS_CTRL_MEM_FREQ_312M     (13<<0)
#define SYS_CTRL_MEM_FREQ_250M     (12<<0)
#define SYS_CTRL_MEM_FREQ_208M     (11<<0)
#define SYS_CTRL_MEM_FREQ_178M     (10<<0)
#define SYS_CTRL_MEM_FREQ_156M     (9<<0)
#define SYS_CTRL_MEM_FREQ_139M     (8<<0)
#define SYS_CTRL_MEM_FREQ_125M     (7<<0)
#define SYS_CTRL_MEM_FREQ_113M     (6<<0)
#define SYS_CTRL_MEM_FREQ_104M     (5<<0)
#define SYS_CTRL_MEM_FREQ_89M      (4<<0)
#define SYS_CTRL_MEM_FREQ_78M      (3<<0)
#define SYS_CTRL_MEM_FREQ_52M      (2<<0)
#define SYS_CTRL_MEM_FREQ_39M      (1<<0)
#define SYS_CTRL_MEM_FREQ_26M      (0<<0)

//Cfg_Clk_Voc
#define SYS_CTRL_VOC_FREQ(n)       (((n)&15)<<0)
#define SYS_CTRL_VOC_FREQ_MASK     (15<<0)
#define SYS_CTRL_VOC_FREQ_SHIFT    (0)
#define SYS_CTRL_VOC_FREQ_312M     (13<<0)
#define SYS_CTRL_VOC_FREQ_250M     (12<<0)
#define SYS_CTRL_VOC_FREQ_208M     (11<<0)
#define SYS_CTRL_VOC_FREQ_178M     (10<<0)
#define SYS_CTRL_VOC_FREQ_156M     (9<<0)
#define SYS_CTRL_VOC_FREQ_139M     (8<<0)
#define SYS_CTRL_VOC_FREQ_125M     (7<<0)
#define SYS_CTRL_VOC_FREQ_113M     (6<<0)
#define SYS_CTRL_VOC_FREQ_104M     (5<<0)
#define SYS_CTRL_VOC_FREQ_89M      (4<<0)
#define SYS_CTRL_VOC_FREQ_78M      (3<<0)
#define SYS_CTRL_VOC_FREQ_52M      (2<<0)
#define SYS_CTRL_VOC_FREQ_39M      (1<<0)
#define SYS_CTRL_VOC_FREQ_26M      (0<<0)

//Cfg_Clk_Out
#define SYS_CTRL_CLKOUT_SEL(n)     (((n)&31)<<0)
#define SYS_CTRL_CLKOUT_SEL_MASK   (31<<0)
#define SYS_CTRL_CLKOUT_SEL_SHIFT  (0)
#define SYS_CTRL_CLKOUT_SEL_CLK_SYS (0<<0)
#define SYS_CTRL_CLKOUT_SEL_CLK_BB_ (1<<0)
#define SYS_CTRL_CLKOUT_SEL_CLK_PER (2<<0)
#define SYS_CTRL_CLKOUT_SEL_CLK_TCU (3<<0)
#define SYS_CTRL_CLKOUT_SEL_CLK_HOST_UART (4<<0)
#define SYS_CTRL_CLKOUT_SEL_CLK_UART1 (5<<0)
#define SYS_CTRL_CLKOUT_SEL_CLK_RF_RX (6<<0)
#define SYS_CTRL_CLKOUT_SEL_CLK_LPS (7<<0)
#define SYS_CTRL_CLKOUT_SEL_CLK_GPIO (8<<0)
#define SYS_CTRL_CLKOUT_SEL_CLK_SPIFLASH (9<<0)
#define SYS_CTRL_CLKOUT_SEL_CLK_MEM_BRIDGE (10<<0)
#define SYS_CTRL_CLKOUT_SEL_CLK_MEM_BRIDGE_OUT (11<<0)
#define SYS_CTRL_CLKOUT_SEL_CLK_MEM_BRIDGE_FAST (12<<0)
#define SYS_CTRL_CLKOUT_SEL_CLK_USB_48M (13<<0)
#define SYS_CTRL_CLKOUT_SEL_CLK_SLOW (14<<0)
#define SYS_CTRL_CLKOUT_SEL_CLK_32K (15<<0)
#define SYS_CTRL_CLKOUT_ENABLE_DISABLE (0<<5)
#define SYS_CTRL_CLKOUT_ENABLE_ENABLE (1<<5)

//Cfg_Clk_BB
#define SYS_CTRL_BB_FREQ(n)        (((n)&15)<<0)
#define SYS_CTRL_BB_FREQ_MASK      (15<<0)
#define SYS_CTRL_BB_FREQ_SHIFT     (0)
#define SYS_CTRL_BB_FREQ_312M      (13<<0)
#define SYS_CTRL_BB_FREQ_250M      (12<<0)
#define SYS_CTRL_BB_FREQ_208M      (11<<0)
#define SYS_CTRL_BB_FREQ_178M      (10<<0)
#define SYS_CTRL_BB_FREQ_156M      (9<<0)
#define SYS_CTRL_BB_FREQ_139M      (8<<0)
#define SYS_CTRL_BB_FREQ_125M      (7<<0)
#define SYS_CTRL_BB_FREQ_113M      (6<<0)
#define SYS_CTRL_BB_FREQ_104M      (5<<0)
#define SYS_CTRL_BB_FREQ_89M       (4<<0)
#define SYS_CTRL_BB_FREQ_78M       (3<<0)
#define SYS_CTRL_BB_FREQ_52M       (2<<0)
#define SYS_CTRL_BB_FREQ_39M       (1<<0)
#define SYS_CTRL_BB_FREQ_26M       (0<<0)

//Cfg_Clk_AudioBCK_Div
#define SYS_CTRL_AUDIOBCK_DIVIDER(n) (((n)&0x7FF)<<0)
#define SYS_CTRL_AUDIOBCK_UPDATE   (1<<11)

//Cfg_Aif_Tx_Stb_Div
#define SYS_CTRL_AIF_TX_STB_DIV(n) (((n)&0x1FFF)<<0)
#define SYS_CTRL_AIF_TX_STB_UPDATE (1<<13)
#define SYS_CTRL_AIF_TX_STB_SEL(n) (((n)&3)<<14)
#define SYS_CTRL_AIF_TX_STB_SEL_MASK (3<<14)
#define SYS_CTRL_AIF_TX_STB_SEL_SHIFT (14)
#define SYS_CTRL_AIF_TX_STB_SEL_SEL_CODEC_CLK (0<<14)
#define SYS_CTRL_AIF_TX_STB_SEL_SEL_52M_CLK (1<<14)
#define SYS_CTRL_AIF_TX_STB_SEL_SEL_FM_CLK (2<<14)
#define SYS_CTRL_AIF_TX_STB_EN     (1<<16)

//Cfg_Clk_Uart
#define SYS_CTRL_UART_DIVIDER(n)   (((n)&0xFFFFFF)<<0)
#define SYS_CTRL_UART_UPDATE       (1<<24)
#define SYS_CTRL_CFG_UART_NUM(n)   (((n)&0x3FF)<<0)
#define SYS_CTRL_CFG_UART_DENOM(n) (((n)&0x3FFF)<<10)

//Cfg_Clk_PWM
#define SYS_CTRL_PWM_DIVIDER(n)    (((n)&0xFF)<<0)

//Cfg_Clk_Auxclk
#define SYS_CTRL_AUXCLK_EN_DISABLE (0<<0)
#define SYS_CTRL_AUXCLK_EN_ENABLE  (1<<0)

//Cfg_Clk_Camera_Out
#define SYS_CTRL_CLK_CAMERA_OUT_EN_DISABLE (0<<0)
#define SYS_CTRL_CLK_CAMERA_OUT_EN_ENABLE (1<<0)
#define SYS_CTRL_CLK_CAM_OUT_MODE_OFF (0<<1)
#define SYS_CTRL_CLK_CAM_OUT_MODE_ON (1<<1)
#define SYS_CTRL_CLK_CAM_OUT_SEL_156MHZ (0<<2)
#define SYS_CTRL_CLK_CAM_OUT_SEL_52MHZ (1<<2)
#define SYS_CTRL_CLK_CAMERA_OUT_DIV(n) (((n)&15)<<4)
#define SYS_CTRL_CLK_CAMERA_OUT_DIV_MASK (15<<4)
#define SYS_CTRL_CLK_CAMERA_OUT_DIV_SHIFT (4)
#define SYS_CTRL_CLK_CAM_OUT_UPDATE (1<<8)
#define SYS_CTRL_CLK_SPI_CAMERA_DIV(n) (((n)&0x7FF)<<16)
#define SYS_CTRL_CLK_SPI_CAM_UPDATE (1<<27)
#define SYS_CTRL_CLK_SPI_CAM_POL   (1<<28)

//Cfg_AHB
#define SYS_CTRL_SYS_NEW_ARBITRATION_ENABLE (1<<0)
#define SYS_CTRL_SYS_NEW_ARBITRATION_DISABLE (0<<0)
#define SYS_CTRL_ENABLE_SYS_MID_BIST_ENABLE (1<<1)
#define SYS_CTRL_ENABLE_SYS_MID_BIST_DISABLE (0<<1)
#define SYS_CTRL_ENABLE_SYS_MID_DMA_ENABLE (1<<2)
#define SYS_CTRL_ENABLE_SYS_MID_DMA_DISABLE (0<<2)
#define SYS_CTRL_ENABLE_SYS_MID_XCPU_ENABLE (1<<3)
#define SYS_CTRL_ENABLE_SYS_MID_XCPU_DISABLE (0<<3)
#define SYS_CTRL_ENABLE_SYS_MID_AHB2AHB_ENABLE (1<<4)
#define SYS_CTRL_ENABLE_SYS_MID_AHB2AHB_DISABLE (0<<4)
#define SYS_CTRL_ENABLE_SYS_MID_IFC_ENABLE (1<<5)
#define SYS_CTRL_ENABLE_SYS_MID_IFC_DISABLE (0<<5)
#define SYS_CTRL_ENABLE_SYS_MID_USB_ENABLE (1<<6)
#define SYS_CTRL_ENABLE_SYS_MID_USB_DISABLE (0<<6)
#define SYS_CTRL_ENABLE_SYS_MID_GOUDA_ENABLE (1<<7)
#define SYS_CTRL_ENABLE_SYS_MID_GOUDA_DISABLE (0<<7)
#define SYS_CTRL_ENABLE_SYS_MID_VOC_ENABLE (1<<8)
#define SYS_CTRL_ENABLE_SYS_MID_VOC_DISABLE (0<<8)
#define SYS_CTRL_ENABLE_SYS_MID_IFC2_ENABLE (1<<9)
#define SYS_CTRL_ENABLE_SYS_MID_IFC2_DISABLE (0<<9)
#define SYS_CTRL_SEL_GOUDA_AHB_MEMB_AHB (1<<15)
#define SYS_CTRL_SEL_GOUDA_AHB_MEMB_MEM_BRIDGE (0<<15)
#define SYS_CTRL_BB_NEW_ARBITRATION_ENABLE (1<<16)
#define SYS_CTRL_BB_NEW_ARBITRATION_DISABLE (0<<16)
#define SYS_CTRL_ENABLE_BB_MID_BIST_ENABLE (1<<17)
#define SYS_CTRL_ENABLE_BB_MID_BIST_DISABLE (0<<17)
#define SYS_CTRL_ENABLE_BB_MID_IFC_ENABLE (1<<18)
#define SYS_CTRL_ENABLE_BB_MID_IFC_DISABLE (0<<18)
#define SYS_CTRL_ENABLE_BB_MID_VOC_ENABLE (1<<19)
#define SYS_CTRL_ENABLE_BB_MID_VOC_DISABLE (0<<19)
#define SYS_CTRL_ENABLE_BB_MID_BCPU_ENABLE (1<<20)
#define SYS_CTRL_ENABLE_BB_MID_BCPU_DISABLE (0<<20)
#define SYS_CTRL_ENABLE_BB_MID_AHB2AHB_ENABLE (1<<21)
#define SYS_CTRL_ENABLE_BB_MID_AHB2AHB_DISABLE (0<<21)
#define SYS_CTRL_USB_RAM_TEST_MODE_ENABLE (1<<31)
#define SYS_CTRL_USB_RAM_TEST_MODE_DISABLE (0<<31)
#define SYS_CTRL_SYS_ENABLE(n)     (((n)&0x1FF)<<1)
#define SYS_CTRL_SYS_ENABLE_MASK   (0x1FF<<1)
#define SYS_CTRL_SYS_ENABLE_SHIFT  (1)
#define SYS_CTRL_BB_ENABLE(n)      (((n)&31)<<17)
#define SYS_CTRL_BB_ENABLE_MASK    (31<<17)
#define SYS_CTRL_BB_ENABLE_SHIFT   (17)

//Ctrl_AHB
#define SYS_CTRL_SPLIT_SYS_MID_BIST_NORMAL (1<<0)
#define SYS_CTRL_SPLIT_SYS_MID_BIST_FORCE (0<<0)
#define SYS_CTRL_SPLIT_SYS_MID_DMA_NORMAL (1<<1)
#define SYS_CTRL_SPLIT_SYS_MID_DMA_FORCE (0<<1)
#define SYS_CTRL_SPLIT_SYS_MID_XCPU_NORMAL (1<<2)
#define SYS_CTRL_SPLIT_SYS_MID_XCPU_FORCE (0<<2)
#define SYS_CTRL_SPLIT_SYS_MID_AHB2AHB_NORMAL (1<<3)
#define SYS_CTRL_SPLIT_SYS_MID_AHB2AHB_FORCE (0<<3)
#define SYS_CTRL_SPLIT_SYS_MID_IFC_NORMAL (1<<4)
#define SYS_CTRL_SPLIT_SYS_MID_IFC_FORCE (0<<4)
#define SYS_CTRL_SPLIT_SYS_MID_USB_NORMAL (1<<5)
#define SYS_CTRL_SPLIT_SYS_MID_USB_FORCE (0<<5)
#define SYS_CTRL_SPLIT_SYS_MID_GOUDA_NORMAL (1<<6)
#define SYS_CTRL_SPLIT_SYS_MID_GOUDA_FORCE (0<<6)
#define SYS_CTRL_SPLIT_SYS_MID_VOC_NORMAL (1<<7)
#define SYS_CTRL_SPLIT_SYS_MID_VOC_FORCE (0<<7)
#define SYS_CTRL_SPLIT_SYS_MID_IFC2_NORMAL (1<<8)
#define SYS_CTRL_SPLIT_SYS_MID_IFC2_FORCE (0<<8)
#define SYS_CTRL_SPLIT_BB_MID_BIST_NORMAL (1<<16)
#define SYS_CTRL_SPLIT_BB_MID_BIST_FORCE (0<<16)
#define SYS_CTRL_SPLIT_BB_MID_IFC_NORMAL (1<<17)
#define SYS_CTRL_SPLIT_BB_MID_IFC_FORCE (0<<17)
#define SYS_CTRL_SPLIT_BB_MID_VOC_NORMAL (1<<18)
#define SYS_CTRL_SPLIT_BB_MID_VOC_FORCE (0<<18)
#define SYS_CTRL_SPLIT_BB_MID_BCPU_NORMAL (1<<19)
#define SYS_CTRL_SPLIT_BB_MID_BCPU_FORCE (0<<19)
#define SYS_CTRL_SPLIT_BB_MID_AHB2AHB_NORMAL (1<<20)
#define SYS_CTRL_SPLIT_BB_MID_AHB2AHB_FORCE (0<<20)
#define SYS_CTRL_SYS_FORCE_HSPLIT(n) (((n)&0x1FF)<<0)
#define SYS_CTRL_SYS_FORCE_HSPLIT_MASK (0x1FF<<0)
#define SYS_CTRL_SYS_FORCE_HSPLIT_SHIFT (0)
#define SYS_CTRL_BB_FORCE_HSPLIT(n) (((n)&31)<<16)
#define SYS_CTRL_BB_FORCE_HSPLIT_MASK (31<<16)
#define SYS_CTRL_BB_FORCE_HSPLIT_SHIFT (16)

//XCpu_Dbg_BKP
#define SYS_CTRL_BKPT_EN           (1<<0)
#define SYS_CTRL_BKPT_MODE(n)      (((n)&3)<<4)
#define SYS_CTRL_BKPT_MODE_I       (0<<4)
#define SYS_CTRL_BKPT_MODE_R       (1<<4)
#define SYS_CTRL_BKPT_MODE_W       (2<<4)
#define SYS_CTRL_BKPT_MODE_RW      (3<<4)
#define SYS_CTRL_STALLED           (1<<8)

//XCpu_Dbg_Addr
#define SYS_CTRL_BREAKPOINT_ADDRESS(n) (((n)&0x3FFFFFF)<<0)

//BCpu_Dbg_BKP
//#define SYS_CTRL_BKPT_EN         (1<<0)
//#define SYS_CTRL_BKPT_MODE(n)    (((n)&3)<<4)
//#define SYS_CTRL_BKPT_MODE_I     (0<<4)
//#define SYS_CTRL_BKPT_MODE_R     (1<<4)
//#define SYS_CTRL_BKPT_MODE_W     (2<<4)
//#define SYS_CTRL_BKPT_MODE_RW    (3<<4)
//#define SYS_CTRL_STALLED         (1<<8)

//BCpu_Dbg_Addr
//#define SYS_CTRL_BREAKPOINT_ADDRESS(n) (((n)&0x3FFFFFF)<<0)

//Cfg_Cpus_Cache_Ram_Disable
#define SYS_CTRL_XCPU_USE_MODE     (1<<0)
#define SYS_CTRL_XCPU_CLK_OFF_MODE (1<<1)
#define SYS_CTRL_BCPU_USE_MODE     (1<<16)
#define SYS_CTRL_BCPU_CLK_OFF_MODE (1<<17)
#define SYS_CTRL_XCPU_CACHE_RAM_DISABLE(n) (((n)&3)<<0)
#define SYS_CTRL_XCPU_CACHE_RAM_DISABLE_MASK (3<<0)
#define SYS_CTRL_XCPU_CACHE_RAM_DISABLE_SHIFT (0)
#define SYS_CTRL_BCPU_CACHE_RAM_DISABLE(n) (((n)&3)<<16)
#define SYS_CTRL_BCPU_CACHE_RAM_DISABLE_MASK (3<<16)
#define SYS_CTRL_BCPU_CACHE_RAM_DISABLE_SHIFT (16)

//Reset_Cause
#define SYS_CTRL_WATCHDOG_RESET_HAPPENED (1<<0)
#define SYS_CTRL_WATCHDOG_RESET_NO (0<<0)
#define SYS_CTRL_GLOBALSOFT_RESET_HAPPENED (1<<4)
#define SYS_CTRL_GLOBALSOFT_RESET_NO (0<<4)
#define SYS_CTRL_HOSTDEBUG_RESET_HAPPENED (1<<8)
#define SYS_CTRL_HOSTDEBUG_RESET_NO (0<<8)
#define SYS_CTRL_ALARMCAUSE_HAPPENED (1<<12)
#define SYS_CTRL_ALARMCAUSE_NO     (0<<12)
#define SYS_CTRL_BOOT_MODE(n)      (((n)&0x3F)<<16)
#define SYS_CTRL_BOOT_MODE_MASK    (0x3F<<16)
#define SYS_CTRL_BOOT_MODE_SHIFT   (16)
#define SYS_CTRL_SW_BOOT_MODE(n)   (((n)&0x3F)<<22)
#define SYS_CTRL_SW_BOOT_MODE_MASK (0x3F<<22)
#define SYS_CTRL_SW_BOOT_MODE_SHIFT (22)
#define SYS_CTRL_FONCTIONAL_TEST_MODE (1<<31)

//WakeUp
#define SYS_CTRL_FORCE_WAKEUP      (1<<0)

//Ignore_Charger_REG
#define SYS_CTRL_IGNORE_CHARGER    (1<<0)

//Cfg_Clk_SpiFlash
#define SYS_CTRL_SPIFLASH_FREQ(n)  (((n)&15)<<0)
#define SYS_CTRL_SPIFLASH_FREQ_MASK (15<<0)
#define SYS_CTRL_SPIFLASH_FREQ_SHIFT (0)
#define SYS_CTRL_SPIFLASH_FREQ_312M (13<<0)
#define SYS_CTRL_SPIFLASH_FREQ_250M (12<<0)
#define SYS_CTRL_SPIFLASH_FREQ_208M (11<<0)
#define SYS_CTRL_SPIFLASH_FREQ_178M (10<<0)
#define SYS_CTRL_SPIFLASH_FREQ_156M (9<<0)
#define SYS_CTRL_SPIFLASH_FREQ_139M (8<<0)
#define SYS_CTRL_SPIFLASH_FREQ_125M (7<<0)
#define SYS_CTRL_SPIFLASH_FREQ_113M (6<<0)
#define SYS_CTRL_SPIFLASH_FREQ_104M (5<<0)
#define SYS_CTRL_SPIFLASH_FREQ_89M (4<<0)
#define SYS_CTRL_SPIFLASH_FREQ_78M (3<<0)
#define SYS_CTRL_SPIFLASH_FREQ_52M (2<<0)
#define SYS_CTRL_SPIFLASH_FREQ_39M (1<<0)
#define SYS_CTRL_SPIFLASH_FREQ_26M (0<<0)

//Clk_Sys_Enable2
#define SYS_CTRL_ENABLE_SYS_GPIO   (1<<0)
#define SYS_CTRL_ENABLE_SYS_IRQ    (1<<1)
#define SYS_CTRL_ENABLE_SYS_TCU    (1<<2)
#define SYS_CTRL_ENABLE_SYS_TIMER  (1<<3)
#define SYS_CTRL_ENABLE_CLK_SYS_MSB(n) (((n)&15)<<0)
#define SYS_CTRL_ENABLE_CLK_SYS_MSB_MASK (15<<0)
#define SYS_CTRL_ENABLE_CLK_SYS_MSB_SHIFT (0)

//Clk_Sys_Disable2
#define SYS_CTRL_DISABLE_SYS_GPIO  (1<<0)
#define SYS_CTRL_DISABLE_SYS_IRQ   (1<<1)
#define SYS_CTRL_DISABLE_SYS_TCU   (1<<2)
#define SYS_CTRL_DISABLE_SYS_TIMER (1<<3)
#define SYS_CTRL_DISABLE_CLK_SYS_MSB(n) (((n)&15)<<0)
#define SYS_CTRL_DISABLE_CLK_SYS_MSB_MASK (15<<0)
#define SYS_CTRL_DISABLE_CLK_SYS_MSB_SHIFT (0)

//CFG_XTAL_DIV
#define SYS_CTRL_CFG_XTAL_DIV(n)   (((n)&31)<<0)
#define SYS_CTRL_CFG_XTAL_DIV_MASK (31<<0)
#define SYS_CTRL_CFG_XTAL_DIV_SHIFT (0)
#define SYS_CTRL_CFG_XTAL_DIV_UPDATE (1<<5)

//Cfg_Rsd
#define SYS_CTRL_RESERVE(n)        (((n)&0xFFFF)<<0)





#endif

