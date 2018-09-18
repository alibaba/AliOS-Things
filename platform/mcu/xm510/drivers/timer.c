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

#include <k_api.h>
#include <board.h>
#include <aos/types.h>

static unsigned long free_timer_overflows; 

static void os_hw_timer_isr(int vector, void *param)
{
	if ((readl(TIMER0_RIS)) & 0x1) {
		//rt_tick_increase();
        krhino_tick_proc();
		writel(0x01, TIMER0_INTCLR);
	}
	if ((readl(TIMER1_RIS)) & 0x1) {
		free_timer_overflows++;
		writel(~0, TIMER1_INTCLR);
	}   
}

int os_hw_timer_init(void)
{

    os_hw_interrupt_create(IRQ_XM510_TIMER01, os_hw_timer_isr, NULL, "tick");
    os_hw_interrupt_enable(IRQ_XM510_TIMER01);

	writel(0, TIMER0_CTRL);

	writel(1500000 /  RHINO_CONFIG_TICKS_PER_SECOND, TIMER0_LOAD);
    /* enable timer */
    writel(TIMER_CTRL_32BIT | TIMER_CTRL_PERIODIC | TIMER_CTRL_IE | TIMER_CTRL_DIV16 | TIMER_CTRL_ENABLE, TIMER0_CTRL);

	writel(0, TIMER1_CTRL);
	writel(0xffffffff, TIMER1_LOAD);
    /* enable timer */
    writel(TIMER_CTRL_32BIT | TIMER_CTRL_PERIODIC | TIMER_CTRL_IE | TIMER_CTRL_DIV1 | TIMER_CTRL_ENABLE, TIMER1_CTRL);
    return 0;
}

static unsigned long long xm510_cycles_2_ns(unsigned long long cyc)
{
    unsigned long cyc2ns_scale = (1000000 << 10)/ 24000;
    return (cyc * cyc2ns_scale) >> 10; 
}

unsigned long long system_clock(void)
{
	unsigned long long ticks64;
	unsigned long ticks2, ticks1;
	ticks2 = 0UL - (unsigned long)readl(TIMER1_VALUE);
	do {
		ticks1 = ticks2;
		ticks64 = free_timer_overflows;
		ticks2 = 0UL - (unsigned long)readl(TIMER1_VALUE);
	} while (ticks1 > ticks2);
	/*  
	 *** If INT is not cleaned, means the function is called with irq_save.
	 *** The ticks has overflow but 'free_timer_overflows' is not be update.
	 **/
	if (readl(TIMER1_MIS)) {
		ticks64 += 1;
		ticks2 = 0UL - (unsigned long)readl(TIMER1_VALUE);
	}
	return xm510_cycles_2_ns((ticks64 << 32) | ticks2);
}

void udelay(unsigned long usec)
{
	unsigned long long tmp;
	unsigned long tmo;

	tmo = usec * 1000;
	tmp = system_clock() + tmo;    /* get current timestamp */

	while (system_clock() < tmp);   /* loop till event */
}

void mdelay(unsigned long msec)
{
	while (msec--)
		udelay(1000);
}

