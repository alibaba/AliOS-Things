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
#ifndef YUNOS_BSP_ICP_H
#define YUNOS_BSP_ICP_H

typedef void (*icpcb_t)(uint8_t cpu_id, uint32_t chan_id);

/**
 * This function is used for mask icp int
 * @param[in]  cpu_id   actor cpu id
 * @return the icp status of the other cpu
 */
int yunos_bsp_icp_mask_int(uint8_t cpu_id, uint32_t chan_id);

/**
 * This function is used for unmask icp int
 * @param[in]  cpu_id   actor cpu id
 * @return the icp status of the other cpu
 */
int yunos_bsp_icp_unmask_int(uint8_t cpu_id, uint32_t chan_id);

/**
 * This function is used for get icp status of other cpu
 * @param[in]  cpu_id   actor cpu id
 * @return the icp status of the other cpu
 */
int yunos_bsp_icp_get_int(uint8_t cpu_id);


/**
 * This function is used for send icp int
 * @param[in]  cpu_id   actor cpu id
 * @param[in]  chan_id  icp channel id, 0-31
 * @return the operation status, 0 is OK, others is error
 */
int yunos_bsp_icp_send_int(uint8_t cpu_id, uint32_t chan_id);


/**
 * This function is used for register icp callback func
 * @param[in]  cpu_id   actor cpu id
 * @param[in]  handler  icp callback func
 * @return the operation status, 0 is OK, others is error
 */
int yunos_bsp_icp_sethandler(uint8_t cpu_id, icpcb_t handler);

/**
 * This function is used for init icp
 * @return the operation status, 0 is OK, others is error
 */
int yunos_bsp_icp_init(void);

#endif