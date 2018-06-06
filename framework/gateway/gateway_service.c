/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <aos/aos.h>
#include <aos/network.h>
#include "umesh.h"
#include "cJSON.h"
#include "devmgr.h"

#include "mqtt_sn.h"
#include "gateway_service.h"

#include "enrollee.h"

#define MODULE_NAME "gateway"
#define ADV_INTERVAL (5 * 1000)
#define MAX_GATEWAY_RECONNECT_TIMEOUT 48
#define GATEWAY_WORKER_THREAD

typedef struct reg_info_s {
    char model_id[sizeof(uint32_t) + 1];
    unsigned char ieee_addr[IEEE_ADDR_BYTES + 1];
    char rand[SUBDEV_RAND_BYTES + 1];
    char sign[STR_SIGN_LEN + 1];
    char model[OS_PRODUCT_MODEL_LEN];
    char secret[OS_PRODUCT_SECRET_LEN];
    bool login;
} reg_info_t;

typedef struct {
    dlist_t next;
    dev_info_t *devinfo;
#if LWIP_IPV6
    struct sockaddr_in6 addr;
#else
    struct sockaddr_in addr;
#endif
    uint8_t timeout;
} client_t;

typedef struct {
#if LWIP_IPV6
    struct sockaddr_in6 gw_addr;
    struct sockaddr_in6 src_addr;
#else
    struct sockaddr_in gw_addr;
    struct sockaddr_in src_addr;
#endif
    int sockfd;
    dlist_t clients;
#ifdef GATEWAY_WORKER_THREAD
    aos_mutex_t *mutex;
#endif
    char uuid[STR_UUID_LEN + 1];
    bool gateway_mode;
    bool yunio_connected;
    bool mesh_connected;
    bool mqtt_connected;
    bool mqtt_reconnect;
    bool login;
} gateway_state_t;

static gateway_state_t gateway_state;
static bool gateway_service_started = false;

char *config_get_main_uuid(void);
static void gateway_service_event(input_event_t *eventinfo, void *priv_data);

#include <ali_crypto.h>

#define KEY_SIZE 16
static uint8_t g_iv[KEY_SIZE];
static uint8_t g_key[KEY_SIZE] = {
    0x11, 0x11, 0x11, 0x11,
    0x22, 0x22, 0x22, 0x22,
    0x13, 0x13, 0x14, 0x14,
    0xf1, 0xf1, 0xe1, 0xe1
};
static uint32_t aes_ctx_size;


#if LWIP_IPV6
static int send_sock(int fd, void *buf, int len, struct sockaddr_in6 *paddr)
#else
static int send_sock(int fd, void *buf, int len, struct sockaddr_in *paddr)
#endif
{
    ali_crypto_result result;
    void *aes_ctx;
    int ret = -1;
    size_t dlen = len;

    aes_ctx = aos_malloc(aes_ctx_size);
    if (aes_ctx == NULL) {
        return -1;
    }

    result = ali_aes_init(AES_CTR, true, g_key, NULL, KEY_SIZE, g_iv, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        goto out;
    }

    result = ali_aes_finish(buf, len, buf, &dlen, SYM_NOPAD, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        goto out;
    }

    ret = sendto(fd, buf, len, MSG_DONTWAIT, (struct sockaddr *)paddr, sizeof(*paddr));

out:
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE(MODULE_NAME, "error encrypting %d", result);
    }

    aos_free(aes_ctx);

    return ret;
}

#if LWIP_IPV6
static int recv_sock(int fd, void *buf, int len, struct sockaddr_in6 *paddr)
#else
static int recv_sock(int fd, void *buf, int len, struct sockaddr_in *paddr)
#endif
{
    size_t slen = sizeof(*paddr);

    len = recvfrom(fd, buf, len, MSG_DONTWAIT, (struct sockaddr *)paddr, &slen);
    if (len <= 0) {
        return len;
    }

    ali_crypto_result result;
    void *aes_ctx;
    int ret;
    size_t dlen = len;

    aes_ctx = aos_malloc(aes_ctx_size);
    if (aes_ctx == NULL) {
        return -1;
    }

    result = ali_aes_init(AES_CTR, false, g_key, NULL, KEY_SIZE, g_iv, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        goto out;
    }

    result = ali_aes_finish(buf, len, buf, &dlen, SYM_NOPAD, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        goto out;
    }

out:
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE(MODULE_NAME, "error encrypting %d", result);
    }
    aos_free(aes_ctx);
    return result != ALI_CRYPTO_SUCCESS ? -1 : len;
}

