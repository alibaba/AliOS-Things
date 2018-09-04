// See LICENSE for license details.

#ifndef _SIFIVE_PLATFORM_H
#define _SIFIVE_PLATFORM_H

// Some things missing from the official encoding.h

#if __riscv_xlen == 32
#define MCAUSE_INT         0x80000000UL
#define MCAUSE_CAUSE       0x000003FFUL
#else
#define MCAUSE_INT         0x8000000000000000UL
#define MCAUSE_CAUSE       0x00000000000003FFUL
#endif

#define MTVEC_DIRECT       0X00
#define MTVEC_VECTORED     0x01
#define MTVEC_CLIC         0x02
#define MTVEC_CLIC_VECT    0X03


#include "sifive/const.h"
#include "sifive/devices/gpio.h"
#include "sifive/devices/clint.h"
#include "sifive/devices/clic.h"
#include "sifive/devices/pwm.h"
#include "sifive/devices/spi.h"
#include "sifive/devices/uart.h"

/****************************************************************************
 * Platform definitions
 *****************************************************************************/

// Memory map
#define CLINT_CTRL_ADDR _AC(0x02000000,UL)
#define CLIC_HART0_ADDR _AC(0x02800000,UL)
#define GPIO_CTRL_ADDR _AC(0x20002000,UL)
#define PWM0_CTRL_ADDR _AC(0x20005000,UL)
#define RAM_MEM_ADDR _AC(0x80000000,UL)
#define RAM_MEM_SIZE _AC(0x10000,UL)
#define SPI0_CTRL_ADDR _AC(0x20004000,UL)
#define SPI0_MEM_ADDR _AC(0x40000000,UL)
#define SPI0_MEM_SIZE _AC(0x20000000,UL)
#define TESTBENCH_MEM_ADDR _AC(0x20000000,UL)
#define TESTBENCH_MEM_SIZE _AC(0x10000000,UL)
//#define TRAPVEC_TABLE_CTRL_ADDR _AC(0x00001010,UL)
#define UART0_CTRL_ADDR _AC(0x20000000,UL)

// IOF masks

// Interrupt numbers
#define RESERVED_INT_BASE 0
#define UART0_INT_BASE 1
#define EXTERNAL_INT_BASE 2
#define SPI0_INT_BASE 6
#define GPIO_INT_BASE 7
#define PWM0_INT_BASE 23

// Helper functions
#define _REG64(p, i) (*(volatile uint64_t *)((p) + (i)))
#define _REG32(p, i) (*(volatile uint32_t *)((p) + (i)))
#define _REG16(p, i) (*(volatile uint16_t *)((p) + (i)))
#define SET_BITS(reg, mask, value) if ((value) == 0) { (reg) &= ~(mask); } else { (reg) |= (mask); }
#define CLINT_REG(offset) _REG32(CLINT_CTRL_ADDR, offset)
#define CLIC0_REG(offset) _REG32(CLIC_HART0_ADDR, offset)
#define CLIC0_REG8(offset)   (*(volatile uint8_t *)((CLIC_HART0_ADDR) + (offset)))
#define GPIO_REG(offset) _REG32(GPIO_CTRL_ADDR, offset)
#define PWM0_REG(offset) _REG32(PWM0_CTRL_ADDR, offset)
#define SPI0_REG(offset) _REG32(SPI0_CTRL_ADDR, offset)
#define UART0_REG(offset) _REG32(UART0_CTRL_ADDR, offset)
#define CLINT_REG(offset) _REG32(CLINT_CTRL_ADDR, offset)
#define CLIC0_REG64(offset) _REG64(CLIC_HART0_ADDR, offset)
#define GPIO_REG64(offset) _REG64(GPIO_CTRL_ADDR, offset)
#define PWM0_REG64(offset) _REG64(PWM0_CTRL_ADDR, offset)
#define SPI0_REG64(offset) _REG64(SPI0_CTRL_ADDR, offset)
#define UART0_REG64(offset) _REG64(UART0_CTRL_ADDR, offset)

// Misc

#define NUM_GPIO 16

#define CLIC_NUM_INTERRUPTS 28 + 16

#ifdef E20
   #define CLIC_CONFIG_BITS 2
#else
   #define CLIC_CONFIG_BITS 4
#endif

#define HAS_BOARD_BUTTONS

#include "coreplexip-arty.h"

unsigned long get_cpu_freq(void);
unsigned long get_timer_freq(void);
uint64_t get_timer_value(void);

#endif /* _SIFIVE_PLATFORM_H */
