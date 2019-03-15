/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
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

#ifndef YUNOS_BSP_RESET_H
#define YUNOS_BSP_RESET_H

/**
 * This function is used for reset system
 * @param[in]  type 	SYS_REBOOT_WARM/SYS_REBOOT_COLD
 * @return	  void
 */
void yunos_sys_reset(uint8_t type);

#endif


