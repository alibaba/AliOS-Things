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

#include <host/ble_gap.h>
#include "rx_stress.h"

/* UUID128 of stress test use cases*/
static uint8_t rx_stress_uuid128[STRESS_UUIDS_NUM][16];

static struct com_stress_test_ctx rx_stress_ctxD = {
    .conn_handle = 0xffff,
    .cur_test_id = 0,
    .s6_rcv_adv_first = 0,
    .s6_rcv_adv_suc = 0,
};

static struct com_stress_test_ctx *rx_stress_ctx = &rx_stress_ctxD;

#define EXTENDED_ADVERT 0
#define LEGACY_ADVERT 1
/* Advertising instances ids */
#define SWITCHER_INSTANCE 0
#define TEST_INSTANCE 1
/* Main test task priority. Set a high value so that the task does not
 * interfere with event handling */
#define RX_STRESS_MAIN_TASK_PRIO 0xf0

/* Advertising settings */
struct rx_stress_adv_set {
    uint8_t instance;
    uint8_t *instance_uuid128;
    uint8_t legacy_pdu;
    ble_gap_event_fn *cb;
    const uint8_t *pattern_data;
    int pattern_len;
};

/* Define task stack and task object */
#define RX_STRESS_MAIN_TASK_STACK_SIZE (2000)
static struct os_task rx_stress_main_task;
static os_stack_t rx_stress_main_task_stack[RX_STRESS_MAIN_TASK_STACK_SIZE];
static struct os_sem rx_stress_main_sem;

static void
rx_stress_on_test_finish(int test_num)
{
    console_printf("\033[0;32m\nStress test %d completed\033[0m\n", test_num);
    os_sem_release(&rx_stress_main_sem);
}

static int
rx_stress_adv_start(uint8_t instance)
{
    int rc;

    /* Resume advertising earlier configured instance */
    rc = ble_gap_ext_adv_start(instance, 0, 0);
    assert (rc == 0 || rc == 2);
    MODLOG_DFLT(INFO, "Instance %d started; rc: %d\n", instance, rc);
    return rc;
}

static int
rx_stress_adv_start_with_rand_addr(uint8_t instance)
{
    int rc;
    ble_addr_t addr;

    ble_gap_ext_adv_stop(instance);

    rc = ble_hs_id_gen_rnd(1, &addr);
    assert (rc == 0);

    /* Set random address for advertising instance */
    rc = ble_gap_ext_adv_set_addr(instance, &addr);
    assert (rc == 0);

    return rx_stress_adv_start(instance);
}

static void
rx_stress_simple_adv(struct rx_stress_adv_set *adv_set)
{
    uint8_t own_addr_type;
    struct ble_gap_ext_adv_params params;
    struct ble_hs_adv_fields fields;
    struct os_mbuf *adv_data;
    ble_addr_t addr;
    const char *name;
    int rc;
    int pattern_len;

    /* Determine own address type */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "\033[0;31mError determining own address type; "
                           "rc=%d\033[0m\n", rc);
        return;
    }

    /* Use defaults for non-set fields */
    memset(&fields, 0, sizeof fields);
    /* General Discoverable and BrEdrNotSupported */
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    /* Set device name as instance name */
    name = ble_svc_gap_device_name();
    fields.name = (uint8_t *) name;
    fields.name_len = strlen(name);
    fields.name_is_complete = 1;
    /* Set UUID 128 data service */
    fields.svc_data_uuid128 = adv_set->instance_uuid128;
    fields.svc_data_uuid128_len = 16;

    /* Use defaults for non-set params */
    memset(&params, 0, sizeof(params));
    /* Set adv mode */
    if (adv_set->legacy_pdu == 1) {
        params.connectable = 1;
        params.scannable = 1;
    } else if (adv_set->pattern_len < 255) {
        params.connectable = 1;
    }

    params.own_addr_type = own_addr_type;
    params.primary_phy = BLE_HCI_LE_PHY_1M;
    /* If legacy, this param will be lowered by API */
    params.secondary_phy = BLE_HCI_LE_PHY_2M;
    params.sid = adv_set->instance;
    params.legacy_pdu = adv_set->legacy_pdu;

    ble_gap_set_prefered_default_le_phy(TX_PHY_MASK, RX_PHY_MASK);

    rc = ble_gap_ext_adv_remove(adv_set->instance);
    assert(rc == 0 || rc == BLE_HS_EALREADY);

    /* Configure instance with the params set */
    rc = ble_gap_ext_adv_configure(adv_set->instance, &params,
                                   NULL, adv_set->cb, NULL);
    assert (rc == 0);

    if (own_addr_type == 0) {
        memcpy(addr.val, MYNEWT_VAL(BLE_PUBLIC_DEV_ADDR), 6);
    } else {
        rc = ble_hs_id_gen_rnd(1, &addr);
        assert (rc == 0);

        /* Set random address for advertising instance */
        rc = ble_gap_ext_adv_set_addr(adv_set->instance, &addr);
        assert (rc == 0);
    }

    /* Get mbuf for adv data */
    adv_data = os_msys_get_pkthdr(16, 0);
    assert(adv_data != NULL);

    /* Fill mbuf with adv fields - structured data */
    rc = ble_hs_adv_set_fields_mbuf(&fields, adv_data);

    if (rc) {
        os_mbuf_free_chain(adv_data);
        assert(0);
    }

    pattern_len = min(adv_set->pattern_len,
                      MYNEWT_VAL(BLE_EXT_ADV_MAX_SIZE) - adv_data->om_len);

    /* Append to mbuf pattern data - raw data */
    rc = os_mbuf_append(adv_data, adv_set->pattern_data, pattern_len);

    if (rc) {
        os_mbuf_free_chain(adv_data);
        assert(0);
    }

    /* Include mbuf data in advertisement */
    rc = ble_gap_ext_adv_set_data(adv_set->instance, adv_data);
    assert (rc == 0);

    /* Start advertising */
    rc = ble_gap_ext_adv_start(adv_set->instance, 0, 0);
    assert (rc == 0);

    MODLOG_DFLT(INFO, "instance %u started\n", adv_set->instance);
}

