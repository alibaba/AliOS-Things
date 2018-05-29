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

#ifndef __ETS_SYS_H__
#define __ETS_SYS_H__

/* interrupt related */
#define ETS_SPI_INUM        2
#define ETS_GPIO_INUM       4
#define ETS_UART_INUM       5
#define ETS_MAX_INUM        6
#define ETS_SOFT_INUM       7
#define ETS_WDT_INUM        8
#define ETS_FRC_TIMER1_INUM 9

extern char NMIIrqIsOn;
extern uint32 WDEV_INTEREST_EVENT;

#define INT_ENA_WDEV        0x3ff20c18
#define WDEV_TSF0_REACH_INT (BIT(27))

#define ETS_INTR_LOCK() do {    \
    if (NMIIrqIsOn == 0) { \
        vPortEnterCritical();   \
        char m = 10;    \
        do {    \
            REG_WRITE(INT_ENA_WDEV, 0); \
            m = 10; \
            for (; m > 0; m--) {}   \
            REG_WRITE(INT_ENA_WDEV, WDEV_TSF0_REACH_INT);   \
        } while(0);  \
    }   \
    } while(0)

#define ETS_INTR_UNLOCK()   do {    \
    if (NMIIrqIsOn == 0) { \
        REG_WRITE(INT_ENA_WDEV, WDEV_INTEREST_EVENT);   \
        vPortExitCritical(); \
    }   \
    } while(0)

#endif /* _ETS_SYS_H */
