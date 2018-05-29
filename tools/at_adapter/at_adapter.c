/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "lwip/netif.h"
#include "lwip/err.h"
#include "lwip/priv/tcpip_priv.h"
#include "lwip/etharp.h"
#include "aos/aos.h"
#include "at_adapter.h"
#include "hal/wifi.h"
#include "atparser.h"

#define TAG "at_adapter"

static struct netif at_netif;
char if_name[3] = "at";

#ifdef DEBUG
static void print_payload(uint32_t len, uint8_t *payload)
{
    int i = 0;
    while (i < len) {printf("%02x ", payload[i]); i++;}
    printf("\r\n");
}
#endif

#define ENET_SEND_FST_LEN_MAX (sizeof(AT_CMD_ENET_SEND)+16)
static err_t at_output(struct netif *netif, struct pbuf *p)
{
    char fst[ENET_SEND_FST_LEN_MAX], out[256];

    LOGD(TAG, "in %s", __func__);

    if (!netif || !p) return ERR_ARG;

    // Only expect PBUF_RAM type for outgoing packet
    if (p->type != PBUF_RAM && p->type != PBUF_POOL) {
        LOGW(TAG, "Not expected outgoing pbuf type: %d, ignore it", p->type);
        return ERR_VAL;
    }

    // <TODO>
    if (p->next) LOGW(TAG, "Attention: payload has next!!!");

    snprintf(fst, sizeof(fst) - 1, "%s=%d", AT_CMD_ENET_SEND, p->len);

    LOGD(TAG, "Payload to send:");
#ifdef DEBUG
    print_payload(p->len, p->payload);
#endif

    // send payload directly
    at.send_data_2stage(fst, p->payload, p->len, out, sizeof(out));

    if (strstr(out, "OK") == NULL) {
        LOGE(TAG, "AT send failed");
        return ERR_IF;
    }

    return ERR_OK;
}

static err_t if_init(struct netif *netif)
{
    LOGD(TAG, "in %s", __func__);

    netif->name[0] = if_name[0];
    netif->name[1] = if_name[1];
    netif->num = 0; // <TODO>
    hal_wifi_get_mac_addr(NULL, netif->hwaddr);
    netif->hwaddr_len = ETHARP_HWADDR_LEN;
#if LWIP_IPV4
    netif->output = etharp_output;
    netif->linkoutput = at_output;
#endif
    netif->mtu = 1500;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | 
      NETIF_FLAG_UP | NETIF_FLAG_LINK_UP | NETIF_FLAG_IGMP;

    return ERR_OK;
}

static void enter_enet_raw_mode()
{
    char out[256];
    at.set_timeout(5000);
    at.send_raw(AT_CMD_ENTER_ENET_MODE, out, sizeof(out));
    if (strstr(out, "ERROR") != NULL)
        LOGE(TAG, "AT enter enet raw mode failed.");
}

static void enet_raw_data_handler();

#define MIN(a, b) ((a) > (b) ? (b) : (a))
static void wifi_event_handler(input_event_t *event, void *priv_data)
{
#ifdef LWIP_IPV4
    hal_wifi_ip_stat_t ip_stat;
    ip4_addr_t ip, mask, gw;
    static uint8_t netif_inited = 0;

    if (netif_inited) {
        LOGI(TAG, "netif already started.");
        return;
    }

    if (event->type != EV_WIFI) return;

    if (event->code == CODE_WIFI_ON_GOT_IP) {
        LOGD(TAG, "AT adapter interface is going to be up.");

        at.oob(AT_EVENT_ENET_DATA, NULL, 0, enet_raw_data_handler, NULL);
        enter_enet_raw_mode();

        if (hal_wifi_get_ip_stat(NULL, &ip_stat, STATION) != 0) {
            LOGE(TAG, "Get ip stat failed.");
            return;
        }

        ip4addr_aton(ip_stat.ip, &ip);
        ip4addr_aton(ip_stat.gate, &gw);
        ip4addr_aton(ip_stat.mask, &mask);
        netif_add(&at_netif, &ip, &mask, &gw, NULL, if_init, tcpip_input);
        netif_set_default(&at_netif);
        netif_set_up(&at_netif);
        netif_inited = 1;
        LOGD(TAG, "netif added.");
        aos_post_event(EV_AT, CODE_AT_IF_READY, 0);
    }
#endif
}

/**
 * +ENETEVENT:data_len,data\r\n
 * data_len is ascii in decimal, e.g. "100"
 * data is raw payload.
 */
static void enet_raw_data_handler_helper(void *arg)
{
    err_t err;
    struct pbuf *pbuf = (struct pbuf *)arg;

    LOGD(TAG, "in %s", __func__);

    LOGD(TAG, "payload received:");
#ifdef DEBUG
    print_payload(pbuf->len, pbuf->payload);
#endif

    if (at_netif.input) {
        err = at_netif.input(pbuf, &at_netif);

        if (err == ERR_OK) {
            LOGD(TAG, "at_netif input finished.");
        } else {
            pbuf_free(pbuf);
            LOGE(TAG, "at_netif input failed, err: %d", err);
        }
    }
}

static void enet_raw_data_handler()
{
    long len = 0, i = 0;
    char buf[16] = {0};
    char c;
    int  ret, tot_read, pbuf_read, to_read;
    struct pbuf *pbuf, *q;
    void *tsk;
    char *p_data;
    
    while (i+1 <= sizeof(buf)) {
        ret = at.getch(&c);
        if (ret != 0) return;
        if (c == ',') break;
        buf[i++] = c;
    }
    buf[i] = '\0';

    len = strtol(buf, NULL, 10);
    LOGD(TAG, "enet data len: %s -> %d", buf, len);

    pbuf = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    LOGD(TAG, "alloc: pbuf - 0x%08x, payload: 0x%08x, ref: %d,"
      " next: 0x%08x, tot_len: %d, len: %d",
      (uint32_t)pbuf, (uint32_t)pbuf->payload, pbuf->ref,
      (uint32_t)pbuf->next, pbuf->tot_len, pbuf->len);

    tot_read = 0;
    q = pbuf;
    while (tot_read < len) {
        // check available pbuf space
        if (!q) {
            LOGE(TAG, "No enough pbuf space to store AT read.");
            pbuf_free(pbuf);
            return;
        }

        // read to a pbuf
        to_read = MIN(len - tot_read, q->len);
        pbuf_read = 0;
        p_data = q->payload;
        
        while (pbuf_read < to_read) {
            ret = at.read(p_data + pbuf_read, to_read - pbuf_read);
            if (ret == -1) {
               LOGE(TAG, "at.read failed, will stop handling.");
               pbuf_free(pbuf);
               return;
            }
            pbuf_read += ret;
        }

        // update val
        tot_read += pbuf_read;
        q = q->next;
    }

    tsk = aos_loop_schedule_work(0, enet_raw_data_handler_helper, pbuf, NULL, NULL);
    if (!tsk) {
        LOGE(TAG, "Failed to created task in %s", __func__);
        pbuf_free(pbuf);
    }
}

int at_adapter_init()
{
    aos_register_event_filter(EV_WIFI, wifi_event_handler, NULL);
    return 0;
}
