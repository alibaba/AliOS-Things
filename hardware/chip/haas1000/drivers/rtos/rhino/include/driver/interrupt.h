/*
 * Allwinnertech rtos interrupt header file.
 * Copyright (C) 2019  Allwinnertech Co., Ltd. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */


#ifndef __GIC_H
#define __GIC_H

#define GIC_DIST_BASE       GIC_DISTRIBUTOR_BASE
#define GIC_CPUIF_BASE      GIC_INTERFACE_BASE

/* GIC registers */
#define GIC_DIST_CON		(GIC_DIST_BASE + 0x0000)
#define GIC_CON_TYPE		(GIC_DIST_BASE + 0x0004)
#define GIC_CON_IIDR		(GIC_DIST_BASE + 0x0008)

#define GIC_CON_IGRP		(GIC_DIST_BASE + 0x0080)

#define GIC_SET_EN(_n)		(GIC_DIST_BASE + 0x100 + 4 * (_n))
#define GIC_SET_EN0			GIC_SET_EN(0)	// 0x100
#define GIC_SET_EN1			GIC_SET_EN(1)   // 0x104
#define GIC_SET_EN2			GIC_SET_EN(2)   // 0x108
#define GIC_SET_EN3			GIC_SET_EN(3)   // 0x10c
#define GIC_SET_EN4			GIC_SET_EN(4)   // 0x110

#define GIC_CLR_EN(_n)		(GIC_DIST_BASE + 0x180 + 4 * (_n))
#define GIC_CLR_EN0			GIC_CLR_EN(0)	// 0x180
#define GIC_CLR_EN1			GIC_CLR_EN(1)   // 0x184
#define GIC_CLR_EN2			GIC_CLR_EN(2)   // 0x188
#define GIC_CLR_EN3			GIC_CLR_EN(3)   // 0x18c
#define GIC_CLR_EN4			GIC_CLR_EN(4)   // 0x190

#define GIC_PEND_SET(_n)	(GIC_DIST_BASE + 0x200 + 4 * (_n))
#define GIC_PEND_SET0		GIC_PEND_SET(0)	// 0x200
#define GIC_PEND_SET1		GIC_PEND_SET(1) // 0x204
#define GIC_PEND_SET2		GIC_PEND_SET(2) // 0x208
#define GIC_PEND_SET3		GIC_PEND_SET(3) // 0x20c
#define GIC_PEND_SET4		GIC_PEND_SET(4) // 0x210

#define GIC_PEND_CLR(_n)	(GIC_DIST_BASE + 0x280 + 4 * (_n))
#define GIC_PEND_CLR0		GIC_PEND_CLR(0)	// 0x280
#define GIC_PEND_CLR1		GIC_PEND_CLR(1) // 0x284
#define GIC_PEND_CLR2		GIC_PEND_CLR(2) // 0x288
#define GIC_PEND_CLR3		GIC_PEND_CLR(3) // 0x28c
#define GIC_PEND_CLR4		GIC_PEND_CLR(4) // 0x290

#define GIC_ACT_SET(_n)		(GIC_DIST_BASE + 0x300 + 4 * (_n))
#define GIC_ACT_SET0		GIC_ACT_SET(0)	// 0x300
#define GIC_ACT_SET1		GIC_ACT_SET(1)  // 0x304
#define GIC_ACT_SET2		GIC_ACT_SET(2)  // 0x308
#define GIC_ACT_SET3		GIC_ACT_SET(3)  // 0x30c
#define GIC_ACT_SET4		GIC_ACT_SET(4)  // 0x310

#define GIC_ACT_CLR(_n)		(GIC_DIST_BASE + 0x380 + 4 * (_n))
#define GIC_ACT_CLR0		GIC_ACT_CLR(0)	// 0x380
#define GIC_ACT_CLR1		GIC_ACT_CLR(1)  // 0x384
#define GIC_ACT_CLR2		GIC_ACT_CLR(2)  // 0x388
#define GIC_ACT_CLR3		GIC_ACT_CLR(3)  // 0x38c
#define GIC_ACT_CLR4		GIC_ACT_CLR(4)  // 0x390

