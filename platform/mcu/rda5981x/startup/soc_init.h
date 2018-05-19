/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RDA59XX_SOC_H
#define __RDA59XX_SOC_H
#define __main_h__
#ifdef __cplusplus
 extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void rda_soc_init();

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
enum {BP_NOT_PUSHED=0, BP_SINGLE_PUSH, BP_MULTIPLE_PUSH};

/* Exported functions --------------------------------------------------------*/
void    Error_Handler(void);
uint8_t Button_WaitForPush(uint32_t timeout);
void    Led_On(void);
void    Led_Off(void);

//extern RNG_HandleTypeDef hrng;
//extern RTC_HandleTypeDef hrtc;

#ifdef __cplusplus
}
#endif

#endif /* __RDA59XX_SOC_H */
