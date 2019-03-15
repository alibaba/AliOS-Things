#ifndef __HK32F1xx_HAL_H
#define __HK32F1xx_HAL_H

#ifdef __cplusplus
extern "C" {
#endif


#include "HK32f1xx_hal_conf.h"


typedef enum
{
  HAL_TICK_FREQ_10HZ         = 100U,
  HAL_TICK_FREQ_100HZ        = 10U,
  HAL_TICK_FREQ_1KHZ         = 1U,
  HAL_TICK_FREQ_DEFAULT      = HAL_TICK_FREQ_1KHZ
} HAL_TickFreqTypeDef;




#define IS_TICKFREQ(FREQ) (((FREQ) == HAL_TICK_FREQ_10HZ)  || \
                           ((FREQ) == HAL_TICK_FREQ_100HZ) || \
                           ((FREQ) == HAL_TICK_FREQ_1KHZ))

HAL_StatusTypeDef HAL_Init(void);
HAL_StatusTypeDef HAL_DeInit(void);
void HAL_MspInit(void);
void HAL_MspDeInit(void);
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority);

/* Peripheral Control functions  ************************************************/
uint32_t HAL_GetTick(void);

#ifdef __cplusplus
}
#endif

#endif /* __HK32F1xx_HAL_H */

