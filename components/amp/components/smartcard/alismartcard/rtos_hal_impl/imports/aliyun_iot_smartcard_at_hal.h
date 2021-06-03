/*
 * Copyright (c) 2019 Alibaba Group. All rights reserved.
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

#ifndef __ALIYUN_IOT_SMARTCARD_HAL_H__
#define __ALIYUN_IOT_SMARTCARD_HAL_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief send at command
 *
 * @param [in] at_cmd: at cmd str
 * @param [in] final_rsp:  final resp str
 * @param [out] str_resp: resp str
 * @param [in] resp_len: resp str len
 * @param [in] timeout: command timeout in ms
 * @return  0 on success, negative error on failure.
 * @see None.
 * @note None.
 */
int aliyun_iot_smartcard_hal_send_at(const char *at_cmd, const char *final_rsp, char *str_resp, int resp_len, long timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