static int
rx_stress_0_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "\nConnection established; status=%d; num=%d\n",
                        event->connect.status,
                        ++rx_stress_ctx->con_stat[0].num);

            /* Stop test advert */
            ble_gap_ext_adv_stop(TEST_INSTANCE);
            ble_gap_terminate(event->connect.conn_handle,
                              BLE_ERR_REM_USER_CONN_TERM);
        } else {
            /* Connection failed; resume advertising */
            MODLOG_DFLT(INFO, "Connection failed; status=%d ",
                        event->connect.status);
        }
        return 0;
    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);
        console_printf("\033[0;32mReceived signal to switch test\033[0m\n");
        /* Add token to semaphore. Main task will start next test. */
        os_sem_release(&rx_stress_main_sem);

        return 0;
    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
rx_stress_2_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        ++rx_stress_ctx->con_stat[2].attempts_num;
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "\nConnection established; status=%d; num=%d\n",
                        event->connect.status,
                        ++rx_stress_ctx->con_stat[2].num);
        } else {
            /* Connection failed; resume advertising. */
            MODLOG_DFLT(INFO, "Connection failed; status=%d ",
                        event->connect.status);

            rx_stress_adv_start(TEST_INSTANCE);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);
        console_printf("\033[0;32m>\033[0m");
        if (rx_stress_ctx->con_stat[2].num >= MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            rx_stress_on_test_finish(2);
        } else {
            /* Connection terminated; resume advertising. */
            rx_stress_adv_start(TEST_INSTANCE);
        }
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
rx_stress_3_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        ++rx_stress_ctx->con_stat[3].attempts_num;
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "\nConnection established; status=%d; num=%d\n",
                        event->connect.status,
                        ++rx_stress_ctx->con_stat[3].num);
        } else {
            /* Connection failed; resume advertising. */
            rx_stress_adv_start(TEST_INSTANCE);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);
        console_printf("\033[0;32m>\033[0m");
        if (rx_stress_ctx->con_stat[3].num >= MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            rx_stress_on_test_finish(3);
        } else {
            /* Connection terminated; resume advertising. */
            rx_stress_adv_start(TEST_INSTANCE);
        }
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
rx_stress_4_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        ++rx_stress_ctx->con_stat[4].attempts_num;
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "Connection established; status=%d; num=%d",
                        event->connect.status,
                        ++rx_stress_ctx->con_stat[4].num);

            /* Remember connection handler */
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            rx_stress_ctx->conn_handle = event->connect.conn_handle;
        } else {
            /* Connection failed; resume advertising. */
            MODLOG_DFLT(INFO, "Connection failed; status=%d ",
                        event->connect.status);
            rx_stress_adv_start(TEST_INSTANCE);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d ", event->disconnect.reason);
        if (rx_stress_ctx->con_stat[4].prms_upd_num >=
            MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            rx_stress_on_test_finish(4);
        } else {
            /* Connection terminated; resume advertising. */
            rx_stress_adv_start(TEST_INSTANCE);
        }
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        if (event->conn_update.status != 0) {
            MODLOG_DFLT(INFO, "Connection update failed\n");
        } else {
            MODLOG_DFLT(INFO, "Connection updated; num=%d\n",
                        ++rx_stress_ctx->con_stat[4].prms_upd_num);
            console_printf("\033[0;32m>\033[0m");
        }

        if (rx_stress_ctx->con_stat[4].prms_upd_num >=
            MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            /* Test completed. */
            ble_gap_terminate(rx_stress_ctx->conn_handle,
                              BLE_ERR_REM_USER_CONN_TERM);
        }
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
rx_stress_5_con_update(void)
{
    int rc;

    /* With every next update at least one param must change. Otherwise no
     * event occurs and test will not be continued */
    struct ble_gap_upd_params params = {
        .itvl_min = BLE_GAP_INITIAL_CONN_ITVL_MIN,
        .itvl_max = BLE_GAP_INITIAL_CONN_ITVL_MAX,
        .latency = BLE_GAP_INITIAL_CONN_LATENCY,
        /* So let's change e.g. timeout value. Put ...% 2 ? 1 : 2 to make sure
         * that value won't grow significantly and will be different with every
         * iteration. */
        .supervision_timeout = BLE_GAP_INITIAL_SUPERVISION_TIMEOUT +
                               (rx_stress_ctx->con_stat[5].prms_upd_num % 2 ?
                                1 : 2),
        .min_ce_len = BLE_GAP_INITIAL_CONN_MIN_CE_LEN,
        .max_ce_len = BLE_GAP_INITIAL_CONN_MAX_CE_LEN,
    };

    rc = ble_gap_update_params(rx_stress_ctx->conn_handle, &params);

    if (rc == BLE_HS_ENOTCONN) {
        MODLOG_DFLT(INFO, "Device disconnected. Connection update failed\n");
        assert(0);
    }

    if (rc != 0) {
        MODLOG_DFLT(ERROR, "\033[0;31mError during connection update; "
                           "rc=%d\033[0m\n", rc);
        assert(0);
    }

    return rc;
}

static int
rx_stress_5_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        ++rx_stress_ctx->con_stat[5].attempts_num;

        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "Connection established; status=%d; num=%d",
                        event->connect.status,
                        ++rx_stress_ctx->con_stat[5].num);

            /* Remember connection handler */
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            rx_stress_ctx->conn_handle = event->connect.conn_handle;

            /* Update connection. */
            rc = rx_stress_5_con_update();
            assert(rc == 0);
        } else {
            /* Connection failed; resume advertising. */
            MODLOG_DFLT(INFO, "Connection failed; status=%d ",
                        event->connect.status);
            rx_stress_adv_start(TEST_INSTANCE);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d ", event->disconnect.reason);
        if (rx_stress_ctx->con_stat[5].prms_upd_num >=
            MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            rx_stress_on_test_finish(5);
        } else {
            /* Connection terminated; resume advertising. */
            rx_stress_adv_start(TEST_INSTANCE);
        }
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        if (event->conn_update.status != 0) {
            MODLOG_DFLT(INFO, "Connection update failed\n");
        } else {
            MODLOG_DFLT(INFO, "Connection updated; num=%d\n",
                        ++rx_stress_ctx->con_stat[5].prms_upd_num);
            console_printf("\033[0;32m>\033[0m");
        }

        if (rx_stress_ctx->con_stat[5].prms_upd_num >=
            MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            /* Test completed. */
            ble_gap_terminate(rx_stress_ctx->conn_handle,
                              BLE_ERR_REM_USER_CONN_TERM);
        } else {
            /* Update connection. */
            rc = rx_stress_5_con_update();
            assert(rc == 0);
        }
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
rx_stress_6_gap_event(struct ble_gap_event *event, void *arg)
{
    MODLOG_DFLT(INFO, "Event occurs=%d\n", event->type);
    return 0;
}

static int
rx_stress_7_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        ++rx_stress_ctx->con_stat[7].attempts_num;

        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "Connection established; status=%d; num=%d",
                        event->connect.status,
                        ++rx_stress_ctx->con_stat[7].num);

            /* Remember connection handler */
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            rx_stress_ctx->conn_handle = event->connect.conn_handle;
        } else {
            /* Connection failed; resume advertising. */
            MODLOG_DFLT(INFO, "Connection failed; status=%d ",
                        event->connect.status);
            rx_stress_adv_start(TEST_INSTANCE);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d ", event->disconnect.reason);
        if (rx_stress_ctx->con_stat[7].phy_upd_num >=
            MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            rx_stress_on_test_finish(7);
        } else {
            /* Connection terminated; resume advertising. */
            rx_stress_adv_start(TEST_INSTANCE);
        }
        return 0;

    case BLE_GAP_EVENT_PHY_UPDATE_COMPLETE:
        if (event->phy_updated.status != 0) {
            MODLOG_DFLT(INFO, "PHY update failed\n");
        } else {
            MODLOG_DFLT(INFO, "PHY updated; num=%d\n",
                        ++rx_stress_ctx->con_stat[7].phy_upd_num);
            console_printf("\033[0;32m>\033[0m");
        }

        if (rx_stress_ctx->con_stat[7].phy_upd_num >=
            MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            /* Test completed. */
            ble_gap_terminate(rx_stress_ctx->conn_handle,
                              BLE_ERR_REM_USER_CONN_TERM);
        }
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
rx_stress_8_con_update(void)
{
    int rc;
    uint8_t tx_phys_mask;
    uint8_t rx_phys_mask;

    ble_gap_read_le_phy(rx_stress_ctx->conn_handle, &tx_phys_mask,
                        &rx_phys_mask);

    /* With every next update at least one param must change */
    switch (rx_phys_mask) {
    case BLE_GAP_LE_PHY_1M_MASK:
        rx_phys_mask = BLE_GAP_LE_PHY_2M_MASK;
        break;
    case BLE_GAP_LE_PHY_2M_MASK:
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_CODED_PHY)
        rx_phys_mask = BLE_GAP_LE_PHY_CODED_MASK;
        break;
    case BLE_GAP_LE_PHY_CODED_MASK:
#endif
        rx_phys_mask = BLE_GAP_LE_PHY_1M_MASK;
        break;
    default:
        rx_phys_mask = BLE_GAP_LE_PHY_1M_MASK;
        break;
    }

    switch (tx_phys_mask) {
    case BLE_GAP_LE_PHY_1M_MASK:
        tx_phys_mask = BLE_GAP_LE_PHY_2M_MASK;
        break;
    case BLE_GAP_LE_PHY_2M_MASK:
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_CODED_PHY)
        tx_phys_mask = BLE_GAP_LE_PHY_CODED_MASK;
        break;
    case BLE_GAP_LE_PHY_CODED_MASK:
#endif
        tx_phys_mask = BLE_GAP_LE_PHY_1M_MASK;
        break;
    default:
        tx_phys_mask = BLE_GAP_LE_PHY_1M_MASK;
        break;
    }

    rc = ble_gap_set_prefered_le_phy(rx_stress_ctx->conn_handle,
                                     tx_phys_mask, rx_phys_mask, 0);

    if (rc == BLE_HS_ENOTCONN) {
        MODLOG_DFLT(INFO, "Device disconnected. Connection update failed\n");
        return rc;
    }

    if (rc != 0) {
        MODLOG_DFLT(ERROR, "\033[0;31mError during PHY update; "
                           "rc=%d\033[0m\n", rc);
    }

    return rc;
}