static char *get_uuid_by_ipaddr(struct sockaddr *addr)
{
    client_t *client = NULL;
    dlist_for_each_entry(&gateway_state.clients, client, client_t, next) {
        if (memcmp(addr, &client->addr, sizeof(client->addr)) == 0) {
            return client->devinfo->dev_base.uuid;
        }
    }
    return NULL;
}

#if LWIP_IPV6
static struct sockaddr_in6 *get_ipaddr_by_uuid(const char *uuid)
#else
static struct sockaddr_in *get_ipaddr_by_uuid(const char *uuid)
#endif
{
    client_t *client = NULL;
    dlist_for_each_entry(&gateway_state.clients, client, client_t, next) {
        if (memcmp(uuid, client->devinfo->dev_base.uuid, STR_UUID_LEN) == 0) {
            return &client->addr;
        }
    }
    return NULL;
}

static int aos_cloud_get_attr(const char *uuid, const char *attr_name)
{
#if LWIP_IPV6
    struct sockaddr_in6 *paddr = NULL;
    paddr = get_ipaddr_by_uuid(uuid);
#else
    struct sockaddr_in *paddr = NULL;
    paddr = get_ipaddr_by_uuid(uuid);
#endif
    if (paddr == NULL) {
        return -1;
    }

    void *buf;
    int len;
    pub_body_t *pub_body = msn_alloc(PUBLISH, 4 + strlen(attr_name) + 1, &buf, &len);
    memcpy(pub_body->payload, "get", 4);
    memcpy(pub_body->payload + 4, attr_name, strlen(attr_name) + 1);

    send_sock(gateway_state.sockfd, buf, len, paddr);

    aos_free(buf);
    return 0;
}

static int aos_cloud_set_attr(const char *uuid, const char *attr_name, const char *attr_value)
{
#if LWIP_IPV6
    struct sockaddr_in6 *paddr = NULL;
    paddr = get_ipaddr_by_uuid(uuid);
#else
    struct sockaddr_in *paddr = NULL;
    paddr = get_ipaddr_by_uuid(uuid);
#endif
    if (paddr == NULL) {
        return -1;
    }

    void *buf;
    int len;
    pub_body_t *pub_body = msn_alloc(PUBLISH, 4 + strlen(attr_name) + 1 + strlen(attr_value) + 1, &buf, &len);
    memcpy(pub_body->payload, "set", 4);
    memcpy(pub_body->payload + 4, attr_name, strlen(attr_name) + 1);
    memcpy(pub_body->payload + 4 + strlen(attr_name) + 1, attr_value, strlen(attr_value) + 1);

    send_sock(gateway_state.sockfd, buf, len, paddr);

    aos_free(buf);
    return 0;
}

static void clear_connected_flag(void *arg)
{
    gateway_state.mqtt_connected = false;
}

static void set_reconnect_flag(void *arg)
{
    gateway_state.mqtt_reconnect = true;
}

#if LWIP_IPV6
static void connect_to_gateway(gateway_state_t *pstate, struct sockaddr_in6 *paddr)
#else
static void connect_to_gateway(gateway_state_t *pstate, struct sockaddr_in *paddr)
#endif
{
    void *buf;
    int len;
    reg_info_t *reginfo;
    const mac_address_t *mac_addr;
    uint32_t model_id = 0x0050099a;

    if (gateway_state.mqtt_connected == true) {
        LOGD(MODULE_NAME, "reconnect to gateway");
    } else {
        LOGD(MODULE_NAME, "connect to new gateway");
    }
    reginfo = (reg_info_t *)msn_alloc(CONNECT, sizeof(reg_info_t), &buf, &len);
    memset(reginfo, 0, sizeof(reg_info_t));
    memcpy(reginfo->model_id, &model_id, sizeof(model_id));
    mac_addr = umesh_get_mac_address(MEDIA_TYPE_DFL);
    memcpy(reginfo->ieee_addr, mac_addr->addr, IEEE_ADDR_BYTES);
    os_product_get_model(reginfo->model);
    os_product_get_secret(reginfo->secret);
    memcpy(reginfo->rand, "randrandrandrand", sizeof(reginfo->rand));
    devmgr_get_device_signature(model_id, reginfo->rand, reginfo->sign, sizeof(reginfo->sign));
    reginfo->login = pstate->login;

    send_sock(pstate->sockfd, buf, len, paddr);

    aos_free(buf);
}

