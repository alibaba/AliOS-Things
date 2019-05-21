#ifndef _KEY_MAIN_H_
#define _KEY_MAIN_H_

#include "gpio_pub.h"
#include "uart_pub.h"

#define KEY_DEBUG

#ifdef KEY_DEBUG
#define KEY_PRT                 os_printf
#define KEY_WPRT                warning_prf
#else
#define KEY_PRT                 os_null_printf
#define KEY_WPRT                os_null_printf
#endif

#define GPIO_TEST_ID           GPIO23
#define KEY_TMR_DURATION       5

extern void key_get_setting(void);
extern void key_configure(void);
extern void key_item_configure(void);

#endif // 
