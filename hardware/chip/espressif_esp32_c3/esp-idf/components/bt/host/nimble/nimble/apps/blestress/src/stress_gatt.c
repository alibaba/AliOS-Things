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

#include "stress_gatt.h"

uint16_t hrs_hrm_handle = 0xffff;

static int
stress_gatt_access_cb(uint16_t conn_handle, uint16_t attr_handle,
                      struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /* Service: Heart-rate */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(STRESS_GATT_UUID),
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                /* Characteristic: read test */
                .uuid = BLE_UUID16_DECLARE(STRESS_GATT_READ_UUID),
                .access_cb = stress_gatt_access_cb,
                .val_handle = &hrs_hrm_handle,
                .flags = BLE_GATT_CHR_F_READ,
            },
            {
                /* Characteristic: write test */
                .uuid = BLE_UUID16_DECLARE(STRESS_GATT_WRITE_UUID),
                .access_cb = stress_gatt_access_cb,
                .val_handle = &hrs_hrm_handle,
                .flags = BLE_GATT_CHR_F_WRITE,
            },
            {
                /* Characteristic: notify test */
                .uuid = BLE_UUID16_DECLARE(STRESS_GATT_NOTIFY_UUID),
                .access_cb = stress_gatt_access_cb,
                .val_handle = &hrs_hrm_handle,
                .flags = BLE_GATT_CHR_F_NOTIFY,
            },
            {
                /* Characteristic: indicate test */
                .uuid = BLE_UUID16_DECLARE(STRESS_GATT_INDICATE_UUID),
                .access_cb = stress_gatt_access_cb,
                .val_handle = &hrs_hrm_handle,
                .flags = BLE_GATT_CHR_F_INDICATE,
            },
            {
                0, /* No more characteristics in this service */
            },}
    },
    {
        0, /* No more services */
    },
};

static int
stress_gatt_access_cb(uint16_t conn_handle, uint16_t attr_handle,
                      struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    /* Sensor location, set to "Chest" */
    static uint8_t chr_value[] = "Hello";
    uint16_t uuid;
    int rc;

    //chr_value = (uint8_t)rand() % 256;
    uuid = ble_uuid_u16(ctxt->chr->uuid);

    switch(uuid){
    case STRESS_GATT_READ_UUID:
        MODLOG_DFLT(INFO, "GATT Read event\n");
        rc = os_mbuf_append(ctxt->om, &chr_value, sizeof(chr_value));
        assert(rc == 0);
        //return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        return 0;
    case STRESS_GATT_WRITE_UUID:
        MODLOG_DFLT(INFO, "GATT Write event\n");
        print_mbuf(ctxt->om);
        return 0;
    case STRESS_GATT_NOTIFY_UUID:
        MODLOG_DFLT(INFO, "GATT Notify event\n");
        return 0;
    case STRESS_GATT_INDICATE_UUID:
        MODLOG_DFLT(INFO, "GATT Indicate event\n");
        return 0;
    default:
        MODLOG_DFLT(ERROR, "GATT UUID does not exist\n");
        assert(0);
        return BLE_ATT_ERR_UNLIKELY;
    }
}

void
gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];

    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
    MODLOG_DFLT(DEBUG, "registered service %s with handle=%d\n",
                ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                ctxt->svc.handle);
        break;

    case BLE_GATT_REGISTER_OP_CHR:
    MODLOG_DFLT(DEBUG, "registering characteristic %s with "
                       "def_handle=%d val_handle=%d\n",
                ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                ctxt->chr.def_handle,
                ctxt->chr.val_handle);
        break;

    case BLE_GATT_REGISTER_OP_DSC:
    MODLOG_DFLT(DEBUG, "registering descriptor %s with handle=%d\n",
                ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                ctxt->dsc.handle);
        break;

    default:
        assert(0);
        break;
    }
}

int
gatt_svr_init(void)
{
    int rc;

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
