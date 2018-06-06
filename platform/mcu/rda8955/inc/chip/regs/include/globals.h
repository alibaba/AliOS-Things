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


#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'globals'."
#endif



// =============================================================================
//  MACROS
// =============================================================================
/// AHB Address bus size
#define NB_BITS_ADDR                            (26)
/// The System SRam size
#define SYS_SRAM_ADDR_WIDTH                     (13)
#define SYS_SRAM_SIZE                           (EXP2(SYS_SRAM_ADDR_WIDTH+2))
/// System Ahb Bus Configuration
#define SYS_NB_BITS_MASTER                      (4)

// ============================================================================
// SYS_MASTER_ID_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    SYS_MID_DUMMY                               = 0x00000000,
    SYS_MID_BIST                                = 0x00000001,
    SYS_MID_DMA                                 = 0x00000002,
    SYS_MID_XCPU                                = 0x00000003,
    SYS_MID_AHB2AHB                             = 0x00000004,
    SYS_MID_IFC                                 = 0x00000005,
    SYS_MID_USB                                 = 0x00000006,
    SYS_MID_GOUDA                               = 0x00000007,
    SYS_MID_VOC                                 = 0x00000008,
    SYS_MID_IFC2                                = 0x00000009
} SYS_MASTER_ID_T;

/// After this starts the Round Robib Pool
#define SYS_MID_RR_START                        (2)
#define SYS_MID_RR_END                          (10)
#define SYS_NB_STATIC_MASTER                    (SYS_MID_RR_START-1)
#define SYS_NB_MASTERS                          (SYS_MID_RR_END-1)
#define SYS_NB_BITS_SLAVE                       (4)

// ============================================================================
// SYS_SLAVE_ID_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    SYS_SID_DUMMY                               = 0x00000000,
    SYS_SID_MEM_BRIDGE                          = 0x00000001,
    SYS_SID_AHB2AHB                             = 0x00000002,
    SYS_SID_IFC                                 = 0x00000003,
    SYS_SID_USB                                 = 0x00000004,
    SYS_SID_GOUDA                               = 0x00000005,
    SYS_SID_XCPU_RAM                            = 0x00000006,
    SYS_SID_SPI_FLASH                           = 0x00000007,
    SYS_SID_VOC                                 = 0x00000008,
    SYS_SID_MPMC                                = 0x00000009,
    SYS_SID_MPMCREG                             = 0x0000000A,
    SYS_SID_MAILBOX                             = 0x0000000B,
    SYS_SID_IFC2                                = 0x0000000C
} SYS_SLAVE_ID_T;

#define SYS_SID_END                             (13)
#define SYS_NB_SLAVES                           (SYS_SID_END-1)
/// System Apb Bus Configuration
#define SYS_NB_BITS_PSEL                        (6)
#define SYS_NB_BITS_PADDR                       (12)
#define SYS_APB_STEP                            (EXP2(SYS_NB_BITS_PADDR))

// ============================================================================
// SYS_MODULE_ID_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    SYS_ID_SYS_CTRL                             = 0x00000000,
    SYS_ID_IRQ                                  = 0x00000001,
    SYS_ID_TIMER                                = 0x00000002,
    SYS_ID_GPIO                                 = 0x00000003,
    SYS_ID_EBC                                  = 0x00000004,
    SYS_ID_KEYPAD                               = 0x00000005,
    SYS_ID_PWM                                  = 0x00000006,
    SYS_ID_I2C                                  = 0x00000007,
    SYS_ID_DMA                                  = 0x00000008,
    SYS_ID_IFC                                  = 0x00000009,
    SYS_ID_CALENDAR                             = 0x0000000A,
    SYS_ID_COMREGS                              = 0x0000000B,
    SYS_ID_PAGE_SPY                             = 0x0000000C,
    SYS_ID_SEG_SCAN                             = 0x0000000D,
    SYS_ID_RF_SPI                               = 0x0000000E,
    SYS_ID_TCU                                  = 0x0000000F,
