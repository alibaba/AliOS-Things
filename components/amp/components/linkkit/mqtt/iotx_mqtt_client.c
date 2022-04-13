/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "mqtt_internal.h"

#ifdef LOG_REPORT_TO_CLOUD
    #include "iotx_log_report.h"
#endif
static int _in_yield_cb;

#ifndef PLATFORM_HAS_DYNMEM
iotx_mc_client_t g_iotx_mc_client[IOTX_MC_CLIENT_MAX_COUNT] = {0};
#endif

static void iotx_mc_release(iotx_mc_client_t *pclient)
{
#ifdef PLATFORM_HAS_DYNMEM
    mqtt_free(pclient);
#else
    memset(pclient, 0, sizeof(iotx_mc_client_t));
#endif
}

#if !WITH_MQTT_ONLY_QOS0
static void iotx_mc_pub_wait_list_init(iotx_mc_client_t *pClient)
{
#ifdef PLATFORM_HAS_DYNMEM
    INIT_LIST_HEAD(&pClient->list_pub_wait_ack);
#else
    memset(pClient->list_pub_wait_ack, 0, sizeof(iotx_mc_pub_info_t) * IOTX_MC_REPUB_NUM_MAX);
#endif
}

static void iotx_mc_pub_wait_list_deinit(iotx_mc_client_t *pClient)
{
#ifdef PLATFORM_HAS_DYNMEM
    iotx_mc_pub_info_t *node = NULL, *next_node = NULL;
    list_for_each_entry_safe(node, next_node, &pClient->list_pub_wait_ack, linked_list, iotx_mc_pub_info_t) {
        list_del(&node->linked_list);
        mqtt_free(node);
    }
#else
    memset(pClient->list_pub_wait_ack, 0, sizeof(iotx_mc_pub_info_t) * IOTX_MC_REPUB_NUM_MAX);
#endif
}
#endif
/* set MQTT connection parameter */
static int iotx_mc_set_connect_params(iotx_mc_client_t *pClient, MQTTPacket_connectData *pConnectParams)
{
    if (NULL == pClient || NULL == pConnectParams) {
        return STATE_USER_INPUT_INVALID;
    }

    memcpy(pClient->connect_data.struct_id, pConnectParams->struct_id, 4);
    pClient->connect_data.struct_version = pConnectParams->struct_version;
    pClient->connect_data.MQTTVersion = pConnectParams->MQTTVersion;
    pClient->connect_data.clientID = pConnectParams->clientID;
    pClient->connect_data.cleansession = pConnectParams->cleansession;
    pClient->connect_data.willFlag = pConnectParams->willFlag;
    pClient->connect_data.username = pConnectParams->username;
    pClient->connect_data.password = pConnectParams->password;
    memcpy(pClient->connect_data.will.struct_id, pConnectParams->will.struct_id, 4);
    pClient->connect_data.will.struct_version = pConnectParams->will.struct_version;
    pClient->connect_data.will.topicName = pConnectParams->will.topicName;
    pClient->connect_data.will.message = pConnectParams->will.message;
    pClient->connect_data.will.qos = pConnectParams->will.qos;
    pClient->connect_data.will.retained = pConnectParams->will.retained;

    if (pConnectParams->keepAliveInterval < CONFIG_MQTT_KEEPALIVE_INTERVAL_MIN) {
        mqtt_warning("Input heartbeat interval(%d ms) < Allowed minimum(%d ms)",
                     (pConnectParams->keepAliveInterval * 1000),
                     (CONFIG_MQTT_KEEPALIVE_INTERVAL_MIN * 1000)
                    );
        mqtt_warning("Reset heartbeat interval => %d Millisecond",
                     (CONFIG_MQTT_KEEPALIVE_INTERVAL * 1000)
                    );
        pClient->connect_data.keepAliveInterval = CONFIG_MQTT_KEEPALIVE_INTERVAL;
    } else if (pConnectParams->keepAliveInterval > CONFIG_MQTT_KEEPALIVE_INTERVAL_MAX) {
        mqtt_warning("Input heartbeat interval(%d ms) > Allowed maximum(%d ms)",
                     (pConnectParams->keepAliveInterval * 1000),
                     (CONFIG_MQTT_KEEPALIVE_INTERVAL_MAX * 1000)
                    );
        mqtt_warning("Reset heartbeat interval => %d Millisecond",
                     (CONFIG_MQTT_KEEPALIVE_INTERVAL * 1000)
                    );
        pClient->connect_data.keepAliveInterval = CONFIG_MQTT_KEEPALIVE_INTERVAL;
    } else {
        pClient->connect_data.keepAliveInterval = pConnectParams->keepAliveInterval;
    }

    return STATE_SUCCESS;
}

/* set state of MQTT client */
static void iotx_mc_set_client_state(iotx_mc_client_t *pClient, iotx_mc_state_t newState)
{
    HAL_MutexLock(pClient->lock_generic);
    pClient->client_state = newState;
    HAL_MutexUnlock(pClient->lock_generic);
}

static iotx_mc_state_t iotx_mc_get_client_state(iotx_mc_client_t *pClient)
{
    iotx_mc_state_t state;
    HAL_MutexLock(pClient->lock_generic);
    state = pClient->client_state;
    HAL_MutexUnlock(pClient->lock_generic);

    return state;
}

/* Initialize internal MQTT client */
static int iotx_mc_init_internal(iotx_mc_client_t *pClient, iotx_mqtt_param_t *pInitParams)
{
    int rc = -1;
    iotx_mc_state_t mc_state = IOTX_MC_STATE_INVALID;
    MQTTPacket_connectData connectdata = MQTTPacket_connectData_initializer;

    if (pClient == NULL || pInitParams == NULL || pInitParams->write_buf_size == 0 || pInitParams->read_buf_size == 0) {
        return STATE_USER_INPUT_INVALID;
    }

    pClient->lock_generic = HAL_MutexCreate();
    if (!pClient->lock_generic) {
        iotx_state_event(ITE_STATE_SYS_DEPEND, STATE_SYS_DEPEND_MUTEX_CREATE, "lock_generic create fail");
        return STATE_SYS_DEPEND_MUTEX_CREATE;
    }

    pClient->lock_list_pub = HAL_MutexCreate();
    if (!pClient->lock_list_pub) {
        iotx_state_event(ITE_STATE_SYS_DEPEND, STATE_SYS_DEPEND_MUTEX_CREATE, "lock_list_pub create fail");
        goto RETURN;
    }

    pClient->lock_yield = HAL_MutexCreate();
    if (!pClient->lock_yield) {
        iotx_state_event(ITE_STATE_SYS_DEPEND, STATE_SYS_DEPEND_MUTEX_CREATE, "lock_yield create fail");
        goto RETURN;
    }

    pClient->lock_write_buf = HAL_MutexCreate();
    if (!pClient->lock_write_buf) {
        iotx_state_event(ITE_STATE_SYS_DEPEND, STATE_SYS_DEPEND_MUTEX_CREATE, "lock_write_buf create fail");
        goto RETURN;
    }

    pClient->lock_read_buf = HAL_MutexCreate();
    if (!pClient->lock_read_buf) {
        iotx_state_event(ITE_STATE_SYS_DEPEND, STATE_SYS_DEPEND_MUTEX_CREATE, "lock_read_buf create fail");
        goto RETURN;
    }

    connectdata.MQTTVersion = IOTX_MC_MQTT_VERSION;
    connectdata.keepAliveInterval = pInitParams->keepalive_interval_ms / 1000;


    connectdata.clientID.cstring = (char *)pInitParams->client_id;
    connectdata.username.cstring = (char *)pInitParams->username;
    connectdata.password.cstring = (char *)pInitParams->password;
    connectdata.cleansession = pInitParams->clean_session;

    if (pInitParams->request_timeout_ms < CONFIG_MQTT_REQ_TIMEOUT_MIN
        || pInitParams->request_timeout_ms > CONFIG_MQTT_REQ_TIMEOUT_MAX) {

        pClient->request_timeout_ms = CONFIG_MQTT_REQUEST_TIMEOUT;
    } else {
        pClient->request_timeout_ms = pInitParams->request_timeout_ms;
    }

#ifdef PLATFORM_HAS_DYNMEM
#if !( WITH_MQTT_DYN_BUF)
    pClient->buf_send = mqtt_malloc(pInitParams->write_buf_size);
    if (pClient->buf_send == NULL) {
        iotx_state_event(ITE_STATE_SYS_DEPEND, STATE_SYS_DEPEND_MALLOC, "");
        goto RETURN;
    }
    pClient->buf_size_send = pInitParams->write_buf_size;

    pClient->buf_read = mqtt_malloc(pInitParams->read_buf_size);
    if (pClient->buf_read == NULL) {
        iotx_state_event(ITE_STATE_SYS_DEPEND, STATE_SYS_DEPEND_MALLOC, "");
        goto RETURN;
    }
    pClient->buf_size_read = pInitParams->read_buf_size;
#else
    pClient->buf_size_send_max = pInitParams->write_buf_size;
    pClient->buf_size_read_max = pInitParams->read_buf_size;
#endif
#else
    pClient->buf_size_send = IOTX_MC_TX_MAX_LEN;
    pClient->buf_size_read = IOTX_MC_RX_MAX_LEN;
#endif

    pClient->keepalive_probes = 0;

    pClient->handle_event.h_fp = pInitParams->handle_event.h_fp;
    pClient->handle_event.pcontext = pInitParams->handle_event.pcontext;

    /* Initialize reconnect parameter */
    pClient->reconnect_param.reconnect_time_interval_ms = IOTX_MC_RECONNECT_INTERVAL_MIN_MS;
#if !WITH_MQTT_ONLY_QOS0
    iotx_mc_pub_wait_list_init(pClient);
#endif

#ifdef PLATFORM_HAS_DYNMEM
    INIT_LIST_HEAD(&pClient->list_sub_handle);
    INIT_LIST_HEAD(&pClient->list_sub_sync_ack);
#endif
    /* Initialize MQTT connect parameter */
    rc = iotx_mc_set_connect_params(pClient, &connectdata);
    if (rc < STATE_SUCCESS) {
        mc_state = IOTX_MC_STATE_INVALID;
        iotx_state_event(ITE_STATE_MQTT_COMM, rc, "");
        goto RETURN;
    }

    iotx_time_init(&pClient->next_ping_time);
    iotx_time_init(&pClient->reconnect_param.reconnect_next_time);

    memset(&pClient->ipstack, 0, sizeof(utils_network_t));

    rc = iotx_net_init_internal(&pClient->ipstack, pInitParams->host, pInitParams->port, pInitParams->pub_key);
    if (STATE_SUCCESS != rc) {
        mc_state = IOTX_MC_STATE_INVALID;
        iotx_state_event(ITE_STATE_MQTT_COMM, rc, "");
        goto RETURN;
    }

    mc_state = IOTX_MC_STATE_INITIALIZED;
    rc = STATE_SUCCESS;

RETURN :
    iotx_mc_set_client_state(pClient, mc_state);
    if (rc < STATE_SUCCESS) {
#ifdef PLATFORM_HAS_DYNMEM
        if (pClient->buf_send != NULL) {
            mqtt_free(pClient->buf_send);
            pClient->buf_send = NULL;
        }
        if (pClient->buf_read != NULL) {
            mqtt_free(pClient->buf_read);
            pClient->buf_read = NULL;
        }
#endif
        if (pClient->lock_list_pub) {
            HAL_MutexDestroy(pClient->lock_list_pub);
            pClient->lock_list_pub = NULL;
        }
        if (pClient->lock_write_buf) {
            HAL_MutexDestroy(pClient->lock_write_buf);
            pClient->lock_write_buf = NULL;
        }
        if (pClient->lock_read_buf) {
            HAL_MutexDestroy(pClient->lock_read_buf);
            pClient->lock_read_buf = NULL;
        }
        if (pClient->lock_yield) {
            HAL_MutexDestroy(pClient->lock_yield);
            pClient->lock_yield = NULL;
        }
    }

    return rc;
}

/* Initialize MQTT client */
static int iotx_mc_init(iotx_mc_client_t *pClient, iotx_mqtt_param_t *pInitParams)
{
    int rc = -1;
    iotx_mc_state_t mc_state = IOTX_MC_STATE_INVALID;
    MQTTPacket_connectData connectdata = MQTTPacket_connectData_initializer;

    if (pClient == NULL || pInitParams == NULL || pInitParams->write_buf_size == 0 || pInitParams->read_buf_size == 0) {
        return STATE_USER_INPUT_INVALID;
    }

    pClient->lock_generic = HAL_MutexCreate();
    if (!pClient->lock_generic) {
        iotx_state_event(ITE_STATE_SYS_DEPEND, STATE_SYS_DEPEND_MUTEX_CREATE, "lock_generic create fail");
        return STATE_SYS_DEPEND_MUTEX_CREATE;
    }

    pClient->lock_list_pub = HAL_MutexCreate();
    if (!pClient->lock_list_pub) {
        iotx_state_event(ITE_STATE_SYS_DEPEND, STATE_SYS_DEPEND_MUTEX_CREATE, "lock_list_pub create fail");
        goto RETURN;
    }

    pClient->lock_yield = HAL_MutexCreate();
    if (!pClient->lock_yield) {
        iotx_state_event(ITE_STATE_SYS_DEPEND, STATE_SYS_DEPEND_MUTEX_CREATE, "lock_yield create fail");
        goto RETURN;
    }

    pClient->lock_write_buf = HAL_MutexCreate();
    if (!pClient->lock_write_buf) {
        iotx_state_event(ITE_STATE_SYS_DEPEND, STATE_SYS_DEPEND_MUTEX_CREATE, "lock_write_buf create fail");
        goto RETURN;
    }

    pClient->lock_read_buf = HAL_MutexCreate();
    if (!pClient->lock_read_buf) {
        iotx_state_event(ITE_STATE_SYS_DEPEND, STATE_SYS_DEPEND_MUTEX_CREATE, "lock_read_buf create fail");
        goto RETURN;
    }

    connectdata.MQTTVersion = IOTX_MC_MQTT_VERSION;
    connectdata.keepAliveInterval = pInitParams->keepalive_interval_ms / 1000;


    connectdata.clientID.cstring = (char *)pInitParams->client_id;
    connectdata.username.cstring = (char *)pInitParams->username;
    connectdata.password.cstring = (char *)pInitParams->password;
    connectdata.cleansession = pInitParams->clean_session;

    if (pInitParams->request_timeout_ms < CONFIG_MQTT_REQ_TIMEOUT_MIN
        || pInitParams->request_timeout_ms > CONFIG_MQTT_REQ_TIMEOUT_MAX) {

        pClient->request_timeout_ms = CONFIG_MQTT_REQUEST_TIMEOUT;
    } else {
        pClient->request_timeout_ms = pInitParams->request_timeout_ms;
    }

#ifdef PLATFORM_HAS_DYNMEM
#if !( WITH_MQTT_DYN_BUF)
    pClient->buf_send = mqtt_malloc(pInitParams->write_buf_size);
    if (pClient->buf_send == NULL) {
        iotx_state_event(ITE_STATE_SYS_DEPEND, STATE_SYS_DEPEND_MALLOC, "");
        goto RETURN;
    }
    pClient->buf_size_send = pInitParams->write_buf_size;

    pClient->buf_read = mqtt_malloc(pInitParams->read_buf_size);
    if (pClient->buf_read == NULL) {
        iotx_state_event(ITE_STATE_SYS_DEPEND, STATE_SYS_DEPEND_MALLOC, "");
        goto RETURN;
    }
    pClient->buf_size_read = pInitParams->read_buf_size;
#else
    pClient->buf_size_send_max = pInitParams->write_buf_size;
    pClient->buf_size_read_max = pInitParams->read_buf_size;
#endif
#else
    pClient->buf_size_send = IOTX_MC_TX_MAX_LEN;
    pClient->buf_size_read = IOTX_MC_RX_MAX_LEN;
#endif

    pClient->keepalive_probes = 0;

    pClient->handle_event.h_fp = pInitParams->handle_event.h_fp;
    pClient->handle_event.pcontext = pInitParams->handle_event.pcontext;

    /* Initialize reconnect parameter */
    pClient->reconnect_param.reconnect_time_interval_ms = IOTX_MC_RECONNECT_INTERVAL_MIN_MS;
#if !WITH_MQTT_ONLY_QOS0
    iotx_mc_pub_wait_list_init(pClient);
#endif

#ifdef PLATFORM_HAS_DYNMEM
    INIT_LIST_HEAD(&pClient->list_sub_handle);
    INIT_LIST_HEAD(&pClient->list_sub_sync_ack);
#endif
    /* Initialize MQTT connect parameter */
    rc = iotx_mc_set_connect_params(pClient, &connectdata);
    if (rc < STATE_SUCCESS) {
        mc_state = IOTX_MC_STATE_INVALID;
        iotx_state_event(ITE_STATE_MQTT_COMM, rc, "");
        goto RETURN;
    }

    iotx_time_init(&pClient->next_ping_time);
    iotx_time_init(&pClient->reconnect_param.reconnect_next_time);

    memset(&pClient->ipstack, 0, sizeof(utils_network_t));

    rc = iotx_net_init(&pClient->ipstack, pInitParams->host, pInitParams->port, pInitParams->pub_key);
    if (STATE_SUCCESS != rc) {
        mc_state = IOTX_MC_STATE_INVALID;
        iotx_state_event(ITE_STATE_MQTT_COMM, rc, "");
        goto RETURN;
    }

    mc_state = IOTX_MC_STATE_INITIALIZED;
    rc = STATE_SUCCESS;

RETURN :
    iotx_mc_set_client_state(pClient, mc_state);
    if (rc < STATE_SUCCESS) {
#ifdef PLATFORM_HAS_DYNMEM
        if (pClient->buf_send != NULL) {
            mqtt_free(pClient->buf_send);
            pClient->buf_send = NULL;
        }
        if (pClient->buf_read != NULL) {
            mqtt_free(pClient->buf_read);
            pClient->buf_read = NULL;
        }
#endif
        if (pClient->lock_list_pub) {
            HAL_MutexDestroy(pClient->lock_list_pub);
            pClient->lock_list_pub = NULL;
        }
        if (pClient->lock_write_buf) {
            HAL_MutexDestroy(pClient->lock_write_buf);
            pClient->lock_write_buf = NULL;
        }
        if (pClient->lock_read_buf) {
            HAL_MutexDestroy(pClient->lock_read_buf);
            pClient->lock_read_buf = NULL;
        }
        if (pClient->lock_yield) {
            HAL_MutexDestroy(pClient->lock_yield);
            pClient->lock_yield = NULL;
        }
    }

    return rc;
}

