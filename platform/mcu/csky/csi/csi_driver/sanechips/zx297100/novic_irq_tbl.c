/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
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

#include <csi_config.h>

extern void ISR_Reserved(void);
extern void WDT_IRQHandler(void);

extern void USART0_IRQHandler(void);
extern void USART1_IRQHandler(void);
extern void USART2_IRQHandler(void);
extern void USART3_IRQHandler(void);
extern void pcu_ao2ap_interrupt(void);
extern void lpm_wakeup_interrupt(void);



extern void bmu_sdncmp_isr(void);
extern void bmu_lp_isr(void);
extern void bmu_sp_isr(void);
extern void bmu_llp_isr(void);
extern void bmu_sdnpvd_isr(void);


extern void i2c0_irqhandler(void);

extern void CORET_IRQHandler(void);

extern void  ITIM_IRQHandler(void);

extern void TIM0_IRQHandler(void);
extern void TIM3_IRQHandler(void);
extern void TIM4_IRQHandler(void);
extern void TIM2_IRQHandler(void);

extern void SPI0_IRQHandler(void);
extern void icu_vector_handle(void);

extern void eflash0_irqhandler(void);

extern void eflash1_irqhandler(void);

extern void gpio13_irqhandler(void);

extern void icp_ceva_isr(void);

extern void rtc_stamp_handler(void);
extern void rtc_timer_handler(void);
extern void rtc_alarm_handler(void);

void (*g_irqvector[])(void) = {
    WDT_IRQHandler, /* 0, default interrupt entry */
    USART3_IRQHandler, /* 1, default interrupt entry */
    ISR_Reserved,       /* 2, default interrupt entry */
    USART1_IRQHandler, /* 3, default interrupt entry */
    ISR_Reserved, /* 4, default interrupt entry */
    ISR_Reserved, /* 5, default interrupt entry */
    i2c0_irqhandler, /* 6, iic interrupt entry */
    ISR_Reserved, /* 7, default interrupt entry */
    rtc_alarm_handler, /* 8, rtc alarm interrupt entry */
    rtc_timer_handler, /* 9, rtc timer interrupt entry */
    eflash0_irqhandler, /* 10, default interrupt entry */
    icp_ceva_isr,
    ISR_Reserved, /* 12, default interrupt entry */
    CORET_IRQHandler, /**/
    TIM0_IRQHandler,    /* 14, time0 interrupt entry */
    TIM3_IRQHandler,    /* 15, ad timer0 interrupt entry */
    TIM4_IRQHandler,    /* 16, ad timer1 interrupt entry */
    TIM2_IRQHandler,   /* 17, lp timer interrupt entry */
    ISR_Reserved, /* 18, default interrupt entry */
    rtc_stamp_handler, /* 19, rtc time satmp interrupt entry */
	lpm_wakeup_interrupt, /* 20, default interrupt entry */
    ISR_Reserved, /* 21, default interrupt entry */
    ISR_Reserved, /* 22, default interrupt entry */
    ISR_Reserved, /* 23, default interrupt entry */
    ISR_Reserved, /* 24, default interrupt entry */
    ISR_Reserved, /* 25, default interrupt entry */
    ISR_Reserved, /* 26, default interrupt entry */
    SPI0_IRQHandler, /* 27, spi interrupt entry */
    ISR_Reserved, /* 28, default interrupt entry */
    ISR_Reserved, /* 29, default interrupt entry */
    eflash1_irqhandler, /* 30, default interrupt entry */
    icu_vector_handle,
};

void (*g_icuvector[])(void) = {
	bmu_sp_isr,
	bmu_lp_isr,
	pcu_ao2ap_interrupt,
    ISR_Reserved, /* 3, default interrupt entry */
    ISR_Reserved, /* 4, default interrupt entry */
    gpio13_irqhandler,
    ISR_Reserved, /* 6, default interrupt entry */
    ISR_Reserved, /* 7, default interrupt entry */
    ISR_Reserved, /* 8, default interrupt entry */
    ISR_Reserved, /* 9, default interrupt entry */
    ISR_Reserved, /* 10, default interrupt entry */
    ISR_Reserved, /* 11, default interrupt entry */
    USART0_IRQHandler, /* 12, uart0 interrupt entry */
    ISR_Reserved, /* 13, default interrupt entry */
    ISR_Reserved, /* 14, default interrupt entry */
    USART2_IRQHandler, /* 15, uart2 interrupt entry */
    ISR_Reserved, /* 16, default interrupt entry */
    ISR_Reserved, /* 17, default interrupt entry */
    ISR_Reserved, /* 18, default interrupt entry */
    ISR_Reserved, /* 19, default interrupt entry */
	lpm_wakeup_interrupt,
    ISR_Reserved, /* 21, default interrupt entry */
    ISR_Reserved, /* 22, default interrupt entry */
    ISR_Reserved, /* 23, default interrupt entry */
    ISR_Reserved, /* 24, default interrupt entry */
    ISR_Reserved, /* 25, default interrupt entry */
    ISR_Reserved, /* 26, default interrupt entry */
    ISR_Reserved, /* 27, default interrupt entry */
	bmu_llp_isr,
	bmu_sdnpvd_isr,
    ISR_Reserved, /* 30, default interrupt entry */
    ISR_Reserved, /* 31, default interrupt entry */
    ISR_Reserved, /* 32, default interrupt entry */
    ISR_Reserved, /* 33, default interrupt entry */
    ISR_Reserved, /* 34, default interrupt entry */
    ISR_Reserved, /* 35, default interrupt entry */
};