#define GIC_SGI_PRIO(_n)	(GIC_DIST_BASE + 0x400 + 4 * (_n))
#define GIC_SGI_PRIO0		GIC_SGI_PRIO(0)	// 0x400
#define GIC_SGI_PRIO1		GIC_SGI_PRIO(1) // 0x404
#define GIC_SGI_PRIO2		GIC_SGI_PRIO(2) // 0x408
#define GIC_SGI_PRIO3		GIC_SGI_PRIO(3) // 0x40C

#define GIC_PPI_PRIO(_n)	(GIC_DIST_BASE + 0x410 + 4 * (_n))
#define GIC_PPI_PRIO0		GIC_PPI_PRIO(0)	// 0x410
#define GIC_PPI_PRIO1		GIC_PPI_PRIO(1) // 0x414
#define GIC_PPI_PRIO2		GIC_PPI_PRIO(2) // 0x418
#define GIC_PPI_PRIO3		GIC_PPI_PRIO(3) // 0x41C

#define GIC_SPI_PRIO(_n)	(GIC_DIST_BASE + 0x420 + 4 * (_n))
#define GIC_SPI_PRIO0		GIC_SPI_PRIO(0 )	// 0x420
#define GIC_SPI_PRIO1 		GIC_SPI_PRIO(1 )    // 0x424
#define GIC_SPI_PRIO2 		GIC_SPI_PRIO(2 )    // 0x428
#define GIC_SPI_PRIO3 		GIC_SPI_PRIO(3 )    // 0x42C
#define GIC_SPI_PRIO4 		GIC_SPI_PRIO(4 )    // 0x430
#define GIC_SPI_PRIO5 		GIC_SPI_PRIO(5 )    // 0x434
#define GIC_SPI_PRIO6 		GIC_SPI_PRIO(6 )    // 0x438
#define GIC_SPI_PRIO7 		GIC_SPI_PRIO(7 )    // 0x43C
#define GIC_SPI_PRIO8 		GIC_SPI_PRIO(8 )    // 0x440
#define GIC_SPI_PRIO9 		GIC_SPI_PRIO(9 )    // 0x444
#define GIC_SPI_PRIO10		GIC_SPI_PRIO(10)    // 0x448
#define GIC_SPI_PRIO11		GIC_SPI_PRIO(11)    // 0x44C
#define GIC_SPI_PRIO12		GIC_SPI_PRIO(12)    // 0x450
#define GIC_SPI_PRIO13		GIC_SPI_PRIO(13)    // 0x454
#define GIC_SPI_PRIO14		GIC_SPI_PRIO(14)    // 0x458
#define GIC_SPI_PRIO15		GIC_SPI_PRIO(15)    // 0x45C
#define GIC_SPI_PRIO16		GIC_SPI_PRIO(16)    // 0x460
#define GIC_SPI_PRIO17		GIC_SPI_PRIO(17)    // 0x464
#define GIC_SPI_PRIO18		GIC_SPI_PRIO(18)    // 0x468
#define GIC_SPI_PRIO19		GIC_SPI_PRIO(19)    // 0x46C
#define GIC_SPI_PRIO20		GIC_SPI_PRIO(20)    // 0x470
#define GIC_SPI_PRIO21		GIC_SPI_PRIO(21)    // 0x474
#define GIC_SPI_PRIO22		GIC_SPI_PRIO(22)    // 0x478
#define GIC_SPI_PRIO23		GIC_SPI_PRIO(23)    // 0x47C
#define GIC_SPI_PRIO24		GIC_SPI_PRIO(24)    // 0x480
#define GIC_SPI_PRIO25		GIC_SPI_PRIO(25)    // 0x484
#define GIC_SPI_PRIO26		GIC_SPI_PRIO(26)    // 0x488
#define GIC_SPI_PRIO27		GIC_SPI_PRIO(27)    // 0x48C
#define GIC_SPI_PRIO28		GIC_SPI_PRIO(28)    // 0x490
#define GIC_SPI_PRIO29		GIC_SPI_PRIO(29)    // 0x494
#define GIC_SPI_PRIO30		GIC_SPI_PRIO(30)    // 0x498
#define GIC_SPI_PRIO31		GIC_SPI_PRIO(31)    // 0x49C