static int
rx_stress_8_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        ++rx_stress_ctx->con_stat[8].attempts_num;

        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "Connection established; status=%d; num=%d",
                        event->connect.status,
                        ++rx_stress_ctx->con_stat[8].num);

            /* Remember connection handler */
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            rx_stress_ctx->conn_handle = event->connect.conn_handle;

            /* Update connection. */
            rc = rx_stress_8_con_update();
            assert(rc == 0);
        } else {
            /* Connection failed; resume advertising. */
            MODLOG_DFLT(INFO, "Connection failed; status=%d ",
                        event->connect.status);
            rx_stress_adv_start(TEST_INSTANCE);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d ", event->disconnect.reason);
        if (rx_stress_ctx->con_stat[8].phy_upd_num >=
            MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            rx_stress_on_test_finish(8);
        } else {
            /* Connection terminated; resume advertising. */
            rx_stress_adv_start(TEST_INSTANCE);
        }
        return 0;

    case BLE_GAP_EVENT_PHY_UPDATE_COMPLETE:
        if (event->phy_updated.status != 0) {
            MODLOG_DFLT(INFO, "PHY update failed\n");
        } else {
            MODLOG_DFLT(INFO, "PHY updated; num=%d; rx:%d, tx:%d\n",
                        ++rx_stress_ctx->con_stat[8].phy_upd_num,
                        event->phy_updated.rx_phy, event->phy_updated.tx_phy);
            console_printf("\033[0;32m>\033[0m");
        }

        if (rx_stress_ctx->con_stat[8].phy_upd_num >=
            MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            /* Test completed. */
            ble_gap_terminate(rx_stress_ctx->conn_handle,
                              BLE_ERR_REM_USER_CONN_TERM);
        } else {
            /* Update connection. */
            rc = rx_stress_8_con_update();
            assert(rc == 0);
        }
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
rx_stress_9_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        ++rx_stress_ctx->con_stat[9].attempts_num;

        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "Connection established; status=%d; num=%d",
                        event->connect.status,
                        ++rx_stress_ctx->con_stat[9].num);
            console_printf("\033[0;32m>\033[0m");

            /* Remember max number of established connections */
            if (rx_stress_ctx->con_stat[9].num >
                rx_stress_ctx->con_stat[9].max_num) {
                rx_stress_ctx->con_stat[9].max_num = rx_stress_ctx->con_stat[9].num;
            }
        } else {
            /* Connection failed; resume advertising. */
            MODLOG_DFLT(INFO, "Connection failed; status=%d ",
                        event->connect.status);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d ", event->disconnect.reason);
        console_printf("\033[0;31mX\033[0m");
        MODLOG_DFLT(INFO, "Connections num: %d\n",
                    --rx_stress_ctx->con_stat[9].num);

        if (rx_stress_ctx->con_stat[9].num != 0 &&
            rx_stress_ctx->con_stat[9].num <
            MYNEWT_VAL(BLE_MAX_CONNECTIONS) + 1) {
            rx_stress_adv_start_with_rand_addr(TEST_INSTANCE);
        } else {
            /* When TX device has terminated all connections, stop advertising. */
            ble_gap_ext_adv_stop(TEST_INSTANCE);
            rx_stress_on_test_finish(9);
        }
        return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        /* Stop test when TX device has terminated all connections or
         * number of connections has reached the max possible value. */
        if (rx_stress_ctx->con_stat[9].num != 0 &&
            rx_stress_ctx->con_stat[9].num <
            MYNEWT_VAL(BLE_MAX_CONNECTIONS) + 1) {
            rx_stress_adv_start_with_rand_addr(TEST_INSTANCE);
        }
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
    }
    return 0;
}

