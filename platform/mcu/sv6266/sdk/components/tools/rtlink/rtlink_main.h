
/*************************************

    Package: RTlink main
    Version: 1.0
    
    Copyright (C) 2018 RangerTek Inc. All Rights Reserved.
    
*************************************/

#ifndef _RTLINK_MAIN_H_
#define _RTLINK_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "soc_types.h"
#include "rtlink_smnt.h"
#include "osal/osal.h"

/**
 * init the rtlink task
 *
 * - init the rtlink task, and run this feature
  *
 * @param   key     aes key string
 */
void RTInit(
    u8 *key);

/**
 * stop the rtlink task
 *
 * - stop the rtlink task, and stop this feature
 */
void RTStop(
    void);

#ifdef __cplusplus
}
#endif

#endif