static void handle_adv(gateway_state_t *pstate, void *pmsg, int len)
{
    if (pstate->gateway_mode) {
        return;
    }

    LOGD(MODULE_NAME, "handle_adv");
    adv_body_t *adv_msg = pmsg;
#if LWIP_IPV6
    struct sockaddr_in6 *gw_addr = &pstate->gw_addr;

    if (pstate->mqtt_connected &&
        memcmp(&gw_addr->sin6_addr, adv_msg->payload, sizeof(gw_addr->sin6_addr)) == 0 &&
        pstate->mqtt_reconnect == false) {
        aos_cancel_delayed_action(-1, clear_connected_flag, &gateway_state);
        aos_post_delayed_action(5 * ADV_INTERVAL, clear_connected_flag, &gateway_state);
        return;
    }

    if (memcmp(&gw_addr->sin6_addr, adv_msg->payload, sizeof(gw_addr->sin6_addr)) != 0) {
        pstate->mqtt_connected = false;
    }

    aos_cancel_delayed_action(-1, clear_connected_flag, &gateway_state);
    memcpy(&gw_addr->sin6_addr, adv_msg->payload, sizeof(gw_addr->sin6_addr));
    gw_addr->sin6_family = AF_INET6;
    gw_addr->sin6_port = htons(MQTT_SN_PORT);
#else
    struct sockaddr_in *gw_addr = &pstate->gw_addr;

    if (pstate->mqtt_connected &&
        memcmp(&gw_addr->sin_addr, adv_msg->payload, sizeof(gw_addr->sin_addr)) == 0 &&
        pstate->mqtt_reconnect == false) {
        aos_cancel_delayed_action(-1, clear_connected_flag, &gateway_state);
        aos_post_delayed_action(5 * ADV_INTERVAL, clear_connected_flag, &gateway_state);
        return;
    }

    if (memcmp(&gw_addr->sin_addr, adv_msg->payload, sizeof(gw_addr->sin_addr)) != 0) {
        pstate->mqtt_connected = false;
    }

    aos_cancel_delayed_action(-1, clear_connected_flag, &gateway_state);
    memcpy(&gw_addr->sin_addr, adv_msg->payload, sizeof(gw_addr->sin_addr));
    gw_addr->sin_family = AF_INET;
    gw_addr->sin_port = htons(MQTT_SN_PORT);
#endif
    connect_to_gateway(pstate, &pstate->gw_addr);
}


static void handle_publish(gateway_state_t *pstate, void *pmsg, int len)
{
    pub_body_t *pub_msg = pmsg;
    const char *op_code = (const char *)(pub_msg + 1);
    const char *attr_name = op_code + 4;
    const char *attr_value = attr_name + strlen(attr_name) + 1;
    if (strcmp(op_code, "rpt") == 0) {
        if (!pstate->gateway_mode) {
            LOGE(MODULE_NAME, "error: cloud report data received at non-gateway node");
            return;
        }

        const char *uuid = get_uuid_by_ipaddr((struct sockaddr *)&pstate->src_addr);
        if (uuid == NULL) {
            LOGE(MODULE_NAME, "error: cloud report data from unconnected node");
            return;
        }

#define POST_ATTR_VALUE_STRING_FMT      "{\"uuid\":\"%s\",%s"
        char *buf;
        int sz = sizeof(POST_ATTR_VALUE_STRING_FMT) + strlen(uuid) + strlen(attr_value) + 16;

        buf = aos_malloc(sz);
        /* tricky : attr_value = {"key":"value"} -> {"uuid":"xx", "key":"value"} */
        snprintf(buf, sz, POST_ATTR_VALUE_STRING_FMT, uuid, attr_value + 1);
        alink_report_async("postDeviceData", buf, NULL, NULL);
        aos_free(buf);
        return;
    }

    if (strcmp(op_code, "get") == 0) {
        LOGD(MODULE_NAME, "recv %s - %s", op_code, attr_name);
        aos_cloud_trigger(GET_DEVICE_STATUS, attr_name);
    } else if (strcmp(op_code, "set") == 0) {
        LOGD(MODULE_NAME, "recv %s - %s - %s", op_code, attr_name, attr_value);
        aos_cloud_trigger(SET_DEVICE_STATUS, attr_name);
    } else {
        const char *payload = (const char *)(pub_msg + 1);
        LOGD(MODULE_NAME, "recv unkown %s", payload);
    }
}

