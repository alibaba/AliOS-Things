/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2004              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

#ifndef __MUSB_FDRC_DEFS_H__
#define __MUSB_FDRC_DEFS_H__

/*
 * FDRC-specific definitions
 * $Revision: 1.7 $
 */

#include "mu_drcdf.h"

/* Define MUSBFDRC register addresses */

#define MGC_O_FDRC_FADDR      0x00   /* 8 bit */
#define MGC_O_FDRC_POWER      0x01   /* 8 bit */
#define MGC_O_FDRC_INTRTX1    0x02
#define MGC_O_FDRC_INTRTX2    0x03
#define MGC_O_FDRC_INTRRX1    0x04
#define MGC_O_FDRC_INTRRX2    0x05
#define MGC_O_FDRC_INTRUSB    0x06
#define MGC_O_FDRC_INTRTX1E   0x07
#define MGC_O_FDRC_INTRTX2E   0x08
#define MGC_O_FDRC_INTRRX1E   0x09
#define MGC_O_FDRC_INTRRX2E   0x0A
#define MGC_O_FDRC_INTRUSBE   0x0B
#define MGC_O_FDRC_FRAME1     0x0C
#define MGC_O_FDRC_FRAME2     0x0D
#define MGC_O_FDRC_INDEX      0x0E
#define MGC_O_FDRC_DEVCTL     0x0F
#define MGC_O_FDRC_TXMAXP     0x10
#define MGC_O_FDRC_CSR0       0x11
#define MGC_O_FDRC_TXCSR1     0x11
#define	MGC_O_FDRC_CSR02      0x12
#define MGC_O_FDRC_TXCSR2     0x12
#define MGC_O_FDRC_RXMAXP     0x13
#define MGC_O_FDRC_RXCSR1     0x14
#define MGC_O_FDRC_RXCSR2     0x15
#define MGC_O_FDRC_COUNT0     0x16
#define MGC_O_FDRC_RXCOUNT1   0x16
#define MGC_O_FDRC_RXCOUNT2   0x17
#define MGC_O_FDRC_TXTYPE     0x18
#define MGC_O_FDRC_TXINTERVAL 0x19
#define	MGC_O_FDRC_NAKLIMIT0  0x19
#define MGC_O_FDRC_RXTYPE     0x1A
#define MGC_O_FDRC_RXINTERVAL 0x1B

/* Dynamic FIFO sizing: */
#define MGC_O_FDRC_TXFIFO1	0x1C	/* lower 8-bits of 8-byte-aligned address */
#define MGC_O_FDRC_TXFIFO2	0x1D	/* 3-bit size, double-buffer flag, upper 4 address bits */
#define MGC_O_FDRC_RXFIFO1	0x1E	/* lower 8-bits of 8-byte-aligned address */
#define MGC_O_FDRC_RXFIFO2	0x1F	/* 3-bit size, double-buffer flag, upper 4 address bits */

/* No dynamic FIFO sizing: */
#define	MGC_O_FDRC_FIFOSIZE	 0x1F
#define	MGC_O_FDRC_CONFIGDATA	 0x1F

/* CSR02 */
#define MGC_M_CSR02_FLUSHFIFO	0x01

/* TXCSR1 bit masks mode-agnostic */
#define MGC_M_TXCSR1_TXPKTRDY      0x01
#define MGC_M_TXCSR1_FIFONOTEMPTY  0x02
#define MGC_M_TXCSR1_FLUSHFIFO     0x08
#define MGC_M_TXCSR1_CLRDATATOG    0x40

/* TXCSR1 bit masks slave mode */
#define MGC_M_TXCSR1_P_UNDERRUN    0x04
#define MGC_M_TXCSR1_P_SENDSTALL   0x10
#define MGC_M_TXCSR1_P_SENTSTALL   0x20

