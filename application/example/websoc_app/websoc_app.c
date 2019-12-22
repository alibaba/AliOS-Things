/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>

#include "aos/cli.h"
#include "aos/kernel.h"
#include "aos/yloop.h"
#include "ulog/ulog.h"
#include <netmgr.h>

#define TAG "websoc_app"

#include "websoc/librws.h"
#define WEBSOCKET_CONNECTED                (0x01)
#define WEBSOCKET_DISCONNECTED             (0x02)
#define WEBSOCKET_DATA_NOT_RECVED          (0x04)
#define WEBSOCKET_MAX_INSTANCE_NUM         1
#define WEBSOCKET_DISCON_PONG_TIMEOUT_MS   15000

typedef struct websoc_instance
{
    int state_flags; /* the state of the websocket */
    rws_socket socket; /* socket context */
    int id; /* instance array index */
    long long last_recv_pong_ms;
} websoc_instance_t, *websoc_instance_ptr;

/* network state flag */
static bool _network_ready = false;
/* mutex used for websocket instance array operation */
static aos_mutex_t websoc_instance_mutex;
/* websocket instance array */
static websoc_instance_t websoc_instances[WEBSOCKET_MAX_INSTANCE_NUM];

static void websoc_instances_init(void)
{
    memset(websoc_instances, 0, sizeof(websoc_instance_t) * WEBSOCKET_MAX_INSTANCE_NUM);
    aos_mutex_new(&websoc_instance_mutex);
}

static websoc_instance_ptr find_websoc_instance(rws_socket socket)
{
    int i;
    websoc_instance_ptr websoc_ptr = NULL;

    if (!socket)
        return NULL;

    aos_mutex_lock(&websoc_instance_mutex, AOS_WAIT_FOREVER);
    for (i = 0; i < WEBSOCKET_MAX_INSTANCE_NUM; i++) {
        if (websoc_instances[i].socket == socket) {
            websoc_instances[i].id = i;
            websoc_ptr = &websoc_instances[i];
            break;
        }
    }
    aos_mutex_unlock(&websoc_instance_mutex);

    return websoc_ptr;
}

static websoc_instance_ptr create_websoc_instance(void)
{
    int i;
    websoc_instance_ptr websoc_ptr = NULL;

    aos_mutex_lock(&websoc_instance_mutex, AOS_WAIT_FOREVER);
    for (i = 0; i < WEBSOCKET_MAX_INSTANCE_NUM; i++) {
        if (websoc_instances[i].socket == NULL) {
            websoc_ptr = &websoc_instances[i];
            break;
        }
    }
    aos_mutex_unlock(&websoc_instance_mutex);

    return websoc_ptr;
}

static void on_socket_received_text(rws_socket socket, const char * text, const unsigned int length, bool finished)
{
    websoc_instance_ptr instance = find_websoc_instance(socket);
    if (instance == NULL)
        return;
    
    LOGI(TAG, "instance %d recv text, len %d finished %d contenct %s \r\n", instance->id, length, finished, text);
}

static void on_socket_received_bin(rws_socket socket, const void * data, const unsigned int length, bool finished)
{
    websoc_instance_ptr instance = find_websoc_instance(socket);
    if (instance == NULL)
        return;

    LOGI(TAG, "instance %d rev bin, len %d finished %d\r\n", instance->id, length, finished);
}

static void on_socket_connected(rws_socket s)
{
    websoc_instance_ptr instance = NULL;
    const char * websoc_send_text =
    "{\"version\":\"1.0\",\"supportedConnectionTypes\":[\"websocket\"],\"minimumVersion\":\"1.0\",\"channel\":\"/meta/handshake\"}";
    
    instance = find_websoc_instance(s);
    if (instance == NULL)
        return;

    LOGI(TAG, "instance %d Socket connected\r\n", instance->id);

    instance->state_flags |= WEBSOCKET_CONNECTED;
    instance->state_flags &= ~WEBSOCKET_DISCONNECTED;
}