/// The following modules are linked to ifc dma req with 2 requests per module
    SYS_ID_SCI                                  = 0x00000010,
    SYS_ID_SPI1                                 = 0x00000011,
    SYS_ID_SPI2                                 = 0x00000012,
    SYS_ID_SPI3                                 = 0x00000013,
    SYS_ID_DEBUG_UART                           = 0x00000014,
    SYS_ID_UART                                 = 0x00000015,
    SYS_ID_UART2                                = 0x00000016,
    SYS_ID_SDMMC                                = 0x00000017,
    SYS_ID_CAMERA                               = 0x00000018,
    SYS_ID_SDMMC2                               = 0x00000019,
    SYS_ID_XCPU_TAG                             = 0x0000001A,
    SYS_ID_XCPU_IDATA                           = 0x0000001B,
    SYS_ID_XCPU_DDATA                           = 0x0000001C,
    SYS_ID_SYS_AHBC_MON                         = 0x0000001D,
    SYS_ID_BB_AHBC_MON                          = 0x0000001E,
    SYS_ID_BIST                                 = 0x0000001F,
    SYS_ID_IFC2                                 = 0x00000020,
    SYS_ID_GOUDA                                = 0x00000021,
    SYS_ID_I2C2                                 = 0x00000022,
    SYS_ID_I2C3                                 = 0x00000023,
    SYS_ID_EXT_APB                              = 0x00000024,
    SYS_ID_SPIFLASH                             = 0x00000025,
    SYS_ID_NF                                   = 0x00000026,
    SYS_ID_NF1                                  = 0x00000027,
    SYS_ID_NF2                                  = 0x00000028,
    SYS_ID_NF3                                  = 0x00000029,
    SYS_ID_IOMUX                                = 0x0000002A,
    SYS_ID_XCPU_REG                             = 0x0000002B
} SYS_MODULE_ID_T;

#define FIRST_SYS_ID_DATA                       (14)
#define LAST_SYS_ID_DATA                        (25)
#define SYS_NB_PSEL                             (44)
/// The debug host is placed at last PSEL63 in the IFC
#define SYS_ID_DEBUG_HOST                       (EXP2(SYS_NB_BITS_PSEL)-1)

// ============================================================================
// SYS_IFC_REQUEST_IDS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    SYS_ID_TX_SCI                               = 0x00000000,
    SYS_ID_RX_SCI                               = 0x00000001,
    SYS_ID_TX_SPI1                              = 0x00000002,
    SYS_ID_RX_SPI1                              = 0x00000003,
    SYS_ID_TX_SPI2                              = 0x00000004,
    SYS_ID_RX_SPI2                              = 0x00000005,
    SYS_ID_TX_SPI3                              = 0x00000006,
    SYS_ID_RX_SPI3                              = 0x00000007,
    SYS_ID_TX_DEBUG_UART                        = 0x00000008,
    SYS_ID_RX_DEBUG_UART                        = 0x00000009,
    SYS_ID_TX_UART                              = 0x0000000A,
    SYS_ID_RX_UART                              = 0x0000000B,
    SYS_ID_TX_UART2                             = 0x0000000C,
    SYS_ID_RX_UART2                             = 0x0000000D,
    SYS_ID_TX_SDMMC                             = 0x0000000E,
    SYS_ID_RX_SDMMC                             = 0x0000000F,
    SYS_ID_TX_FREE                              = 0x00000010,
    SYS_ID_RX_CAMERA                            = 0x00000011,
    SYS_ID_TX_SDMMC2                            = 0x00000012,
    SYS_ID_RX_SDMMC2                            = 0x00000013
} SYS_IFC_REQUEST_IDS_T;

#define SYS_NB_DMA_REQ                          (20)
#define SYS_NB_DMA_REQ_WIDTH                    (5)

// ============================================================================
// SYS_IRQ_ID_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
/// System IRQ IDs
/// Pulse IRQ
    SYS_IRQ_TCU0                                = 0x00000000,
    SYS_IRQ_TCU1                                = 0x00000001,
    SYS_IRQ_FRAME                               = 0x00000002,
