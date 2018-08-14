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


#ifndef _GLOBALS_ASM_H_
#define _GLOBALS_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'globals'."
#endif


/// AHB Address bus size
#define NB_BITS_ADDR                            (26)
/// The System SRam size
#define SYS_SRAM_ADDR_WIDTH                     (13)
#define SYS_SRAM_SIZE                           (EXP2(SYS_SRAM_ADDR_WIDTH+2))
/// System Ahb Bus Configuration
#define SYS_NB_BITS_MASTER                      (4)
#define SYS_MID_DUMMY                           (0)
#define SYS_MID_BIST                            (1)
/// After this starts the Round Robib Pool
#define SYS_MID_RR_START                        (2)
#define SYS_MID_DMA                             (2)
#define SYS_MID_XCPU                            (3)
#define SYS_MID_AHB2AHB                         (4)
#define SYS_MID_IFC                             (5)
#define SYS_MID_USB                             (6)
#define SYS_MID_GOUDA                           (7)
#define SYS_MID_VOC                             (8)
#define SYS_MID_IFC2                            (9)
#define SYS_MID_RR_END                          (10)
#define SYS_NB_STATIC_MASTER                    (SYS_MID_RR_START-1)
#define SYS_NB_MASTERS                          (SYS_MID_RR_END-1)
#define SYS_NB_BITS_SLAVE                       (4)
#define SYS_SID_DUMMY                           (0)
#define SYS_SID_MEM_BRIDGE                      (1)
#define SYS_SID_AHB2AHB                         (2)
#define SYS_SID_IFC                             (3)
#define SYS_SID_USB                             (4)
#define SYS_SID_GOUDA                           (5)
#define SYS_SID_XCPU_RAM                        (6)
#define SYS_SID_SPI_FLASH                       (7)
#define SYS_SID_VOC                             (8)
#define SYS_SID_MPMC                            (9)
#define SYS_SID_MPMCREG                         (10)
#define SYS_SID_MAILBOX                         (11)
#define SYS_SID_IFC2                            (12)
#define SYS_SID_END                             (13)
#define SYS_NB_SLAVES                           (SYS_SID_END-1)
/// System Apb Bus Configuration
#define SYS_NB_BITS_PSEL                        (6)
#define SYS_NB_BITS_PADDR                       (12)
#define SYS_APB_STEP                            (EXP2(SYS_NB_BITS_PADDR))
#define SYS_ID_SYS_CTRL                         (0)
#define SYS_ID_IRQ                              (1)
#define SYS_ID_TIMER                            (2)
#define SYS_ID_GPIO                             (3)
#define SYS_ID_EBC                              (4)
#define SYS_ID_KEYPAD                           (5)
#define SYS_ID_PWM                              (6)
#define SYS_ID_I2C                              (7)
#define SYS_ID_DMA                              (8)
#define SYS_ID_IFC                              (9)
#define SYS_ID_CALENDAR                         (10)
#define SYS_ID_COMREGS                          (11)
#define SYS_ID_PAGE_SPY                         (12)
#define SYS_ID_SEG_SCAN                         (13)
#define FIRST_SYS_ID_DATA                       (14)
#define SYS_ID_RF_SPI                           (14)
#define SYS_ID_TCU                              (15)
/// The following modules are linked to ifc dma req with 2 requests per module
#define SYS_ID_SCI                              (16)
#define SYS_ID_SPI1                             (17)
#define SYS_ID_SPI2                             (18)
#define SYS_ID_SPI3                             (19)
#define SYS_ID_DEBUG_UART                       (20)
#define SYS_ID_UART                             (21)
#define SYS_ID_UART2                            (22)
#define SYS_ID_SDMMC                            (23)
#define SYS_ID_CAMERA                           (24)
#define LAST_SYS_ID_DATA                        (25)
#define SYS_ID_SDMMC2                           (25)
#define SYS_ID_XCPU_TAG                         (26)
#define SYS_ID_XCPU_IDATA                       (27)
#define SYS_ID_XCPU_DDATA                       (28)
#define SYS_ID_SYS_AHBC_MON                     (29)
#define SYS_ID_BB_AHBC_MON                      (30)
#define SYS_ID_BIST                             (31)
#define SYS_ID_IFC2                             (32)
#define SYS_ID_GOUDA                            (33)
#define SYS_ID_I2C2                             (34)
#define SYS_ID_I2C3                             (35)
#define SYS_ID_EXT_APB                          (36)
#define SYS_ID_SPIFLASH                         (37)
#define SYS_ID_NF                               (38)
#define SYS_ID_NF1                              (39)
#define SYS_ID_NF2                              (40)
#define SYS_ID_NF3                              (41)
#define SYS_ID_IOMUX                            (42)
#define SYS_ID_XCPU_REG                         (43)
#define SYS_NB_PSEL                             (44)
/// The debug host is placed at last PSEL63 in the IFC
#define SYS_ID_DEBUG_HOST                       (EXP2(SYS_NB_BITS_PSEL)-1)
#define SYS_ID_TX_SCI                           (0)
#define SYS_ID_RX_SCI                           (1)
#define SYS_ID_TX_SPI1                          (2)
#define SYS_ID_RX_SPI1                          (3)
#define SYS_ID_TX_SPI2                          (4)
#define SYS_ID_RX_SPI2                          (5)
#define SYS_ID_TX_SPI3                          (6)
#define SYS_ID_RX_SPI3                          (7)
#define SYS_ID_TX_DEBUG_UART                    (8)
#define SYS_ID_RX_DEBUG_UART                    (9)
#define SYS_ID_TX_UART                          (10)
#define SYS_ID_RX_UART                          (11)
#define SYS_ID_TX_UART2                         (12)
#define SYS_ID_RX_UART2                         (13)
#define SYS_ID_TX_SDMMC                         (14)
#define SYS_ID_RX_SDMMC                         (15)
#define SYS_ID_TX_FREE                          (16)
#define SYS_ID_RX_CAMERA                        (17)
#define SYS_ID_TX_SDMMC2                        (18)
#define SYS_ID_RX_SDMMC2                        (19)
#define SYS_NB_DMA_REQ                          (20)
#define SYS_NB_DMA_REQ_WIDTH                    (5)
/// System IRQ IDs
/// Pulse IRQ
#define SYS_IRQ_TCU0                            (0)
#define SYS_IRQ_TCU1                            (1)
#define SYS_IRQ_FRAME                           (2)
/// Number of Pulse IRQ
#define SYS_NB_IRQ_PULSE                        (3)
/// Level IRQ
#define SYS_IRQ_COM0                            (3)
#define SYS_IRQ_COM1                            (4)
#define SYS_IRQ_VOC                             (5)
#define SYS_IRQ_DMA                             (6)
#define SYS_IRQ_GPIO                            (7)
#define SYS_IRQ_KEYPAD                          (8)
#define SYS_IRQ_TIMERS                          (9)
#define SYS_IRQ_OS_TIMER                        (10)
#define SYS_IRQ_CALENDAR                        (11)
#define SYS_IRQ_SPI1                            (12)
#define SYS_IRQ_SPI2                            (13)
#define SYS_IRQ_SPI3                            (14)
#define SYS_IRQ_DEBUG_UART                      (15)
#define SYS_IRQ_UART                            (16)
#define SYS_IRQ_UART2                           (17)
#define SYS_IRQ_I2C                             (18)
#define SYS_IRQ_I2C2                            (19)
#define SYS_IRQ_I2C3                            (20)
#define SYS_IRQ_SCI                             (21)
#define SYS_IRQ_RF_SPI                          (22)
#define SYS_IRQ_LPS                             (23)
#define SYS_IRQ_BBIFC0                          (24)
#define SYS_IRQ_BBIFC1                          (25)
#define SYS_IRQ_USBC                            (26)
#define SYS_IRQ_GOUDA                           (27)
#define SYS_IRQ_SDMMC                           (28)
#define SYS_IRQ_CAMERA                          (29)
#define SYS_IRQ_PMU                             (30)
#define SYS_IRQ_SDMMC2                          (31)
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
#define BB_MID_DUMMY                            (0)
#define BB_MID_BIST                             (1)
#define BB_MID_IFC                              (2)
/// After this starts the Round Robib Pool
#define BB_MID_RR_START                         (3)
#define BB_MID_VOC                              (3)
#define BB_MID_BCPU                             (4)
#define BB_MID_AHB2AHB                          (5)
#define BB_MID_RR_END                           (6)
#define BB_NB_STATIC_MASTER                     (BB_MID_RR_START-1)
#define BB_NB_MASTERS                           (BB_MID_RR_END-1)
#define BB_MAX_BURST_SIZE                       (4)
#define BB_MAX_BURST_BITS                       (2)
#define BB_NB_BITS_SLAVE                        (3)
#define BB_SID_DUMMY                            (0)
#define BB_SID_MEM_BRIDGE                       (1)
#define BB_SID_AHB2AHB                          (2)
#define BB_SID_IFC                              (3)
#define BB_SID_ROM                              (4)
#define BB_SID_SRAM                             (5)
#define BB_SID_MAILBOX                          (6)
#define BB_SID_END                              (7)
#define BB_NB_SLAVES                            (BB_SID_END-1)
/// BaseBand Apb Bus Configuration
#define BB_NB_BITS_PSEL                         (5)
#define BB_NB_BITS_PADDR                        (12)
#define BB_APB_STEP                             (EXP2(BB_NB_BITS_PADDR))
#define BB_ID_XCOR                              (0)
#define BB_ID_IFC                               (1)
#define BB_ID_IRQ                               (2)
#define BB_ID_ITLV                              (3)
#define BB_ID_VITERBI                           (4)
#define BB_ID_A5                                (5)
#define BB_ID_RF                                (6)
#define BB_ID_AIF                               (7)
#define BB_ID_CP2                               (8)
#define BB_ID_BIST                              (9)
#define BB_ID_BCPU_REG                          (10)
#define BB_ID_BCPU_TAG                          (11)
#define BB_ID_BCPU_IDATA                        (12)
#define BB_ID_BCPU_DDATA                        (13)
#define BB_ID_COMREGS                           (14)
#define BB_ID_EXCOR                             (15)
#define BB_ID_CHOLK                             (16)
#define BB_ID_CORDIC                            (17)
#define BB_ID_MEMBRIDGE                         (18)
/// The maximum number of slaves is currently 32.
#define BB_NB_PSEL                              (19)
/// Baseband IRQ IDs
/// Pulse IRQ
#define BB_IRQ_TCU0                             (0)
#define BB_IRQ_TCU1                             (1)
#define BB_IRQ_FRAME                            (2)
/// Number of Pulse IRQ
#define BB_NB_IRQ_PULSE                         (3)
/// Level IRQ
#define BB_IRQ_COM0                             (3)
#define BB_IRQ_COM1                             (4)
#define BB_IRQ_VOC                              (5)
#define BB_IRQ_IFC0                             (6)
#define BB_IRQ_IFC1                             (7)
#define BB_IRQ_IFC2                             (8)
#define BB_IRQ_IFC3                             (9)
#define BB_IRQ_RF_IF                            (10)
#define BB_IRQ_ITLV                             (11)
#define BB_IRQ_VITAC                            (12)
#define BB_IRQ_XCOR                             (13)
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

//==============================================================================
// Sys_Master_Id
//------------------------------------------------------------------------------
///
//==============================================================================



//==============================================================================
// Sys_slave_Id
//------------------------------------------------------------------------------
///
//==============================================================================



//==============================================================================
// Sys_Module_Id
//------------------------------------------------------------------------------
///
//==============================================================================



//==============================================================================
// Sys_Ifc_Request_IDs
//------------------------------------------------------------------------------
///
//==============================================================================



//==============================================================================
// Sys_Irq_Id
//------------------------------------------------------------------------------
///
//==============================================================================



//==============================================================================
// BB_Master_Id
//------------------------------------------------------------------------------
///
//==============================================================================



//==============================================================================
// BB_slave_Id
//------------------------------------------------------------------------------
///
//==============================================================================



//==============================================================================
// BB_Module_Id
//------------------------------------------------------------------------------
///
//==============================================================================



//==============================================================================
// BB_Irq_Id
//------------------------------------------------------------------------------
///
//==============================================================================




#endif
