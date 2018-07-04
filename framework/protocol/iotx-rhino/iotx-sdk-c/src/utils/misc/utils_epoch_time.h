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


#ifndef _UTILS_EPOCH_TIME_H_
#define _UTILS_EPOCH_TIME_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "iot_import.h"

/**
 * @brief Get epoch time from the Aliyun NTP(ntp%d.aliyun.com).
 *        The type of the epoch time is millisecond.
 *
 * @param none
 *
 * @return 0, failed to get epoch time; OTHERS, the actual value of epoch time
 */
uint64_t utils_get_epoch_time_from_ntp(char copy[], int len);

#ifdef __cplusplus
extern "C" {
#endif
#endif /* _ALIOT_EPOCH_TIME_H_ */
