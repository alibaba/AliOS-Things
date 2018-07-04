/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#ifndef __SECURITY_H__
#define __SECURITY_H__

#if defined(__cplusplus)
extern "C" {
#endif
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "lite-log.h"
#include "lite-utils.h"

#include "tfs/tfs.h"

int unittest_tfs(uint64_t timestamp);

#if defined(__cplusplus)
}
#endif
#endif  /* __SECURITY_H__ */

