/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <types.h>
#include "kernel/os/os.h"
#include "sys/param.h"
#include "driver/chip/chip.h"

#ifndef __CONFIG_BOOTLOADER

#define DUMP_BUF_LEN        64

#define readb(addr)         (*((volatile unsigned char  *)(addr)))
#define readw(addr)         (*((volatile unsigned short *)(addr)))
#define readl(addr)         (*((volatile unsigned int *)(addr)))

#define  CPU_REG_NVIC_ICSR      (readl(0xE000ED04))     /* Int Ctrl State  Reg.         */
#define  CPU_REG_NVIC_SHCSR     (readl(0xE000ED24))     /* Hard  Fault Status Reg.      */
#define  CPU_REG_NVIC_HFSR      (readl(0xE000ED2C))     /* Hard  Fault Status Reg.      */
#define  CPU_REG_NVIC_DFSR      (readl(0xE000ED30))     /* Debug Fault Status Reg.      */
#define  CPU_REG_NVIC_MMFAR     (readl(0xE000ED34))     /* Mem Manage Addr Reg.         */
#define  CPU_REG_NVIC_BFAR      (readl(0xE000ED38))     /* Bus Fault  Addr Reg.         */
#define  CPU_REG_FPU_FPSCR      (readl(0xE000ED38))     /* FP status and control Reg.   */

extern uint8_t __text_start__[];
extern uint8_t __text_end__[];
extern uint8_t	_estack[];

#if configDEBUG_TRACE_TASK_MOREINFO
static char dbg_tasks_buf[1024];
#endif

/* defined for other modules to trace exception */
volatile int exceptin_step;

/*
 * GET CURRENT EXCETOPM
 *
 * Description: get the source number of current exception, include all exceptions.
 * Arguments  : none.
 * Returns    : the source number of current exception.
 * Note       :
 */
static uint32_t intc_get_current_exception(void)
{
	return (CPU_REG_NVIC_ICSR & 0x3ff);
}

static void exception_hex_dump(const uint32_t *addr, uint32_t num)
{
	uint32_t i;

	for (i = 0; i < num; i++) {
		if ((i & 0x03) == 0x0)
			printf("\n[%p]: ", addr);
		printf("0x%08x ", *addr++);
	}
	printf("\n");
}

/*
 * EXCEPTION ENTRY
 *
 * Description:  the entry of CPU exception, mainly for CPU nmi, hard fault,
 * memmanger fault, bus fault, usage fault, SVCall, debugmonitor exception.
 *
 * register list in stack:
 * ---------LOW ADDR--------
 * R4            <-PSP(pstack)
 * R5
 * R6
 * R7
 * R8
 * R9
 * R10
 * R11           <-PSP+4*7
 * R0            <-PSP+4*8
 * R1
 * R2
 * R3
 * R12           <-PSP+4*12
 * R14(LR)       <-PSP+4*13
 * R15(PC)       <-PSP+4*14
 * xPSR          <-PSP+4*15
 * --------HIGH ADDR--------
 * Arguments  :  pstack:the pointer of stack, PSP before the exception happen.
 * Returns    :  OK if process CPU exception succeeded, others if failed.
 */
