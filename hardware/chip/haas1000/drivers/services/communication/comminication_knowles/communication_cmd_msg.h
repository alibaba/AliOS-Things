/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __COMMUNICATION_CMD_MSG_H__
#define __COMMUNICATION_CMD_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif

enum COMMUNICATION_CMD_TYPE {
    COMMUNICATION_CMD_EQ_OP = 0xA1,
    COMMUNICATION_CMD_DRC_OP  = 0xA2,
    COMMUNICATION_CMD_HF_OP  = 0xA3,
};


#ifdef __cplusplus
}
#endif

#endif

