/**
 * Copyright (2017) Baidu Inc. All rights reserved.
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
/**
 * File: baidu_ca_adapter.h
 * Auth: Su Hao (suhao@baidu.com)
 * Desc: Adapt the IoT CA to different platform.
 */
#ifndef BAIDU_DUER_IOT_CA_ADAPTER_BAIDU_CA_ADAPTER_H
#define BAIDU_DUER_IOT_CA_ADAPTER_BAIDU_CA_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

extern void baidu_ca_adapter_initialize(void);

extern void baidu_ca_adapter_finalize(void);

#ifdef __cplusplus
}
#endif

#endif/*BAIDU_DUER_IOT_CA_ADAPTER_BAIDU_CA_ADAPTER_H*/
