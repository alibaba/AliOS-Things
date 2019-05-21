#ifndef _KEY_HANDLE_H_
#define _KEY_HANDLE_H_

#include "sys_rtos.h"
#include "rtos_pub.h"

#define KEY_DEBUG

#ifdef KEY_DEBUG
#define KEY_PRT       os_printf
#else
#define APP_PRT       os_null_printf
#endif


#endif
