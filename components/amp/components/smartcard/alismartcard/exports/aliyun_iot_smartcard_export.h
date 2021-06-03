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
 * aliyun_iot_smartcard_export.h: export file
 *
 */
#ifndef __ALIYUN_IOT_SMARTCARD_EXPORT_H__
#define __ALIYUN_IOT_SMARTCARD_EXPORT_H__

typedef enum {
    SWITCH_TO_SIM1 = 1, /* CHINA_MOBILE */
    SWITCH_TO_SIM2,     /* CHINA_UNICOM */
    SWITCH_TO_SIM3,     /* CHINA_TELECOM */
    SWITCH_TO_NEXT,
    SWITCH_MAX
} aliyun_iot_smartcard_type_t;

/* mn error code definitions */
#define MN_SUCCESS                   0  /* The operation was successful */
#define MN_ERROR_GENERIC            -1  /* The generic error */
#define MN_ERROR_INIT               -2  /* init error */
#define MN_ERROR_COMMON_PARAMETER   -3  /* Invalid parameter */
#define MN_ERROR_NO_SIM             -4  /* no sim card detected */
#define MN_ERROR_SAME_CARD          -5  /* iccid is same after switch */
#define MN_ERROR_TIMEOUT            -6  /* timeout */
#define MN_ERROR_NOT_SMARTCARD      -7  /* not alibaba iot smart card */
#define MN_ERROR_SERIAL             -8  /* serial init error */
#define MN_ERROR_CHECK              -9  /* fail to parse alibaba iot smard card info */

/*!
 * \brief   aliyun smart card set config file path
 *
 * \retval  Possible returns are:
 *           0: init succesful
 *          -2: already inited
 *
 * \note should be called before aliyun_iot_smartcard_init
 */
extern int aliyun_iot_smartcard_set_config_file(const char *file_path);

/*!
 * \brief   aliyun smart card initialization
 *
 * \retval  Possible returns are:
 *           0: init succesful
 *          -1: init failed
 *          -2: already inited
 *          -3: internal parameter error
 *          -4: sim card not detected
 *          -6: timeout
 *          -8: serial init error
 *          -9: fail to parse smartcard info
 */
extern int aliyun_iot_smartcard_init(void);

/*!
 * \brief   aliyun smart card deinitialization
 *
 * \retval  Possible returns are:
 *           0: deinit succesful
 *          -1: deinit failed
 *          -2: not inited
 */
extern int aliyun_iot_smartcard_deinit(void);

/*!
 * \brief   check if aliyun smart card or not
 *
 * \retval  Possible returns are:
 *           0: is aliyun smart card
 *          -2: not inited
 *          -7: not aliyun smart card
 */
extern int aliyun_iot_is_smartcard(void);

/*!
 * \brief   switch to specified operator
 *
 * \param   [IN] type: switch type
 *
 * \retval  Possible returns are:
 *            0: switch success
 *           -1: switch fail
 *           -2: not inited
 *           -3: parameter error
 *           -5: iccid is same after switch
 *           -6: timeout
 *           -7: not alibaba iot smart card
 */
extern int aliyun_iot_smartcard_change_operator(aliyun_iot_smartcard_type_t type);

/*!
 * \brief   aliyun smart card get sdk version
 *
 * \param   [OUT] major: major version
 * \param   [OUT] minor: minor version
 *
 * \retval  Possible returns are:
 *           0: succesful
 */
extern int aliyun_iot_smartcard_get_version(int *major, int *minor);
#endif /* __ALIYUN_IOT_SMARTCARD_EXPORT_H__ */
