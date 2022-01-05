// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <protocomm_security.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Protocomm security version 0 implementation
 *
 * This is a simple implementation to be used when no
 * security is required for the protocomm instance
 */
extern const protocomm_security_t protocomm_security0;

#ifdef __cplusplus
}
#endif
