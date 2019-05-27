#ifndef _MCU_PS_H_
#define _MCU_PS_H_

//#define MCU_PS_DEBUG

#ifdef MCU_PS_DEBUG
#define MCU_PS_PRT                 os_printf

#else
#define MCU_PS_PRT                 os_null_printf

#endif

#define     MS_TO_TICK      2

#endif