#ifdef PLATFORM_HAS_DYNMEM
    #if  WITH_MQTT_DYN_BUF
        extern int MQTTPacket_len(int rem_len);
        extern int MQTTSerialize_connectLength(MQTTPacket_connectData *options);
    #endif
#endif

static int _get_connect_length(MQTTPacket_connectData *options)
{
#ifdef PLATFORM_HAS_DYNMEM
#if  WITH_MQTT_DYN_BUF
    return MQTTPacket_len(MQTTSerialize_connectLength(options));
#else
    return STATE_SUCCESS;
#endif
#else
    return STATE_SUCCESS;
#endif
}

static int _reset_send_buffer(iotx_mc_client_t *c)
{
#ifdef PLATFORM_HAS_DYNMEM
#if  WITH_MQTT_DYN_BUF
    if (c == NULL || c->buf_send == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    mqtt_free(c->buf_send);
    c->buf_send = NULL;
    c->buf_size_send = 0;
    return STATE_SUCCESS;
#else
    return STATE_SUCCESS;
#endif
#else
    return STATE_SUCCESS;
#endif
}

static int _reset_recv_buffer(iotx_mc_client_t *c)
{
#ifdef PLATFORM_HAS_DYNMEM
#if  WITH_MQTT_DYN_BUF
    if (c == NULL || c->buf_read == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    mqtt_free(c->buf_read);
    c->buf_read = NULL;
    c->buf_size_read = 0;
    return STATE_SUCCESS;
#else
    return STATE_SUCCESS;
#endif
#else
    return STATE_SUCCESS;
#endif
}

static int _alloc_send_buffer(iotx_mc_client_t *c, int len)
{
#ifdef PLATFORM_HAS_DYNMEM
#if  WITH_MQTT_DYN_BUF
    int tmp_len;

    if (c == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    tmp_len = MQTT_DYNBUF_SEND_MARGIN + len;
    if (tmp_len > c->buf_size_send_max) {
        tmp_len = c->buf_size_send_max;
    }
    if (c->buf_send != NULL) {
        mqtt_warning("c->buf_send is not null,free it first!");
        mqtt_free(c->buf_send);
    }
    c->buf_send = mqtt_malloc(tmp_len);
    if (c->buf_send == NULL) {
        return STATE_SYS_DEPEND_MALLOC;
    }
    memset(c->buf_send, 0, tmp_len);
    c->buf_size_send = tmp_len;
    return STATE_SUCCESS;
#else
    return STATE_SUCCESS;
#endif
#else
    return STATE_SUCCESS;
#endif
}

static int _alloc_recv_buffer(iotx_mc_client_t *c, int len)
{
#ifdef PLATFORM_HAS_DYNMEM
#if  WITH_MQTT_DYN_BUF
    int tmp_len;

    if (c == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    tmp_len = MQTT_DYNBUF_RECV_MARGIN + len;
    if (tmp_len > c->buf_size_read_max) {
        tmp_len = c->buf_size_read_max;
    }
    if (c->buf_read != NULL) { /* do realloc */
        char *temp = mqtt_malloc(tmp_len);
        if (temp == NULL) {
            return STATE_SYS_DEPEND_MALLOC;
        }
        memset(temp, 0, tmp_len);
        memcpy(temp, c->buf_read, c->buf_size_read < tmp_len ? c->buf_size_read : tmp_len);
        mqtt_free(c->buf_read);
        c->buf_read = temp;
    } else {
        c->buf_read = mqtt_malloc(tmp_len);
        if (c->buf_read == NULL) {
            return STATE_SYS_DEPEND_MALLOC;
        }
        memset(c->buf_read, 0, tmp_len);
    }
    c->buf_size_read = tmp_len;
    return STATE_SUCCESS;
#else
    return STATE_SUCCESS;
#endif
#else
    return STATE_SUCCESS;
#endif
}

static int iotx_mc_send_packet(iotx_mc_client_t *c, char *buf, int length, iotx_time_t *time)
{
    int rc = STATE_SUCCESS;
    int sent = 0;
    unsigned int left_t = 0;

    if (!c || !buf || !time) {
        return STATE_USER_INPUT_INVALID;
    }

    while (sent < length && !utils_time_is_expired(time)) {
        left_t = iotx_time_left(time);
        left_t = (left_t == 0) ? 1 : left_t;
        rc = c->ipstack.write(&c->ipstack, &buf[sent], length - sent, left_t);
        if (rc < 0) { /* there was an error writing the data */
            break;
        }
        sent += rc;
    }

    if (sent == length) {
        rc = STATE_SUCCESS;
    } else {
        rc = STATE_SYS_DEPEND_NWK_CLOSE;
    }
    return rc;
}

int MQTTConnect(iotx_mc_client_t *pClient)
{
    MQTTPacket_connectData *pConnectParams;
    iotx_time_t connectTimer;
    int len = 0, res = 0;

    if (!pClient) {
        return STATE_USER_INPUT_INVALID;
    }

    pConnectParams = &pClient->connect_data;
    HAL_MutexLock(pClient->lock_write_buf);

    len = _get_connect_length(pConnectParams);

    res = _alloc_send_buffer(pClient, len);
    if (res < STATE_SUCCESS) {
        HAL_MutexUnlock(pClient->lock_write_buf);
        return res;
    }

    if ((len = MQTTSerialize_connect((unsigned char *)pClient->buf_send, pClient->buf_size_send, pConnectParams)) <= 0) {
        _reset_send_buffer(pClient);
        HAL_MutexUnlock(pClient->lock_write_buf);
        return STATE_MQTT_SERIALIZE_CONN_ERROR;
    }

    /* send the connect packet */
    iotx_time_init(&connectTimer);
    utils_time_countdown_ms(&connectTimer, pClient->request_timeout_ms);
    res = iotx_mc_send_packet(pClient, pClient->buf_send, len, &connectTimer);
    if (res < STATE_SUCCESS) {
        _reset_send_buffer(pClient);
        HAL_MutexUnlock(pClient->lock_write_buf);
        return res;
    }
    _reset_send_buffer(pClient);
    HAL_MutexUnlock(pClient->lock_write_buf);
    return STATE_SUCCESS;
}

static int iotx_mc_decode_packet(iotx_mc_client_t *c, int *value, int timeout)
{
    char i;
    int multiplier = 1;
    int len = 0;
    const int MAX_NO_OF_REMAINING_LENGTH_BYTES = 4;

    if (!c || !value) {
        return STATE_USER_INPUT_INVALID;
    }

    *value = 0;
    do {
        int rc = MQTTPACKET_READ_ERROR;

        if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES) {
            return MQTTPACKET_READ_ERROR; /* bad data */
        }

        rc = c->ipstack.read(&c->ipstack, &i, 1, timeout == 0 ? 1 : timeout);
        if (rc <= STATE_SUCCESS) {
            return STATE_SYS_DEPEND_NWK_CLOSE;
        }

        *value += (i & 127) * multiplier;
        multiplier *= 128;
    } while ((i & 128) != 0);

    return len;
}

static int _handle_event(iotx_mqtt_event_handle_pt handle, iotx_mc_client_t *c, iotx_mqtt_event_msg_pt msg)
{
    if (handle == NULL || handle->h_fp == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    _in_yield_cb = 1;
    handle->h_fp(handle->pcontext, c, msg);
    _in_yield_cb = 0;
    return STATE_SUCCESS;
}

static int iotx_mc_read_packet(iotx_mc_client_t *c, iotx_time_t *timer, unsigned int *packet_type)
{
    MQTTHeader header = {0};
    int len = 0;
    int rem_len = 0;
    int rc = 0;
    unsigned int left_t = 0;

    if (!c || !timer || !packet_type) {
        return STATE_USER_INPUT_INVALID;
    }
    HAL_MutexLock(c->lock_read_buf);
    rc = _alloc_recv_buffer(c, 0);
    if (rc < 0) {
        HAL_MutexUnlock(c->lock_read_buf);
        return rc;
    }
    /* 1. read the header byte.  This has the packet type in it */
    left_t = iotx_time_left(timer);
    left_t = (left_t == 0) ? 1 : left_t;
    rc = c->ipstack.read(&c->ipstack, c->buf_read, 1, left_t);
    if (0 == rc) { /* timeout */
        *packet_type = 0;
        HAL_MutexUnlock(c->lock_read_buf);
        return STATE_SUCCESS;
    } else if (rc < STATE_SUCCESS) {
        HAL_MutexUnlock(c->lock_read_buf);
        return STATE_SYS_DEPEND_NWK_CLOSE;
    }

    len = 1;

    /* 2. read the remaining length.  This is variable in itself */
    left_t = iotx_time_left(timer);
    left_t = (left_t == 0) ? 1 : left_t;
    if ((rc = iotx_mc_decode_packet(c, &rem_len, left_t)) < 0) {
        HAL_MutexUnlock(c->lock_read_buf);
        return rc;
    }

    len += MQTTPacket_encode((unsigned char *)c->buf_read + 1,
                             rem_len); /* put the original remaining length back into the buffer */

    rc = _alloc_recv_buffer(c, rem_len + len);
    if (rc < 0) {
        HAL_MutexUnlock(c->lock_read_buf);
        return rc;
    }

    /* Check if the received data length exceeds mqtt read buffer length */
    if ((rem_len > 0) && ((rem_len + len) > c->buf_size_read)) {
        int needReadLen;

        iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_RX_BUFFER_TOO_SHORT, "");
        mqtt_err("mqtt read buffer is too short, mqttReadBufLen : %u, remainDataLen : %d", c->buf_size_read, rem_len);
        *packet_type = 0;
        left_t = iotx_time_left(timer);
        left_t = (left_t == 0) ? 1 : left_t;
        do {
            needReadLen = (rem_len > c->buf_size_read) ? c->buf_size_read : rem_len;
            mqtt_info("read len:%d\n", needReadLen);
            if (c->ipstack.read(&c->ipstack, c->buf_read, needReadLen, left_t) != needReadLen) {
                mqtt_err("mqtt read error");
                HAL_MutexUnlock(c->lock_read_buf);
                return STATE_SYS_DEPEND_NWK_READ_ERROR;
            }
            rem_len -= needReadLen;
        } while (rem_len);

        HAL_MutexUnlock(c->lock_read_buf);
        if (NULL != c->handle_event.h_fp) {
            iotx_mqtt_event_msg_t msg;

            msg.event_type = IOTX_MQTT_EVENT_BUFFER_OVERFLOW;
            msg.msg = "mqtt read buffer is too short";
            _handle_event(&c->handle_event, c, &msg);
        }

        return STATE_SUCCESS;

    }

    /* 3. read the rest of the buffer using a callback to supply the rest of the data */
    left_t = iotx_time_left(timer);
    left_t = (left_t == 0) ? 1 : left_t;

    rc = c->ipstack.read(&c->ipstack, c->buf_read + len, rem_len, left_t);
    if (rem_len > 0) {
        if (rc < 0) {
            HAL_MutexUnlock(c->lock_read_buf);
            return STATE_SYS_DEPEND_NWK_CLOSE;
        } else if (rc != rem_len) {
            HAL_MutexUnlock(c->lock_read_buf);
            return STATE_MQTT_PACKET_READ_ERROR;
        }
    }

    header.byte = c->buf_read[0];
    *packet_type = MQTT_HEADER_GET_TYPE(header.byte);
    if ((len + rem_len) < c->buf_size_read) {
        c->buf_read[len + rem_len] = '\0';
    }
    HAL_MutexUnlock(c->lock_read_buf);
    return STATE_SUCCESS;
}

static int iotx_mc_handle_recv_CONNACK(iotx_mc_client_t *c)
{
    int rc = STATE_SUCCESS;
    unsigned char connack_rc = 255;
    char sessionPresent = 0;

    if (!c) {
        return STATE_USER_INPUT_INVALID;
    }

    if (MQTTDeserialize_connack((unsigned char *)&sessionPresent, &connack_rc, (unsigned char *)c->buf_read,
                                c->buf_size_read) != 1) {
        return STATE_MQTT_DESERIALIZE_CONNACK_ERROR;
    }

    switch (connack_rc) {
        case IOTX_MC_CONNECTION_ACCEPTED:
            rc = STATE_SUCCESS;
            break;
        case IOTX_MC_CONNECTION_REFUSED_UNACCEPTABLE_PROTOCOL_VERSION:
            rc = STATE_MQTT_CONNACK_VERSION_UNACCEPT;
            break;
        case IOTX_MC_CONNECTION_REFUSED_IDENTIFIER_REJECTED:
            rc = STATE_MQTT_CONNACK_IDENT_REJECT;
            break;
        case IOTX_MC_CONNECTION_REFUSED_SERVER_UNAVAILABLE:
            rc = STATE_MQTT_CONNACK_SERVICE_NA;
            break;
        case IOTX_MC_CONNECTION_REFUSED_NOT_AUTHORIZED:
            rc = STATE_MQTT_CONNACK_NOT_AUTHORIZED;
            break;
        case IOTX_MC_CONNECTION_REFUSED_BAD_USERDATA:
            rc = STATE_MQTT_CONNACK_BAD_USERDATA;
            break;
        default:
            rc = STATE_MQTT_CONNACK_UNKNOWN_ERROR;
            break;
    }

    return rc;
}

static int iotx_mc_wait_CONNACK(iotx_mc_client_t *c)
{
#define WAIT_CONNACK_MAX (10)
    unsigned char wait_connack = 0;
    unsigned int packetType = 0;
    int rc = 0;
    iotx_time_t timer;

    if (!c) {
        return STATE_USER_INPUT_INVALID;
    }

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

    do {
        /* read the socket, see what work is due */

        rc = iotx_mc_read_packet(c, &timer, &packetType);
        if (rc < STATE_SUCCESS) {
            HAL_MutexLock(c->lock_read_buf);
            _reset_recv_buffer(c);
            HAL_MutexUnlock(c->lock_read_buf);
            return rc;
        }

        if (++wait_connack > WAIT_CONNACK_MAX) {
            HAL_MutexLock(c->lock_read_buf);
            _reset_recv_buffer(c);
            HAL_MutexUnlock(c->lock_read_buf);
            return STATE_SYS_DEPEND_NWK_CLOSE;
        }
    } while (packetType != CONNACK);
    HAL_MutexLock(c->lock_read_buf);

    rc = iotx_mc_handle_recv_CONNACK(c);
    _reset_recv_buffer(c);
    HAL_MutexUnlock(c->lock_read_buf);

    return rc;
}

static int _mqtt_connect(void *client)
{
#define RETRY_TIME_LIMIT    (8+1)
#define RETRY_INTV_PERIOD   (2000)
    int rc = STATE_SUCCESS;
    int try_count = 1;
    iotx_mc_client_t *pClient = (iotx_mc_client_t *)client;
    int userKeepAliveInterval = 0;

    if (NULL == pClient) {
        return STATE_USER_INPUT_INVALID;
    }
    userKeepAliveInterval = pClient->connect_data.keepAliveInterval;
    pClient->connect_data.keepAliveInterval = (userKeepAliveInterval * 2);
    if(pClient->connect_data.keepAliveInterval > CONFIG_MQTT_KEEPALIVE_INTERVAL_MAX) {
       pClient->connect_data.keepAliveInterval = CONFIG_MQTT_KEEPALIVE_INTERVAL_MAX;
    }
    mqtt_info("connect params: MQTTVersion=%d, clientID=%s, keepAliveInterval=%d, username=%s",
              pClient->connect_data.MQTTVersion,
              pClient->connect_data.clientID.cstring,
              pClient->connect_data.keepAliveInterval,
              pClient->connect_data.username.cstring);

    /* Establish TCP or TLS connection */
    do {
        rc = MQTTConnect(pClient);
        pClient->connect_data.keepAliveInterval = userKeepAliveInterval;

        if (rc < STATE_SUCCESS) {
            pClient->ipstack.disconnect(&pClient->ipstack);
            mqtt_err("send connect packet failed, rc = %d", rc);
            return rc;
        }

        rc = iotx_mc_wait_CONNACK(pClient);

        if (rc <= STATE_MQTT_CONNACK_VERSION_UNACCEPT && rc >= STATE_MQTT_CONNACK_NOT_AUTHORIZED) {
            mqtt_err("received reject ACK from MQTT server! rc = %d", rc);
            pClient->ipstack.disconnect(&pClient->ipstack);
            return rc;
        }

        if (STATE_SUCCESS != rc) {
            mqtt_err("wait connect ACK timeout! rc = %d", rc);
            mqtt_warning("tried [%d/%d] times CONN, waiting for %d ms...", try_count, RETRY_TIME_LIMIT - 1, RETRY_INTV_PERIOD);

            HAL_SleepMs(RETRY_INTV_PERIOD);

            pClient->ipstack.disconnect(&pClient->ipstack);
            pClient->ipstack.connect(&pClient->ipstack);
            continue;
        } else {
            break;
        }

    } while (++try_count < RETRY_TIME_LIMIT);

    if (try_count == RETRY_TIME_LIMIT) {
        return STATE_MQTT_CONN_RETRY_EXCEED_MAX;
    }

    pClient->keepalive_probes = 0;

    iotx_mc_set_client_state(pClient, IOTX_MC_STATE_CONNECTED);

    utils_time_countdown_ms(&pClient->next_ping_time, pClient->connect_data.keepAliveInterval * 1000);

    mqtt_info("mqtt connect success!");

    return STATE_SUCCESS;
}

#if !WITH_MQTT_ONLY_QOS0
static int iotx_mc_push_pubInfo_to(iotx_mc_client_t *c, int len, unsigned short msgId, iotx_mc_pub_info_t **node)
{
#ifdef PLATFORM_HAS_DYNMEM
    int list_number;
    iotx_mc_pub_info_t *repubInfo;
#else
    int idx;
#endif

    if (!c || !node) {
        return STATE_USER_INPUT_INVALID;
    }

    if ((len < 0) || (len > c->buf_size_send)) {
#ifndef PLATFORM_HAS_DYNMEM
        if (len >= c->buf_size_send) {
            mqtt_err("IOTX_MC_TX_MAX_LEN is too short, len: %d, IOTX_MC_TX_MAX_LEN: %d", len, IOTX_MC_TX_MAX_LEN);
        }
#endif
        return STATE_MQTT_TX_BUFFER_TOO_SHORT;
    }

#ifdef PLATFORM_HAS_DYNMEM
    list_number = list_entry_number(&c->list_pub_wait_ack);

    if (list_number >= IOTX_MC_REPUB_NUM_MAX) {
        mqtt_err("more than %u elements in republish list. List overflow!", list_number);
        return STATE_MQTT_QOS1_REPUB_EXCEED_MAX;
    }

    repubInfo = (iotx_mc_pub_info_t *)mqtt_malloc(sizeof(iotx_mc_pub_info_t) + len);
    if (NULL == repubInfo) {
        mqtt_err("run iotx_memory_malloc is error!");
        return STATE_SYS_DEPEND_MALLOC;
    }

    repubInfo->node_state = IOTX_MC_NODE_STATE_NORMANL;
    repubInfo->msg_id = msgId;
    repubInfo->len = len;
    iotx_time_start(&repubInfo->pub_start_time);
    repubInfo->buf = (unsigned char *)repubInfo + sizeof(iotx_mc_pub_info_t);

    memcpy(repubInfo->buf, c->buf_send, len);
    INIT_LIST_HEAD(&repubInfo->linked_list);

    list_add_tail(&repubInfo->linked_list, &c->list_pub_wait_ack);

    *node = repubInfo;
    return STATE_SUCCESS;
#else
    for (idx = 0; idx < IOTX_MC_REPUB_NUM_MAX; idx++) {
        if (c->list_pub_wait_ack[idx].used == 0) {
            c->list_pub_wait_ack[idx].node_state = IOTX_MC_NODE_STATE_NORMANL;
            c->list_pub_wait_ack[idx].msg_id = msgId;
            c->list_pub_wait_ack[idx].len = len;
            iotx_time_start(&c->list_pub_wait_ack[idx].pub_start_time);
            memcpy(c->list_pub_wait_ack[idx].buf, c->buf_send, len);
            c->list_pub_wait_ack[idx].used = 1;
            *node = &c->list_pub_wait_ack[idx];
            return STATE_SUCCESS;
        }
    }

    mqtt_err("IOTX_MC_REPUB_NUM_MAX is too short");

    return STATE_MQTT_QOS1_REPUB_EXCEED_MAX;
#endif
}

static int iotx_mc_mask_pubInfo_from(iotx_mc_client_t *c, uint16_t msgId)
{
#ifdef PLATFORM_HAS_DYNMEM
    iotx_mc_pub_info_t *node = NULL;

    if (!c) {
        return STATE_USER_INPUT_INVALID;
    }

    HAL_MutexLock(c->lock_list_pub);
    list_for_each_entry(node, &c->list_pub_wait_ack, linked_list, iotx_mc_pub_info_t) {
        if (node->msg_id == msgId) {
            node->node_state = IOTX_MC_NODE_STATE_INVALID; /* mark as invalid node */
        }
    }
    HAL_MutexUnlock(c->lock_list_pub);
#else
    int idx;

    for (idx = 0; idx < IOTX_MC_REPUB_NUM_MAX; idx++) {
        if (c->list_pub_wait_ack[idx].used &&
            c->list_pub_wait_ack[idx].msg_id == msgId) {
            c->list_pub_wait_ack[idx].node_state = IOTX_MC_NODE_STATE_INVALID; /* mark as invalid node */
        }
    }
#endif
    return STATE_SUCCESS;
}

static int MQTTRePublish(iotx_mc_client_t *c, char *buf, int len)
{
    iotx_time_t timer;
    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

    HAL_MutexLock(c->lock_write_buf);

    if (iotx_mc_send_packet(c, buf, len, &timer) != STATE_SUCCESS) {
        HAL_MutexUnlock(c->lock_write_buf);
        return STATE_SYS_DEPEND_NWK_CLOSE;
    }

    HAL_MutexUnlock(c->lock_write_buf);
    return STATE_SUCCESS;
}

static int MQTTPubInfoProc(iotx_mc_client_t *pClient)
{
    int rc = 0;
    iotx_mc_state_t state = IOTX_MC_STATE_INVALID;
#ifdef PLATFORM_HAS_DYNMEM
    iotx_mc_pub_info_t *node = NULL, *next_node = NULL;
#else
    int idx;
#endif

    if (!pClient) {
        return STATE_USER_INPUT_INVALID;
    }

    HAL_MutexLock(pClient->lock_list_pub);
#ifdef PLATFORM_HAS_DYNMEM
    list_for_each_entry_safe(node, next_node, &pClient->list_pub_wait_ack, linked_list, iotx_mc_pub_info_t) {
        /* remove invalid node */
        if (IOTX_MC_NODE_STATE_INVALID == node->node_state) {
            list_del(&node->linked_list);
            mqtt_free(node);
            continue;
        }

        state = iotx_mc_get_client_state(pClient);
        if (state != IOTX_MC_STATE_CONNECTED) {
            continue;
        }

        /* check the request if timeout or not */
        if (utils_time_spend(&node->pub_start_time) <= (pClient->request_timeout_ms * 2)) {
            continue;
        }

        /* If wait ACK timeout, republish */
        rc = MQTTRePublish(pClient, (char *)node->buf, node->len);
        iotx_time_start(&node->pub_start_time);

        if (STATE_SYS_DEPEND_NWK_CLOSE == rc) {
            iotx_mc_set_client_state(pClient, IOTX_MC_STATE_DISCONNECTED);
            break;
        }
    }
#else
    for (idx = 0; idx < IOTX_MC_REPUB_NUM_MAX; idx++) {
        if (pClient->list_pub_wait_ack[idx].used == 0) {
            continue;
        }

        if (IOTX_MC_NODE_STATE_INVALID == pClient->list_pub_wait_ack[idx].node_state) {
            memset(&pClient->list_pub_wait_ack[idx], 0, sizeof(iotx_mc_pub_info_t));
            continue;
        }

        state = iotx_mc_get_client_state(pClient);
        if (state != IOTX_MC_STATE_CONNECTED) {
            continue;
        }

        /* check the request if timeout or not */
        if (utils_time_spend(&pClient->list_pub_wait_ack[idx].pub_start_time) <= (pClient->request_timeout_ms * 2)) {
            continue;
        }

        /* If wait ACK timeout, republish */
        rc = MQTTRePublish(pClient, (char *)pClient->list_pub_wait_ack[idx].buf, pClient->list_pub_wait_ack[idx].len);
        iotx_time_start(&pClient->list_pub_wait_ack[idx].pub_start_time);

        if (STATE_SYS_DEPEND_NWK_CLOSE == rc) {
            iotx_mc_set_client_state(pClient, IOTX_MC_STATE_DISCONNECTED);
            break;
        }
    }
#endif
    HAL_MutexUnlock(pClient->lock_list_pub);

    return STATE_SUCCESS;
}

/* handle PUBACK packet received from remote MQTT broker */
static int iotx_mc_handle_recv_PUBACK(iotx_mc_client_t *c)
{
    unsigned short mypacketid;
    unsigned char dup = 0;
    unsigned char type = 0;

    if (!c) {
        return STATE_USER_INPUT_INVALID;
    }

    if (MQTTDeserialize_ack(&type, &dup, &mypacketid, (unsigned char *)c->buf_read, c->buf_size_read) != 1) {
        return MQTT_PUBLISH_ACK_PACKET_ERROR;
    }

    (void)iotx_mc_mask_pubInfo_from(c, mypacketid);

    /* call callback function to notify that PUBLISH is successful */
    if (NULL != c->handle_event.h_fp) {
        iotx_mqtt_event_msg_t msg;
        msg.event_type = IOTX_MQTT_EVENT_PUBLISH_SUCCESS;
        msg.msg = (void *)(uintptr_t)mypacketid;
        _handle_event(&c->handle_event, c, &msg);
    }

    return STATE_SUCCESS;
}
#endif

static void _iotx_mqtt_event_handle_sub(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mc_client_t *client;
    uintptr_t packet_id;
#ifdef PLATFORM_HAS_DYNMEM
    mqtt_sub_sync_node_t *node = NULL;
    mqtt_sub_sync_node_t *next = NULL;
#else
    int idx;
#endif

    if (pclient == NULL || msg == NULL) {
        return;
    }

    client = (iotx_mc_client_t *)pclient;
    packet_id = (uintptr_t) msg->msg;

    mqtt_debug("packet_id = %lu, event_type=%d", packet_id, msg->event_type);

    HAL_MutexLock(client->lock_generic);
#ifdef PLATFORM_HAS_DYNMEM
    list_for_each_entry_safe(node, next, &client->list_sub_sync_ack, linked_list, mqtt_sub_sync_node_t) {
        if (node->packet_id == packet_id) {
            node->ack_type = msg->event_type;
        }
    }
#else
    for (idx = 0; idx < IOTX_MC_SUBSYNC_LIST_MAX_LEN; idx++) {
        if (client->list_sub_sync_ack[idx].used &&
            client->list_sub_sync_ack[idx].packet_id == packet_id) {
            client->list_sub_sync_ack[idx].ack_type = msg->event_type;
        }
    }
#endif
    HAL_MutexUnlock(client->lock_generic);
}

static int iotx_mc_handle_recv_SUBACK(iotx_mc_client_t *c)
{
    unsigned short mypacketid;
    iotx_mqtt_event_msg_t msg;
    int i = 0, count = 0, fail_flag = -1, j = 0;
    int grantedQoS[MUTLI_SUBSCIRBE_MAX];
    int rc;

    if (!c) {
        return STATE_USER_INPUT_INVALID;
    }

    rc = MQTTDeserialize_suback(&mypacketid, MUTLI_SUBSCIRBE_MAX, &count, grantedQoS, (unsigned char *)c->buf_read,
                                c->buf_size_read);

    if (rc < 0) {
        mqtt_err("Sub ack packet error, rc = MQTTDeserialize_suback() = %d", rc);
        return STATE_MQTT_DESERIALIZE_SUBACK_ERROR;
    }

    mqtt_debug("%20s : %d", "Return Value", rc);
    mqtt_debug("%20s : %d", "Packet ID", mypacketid);
    mqtt_debug("%20s : %d", "Count", count);
    for (i = 0; i < count; ++i) {
        mqtt_debug("%16s[%02d] : %d", "Granted QoS", i, grantedQoS[i]);
    }

    for (j = 0; j <  count; j++) {
        fail_flag = 0;
        /* In negative case, grantedQoS will be 0xFFFF FF80, which means -128 */
        if ((uint8_t)grantedQoS[j] == 0x80) {
            fail_flag = 1;
            mqtt_err("MQTT SUBSCRIBE failed, ack code is 0x80");
        }
    }

    /* call callback function to notify that SUBSCRIBE is successful */
    msg.msg = (void *)(uintptr_t)mypacketid;
    if (fail_flag == 1) {
        msg.event_type = IOTX_MQTT_EVENT_SUBCRIBE_NACK;
    } else {
        msg.event_type = IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS;
    }

    _iotx_mqtt_event_handle_sub(c->handle_event.pcontext, c, &msg);

    if (NULL != c->handle_event.h_fp) {
        _handle_event(&c->handle_event, c, &msg);
    }

    return STATE_SUCCESS;
}

#if WITH_MQTT_ZIP_TOPIC
#define MQTT_ZIP_PATH_DEFAULT_LEN (32)

static int iotx_mc_get_zip_topic(const char *path, int len, char outbuf[], int outlen)
{
    unsigned char comp_data[MQTT_ZIP_PATH_DEFAULT_LEN] = {0};
    if (!path || !len || !outbuf || !outlen) {
        return STATE_USER_INPUT_INVALID;
    }

    utils_sha256((unsigned char *)path, (size_t)len, comp_data);

    memcpy(outbuf, comp_data, outlen > MQTT_ZIP_PATH_DEFAULT_LEN ? MQTT_ZIP_PATH_DEFAULT_LEN : outlen);
    return STATE_SUCCESS;
}
#endif

static char iotx_mc_is_topic_matched(char *topicFilter, MQTTString *topicName)
{
    char *curf;
    char *curn;
    char *curn_end;

    if (!topicFilter || !topicName) {
        return STATE_SUCCESS;
    }

    curf = topicFilter;
    curn = topicName->lenstring.data;
    curn_end = curn + topicName->lenstring.len;

    while (*curf && curn < curn_end) {
        if (*curn == '/' && *curf != '/') {
            break;
        }

        if (*curf != '+' && *curf != '#' && *curf != *curn) {
            break;
        }

        if (*curf == '+') {
            /* skip until we meet the next separator, or end of string */
            char *nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/') {
                nextpos = ++curn + 1;
            }
        } else if (*curf == '#') {
            curn = curn_end - 1;    /* skip until end of string */
        }
        curf++;
        curn++;
    }

    return (curn == curn_end) && (*curf == '\0');
}

/* /ext/auth/identity/response, payload: {"productKey":"","deviceName":""} */
static void iotx_mc_parse_identity_response(iotx_mqtt_topic_info_pt topic_msg)
{
    char *identify_topic = "/ext/auth/identity/response";
    void *callback = NULL;

    if (topic_msg->topic_len == strlen(identify_topic) &&
        (memcmp(topic_msg->ptopic, identify_topic, strlen(identify_topic)) == 0)) {
        char *payload = NULL;

        payload = HAL_Malloc(topic_msg->payload_len + 1);
        if (payload == NULL) {
            iotx_state_event(ITE_STATE_MQTT_COMM, STATE_SYS_DEPEND_MALLOC, "identify response fail");
            return;
        }
        memset(payload, 0, topic_msg->payload_len + 1);
        memcpy(payload, topic_msg->payload, topic_msg->payload_len);

        callback = iotx_event_callback(ITE_IDENTITY_RESPONSE);
        if (callback) {
            ((int (*)(const char*))callback)(payload);
        }

        HAL_Free(payload);

        /* parse payload then set product key and device name */
        {
            char *product_key = NULL, *device_name = NULL, *str_product_key = NULL, *str_device_name = NULL;
            uint32_t product_key_len = 0, device_name_len = 0;
            if (infra_json_value(topic_msg->payload, topic_msg->payload_len, "productKey", strlen("productKey"), &product_key, &product_key_len) != STATE_SUCCESS ||
                infra_json_value(topic_msg->payload, topic_msg->payload_len, "deviceName", strlen("deviceName"), &device_name, &device_name_len) != STATE_SUCCESS) {
                return;
            }

            str_product_key = HAL_Malloc(product_key_len - 2 + 1);
            if (str_product_key == NULL) {
                iotx_state_event(ITE_STATE_MQTT_COMM, STATE_SYS_DEPEND_MALLOC, "identify response fail");
                return;
            }
            memset(str_product_key, 0, product_key_len - 2 + 1);
            memcpy(str_product_key, product_key + 1, product_key_len - 2);

            str_device_name = HAL_Malloc(device_name_len - 2 + 1);
            if (str_device_name == NULL) {
                HAL_Free(str_product_key);
                iotx_state_event(ITE_STATE_MQTT_COMM, STATE_SYS_DEPEND_MALLOC, "identify response fail");
                return;
            }
            memset(str_device_name, 0, device_name_len - 2 + 1);
            memcpy(str_device_name, device_name + 1, device_name_len - 2);

            IOT_Ioctl(IOTX_IOCTL_SET_PRODUCT_KEY, str_product_key);
            IOT_Ioctl(IOTX_IOCTL_SET_DEVICE_NAME, str_device_name);

            HAL_Free(str_product_key);
            HAL_Free(str_device_name);
        }
    }
}

static void iotx_mc_deliver_message(iotx_mc_client_t *c, MQTTString *topicName, iotx_mqtt_topic_info_pt topic_msg)
{
    int flag_matched = 0;
    MQTTString *compare_topic = NULL;
#ifdef PLATFORM_HAS_DYNMEM
    iotx_mc_topic_handle_t *node = NULL;
#else
    int idx = 0;
#endif

#if WITH_MQTT_ZIP_TOPIC
    MQTTString      md5_topic;
    char            md5_topic_data[MQTT_ZIP_PATH_DEFAULT_LEN] = {0};
    char           *net_topic;
    uint32_t        net_topic_len;
#endif

    if (!c || !topicName || !topic_msg) {
        return;
    }

    topic_msg->ptopic = topicName->lenstring.data;
    topic_msg->topic_len = topicName->lenstring.len;

    iotx_mc_parse_identity_response(topic_msg);

#if WITH_MQTT_ZIP_TOPIC
    if (topicName->cstring) {
        net_topic = topicName->cstring;
        net_topic_len = strlen(topicName->cstring);
    } else {
        net_topic = topicName->lenstring.data;
        net_topic_len = topicName->lenstring.len;
    }
    md5_topic.cstring = NULL;
    md5_topic.lenstring.data = md5_topic_data;
    md5_topic.lenstring.len = MQTT_ZIP_PATH_DEFAULT_LEN;
    iotx_mc_get_zip_topic(net_topic, net_topic_len, md5_topic_data, MQTT_ZIP_PATH_DEFAULT_LEN);
    compare_topic = &md5_topic;
#else
    compare_topic = topicName;
#endif

    /* we have to find the right message handler - indexed by topic */
    HAL_MutexLock(c->lock_generic);
#ifdef PLATFORM_HAS_DYNMEM
    list_for_each_entry(node, &c->list_sub_handle, linked_list, iotx_mc_topic_handle_t) {
        if (MQTTPacket_equals(compare_topic, (char *)node->topic_filter)
            || iotx_mc_is_topic_matched((char *)node->topic_filter, topicName)) {
            mqtt_debug("topic be matched");

            HAL_MutexUnlock(c->lock_generic);
            if (NULL != node->handle.h_fp) {
                iotx_mqtt_event_msg_t msg;
                msg.event_type = IOTX_MQTT_EVENT_PUBLISH_RECEIVED;
                msg.msg = (void *)topic_msg;
                _handle_event(&node->handle, c, &msg);
                flag_matched = 1;
            }
            HAL_MutexLock(c->lock_generic);
        }
    }
#else
    for (idx = 0; idx < IOTX_MC_SUBHANDLE_LIST_MAX_LEN; idx++) {
        if ((c->list_sub_handle[idx].used == 1) &&
            (MQTTPacket_equals(compare_topic, (char *)c->list_sub_handle[idx].topic_filter)
             || iotx_mc_is_topic_matched((char *)c->list_sub_handle[idx].topic_filter, topicName))) {
            mqtt_debug("topic be matched");

            HAL_MutexUnlock(c->lock_generic);
            if (NULL != c->list_sub_handle[idx].handle.h_fp) {
                iotx_mqtt_event_msg_t msg;
                msg.event_type = IOTX_MQTT_EVENT_PUBLISH_RECEIVED;
                msg.msg = (void *)topic_msg;
                _handle_event(&c->list_sub_handle[idx].handle, c, &msg);
                flag_matched = 1;
            }
            HAL_MutexLock(c->lock_generic);
        }
    }
#endif
    HAL_MutexUnlock(c->lock_generic);

    if (0 == flag_matched) {
        mqtt_info("NO matching any topic, call default handle function");

        if (NULL != c->handle_event.h_fp) {
            iotx_mqtt_event_msg_t msg;

            msg.event_type = IOTX_MQTT_EVENT_PUBLISH_RECEIVED;
            msg.msg = topic_msg;
            _handle_event(&c->handle_event, c, &msg);
        }
    }
}

static int MQTTPuback(iotx_mc_client_t *c, unsigned int msgId, enum msgTypes type)
{
    int rc = 0;
    int len = 0;
    iotx_time_t timer;

    if (!c) {
        return STATE_USER_INPUT_INVALID;
    }

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

    HAL_MutexLock(c->lock_write_buf);
    if (type == PUBACK) {

        rc = _alloc_send_buffer(c, 0);
        if (rc < STATE_SUCCESS) {
            HAL_MutexUnlock(c->lock_write_buf);
            return rc;
        }

        len = MQTTSerialize_ack((unsigned char *)c->buf_send, c->buf_size_send, PUBACK, 0, msgId);
#if WITH_MQTT_QOS2_PACKET
    } else if (type == PUBREC) {
        rc = _alloc_send_buffer(c, 0);
        if (rc < STATE_SUCCESS) {
            HAL_MutexUnlock(c->lock_write_buf);
            return rc;
        }
        len = MQTTSerialize_ack((unsigned char *)c->buf_send, c->buf_size_send, PUBREC, 0, msgId);
    } else if (type == PUBREL) {
        rc = _alloc_send_buffer(c, 0);
        if (rc < STATE_SUCCESS) {
            HAL_MutexUnlock(c->lock_write_buf);
            return rc;
        }
        len = MQTTSerialize_ack((unsigned char *)c->buf_send, c->buf_size_send, PUBREL, 0, msgId);
#endif  /* #if WITH_MQTT_QOS2_PACKET */
    } else {
        HAL_MutexUnlock(c->lock_write_buf);
        return STATE_MQTT_SERIALIZE_PUBACK_ERROR;
    }

    if (len <= 0) {
        _reset_send_buffer(c);
        HAL_MutexUnlock(c->lock_write_buf);
        return STATE_MQTT_SERIALIZE_PUBACK_ERROR;
    }

    rc = iotx_mc_send_packet(c, c->buf_send, len, &timer);
    if (rc < STATE_SUCCESS) {
        _reset_send_buffer(c);
        HAL_MutexUnlock(c->lock_write_buf);
        return rc;
    }

    _reset_send_buffer(c);
    HAL_MutexUnlock(c->lock_write_buf);
    return STATE_SUCCESS;
}

static int iotx_mc_handle_recv_PUBLISH(iotx_mc_client_t *c)
{
    int result = 0;
    MQTTString topicName;
    iotx_mqtt_topic_info_t topic_msg;
    int qos = 0;
    uint32_t payload_len = 0;
#ifdef INFRA_LOG_NETWORK_PAYLOAD
    const char     *json_payload = NULL;
#endif

    if (!c) {
        return STATE_USER_INPUT_INVALID;
    }

    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    memset(&topicName, 0x0, sizeof(MQTTString));

    if (1 != MQTTDeserialize_publish((unsigned char *)&topic_msg.dup,
                                     (int *)&qos,
                                     (unsigned char *)&topic_msg.retain,
                                     (unsigned short *)&topic_msg.packet_id,
                                     &topicName,
                                     (unsigned char **)&topic_msg.payload,
                                     (int *)&payload_len,
                                     (unsigned char *)c->buf_read,
                                     c->buf_size_read)) {
        return MQTT_PUBLISH_PACKET_ERROR;
    }
    topic_msg.qos = (unsigned char)qos;
    topic_msg.payload_len = payload_len;

    if (topicName.lenstring.len == 0 || topicName.lenstring.data == NULL) {
        mqtt_err("Null topicName");
        return STATE_MQTT_DESERIALIZE_PUB_ERROR;
    }

#ifdef INFRA_LOG_NETWORK_PAYLOAD

    json_payload = (const char *)topic_msg.payload;
    mqtt_info("Downstream Topic: '%.*s'", topicName.lenstring.len, topicName.lenstring.data);
    mqtt_info("Downstream Payload:");
    iotx_facility_json_print(json_payload, LOG_INFO_LEVEL, '<');

#endif  /* #ifdef INFRA_LOG */

    mqtt_debug("%20s : %08d", "Packet Ident", topic_msg.packet_id);
    mqtt_debug("%20s : %d", "Topic Length", topicName.lenstring.len);
    mqtt_debug("%20s : %.*s",
               "Topic Name",
               topicName.lenstring.len,
               topicName.lenstring.data);
    mqtt_debug("%20s : %u / %d", "Payload Len/Room",
               (unsigned int)topic_msg.payload_len,
               (int)(c->buf_read + c->buf_size_read - topic_msg.payload));
    mqtt_debug("%20s : %lu", "Receive Buflen", c->buf_size_read);

#if defined(INSPECT_MQTT_FLOW)
    mqtt_debug("%20s : %p", "Payload Buffer", topic_msg.payload);
    mqtt_debug("%20s : %p", "Receive Buffer", c->buf_read);
#ifdef INFRA_LOG
    HEXDUMP_DEBUG(topic_msg.payload, topic_msg.payload_len);
#endif
#endif
#if defined(LOG_REPORT_TO_CLOUD) && !defined(DEVICE_MODEL_RAWDATA_SOLO)
    get_msgid(topicName.lenstring.data, 1);
#endif

    topic_msg.ptopic = NULL;
    topic_msg.topic_len = 0;

    mqtt_debug("delivering msg ...");

#if WITH_MQTT_FLOW_CTRL
    /* flowControl for specific topic */
    static uint64_t time_prev = 0;
    uint64_t time_curr = 0;
    char *filterStr = "{\"method\":\"thing.service.property.set\"";
    int filterLen = strlen(filterStr);

    if (0 == memcmp(topic_msg.payload, filterStr, filterLen)) {
        time_curr = HAL_UptimeMs();
        if (time_curr < time_prev) {
            time_curr = time_prev;
        }
        if ((time_curr - time_prev) <= (uint64_t)50) {
            mqtt_info("MQTT over threshould");
            return STATE_SUCCESS;
        } else {
            time_prev = time_curr;
        }
    }
#endif

    iotx_mc_deliver_message(c, &topicName, &topic_msg);

    if (topic_msg.qos == IOTX_MQTT_QOS0) {
        return STATE_SUCCESS;
    } else if (topic_msg.qos == IOTX_MQTT_QOS1) {
        result = MQTTPuback(c, topic_msg.packet_id, PUBACK);
    } else if (topic_msg.qos == IOTX_MQTT_QOS2) {
        result = MQTTPuback(c, topic_msg.packet_id, PUBREC);
    } else {
        mqtt_err("Invalid QOS, QOSvalue = %d", topic_msg.qos);
        return STATE_MQTT_PUB_QOS_INVALID;
    }

    return result;
}

static int iotx_mc_handle_recv_UNSUBACK(iotx_mc_client_t *c)
{
    unsigned short mypacketid = 0;  /* should be the same as the packetid above */
    if (!c) {
        return STATE_USER_INPUT_INVALID;
    }

    if (MQTTDeserialize_unsuback(&mypacketid, (unsigned char *)c->buf_read, c->buf_size_read) != 1) {
        return STATE_MQTT_DESERIALIZE_UNSUBACK_ERROR;
    }

    if (NULL != c->handle_event.h_fp) {
        iotx_mqtt_event_msg_t msg;
        msg.event_type = IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS;
        msg.msg = (void *)(uintptr_t)mypacketid;
        _handle_event(&c->handle_event, c, &msg);
    }

    return STATE_SUCCESS;
}

static int iotx_mc_cycle(iotx_mc_client_t *c, iotx_time_t *timer)
{
    unsigned int packetType;
    iotx_mc_state_t state;
    int rc = STATE_SUCCESS;

    if (!c) {
        return STATE_USER_INPUT_INVALID;
    }

    state = iotx_mc_get_client_state(c);
    if (state != IOTX_MC_STATE_CONNECTED) {
        return STATE_MQTT_IN_OFFLINE_STATUS;
    }


    if (IOTX_MC_KEEPALIVE_PROBE_MAX < c->keepalive_probes) {
        iotx_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        c->keepalive_probes = 0;
        mqtt_debug("keepalive_probes more than %u, disconnected\n", IOTX_MC_KEEPALIVE_PROBE_MAX);
        return STATE_MQTT_IN_OFFLINE_STATUS;
    }

    /* read the socket, see what work is due */
    rc = iotx_mc_read_packet(c, timer, &packetType);
    if (rc != STATE_SUCCESS) {
        HAL_MutexLock(c->lock_read_buf);
        _reset_recv_buffer(c);
        HAL_MutexUnlock(c->lock_read_buf);
        if (rc == STATE_SYS_DEPEND_NWK_CLOSE) {
            iotx_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        }
        mqtt_err("readPacket error,result = %d", rc);
        return STATE_SYS_DEPEND_NWK_CLOSE;
    }

    if (MQTT_CPT_RESERVED == packetType) {
        /* mqtt_debug("wait data timeout"); */
        HAL_MutexLock(c->lock_read_buf);
        _reset_recv_buffer(c);
        HAL_MutexUnlock(c->lock_read_buf);
        return STATE_SUCCESS;
    }

    /* clear ping mark when any data received from MQTT broker */
    c->keepalive_probes = 0;
    HAL_MutexLock(c->lock_read_buf);
    switch (packetType) {
        case CONNACK: {
            mqtt_debug("CONNACK");
            break;
        }
#if !WITH_MQTT_ONLY_QOS0
        case PUBACK: {
            mqtt_debug("PUBACK");
            rc = iotx_mc_handle_recv_PUBACK(c);
            if (rc < STATE_SUCCESS) {
                mqtt_err("recvPubackProc error,result = %d", rc);
            }

            break;
        }
#endif
        case SUBACK: {
            mqtt_debug("SUBACK");
            rc = iotx_mc_handle_recv_SUBACK(c);
            if (rc < STATE_SUCCESS) {
                mqtt_err("recvSubAckProc error,result = %d", rc);
            }
            break;
        }
        case PUBLISH: {
            mqtt_debug("PUBLISH");
            /* HEXDUMP_DEBUG(c->buf_read, 32); */

            rc = iotx_mc_handle_recv_PUBLISH(c);
            if (rc < STATE_SUCCESS) {
                mqtt_err("recvPublishProc error,result = %d", rc);
            }
            break;
        }
        case UNSUBACK: {
            mqtt_debug("UNSUBACK");
            rc = iotx_mc_handle_recv_UNSUBACK(c);
            if (rc < STATE_SUCCESS) {
                mqtt_err("recvUnsubAckProc error,result = %d", rc);
            }
            break;
        }
        case PINGRESP: {
            rc = STATE_SUCCESS;
            mqtt_info("receive ping response!");
            break;
        }
        default:
            mqtt_err("INVALID TYPE");
            _reset_recv_buffer(c);
            HAL_MutexUnlock(c->lock_read_buf);
            return STATE_MQTT_RECV_UNKNOWN_PACKET;
    }
    _reset_recv_buffer(c);
    HAL_MutexUnlock(c->lock_read_buf);
    return rc;
}

void _mqtt_cycle(void *client)
{
    int                 rc = STATE_SUCCESS;
    iotx_time_t         time;
    iotx_mc_client_t *pClient = (iotx_mc_client_t *)client;

    iotx_time_init(&time);
    utils_time_countdown_ms(&time, pClient->cycle_timeout_ms);

    do {
        unsigned int left_t;

        if (rc < STATE_SUCCESS) {
            /*mqtt_err("error occur rc=%d", rc);*/
        }

        HAL_MutexLock(pClient->lock_yield);

        /* acquire package in cycle, such as PINGRESP or PUBLISH */
        rc = iotx_mc_cycle(pClient, &time);
        if (rc == STATE_SUCCESS) {
#ifndef ASYNC_PROTOCOL_STACK
#if !WITH_MQTT_ONLY_QOS0
            /* check list of wait publish ACK to remove node that is ACKED or timeout */
            MQTTPubInfoProc(pClient);
#endif
#endif
        }
        HAL_MutexUnlock(pClient->lock_yield);

        left_t = iotx_time_left(&time);
        if (left_t < 10) {
            HAL_SleepMs(left_t);
        } else {
            HAL_SleepMs(10);
        }
    } while (!utils_time_is_expired(&time));
}

static int MQTTKeepalive(iotx_mc_client_t *pClient)
{
    int len = 0;
    int rc = 0;
    /* there is no ping outstanding - send ping packet */
    iotx_time_t timer;

    if (!pClient) {
        return STATE_USER_INPUT_INVALID;
    }

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, 1000);

    HAL_MutexLock(pClient->lock_write_buf);
    rc = _alloc_send_buffer(pClient, 0);
    if (rc < STATE_SUCCESS) {
        HAL_MutexUnlock(pClient->lock_write_buf);
        return rc;
    }

    len = MQTTSerialize_pingreq((unsigned char *)pClient->buf_send, pClient->buf_size_send);
    mqtt_debug("len = MQTTSerialize_pingreq() = %d", len);

    if (len <= 0) {
        mqtt_err("Serialize ping request is error");
        _reset_send_buffer(pClient);
        HAL_MutexUnlock(pClient->lock_write_buf);
        return STATE_MQTT_SERIALIZE_PINGREQ_ERROR;
    }

    rc = iotx_mc_send_packet(pClient, pClient->buf_send, len, &timer);
    if (rc < STATE_SUCCESS) {
        /* ping outstanding, then close socket unsubscribe topic and handle callback function */
        mqtt_err("ping outstanding is error,result = %d", rc);

        _reset_send_buffer(pClient);
        HAL_MutexUnlock(pClient->lock_write_buf);
        return STATE_SYS_DEPEND_NWK_CLOSE;
    }
    _reset_send_buffer(pClient);
    HAL_MutexUnlock(pClient->lock_write_buf);
    return STATE_SUCCESS;
}

static int iotx_mc_keepalive_sub(iotx_mc_client_t *pClient)
{
    int rc = STATE_SUCCESS;

    if (NULL == pClient) {
        return STATE_USER_INPUT_INVALID;
    }

    /* if in disabled state, without having to send ping packets */
    if (!wrapper_mqtt_check_state(pClient)) {
        return STATE_SUCCESS;
    }

    /* if there is no ping_timer timeout, then return success */
    if (!utils_time_is_expired(&pClient->next_ping_time)) {
        return STATE_SUCCESS;
    }

    /* update to next time sending MQTT keep-alive */
    utils_time_countdown_ms(&pClient->next_ping_time, pClient->connect_data.keepAliveInterval * 1000);

    rc = MQTTKeepalive(pClient);
    if (rc < STATE_SUCCESS) {
        if (rc == STATE_SYS_DEPEND_NWK_CLOSE) {
            iotx_mc_set_client_state(pClient, IOTX_MC_STATE_DISCONNECTED);
        }
        mqtt_err("ping outstanding is error,result = %d", rc);
        return rc;
    }

    mqtt_info("send MQTT ping...");
    pClient->keepalive_probes++;
    return STATE_SUCCESS;
}

int _mqtt_nwk_connect(void *client);
static int iotx_mc_attempt_reconnect(iotx_mc_client_t *pClient)
{
    int rc;
    if (pClient == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    pClient->ipstack.disconnect(&pClient->ipstack);

    mqtt_info("reconnect params: MQTTVersion=%d, clientID=%s, keepAliveInterval=%d, username=%s",
              pClient->connect_data.MQTTVersion,
              pClient->connect_data.clientID.cstring,
              pClient->connect_data.keepAliveInterval,
              pClient->connect_data.username.cstring);

    /* Ignoring return code. failures expected if network is disconnected */
    rc = _mqtt_nwk_connect(pClient);
    if (rc < STATE_SUCCESS) {
        mqtt_err("run _mqtt_nwk_connect() error!");
    }

    rc = _mqtt_connect(pClient);
    if (rc < STATE_SUCCESS && rc != STATE_MQTT_ASYNC_STACK_CONN_IN_PROG) {
        mqtt_err("run _mqtt_connect() error!");
    }

    return rc;
}

static int iotx_mc_handle_reconnect(iotx_mc_client_t *pClient)
{
    int             rc = STATE_SUCCESS;
    uint32_t        interval_ms = 0;

    if (NULL == pClient) {
        return STATE_USER_INPUT_INVALID;
    }

    if (!utils_time_is_expired(&(pClient->reconnect_param.reconnect_next_time))) {
        /* Timer has not expired. Not time to attempt reconnect yet. Return attempting reconnect */
        HAL_SleepMs(100);
        return STATE_MQTT_WAIT_RECONN_TIMER;
    }

    mqtt_info("start to reconnect");
    /*
        rc = _conn_info_dynamic_reload(pClient);
        if (STATE_SUCCESS != rc) {
            mqtt_err("update connect info err");
            return -1;
        }
    */
    rc = iotx_mc_attempt_reconnect(pClient);
    if (rc == STATE_SUCCESS) {
        iotx_mc_set_client_state(pClient, IOTX_MC_STATE_CONNECTED);
        /*
                _conn_info_dynamic_reload_clear(pClient);
        */
        return STATE_SUCCESS;
    } else if (STATE_MQTT_ASYNC_STACK_CONN_IN_PROG == rc) {
        return rc;
    } else {
        /* if reconnect network failed, then increase currentReconnectWaitInterval */
        /* e.g. init currentReconnectWaitInterval=1s, reconnect failed, then 2s..4s..8s */
        if (IOTX_MC_RECONNECT_INTERVAL_MAX_MS > pClient->reconnect_param.reconnect_time_interval_ms) {
            pClient->reconnect_param.reconnect_time_interval_ms *= 2;
        } else {
            pClient->reconnect_param.reconnect_time_interval_ms = IOTX_MC_RECONNECT_INTERVAL_MAX_MS;
        }
    }
    /*
        _conn_info_dynamic_reload_clear(pClient);
    */
    interval_ms = pClient->reconnect_param.reconnect_time_interval_ms;
    if (IOTX_MC_RECONNECT_INTERVAL_MAX_MS < interval_ms) {
        interval_ms = IOTX_MC_RECONNECT_INTERVAL_MAX_MS;
    }
    utils_time_countdown_ms(&(pClient->reconnect_param.reconnect_next_time), interval_ms);

    mqtt_err("mqtt reconnect failed rc = %d", rc);

    return rc;
}

static void iotx_mc_reconnect_callback(iotx_mc_client_t *pClient)
{

    /* handle callback function */
    if (NULL != pClient->handle_event.h_fp) {
        iotx_mqtt_event_msg_t msg;
        msg.event_type = IOTX_MQTT_EVENT_RECONNECT;
        msg.msg = NULL;

        pClient->handle_event.h_fp(pClient->handle_event.pcontext,
                                   pClient,
                                   &msg);
    }
}

static void iotx_mc_disconnect_callback(iotx_mc_client_t *pClient)
{

    if (NULL != pClient->handle_event.h_fp) {
        iotx_mqtt_event_msg_t msg;
        msg.event_type = IOTX_MQTT_EVENT_DISCONNECT;
        msg.msg = NULL;

        pClient->handle_event.h_fp(pClient->handle_event.pcontext,
                                   pClient,
                                   &msg);
    }
}

static void iotx_mc_keepalive(iotx_mc_client_t *pClient)
{
    int rc = 0;
    iotx_mc_state_t currentState;

    if (!pClient) {
        return;
    }

    /* Periodic sending ping packet to detect whether the network is connected */
    iotx_mc_keepalive_sub(pClient);

    currentState = iotx_mc_get_client_state(pClient);
    do {
        /* if Exceeds the maximum delay time, then return reconnect timeout */
        if (IOTX_MC_STATE_DISCONNECTED_RECONNECTING == currentState ||
            IOTX_MC_STATE_CONNECT_BLOCK == currentState) {
            /* Reconnection is successful, Resume regularly ping packets */
            rc = iotx_mc_handle_reconnect(pClient);
            if (rc < STATE_SUCCESS) {
                if (rc == STATE_MQTT_ASYNC_STACK_CONN_IN_PROG) {
                    mqtt_debug("now using async protocol stack, wait network connected...");
                }
            } else {
                mqtt_info("network is reconnected!");
                iotx_mc_reconnect_callback(pClient);
                pClient->reconnect_param.reconnect_time_interval_ms = IOTX_MC_RECONNECT_INTERVAL_MIN_MS;
            }

            break;
        }

        /* If network suddenly interrupted, stop pinging packet, try to reconnect network immediately */
        if (currentState == IOTX_MC_STATE_DISCONNECTED) {
            mqtt_err("network is disconnected!");
            iotx_mc_disconnect_callback(pClient);

            pClient->reconnect_param.reconnect_time_interval_ms = IOTX_MC_RECONNECT_INTERVAL_MIN_MS;
            utils_time_countdown_ms(&(pClient->reconnect_param.reconnect_next_time),
                                    pClient->reconnect_param.reconnect_time_interval_ms);

            pClient->ipstack.disconnect(&pClient->ipstack);
            iotx_mc_set_client_state(pClient, IOTX_MC_STATE_DISCONNECTED_RECONNECTING);
            break;
        }

    } while (0);
}

static int iotx_mc_check_handle_is_identical_ex(iotx_mc_topic_handle_t *messageHandlers1,
        iotx_mc_topic_handle_t *messageHandler2)
{
    int topicNameLen = 0;

    if (!messageHandlers1 || !messageHandler2) {
        return 1;
    }

    if (!(messageHandlers1->topic_filter) || !(messageHandler2->topic_filter)) {
        return 1;
    }

#if !(WITH_MQTT_ZIP_TOPIC)
    topicNameLen = strlen(messageHandlers1->topic_filter);

    if (topicNameLen != strlen(messageHandler2->topic_filter)) {
        return 1;
    }

    if (0 != strncmp(messageHandlers1->topic_filter, messageHandler2->topic_filter, topicNameLen)) {
        return 1;
    }
#else

    if (messageHandlers1->topic_type != messageHandler2->topic_type) {
        return 1;
    }

    if (messageHandlers1->topic_type == TOPIC_NAME_TYPE) {
        int i;
        for (i = 0; i < MQTT_ZIP_PATH_DEFAULT_LEN; i++) {
            if (messageHandler2->topic_filter[i] != messageHandlers1->topic_filter[i]) {
                return 1;
            }
        }
    } else {
        topicNameLen = strlen(messageHandlers1->topic_filter);

        if (topicNameLen != strlen(messageHandler2->topic_filter)) {
            return 1;
        }

        if (0 != strncmp(messageHandlers1->topic_filter, messageHandler2->topic_filter, topicNameLen)) {
            return 1;
        }
    }
#endif

    return 0;
}

static int iotx_mc_check_handle_is_identical(iotx_mc_topic_handle_t *messageHandlers1,
        iotx_mc_topic_handle_t *messageHandler2)
{
    if (iotx_mc_check_handle_is_identical_ex(messageHandlers1, messageHandler2) != 0) {
        return 1;
    }

    if (messageHandlers1->handle.h_fp != messageHandler2->handle.h_fp) {
        return 1;
    }

    /* context must be identical also */
    if (messageHandlers1->handle.pcontext != messageHandler2->handle.pcontext) {
        return 1;
    }

    return 0;
}

static int MQTTSubscribe(iotx_mc_client_t *c, const char *topicFilter, iotx_mqtt_qos_t qos, unsigned int msgId,
                         iotx_mqtt_event_handle_func_fpt messageHandler, void *pcontext)
{
    int                         len = 0, res = 0;
    iotx_time_t                 timer;
    MQTTString                  topic = MQTTString_initializer;
    /*iotx_mc_topic_handle_t handler = {topicFilter, {messageHandler, pcontext}};*/
    iotx_mc_topic_handle_t     *handler = NULL;
#ifndef PLATFORM_HAS_DYNMEM
    int idx = 0;
#endif

    if (!c || !topicFilter || !messageHandler) {
        return STATE_USER_INPUT_INVALID;
    }
#if !( WITH_MQTT_DYN_BUF)
    if (!c->buf_send) {
        return STATE_USER_INPUT_INVALID;
    }
#endif

    topic.cstring = (char *)topicFilter;
    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

#ifdef PLATFORM_HAS_DYNMEM
    handler = mqtt_malloc(sizeof(iotx_mc_topic_handle_t));
    if (NULL == handler) {
        return STATE_SYS_DEPEND_MALLOC;
    }
    memset(handler, 0, sizeof(iotx_mc_topic_handle_t));
    INIT_LIST_HEAD(&handler->linked_list);
#else
    for (idx = 0; idx < IOTX_MC_SUBHANDLE_LIST_MAX_LEN; idx++) {
        if (c->list_sub_handle[idx].used == 0) {
            handler = &c->list_sub_handle[idx];
            memset(handler, 0, sizeof(iotx_mc_topic_handle_t));
            c->list_sub_handle[idx].used = 1;
            break;
        }
    }

    if (handler == NULL) {
        return STATE_MQTT_SUB_EXCEED_MAX;
    }
#endif

#if !(WITH_MQTT_ZIP_TOPIC)
#ifdef PLATFORM_HAS_DYNMEM
    handler->topic_filter = mqtt_malloc(strlen(topicFilter) + 1);
    if (NULL == handler->topic_filter) {
        mqtt_free(handler);
        return STATE_SYS_DEPEND_MALLOC;
    }
    memset((char *)handler->topic_filter, 0, strlen(topicFilter) + 1);
#else
    if (strlen(topicFilter) >= CONFIG_MQTT_TOPIC_MAXLEN) {
        memset(handler, 0, sizeof(iotx_mc_topic_handle_t));
        return STATE_MQTT_TOPIC_BUF_TOO_SHORT;
    }

    memset((char *)handler->topic_filter, 0, CONFIG_MQTT_TOPIC_MAXLEN);
#endif
    memcpy((char *)handler->topic_filter, topicFilter, strlen(topicFilter) + 1);
#else
    if (strstr(topicFilter, "/+") != NULL || strstr(topicFilter, "/#") != NULL) {
#ifdef PLATFORM_HAS_DYNMEM
        handler->topic_filter = mqtt_malloc(strlen(topicFilter) + 1);
        if (NULL == handler->topic_filter) {
            mqtt_free(handler);
            return STATE_SYS_DEPEND_MALLOC;
        }
        memset((char *)handler->topic_filter, 0, strlen(topicFilter) + 1);
#else
        if (strlen(topicFilter) >= CONFIG_MQTT_TOPIC_MAXLEN) {
            memset(handler, 0, sizeof(iotx_mc_topic_handle_t));
            return STATE_MQTT_TOPIC_BUF_TOO_SHORT;
        }
        memset((char *)handler->topic_filter, 0, CONFIG_MQTT_TOPIC_MAXLEN);
#endif
        handler->topic_type = TOPIC_FILTER_TYPE;
        memcpy((char *)handler->topic_filter, topicFilter, strlen(topicFilter) + 1);
    } else {
#ifdef PLATFORM_HAS_DYNMEM
        handler->topic_filter = mqtt_malloc(MQTT_ZIP_PATH_DEFAULT_LEN);
        if (NULL == handler->topic_filter) {
            mqtt_free(handler);
            return STATE_SYS_DEPEND_MALLOC;
        }
        memset((char *)handler->topic_filter, 0, MQTT_ZIP_PATH_DEFAULT_LEN);
#else
        if (MQTT_ZIP_PATH_DEFAULT_LEN >= CONFIG_MQTT_TOPIC_MAXLEN) {
            memset(handler, 0, sizeof(iotx_mc_topic_handle_t));
            return MQTT_TOPIC_LEN_TOO_SHORT;
        }
        memset((char *)handler->topic_filter, 0, CONFIG_MQTT_TOPIC_MAXLEN);
#endif
        handler->topic_type = TOPIC_NAME_TYPE;
        res = iotx_mc_get_zip_topic(topicFilter, strlen(topicFilter), (char *)handler->topic_filter, MQTT_ZIP_PATH_DEFAULT_LEN);
        if (res < STATE_SUCCESS) {
#ifdef PLATFORM_HAS_DYNMEM
            mqtt_free(handler->topic_filter);
            mqtt_free(handler);
#else
            memset(handler, 0, sizeof(iotx_mc_topic_handle_t));
#endif
            return res;
        }
    }
#endif
    handler->handle.h_fp = messageHandler;
    handler->handle.pcontext = pcontext;

    if (qos == IOTX_MQTT_QOS3_SUB_LOCAL) {
        uint8_t dup = 0;
#ifdef PLATFORM_HAS_DYNMEM
        iotx_mc_topic_handle_t *node;
#endif
        HAL_MutexLock(c->lock_generic);
#ifdef PLATFORM_HAS_DYNMEM
#if defined(INSPECT_MQTT_FLOW) && defined (INFRA_LOG)
#if WITH_MQTT_ZIP_TOPIC
        HEXDUMP_DEBUG(handler->topic_filter, MQTT_ZIP_PATH_DEFAULT_LEN);
#else
        mqtt_warning("handler->topic: %s", handler->topic_filter);
#endif
#endif
        list_for_each_entry(node, &c->list_sub_handle, linked_list, iotx_mc_topic_handle_t) {
            /* If subscribe the same topic and callback function, then ignore */
#if defined(INSPECT_MQTT_FLOW) && defined (INFRA_LOG)
#if WITH_MQTT_ZIP_TOPIC
            HEXDUMP_DEBUG(node->topic_filter, MQTT_ZIP_PATH_DEFAULT_LEN);
#else
            mqtt_warning("node->topic: %s", node->topic_filter);
#endif
#endif
            if (0 == iotx_mc_check_handle_is_identical(node, handler)) {
                mqtt_warning("dup sub,topic = %s", topicFilter);
                dup = 1;
            }
        }
#else
        for (idx = 0; idx < IOTX_MC_SUBHANDLE_LIST_MAX_LEN; idx++) {
            /* If subscribe the same topic and callback function, then ignore */
            if (&c->list_sub_handle[idx] != handler &&
                0 == iotx_mc_check_handle_is_identical(&c->list_sub_handle[idx], handler)) {
                mqtt_warning("dup sub,topic = %s", topicFilter);
                dup = 1;
            }
        }
#endif
        if (dup == 0) {
#ifdef PLATFORM_HAS_DYNMEM
            list_add_tail(&handler->linked_list, &c->list_sub_handle);
#endif
        } else {
#ifdef PLATFORM_HAS_DYNMEM
            mqtt_free(handler->topic_filter);
            mqtt_free(handler);
#else
            memset(handler, 0, sizeof(iotx_mc_topic_handle_t));
#endif
        }
        HAL_MutexUnlock(c->lock_generic);
        return STATE_SUCCESS;
    }

    HAL_MutexLock(c->lock_write_buf);
    res = _alloc_send_buffer(c, strlen(topicFilter));
    if (res < STATE_SUCCESS) {
        HAL_MutexUnlock(c->lock_write_buf);
#ifdef PLATFORM_HAS_DYNMEM
        mqtt_free(handler->topic_filter);
        mqtt_free(handler);
#else
        memset(handler, 0, sizeof(iotx_mc_topic_handle_t));
#endif
        return res;
    }

    len = MQTTSerialize_subscribe((unsigned char *)c->buf_send, c->buf_size_send, 0, (unsigned short)msgId, 1, &topic,
                                  (int *)&qos);
    if (len <= 0) {
#ifdef PLATFORM_HAS_DYNMEM
        mqtt_free(handler->topic_filter);
        mqtt_free(handler);
#else
        memset(handler, 0, sizeof(iotx_mc_topic_handle_t));
#endif
        _reset_send_buffer(c);
        HAL_MutexUnlock(c->lock_write_buf);
        return STATE_MQTT_SERIALIZE_SUB_ERROR;
    }

    mqtt_debug("%20s : %08d", "Packet Ident", msgId);
    mqtt_debug("%20s : %s", "Topic", topicFilter);
    mqtt_debug("%20s : %d", "QoS", (int)qos);
    mqtt_debug("%20s : %d", "Packet Length", len);
#if defined(INSPECT_MQTT_FLOW) && defined (INFRA_LOG)
    HEXDUMP_DEBUG(c->buf_send, len);
#endif

    if ((iotx_mc_send_packet(c, c->buf_send, len, &timer)) != STATE_SUCCESS) { /* send the subscribe packet */
        /* If send failed, remove it */
        mqtt_err("run sendPacket error!");
#ifdef PLATFORM_HAS_DYNMEM
        mqtt_free(handler->topic_filter);
        mqtt_free(handler);
#else
        memset(handler, 0, sizeof(iotx_mc_topic_handle_t));
#endif
        _reset_send_buffer(c);
        HAL_MutexUnlock(c->lock_write_buf);
        return STATE_SYS_DEPEND_NWK_CLOSE;
    }
    _reset_send_buffer(c);
    HAL_MutexUnlock(c->lock_write_buf);

    {
        uint8_t dup = 0;
#ifdef PLATFORM_HAS_DYNMEM
        iotx_mc_topic_handle_t *node;
#endif
        HAL_MutexLock(c->lock_generic);
#ifdef PLATFORM_HAS_DYNMEM
#if defined(INSPECT_MQTT_FLOW) && defined (INFRA_LOG)
#if WITH_MQTT_ZIP_TOPIC
        HEXDUMP_DEBUG(handler->topic_filter, MQTT_ZIP_PATH_DEFAULT_LEN);
#else
        mqtt_warning("handler->topic: %s", handler->topic_filter);
#endif
#endif
        list_for_each_entry(node, &c->list_sub_handle, linked_list, iotx_mc_topic_handle_t) {
            /* If subscribe the same topic and callback function, then ignore */
#if defined(INSPECT_MQTT_FLOW) && defined (INFRA_LOG)
#if WITH_MQTT_ZIP_TOPIC
            HEXDUMP_DEBUG(node->topic_filter, MQTT_ZIP_PATH_DEFAULT_LEN);
#else
            mqtt_warning("node->topic: %s", node->topic_filter);
#endif
#endif
            if (0 == iotx_mc_check_handle_is_identical(node, handler)) {
                mqtt_warning("dup sub,topic = %s", topicFilter);
                dup = 1;
            }
        }
#else
        for (idx = 0; idx < IOTX_MC_SUBHANDLE_LIST_MAX_LEN; idx++) {
            /* If subscribe the same topic and callback function, then ignore */
            if (&c->list_sub_handle[idx] != handler &&
                0 == iotx_mc_check_handle_is_identical(&c->list_sub_handle[idx], handler)) {
                mqtt_warning("dup sub,topic = %s", topicFilter);
                dup = 1;
            }
        }
#endif
        if (dup == 0) {
#ifdef PLATFORM_HAS_DYNMEM
            list_add_tail(&handler->linked_list, &c->list_sub_handle);
#endif
        } else {
#ifdef PLATFORM_HAS_DYNMEM
            mqtt_free(handler->topic_filter);
            mqtt_free(handler);
#else
            memset(handler, 0, sizeof(iotx_mc_topic_handle_t));
#endif
        }
        HAL_MutexUnlock(c->lock_generic);
    }

    return STATE_SUCCESS;
}

static int iotx_mc_get_next_packetid(iotx_mc_client_t *c)
{
    unsigned int id = 0;

    if (!c) {
        return STATE_USER_INPUT_INVALID;
    }

    HAL_MutexLock(c->lock_generic);
    c->packet_id = (c->packet_id == IOTX_MC_PACKET_ID_MAX) ? 1 : c->packet_id + 1;
    id = c->packet_id;
    HAL_MutexUnlock(c->lock_generic);

    return id;
}

static int iotx_mc_check_rule(char *iterm, iotx_mc_topic_type_t type)
{
    int i = 0;
    int len = 0;

    if (NULL == iterm) {
        mqtt_err("iterm is NULL");
        return STATE_USER_INPUT_INVALID;
    }

    len = strlen(iterm);

    for (i = 0; i < len; i++) {
        if (TOPIC_FILTER_TYPE == type) {
            if ('+' == iterm[i] || '#' == iterm[i]) {
                if (1 != len) {
                    mqtt_err("the character # and + is error");
                    return STATE_MQTT_UNEXP_TOPIC_FORMAT;
                }
            }
        } else {
            if ('+' == iterm[i] || '#' == iterm[i]) {
                mqtt_err("has character # and + is error");
                return STATE_MQTT_UNEXP_TOPIC_FORMAT;
            }
        }

        if (iterm[i] < 32 || iterm[i] >= 127) {
            return STATE_MQTT_UNEXP_TOPIC_FORMAT;
        }
    }
    return STATE_SUCCESS;
}

static int iotx_mc_check_topic(const char *topicName, iotx_mc_topic_type_t type)
{
    int mask = 0, res = 0;
    char *delim = "/";
    char *iterm = NULL;
    char topicString[CONFIG_MQTT_TOPIC_MAXLEN];
    if (NULL == topicName /*|| '/' != topicName[0]*/) {
        return STATE_MQTT_UNEXP_TOPIC_FORMAT;
    }

    if (strlen(topicName) > CONFIG_MQTT_TOPIC_MAXLEN) {
        mqtt_err("len of topicName exceeds %d", CONFIG_MQTT_TOPIC_MAXLEN);
        return STATE_MQTT_TOPIC_BUF_TOO_SHORT;
    }

    memset(topicString, 0x0, CONFIG_MQTT_TOPIC_MAXLEN);
    strncpy(topicString, topicName, CONFIG_MQTT_TOPIC_MAXLEN - 1);

    iterm = infra_strtok(topicString, delim);

    res = iotx_mc_check_rule(iterm, type);
    if (res < STATE_SUCCESS) {
        mqtt_err("run iotx_check_rule error");
        return res;
    }

    for (;;) {
        iterm = infra_strtok(NULL, delim);

        if (iterm == NULL) {
            break;
        }

        /* The character '#' is not in the last */
        if (1 == mask) {
            mqtt_err("the character # is error");
            return STATE_MQTT_UNEXP_TOPIC_FORMAT;
        }

        res = iotx_mc_check_rule(iterm, type);
        if (res < STATE_SUCCESS) {
            mqtt_err("run iotx_check_rule error");
            return res;
        }

        if (iterm[0] == '#') {
            mask = 1;
        }
    }

    return STATE_SUCCESS;
}

static inline int _is_in_yield_cb()
{
    return _in_yield_cb;
}

static int MQTTUnsubscribe(iotx_mc_client_t *c, const char *topicFilter, unsigned int msgId)
{
    MQTTString cur_topic;
    iotx_time_t timer;
    MQTTString topic = MQTTString_initializer;
    int len = 0, res = 0;
    /*iotx_mc_topic_handle_t handler = {topicFilter, {NULL, NULL}};*/
    iotx_mc_topic_handle_t *handler = NULL;
#ifdef PLATFORM_HAS_DYNMEM
    iotx_mc_topic_handle_t *node = NULL;
    iotx_mc_topic_handle_t *next = NULL;
#else
    int idx = 0;
    iotx_mc_topic_handle_t s_handler;
#endif
    if (!c || !topicFilter) {
        return STATE_USER_INPUT_INVALID;
    }

    topic.cstring = (char *)topicFilter;
    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

#ifdef PLATFORM_HAS_DYNMEM
    handler = mqtt_malloc(sizeof(iotx_mc_topic_handle_t));
    if (NULL == handler) {
        return STATE_SYS_DEPEND_MALLOC;
    }
#else
    handler = &s_handler;
#endif

    memset(handler, 0, sizeof(iotx_mc_topic_handle_t));

#if !(WITH_MQTT_ZIP_TOPIC)
#ifdef PLATFORM_HAS_DYNMEM
    handler->topic_filter = mqtt_malloc(strlen(topicFilter) + 1);
    if (NULL == handler->topic_filter) {
        mqtt_free(handler);
        return STATE_SYS_DEPEND_MALLOC;
    }
    memset((char *)handler->topic_filter, 0, strlen(topicFilter) + 1);
#else
    if (strlen(topicFilter) >= CONFIG_MQTT_TOPIC_MAXLEN) {
        return MQTT_TOPIC_LEN_TOO_SHORT;
    }
    memset((char *)handler->topic_filter, 0, CONFIG_MQTT_TOPIC_MAXLEN);
#endif
    memcpy((char *)handler->topic_filter, topicFilter, strlen(topicFilter) + 1);
#else
    if (strstr(topicFilter, "/+") != NULL || strstr(topicFilter, "/#") != NULL) {
#ifdef PLATFORM_HAS_DYNMEM
        handler->topic_filter = mqtt_malloc(strlen(topicFilter) + 1);
        if (NULL == handler->topic_filter) {
            mqtt_free(handler);
            return STATE_SYS_DEPEND_MALLOC;
        }
        memset((char *)handler->topic_filter, 0, strlen(topicFilter) + 1);
#else
        if (strlen(topicFilter) >= CONFIG_MQTT_TOPIC_MAXLEN) {
            return MQTT_TOPIC_LEN_TOO_SHORT;
        }
        memset((char *)handler->topic_filter, 0, CONFIG_MQTT_TOPIC_MAXLEN);
#endif
        handler->topic_type = TOPIC_FILTER_TYPE;
        memcpy((char *)handler->topic_filter, topicFilter, strlen(topicFilter) + 1);
    } else {
#ifdef PLATFORM_HAS_DYNMEM
        handler->topic_filter = mqtt_malloc(MQTT_ZIP_PATH_DEFAULT_LEN);
        if (NULL == handler->topic_filter) {
            mqtt_free(handler);
            return STATE_SYS_DEPEND_MALLOC;
        }
        memset((char *)handler->topic_filter, 0, MQTT_ZIP_PATH_DEFAULT_LEN);
#else
        if (MQTT_ZIP_PATH_DEFAULT_LEN >= CONFIG_MQTT_TOPIC_MAXLEN) {
            return MQTT_TOPIC_LEN_TOO_SHORT;
        }
        memset((char *)handler->topic_filter, 0, CONFIG_MQTT_TOPIC_MAXLEN);
#endif
        handler->topic_type = TOPIC_NAME_TYPE;
        res = iotx_mc_get_zip_topic(topicFilter, strlen(topicFilter), (char *)handler->topic_filter, MQTT_ZIP_PATH_DEFAULT_LEN);
        if (res < STATE_SUCCESS) {
#ifdef PLATFORM_HAS_DYNMEM
            mqtt_free(handler->topic_filter);
            mqtt_free(handler);
#endif
            return res;
        }
    }
#endif

    HAL_MutexLock(c->lock_write_buf);
    res = _alloc_send_buffer(c, strlen(topicFilter));
    if (res < STATE_SUCCESS) {
        HAL_MutexUnlock(c->lock_write_buf);
#ifdef PLATFORM_HAS_DYNMEM
        mqtt_free(handler->topic_filter);
        mqtt_free(handler);
#endif
        return res;
    }

    if ((len = MQTTSerialize_unsubscribe((unsigned char *)c->buf_send, c->buf_size_send, 0, (unsigned short)msgId, 1,
                                         &topic)) <= 0) {
#ifdef PLATFORM_HAS_DYNMEM
        mqtt_free(handler->topic_filter);
        mqtt_free(handler);
#endif
        _reset_send_buffer(c);
        HAL_MutexUnlock(c->lock_write_buf);
        return STATE_MQTT_SERIALIZE_UNSUB_ERROR;
    }

    if ((iotx_mc_send_packet(c, c->buf_send, len, &timer)) != STATE_SUCCESS) { /* send the subscribe packet */
#ifdef PLATFORM_HAS_DYNMEM
        mqtt_free(handler->topic_filter);
        mqtt_free(handler);
#endif
        _reset_send_buffer(c);
        HAL_MutexUnlock(c->lock_write_buf);
        return STATE_SYS_DEPEND_NWK_CLOSE;
    }

    cur_topic.cstring = NULL;
    cur_topic.lenstring.data = (char *)handler->topic_filter;

#if !(WITH_MQTT_ZIP_TOPIC)
    cur_topic.lenstring.len = strlen(handler->topic_filter);
#else
    if (handler->topic_type == TOPIC_FILTER_TYPE) {
        cur_topic.lenstring.len = strlen(handler->topic_filter) + 1;
    } else {
        cur_topic.lenstring.len = MQTT_ZIP_PATH_DEFAULT_LEN;
    }
#endif
    /* we have to find the right message handler - indexed by topic */
    HAL_MutexLock(c->lock_generic);
#ifdef PLATFORM_HAS_DYNMEM
    list_for_each_entry_safe(node, next, &c->list_sub_handle, linked_list, iotx_mc_topic_handle_t) {
        if (MQTTPacket_equals(&cur_topic, (char *)node->topic_filter)
            || iotx_mc_is_topic_matched((char *)node->topic_filter, &cur_topic)) {
            mqtt_debug("topic be matched");
            list_del(&node->linked_list);
            mqtt_free(node->topic_filter);
            mqtt_free(node);
        }
    }
    mqtt_free(handler->topic_filter);
    mqtt_free(handler);
#else
    for (idx = 0; idx < IOTX_MC_SUBHANDLE_LIST_MAX_LEN; idx++) {
        if ((c->list_sub_handle[idx].used == 1) &&
            (MQTTPacket_equals(&cur_topic, (char *)c->list_sub_handle[idx].topic_filter) ||
             iotx_mc_is_topic_matched((char *)c->list_sub_handle[idx].topic_filter, &cur_topic))) {
            mqtt_debug("topic be matched");
            memset(&c->list_sub_handle[idx], 0, sizeof(iotx_mc_topic_handle_t));
        }
    }
#endif
    HAL_MutexUnlock(c->lock_generic);
    _reset_send_buffer(c);
    HAL_MutexUnlock(c->lock_write_buf);
    return STATE_SUCCESS;
}

int MQTTPublish(iotx_mc_client_t *c, const char *topicName, iotx_mqtt_topic_info_pt topic_msg)

{
    int res = 0;
    iotx_time_t         timer;
    MQTTString          topic = MQTTString_initializer;
    int                 len = 0;
#if !WITH_MQTT_ONLY_QOS0
    iotx_mc_pub_info_t  *node = NULL;
#endif
#ifdef INFRA_LOG_NETWORK_PAYLOAD
    const char     *json_payload = NULL;
#endif

    if (!c || !topicName || !topic_msg) {
        return STATE_USER_INPUT_INVALID;
    }

    topic.cstring = (char *)topicName;
    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

    HAL_MutexLock(c->lock_list_pub);
    HAL_MutexLock(c->lock_write_buf);

    res = _alloc_send_buffer(c, strlen(topicName) + topic_msg->payload_len);
    if (res < STATE_SUCCESS) {
        HAL_MutexUnlock(c->lock_write_buf);
        HAL_MutexUnlock(c->lock_list_pub);
        return res;
    }

    len = MQTTSerialize_publish((unsigned char *)c->buf_send,
                                c->buf_size_send,
                                0,
                                topic_msg->qos,
                                topic_msg->retain,
                                topic_msg->packet_id,
                                topic,
                                (unsigned char *)topic_msg->payload,
                                topic_msg->payload_len);
    if (len <= 0) {
        mqtt_err("MQTTSerialize_publish is error, len=%d, buf_size_send=%u, payloadlen=%u",
                 len,
                 c->buf_size_send,
                 topic_msg->payload_len);
        _reset_send_buffer(c);
        HAL_MutexUnlock(c->lock_write_buf);
        HAL_MutexUnlock(c->lock_list_pub);
        return STATE_MQTT_SERIALIZE_PUB_ERROR;
    }

#if !WITH_MQTT_ONLY_QOS0
    node = NULL;
    /* If the QOS >1, push the information into list of wait publish ACK */
    if (topic_msg->qos > IOTX_MQTT_QOS0) {
        /* push into list */
        res = iotx_mc_push_pubInfo_to(c, len, topic_msg->packet_id, &node);
        if (res < STATE_SUCCESS) {
            mqtt_err("push publish into to pubInfolist failed!");
            _reset_send_buffer(c);
            HAL_MutexUnlock(c->lock_write_buf);
            HAL_MutexUnlock(c->lock_list_pub);
            return res;
        }
    }
#endif
    /* send the publish packet */
    if (iotx_mc_send_packet(c, c->buf_send, len, &timer) != STATE_SUCCESS) {
#if !WITH_MQTT_ONLY_QOS0
        if (topic_msg->qos > IOTX_MQTT_QOS0) {
            /* If not even successfully sent to IP stack, meaningless to wait QOS1 ack, give up waiting */
#ifdef PLATFORM_HAS_DYNMEM
            list_del(&node->linked_list);
            mqtt_free(node);
#else
            memset(node, 0, sizeof(iotx_mc_pub_info_t));
#endif
        }
#endif
        _reset_send_buffer(c);
        HAL_MutexUnlock(c->lock_write_buf);
        HAL_MutexUnlock(c->lock_list_pub);
        return STATE_SYS_DEPEND_NWK_CLOSE;
    }

#ifdef INFRA_LOG_NETWORK_PAYLOAD
    json_payload = (const char *)topic_msg->payload;

    mqtt_info("Upstream Topic: '%s'", topicName);
    mqtt_info("Upstream Payload:");
    iotx_facility_json_print(json_payload, LOG_INFO_LEVEL, '>');

#endif  /* #ifdef INFRA_LOG */

    _reset_send_buffer(c);
    HAL_MutexUnlock(c->lock_write_buf);
    HAL_MutexUnlock(c->lock_list_pub);

    return STATE_SUCCESS;
}

static int MQTTDisconnect(iotx_mc_client_t *c)
{
    int             rc = STATE_SUCCESS;
    int             len = 0, res = 0;
    iotx_time_t     timer;     /* we might wait for incomplete incoming publishes to complete */

    if (!c) {
        return STATE_USER_INPUT_INVALID;
    }

    HAL_MutexLock(c->lock_write_buf);

    res = _alloc_send_buffer(c, 0);
    if (res < STATE_SUCCESS) {
        HAL_MutexUnlock(c->lock_write_buf);
        return res;
    }

    len = MQTTSerialize_disconnect((unsigned char *)c->buf_send, c->buf_size_send);

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

    if (len > 0) {
        rc = iotx_mc_send_packet(c, c->buf_send, len, &timer);           /* send the disconnect packet */
    }
    _reset_send_buffer(c);
    HAL_MutexUnlock(c->lock_write_buf);
    return rc;
}

static int iotx_mc_disconnect(iotx_mc_client_t *pClient)
{
    int             rc = -1;

    if (NULL == pClient) {
        return STATE_USER_INPUT_INVALID;
    }

    if (wrapper_mqtt_check_state(pClient)) {
        rc = MQTTDisconnect(pClient);
        mqtt_debug("rc = MQTTDisconnect() = %d", rc);
        rc = rc;
    }

    /* close tcp/ip socket or free tls resources */
    pClient->ipstack.disconnect(&pClient->ipstack);

    iotx_mc_set_client_state(pClient, IOTX_MC_STATE_INITIALIZED);

    mqtt_info("mqtt disconnect!");
    return STATE_SUCCESS;
}

/************************  Public Interface ************************/
void *wrapper_mqtt_init_internal(iotx_mqtt_param_t *mqtt_params)
{
    int err;
    iotx_mc_client_t *pclient = NULL;
#ifndef PLATFORM_HAS_DYNMEM
    int idx;
#endif

#ifdef PLATFORM_HAS_DYNMEM
    pclient = (iotx_mc_client_t *)mqtt_malloc(sizeof(iotx_mc_client_t));
    if (NULL == pclient) {
        iotx_state_event(ITE_STATE_MQTT_COMM, STATE_SYS_DEPEND_MALLOC, "");
        return NULL;
    }
    memset(pclient, 0, sizeof(iotx_mc_client_t));
#else
    for (idx = 0; idx < IOTX_MC_CLIENT_MAX_COUNT; idx++) {
        if (g_iotx_mc_client[idx].used == 0) {
            g_iotx_mc_client[idx].used = 1;
            pclient = &g_iotx_mc_client[idx];
            break;
        }
    }

    if (NULL == pclient) {
        iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_CLI_EXCEED_MAX, "");
        return NULL;
    }
#endif

    err = iotx_mc_init_internal(pclient, mqtt_params);

    if (err < STATE_SUCCESS) {
        iotx_state_event(ITE_STATE_MQTT_COMM, err, "iotx_mc_init fail");
        iotx_mc_release(pclient);
        return NULL;
    }

    return pclient;
}

void *wrapper_mqtt_init(iotx_mqtt_param_t *mqtt_params)
{
    int err;
    iotx_mc_client_t *pclient = NULL;
#ifndef PLATFORM_HAS_DYNMEM
    int idx;
#endif

#ifdef PLATFORM_HAS_DYNMEM
    pclient = (iotx_mc_client_t *)mqtt_malloc(sizeof(iotx_mc_client_t));
    if (NULL == pclient) {
        iotx_state_event(ITE_STATE_MQTT_COMM, STATE_SYS_DEPEND_MALLOC, "");
        return NULL;
    }
    memset(pclient, 0, sizeof(iotx_mc_client_t));
#else
    for (idx = 0; idx < IOTX_MC_CLIENT_MAX_COUNT; idx++) {
        if (g_iotx_mc_client[idx].used == 0) {
            g_iotx_mc_client[idx].used = 1;
            pclient = &g_iotx_mc_client[idx];
            break;
        }
    }

    if (NULL == pclient) {
        iotx_state_event(ITE_STATE_MQTT_COMM, STATE_MQTT_CLI_EXCEED_MAX, "");
        return NULL;
    }
#endif

    err = iotx_mc_init(pclient, mqtt_params);

    if (err < STATE_SUCCESS) {
        iotx_state_event(ITE_STATE_MQTT_COMM, err, "iotx_mc_init fail");
        iotx_mc_release(pclient);
        return NULL;
    }

    return pclient;
}

int _mqtt_nwk_connect(void *client)
{
    int rc = STATE_SUCCESS;
    int retry_max = 3;
    int retry_cnt = 1;
    int retry_interval = 1000;

    iotx_mc_client_t *pClient = (iotx_mc_client_t *)client;

    /* Establish TCP or TLS connection */
    do {
        mqtt_debug("calling TCP or TLS connect HAL for [%d/%d] iteration", retry_cnt, retry_max);

        rc = pClient->ipstack.connect(&pClient->ipstack);
        if (STATE_SUCCESS != rc) {
            pClient->ipstack.disconnect(&pClient->ipstack);
            mqtt_err("TCP or TLS Connection failed");

            if (ERROR_CERTIFICATE_EXPIRED == rc) {
                mqtt_err("certificate is expired! rc = %d", rc);
                rc = ERROR_CERT_VERIFY_FAIL;
                HAL_SleepMs(retry_interval);
                continue;
            } else {
                rc = STATE_SYS_DEPEND_NWK_CLOSE;
                HAL_SleepMs(retry_interval);
                continue;
            }
        } else {
            mqtt_debug("rc = pClient->ipstack.connect() = %d, success @ [%d/%d] iteration", rc, retry_cnt, retry_max);
            break;
        }
    } while (++retry_cnt <= retry_max);

    return rc;
}

int wrapper_mqtt_connect(void *client)
{
    int rc = STATE_SUCCESS;
#ifndef ASYNC_PROTOCOL_STACK
    int try_count = 0;
    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
#endif

    iotx_mc_client_t *pClient = (iotx_mc_client_t *)client;

    if (NULL == pClient) {
        return STATE_USER_INPUT_INVALID;
    }

    rc = _mqtt_nwk_connect(client);
    if (rc != STATE_SUCCESS) {
        return rc;
    }

#ifdef ASYNC_PROTOCOL_STACK
    iotx_mc_set_client_state(pClient, IOTX_MC_STATE_CONNECT_BLOCK);
    rc = STATE_MQTT_ASYNC_STACK_CONN_IN_PROG;
#else
    rc = _mqtt_connect(pClient);

    // do {
    //     HAL_MutexLock(pClient->lock_yield);
    //     pClient->cycle_timeout_ms = 500;
    //     HAL_MutexUnlock(pClient->lock_yield);
    //     _mqtt_cycle(client);

    //     IOT_Ioctl(IOTX_IOCTL_GET_PRODUCT_KEY, product_key);
    //     IOT_Ioctl(IOTX_IOCTL_GET_DEVICE_NAME, device_name);

    //     if (strlen(product_key) != 0 && strlen(device_name) != 0) {
    //         break;
    //     }
    // } while (++try_count < RETRY_TIME_LIMIT);
#endif
    return rc;
}

int wrapper_mqtt_release(void **c)
{
    iotx_mc_client_t *pClient;
#ifdef PLATFORM_HAS_DYNMEM
    iotx_mc_topic_handle_t *node = NULL, *next = NULL;
#endif
    if (NULL == c) {
        return STATE_USER_INPUT_INVALID;
    }

    pClient = (iotx_mc_client_t *)*c;
    if (NULL == pClient) {
        return STATE_USER_INPUT_INVALID;
    }
    /* iotx_delete_thread(pClient); */
    HAL_SleepMs(100);

    iotx_mc_disconnect(pClient);
    iotx_mc_set_client_state(pClient, IOTX_MC_STATE_INVALID);
    HAL_SleepMs(100);

#ifdef PLATFORM_HAS_DYNMEM
    list_for_each_entry_safe(node, next, &pClient->list_sub_handle, linked_list, iotx_mc_topic_handle_t) {
        list_del(&node->linked_list);
        mqtt_free(node->topic_filter);
        mqtt_free(node);
    }
#else
    memset(pClient->list_sub_handle, 0, sizeof(iotx_mc_topic_handle_t) * IOTX_MC_SUBHANDLE_LIST_MAX_LEN);
#endif
    HAL_MutexDestroy(pClient->lock_generic);
    HAL_MutexDestroy(pClient->lock_list_pub);
    HAL_MutexDestroy(pClient->lock_write_buf);
    HAL_MutexDestroy(pClient->lock_yield);
    HAL_MutexDestroy(pClient->lock_read_buf);

#if !WITH_MQTT_ONLY_QOS0
    iotx_mc_pub_wait_list_deinit(pClient);
#endif
#ifdef PLATFORM_HAS_DYNMEM
    if (pClient->buf_send != NULL) {
        mqtt_free(pClient->buf_send);
        pClient->buf_send = NULL;
    }
    if (pClient->buf_read != NULL) {
        mqtt_free(pClient->buf_read);
        pClient->buf_read = NULL;
    }
    mqtt_free(pClient);
#else
    memset(pClient, 0, sizeof(iotx_mc_client_t));
#endif
    *c = NULL;
    mqtt_info("mqtt release!");
    return STATE_SUCCESS;
}

int wrapper_mqtt_yield(void *client, int timeout_ms)
{
    iotx_mc_client_t *pClient = (iotx_mc_client_t *)client;

    if (pClient == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    if (timeout_ms < 0) {
        mqtt_err("Invalid argument, timeout_ms = %d", timeout_ms);
        return STATE_USER_INPUT_INVALID;
    }
    if (timeout_ms == 0) {
        timeout_ms = 10;
    }

    HAL_MutexLock(pClient->lock_yield);
    pClient->cycle_timeout_ms = timeout_ms;
    /* Keep MQTT alive or reconnect if connection abort */
    iotx_mc_keepalive(pClient);
    HAL_MutexUnlock(pClient->lock_yield);

#ifndef ASYNC_PROTOCOL_STACK
    _mqtt_cycle(client);
#else
    if (pClient->client_state == IOTX_MC_STATE_CONNECTED) {
#if !WITH_MQTT_ONLY_QOS0
        /* check list of wait publish ACK to remove node that is ACKED or timeout */
        MQTTPubInfoProc(pClient);
#endif
    }
    HAL_SleepMs(timeout_ms);
#endif

    return STATE_SUCCESS;
}


/* check MQTT client is in normal state */
/* 0, in abnormal state; 1, in normal state */
int wrapper_mqtt_check_state(void *client)
{
    if (!client) {
        return 0;
    }

    if (iotx_mc_get_client_state((iotx_mc_client_t *)client) == IOTX_MC_STATE_CONNECTED) {
        return 1;
    }

    return 0;
}

int wrapper_mqtt_subscribe(void *client,
                           const char *topicFilter,
                           iotx_mqtt_qos_t qos,
                           iotx_mqtt_event_handle_func_fpt topic_handle_func,
                           void *pcontext)
{
    int rc = STATE_SUCCESS;
    unsigned int msgId;
    iotx_mc_client_t *c;

    if (NULL == client || NULL == topicFilter || strlen(topicFilter) == 0 || !topic_handle_func) {
        mqtt_err(" paras error");
        return STATE_USER_INPUT_INVALID;
    }

    c = (iotx_mc_client_t *)client;

    msgId = iotx_mc_get_next_packetid(c);

    if (!wrapper_mqtt_check_state(c)) {
        mqtt_err("mqtt client state is error,state = %d", iotx_mc_get_client_state(c));
        return STATE_MQTT_IN_OFFLINE_STATUS;
    }

    rc = iotx_mc_check_topic(topicFilter, TOPIC_FILTER_TYPE);
    if (rc < STATE_SUCCESS) {
        mqtt_err("topic format is error,topicFilter = %s", topicFilter);
        return rc;
    }

    mqtt_debug("PERFORM subscribe to '%s' (msgId=%d)", topicFilter, msgId);
    rc = MQTTSubscribe(c, topicFilter, qos, msgId, topic_handle_func, pcontext);
    if (rc < STATE_SUCCESS) {
        if (rc == STATE_SYS_DEPEND_NWK_CLOSE) {
            iotx_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        }

        mqtt_err("run MQTTSubscribe error, rc = %d", rc);
        return rc;
    }

    mqtt_info("mqtt subscribe packet sent,topic = %s!", topicFilter);
    return msgId;
}

int wrapper_mqtt_subscribe_sync(void *c,
                                const char *topic_filter,
                                iotx_mqtt_qos_t qos,
                                iotx_mqtt_event_handle_func_fpt topic_handle_func,
                                void *pcontext,
                                int timeout_ms)
{
    int             subed;
    int             ret;
    iotx_time_t     timer;
    iotx_mc_client_t *client = (iotx_mc_client_t *)c;
    int cnt = 0;
    mqtt_sub_sync_node_t *node = NULL;
#ifdef PLATFORM_HAS_DYNMEM
    mqtt_sub_sync_node_t *next = NULL;
#else
    int idx = 0;
#endif
    if (client == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    if (qos > IOTX_MQTT_QOS3_SUB_LOCAL) {
        mqtt_warning("Invalid qos(%d) out of [%d, %d], using %d",
                     qos,
                     IOTX_MQTT_QOS0, IOTX_MQTT_QOS3_SUB_LOCAL, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, timeout_ms);

    ret = -1;
    subed = 0;
    cnt = 0;
    cnt = cnt;
    do {
#ifdef PLATFORM_HAS_DYNMEM
        mqtt_sub_sync_node_t *node = NULL;
        mqtt_sub_sync_node_t *next = NULL;
#else
        int idx = 0;
#endif
        if (ret < 0) {
            ret = wrapper_mqtt_subscribe(client, topic_filter, qos, topic_handle_func, pcontext);
            if (_is_in_yield_cb() != 0 || qos == IOTX_MQTT_QOS3_SUB_LOCAL) {
                return ret;
            }
        }

        if (!subed && ret >= 0) {
            mqtt_sub_sync_node_t *node = NULL;
#ifndef PLATFORM_HAS_DYNMEM
            int idx = 0;
#endif
#ifdef PLATFORM_HAS_DYNMEM
            node = (mqtt_sub_sync_node_t *)mqtt_malloc(sizeof(mqtt_sub_sync_node_t));
#else
            for (idx = 0; idx < IOTX_MC_SUBSYNC_LIST_MAX_LEN; idx++) {
                if (client->list_sub_sync_ack[idx].used == 0) {
                    memset(&client->list_sub_sync_ack[idx], 0, sizeof(mqtt_sub_sync_node_t));
                    client->list_sub_sync_ack[idx].used = 1;
                    node = &client->list_sub_sync_ack[idx];
                    break;
                }
            }
#endif
            if (node != NULL) {
                mqtt_debug("packet_id = %d", ret);
                node->packet_id = ret;
                node->ack_type = IOTX_MQTT_EVENT_UNDEF;
#ifdef PLATFORM_HAS_DYNMEM
                HAL_MutexLock(client->lock_generic);
                list_add_tail(&node->linked_list, &client->list_sub_sync_ack);
                HAL_MutexUnlock(client->lock_generic);
#endif
                subed = 1;
            }

        }
        wrapper_mqtt_yield(client, 100);

        HAL_MutexLock(client->lock_generic);
#ifdef PLATFORM_HAS_DYNMEM
        list_for_each_entry_safe(node, next, &client->list_sub_sync_ack, linked_list, mqtt_sub_sync_node_t) {
            if (node->packet_id == ret) {
                mqtt_debug("node->ack_type=%d cnt=%d", node->ack_type, cnt++);
                if (node->ack_type == IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS) {
                    list_del(&node->linked_list);
                    mqtt_free(node);
                    mqtt_debug("success!!");
                    HAL_MutexUnlock(client->lock_generic);
                    return ret;
                } else if (node->ack_type == IOTX_MQTT_EVENT_SUBCRIBE_NACK) {
                    list_del(&node->linked_list);
                    mqtt_free(node);
                    ret = -1; /* resub */
                    subed = 0;
                } else if (node->ack_type == IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT) {
                    list_del(&node->linked_list);
                    mqtt_free(node);
                    ret = -1; /* resub */
                    subed = 0;
                }
            }
            break;
        }
#else
        for (idx = 0; idx < IOTX_MC_SUBSYNC_LIST_MAX_LEN; idx++) {
            if (client->list_sub_sync_ack[idx].used == 0) {
                continue;
            }

            if (client->list_sub_sync_ack[idx].packet_id == ret) {
                mqtt_debug("client->list_sub_sync_ack[%d].ack_type=%d cnt=%d", idx, client->list_sub_sync_ack[idx].ack_type, cnt++);
                if (client->list_sub_sync_ack[idx].ack_type == IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS) {
                    memset(&client->list_sub_sync_ack[idx], 0, sizeof(mqtt_sub_sync_node_t));
                    mqtt_debug("success!!");
                    HAL_MutexUnlock(client->lock_generic);
                    return ret;
                } else if (client->list_sub_sync_ack[idx].ack_type == IOTX_MQTT_EVENT_SUBCRIBE_NACK) {
                    memset(&client->list_sub_sync_ack[idx], 0, sizeof(mqtt_sub_sync_node_t));
                    ret = -1; /* resub */
                    subed = 0;
                } else if (client->list_sub_sync_ack[idx].ack_type == IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT) {
                    memset(&client->list_sub_sync_ack[idx], 0, sizeof(mqtt_sub_sync_node_t));
                    ret = -1; /* resub */
                    subed = 0;
                }
            }
            break;
        }
#endif
        HAL_MutexUnlock(client->lock_generic);
    } while (!utils_time_is_expired(&timer));
    mqtt_warning("sync subscribe time out!!");

    HAL_MutexLock(client->lock_generic);
#ifdef PLATFORM_HAS_DYNMEM
    list_for_each_entry_safe(node, next, &client->list_sub_sync_ack, linked_list, mqtt_sub_sync_node_t) {
        if (node->packet_id == ret) {
            list_del(&node->linked_list);
            mqtt_free(node);
        }
    }
#else
    for (idx = 0; idx < IOTX_MC_SUBSYNC_LIST_MAX_LEN; idx++) {
        if (client->list_sub_sync_ack[idx].used && node->packet_id == ret) {
            memset(&client->list_sub_sync_ack[idx], 0, sizeof(mqtt_sub_sync_node_t));
        }
    }
#endif
    HAL_MutexUnlock(client->lock_generic);

    return STATE_MQTT_SYNC_SUB_TIMEOUT;
}

int wrapper_mqtt_unsubscribe(void *client, const char *topicFilter)
{
    int rc = STATE_SUCCESS;
    iotx_mc_client_t *c = (iotx_mc_client_t *)client;
    unsigned int msgId;

    if (NULL == c || NULL == topicFilter) {
        return STATE_USER_INPUT_INVALID;
    }
    msgId = iotx_mc_get_next_packetid(c);

    rc = iotx_mc_check_topic(topicFilter, TOPIC_FILTER_TYPE);
    if (rc < STATE_SUCCESS) {
        mqtt_err("topic format is error,topicFilter = %s", topicFilter);
        return rc;
    }

    if (!wrapper_mqtt_check_state(c)) {
        mqtt_err("mqtt client state is error,state = %d", iotx_mc_get_client_state(c));
        return STATE_MQTT_IN_OFFLINE_STATUS;
    }

    rc = MQTTUnsubscribe(c, topicFilter, msgId);
    if (rc < STATE_SUCCESS) {
        if (rc == STATE_SYS_DEPEND_NWK_CLOSE) { /* send the subscribe packet */
            iotx_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        }

        mqtt_err("run MQTTUnsubscribe error!, rc = %d", rc);
        return rc;
    }

    mqtt_info("mqtt unsubscribe packet sent,topic = %s!", topicFilter);
    return (int)msgId;
}

int wrapper_mqtt_publish(void *client, const char *topicName, iotx_mqtt_topic_info_pt topic_msg)
{
    uint16_t msg_id = 0;
    int rc = STATE_SUCCESS;
    iotx_mc_client_t *c = (iotx_mc_client_t *)client;
    if (c == NULL || topicName == NULL || topic_msg == NULL || topic_msg->payload == NULL) {
        return STATE_USER_INPUT_INVALID;
    }

    rc = iotx_mc_check_topic(topicName, TOPIC_NAME_TYPE);
    if (rc < STATE_SUCCESS) {
        mqtt_err("topic format is error,topicFilter = %s", topicName);
        return rc;
    }

    if (!wrapper_mqtt_check_state(c)) {
        mqtt_err("mqtt client state is error,state = %d", iotx_mc_get_client_state(c));
        return STATE_MQTT_IN_OFFLINE_STATUS;
    }

#if !WITH_MQTT_ONLY_QOS0
    if (topic_msg->qos == IOTX_MQTT_QOS1 || topic_msg->qos == IOTX_MQTT_QOS2) {
        msg_id = iotx_mc_get_next_packetid(c);
        topic_msg->packet_id = msg_id;
    }
    if (topic_msg->qos == IOTX_MQTT_QOS2) {
        mqtt_err("MQTTPublish return error,MQTT_QOS2 is now not supported.");
        return STATE_MQTT_PUB_QOS_INVALID;
    }
#else
    topic_msg->qos = IOTX_MQTT_QOS0;
#endif

#if defined(INSPECT_MQTT_FLOW) && defined(INFRA_LOG)
    HEXDUMP_DEBUG(topicName, strlen(topicName));
    HEXDUMP_DEBUG(topic_msg->payload, topic_msg->payload_len);
#endif

    rc = MQTTPublish(c, topicName, topic_msg);
    if (rc < STATE_SUCCESS) { /* send the subscribe packet */
        if (rc == STATE_SYS_DEPEND_NWK_CLOSE) {
            iotx_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        }
        mqtt_err("MQTTPublish is error, rc = %d", rc);
        return rc;
    }

    return (int)msg_id;
}

#ifdef ASYNC_PROTOCOL_STACK
int wrapper_mqtt_nwk_event_handler(void *client, iotx_mqtt_nwk_event_t event, iotx_mqtt_nwk_param_t *param)
{
    int rc = STATE_SUCCESS;
    iotx_mc_client_t *pClient = (iotx_mc_client_t *)client;
    if (client == NULL || event >= IOTX_MQTT_SOC_MAX) {
        return STATE_USER_INPUT_INVALID;
    }

    switch (event) {
        case IOTX_MQTT_SOC_CONNECTED: {
            rc = _mqtt_connect(pClient);
            if (rc == STATE_SUCCESS) {
                iotx_mc_set_client_state(pClient, IOTX_MC_STATE_CONNECTED);
            }
        }
        break;
        case IOTX_MQTT_SOC_CLOSE: {
            iotx_mc_set_client_state(pClient, IOTX_MC_STATE_DISCONNECTED);
        }
        break;
        case IOTX_MQTT_SOC_READ: {
            HAL_MutexLock(pClient->lock_yield);
            _mqtt_cycle(pClient);
            HAL_MutexUnlock(pClient->lock_yield);
            rc = STATE_SUCCESS;
        }
        break;
        case IOTX_MQTT_SOC_WRITE: {

        }
        break;
        default: {
            mqtt_err("unknown event: %d", event);
            rc = STATE_MQTT_ASYNC_STACK_UNKNOWN_EVENT;
        }
        break;
    }

    return rc;
}
#endif

