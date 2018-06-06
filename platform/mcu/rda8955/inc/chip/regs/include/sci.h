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


#ifndef _SCI_H_
#define _SCI_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'sci'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SCI_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_SCI_BASE               0x01A10000

typedef volatile struct
{
    REG32                          Config;                       //0x00000000
    REG32                          Status;                       //0x00000004
    REG32                          Data;                         //0x00000008
    REG32                          ClkDiv;                       //0x0000000C
    REG32                          RxCnt;                        //0x00000010
    REG32                          Times;                        //0x00000014
    REG32                          Ch_Filt;                      //0x00000018
    /// UNDOCUMENTED FEATURE
    REG32                          dbg;                          //0x0000001C
    /// This register is a <b>READ ONLY</b> register that returns the logical
    /// <b>and</b> of the SCI_INT_STATUS register and the SCI_INT_MASK. If any
    /// of these bits is '1', the SCI module will generate an interrupt. Bits
    /// 21:16 return the <u>status</u> of the interrupt which is the interrupt
    /// state before the mask is applied. These bits should only be used for
    /// debugging.
    REG32                          Int_Cause;                    //0x00000020
    /// This is a WRITE ONLY register that is used to clear an SCI interrupt.
    /// Write a '1' to the interrupt that is to be cleared. Writing '0' has no
    /// effect.
    REG32                          Int_Clr;                      //0x00000024
    /// This register is READ/WRITE register that enables the desired interrupt.
    /// A '1' in a bit position indicates that the corresponding interrupt is
    /// enabled and if the interrupt occurs, the SCI will generate a hardware
    /// interrupt.
    REG32                          Int_Mask;                     //0x00000028
} HWP_SCI_T;

#define hwp_sci                    ((HWP_SCI_T*) KSEG1(REG_SCI_BASE))


//Config
#define SCI_ENABLE                 (1<<0)
#define SCI_PARITY_MASK            (1<<1)
#define SCI_PARITY_SHIFT           (1)
#define SCI_PARITY_EVEN_PARITY     (0<<1)
#define SCI_PARITY_ODD_PARITY      (1<<1)
#define SCI_PERF                   (1<<2)
#define SCI_FILTER_DISABLE         (1<<3)
#define SCI_CLOCKSTOP              (1<<4)
#define SCI_AUTOSTOP_EN_H          (1<<5)
#define SCI_MSBH_LSBL              (1<<6)
#define SCI_LLI                    (1<<7)
#define SCI_PEGEN_LEN              (1<<8)
#define SCI_PARITY_EN              (1<<9)
#define SCI_STOP_LEVEL             (1<<10)
#define SCI_ARG_H                  (1<<16)
#define SCI_AFD_EN_H               (1<<17)
#define SCI_TX_RESEND_EN_H         (1<<18)
#define SCI_RESET                  (1<<20)
#define SCI_DLY_SEL                (1<<21)
#define SCI_IN_AVG_EN              (1<<22)
#define SCI_PAR_CHK_OFFSET(n)      (((n)&0x3F)<<24)
#define SCI_SCI_MODE(n)            (((n)&3)<<30)

//Status
#define SCI_RXDATA_RDY             (1<<0)
#define SCI_TX_FIFO_RDY            (1<<1)
#define SCI_FORMAT_DET             (1<<2)
#define SCI_ARG_DET                (1<<3)
#define SCI_RESET_DET              (1<<4)
#define SCI_CLK_RDY_H              (1<<5)
#define SCI_CLK_OFF                (1<<6)
#define SCI_RX_ERR                 (1<<8)
#define SCI_TX_ERR                 (1<<9)
#define SCI_RXOVERFLOW             (1<<10)
#define SCI_TXOVERFLOW             (1<<11)
#define SCI_AUTOSTOP_STATE_MASK    (3<<30)
#define SCI_AUTOSTOP_STATE_SHIFT   (30)
#define SCI_AUTOSTOP_STATE_STARTUP_PHASE (0<<30)
#define SCI_AUTOSTOP_STATE_AUTO_ON (1<<30)
#define SCI_AUTOSTOP_STATE_SHUTDOWN_PHASE (2<<30)
#define SCI_AUTOSTOP_STATE_CLOCK_OFF (3<<30)

//Data
#define SCI_DATA_IN(n)             (((n)&0xFF)<<0)
#define SCI_DATA_OUT(n)            (((n)&0xFF)<<0)

//ClkDiv
#define SCI_CLKDIV(n)              (((n)&0x1FF)<<0)
#define SCI_BAUD_X8_EN             (1<<9)
#define SCI_RX_CLK_CNT_LIMIT(n)    (((n)&31)<<10)
#define SCI_CLK_TST                (1<<15)
#define SCI_CLKDIV_16(n)           (((n)&0xFF)<<16)
#define SCI_MAINDIV(n)             (((n)&0x3F)<<24)
#define SCI_MAINDIV_MASK           (0x3F<<24)
#define SCI_CLK_OUT_INV            (1<<30)
#define SCI_CLK_INV                (1<<31)

//RxCnt
#define SCI_RXCNT(n)               (((n)&0x3FF)<<0)
#define SCI_CLK_PERSIST            (1<<31)

//Times
#define SCI_CHGUARD(n)             (((n)&0xFF)<<0)
#define SCI_CHGUARD_MASK           (0xFF<<0)
#define SCI_CHGUARD_SHIFT          (0)
#define SCI_TURNAROUNDGUARD(n)     (((n)&15)<<8)
#define SCI_TURNAROUNDGUARD_MASK   (15<<8)
#define SCI_TURNAROUNDGUARD_SHIFT  (8)
#define SCI_WI(n)                  (((n)&0xFF)<<16)
#define SCI_WI_MASK                (0xFF<<16)
#define SCI_WI_SHIFT               (16)
#define SCI_TX_PERT(n)             (((n)&0xFF)<<24)

//Ch_Filt
#define SCI_CH_FILT(n)             (((n)&0xFF)<<0)

//dbg
#define SCI_FIFO_RX_CLR            (1<<0)
#define SCI_FIFO_TX_CLR            (1<<1)

//Int_Cause
#define SCI_RX_DONE                (1<<0)
#define SCI_RX_HALF                (1<<1)
#define SCI_WWT_TIMEOUT            (1<<2)
#define SCI_EXTRA_RX               (1<<3)
#define SCI_RESEND_OVFL            (1<<4)
#define SCI_ARG_END                (1<<5)
#define SCI_SCI_DMA_TX_DONE        (1<<6)
#define SCI_SCI_DMA_RX_DONE        (1<<7)

//Int_Clr
//#define SCI_RX_DONE              (1<<0)
//#define SCI_RX_HALF              (1<<1)
//#define SCI_WWT_TIMEOUT          (1<<2)
//#define SCI_EXTRA_RX             (1<<3)
//#define SCI_RESEND_OVFL          (1<<4)
//#define SCI_ARG_END              (1<<5)
//#define SCI_SCI_DMA_TX_DONE      (1<<6)
//#define SCI_SCI_DMA_RX_DONE      (1<<7)

//Int_Mask
//#define SCI_RX_DONE              (1<<0)
//#define SCI_RX_HALF              (1<<1)
//#define SCI_WWT_TIMEOUT          (1<<2)
//#define SCI_EXTRA_RX             (1<<3)
//#define SCI_RESEND_OVFL          (1<<4)
//#define SCI_ARG_END              (1<<5)
//#define SCI_SCI_DMA_TX_DONE      (1<<6)
//#define SCI_SCI_DMA_RX_DONE      (1<<7)





#endif

