#ifndef __XM510_H__
#define __XM510_H__
/*
 * COPYRIGHT (C) 2013-2014, Shanghai Real-Thread Technology Co., Ltd
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#define XM510_GPIO_BASE       		0x10020000
#define XM510_UART0_BASE			0x10030000
#define XM510_UART1_BASE			0x10040000
#define XM510_TIMER01_BASE			0x100C0000
#define XM510_SFC_BASE				0x100F0000
#define XM510_SYS_CTRL_BASE			0x20000000
#define XM510_INTC_BASE				0x20010000
#define XM510_DMAC_BASE				0x20020000
#define XM510_GMAC_BASE				0x20040000
#define XM510_SDIO0_BASE      		0x50000000
#define XM510_SDIO1_BASE      		0x50010000

#define IRQ_XM510_START		0
#define IRQ_XM510_END		32

#define IRQ_XM510_UART0		(0)
#define IRQ_XM510_UART1		(1)
#define IRQ_XM510_TIMER01   (4)    
#define IRQ_XM510_TIMER23   (5)
#define IRQ_XM510_GMAC		(3)
#define IRQ_XM510_DMAC		(10)
#define IRQ_XM510_SDIO0		(24)
#define IRQ_XM510_SDIO1		(25)


#define TIMER0_LOAD              (XM510_TIMER01_BASE + 0x00)
#define TIMER0_VALUE             (XM510_TIMER01_BASE + 0x04)
#define TIMER0_CTRL              (XM510_TIMER01_BASE + 0x08)
#define TIMER0_INTCLR            (XM510_TIMER01_BASE + 0x0c)
#define TIMER0_RIS               (XM510_TIMER01_BASE + 0x10)
#define TIMER0_MIS               (XM510_TIMER01_BASE + 0x14)
#define TIMER0_BGLOAD            (XM510_TIMER01_BASE + 0x18)
#define TIMER1_LOAD              (XM510_TIMER01_BASE + 0x20)
#define TIMER1_VALUE             (XM510_TIMER01_BASE + 0x24)
#define TIMER1_CTRL              (XM510_TIMER01_BASE + 0x28)
#define TIMER1_INTCLR            (XM510_TIMER01_BASE + 0x2c)
#define TIMER1_RIS               (XM510_TIMER01_BASE + 0x30)
#define TIMER1_MIS               (XM510_TIMER01_BASE + 0x34)
#define TIMER1_BGLOAD            (XM510_TIMER01_BASE + 0x38)
#define TIMER_CTRL_ONESHOT      (1 << 0)
#define TIMER_CTRL_32BIT        (1 << 1)
#define TIMER_CTRL_DIV1         (0 << 2)
#define TIMER_CTRL_DIV16        (1 << 2)
#define TIMER_CTRL_DIV256       (2 << 2)
#define TIMER_CTRL_IE           (1 << 5)        /* Interrupt Enable (versatile only) */
#define TIMER_CTRL_PERIODIC     (1 << 6)
#define TIMER_CTRL_ENABLE       (1 << 7)

#define INTC_IRQSTATUS			(XM510_INTC_BASE + 0x000)
#define INTC_FIQSTATUS			(XM510_INTC_BASE + 0x004)
#define INTC_RAWSTATUS			(XM510_INTC_BASE + 0x008)
#define INTC_INTSELECT			(XM510_INTC_BASE + 0x00C)
#define INTC_INTENABLE			(XM510_INTC_BASE + 0x010)
#define INTC_INTENCLEAR			(XM510_INTC_BASE + 0x014)
#define INTC_SOFTINT			(XM510_INTC_BASE + 0x018)
#define INTC_SOFTINTCLEAR		(XM510_INTC_BASE + 0x01C)

#define XM_SFC_ADDR          	(XM510_SFC_BASE + 0x004)  
#define XM_SFC_OP            	(XM510_SFC_BASE + 0x008)  
#define XM_SFC_CMD           	(XM510_SFC_BASE + 0x00C)  
#define XM_SFC_SRW           	(XM510_SFC_BASE + 0x010)  
#define XM_SFC_WRNUM         	(XM510_SFC_BASE + 0x014)  
#define XM_SFC_RW_SR_BSEL    	(XM510_SFC_BASE + 0x018)  
#define XM_SFC_DOUT          	(XM510_SFC_BASE + 0x034)  
#define XM_SFC_ST            	(XM510_SFC_BASE + 0x038)  
#define XM_SFC_SRR           	(XM510_SFC_BASE + 0x03C)  
#define XM_SFC_CACHE_CMD		(XM510_SFC_BASE + 0x080) 
#define XM_SFC_BUF           	(XM510_SFC_BASE + 0x400)  
#define XM_SFC_BUF_SIZE      	256

#define XM_SYS_SOFT_RSTEN           0x20000030
#define XM_SYS_SOFT_RST             0x20000034

#define GPIO5_MULT_USE_EN 		(XM510_GPIO_BASE + 0x14)
#define GPIO5_IO				(XM510_GPIO_BASE + 0x90)
#define GPIO5_OUT_EN			(XM510_GPIO_BASE + 0x94)
#define GPIO5_OUT_REG			(XM510_GPIO_BASE + 0x98)

#define GPIO6_MULT_USE_EN 		(XM510_GPIO_BASE + 0x18)
#define GPIO6_IO				(XM510_GPIO_BASE + 0xA0)
#define GPIO6_OUT_EN			(XM510_GPIO_BASE + 0xA4)
#define GPIO6_OUT_REG			(XM510_GPIO_BASE + 0xA8)

#endif
