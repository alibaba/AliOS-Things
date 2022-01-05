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
#ifndef __DDBIF_H__
#define __DDBIF_H__

#include "bluetooth.h"
#include "me_api.h"

void ddbif_list_saved_flash(void);

bt_status_t ddbif_close(void);

bt_status_t ddbif_add_record(btif_device_record_t *record);

bt_status_t ddbif_open(const bt_bdaddr_t *bdAddr);

bt_status_t ddbif_find_record(const bt_bdaddr_t *bdAddr, btif_device_record_t *record);

bt_status_t ddbif_delete_record(const bt_bdaddr_t *bdAddr);

bt_status_t ddbif_enum_device_records(I16 index, btif_device_record_t *record);

size_t ddbif_device_records_num(void);
#endif /*__DDBIF_H__*/

