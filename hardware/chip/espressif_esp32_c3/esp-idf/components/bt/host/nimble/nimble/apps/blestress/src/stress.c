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

#include "stress.h"

void
com_stress_print_report(const struct com_stress_test_ctx *test_ctxs)
{
    console_printf("\033[0;32mAll tests completed\033[0m\n");
    console_printf("Tests results:\n");

    console_printf(
        "\033[0;33mUse case 1 - Stress Connect -> Connect Cancel: \n\033[0m");
    console_printf("Con attempts = %d\n", test_ctxs->con_stat[1].attempts_num);
    console_printf("Con success = %d\n", test_ctxs->con_stat[1].num);

    console_printf(
        "\033[0;33mUse case 2 - Stress Connect/Disconnect legacy: \n\033[0m");
    console_printf("Con attempts = %d\n", test_ctxs->con_stat[2].attempts_num);
    console_printf("Con success = %d\n", test_ctxs->con_stat[2].num);

    console_printf(
        "\033[0;33mUse case 3 - Stress Connect/Disconnect ext adv: \n\033[0m");
    console_printf("Con attempts = %d\n", test_ctxs->con_stat[3].attempts_num);
    console_printf("Con success = %d\n", test_ctxs->con_stat[3].num);

    console_printf(
        "\033[0;33mUse case 4 - Stress connection params update (TX): \n\033[0m");
    console_printf("Params updates = %d\n",
                   test_ctxs->con_stat[4].prms_upd_num);

    console_printf(
        "\033[0;33mUse case 5 - Stress connection params update (RX): \n\033[0m");
    console_printf("Params updates = %d\n",
                   test_ctxs->con_stat[5].prms_upd_num);

    console_printf("\033[0;33mUse case 6 - Stress Scan: \n\033[0m");
    console_printf("Received first packets = %d\n",
                   test_ctxs->s6_rcv_adv_first);
    console_printf("Received all packets = %d\n", test_ctxs->s6_rcv_adv_suc);

    console_printf("\033[0;33mUse case 7 - Stress PHY Update (TX): \n\033[0m");
    console_printf("PHY updates = %d\n", test_ctxs->con_stat[7].phy_upd_num);

    console_printf("\033[0;33mUse case 8 - Stress PHY Update (RX): \n\033[0m");
    console_printf("PHY updates = %d\n", test_ctxs->con_stat[8].phy_upd_num);

    console_printf(
        "\033[0;33mUse case 9 - Stress multi connection: \n\033[0m");
    console_printf("Max reached num of connections = %d\n",
                   test_ctxs->con_stat[9].max_num);

    console_printf("\033[0;33mUse case 10 - Stress L2CAP send: \n\033[0m");
    console_printf("Average bit rate = %d\n", test_ctxs->s10_bit_rate);
    console_printf("Max received MTU = %lld\n", test_ctxs->s10_max_mtu);

    console_printf("\033[0;33mUse case 11 - "
                   "Stress Advertise/Connect/Continue adv \n\033[0m");
//    console_printf(" = %d\n",);

    console_printf("\033[0;33mUse case 12 - "
                   "Stress GATT indication: \n\033[0m");
    console_printf("Average bit rate = %d\n", test_ctxs->s12_notif_time);

    console_printf("\033[0;33mUse case 13 - "
                   "Stress GATT notification: \n\033[0m");
    console_printf("Average time = %d\n", test_ctxs->s13_notif_time);

    console_printf("\033[0;33mUse case 14 - "
                   "Stress GATT Subscribe/Notify/Unsubscribe: \n\033[0m");
    console_printf("Average time = %d\n", test_ctxs->s14_notif_time);

    console_printf("\033[0;33mUse case 15 - "
                   "Stress Connect/Send/Disconnect: \n\033[0m");
    console_printf("Con num = %d\n", test_ctxs->con_stat[15].num);
}

void
stress_clear_ctx_reusable_var(struct com_stress_test_ctx *ctx)
{
    ctx->cur_test_id = 0;
    ctx->dev_addr.type = 0;
    ctx->dev_addr.type = 0;
    ctx->conn_handle = 0;
    ctx->chan = 0;
    ctx->rcv_data_bytes = 0;
    ctx->rcv_num = 0;
    ctx->send_num = 0;
    ctx->begin_us = 0;
    ctx->end_us = 0;
    ctx->time_sum = 0;
    ctx->bytes_sum = 0;
    ctx->timeout_flag = 0;
    ctx->rcv_data_flag = 0;
}

