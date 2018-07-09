/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_sci_rx.h
* Description  : Functions for using SCI on the RX devices.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           01.10.2016 1.80    Initial Release. (The remake of the r01an1815ju0170 to the base.)
***********************************************************************************************************************/

#ifndef SCI_RX_H
#define SCI_RX_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "../r_sci_rx_if.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Bit position masks */
#define BIT0_MASK   (0x00000001U)
#define BIT1_MASK   (0x00000002U)
#define BIT2_MASK   (0x00000004U)
#define BIT3_MASK   (0x00000008U)
#define BIT4_MASK   (0x00000010U)
#define BIT5_MASK   (0x00000020U)
#define BIT6_MASK   (0x00000040U)
#define BIT7_MASK   (0x00000080U)
#define BIT8_MASK   (0x00000100U)
#define BIT9_MASK   (0x00000200U)
#define BIT10_MASK  (0x00000400U)
#define BIT11_MASK  (0x00000800U)
#define BIT12_MASK  (0x00001000U)
#define BIT13_MASK  (0x00002000U)
#define BIT14_MASK  (0x00004000U)
#define BIT15_MASK  (0x00008000U)
#define BIT16_MASK  (0x00010000U)
#define BIT17_MASK  (0x00020000U)
#define BIT18_MASK  (0x00040000U)
#define BIT19_MASK  (0x00080000U)
#define BIT20_MASK  (0x00100000U)
#define BIT21_MASK  (0x00200000U)
#define BIT22_MASK  (0x00400000U)
#define BIT23_MASK  (0x00800000U)
#define BIT24_MASK  (0x01000000U)
#define BIT25_MASK  (0x02000000U)
#define BIT26_MASK  (0x04000000U)
#define BIT27_MASK  (0x08000000U)
#define BIT28_MASK  (0x10000000U)
#define BIT29_MASK  (0x20000000U)
#define BIT30_MASK  (0x40000000U)
#define BIT31_MASK  (0x80000000U)

#ifndef NULL    /* Resolves e2studio code analyzer false error message. */
    #define NULL (0)
#endif

#if (SCI_CFG_CH10_FIFO_INCLUDED) || (SCI_CFG_CH11_FIFO_INCLUDED)
    #define SCI_CFG_FIFO_INCLUDED (1)
#endif

#if SCI_CFG_FIFO_INCLUDED
#define SCI_SSRFIFO_ORER (hdl->rom->regs->SSRFIFO.BIT.ORER)
#define SCI_SSRFIFO_PER  (hdl->rom->regs->SSRFIFO.BIT.PER)
#define SCI_SSRFIFO_FER  (hdl->rom->regs->SSRFIFO.BIT.FER)
#define SCI_SSRFIFO_RDF  (hdl->rom->regs->SSRFIFO.BIT.RDF)
#define SCI_SSRFIFO      (hdl->rom->regs->SSRFIFO.BYTE)
#endif
#define SCI_SSR_ORER     (hdl->rom->regs->SSR.BIT.ORER)
#define SCI_SSR_PER      (hdl->rom->regs->SSR.BIT.PER)
#define SCI_SSR_FER      (hdl->rom->regs->SSR.BIT.FER)
#define SCI_SSR          (hdl->rom->regs->SSR.BYTE)

#if SCI_CFG_FIFO_INCLUDED
#define SCI_FIFO_FRAME_SIZE (16)
#endif

/* SCR register dummy read */
#define SCI_SCR_DUMMY_READ                \
    if (0x00 == hdl->rom->regs->SCR.BYTE) \
    {                                     \
        nop();                            \
    }

/* Interrupt Request register flag clear */
#define SCI_IR_TXI_CLEAR (*hdl->rom->ir_txi = 0)

/* TDR/FTDR register write access */
#if SCI_CFG_FIFO_INCLUDED
#define SCI_TDR(byte)                       \
    if (true == hdl->fifo_ctrl)             \
    {                                       \
        hdl->rom->regs->FTDR.BYTE.L = byte; \
    }                                       \
    else                                    \
    {                                       \
        hdl->rom->regs->TDR = byte;         \
    }
#else
#define SCI_TDR(byte)                       \
        hdl->rom->regs->TDR = byte;
#endif

/* RDR/FRDR register read access */
#if SCI_CFG_FIFO_INCLUDED
#define SCI_RDR(byte)                       \
    if (true == hdl->fifo_ctrl)             \
    {                                       \
        byte = hdl->rom->regs->FRDR.BYTE.L; \
    }                                       \
    else                                    \
    {                                       \
        byte = hdl->rom->regs->RDR;         \
    }
#else
#define SCI_RDR(byte)                       \
        byte = hdl->rom->regs->RDR;
#endif

/*****************************************************************************
Typedef definitions
******************************************************************************/

/*****************************************************************************
Private global variables and functions
******************************************************************************/
#if (SCI_CFG_ASYNC_INCLUDED)
extern void txi_handler(sci_hdl_t const hdl);
#endif

#if SCI_CFG_TEI_INCLUDED
extern void tei_handler(sci_hdl_t const hdl);
#endif

extern void rxi_handler(sci_hdl_t const hdl);

extern void eri_handler(sci_hdl_t const hdl);

#endif /* SCI_RX_H */

