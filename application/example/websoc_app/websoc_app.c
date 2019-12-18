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
#define WEBSOCKET_CONNECTED             (0x01)
#define WEBSOCKET_DISCONNECTED          (0x02)
#define WEBSOCKET_DATA_NOT_RECVED       (0x04)
#define WEBSOCKET_MAX_INSTANCE_NUM      2

typedef struct websoc_instance
{
    int state_flags; /* the state of the websocket */
    rws_socket socket; /* socket context */
    int id; /* instance array index */
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

static void delete_websoc_instance(websoc_instance_ptr ptr)
{
    if (!ptr) return;
    aos_mutex_lock(&websoc_instance_mutex, AOS_WAIT_FOREVER);
    memset(ptr, 0, sizeof(websoc_instance_ptr));
    aos_mutex_unlock(&websoc_instance_mutex);
}

static void on_socket_received_text(rws_socket socket, const char * text, const unsigned int length, bool finished)
{
    unsigned int i;
    
    LOGI(TAG, "\nSocket text received %s", text);
}

static void on_socket_received_bin(rws_socket socket, const void * data, const unsigned int length, bool finished)
{
    LOGI(TAG, "rev bin\r\n");
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
        LOGE(TAG, "intance %d socket disconnect with code, error: %i, %s",
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

    LOGI(TAG, "intance %d socket received pong!", instance->id);
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
        } else if (instance->state_flags & WEBSOCKET_DISCONNECTED) {
            LOGI(TAG, "websocket disconnected!");
        } else {
            LOGE(TAG,"unknown state %d\n", instance->state_flags);
        }

        rws_thread_sleep(2000);
   }

    return 0;
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
}

static void app_delayed_action(void *arg)
{
    LOG("%s:%d %s\r\n", __func__, __LINE__, aos_task_name());
}

static void print_help_message()
{
    LOG("\r\n"
        "Usage:\r\n"
        "    # websoc [-h|--help] | [cert] | [scheme host pah port]\r\n"
        "- Note: please make sure network is connected before executing \"websoc\"\r\n"
        "        You can connect network with \"netmgr connect <ssid> <passwd>\"\r\n"
        "- To get help:\r\n"
        "    # websoc -h|--help\r\n"
        "- To connect default server:\r\n"
        "    # websoc\r\n"
        "  The default server is %s://%s%s:%d\r\n"
        "- To connect default server with certificate:\r\n"
        "    # websoc cert\r\n"
        "  The certificate is:\r\n\r\n%s\r\n"
        "  To connect your specific server:\r\n"
        "    # websoc <scheme> <host> <path> <port> (e.g. \"ws demos.kaazing.com /echo 80\")",
        WEBSOC_SCHEME, WEBSOC_SERVER, WEBSOC_PATH, WEBSOC_PORT, WEBSOC_CERT);
}

static void websoc_handler(char *outbuf, int len, int argc, char **argv)
{
    if (argc != 1 && argc != 2 && argc != 5) {
        LOGE(TAG, "%s invalid argument", __func__);
         return;
    }

    if ((argc == 2) && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        print_help_message();
        return;
    }

    if (argc == 5) {
        strncpy(_scheme, argv[1], sizeof(_scheme) - 1);
        strncpy(_host, argv[2], sizeof(_host) - 1);
        strncpy(_path, argv[3], sizeof(_path) - 1);
        _port = atoi(argv[4]);
    } else {
        strncpy(_scheme, WEBSOC_SCHEME, sizeof(_scheme) - 1);
        strncpy(_host, WEBSOC_SERVER, sizeof(_host) - 1);
        strncpy(_path, WEBSOC_PATH, sizeof(_path) - 1);
        _port = WEBSOC_PORT;
    }

    if (argc == 2 && strcmp(argv[1], "cert") == 0) {
        _cert = WEBSOC_CERT;
    }

    LOGD(TAG, "Server Info: %s %s %s %d", _scheme, _host, _path, _port);

    aos_task_new("websoc_upload_1", websoc_client_task, NULL, 8 * 1024);
    aos_task_new("websoc_upload_2", websoc_client_task, NULL, 8 * 1024);
}

/**
 * To use the websocket commands, please follow below steps:
 *   - Setup the network with "netmgr" cli:
 *     # netmgr connect <WiFi_AP_ssid> <WiFi_AP_password>
 *   - Start websocket with "websoc" cli, with default server setting:
 *     # websoc
 *     Or, use default server but with certificate:
 *     # websoc cert
 *     Or, specify the server setting as below:
 *     # websoc ws demos.kaazing.com /echo 80
 *   - If debug message needed, set the log level as below:
 *     # loglevel D 
 */
static struct cli_command websoc_commands[] = {
    {"websoc", "websoc [-h|--help] | [cert] | [scheme host path port]", websoc_handler},
};

int application_start(int argc, char *argv[])
{
    /* set log level */
    aos_set_log_level(AOS_LL_INFO);

    /* register websocket commands */
    aos_cli_register_commands(websoc_commands,
        sizeof(websoc_commands) / sizeof(struct cli_command));

    /* setup wifi network event handler */
    aos_register_event_filter(EV_WIFI, wifi_event_handler, NULL);

    /* use netmgr command to setup network connection */
    netmgr_init();
    netmgr_start(false);

    /* initialize instances */
    websoc_instances_init();

    /* enter AOS loop */
    aos_post_delayed_action(1000, app_delayed_action, NULL);
    aos_loop_run();

    return 0;
}

