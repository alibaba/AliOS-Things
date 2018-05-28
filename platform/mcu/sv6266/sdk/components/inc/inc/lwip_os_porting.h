#ifndef _LWIP_OS_PRTING_H_
#define _LWIP_OS_PRTING_H_

//#include <ssv_lib.h>
#include <osal.h>

/*============Console setting===================*/
#define LOG_PRINTF hal_print
#define PRINTF LOG_PRINTF
#define FFLUSH(x) 

#define hal_print printf

/*============Memory operation===================*/
#define MALLOC OS_MemAlloc
#define FREE OS_MemFree

/*============String operation===================*/
#define STRCMP strcmp

#endif //#ifndef _LWIP_OS_PRTING_H_