#define GIC_SGI_PROC_TARG(_n)	(GIC_DIST_BASE + 0x800 + 4 * (_n))
#define GIC_SGI_PROC_TARG0		GIC_SGI_PROC_TARG(0)	// 0x800
#define GIC_SGI_PROC_TARG1		GIC_SGI_PROC_TARG(1)    // 0x804
#define GIC_SGI_PROC_TARG2		GIC_SGI_PROC_TARG(2)    // 0x808
#define GIC_SGI_PROC_TARG3		GIC_SGI_PROC_TARG(3)    // 0x80C

#define GIC_PPI_PROC_TARG(_n)	(GIC_DIST_BASE + 0x810 + 4 * (_n))
#define GIC_PPI_PROC_TARG0		GIC_PPI_PROC_TARG(0)	// 0x810
#define GIC_PPI_PROC_TARG1		GIC_PPI_PROC_TARG(1)    // 0x814
#define GIC_PPI_PROC_TARG2		GIC_PPI_PROC_TARG(2)    // 0x818
#define GIC_PPI_PROC_TARG3		GIC_PPI_PROC_TARG(3)    // 0x81C

#define GIC_SPI_PROC_TARG(_n)	(GIC_DIST_BASE + 0x820 + 4 * (_n))
#define GIC_SPI_PROC_TARG0 		GIC_SPI_PROC_TARG(0 )	// 0x820
#define GIC_SPI_PROC_TARG1 		GIC_SPI_PROC_TARG(1 )   // 0x824
#define GIC_SPI_PROC_TARG2 		GIC_SPI_PROC_TARG(2 )   // 0x828
#define GIC_SPI_PROC_TARG3 		GIC_SPI_PROC_TARG(3 )   // 0x82C
#define GIC_SPI_PROC_TARG4 		GIC_SPI_PROC_TARG(4 )   // 0x830
#define GIC_SPI_PROC_TARG5 		GIC_SPI_PROC_TARG(5 )   // 0x834
#define GIC_SPI_PROC_TARG6 		GIC_SPI_PROC_TARG(6 )   // 0x838
#define GIC_SPI_PROC_TARG7 		GIC_SPI_PROC_TARG(7 )   // 0x83C
#define GIC_SPI_PROC_TARG8 		GIC_SPI_PROC_TARG(8 )   // 0x840
#define GIC_SPI_PROC_TARG9 		GIC_SPI_PROC_TARG(9 )   // 0x844
#define GIC_SPI_PROC_TARG10		GIC_SPI_PROC_TARG(10)   // 0x848
#define GIC_SPI_PROC_TARG11		GIC_SPI_PROC_TARG(11)   // 0x84C
#define GIC_SPI_PROC_TARG12		GIC_SPI_PROC_TARG(12)   // 0x850
#define GIC_SPI_PROC_TARG13		GIC_SPI_PROC_TARG(13)   // 0x854
#define GIC_SPI_PROC_TARG14		GIC_SPI_PROC_TARG(14)   // 0x858
#define GIC_SPI_PROC_TARG15		GIC_SPI_PROC_TARG(15)   // 0x85C
#define GIC_SPI_PROC_TARG16		GIC_SPI_PROC_TARG(16)   // 0x860
#define GIC_SPI_PROC_TARG17		GIC_SPI_PROC_TARG(17)   // 0x864
#define GIC_SPI_PROC_TARG18		GIC_SPI_PROC_TARG(18)   // 0x868
#define GIC_SPI_PROC_TARG19		GIC_SPI_PROC_TARG(19)   // 0x86C
#define GIC_SPI_PROC_TARG20		GIC_SPI_PROC_TARG(20)   // 0x870
#define GIC_SPI_PROC_TARG21		GIC_SPI_PROC_TARG(21)   // 0x874
#define GIC_SPI_PROC_TARG22		GIC_SPI_PROC_TARG(22)   // 0x878
#define GIC_SPI_PROC_TARG23		GIC_SPI_PROC_TARG(23)   // 0x87C
#define GIC_SPI_PROC_TARG24		GIC_SPI_PROC_TARG(24)   // 0x880
#define GIC_SPI_PROC_TARG25		GIC_SPI_PROC_TARG(25)   // 0x884
#define GIC_SPI_PROC_TARG26		GIC_SPI_PROC_TARG(26)   // 0x888
#define GIC_SPI_PROC_TARG27		GIC_SPI_PROC_TARG(27)   // 0x88C
#define GIC_SPI_PROC_TARG28		GIC_SPI_PROC_TARG(28)   // 0x890
#define GIC_SPI_PROC_TARG29		GIC_SPI_PROC_TARG(29)   // 0x894
#define GIC_SPI_PROC_TARG30		GIC_SPI_PROC_TARG(30)   // 0x898
#define GIC_SPI_PROC_TARG31		GIC_SPI_PROC_TARG(31)   // 0x89C