static void
tx_stress_10_l2cap_update_event(uint16_t conn_handle, int status, void *arg)
{
    if (status == 0) {
        MODLOG_DFLT(INFO, "L2CAP params updated\n");
    } else {
        MODLOG_DFLT(INFO, "L2CAP params update failed; rc=%d\n", status);
        assert(0);
    }
}

static int
rx_stress_10_l2cap_event(struct ble_l2cap_event *event, void *arg)
{
    int rc;
    struct os_mbuf *data_buf;
    static int data_len = 1000;
    static int send_cnt = 0;
    static bool stalled = false;
    struct ble_l2cap_chan_info chan_info;

    switch (event->type) {
    case BLE_L2CAP_EVENT_COC_CONNECTED:
        if (event->connect.status) {
            MODLOG_DFLT(INFO, "LE COC error: %d\n", event->connect.status);
            return 0;
        }

        ble_l2cap_get_chan_info(event->connect.chan, &chan_info);

        MODLOG_DFLT(INFO,
                    "LE COC connected, conn: %d, chan: 0x%08lx, scid: 0x%04x, "
                    "dcid: 0x%04x, our_mtu: 0x%04x, peer_mtu: 0x%04x\n",
                    event->connect.conn_handle,
                    (uint32_t) event->connect.chan,
                    chan_info.scid,
                    chan_info.dcid,
                    chan_info.our_l2cap_mtu,
                    chan_info.peer_l2cap_mtu);

        struct ble_l2cap_sig_update_params params = {
            .itvl_min = 0x0006,//BLE_GAP_INITIAL_CONN_ITVL_MIN
            .itvl_max = 0x0006,//BLE_GAP_INITIAL_CONN_ITVL_MIN
            .slave_latency = 0x0000,
            .timeout_multiplier = 0x0100,
        };

        rc = ble_l2cap_sig_update(event->connect.conn_handle, &params,
                                  &tx_stress_10_l2cap_update_event, NULL);
        assert(rc == 0);
        return 0;

    case BLE_L2CAP_EVENT_COC_DISCONNECTED:
        MODLOG_DFLT(INFO, "LE CoC disconnected, chan: 0x%08lx\n",
                    (uint32_t) event->disconnect.chan);
        return 0;

    case BLE_L2CAP_EVENT_COC_ACCEPT:
        stress_l2cap_coc_accept(event->accept.peer_sdu_size,
                                event->accept.chan);
        return 0;

    case BLE_L2CAP_EVENT_COC_DATA_RECEIVED:
        stress_l2cap_coc_recv(event->receive.chan, event->receive.sdu_rx);

        MODLOG_DFLT(INFO, "L2CAP server received data; num=%d\n",
                    ++rx_stress_ctx->rcv_num);
        rx_stress_ctx->chan = event->receive.chan;

        /* In this use case, receiving any data by RX device L2CAP server means
         * request from TX device to send data. */

        /* Do not send if stalled on the last sending. */
        if (stalled) {
            return 0;
        }
        break;

    case BLE_L2CAP_EVENT_COC_TX_UNSTALLED:
        MODLOG_DFLT(INFO, "L2CAP unstalled event\n");

        stalled = false;

        /* Send if was stalled on the last request to send. */
        if (rx_stress_ctx->rcv_num > send_cnt) {
            break;
        }
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other L2CAP event occurs: %d\n", event->type);
        return 0;
    }

    /* Send pattern data */

    /* Get mbuf for adv data */
    data_buf = os_msys_get_pkthdr(data_len, 0);

    MODLOG_DFLT(INFO, "Data buf %s\n", data_buf ? "OK" : "NOK");
    assert(data_buf != NULL);

    /* The first 2 bytes of data is the size of appended pattern data. */
    rc = os_mbuf_append(data_buf, (uint8_t[]) {data_len >> 8, data_len},
                        2);
    if (rc) {
        os_mbuf_free_chain(data_buf);
        assert(0);
    }

    /* Fill mbuf with the pattern */
    stress_fill_mbuf_with_pattern(data_buf, data_len);

    /* Send data */
    rc = ble_l2cap_send(rx_stress_ctx->chan, data_buf);
    MODLOG_DFLT(INFO, "Return code=%d\n", rc);
    if (rc) {
        MODLOG_DFLT(INFO, "L2CAP stalled - waiting\n");
        stalled = true;
    }

    MODLOG_DFLT(INFO, " %d, %d\n", ++send_cnt, data_len);
    data_len += 500;

    return 0;
}