static void on_socket_disconnected(rws_socket socket)
{
    websoc_instance_ptr instance = NULL;

    instance = find_websoc_instance(socket);
    if (instance == NULL)
        return;

    rws_error error = rws_socket_get_error(socket);
    if (error) {
        LOGE(TAG, "intance %d socket disconnect with code, error: %d, %s",
             instance->id,
             rws_error_get_code(error),
             rws_error_get_description(error));
    }

    instance->state_flags &= ~WEBSOCKET_CONNECTED;
    instance->state_flags |= WEBSOCKET_DISCONNECTED;
}

static void on_socket_received_pong(rws_socket socket)
{
    websoc_instance_ptr instance = NULL;

    instance = find_websoc_instance(socket);
    if (instance == NULL)
        return;

    if (!socket)
    {
        LOGE(TAG, "%s: Invalid parameter(s).", __FUNCTION__);
        return;
    }

    LOGI(TAG, "instance %d socket received pong!", instance->id);
    instance->last_recv_pong_ms = aos_now_ms();
}

static void on_socket_send_ping(rws_socket socket)
{
    websoc_instance_ptr instance = NULL;

    instance = find_websoc_instance(socket);
    if (instance == NULL)
        return;

    if (!socket)
    {
        LOGE(TAG, "%s: Invalid parameter(s).", __FUNCTION__);
        return;
    }

    LOGI(TAG, "intance %d socket ping sent!", instance->id);
}

static char *text_arr[] = {"12345566778888999999999999999999999999",
                           "12345566778888999999999999999999999999",
                           "12345566778888999999999999999999999999",
                            NULL};

#define WEBSOC_SCHEME "wss"
#define WEBSOC_SERVER "echo.websocket.org"
#define WEBSOC_PATH "/"
#define WEBSOC_PORT 443
/* certificate is optional for the above server */
const char *WEBSOC_CERT = {
		"-----BEGIN CERTIFICATE-----\r\n"
		"MIIEkjCCA3qgAwIBAgIQCgFBQgAAAVOFc2oLheynCDANBgkqhkiG9w0BAQsFADA/\r\n"
		"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\r\n"
		"DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0Nlow\r\n"
		"SjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxIzAhBgNVBAMT\r\n"
		"GkxldCdzIEVuY3J5cHQgQXV0aG9yaXR5IFgzMIIBIjANBgkqhkiG9w0BAQEFAAOC\r\n"
		"AQ8AMIIBCgKCAQEAnNMM8FrlLke3cl03g7NoYzDq1zUmGSXhvb418XCSL7e4S0EF\r\n"
		"q6meNQhY7LEqxGiHC6PjdeTm86dicbp5gWAf15Gan/PQeGdxyGkOlZHP/uaZ6WA8\r\n"
		"SMx+yk13EiSdRxta67nsHjcAHJyse6cF6s5K671B5TaYucv9bTyWaN8jKkKQDIZ0\r\n"
		"Z8h/pZq4UmEUEz9l6YKHy9v6Dlb2honzhT+Xhq+w3Brvaw2VFn3EK6BlspkENnWA\r\n"
		"a6xK8xuQSXgvopZPKiAlKQTGdMDQMc2PMTiVFrqoM7hD8bEfwzB/onkxEz0tNvjj\r\n"
		"/PIzark5McWvxI0NHWQWM6r6hCm21AvA2H3DkwIDAQABo4IBfTCCAXkwEgYDVR0T\r\n"
		"AQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAYYwfwYIKwYBBQUHAQEEczBxMDIG\r\n"
		"CCsGAQUFBzABhiZodHRwOi8vaXNyZy50cnVzdGlkLm9jc3AuaWRlbnRydXN0LmNv\r\n"
		"bTA7BggrBgEFBQcwAoYvaHR0cDovL2FwcHMuaWRlbnRydXN0LmNvbS9yb290cy9k\r\n"
		"c3Ryb290Y2F4My5wN2MwHwYDVR0jBBgwFoAUxKexpHsscfrb4UuQdf/EFWCFiRAw\r\n"
		"VAYDVR0gBE0wSzAIBgZngQwBAgEwPwYLKwYBBAGC3xMBAQEwMDAuBggrBgEFBQcC\r\n"
		"ARYiaHR0cDovL2Nwcy5yb290LXgxLmxldHNlbmNyeXB0Lm9yZzA8BgNVHR8ENTAz\r\n"
		"MDGgL6AthitodHRwOi8vY3JsLmlkZW50cnVzdC5jb20vRFNUUk9PVENBWDNDUkwu\r\n"
		"Y3JsMB0GA1UdDgQWBBSoSmpjBH3duubRObemRWXv86jsoTANBgkqhkiG9w0BAQsF\r\n"
		"AAOCAQEA3TPXEfNjWDjdGBX7CVW+dla5cEilaUcne8IkCJLxWh9KEik3JHRRHGJo\r\n"
		"uM2VcGfl96S8TihRzZvoroed6ti6WqEBmtzw3Wodatg+VyOeph4EYpr/1wXKtx8/\r\n"
		"wApIvJSwtmVi4MFU5aMqrSDE6ea73Mj2tcMyo5jMd6jmeWUHK8so/joWUoHOUgwu\r\n"
		"X4Po1QYz+3dszkDqMp4fklxBwXRsW10KXzPMTZ+sOPAveyxindmjkW8lGy+QsRlG\r\n"
		"PfZ+G6Z6h7mjem0Y+iWlkYcV4PIWL1iwBi8saCbGS5jN2p8M+X+Q7UNKEkROb3N6\r\n"
		"KOqkqm57TH2H3eDJAkSnh6/DNFu0Qg==\r\n"
		"-----END CERTIFICATE-----\r\n"
};

