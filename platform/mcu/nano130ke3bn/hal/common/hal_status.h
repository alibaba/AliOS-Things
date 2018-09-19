#ifndef __HAL_STATUS
#define __HAL_STATUS

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdio.h>

typedef enum
{
  HAL_OK       = 0x00,
  HAL_ERROR    = 0x01,
  HAL_BUSY     = 0x02,
  HAL_TIMEOUT  = 0x03
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
