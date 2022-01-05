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
#ifndef _APP_FACTORY_WIFI_H_
#define _APP_FACTORY_WIFI_H_

#ifdef __cplusplus
extern "C" {
#endif

extern void app_factorymode_wifi_nosignalingtest(void const *arg);

extern void app_factory_enter_wifi_nosignaltest_mode(void);
extern void app_factory_exit_wifi_nosignaltest_mode(void);

#ifdef __cplusplus
}
#endif

#endif /* _APP_FACTORY_WIFI_H_ */