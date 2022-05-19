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

#ifndef BLE_TGT_MISC_H
#define BLE_TGT_MISC_H

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "host/ble_hs_adv.h"

#ifdef __cplusplus
extern "C" {
#endif

void rand_bytes(uint8_t *data, int len);

void print_bytes(const uint8_t *bytes, int len);

void print_addr(const void *addr);

void print_mbuf(const struct os_mbuf *om);

char *addr_str(const void *addr);

void print_uuid(const ble_uuid_t *uuid);

void print_conn_desc(const struct ble_gap_conn_desc *desc);

void print_adv_fields(const struct ble_hs_adv_fields *fields);

#ifdef __cplusplus
}
#endif

#endif //BLE_TGT_MISC_H
