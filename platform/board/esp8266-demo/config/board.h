#ifndef __BOARD_H
#define __BOARD_H

#if defined(SUPPORT_SINGAPORE_DOMAIN)
#define AOS_MAIN_TASK_STACK_SIZE (7.5*1024)
#elif defined(ESP8266_CHIPSET)
#define AOS_MAIN_TASK_STACK_SIZE (2*1024)
#else
#define AOS_MAIN_TASK_STACK_SIZE (6*1024)
#endif

#endif /* __BOARD_H */