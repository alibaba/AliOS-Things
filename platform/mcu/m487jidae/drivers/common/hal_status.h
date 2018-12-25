#ifndef __HAL_STATUS
#define __HAL_STATUS

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdio.h>

typedef enum
{
	HAL_I2C_ERROR_NO_SLAVE = -4,
  HAL_BUSY     = -3,
  HAL_TIMEOUT  = -2,
  HAL_ERROR    = -1,
  HAL_OK       = 0
} E_HAL_Status;

typedef enum
{
  HAL_UNLOCKED = 0x00,
  HAL_LOCKED   = 0x01
} E_HAL_Lock;

#define HAL_MAX_DELAY      0xFFFFFFFFU

#ifdef __cplusplus
}
#endif

#endif /* __HAL_STATUS */