static int
rx_stress_10_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc out_desc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        ++rx_stress_ctx->con_stat[10].attempts_num;

        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "Connection established; status=%d; num=%d",
                        event->connect.status,
                        ++rx_stress_ctx->con_stat[10].num);

            ble_gap_conn_find(event->connect.conn_handle, &out_desc);
            MODLOG_DFLT(INFO, "Address %s",
                        addr_str(out_desc.peer_id_addr.val));
        } else {
            /* Connection failed; resume advertising. */
            MODLOG_DFLT(INFO, "Connection failed; status=%d ",
                        event->connect.status);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d ", event->disconnect.reason);
        rx_stress_ctx->completed[10] = true;
        rx_stress_on_test_finish(10);
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
rx_stress_11_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        ++rx_stress_ctx->con_stat[11].attempts_num;
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "\nConnection established; status=%d; num=%d\n",
                        event->connect.status,
                        ++rx_stress_ctx->con_stat[11].num);
        } else {
            MODLOG_DFLT(INFO, "\033[0;31mError: connection attempt failed; "
                              "status=%d\033[0m\n", event->connect.status);
        }

        ble_gap_terminate(event->connect.conn_handle,
                          BLE_ERR_REM_USER_CONN_TERM);
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);
        console_printf("\033[0;32m>\033[0m");
        if (rx_stress_ctx->con_stat[11].num >= MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            rx_stress_on_test_finish(11);
        } else {
            /* Connection terminated; resume advertising. */
            rx_stress_adv_start(TEST_INSTANCE);
        }
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
rx_stress_12_gap_event(struct ble_gap_event *event, void *arg)
{
    int om_len = 10000;
    struct os_mbuf *om;
    int64_t us = 0;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "\nConnection established; status=%d; num=%d\n",
                        event->connect.status,
                        ++rx_stress_ctx->con_stat[12].num);
            rx_stress_ctx->conn_handle = event->connect.conn_handle;

            break;
        } else {
            /* Connection failed; resume advertising */
            MODLOG_DFLT(INFO, "Connection failed; status=%d ",
                        event->connect.status);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);

        rx_stress_ctx->s12_notif_time = rx_stress_ctx->time_sum /
                                        rx_stress_ctx->send_num;

        MODLOG_DFLT(INFO, "Average time: %d us\n",
                    rx_stress_ctx->s12_notif_time);

        rx_stress_on_test_finish(12);
        return 0;

    case BLE_GAP_EVENT_NOTIFY_TX:
        rx_stress_ctx->end_us = os_get_uptime_usec();
        MODLOG_DFLT(INFO, "Notify TX event\n");

        if (!event->notify_tx.status) {
            /* Send next only after previous indication is done */
            return 0;
        }
        assert(event->notify_tx.status == BLE_HS_EDONE);

        if (rx_stress_ctx->send_num++ >= MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            ble_gap_terminate(event->notify_tx.conn_handle,
                              BLE_ERR_REM_USER_CONN_TERM);
            return 0;
        }

        /* Time of data sending */
        us = rx_stress_ctx->end_us - rx_stress_ctx->begin_us;
        console_printf("Indication time: %lld\n", us);
        rx_stress_ctx->time_sum += us;
        console_printf("\033[0;32m>\033[0m");
        break;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }

    /* Indicate data pattern */
    rx_stress_ctx->begin_us = os_get_uptime_usec();
    om = os_msys_get_pkthdr(om_len, 0);
    stress_fill_mbuf_with_pattern(om, om_len);
    rc = ble_gattc_indicate_custom(rx_stress_ctx->conn_handle, hrs_hrm_handle,
                                   om);
    assert(rc == 0);
    return 0;
}

