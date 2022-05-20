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

#ifndef _BLE_STRESS_TX_H
#define _BLE_STRESS_TX_H

#include <assert.h>
#include <string.h>
#include <host/ble_gap.h>
#include <stdlib.h>

/* BLE */
#include "nimble/ble.h"
#include "host/ble_hs.h"

#include "misc.h"
#include "stress.h"
#include "stress_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Scan and execute tests one by one.
 */
void tx_stress_start_auto();

#ifdef __cplusplus
}
#endif

#endif //_BLE_STRESS_TX_H