#define GIC_IRQ_MOD_CFG(_n)		(GIC_DIST_BASE + 0xc00 + 4 * (_n))
#define GIC_IRQ_MOD_CFG0		GIC_IRQ_MOD_CFG(0)		// 0xc00 - SGI
#define GIC_IRQ_MOD_CFG1		GIC_IRQ_MOD_CFG(1)  	// 0xc04 - PPI
#define GIC_IRQ_MOD_CFG2		GIC_IRQ_MOD_CFG(2)  	// 0xc08 - SPI0 ~ 15
#define GIC_IRQ_MOD_CFG3		GIC_IRQ_MOD_CFG(3)  	// 0xc0C - SPI16 ~ 31
#define GIC_IRQ_MOD_CFG4		GIC_IRQ_MOD_CFG(4)  	// 0xc10 - SPI32 ~ 47
#define GIC_IRQ_MOD_CFG5		GIC_IRQ_MOD_CFG(5)  	// 0xc14 - SPI48 ~ 63
#define GIC_IRQ_MOD_CFG6		GIC_IRQ_MOD_CFG(6)  	// 0xc18 - SPI64 ~ 79
#define GIC_IRQ_MOD_CFG7		GIC_IRQ_MOD_CFG(7)  	// 0xc1C - SPI80 ~ 95
#define GIC_IRQ_MOD_CFG8		GIC_IRQ_MOD_CFG(8)  	// 0xc20 - SPI96 ~ 111
#define GIC_IRQ_MOD_CFG9		GIC_IRQ_MOD_CFG(9)  	// 0xc24 - SPI112 ~ 127

#define GIC_SOFT_IRQ_GEN		(GIC_DIST_BASE + 0xf00)	// 0xf00
#define GIC_SGI_PEND_CLR(_n)	(GIC_DIST_BASE + 0xf10 + 4 * (_n))
#define GIC_SGI_PEND_CLR0		GIC_SGI_PEND_CLR(0)		// 0xf10
#define GIC_SGI_PEND_CLR1		GIC_SGI_PEND_CLR(1)		// 0xf14
#define GIC_SGI_PEND_CLR2		GIC_SGI_PEND_CLR(2)		// 0xf18
#define GIC_SGI_PEND_CLR3		GIC_SGI_PEND_CLR(3)		// 0xf1C
#define GIC_SGI_PEND_SET(_n)	(GIC_DIST_BASE + 0xf20 + 4 * (_n))
#define GIC_SGI_PEND_SET0		GIC_SGI_PEND_SET(0)		// 0xf20
#define GIC_SGI_PEND_SET1		GIC_SGI_PEND_SET(1)		// 0xf24
#define GIC_SGI_PEND_SET2		GIC_SGI_PEND_SET(2)		// 0xf28
#define GIC_SGI_PEND_SET3		GIC_SGI_PEND_SET(3)		// 0xf2C


#define GIC_CPU_IF_CTRL			(GIC_CPUIF_BASE + 0x000)	// 0x000
#define GIC_INT_PRIO_MASK		(GIC_CPUIF_BASE + 0x004) // 0x004
#define GIC_BINARY_POINT		(GIC_CPUIF_BASE + 0x008) // 0x008
#define GIC_INT_ACK_REG			(GIC_CPUIF_BASE + 0x00c) // 0x00c
#define GIC_END_INT_REG			(GIC_CPUIF_BASE + 0x010) // 0x010
#define GIC_RUNNING_PRIO		(GIC_CPUIF_BASE + 0x014) // 0x014
#define GIC_HIGHEST_PENDINT		(GIC_CPUIF_BASE + 0x018) // 0x018

