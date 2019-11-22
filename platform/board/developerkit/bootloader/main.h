#ifndef __MAIN_H__
#define __MAIN_H__
#include "stm32l4xx_hal.h"

#define WDT_INIT_TIME_SECOND        (15)


#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