static int
rx_stress_13_gap_event(struct ble_gap_event *event, void *arg)
{
    int rc;
    struct os_mbuf *om = NULL;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "\nConnection established; status=%d; num=%d\n",
                        event->connect.status,
                        ++rx_stress_ctx->con_stat[13].num);
            rx_stress_ctx->conn_handle = event->connect.conn_handle;

            rx_stress_ctx->begin_us = os_get_uptime_usec();
            break;
        } else {
            /* Connection failed; resume advertising */
            MODLOG_DFLT(INFO, "Connection failed; status=%d ",
                        event->connect.status);
            assert(0);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);

        rx_stress_ctx->time_sum = rx_stress_ctx->end_us -
                                  rx_stress_ctx->begin_us;

        rx_stress_ctx->s13_notif_time = rx_stress_ctx->time_sum /
                                        rx_stress_ctx->send_num;

        MODLOG_DFLT(INFO, "Average time: %lld us\n",
                    rx_stress_ctx->s13_notif_time);
        rx_stress_on_test_finish(13);
        return 0;

    case BLE_GAP_EVENT_NOTIFY_TX:
        MODLOG_DFLT(INFO, "Notify TX event; num=%d\n",
                    ++rx_stress_ctx->send_num);
        assert(event->notify_tx.status == 0);

        if (rx_stress_ctx->send_num >= MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            rx_stress_ctx->end_us = os_get_uptime_usec();
            ble_gap_terminate(event->connect.conn_handle,
                              BLE_ERR_REM_USER_CONN_TERM);
            return 0;
        }
        break;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }

    om = ble_hs_mbuf_from_flat(test_6_pattern, 10);
    rc = ble_gattc_notify_custom(rx_stress_ctx->conn_handle,
                                 hrs_hrm_handle, om);
    assert(rc == 0);
    return 0;
}

static int
rx_stress_14_gap_event(struct ble_gap_event *event, void *arg)
{
    int bytes_num = 10000;
    static struct os_mbuf *om = NULL;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "\nConnection established; status=%d; num=%d\n",
                        event->connect.status,
                        ++rx_stress_ctx->con_stat[14].num);
            rx_stress_ctx->conn_handle = event->connect.conn_handle;
        } else {
            /* Connection failed; resume advertising */
            MODLOG_DFLT(INFO, "Connection failed; status=%d ",
                        event->connect.status);
            assert(0);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);

        rx_stress_ctx->s14_notif_time = rx_stress_ctx->time_sum /
                                        rx_stress_ctx->send_num;

        MODLOG_DFLT(INFO, "Average time: %d us\n",
                    rx_stress_ctx->s14_notif_time);

        rx_stress_on_test_finish(14);
        return 0;

    case BLE_GAP_EVENT_NOTIFY_TX:
        MODLOG_DFLT(INFO, "Notify TX event\n");
        assert(event->notify_tx.status == 0);
        return 0;

    case BLE_GAP_EVENT_SUBSCRIBE:
        MODLOG_DFLT(INFO, "Subscribe event\n");

        if (event->subscribe.cur_notify) {
            MODLOG_DFLT(INFO, "Notification subscribed\n");

            ++rx_stress_ctx->send_num;

            /* Notify data pattern */
            om = ble_hs_mbuf_from_flat(test_6_pattern, bytes_num);

            rc = ble_gattc_notify_custom(rx_stress_ctx->conn_handle,
                                         hrs_hrm_handle, om);
            assert(rc == 0);

            console_printf("\033[0;32m>\033[0m");
        } else if (event->subscribe.prev_notify) {
            MODLOG_DFLT(INFO, "Notification unsubscribed\n");
        } else {
            MODLOG_DFLT(INFO, "Other subscription\n");
        }
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
rx_stress_15_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        ++rx_stress_ctx->con_stat[15].attempts_num;
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "\nConnection established; status=%d; num=%d\n",
                        event->connect.status,
                        ++rx_stress_ctx->con_stat[15].num);
        } else {
            MODLOG_DFLT(INFO, "\033[0;31mError: connection attempt failed; "
                              "status=%d\033[0m\n", event->connect.status);
            assert(0);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);
        console_printf("\033[0;32m>\033[0m");
        if (rx_stress_ctx->con_stat[15].num >= MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            rx_stress_on_test_finish(15);
        } else {
            /* Connection terminated; resume advertising. */
            rx_stress_adv_start(TEST_INSTANCE);
        }
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