#define GIC_AIAR_REG			(GIC_CPUIF_BASE + 0x020) // 0x8020
#define GIC_AEOI_REG			(GIC_CPUIF_BASE + 0x024) // 0x8024

#define GIC_DEACT_INT_REG		(GIC_CPUIF_BASE + 0x1000)// 0x1000

/* gic source list */
/* software generated interrupt */
#define GIC_SRC_SGI(_n)		(_n)
#define GIC_SRC_SGI0		GIC_SRC_SGI(0 )  // (0 )
#define GIC_SRC_SGI1        GIC_SRC_SGI(1 )  // (1 )
#define GIC_SRC_SGI2        GIC_SRC_SGI(2 )  // (2 )
#define GIC_SRC_SGI3        GIC_SRC_SGI(3 )  // (3 )
#define GIC_SRC_SGI4        GIC_SRC_SGI(4 )  // (4 )
#define GIC_SRC_SGI5        GIC_SRC_SGI(5 )  // (5 )
#define GIC_SRC_SGI6        GIC_SRC_SGI(6 )  // (6 )
#define GIC_SRC_SGI7        GIC_SRC_SGI(7 )  // (7 )
#define GIC_SRC_SGI8        GIC_SRC_SGI(8 )  // (8 )
#define GIC_SRC_SGI9        GIC_SRC_SGI(9 )  // (9 )
#define GIC_SRC_SGI10       GIC_SRC_SGI(10)  // (10)
#define GIC_SRC_SGI11       GIC_SRC_SGI(11)  // (11)
#define GIC_SRC_SGI12       GIC_SRC_SGI(12)  // (12)
#define GIC_SRC_SGI13       GIC_SRC_SGI(13)  // (13)
#define GIC_SRC_SGI14       GIC_SRC_SGI(14)  // (14)
#define GIC_SRC_SGI15       GIC_SRC_SGI(15)  // (15)
/* private peripheral interrupt */
#define GIC_SRC_PPI(_n)		(16 + (_n))
#define GIC_SRC_PPI0		GIC_SRC_PPI(0 )  // (16)
#define GIC_SRC_PPI1        GIC_SRC_PPI(1 )  // (17)
#define GIC_SRC_PPI2        GIC_SRC_PPI(2 )  // (18)
#define GIC_SRC_PPI3        GIC_SRC_PPI(3 )  // (19)
#define GIC_SRC_PPI4        GIC_SRC_PPI(4 )  // (20)
#define GIC_SRC_PPI5        GIC_SRC_PPI(5 )  // (21)
#define GIC_SRC_PPI6        GIC_SRC_PPI(6 )  // (22)
#define GIC_SRC_PPI7        GIC_SRC_PPI(7 )  // (23)
#define GIC_SRC_PPI8        GIC_SRC_PPI(8 )  // (24)
#define GIC_SRC_PPI9        GIC_SRC_PPI(9 )  // (25)
#define GIC_SRC_PPI10       GIC_SRC_PPI(10)  // (26)
#define GIC_SRC_PPI11       GIC_SRC_PPI(11)  // (27)
#define GIC_SRC_PPI12       GIC_SRC_PPI(12)  // (28)
#define GIC_SRC_PPI13       GIC_SRC_PPI(13)  // (29)
#define GIC_SRC_PPI14       GIC_SRC_PPI(14)  // (30)
#define GIC_SRC_PPI15       GIC_SRC_PPI(15)  // (31)
/* external peripheral interrupt */
#define GIC_SRC_SPI(_n)		(32 + (_n))