int
stress_fill_mbuf_with_pattern(struct os_mbuf *om, uint16_t len)
{
    int rc, i, mul, rest;

    mul = len / STRESS_PAT_LEN;
    rest = len % STRESS_PAT_LEN;

    for (i = 0; i < mul; ++i) {
        rc = os_mbuf_append(om, &test_6_pattern[29], STRESS_PAT_LEN);

        if (rc) {
            os_mbuf_free_chain(om);
            assert(0);
        }
    }

    rc = os_mbuf_append(om, &test_6_pattern[29], rest);

    if (rc) {
        os_mbuf_free_chain(om);
        assert(0);
    }

    return rc;
}

void
stress_l2cap_coc_recv(struct ble_l2cap_chan *chan, struct os_mbuf *sdu)
{
    int rc;
    console_printf("LE CoC SDU received, chan: 0x%08lx, data len %d\n",
                   (uint32_t) chan, OS_MBUF_PKTLEN(sdu));

    rc = os_mbuf_free_chain(sdu);
    assert(rc == 0);

    /* Get buffer for data chain */
    sdu = os_msys_get_pkthdr(STRESS_COC_MTU, 0);
    assert(sdu != NULL);

    /* Receive data chain */
    rc = ble_l2cap_recv_ready(chan, sdu);
    assert(rc == 0);
}

void
stress_l2cap_coc_accept(uint16_t peer_mtu, struct ble_l2cap_chan *chan)
{
    struct os_mbuf *sdu_rx;
    int rc;

    console_printf("LE CoC accepting, chan: 0x%08lx, peer_mtu %d\n",
                   (uint32_t) chan, peer_mtu);

    sdu_rx = os_msys_get_pkthdr(STRESS_COC_MTU, 0);
    assert(sdu_rx != NULL);

    rc = ble_l2cap_recv_ready(chan, sdu_rx);
    assert(rc == 0);
}

void
stress_start_timer(uint32_t timeout_ms, os_event_fn *ev_cb)
{
    int rc;
    os_callout_stop(&stress_timer_callout);

    os_callout_init(&stress_timer_callout, os_eventq_dflt_get(), ev_cb, NULL);

    rc = os_callout_reset(&stress_timer_callout,
                          os_time_ms_to_ticks32(timeout_ms));

    assert(rc == 0);
}

int64_t
stress_calc_bit_rate(int64_t us, int64_t bytes_num)
{
    int bit_rate;

    /* Multiply by 1000000 so you don't lose accuracy */
    bit_rate = 1000000 * bytes_num / us;

    return bit_rate;
}

static int
stress_disc_dsc_fn(uint16_t conn_handle,
                   const struct ble_gatt_error *error,
                   uint16_t chr_val_handle,
                   const struct ble_gatt_dsc *dsc,
                   void *arg)
{
    struct stress_gatt_search_ctx *search_ctx;
    static bool found = false;

    search_ctx = (struct stress_gatt_search_ctx *) arg;

    if (error->status == 0) {
        if (!ble_uuid_cmp(&dsc->uuid.u, &search_ctx->dsc_uuid.u) && !found) {
            MODLOG_DFLT(INFO, "Found chr descriptor\n");
            search_ctx->dsc_handle = dsc->handle;
            MODLOG_DFLT(INFO, "uuid=%#06x; handle=%#06x", dsc->uuid.u16.value,
                        dsc->handle);
            found = true;
        }
        return 0;
    }

    if (error->status == BLE_HS_EDONE) {
        MODLOG_DFLT(INFO, "Done descriptor discovery\n");

        if (found) {
            found = false;
            search_ctx->disc_end_fn(search_ctx);
            return 0;
        }

        MODLOG_DFLT(ERROR, "\033[0;31mDid not find particular descriptor"
                           "\033[0m\n");
        return 0;
    }

    MODLOG_DFLT(ERROR, "\033[0;31mError during descriptor discovery"
                       "\033[0m\n");
    assert(0);
    return 0;
}

static int
stress_disc_chr_fn(uint16_t conn_handle,
                   const struct ble_gatt_error *error,
                   const struct ble_gatt_chr *chr, void *arg)
{
    int rc;
    struct stress_gatt_search_ctx *search_ctx;
    static bool found = false;

    search_ctx = (struct stress_gatt_search_ctx *) arg;

    if (error->status == 0) {
        MODLOG_DFLT(INFO, "Found characteristic\n");
        search_ctx->chr_start_handle = chr->val_handle;
        found = true;
        return 0;
    }

    if (error->status == BLE_HS_EDONE) {
        MODLOG_DFLT(INFO, "Done characteristic discovery\n");

        if (found) {
            found = false;

            if (search_ctx->search_goal == STRESS_FIND_CHR) {
                search_ctx->disc_end_fn(search_ctx);
                return 0;
            }

            rc = ble_gattc_disc_all_dscs(conn_handle,
                                         search_ctx->chr_start_handle,
                                         search_ctx->srv_end_handle,
                                         &stress_disc_dsc_fn, search_ctx);
            assert(rc == 0);
            return 0;
        }

        MODLOG_DFLT(ERROR, "\033[0;31mDid not find particular "
                           "characteristic\033[0m\n");
        return 0;
    }

    MODLOG_DFLT(ERROR,
                "\033[0;31mError during characteristic discovery\033[0m\n");
    assert(0);
    return 0;
}

