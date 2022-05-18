/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef BLE_TGT_STRESS_GATT_H
#define BLE_TGT_STRESS_GATT_H

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "nimble/ble.h"
#include "modlog/modlog.h"
#include "misc.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint16_t hrs_hrm_handle;

/* Heart-rate configuration */
#define STRESS_GATT_UUID                     0xC0DE
#define STRESS_GATT_READ_UUID                0xC1DE
#define STRESS_GATT_WRITE_UUID               0xC2DE
#define STRESS_GATT_INDICATE_UUID            0xC3DE
#define STRESS_GATT_NOTIFY_UUID              0xC4DE

int gatt_svr_init(void);

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);

#ifdef __cplusplus
}
#endif


#endif //BLE_TGT_STRESS_GATT_H