#define GIC_SRC_UART0		GIC_SRC_SPI(0  ) // (32)
#define GIC_SRC_UART1		GIC_SRC_SPI(1  ) // (33)
#define GIC_SRC_UART2       GIC_SRC_SPI(2  ) // (34)
//#define        			GIC_SRC_SPI(3  ) // (35)
//#define        			GIC_SRC_SPI(4  ) // (36)
//#define        			GIC_SRC_SPI(5  ) // (37)
#define GIC_SRC_TWI0        GIC_SRC_SPI(6  ) // (38)
#define GIC_SRC_TWI1        GIC_SRC_SPI(7  ) // (39)
//#define         			GIC_SRC_SPI(8  ) // (40)
//#define         			GIC_SRC_SPI(9  ) // (41)
//#define 					GIC_SRC_SPI(10 ) // (42)
//#define       			GIC_SRC_SPI(11 ) // (43)
//#define 					GIC_SRC_SPI(12 ) // (44)
#define GIC_SRC_IIS	    	GIC_SRC_SPI(13 ) // (45)
//#define 					GIC_SRC_SPI(14 ) // (46)
#define GIC_SRC_PBEINT    	GIC_SRC_SPI(15 ) // (47)
//#define 					GIC_SRC_SPI(16 ) // (48)
#define GIC_SRC_PGEINT      GIC_SRC_SPI(17 ) // (49)
#define GIC_SRC_TIMER0      GIC_SRC_SPI(18 ) // (50)
#define GIC_SRC_TIMER1      GIC_SRC_SPI(19 ) // (51)
#define GIC_SRC_TIMER2		GIC_SRC_SPI(20 ) // (52)
//#define 					GIC_SRC_SPI(21 ) // (53)
//#define 					GIC_SRC_SPI(22 ) // (54)
//#define 					GIC_SRC_SPI(23 ) // (55)
//#define 					GIC_SRC_SPI(24 ) // (56)
#define GIC_SRC_WATCHDOG	GIC_SRC_SPI(25 ) // (57)
//#define				    GIC_SRC_SPI(26 ) // (58)
//#define     				GIC_SRC_SPI(27 ) // (59)
//#define           		GIC_SRC_SPI(28 ) // (60)
#define GIC_SRC_CODEC		GIC_SRC_SPI(29 ) // (61)
#define GIC_SRC_KEYADC      GIC_SRC_SPI(30 ) // (62)
//#define       			GIC_SRC_SPI(31 ) // (63)
#define GIC_SRC_NMI         GIC_SRC_SPI(32 ) // (64)
//#define      				GIC_SRC_SPI(33 ) // (65)
//#define      				GIC_SRC_SPI(34 ) // (66)
//#define 				    GIC_SRC_SPI(35 ) // (67)
//#define    				GIC_SRC_SPI(36 ) // (68)
//#define      				GIC_SRC_SPI(37 ) // (69)
//#define        			GIC_SRC_SPI(38 ) // (70)
//#define         			GIC_SRC_SPI(39 ) // (71)
#define GIC_SRC_ALM0    	GIC_SRC_SPI(40 ) // (72)
#define GIC_SRC_ALM1     	GIC_SRC_SPI(41 ) // (73)
//#define      				GIC_SRC_SPI(42 ) // (74)
//#define       			GIC_SRC_SPI(43 ) // (75)
//#define         			GIC_SRC_SPI(44 ) // (76)
//#define      				GIC_SRC_SPI(45 ) // (77)
//#define         			GIC_SRC_SPI(46 ) // (78)
//#define 	   				GIC_SRC_SPI(47 ) // (79)
//#define 					GIC_SRC_SPI(48 ) // (80)
//#define         			GIC_SRC_SPI(49 ) // (81)
#define GIC_SRC_DMA         GIC_SRC_SPI(50 ) // (82)
#define GIC_SRC_HSTMR0      GIC_SRC_SPI(51 ) // (83)
#define GIC_SRC_HSTMR1      GIC_SRC_SPI(52 ) // (84)
//#define       			GIC_SRC_SPI(53 ) // (85)
//#define       			GIC_SRC_SPI(54 ) // (86)
//#define       			GIC_SRC_SPI(55 ) // (87)
//#define        			GIC_SRC_SPI(56 ) // (88)
//#define        			GIC_SRC_SPI(57 ) // (89)
#define GIC_SRC_VE          GIC_SRC_SPI(58 ) // (90)
//#define       			GIC_SRC_SPI(59 ) // (91)
#define GIC_SRC_MMC0        GIC_SRC_SPI(60 ) // (92)
#define GIC_SRC_MMC1        GIC_SRC_SPI(74 ) // (93)
#define GIC_SRC_MMC2        GIC_SRC_SPI(62 ) // (94)
//#define         			GIC_SRC_SPI(63 ) // (95)
//#define         			GIC_SRC_SPI(63 ) // (96)
#define GIC_SRC_SPI0        GIC_SRC_SPI(65 ) // (97 )
//#define         			GIC_SRC_SPI(66 ) // (98 )
//#define         			GIC_SRC_SPI(67 ) // (99 )
//#define         			GIC_SRC_SPI(68 ) // (100)
//#define        			GIC_SRC_SPI(69 ) // (101)
//#define         			GIC_SRC_SPI(70 ) // (102)
#define GIC_SRC_USBOTG      GIC_SRC_SPI(71 ) // (103)
#define GIC_SRC_USBEHCI  	GIC_SRC_SPI(72 ) // (104)
#define GIC_SRC_USBOHCI  	GIC_SRC_SPI(73 ) // (105)
//#define     				GIC_SRC_SPI(74 ) // (106)
//#define     				GIC_SRC_SPI(75 ) // (107)
//#define    				GIC_SRC_SPI(76 ) // (108)
//#define     				GIC_SRC_SPI(77 ) // (109)
//#define     				GIC_SRC_SPI(78 ) // (110)
//#define     				GIC_SRC_SPI(79 ) // (111)
#define GIC_SRC_SS         	GIC_SRC_SPI(80 ) // (112)
//#define           		GIC_SRC_SPI(81 ) // (113)
//#define        			GIC_SRC_SPI(82 ) // (114)
#define GIC_SRC_CSI0		GIC_SRC_SPI(83 ) // (115)
#define GIC_SRC_CSI1        GIC_SRC_SPI(84 ) // (116)
#define GIC_SRC_CSICCI		GIC_SRC_SPI(85 ) // (117)
#define GIC_SRC_LCD         GIC_SRC_SPI(86 ) // (118)
//#define         			GIC_SRC_SPI(87 ) // (119)
//#define       			GIC_SRC_SPI(88 ) // (120)
//#define      				GIC_SRC_SPI(89 ) // (121)
#define GIC_SRC_MIPICSI    	GIC_SRC_SPI(90 ) // (122)
//#define        			GIC_SRC_SPI(91 ) // (123)
//#define        			GIC_SRC_SPI(92 ) // (124)
//#define          			GIC_SRC_SPI(93 ) // (125)
//#define        			GIC_SRC_SPI(94 ) // (126)
#define GIC_SRC_DE          GIC_SRC_SPI(95 ) // (127)
//#define                   GIC_SRC_SPI(96 ) // (128)