static int
stress_disc_svc_fn(uint16_t conn_handle, const struct ble_gatt_error *error,
                   const struct ble_gatt_svc *service, void *arg)
{
    int rc;
    struct stress_gatt_search_ctx *search_ctx = NULL;
    static bool found = false;

    search_ctx = (struct stress_gatt_search_ctx *) arg;

    if (error->status == 0) {
        MODLOG_DFLT(INFO, "Found service\n");
        search_ctx->srv_start_handle = service->start_handle;
        search_ctx->srv_end_handle = service->end_handle;
        found = true;
        return 0;
    }

    if (error->status == BLE_HS_EDONE) {
        MODLOG_DFLT(INFO, "Done service discovery\n");

        if (found) {
            found = false;
            if (search_ctx->search_goal == STRESS_FIND_SRV) {
                search_ctx->disc_end_fn(search_ctx);
                return 0;
            }

            rc = ble_gattc_disc_chrs_by_uuid(conn_handle,
                                             search_ctx->srv_start_handle,
                                             search_ctx->srv_end_handle,
                                             &search_ctx->chr_uuid.u,
                                             &stress_disc_chr_fn,
                                             search_ctx);
            MODLOG_DFLT(INFO, "rc=%d\n", rc);
            assert(rc == 0);
            return 0;
        }

        MODLOG_DFLT(ERROR,
                    "\033[0;31mDid not find particular service\033[0m\n");
        return 0;
    }

    MODLOG_DFLT(ERROR, "\033[0;31mError during service discovery\033[0m\n");
    assert(0);
    return 0;
}

static void
stress_gatt_find_handle(uint16_t conn_handle, const ble_uuid_t *srv_uuid,
                        const ble_uuid_t *chr_uuid, const ble_uuid_t *dsc_uuid,
                        stress_gatt_disc_end_fn *disc_end_fn,
                        uint8_t search_goal)
{
    static struct stress_gatt_search_ctx search_ctx;
    int rc;

    search_ctx.conn_handle = conn_handle;
    ble_uuid_copy((ble_uuid_any_t *) &search_ctx.srv_uuid, srv_uuid);
    ble_uuid_copy((ble_uuid_any_t *) &search_ctx.chr_uuid, chr_uuid);
    ble_uuid_copy((ble_uuid_any_t *) &search_ctx.dsc_uuid, dsc_uuid);
    search_ctx.disc_end_fn = disc_end_fn;
    search_ctx.search_goal = search_goal;
    search_ctx.srv_start_handle = 0;
    search_ctx.srv_end_handle = 0;
    search_ctx.chr_start_handle = 0;
    search_ctx.dsc_handle = 0;

    rc = ble_gattc_disc_svc_by_uuid(conn_handle, srv_uuid, &stress_disc_svc_fn,
                                    &search_ctx);
    assert(rc == 0);
}

void
stress_find_svc_handle(uint16_t conn_handle, const ble_uuid_t *srv_uuid,
                       stress_gatt_disc_end_fn *disc_end_fn)
{
    stress_gatt_find_handle(conn_handle, srv_uuid, NULL, NULL, disc_end_fn,
                            STRESS_FIND_SRV);
}

void
stress_find_chr_handle(uint16_t conn_handle, const ble_uuid_t *srv_uuid,
                       const ble_uuid_t *chr_uuid,
                       stress_gatt_disc_end_fn *disc_end_fn)
{
    stress_gatt_find_handle(conn_handle, srv_uuid, chr_uuid, NULL, disc_end_fn,
                            STRESS_FIND_CHR);
}

void
stress_find_dsc_handle(uint16_t conn_handle, const ble_uuid_t *srv_uuid,
                       const ble_uuid_t *chr_uuid, const ble_uuid_t *dsc_uuid,
                       stress_gatt_disc_end_fn *disc_end_fn)
{
    stress_gatt_find_handle(conn_handle, srv_uuid, chr_uuid, dsc_uuid,
                            disc_end_fn, STRESS_FIND_DSC);
}
