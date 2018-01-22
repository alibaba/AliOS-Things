/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _EAGLE_SOC_H_
#define _EAGLE_SOC_H_

//Register Bits{{
#define BIT31   0x80000000
#define BIT30   0x40000000
#define BIT29   0x20000000
#define BIT28   0x10000000
#define BIT27   0x08000000
#define BIT26   0x04000000
#define BIT25   0x02000000
#define BIT24   0x01000000
#define BIT23   0x00800000
#define BIT22   0x00400000
#define BIT21   0x00200000
#define BIT20   0x00100000
#define BIT19   0x00080000
#define BIT18   0x00040000
#define BIT17   0x00020000
#define BIT16   0x00010000
#define BIT15   0x00008000
#define BIT14   0x00004000
#define BIT13   0x00002000
#define BIT12   0x00001000
#define BIT11   0x00000800
#define BIT10   0x00000400
#define BIT9    0x00000200
#define BIT8    0x00000100
#define BIT7    0x00000080
#define BIT6    0x00000040
#define BIT5    0x00000020
#define BIT4    0x00000010
#define BIT3    0x00000008
#define BIT2    0x00000004
#define BIT1    0x00000002
#define BIT0    0x00000001
//}}

//Registers Operation {{
#define ETS_UNCACHED_ADDR(addr) (addr)
#define ETS_CACHED_ADDR(addr)   (addr)

#define READ_PERI_REG(addr)                             (*((volatile uint32 *)ETS_UNCACHED_ADDR(addr)))
#define WRITE_PERI_REG(addr, val)                       (*((volatile uint32 *)ETS_UNCACHED_ADDR(addr))) = (uint32)(val)
#define CLEAR_PERI_REG_MASK(reg, mask)                  WRITE_PERI_REG((reg), (READ_PERI_REG(reg) & (~(mask))))
#define SET_PERI_REG_MASK(reg, mask)                    WRITE_PERI_REG((reg), (READ_PERI_REG(reg) | (mask)))
#define GET_PERI_REG_BITS(reg, hipos, lowpos)           ((READ_PERI_REG(reg) >> (lowpos)) & ((1 << ((hipos) - (lowpos) + 1)) - 1))
#define SET_PERI_REG_BITS(reg, bit_map, value, shift)   (WRITE_PERI_REG((reg), (READ_PERI_REG(reg) & (~((bit_map) << (shift)))) | ((value) << (shift)) ))
//}}

//Periheral Clock {{
#define CPU_CLK_FREQ                80 * 1000000       // unit: Hz
#define APB_CLK_FREQ                CPU_CLK_FREQ
#define UART_CLK_FREQ               APB_CLK_FREQ
#define TIMER_CLK_FREQ              (APB_CLK_FREQ >> 8) // divided by 256
//}}

//Peripheral device base address define{{
#define PERIPHS_DPORT_BASEADDR      0x3ff00000
#define PERIPHS_RTC_BASEADDR        0x60000700
//}}

//DPORT{{
#define HOST_INF_SEL                (PERIPHS_DPORT_BASEADDR + 0x28)
#define DPORT_LINK_DEVICE_SEL       0x000000FF
#define DPORT_LINK_DEVICE_SEL_S     8
#define DPORT_PERI_IO_SWAP          0x000000FF
#define DPORT_PERI_IO_SWAP_S        0
#define PERI_IO_CSPI_OVERLAP        (BIT(7)) // two spi masters on cspi
#define PERI_IO_HSPI_OVERLAP        (BIT(6)) // two spi masters on hspi
#define PERI_IO_HSPI_PRIO           (BIT(5)) // hspi is with the higher prior
#define PERI_IO_UART1_PIN_SWAP      (BIT(3)) // swap uart1 pins (u1rxd <-> u1cts), (u1txd <-> u1rts)
#define PERI_IO_UART0_PIN_SWAP      (BIT(2)) // swap uart0 pins (u0rxd <-> u0cts), (u0txd <-> u0rts)
#define PERI_IO_SPI_PORT_SWAP       (BIT(1)) // swap two spi
#define PERI_IO_UART_PORT_SWAP      (BIT(0)) // swap two uart
//}}

//Interrupt remap control registers define{{
#define EDGE_INT_ENABLE_REG         (PERIPHS_DPORT_BASEADDR + 0x04)
#define TM1_EDGE_INT_ENABLE()       SET_PERI_REG_MASK(EDGE_INT_ENABLE_REG, BIT1)
#define TM1_EDGE_INT_DISABLE()      CLEAR_PERI_REG_MASK(EDGE_INT_ENABLE_REG, BIT1)
//}}

//RTC reg {{
#define REG_RTC_BASE                PERIPHS_RTC_BASEADDR

#define RTC_SLP_VAL                     (REG_RTC_BASE + 0x004) // the target value of RTC_COUNTER for wakeup from light-sleep/deep-sleep
#define RTC_SLP_CNT_VAL                 (REG_RTC_BASE + 0x01C) // the current value of RTC_COUNTER

#define RTC_SCRATCH0                    (REG_RTC_BASE + 0x030) // the register for software to save some values for watchdog reset
#define RTC_SCRATCH1                    (REG_RTC_BASE + 0x034) // the register for software to save some values for watchdog reset
#define RTC_SCRATCH2                    (REG_RTC_BASE + 0x038) // the register for software to save some values for watchdog reset
#define RTC_SCRATCH3                    (REG_RTC_BASE + 0x03C) // the register for software to save some values for watchdog reset

#define RTC_GPIO_OUT                    (REG_RTC_BASE + 0x068) // used by gpio16
#define RTC_GPIO_ENABLE                 (REG_RTC_BASE + 0x074)
#define RTC_GPIO_IN_DATA                (REG_RTC_BASE + 0x08C)
#define RTC_GPIO_CONF                   (REG_RTC_BASE + 0x090)
#define PAD_XPD_DCDC_CONF               (REG_RTC_BASE + 0x0A0)
//}}

#endif //_EAGLE_SOC_H_
