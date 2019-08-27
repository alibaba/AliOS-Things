#ifndef __SYSTICK_DELAY_H
#define __SYSTICK_DELAY_H


#include "lega_cm4.h"


#ifdef ALIOS_SUPPORT
uint32_t lega_systick_csr_get();
void lega_systick_csr_set(uint32_t ctrl);
#endif
void delay_init(uint8_t SYSCLK);
void delay_us(u32 nus);
void delay_ms(u32 nms);
void delay_xms(u32 nms);


#endif //__SYSTICK_DELAY_H





























