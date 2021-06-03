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
 * Description:
 * moudle_at.h: module at api
 *
 */

#ifndef __MODULE_AT_H__
#define __MODULE_AT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "aliyun_iot_smartcard_internal.h"

/**
 * @brief process at response
 *
 * @param [in] cmd: at cmd header
 * @param [in] str_resp: resp str to be processed
 * @param [out] argc: para cnt
 * @param [out] argv: pointer to each para
 * @return  0 on success, negative error on failure.
 * @see None.
 * @note None.
 */
int module_proc_at_resp(const char *cmd, char *str_resp, char **str_resp_next, int *argc, char *argv[]);

/**
 * @brief serial init
 *
 * @return  0 on success, negative error on failure.
 * @see None.
 * @note None.
 */
int module_serial_init(void);

/**
 * @brief serial deinit
 *
 * @return  0 on success, negative error on failure.
 * @see None.
 * @note None.
 */
int module_serial_deinit(void);

/*!
 * \brief   get serial name
 *
 * \param   [IN] name: serial name to be saved
 * \param   [IN] len: name len
 *
 * \retval  Possible returns are:
 *          -1: init failed.
 *           0: init succesful.
 */
int module_serial_get_name(char *name, int len);

#ifdef __cplusplus
}
#endif

#endif
