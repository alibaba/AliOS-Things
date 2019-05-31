
#ifndef __SYSTEM_CM4_H
#define __SYSTEM_CM4_H

#ifdef __cplusplus
 extern "C" {
#endif
#include "lega_cm4.h"


extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */




extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);
extern void lega_system_reset(void);

#ifdef __cplusplus
}
#endif

#endif
