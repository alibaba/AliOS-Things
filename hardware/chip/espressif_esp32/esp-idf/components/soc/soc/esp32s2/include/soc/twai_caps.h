// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#ifdef __cplusplus
extern "C" {
#endif

//Chip specific TWAI related macros
//Any even number from 2 to 32768
#define TWAI_BRP_IS_VALID(brp)       ((brp) >= 2 && (brp) <= 32768 && ((brp) & 0x1) == 0)
#define TWAI_BRP_MAX                 32768

#ifdef __cplusplus
}
#endif
