/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2005              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

#ifndef __MUSB_HSFC_DEFS_H__
#define __MUSB_HSFC_DEFS_H__

/*
 * HSFC-specific definitions
 * $Revision: 1.2 $
 */

#define MGC_MAX_USB_ENDS	16

#define MGC_END0_FIFOSIZE    64      /* this is non-configurable */

#define MGC_M_FIFO_EP0     0x20

/*
 *     MUSBHSFC Register map
 */

/* Common USB registers */

#define MGC_O_HSFC_FADDR	0x00	/* 8-bit */
#define MGC_O_HSFC_POWER	0x01	/* 8-bit */

#define MGC_O_HSFC_INTRIN	0x02	/* 16-bit */
#define MGC_O_HSFC_INTRTX   MGC_O_HSFC_INTRIN
#define MGC_O_HSFC_INTROUT      0x04
#define MGC_O_HSFC_INTRRX   MGC_O_HSFC_INTROUT
#define MGC_O_HSFC_INTRINE      0x06
#define MGC_O_HSFC_INTRTXE  MGC_O_HSFC_INTRINE
#define MGC_O_HSFC_INTROUTE     0x08
#define MGC_O_HSFC_INTRRXE  MGC_O_HSFC_INTROUTE
#define MGC_O_HSFC_INTRUSB      0x0A   /* 8 bit */
#define MGC_O_HSFC_INTRUSBE     0x0B   /* 8 bit */
#define MGC_O_HSFC_FRAME        0x0C
#define MGC_O_HSFC_INDEX        0x0E   /* 8 bit */
#define MGC_O_HSFC_TESTMODE     0x0F   /* 8 bit */

/* These are actually indexed: */
#define MGC_O_HSFC_TXFIFOSZ	0x1a	/* 8-bit (see masks) */
#define MGC_O_HSFC_RXFIFOSZ	0x1b	/* 8-bit (see masks) */
#define MGC_O_HSFC_TXFIFOADD	0x1c	/* 16-bit offset shifted right 3 */
#define MGC_O_HSFC_RXFIFOADD	0x1e	/* 16-bit offset shifted right 3 */

/* Endpoint registers */
#define MGC_O_HSFC_TXMAXP	0x00
#define MGC_O_HSFC_TXCSR	0x02
#define MGC_O_HSFC_CSR0		MGC_O_HSFC_TXCSR	/* re-used for EP0 */
#define MGC_O_HSFC_RXMAXP	0x04
#define MGC_O_HSFC_RXCSR	0x06
#define MGC_O_HSFC_RXCOUNT	0x08
#define MGC_O_HSFC_COUNT0	MGC_O_HSFC_RXCOUNT	/* re-used for EP0 */

/*
 *     MUSBHSFC Register bit masks
 */

/* POWER */

#define MGC_M_POWER_ISOUPDATE   0x80
#define	MGC_M_POWER_SOFTCONN    0x40
#define	MGC_M_POWER_HSENAB	0x20
#define	MGC_M_POWER_HSMODE	0x10
#define MGC_M_POWER_RESET       0x08
#define MGC_M_POWER_RESUME      0x04
#define MGC_M_POWER_SUSPENDM    0x02
#define MGC_M_POWER_ENSUSPEND   0x01

/* Interrupt register bit masks */
#define MGC_M_INTR_SUSPEND    0x01
#define MGC_M_INTR_RESUME     0x02
#define MGC_M_INTR_RESET      0x04
#define MGC_M_INTR_SOF        0x08

/* TESTMODE */

#define MGC_M_TEST_FORCEFS      0x20
#define MGC_M_TEST_FORCEHS      0x10
#define MGC_M_TEST_PACKET       0x08
#define MGC_M_TEST_K            0x04
#define MGC_M_TEST_J            0x02
#define MGC_M_TEST_SE0_NAK      0x01

/* allocate for double-packet buffering (effectively doubles assigned _SIZE) */
#define MGC_M_FIFOSZ_DPB	0x10
/* allocation size (8, 16, 32, ... 4096) */
#define MGC_M_FIFOSZ_SIZE	0x0f

/* CSR0 */

#define MGC_M_CSR0_P_SVDSETUPEND  0x0080
#define MGC_M_CSR0_P_SVDRXPKTRDY  0x0040
#define MGC_M_CSR0_P_SENDSTALL    0x0020
#define MGC_M_CSR0_P_SETUPEND     0x0010
#define MGC_M_CSR0_P_DATAEND      0x0008
#define MGC_M_CSR0_P_SENTSTALL    0x0004
#define MGC_M_CSR0_TXPKTRDY       0x0002
#define MGC_M_CSR0_RXPKTRDY       0x0001

/* TXCSR */

#define MGC_M_TXCSR_AUTOSET       0x8000
#define MGC_M_TXCSR_ISO           0x4000
#define MGC_M_TXCSR_MODE          0x2000
#define MGC_M_TXCSR_DMAENAB       0x1000
#define MGC_M_TXCSR_FRCDATATOG    0x0800
#define MGC_M_TXCSR_P_INCOMPTX    0x0080
#define MGC_M_TXCSR_CLRDATATOG    0x0040
#define MGC_M_TXCSR_P_SENTSTALL   0x0020
#define MGC_M_TXCSR_P_SENDSTALL   0x0010
#define MGC_M_TXCSR_FLUSHFIFO     0x0008
#define MGC_M_TXCSR_P_UNDERRUN    0x0004
#define MGC_M_TXCSR_FIFONOTEMPTY  0x0002
#define MGC_M_TXCSR_TXPKTRDY      0x0001

/* RXCSR */

#define MGC_M_RXCSR_AUTOCLEAR     0x8000
#define MGC_M_RXCSR_P_ISO         0x4000
#define MGC_M_RXCSR_DMAENAB       0x2000
#define MGC_M_RXCSR_DISNYET       0x1000
#define MGC_M_RXCSR_DMAMODE       0x0800
#define MGC_M_RXCSR_INCOMPRX      0x0100
#define MGC_M_RXCSR_CLRDATATOG    0x0080
#define MGC_M_RXCSR_P_SENTSTALL   0x0040
#define MGC_M_RXCSR_P_SENDSTALL   0x0020
#define MGC_M_RXCSR_FLUSHFIFO     0x0010
#define MGC_M_RXCSR_DATAERR       0x0008
#define MGC_M_RXCSR_P_OVERRUN     0x0004
#define MGC_M_RXCSR_FIFOFULL      0x0002
#define MGC_M_RXCSR_RXPKTRDY      0x0001

/*
 *  register access macros
 */

/* Get offset for a given FIFO */
#define MGC_FIFO_OFFSET(_bEnd) (MGC_M_FIFO_EP0 + (_bEnd * 4))

#define MGC_SelectEnd(_pBase, _bEnd) \
    MGC_Write8(_pBase, MGC_O_HSFC_INDEX, _bEnd)
#define MGC_ReadCsr8(_pBase, _bOffset, _bEnd) \
    MGC_Read8(_pBase, (_bOffset + 0x10))
#define MGC_ReadCsr16(_pBase, _bOffset, _bEnd) \
    MGC_Read16(_pBase, (_bOffset + 0x10))
#define MGC_WriteCsr8(_pBase, _bOffset, _bEnd, _bData) \
    MGC_Write8(_pBase, (_bOffset + 0x10), _bData)
#define MGC_WriteCsr16(_pBase, _bOffset, _bEnd, _bData) \
    MGC_Write16(_pBase, (_bOffset + 0x10), _bData)

#endif	/* multiple inclusion protection */