static char _scheme[8];
static char _host[64];
static char _path[128];
static int _port;
static char *_cert;

/**
 * This routine demonstrates:
 *   (1) how to setup multiple websocket clients, including socket 
 *       creating, parameters setup;
 *   (2) how websocket events are handled;
 *   (3) websocket connection, and data send/receive.
 */
static int websoc_client_task(void)
{
    rws_bool ret;
    rws_socket _socket;
    websoc_instance_ptr instance;
    int _cnt = 0;

    /* wait for network connection, max 10s */
    while (!_network_ready) {
        if (_cnt++ > 30) {
            LOGE(TAG, "network is not ready, %s will exit now.", __func__);
            return -1;
        }
        rws_thread_sleep(1000);
    }

    /* instance setup */
    instance = create_websoc_instance();
    if (!instance) {
        LOGE(TAG, "Failed to create websocket instance.");
        return -1;
    }

reconnect:
    /* create websocket */
    _socket = rws_socket_create();
    /* store the socket handle in instance array */
    instance->socket = _socket;

    /* parameter setup: scheme, host, path, port ... */
    rws_socket_set_scheme(_socket, _scheme);
    rws_socket_set_host(_socket, _host);
    rws_socket_set_path(_socket, _path);
    rws_socket_set_port(_socket, _port);
    if (_cert) rws_socket_set_server_cert(_socket, _cert, strlen(_cert) + 1);

    /* setup websocket event handlers */
    rws_socket_set_on_disconnected(_socket, &on_socket_disconnected);
    rws_socket_set_on_connected(_socket, &on_socket_connected);
    rws_socket_set_on_received_text(_socket, &on_socket_received_text);
    rws_socket_set_on_received_bin(_socket, &on_socket_received_bin);
    rws_socket_set_on_received_pong(_socket, &on_socket_received_pong);
    rws_socket_set_on_send_ping(_socket, &on_socket_send_ping);

    /* connect to websocket server */
    ret = rws_socket_connect(_socket);
    if (ret != rws_true) {
        LOGE(TAG, "Failed to connect to %s:%d.", WEBSOC_SERVER, WEBSOC_PORT);
        rws_socket_delete(_socket);
        return -1;
    }

    /* wait until websocket connection is ready */
    while (!(instance->state_flags & WEBSOCKET_CONNECTED) &&
           !(instance->state_flags & WEBSOCKET_DISCONNECTED)) {
        rws_thread_sleep(1000);
    }

    while (1) {
        if (instance->state_flags & WEBSOCKET_CONNECTED) {
            int i = 0;
            rws_bool ret;

            /* check whether pong timeout */
            if (instance->last_recv_pong_ms != 0 &&
                (aos_now_ms() - instance->last_recv_pong_ms) > WEBSOCKET_DISCON_PONG_TIMEOUT_MS) {
                instance->state_flags &= ~WEBSOCKET_CONNECTED;
                instance->state_flags |= WEBSOCKET_DISCONNECTED;
                continue;
            }

            /* send bin start, if failure this round should be stopped */
            ret = rws_socket_send_bin_start(_socket, "start", strlen("start"));
            if (ret == rws_false) {
                LOGE(TAG, "Failed to send bin start");
                rws_thread_sleep(2000);
                continue;
            }

            /* send bin body */
            while(text_arr[i] != NULL) {
                rws_socket_send_bin_continue(_socket, text_arr[i], strlen(text_arr[i]));
                rws_thread_sleep(1000);
                i++;
            }

            /* send bin finish, indicate the end of the bin sending */
            rws_socket_send_bin_finish(_socket, "finish", strlen("finish"));

            rws_socket_send_ping(_socket);
        } else if (instance->state_flags & WEBSOCKET_DISCONNECTED) {
            rws_error err = rws_socket_get_error(_socket);

            LOGI(TAG, "instance %d disconnected! code: %d descr: %s",
                instance->id,
                rws_error_get_code(err),
                rws_error_get_description(err));

            rws_socket_disconnect_and_release(_socket);
            instance->socket = _socket = NULL;
            instance->state_flags = 0;
            instance->last_recv_pong_ms = 0;

            goto reconnect;
        } else {
            LOGE(TAG,"unknown state %d\n", instance->state_flags);
            break;
        }

        rws_thread_sleep(2000);
   }

    return 0;
}