/* Example: 0x12345678 -> "78056034012" */
static void mac_to_devid(unsigned char *mac, uint8_t *devid)
{
    int i;
    char c;

    if (!mac || !devid) {
        LOGE(MODULE_NAME, "%s error", __func__);
        return;
    }

    for (i = 0; i < 6; i++) {
        c = mac[i] >> 4;
        if (c >= 0 && c <= 9) {
            *devid++ = c + '0';
        } else if (c >= 0xa && c <= 0xf) {
            *devid++ = c + 'A' - 0xa;
        }
        c = mac[i] & 0xf;
        if (c >= 0 && c <= 9) {
            *devid++ = c + '0';
        } else if (c >= 0xa && c <= 0xf) {
            *devid++ = c + 'A' - 0xa;
        }
        *devid++ = '0';
    }
    *--devid = '\0'; // Eat the last '0'
}

static int add_mesh_enrollee(reg_info_t *reginfo)
{
#ifdef CONFIG_YWSS
    struct enrollee_info *mesh_enrollee;
    uint8_t sign[ENROLLEE_SIGN_SIZE];
    uint32_t rand;
    char devid[MAX_DEVID_LEN], *model, *secret;

    if (!reginfo) {
        LOGE(MODULE_NAME, "Failed to add mesh enrollee - reginfo is NULL");
        return -1;
    }

    mesh_enrollee = (struct enrollee_info *)os_malloc(sizeof(struct enrollee_info));
    OS_CHECK_MALLOC(mesh_enrollee);

    memset(mesh_enrollee, 0, sizeof(struct enrollee_info));
    mesh_enrollee->dev_type_ver = DEVICE_TYPE_VERSION;

    mac_to_devid(reginfo->ieee_addr, devid);
    LOGD(MODULE_NAME, "mac(%02x%02x%02x%02x%02x%02x) -> devid (%s)",
         reginfo->ieee_addr[0], reginfo->ieee_addr[1], reginfo->ieee_addr[2],
         reginfo->ieee_addr[3], reginfo->ieee_addr[4], reginfo->ieee_addr[5], devid);
    mesh_enrollee->devid_len = strlen(devid);
    memcpy(mesh_enrollee->devid, devid, mesh_enrollee->devid_len);

    mesh_enrollee->frame_type = ENROLLEE_FRAME_TYPE;

    model = reginfo->model;
    mesh_enrollee->model_len = strlen(model);
    memcpy(mesh_enrollee->model, model, mesh_enrollee->model_len);

    rand = get_random_digital();
    memcpy(mesh_enrollee->random, (uint8_t *)&rand, sizeof(uint32_t));
    LOGD(MODULE_NAME, "rand %d, devid: %s, model: %s", rand,
         mesh_enrollee->devid, mesh_enrollee->model);

    secret = reginfo->secret;

    awss_calc_sign(rand, devid, model, secret, sign);

    memcpy(mesh_enrollee->sign, sign, ENROLLEE_SIGN_SIZE);

    mesh_enrollee->rssi = 0;

    enrollee_put(mesh_enrollee);

    os_free(mesh_enrollee);
#endif
    return 0;
}

static client_t *new_client(gateway_state_t *pstate, reg_info_t *reginfo)
{
    client_t *client = NULL;
    dlist_for_each_entry(&pstate->clients, client, client_t, next) {
        if (client->devinfo == NULL) {
            continue;
        }
        if (memcmp(reginfo->ieee_addr, client->devinfo->dev_base.u.ieee_addr, IEEE_ADDR_BYTES) == 0) {
            LOGD(MODULE_NAME, "existing client %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
                 (uint8_t)client->devinfo->dev_base.u.ieee_addr[0],
                 (uint8_t)client->devinfo->dev_base.u.ieee_addr[1],
                 (uint8_t)client->devinfo->dev_base.u.ieee_addr[2],
                 (uint8_t)client->devinfo->dev_base.u.ieee_addr[3],
                 (uint8_t)client->devinfo->dev_base.u.ieee_addr[4],
                 (uint8_t)client->devinfo->dev_base.u.ieee_addr[5],
                 (uint8_t)client->devinfo->dev_base.u.ieee_addr[6],
                 (uint8_t)client->devinfo->dev_base.u.ieee_addr[7]);
            goto add_enrollee;
        }
    }

    client = aos_malloc(sizeof(client_t));
    PTR_RETURN(client, NULL, "alloc memory failed");
    bzero(client, sizeof(*client));
    uint32_t model_id;
    memcpy(&model_id, reginfo->model_id, sizeof(model_id));
    int ret = devmgr_join_zigbee_device(reginfo->ieee_addr, model_id, reginfo->rand, reginfo->sign);
    if (ret ==  SERVICE_RESULT_ERR) {
        LOGD(MODULE_NAME, "register device:%s to alink server failed", reginfo->ieee_addr);
        aos_free(client);
        return NULL;
    }
    client->devinfo = devmgr_get_devinfo_by_ieeeaddr(reginfo->ieee_addr);
    if (client->devinfo == NULL) {
        LOGD(MODULE_NAME, "register device:%s to alink server failed", reginfo->ieee_addr);
        aos_free(client);
        return NULL;
    }
    devmgr_put_devinfo_ref(client->devinfo);
    dlist_add_tail(&client->next, &pstate->clients);

    LOG("GATEWAY: new client %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x connected",
        (uint8_t)client->devinfo->dev_base.u.ieee_addr[0],
        (uint8_t)client->devinfo->dev_base.u.ieee_addr[1],
        (uint8_t)client->devinfo->dev_base.u.ieee_addr[2],
        (uint8_t)client->devinfo->dev_base.u.ieee_addr[3],
        (uint8_t)client->devinfo->dev_base.u.ieee_addr[4],
        (uint8_t)client->devinfo->dev_base.u.ieee_addr[5],
        (uint8_t)client->devinfo->dev_base.u.ieee_addr[6],
        (uint8_t)client->devinfo->dev_base.u.ieee_addr[7]);

add_enrollee:
    client->timeout = 0;
    if (reginfo->login == true) {
        devmgr_login_device(client->devinfo);
    } else {
        devmgr_logout_device(client->devinfo);
    }
    add_mesh_enrollee(reginfo);

    return client;
}