/// Level IRQ
    SYS_IRQ_COM0                                = 0x00000003,
    SYS_IRQ_COM1                                = 0x00000004,
    SYS_IRQ_VOC                                 = 0x00000005,
    SYS_IRQ_DMA                                 = 0x00000006,
    SYS_IRQ_GPIO                                = 0x00000007,
    SYS_IRQ_KEYPAD                              = 0x00000008,
    SYS_IRQ_TIMERS                              = 0x00000009,
    SYS_IRQ_OS_TIMER                            = 0x0000000A,
    SYS_IRQ_CALENDAR                            = 0x0000000B,
    SYS_IRQ_SPI1                                = 0x0000000C,
    SYS_IRQ_SPI2                                = 0x0000000D,
    SYS_IRQ_SPI3                                = 0x0000000E,
    SYS_IRQ_DEBUG_UART                          = 0x0000000F,
    SYS_IRQ_UART                                = 0x00000010,
    SYS_IRQ_UART2                               = 0x00000011,
    SYS_IRQ_I2C                                 = 0x00000012,
    SYS_IRQ_I2C2                                = 0x00000013,
    SYS_IRQ_I2C3                                = 0x00000014,
    SYS_IRQ_SCI                                 = 0x00000015,
    SYS_IRQ_RF_SPI                              = 0x00000016,
    SYS_IRQ_LPS                                 = 0x00000017,
    SYS_IRQ_BBIFC0                              = 0x00000018,
    SYS_IRQ_BBIFC1                              = 0x00000019,
    SYS_IRQ_USBC                                = 0x0000001A,
    SYS_IRQ_GOUDA                               = 0x0000001B,
    SYS_IRQ_SDMMC                               = 0x0000001C,
    SYS_IRQ_CAMERA                              = 0x0000001D,
    SYS_IRQ_PMU                                 = 0x0000001E,
    SYS_IRQ_SDMMC2                              = 0x0000001F
} SYS_IRQ_ID_T;

/// Number of Pulse IRQ
#define SYS_NB_IRQ_PULSE                        (3)
/// Number of IRQ handled by the production driver
#define SYS_IRQ_QTY                             (32)
/// Number of IRQ
#define SYS_NB_IRQ                              (32)
/// Number of Level IRQ
#define SYS_NB_IRQ_LEVEL                        (SYS_NB_IRQ - SYS_NB_IRQ_PULSE)
/// The Baseband SRam size
#define BB_SRAM_ADDR_WIDTH                      (13)
#define BB_SRAM_SIZE                            (24*1024)
/// Baseband Ahb Bus Configuration
#define BB_NB_BITS_MASTER                       (3)

// ============================================================================
// BB_MASTER_ID_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    BB_MID_DUMMY                                = 0x00000000,
    BB_MID_BIST                                 = 0x00000001,
    BB_MID_IFC                                  = 0x00000002,
    BB_MID_VOC                                  = 0x00000003,
    BB_MID_BCPU                                 = 0x00000004,
    BB_MID_AHB2AHB                              = 0x00000005
} BB_MASTER_ID_T;

/// After this starts the Round Robib Pool
#define BB_MID_RR_START                         (3)
#define BB_MID_RR_END                           (6)
#define BB_NB_STATIC_MASTER                     (BB_MID_RR_START-1)
#define BB_NB_MASTERS                           (BB_MID_RR_END-1)
#define BB_MAX_BURST_SIZE                       (4)
#define BB_MAX_BURST_BITS                       (2)
#define BB_NB_BITS_SLAVE                        (3)

// ============================================================================
// BB_SLAVE_ID_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    BB_SID_DUMMY                                = 0x00000000,
    BB_SID_MEM_BRIDGE                           = 0x00000001,
    BB_SID_AHB2AHB                              = 0x00000002,
    BB_SID_IFC                                  = 0x00000003,
    BB_SID_ROM                                  = 0x00000004,
    BB_SID_SRAM                                 = 0x00000005,
    BB_SID_MAILBOX                              = 0x00000006
} BB_SLAVE_ID_T;