/* TXCSR1 bit masks MASTER MODE */
#define MGC_M_TXCSR1_H_ERROR       0x04
#define MGC_M_TXCSR1_H_RXSTALL     0x20

/* RXCSR1 bit masks mode-agnostic */
#define MGC_M_RXCSR1_RXPKTRDY      0x01
#define MGC_M_RXCSR1_FIFOFULL      0x02
#define MGC_M_RXCSR1_DATAERR       0x08
#define MGC_M_RXCSR1_FLUSHFIFO     0x10
#define MGC_M_RXCSR1_CLRDATATOG    0x80

/* RXCSR1 bit masks slave mode */
#define MGC_M_RXCSR1_P_OVERRUN     0x04
#define MGC_M_RXCSR1_P_SENDSTALL   0x20
#define MGC_M_RXCSR1_P_SENTSTALL   0x40

/* RXCSR1 bit masks MASTER MODE */
#define MGC_M_RXCSR1_H_ERROR      0x04
#define MGC_M_RXCSR1_H_REQPKT     0x20
#define MGC_M_RXCSR1_H_RXSTALL    0x40

/* TXCSR2 bit masks** THERE IS NO DIFFERENCE FOR MASTER AND SLAVE MODE */
#define MGC_M_TXCSR2_DMAMODE  0x04
#define MGC_M_TXCSR2_FRCDT    0x08
#define MGC_M_TXCSR2_DMAENAB  0x10
#define MGC_M_TXCSR2_MODE     0x20
#define MGC_M_TXCSR2_ISO      0x40
#define MGC_M_TXCSR2_AUTOSET  0x80

/* RXCSR2 bit masks */
#define MGC_M_RXCSR2_DMAMODE   0x10
#define MGC_M_RXCSR2_DMAENAB   0x20
#define MGC_M_RXCSR2_P_ISO     0x40   /* in peripheral mode */
#define MGC_M_RXCSR2_H_AUTOREQ 0x40   /* in host mode */
#define MGC_M_RXCSR2_AUTOCLEAR 0x80

/* TxType/RxType */
#define MGC_M_TYPE_PROTO	0x30
#define MGC_S_TYPE_PROTO	4
#define MGC_M_TYPE_REMOTE_END	0xf

/* For POWER REGISTER */
#define MGC_M_POWER_ENSUSPEND 0x01
#define MGC_M_POWER_SUSPENDM  0x02
#define MGC_M_POWER_RESUME    0x04
#define MGC_M_POWER_RESET     0x08
#define MGC_M_POWER_VBUSLO    0x10
#define MGC_M_POWER_VBUSSESS  0x20
#define MGC_M_POWER_VBUSVAL   0x40
#define MGC_M_POWER_ISOUP     0x80

/* FOR EP0 INTERRUPT in INTRTX1 register */
#define MGC_M_INTR_EP1      0x02

/* dynamic FIFOs */
#define MGC_M_FIFO_DPB	    0x10
#define MGC_M_FIFO_SZ	    0xe0
#define MGC_S_FIFO_SZ	    5

/*
 *  DRC register access macros
 */

/* Get offset for a given FIFO */
#ifdef MUSB_FDRC_AHB
#define MGC_FIFO_OFFSET(_bEnd) (MGC_M_FIFO_EP0 + (_bEnd * 4))
#else
#define MGC_FIFO_OFFSET(_bEnd) (MGC_M_FIFO_EP0 + _bEnd)
#endif

__inline uint16_t MGC_ReadRxCount(MGC_Controller *pController)
{
    uint16_t count = 0;

    count = MGC_Read8((uint8_t *)pController->pControllerAddressIst, MGC_O_FDRC_RXCOUNT2);
    count <<= 8;
    count |= MGC_Read8((uint8_t *)pController->pControllerAddressIst, MGC_O_FDRC_RXCOUNT1);
    return count;
}

#define MGC_VBUS_MASK            0x70    /* Power D6 - D4 */

#endif	/* multiple inclusion protection */