static void handle_connect(gateway_state_t *pstate, void *pmsg, int len)
{
    client_t *client = NULL;

    if (!pstate->gateway_mode) {
        LOGE(MODULE_NAME, "error recv CONNECT cmd!");
        return;
    }

    if (pstate->yunio_connected) {
        client = new_client(pstate, (reg_info_t *)pmsg);
        if (client != NULL) {
            memcpy(&client->addr, &pstate->src_addr, sizeof(client->addr));
        }
    }

    void *buf;
    conn_ack_t *conn_ack = msn_alloc(CONNACK, STR_UUID_LEN + 1, &buf, &len);
    if (client == NULL) {
        conn_ack->ReturnCode = -1;
        memset(conn_ack->payload, 0x00, STR_UUID_LEN + 1);
    } else {
        conn_ack->ReturnCode = 0;
        memcpy(conn_ack->payload, client->devinfo->dev_base.uuid, STR_UUID_LEN + 1);
    }
    send_sock(pstate->sockfd, buf, len, &pstate->src_addr);
    aos_free(buf);
}

static int gateway_cloud_report(const char *method, const char *json_buffer)
{
    gateway_state_t *pstate = &gateway_state;

    if (pstate->yunio_connected) {
        LOGE(MODULE_NAME, "strange, yunio is ready!");
        return 0;
    }

    if (!pstate->mqtt_connected) {
        LOGE(MODULE_NAME, "mqtt not ready!");
        return -1;
    }

    void *buf;
    int len;
    int sz = 4 + strlen(method) + 1 + strlen(json_buffer) + 1;
    pub_body_t *pub_body = msn_alloc(PUBLISH, sz, &buf, &len);

    memcpy(pub_body->payload, "rpt", 4);
    memcpy(pub_body->payload + 4, method, strlen(method) + 1);
    memcpy(pub_body->payload + 4 + strlen(method) + 1, json_buffer, strlen(json_buffer) + 1);

    send_sock(pstate->sockfd, buf, len, &pstate->gw_addr);

    aos_free(buf);
    return 0;
}

static void handle_connack(gateway_state_t *pstate, void *pmsg, int len)
{
    if (pstate->gateway_mode) {
        LOGE(MODULE_NAME, "error recv CONNACK cmd!");
        return;
    }

    conn_ack_t *conn_ack = pmsg;
    if (conn_ack->ReturnCode != 0) {
        LOG("GATEWAY: connect to server fail, ret=%d!", conn_ack->ReturnCode);
        return;
    }

    if (pstate->mesh_connected == false) {
        LOG("GATEWAY: connect to server succeed");
    }

    memcpy(pstate->uuid, conn_ack->payload, sizeof(pstate->uuid));
    pstate->uuid[STR_UUID_LEN] = '\x0';
    pstate->mqtt_connected = true;
    aos_post_delayed_action(5 * ADV_INTERVAL, clear_connected_flag, &gateway_state);

    aos_cloud_register_backend(&gateway_cloud_report);
    if (pstate->mqtt_reconnect == false) {
        aos_cloud_trigger(CLOUD_CONNECTED, NULL);
    }

    pstate->mqtt_reconnect = false;
    aos_cancel_delayed_action(-1, set_reconnect_flag, &gateway_state);
    aos_post_delayed_action((8 + (rand() & 0x7)) * ADV_INTERVAL, set_reconnect_flag, &gateway_state);

}