#define BB_SID_END                              (7)
#define BB_NB_SLAVES                            (BB_SID_END-1)
/// BaseBand Apb Bus Configuration
#define BB_NB_BITS_PSEL                         (5)
#define BB_NB_BITS_PADDR                        (12)
#define BB_APB_STEP                             (EXP2(BB_NB_BITS_PADDR))

// ============================================================================
// BB_MODULE_ID_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    BB_ID_XCOR                                  = 0x00000000,
    BB_ID_IFC                                   = 0x00000001,
    BB_ID_IRQ                                   = 0x00000002,
    BB_ID_ITLV                                  = 0x00000003,
    BB_ID_VITERBI                               = 0x00000004,
    BB_ID_A5                                    = 0x00000005,
    BB_ID_RF                                    = 0x00000006,
    BB_ID_AIF                                   = 0x00000007,
    BB_ID_CP2                                   = 0x00000008,
    BB_ID_BIST                                  = 0x00000009,
    BB_ID_BCPU_REG                              = 0x0000000A,
    BB_ID_BCPU_TAG                              = 0x0000000B,
    BB_ID_BCPU_IDATA                            = 0x0000000C,
    BB_ID_BCPU_DDATA                            = 0x0000000D,
    BB_ID_COMREGS                               = 0x0000000E,
    BB_ID_EXCOR                                 = 0x0000000F,
    BB_ID_CHOLK                                 = 0x00000010,
    BB_ID_CORDIC                                = 0x00000011,
    BB_ID_MEMBRIDGE                             = 0x00000012
} BB_MODULE_ID_T;

/// The maximum number of slaves is currently 32.
#define BB_NB_PSEL                              (19)

// ============================================================================
// BB_IRQ_ID_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
/// Baseband IRQ IDs
/// Pulse IRQ
    BB_IRQ_TCU0                                 = 0x00000000,
    BB_IRQ_TCU1                                 = 0x00000001,
    BB_IRQ_FRAME                                = 0x00000002,
/// Level IRQ
    BB_IRQ_COM0                                 = 0x00000003,
    BB_IRQ_COM1                                 = 0x00000004,
    BB_IRQ_VOC                                  = 0x00000005,
    BB_IRQ_IFC0                                 = 0x00000006,
    BB_IRQ_IFC1                                 = 0x00000007,
    BB_IRQ_IFC2                                 = 0x00000008,
    BB_IRQ_IFC3                                 = 0x00000009,
    BB_IRQ_RF_IF                                = 0x0000000A,
    BB_IRQ_ITLV                                 = 0x0000000B,
    BB_IRQ_VITAC                                = 0x0000000C,
    BB_IRQ_XCOR                                 = 0x0000000D
} BB_IRQ_ID_T;

/// Number of Pulse IRQ
#define BB_NB_IRQ_PULSE                         (3)
/// Number of IRQ
#define BB_NB_IRQ                               (14)
/// Number of Level IRQ
#define BB_NB_IRQ_LEVEL                         (BB_NB_IRQ - BB_NB_IRQ_PULSE)
/// RF Input samples size (signed)
#define BB_SYMBOL_SIZE                          (13)
/// Vitac Multipliers size
#define VITAC_MULT_SIZE                         (14)
/// Total bits numbers of signals to spy on the SYS side
#define NB_DBG_SYS_SPY                          (11)
/// Total bits numbers of signals to spy on the BB side
#define NB_DBG_BB_SPY                           (28)

// =============================================================================
//  TYPES
// =============================================================================



    /// XHALT macro will send the event 0x4a17 to the debug host and
    /// will stall the XCPU.  The XCPU can be released from Coolwatcher
    /// by issuing a xrbp command.
    #define XHALT { \
        asm("nop "); \
        asm("nop "); \
        asm("nop "); \
        asm("nop "); \
        while (hwp_debugHost->event != DEBUG_HOST_EVENT0_SEMA); \
        hwp_debugHost->event = 0x4a17; \
        hwp_sysCtrl->XCpu_Dbg_BKP |= SYS_CTRL_STALLED; \
        asm("nop "); \
        asm("nop "); \
        asm("nop "); \
        asm("nop "); \
    }
  

#endif