static void websoc_set_default_para(void)
{
    strncpy(_scheme, WEBSOC_SCHEME, sizeof(_scheme) - 1);
    strncpy(_host, WEBSOC_SERVER, sizeof(_host) - 1);
    strncpy(_path, WEBSOC_PATH, sizeof(_path) - 1);
    _port = WEBSOC_PORT;
    _cert = WEBSOC_CERT;
}

static void wifi_event_handler(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }
    
    if (event->code != CODE_WIFI_ON_GOT_IP){
        return;
    }

    _network_ready = true;

    websoc_set_default_para();
    aos_task_new("websoc_upload_1", websoc_client_task, NULL, 2 * 1024);
#if (WEBSOCKET_MAX_INSTANCE_NUM > 1)
    aos_task_new("websoc_upload_2", websoc_client_task, NULL, 2 * 1024);
#endif
}

/*
 * This example demostrates how to use websocket API to interact with remote server.
 * Specifcially, this example set up multiple websocket instances for echo. 
 */
int application_start(int argc, char *argv[])
{
    /* set log level */
    aos_set_log_level(AOS_LL_INFO);

    /* setup wifi network event handler */
    aos_register_event_filter(EV_WIFI, wifi_event_handler, NULL);

    /* use netmgr command to setup network connection */
    netmgr_init();
    netmgr_start(false);

    /* initialize instances */
    websoc_instances_init();

    /* enter AOS loop */
    aos_loop_run();

    return 0;
}