int32_t exception_entry(uint32_t *pstack, uint32_t *msp, uint32_t *psp)
{
	uint32_t i;
	uint32_t exceptionno = intc_get_current_exception();
	const uint32_t *add = pstack + 16;
	uint32_t lr, pc;
	uint32_t len;

	/* intno can't beyond then 16 */
	//ASSERT(exceptionno < 16);

	printf("\nexception:%d happen!!\n", exceptionno);
	printf("appos pstack:0x%x msp:0x%x psp:0x%x\n",
	       (uint32_t)pstack, (uint32_t)msp, (uint32_t)psp);

	switch (exceptionno) {
	case 2:
		printf("NMI happen\n");
		break;
	case 3:
		printf("hard  fault       happen, HFSR:0x%x\n", CPU_REG_NVIC_HFSR);
		printf("memm  fault maybe happen, MFSR:0x%x, MMFAR:0x%x\n",
		       readb(0xE000ED28), CPU_REG_NVIC_MMFAR);
		printf("bus   fault maybe happen, BFSR:0x%x, BFAR:0x%x\n",
		       readb(0xE000ED29), CPU_REG_NVIC_BFAR);
		printf("usage fault maybe happen, UFSR:0x%x\n", readw(0xE000ED2A));
		break;
	case 4:
		printf("memm fault happen, MFSR:0x%x, MMFAR:0x%x\n",
		       readb(0xE000ED28), CPU_REG_NVIC_MMFAR);
		break;
	case 5:
		printf("bus fault happen, BFSR:0x%x, BFAR:0x%x\n",
		       readb(0xE000ED29), CPU_REG_NVIC_BFAR);
		break;
	case 6:
		printf("usage fault happen, UFSR:0x%x\n", readw(0xE000ED2A));
		break;
	case 11:
		printf("SVCall fault happen\n");
		break;
	case 12:
		printf("SVCall fault happen, DFSR:0x%x\n", CPU_REG_NVIC_DFSR);
		break;
	default:
		/* invalid exception nr */
		printf("invalid exception nr\n");
	}

	/* print R0-R3 */
	printf("CPU registers:\n");
	for (i = 0; i <= 3; i++) {
		printf("R%02d:[%p]: 0x%08x\n", i, (pstack + (8 + i)), *(pstack + (8 + i)));
	}

	/* print R4-R11 */
	for (i = 0; i <= 7; i++) {
		printf("R%02d:[%p]: 0x%08x\n", i + 4, (pstack + i), *(pstack + i));
	}

	/* print R12, R14(LR), R15(PC), xPSR */
	printf("R12:[%p]: 0x%08x\n", (pstack + 12), *(pstack + 12));
	lr = *(pstack + 13);
	printf("R14(LR):[%p]: 0x%08x\n", (pstack + 13), lr);
	pc = *(pstack + 14);
	printf("R15(PC):[%p]: 0x%08x\n", (pstack + 14), pc);
	printf("xPSR:[%p]: 0x%08x\n", (pstack + 15), *(pstack + 15));
	printf("SHCSR:0x%08x step:%x\n", CPU_REG_NVIC_SHCSR, exceptin_step);
#if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
	printf("FPSCR:0x%08x\n", CPU_REG_FPU_FPSCR);
#endif

	printf("\nstack info:");
	if ((((uint32_t)add) + DUMP_BUF_LEN*4) <= (uint32_t)_estack)
		len = DUMP_BUF_LEN;
	else
		len = (((uint32_t)_estack) - ((uint32_t)add))/4;
	len = MIN(len, DUMP_BUF_LEN*2);
	exception_hex_dump(add, len);

	printf("\n[LR]:0x%x", lr);
	lr &= ~0x07;
	if (lr > (uint32_t)__text_start__ && lr < (uint32_t)__text_end__) {
		lr -= DUMP_BUF_LEN * 2;
		exception_hex_dump((const uint32_t *)lr, DUMP_BUF_LEN);
	}

	printf("\n[PC]:0x%x", pc);
	pc &= ~0x07;
	if (pc < (uint32_t)__text_end__) {
		pc -= DUMP_BUF_LEN * 2;
		pc = (pc < (uint32_t)__text_end__) ? pc : 0;
		exception_hex_dump((const uint32_t *)pc, DUMP_BUF_LEN);
	}

#if configDEBUG_TRACE_TASK_MOREINFO
	printf("\ntasks state:\n");
	vTaskList(dbg_tasks_buf);
	printf("%s\n", dbg_tasks_buf);
#endif

	/* if happen fault, print important information and drop-dead halt */
	while (1)
		;

	return 0;
}

void exception_panic(const char *file, const char *func, const int line)
{
	printf("panic at %s func:%s line:%d!!\n", file, func, line);

	__asm volatile ("bkpt 0");
}

#endif /* __CONFIG_BOOTLOADER */
