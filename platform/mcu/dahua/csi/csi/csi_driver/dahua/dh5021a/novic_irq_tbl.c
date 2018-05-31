/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <config.h>

extern void NOVIC_IRQ_Default_Handler(void);
extern void TIMER0_IRQHandler(void);
extern uint32_t csi_intc_get_pending_irq(int32_t IRQn);

extern void USART0_IRQHandler(void);
extern void I2C0_IRQHandler(void);
extern void ADC_IRQHandler(void);
void decode_irq(void);

/*void (*g_irqvector[64])(void) = {
    0,                        
    TIMER0_IRQHandler,
    0,
    0,
    0,
    0,
    0,
    0,
    I2C0_IRQHandler,
    0,
    0,
    0,
    USART0_IRQHandler,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    ADC_IRQHandler
};*/
                                         /*0,  1,  2,  3,  4,   5,   6,   7,   8,    9,  10, 11, 12, 13, 14, 15, 16, 17, 18,  19, 20,  21,  22,  23,   24,  25,  26,  27,  28, 29, 30,  31*/
const unsigned char g_irqrelocation[64] = {0, 1, 2, 3, 4, 21, 21, 21, 5, 21, 21, 21, 6, 21, 21, 7, 8, 21, 9, 21, 21, 21, 21, 21, 10, 11, 12, 21, 21, 21, 21, 21,
                                        /* 32,  33,  34,  35, 36,  37,  38,  39,  40,  41,  42,  43, 44,  45,  46,  47,  48,  49,  50,  51, 52,  53,  54,  55,  56,  57,  58, 59,  60,  61,  62,  63*/
                                           21, 13, 14, 21, 21, 21, 21, 21, 15, 16, 17, 18, 19, 21, 21, 21, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21 ,21, 21};

void (*g_irqvector[22])(void) = {
    0,
};

void (*g_irqvector_intc[22])(void) = {
    0
};

void decode_irq(void)
{
    unsigned char irq_num = csi_intc_get_pending_irq(0);
    if (g_irqvector_intc[irq_num] != 0) {
        g_irqvector_intc[irq_num]();
    }
}

