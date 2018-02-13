/**
 * File: infra_config.h
 * Brief: Public configuration of infra
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * Author: Ningkun Deng <deng.ningkun@sanechips.com.cn>
 * @ingroup si_pub_infra_id
 * 
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _INFRA_CONFIG_H
#define _INFRA_CONFIG_H

/*******************************************************************************
 *                           Include header files                              *
 ******************************************************************************/
#define _CHIP_ZX297100
#if defined  (_CHIP_ZX297100)
#include "infra_config_7100.h"
#else
# error "unknown chipset"
#endif

/*******************************************************************************
 *                             Macro definitions                               *
 ******************************************************************************/

/*******************************************************************************
 *                             Type definitions                                *
 ******************************************************************************/

/*******************************************************************************
 *                       Global variable declarations                          *
 ******************************************************************************/

/*******************************************************************************
 *                       Global function declarations                          *
 ******************************************************************************/
 
/*******************************************************************************
 *                      Inline function implementations                        *
 ******************************************************************************/

#endif  // #ifndef _INFRA_CONFIG_H