/* Advert settings for each test. */
static struct rx_stress_adv_set rx_stress_adv_sets[] = {
    {
        .instance = SWITCHER_INSTANCE,
        .instance_uuid128 = rx_stress_uuid128[0],
        .legacy_pdu = LEGACY_ADVERT,
        .cb = rx_stress_0_gap_event,
        .pattern_data = NULL,
        .pattern_len = 0,
    },
    {
        .instance = SWITCHER_INSTANCE,
        .instance_uuid128 = rx_stress_uuid128[0],
        .legacy_pdu = LEGACY_ADVERT,
        .cb = rx_stress_0_gap_event,
        .pattern_data = NULL,
        .pattern_len = 0,
    },
    {
        .instance = TEST_INSTANCE,
        .instance_uuid128 = rx_stress_uuid128[2],
        .legacy_pdu = LEGACY_ADVERT,
        .cb = rx_stress_2_gap_event,
        .pattern_data = NULL,
        .pattern_len = 0,
    },
    {
        .instance = TEST_INSTANCE,
        .instance_uuid128 = rx_stress_uuid128[3],
        .legacy_pdu = EXTENDED_ADVERT,
        .cb = rx_stress_3_gap_event,
        .pattern_data = NULL,
        .pattern_len = 0,
    },
    {
        .instance = TEST_INSTANCE,
        .instance_uuid128 = rx_stress_uuid128[4],
        .legacy_pdu = LEGACY_ADVERT,
        .cb = rx_stress_4_gap_event,
        .pattern_data = NULL,
        .pattern_len = 0,
    },
    {
        .instance = TEST_INSTANCE,
        .instance_uuid128 = rx_stress_uuid128[5],
        .legacy_pdu = LEGACY_ADVERT,
        .cb = rx_stress_5_gap_event,
        .pattern_data = NULL,
        .pattern_len = 0,
    },
    {
        .instance = TEST_INSTANCE,
        .instance_uuid128 = rx_stress_uuid128[6],
        .legacy_pdu = EXTENDED_ADVERT,
        .cb = rx_stress_6_gap_event,
        .pattern_data = test_6_pattern,
        .pattern_len = 1640,
    },
    {
        .instance = TEST_INSTANCE,
        .instance_uuid128 = rx_stress_uuid128[7],
        .legacy_pdu = EXTENDED_ADVERT,
        .cb = rx_stress_7_gap_event,
        .pattern_data = NULL,
        .pattern_len = 0,
    },
    {
        .instance = TEST_INSTANCE,
        .instance_uuid128 = rx_stress_uuid128[8],
        .legacy_pdu = EXTENDED_ADVERT,
        .cb = rx_stress_8_gap_event,
        .pattern_data = NULL,
        .pattern_len = 0,
    },
    {
        .instance = TEST_INSTANCE,
        .instance_uuid128 = rx_stress_uuid128[9],
        .legacy_pdu = EXTENDED_ADVERT,
        .cb = rx_stress_9_gap_event,
        .pattern_data = NULL,
        .pattern_len = 0,
    },
    {
        .instance = TEST_INSTANCE,
        .instance_uuid128 = rx_stress_uuid128[10],
        .legacy_pdu = EXTENDED_ADVERT,
        .cb = rx_stress_10_gap_event,
        .pattern_data = NULL,
        .pattern_len = 0,
    },
    {
        .instance = TEST_INSTANCE,
        .instance_uuid128 = rx_stress_uuid128[11],
        .legacy_pdu = EXTENDED_ADVERT,
        .cb = rx_stress_11_gap_event,
        .pattern_data = NULL,
        .pattern_len = 0,
    },
    {
        .instance = TEST_INSTANCE,
        .instance_uuid128 = rx_stress_uuid128[12],
        .legacy_pdu = EXTENDED_ADVERT,
        .cb = rx_stress_12_gap_event,
        .pattern_data = NULL,
        .pattern_len = 0,
    },
    {
        .instance = TEST_INSTANCE,
        .instance_uuid128 = rx_stress_uuid128[13],
        .legacy_pdu = EXTENDED_ADVERT,
        .cb = rx_stress_13_gap_event,
        .pattern_data = NULL,
        .pattern_len = 0,
    },
    {
        .instance = TEST_INSTANCE,
        .instance_uuid128 = rx_stress_uuid128[14],
        .legacy_pdu = EXTENDED_ADVERT,
        .cb = rx_stress_14_gap_event,
        .pattern_data = NULL,
        .pattern_len = 0,
    },
    {
        .instance = TEST_INSTANCE,
        .instance_uuid128 = rx_stress_uuid128[15],
        .legacy_pdu = EXTENDED_ADVERT,
        .cb = rx_stress_15_gap_event,
        .pattern_data = NULL,
        .pattern_len = 0,
    },
};

