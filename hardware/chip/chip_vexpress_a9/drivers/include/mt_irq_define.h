/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __MT_IRQ_DEFINE_H
#define __MT_IRQ_DEFINE_H

#define OS_SMP_CROSSCORE_INTNO    3
#define OS_SMP_FREEZE_INTNO       10

/*
 * 26 : EL2 physical timer
 * 27 : Virtual timer interrupt
 * 28 : EL2 virtual timer
 * 29 : EL3 physical timer
 * 30 : EL1 physical timer in non-secure mode
 */
#define GIC_PPI_TIMER_VI_INT  (27)
#define GIC_PPI_TIMER_PH_EL3  (29)
#define GIC_PPI_TIMER_NS_EL1  (30)

#define	UART0_IRQ			(56)
#define	UART1_IRQ			(57)
#define	UART2_IRQ			(58)
#define IPC_INT_NUM			(161)

#define WDT_IRQ         160

#endif //__MT_IRQ_DEFINE_H