static void handle_msg(gateway_state_t *pstate, uint8_t *pmsg, int len)
{
    uint8_t msg_type = *pmsg++;
    int ret = -1;

    len --;
#ifdef GATEWAY_WORKER_THREAD
    ret = aos_mutex_lock(gateway_state.mutex, AOS_WAIT_FOREVER);
    if (ret != 0) {
        return;
    }
#endif
    switch (msg_type) {
        case PUBLISH:
            handle_publish(pstate, pmsg, len);
            break;
        case CONNECT:
            handle_connect(pstate, pmsg, len);
            break;
        case CONNACK:
            handle_connack(pstate, pmsg, len);
            break;
        case ADVERTISE:
            handle_adv(pstate, pmsg, len);
            break;
    }
#ifdef GATEWAY_WORKER_THREAD
    aos_mutex_unlock(gateway_state.mutex);
#endif
}

static void gateway_sock_read_cb(int fd, void *priv)
{
    gateway_state_t *pstate = priv;
#define BUF_LEN 256
    uint8_t buf[BUF_LEN], *pmsg;
    int len;
    socklen_t slen = sizeof(pstate->src_addr);

    len = recv_sock(pstate->sockfd, buf, BUF_LEN, &pstate->src_addr);
    if (len <= 2) {
        LOGE(MODULE_NAME, "error read count %d", len);
        return;
    }

    len = msn_parse_header(buf, len, &pmsg);
    if (len < 0) {
        LOGE(MODULE_NAME, "error parsing header %x", buf[0]);
        return;
    }

    handle_msg(pstate, pmsg, len);
}

static void gateway_advertise(void *arg)
{
    gateway_state_t *pstate = arg;
    void *buf;
    int len;
    adv_body_t *adv;
    client_t *client;

    aos_post_delayed_action(ADV_INTERVAL, gateway_advertise, arg);

#if LWIP_IPV6
    ip6_addr_t *mcast_addr = (ip6_addr_t *)ur_adapter_get_mcast_ipaddr();
    ip6_addr_t *ucast_addr = (ip6_addr_t *)ur_adapter_get_default_ipaddr();
    if (!mcast_addr || !ucast_addr) {
        return;
    }

    adv = msn_alloc(ADVERTISE, sizeof(*ucast_addr), &buf, &len);
    bzero(adv, sizeof(*adv));
    memcpy(adv->payload, ucast_addr, UR_IP6_ADDR_SIZE);

    struct sockaddr_in6 addr;
    bzero(&addr, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(MQTT_SN_PORT);
    memcpy(&addr.sin6_addr, mcast_addr, sizeof(addr.sin6_addr));
#else
    ip4_addr_t *ip4_addr = (ip4_addr_t *)ur_adapter_get_default_ipaddr();
    if (!ip4_addr) {
        return;
    }

    adv = msn_alloc(ADVERTISE, sizeof(ip4_addr_t), &buf, &len);
    bzero(adv, sizeof(*adv));
    memcpy(adv->payload, (uint8_t *)&ip4_addr->addr, sizeof(ip4_addr_t));

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(MQTT_SN_PORT);
    ip4_addr = (ip4_addr_t *)ur_adapter_get_mcast_ipaddr();
    memcpy(&addr.sin_addr, (uint8_t *)&ip4_addr->addr, sizeof(addr.sin_addr));
#endif

    send_sock(pstate->sockfd, buf, len, &addr);
    LOGD(MODULE_NAME, "gateway_advertise");

    aos_free(buf);

    dlist_for_each_entry(&pstate->clients, client, client_t, next) {
        if (client->timeout < MAX_GATEWAY_RECONNECT_TIMEOUT) {
            client->timeout ++;
            if (client->timeout >= MAX_GATEWAY_RECONNECT_TIMEOUT) {
                devmgr_logout_device(client->devinfo);
                LOG("GATEWAY: client %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x went offline",
                    (uint8_t)client->devinfo->dev_base.u.ieee_addr[0],
                    (uint8_t)client->devinfo->dev_base.u.ieee_addr[1],
                    (uint8_t)client->devinfo->dev_base.u.ieee_addr[2],
                    (uint8_t)client->devinfo->dev_base.u.ieee_addr[3],
                    (uint8_t)client->devinfo->dev_base.u.ieee_addr[4],
                    (uint8_t)client->devinfo->dev_base.u.ieee_addr[5],
                    (uint8_t)client->devinfo->dev_base.u.ieee_addr[6],
                    (uint8_t)client->devinfo->dev_base.u.ieee_addr[7]);
            }
        }
    }
}

#ifdef GATEWAY_WORKER_THREAD
static void gateway_worker(void *arg)
{
    int sockfd;
    fd_set rfds;
    struct timeval timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    while (1) {
        sockfd = gateway_state.sockfd;

        if (sockfd < 0) {
            aos_msleep(100);
            continue;
        }

        FD_ZERO(&rfds);
        FD_SET(gateway_state.sockfd, &rfds);

        int ret = select(sockfd + 1, &rfds, NULL, NULL, &timeout);
        if (ret < 0) {
            if (errno != EINTR) {
                LOGD(MODULE_NAME, "select error %d", errno);
                continue;
            }
        }

        if (sockfd != gateway_state.sockfd) {
            gateway_state.mqtt_connected = false;
            close(sockfd);
            continue;
        }

        if (FD_ISSET(sockfd, &rfds)) {
            gateway_sock_read_cb(sockfd, &gateway_state);
        }
    }

    LOGE(MODULE_NAME, "return");
}
#endif

static void handle_gateway_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if (argc == 1) {
        aos_cli_printf("Usage: gateway login/logout. Currently %s\r\n",
                       gateway_state.login ? "login" : "logout");
        return;
    }

    if (strcmp(argv[1], "login") == 0) {
        gateway_state.login = true;
    } else if (strcmp(argv[1], "logout") == 0) {
        gateway_state.login = false;
    }

    if (gateway_state.mqtt_connected) {
        connect_to_gateway(&gateway_state, &gateway_state.gw_addr);
    }
}

