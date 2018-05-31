/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/******************************************************************************
 * @file     drv_iic.h
 * @brief    header file for iic driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <drv_common.h>

typedef int (*irq_handler_t)(int, void *);


int32_t request_irq(uint32_t u32IrqNo, irq_handler_t pHandler,  
                        uint32_t u32Flags, const char *name, void *dev);
int32_t free_irq(uint32_t u32IrqNo, void* dev);


#ifdef __cplusplus
}
#endif

#endif /* _CSI_IIC_H_ */