// #define GIC_IRQ_NUM			140
 #define GIC_IRQ_NUM			96

/* processer target */
#define GIC_CPU_TARGET(_n)	(1 << (_n))
#define GIC_CPU_TARGET0		GIC_CPU_TARGET(0)
#define GIC_CPU_TARGET1		GIC_CPU_TARGET(1)
#define GIC_CPU_TARGET2		GIC_CPU_TARGET(2)
#define GIC_CPU_TARGET3		GIC_CPU_TARGET(3)
#define GIC_CPU_TARGET4		GIC_CPU_TARGET(4)
#define GIC_CPU_TARGET5		GIC_CPU_TARGET(5)
#define GIC_CPU_TARGET6		GIC_CPU_TARGET(6)
#define GIC_CPU_TARGET7		GIC_CPU_TARGET(7)
/* trigger mode */
#define GIC_SPI_LEVEL_TRIGGER	(0)	//2b'00
#define GIC_SPI_EDGE_TRIGGER	(2)	//2b'10

#ifndef s32
typedef long s32;
typedef unsigned long u32;
#endif

//typedef void (*pIRQ_Handler)(void);
typedef void (interrupt_handler_t)(int, void *);
s32 irq_request(u32 irq_no, interrupt_handler_t  *hdle, void *data);
s32 irq_free(u32 irq_no);
s32 irq_enable(u32 irq_no);
s32 irq_disable(u32 irq_no);
void irq_set_nmi(u32 irq_no);

void init_gic(void);
void gic_irq_handler(void);
s32 gic_test(void);
s32 set_irq_target(u32 irq_no, u32 target);

void gic_send_sgi(u32 irqID, u32 target_list, u32 filter_list);
void gic_set_irq_priority(u32 ID, u32 priority);
void gic_set_irq_group(u32 irq_no, u32 group);

#endif

