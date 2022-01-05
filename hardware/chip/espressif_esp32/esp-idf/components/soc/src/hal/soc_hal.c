// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include <stdlib.h>

#include "esp_err.h"

#include "hal/soc_hal.h"
#include "hal/soc_ll.h"
#include "soc/soc_caps.h"

#if SOC_CPU_CORES_NUM > 1
void soc_hal_stall_core(int core)
{
    soc_ll_stall_core(core);
}

void soc_hal_unstall_core(int core)
{
    soc_ll_unstall_core(core);
}
#endif // SOC_CPU_CORES_NUM > 1