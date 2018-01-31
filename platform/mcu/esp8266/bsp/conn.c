
#include "esp_common.h"

#include "lwip/sockets.h"

#include "espos_task.h"

#define SSID "Yuemewifi-3766"
#define PASSWORD "aos12345"

#define TCP_DEMO_TARGET_NAME "www.baidu.com"
#define TCP_DEMO_LOCAL_TCP_PORT 443
#define TCP_DEMO_TARGET_TCP_PORT 443

//#define lwip_htons(x) ((((x) & 0xff) << 8) | (((x) & 0xff00) >> 8))

static void tcp_demo_thread(void *p)
{
    int ret;

    int socket;
    struct sockaddr_in sock_addr;

    ip_addr_t target_ip;

    int recv_bytes = 0;

    printf("OpenSSL demo thread start...\n");

    do {
        ret = netconn_gethostbyname(TCP_DEMO_TARGET_NAME, &target_ip);
    } while(ret);
    printf("get target IP is %d.%d.%d.%d\n", (unsigned char)((target_ip.addr & 0x000000ff) >> 0),
                                                (unsigned char)((target_ip.addr & 0x0000ff00) >> 8),
                                                (unsigned char)((target_ip.addr & 0x00ff0000) >> 16),
                                                (unsigned char)((target_ip.addr & 0xff000000) >> 24));

    printf("create socket ......");
    socket = socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0) {
        printf("failed\n");
        goto err;
    }
    printf("OK\n");

    printf("bind socket ......");
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = 0;
    sock_addr.sin_port = htons(TCP_DEMO_LOCAL_TCP_PORT);
    ret = bind(socket, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (ret) {
        printf("failed\n");
        goto err;
    }
    printf("OK\n");

    printf("socket connect to remote ......");
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = target_ip.addr;
    sock_addr.sin_port = htons(TCP_DEMO_TARGET_TCP_PORT);
    ret = connect(socket, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (ret) {
        printf("failed\n", TCP_DEMO_TARGET_TCP_PORT);
        goto err;
    }
    printf("OK\n");

err:
    espos_task_del(ESPOS_TASK_DELETE_SELF);
}

static void wifi_event_handler_cb(System_Event_t *event)
{
    int ret;
    espos_task_t task;

    if (event == NULL) {
        return;
    }

    switch (event->event_id) {
        case EVENT_STAMODE_GOT_IP:
            ret = espos_task_create(&task, "tcp_demo", NULL, 5, 1, 1024, tcp_demo_thread, ESPOS_TASK_CREATE_NORMAL);
            printf("sta got ip %d\n", ret);
            break;
        default:
            break;
    }
}

void user_conn_test_init(void)
{
    wifi_set_opmode(STATION_MODE);

    // set AP parameter
    struct station_config config;
    bzero(&config, sizeof(struct station_config));
    sprintf(config.ssid, SSID);
    sprintf(config.password, PASSWORD);
    wifi_station_set_config(&config);

    wifi_set_event_handler_cb(wifi_event_handler_cb);
}