static struct cli_command gatewaycmd = {
    .name = "gateway",
    .help = "gateway login/logout",
    .function = handle_gateway_cmd
};

int gateway_service_init(void)
{
    ali_crypto_result result;
    gateway_state_t *pstate = &gateway_state;

    result = ali_aes_get_ctx_size(AES_CTR, &aes_ctx_size);
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE(MODULE_NAME, "ali aes get ctx size fail(%08x)", result);
        return 1;
    }

    pstate->gateway_mode = false;
    pstate->yunio_connected = false;
    pstate->mesh_connected = false;
    pstate->mqtt_connected = false;
    pstate->mqtt_reconnect = false;
    pstate->sockfd = -1;
    pstate->login = false;
    dlist_init(&gateway_state.clients);
    aos_cli_register_command(&gatewaycmd);
    aos_register_event_filter(EV_YUNIO, gateway_service_event, NULL);
    aos_register_event_filter(EV_MESH, gateway_service_event, NULL);
#ifdef GATEWAY_WORKER_THREAD
    pstate->mutex = (aos_mutex_t *)aos_malloc(sizeof(aos_mutex_t));
    if (pstate->mutex == NULL) {
        LOGE(MODULE_NAME, "error: allocate memory for mutex failed");
        return 1;
    }
    if (aos_mutex_new(pstate->mutex) != 0) {
        aos_free(pstate->mutex);
        LOGE(MODULE_NAME, "error: create mutex failed");
        return 1;
    }
    aos_task_new("gatewayworker", gateway_worker, NULL, 4096);
#endif
    return 0;
}

void gateway_service_deinit(void)
{
    aos_unregister_event_filter(EV_YUNIO, gateway_service_event, NULL);
    aos_unregister_event_filter(EV_MESH, gateway_service_event, NULL);
}

bool gateway_is_connected(void)
{
    return gateway_state.mqtt_connected;
}

const char *gateway_get_uuid(void)
{
    return gateway_state.uuid;
}

static int init_socket(void)
{
    gateway_state_t *pstate = &gateway_state;
    int sockfd;
    int ret;

#if LWIP_IPV6
    sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
#else
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#endif
    if (sockfd < 0) {
        LOGE(MODULE_NAME, "error open socket %d", errno);
        return -1;
    }

    if (pstate->sockfd >= 0) {
#ifndef GATEWAY_WORKER_THREAD
        aos_cancel_poll_read_fd(pstate->sockfd, gateway_sock_read_cb, pstate);
        close(pstate->sockfd);

#endif
        pstate->sockfd = -1;
    }

    int val = 1;
#if LWIP_IPV6
    setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &val, sizeof(val));
#endif
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int) {
        1
    }, sizeof(int));

#if LWIP_IPV6
    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(MQTT_SN_PORT);