static void
rx_stress_start(int test_num)
{
    int rc;

    /* Init semaphore with 0 tokens. */
    os_sem_init(&rx_stress_main_sem, 0);

    console_printf("\033[1;36mStart test num %d - ", test_num);

    /* Start test. */
    switch (test_num) {
    case 2:
        console_printf("Stress Connect/Disconnect legacy\033[0m\n");
        rx_stress_simple_adv(&rx_stress_adv_sets[2]);
        break;
    case 3:
        console_printf("Stress Connect/Disconnect ext adv\033[0m\n");
        rx_stress_simple_adv(&rx_stress_adv_sets[3]);
        break;
    case 4:
        console_printf("Stress connection params update (TX)\033[0m\n");
        rx_stress_simple_adv(&rx_stress_adv_sets[4]);
        break;
    case 5:
        console_printf("Stress connection params update (RX)\033[0m\n");
        rx_stress_simple_adv(&rx_stress_adv_sets[5]);
        break;
    case 6:
        /* Start SWITCHER advert that gives possibility to remotely start
         * next test advert */
        console_printf("Stress Scan\033[0m\n");
        rx_stress_simple_adv(&rx_stress_adv_sets[0]);
        rx_stress_simple_adv(&rx_stress_adv_sets[6]);
        break;
    case 7:
        console_printf("Stress PHY Update (TX)\033[0m\n");
        rx_stress_simple_adv(&rx_stress_adv_sets[7]);
        break;
    case 8:
        console_printf("Stress PHY Update (RX)\033[0m\n");
        rx_stress_simple_adv(&rx_stress_adv_sets[8]);
        break;
    case 9:
        console_printf("Stress multi connection\033[0m\n");
        rx_stress_simple_adv(&rx_stress_adv_sets[9]);
        break;
    case 10:
        console_printf("Stress L2CAP send\033[0m\n");
        rc = ble_l2cap_create_server(1, STRESS_COC_MTU,
                                     rx_stress_10_l2cap_event, NULL);
        assert(rc == 0);
        rx_stress_simple_adv(&rx_stress_adv_sets[10]);
        break;
    case 11:
        console_printf("Stress Advertise/Connect/Disconnect\033[0m\n");
        rx_stress_simple_adv(&rx_stress_adv_sets[11]);
        break;
    case 12:
        console_printf("Stress GATT indication\033[0m\n");
        rx_stress_simple_adv(&rx_stress_adv_sets[12]);
        break;
    case 13:
        console_printf("Stress GATT notification\033[0m\n");
        rx_stress_simple_adv(&rx_stress_adv_sets[13]);
        break;
    case 14:
        console_printf("Stress GATT Subscribe/Notify/Unsubscribe\033[0m\n");
        rx_stress_simple_adv(&rx_stress_adv_sets[14]);
        break;
    case 15:
        console_printf("Stress Connect/Send/Disconnect\033[0m\n");
        rx_stress_simple_adv(&rx_stress_adv_sets[15]);
        break;
    default:
        console_printf("\033[0;31mFound test, but do not know how to perform."
                       "\033[0m\n");
        assert(0);
    }

    /* Wait for the test to finish. Then 1 token will be released
    * allowing to pass through semaphore. */
    os_sem_pend(&rx_stress_main_sem, OS_TIMEOUT_NEVER);

    ble_gap_ext_adv_stop(SWITCHER_INSTANCE);

    stress_clear_ctx_reusable_var(rx_stress_ctx);
}

static void
stress_uuid_init()
{
    uint8_t i;

    for (i = 0; i < STRESS_UUIDS_NUM; ++i) {
        /* Fill all 16 bytes of UUID128 */
        rx_stress_uuid128[i][0] = 0xC0;
        rx_stress_uuid128[i][1] = 0xDE;
        rx_stress_uuid128[i][2] = i;
        memcpy(&rx_stress_uuid128[i][3], MYNEWT_VAL(BLE_STRESS_UUID_BASE), 13);
    }
}

static void
rx_stress_read_command_cb(void)
{
    console_printf("Start testing\n");
    os_sem_release(&rx_stress_main_sem);
}

static void
rx_stress_main_task_fn(void *arg)
{
    int i;

    stress_uuid_init();

    console_printf("\033[1;36mRX device\033[0m\n");
    console_printf("Press ENTER to start: \n");
    console_init(&rx_stress_read_command_cb);

    /* Waite for pressing ENTER in console */
    os_sem_pend(&rx_stress_main_sem, OS_TIMEOUT_NEVER);

    /* Standard tests perform */
    for (i = 11; i < STRESS_UUIDS_NUM; ++i) {
        if (i == 7 || i == 8 || i == 13) {
            /* 7,8: PHY update tests cause that the device during the next test
             * will stuck somewhere and will reset. Skip them for now.
             * 13: Should work after fixing ble_gattc_notify_custom (nimble issue on GitHub)*/
            continue;
        }
        /* Start test. */
        rx_stress_start(i);
    }

    /* Print tests results */
    com_stress_print_report(rx_stress_ctx);

    /* Task should never return */
    while (1) {
    }
}

void
rx_stress_start_auto()
{
    /* Start task that will run all stress tests one by one. */
    os_task_init(&rx_stress_main_task, "rx_stress_main_task",
                 rx_stress_main_task_fn, NULL, RX_STRESS_MAIN_TASK_PRIO,
                 OS_WAIT_FOREVER, rx_stress_main_task_stack,
                 RX_STRESS_MAIN_TASK_STACK_SIZE);
}