#else
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(MQTT_SN_PORT);
#endif
    ret = bind(sockfd, (const struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        LOGE(MODULE_NAME, "error bind socket %d", errno);
        close(sockfd);
        return -1;
    }

    pstate->sockfd = sockfd;
#ifndef GATEWAY_WORKER_THREAD
    aos_poll_read_fd(sockfd, gateway_sock_read_cb, pstate);
#endif

    return 0;
}

#include "json_parser.h"
#define MAX_UUID_LEN        33
#define JSON_KEY_UUID       "uuid"
static void gateway_handle_sub_status(int event, const char *json_buffer)
{
    char *str_pos = NULL;
    int str_len = 0;
    char uuid[MAX_UUID_LEN] = {0};

    str_pos = json_get_value_by_name((char *)json_buffer, strlen(json_buffer), JSON_KEY_UUID, &str_len, NULL);
    strncpy(uuid, str_pos, str_len);

    if (event == GET_SUB_DEVICE_STATUS) {
        aos_cloud_get_attr(uuid, json_buffer);
    } else if (event == SET_SUB_DEVICE_STATUS) {
        aos_cloud_set_attr(uuid, json_buffer, "");
    }
}

int gateway_service_start(void)
{
    if (gateway_state.gateway_mode) {
        aos_cloud_register_callback(GET_SUB_DEVICE_STATUS, gateway_handle_sub_status);
        aos_cloud_register_callback(SET_SUB_DEVICE_STATUS, gateway_handle_sub_status);

        aos_cancel_delayed_action(-1, gateway_advertise, &gateway_state);
        aos_post_delayed_action(ADV_INTERVAL, gateway_advertise, &gateway_state);
        LOG("GATEWAY: start service as a server");
    } else {
        aos_cancel_delayed_action(-1, gateway_advertise, &gateway_state);
        LOG("GATEWAY: start service as a client");
    }

    if (gateway_service_started == true) {
        return 0;
    }
    gateway_service_started = true;

    init_socket();

    return 0;
}

void gateway_service_stop(void)
{
    client_t *client;
    int ret = -1;
    gateway_service_started = false;

    aos_cancel_delayed_action(-1, clear_connected_flag, &gateway_state);
    aos_cancel_delayed_action(-1, set_reconnect_flag, &gateway_state);
    aos_cancel_delayed_action(-1, gateway_advertise, &gateway_state);
#ifndef GATEWAY_WORKER_THREAD
    close(gateway_state.sockfd);
#endif
    gateway_state.sockfd = -1;
    gateway_state.mqtt_connected = false;
#ifdef GATEWAY_WORKER_THREAD
    ret = aos_mutex_lock(gateway_state.mutex, AOS_WAIT_FOREVER);
    if (ret != 0) {
        LOG("GATEWAY: stop service failed, can not lock muted");
        return;
    }
#endif
    while (!dlist_empty(&gateway_state.clients)) {
        client = dlist_first_entry(&gateway_state.clients, client_t, next);
        dlist_del(&client->next);
        devmgr_leave_zigbee_device(client->devinfo->dev_base.u.ieee_addr);
        aos_free(client);
    }
#ifdef GATEWAY_WORKER_THREAD
    aos_mutex_unlock(gateway_state.mutex);
#endif
    LOG("GATEWAY: stop service");
}

bool gateway_service_get_mesh_mqtt_state()
{
    return gateway_state.mqtt_connected;
}

static void gateway_service_event(input_event_t *eventinfo, void *priv_data)
{
    if (eventinfo->type == EV_YUNIO) {
        if (eventinfo->code == CODE_YUNIO_ON_CONNECTED) {
            gateway_state.yunio_connected = true;
        } else if (eventinfo->code == CODE_YUNIO_ON_DISCONNECTED) {
            gateway_state.yunio_connected = false;
        } else {
            return;
        }
    }

    if (eventinfo->type == EV_MESH) {
        if (eventinfo->code == CODE_MESH_CONNECTED) {
            gateway_state.mesh_connected = true;
        } else if (eventinfo->code == CODE_MESH_DISCONNECTED) {
            gateway_state.mesh_connected = false;
        } else {
            return;
        }
    }

    if (umesh_get_device_state() == DEVICE_STATE_LEADER && gateway_state.yunio_connected == true) {
        gateway_state.gateway_mode = true;
    } else {
        gateway_state.gateway_mode = false;
    }

    if (gateway_state.mesh_connected == true) {
        gateway_service_start();
    } else {
        gateway_service_stop();
    }
}

