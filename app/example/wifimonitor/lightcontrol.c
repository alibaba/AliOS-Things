/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <hal/soc/soc.h>
#include <aos/network.h>
#include <umesh.h>
#include <aos/errno.h>

#define MODULE_NAME "lightcontrol"

#define LIGHT_CONTROL_PERIOD  3000
#define LIGHT_CONTROL_PORT    19999
#define DEFAULT_THRESHOLD    -35
#define RSSI_PROC_THRESHOLD   -80

/* message type defination */
#define TYPE_RSSI_DATA        1
#define TYPE_RSSI_THRESHOLD   2
#define TYPE_RSSI_COMPENSATE  3

/* rssi trend difination */
#define RSSI_NONEXIST   0
#define RSSI_INCREASING 1
#define RSSI_DECREASING 2
#define RSSI_STABLE     3

#define NEIGHBOR_COOPERATE 1
//DEBUG LIGHTS
//#define LIGHT1_MAC "\xb0\xf8\x93\x10\x87\xbf"
//#define LIGHT2_MAC "\xb0\xf8\x93\x10\x87\xcb"
//#define LIGHT3_MAC "\xb0\xf8\x93\x10\x87\xae"
//#define LIGHT4_MAC "\xb0\xf8\x93\x10\x87\xb3"
//#define LIGHT5_MAC "\xb0\xf8\x93\x10\x87\xba"
//#define LIGHT6_MAC "\xb0\xf8\x93\x10\x87\xdd"
//#define LIGHT7_MAC "\xb0\xf8\x93\x10\x87\xd3"
//#define LIGHT8_MAC "\xb0\xf8\x93\x10\x87\x8c"

//REAL LIGHTS
#define LIGHT1_MAC "\xb0\xf8\x93\x10\x87\x99"
#define LIGHT2_MAC "\xb0\xf8\x93\x10\x87\xf9"
#define LIGHT3_MAC "\xb0\xf8\x93\x10\x87\xa9"
#define LIGHT4_MAC "\xb0\xf8\x93\x10\x88\x00"
#define LIGHT5_MAC "\xb0\xf8\x93\x10\x87\xfa"
#define LIGHT6_MAC "\xb0\xf8\x93\x10\x87\xa0"
#define LIGHT7_MAC "\xb0\xf8\x93\x10\x87\x8b"
#define LIGHT8_MAC "\xb0\xf8\x93\x10\x87\x9e"

static int send_message(void *buf, int len);

#if NEIGHBOR_COOPERATE
typedef struct neighbor_light_s {
    slist_t  next;
    uint8_t  mac[6];
    int8_t   rssi, trend;
} neighbor_light_t;
slist_t neighbor_lights;

static neighbor_light_t *get_neighbor_light(char *mac)
{
    neighbor_light_t *nbr = NULL;
    slist_for_each_entry(&neighbor_lights, nbr, neighbor_light_t, next) {
        if (memcmp(nbr->mac, mac, sizeof(nbr->mac)) == 0)
            return nbr;
    }
    return nbr;
}

static int add_neighbor_light(char *mac)
{
    neighbor_light_t *nbr = get_neighbor_light(mac);
    if (nbr != NULL)
        return 0;

    nbr = (neighbor_light_t *)aos_malloc(sizeof(neighbor_light_t));
    if (nbr == NULL)
        return -1;

    memcpy(nbr->mac, mac, sizeof(nbr->mac));
    nbr->rssi = -127;
    nbr->trend = RSSI_NONEXIST;
    slist_add_tail(&nbr->next, &neighbor_lights);
    LOGD(MODULE_NAME, "neighbor_light %02x:%02x:%02x:%02x:%02x:%02x added",
         mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return 0;
}
#endif

typedef struct phone_s {
    slist_t  next;
    uint32_t heard_time;
    uint8_t  mac[6];
    int8_t   rssi_tmp, rssi_prv, rssi_trend;
    int8_t   rssi_compensate;
} phone_t;

uint8_t snd_buf[128];
slist_t phone_list;
static  gpio_dev_t gpio_led;
static  gpio_dev_t gpio_light;
uint8_t light_status;
int     light_sockfd;
int8_t  rssi_threshold;
int8_t  max_rssi = -127;
int8_t  rssi_adj = 0;

static int add_phone(uint8_t *mac, int8_t rssi_compensate)
{
    phone_t *phone = NULL;
    slist_for_each_entry(&phone_list, phone, phone_t, next) {
        if (memcmp(phone->mac, mac, sizeof(phone->mac)) == 0) {
            phone->rssi_compensate = rssi_compensate;
            return 0;
        }
    }

    phone = (phone_t *)aos_malloc(sizeof(phone_t));
    if (phone == NULL)
        return -1;

    phone->heard_time = aos_now_ms();
    memcpy(phone->mac, mac, sizeof(phone->mac));
    phone->rssi_tmp = -127;
    phone->rssi_prv = -127;
    phone->rssi_trend = RSSI_NONEXIST;
    phone->rssi_compensate = rssi_compensate;
    slist_add_tail(&phone->next, &phone_list);
    return 0;
}

static void handle_set_rssi_threshold(char *pwbuf, int blen, int argc, char **argv)
{
    int rssi_tmp;
    int ret;
    if (argc != 2)
        return;
    ret = sscanf(argv[1], "%d", &rssi_tmp);
    if (ret != 1) {
        aos_cli_printf("error input: %s\r\n", argv[1]);
        return;
    }

    int8_t rssi = rssi_tmp;
    rssi_threshold = rssi;
    aos_kv_set("rssi_threshold", &rssi, 1, 1);
    aos_cli_printf("set threshold value %d\r\n", rssi);
    snd_buf[0] = TYPE_RSSI_THRESHOLD;
    snd_buf[1] = (int8_t)rssi;
    snd_buf[2] = '\x0';
    send_message(snd_buf, 3);
}

static void handle_set_rssi_compensate(char *pwbuf, int blen, int argc, char **argv)
{
    uint32_t mac;
    int rssi;
    int ret;
    if (argc != 3)
        return;
    ret = sscanf(argv[1], "%04x", &mac);
    if (ret != 1) {
        aos_cli_printf("error input: %s\r\n", argv[1]);
        return;
    }
    ret = sscanf(argv[2], "%d", &rssi);
    if (ret != 1) {
        aos_cli_printf("error input: %s\r\n", argv[2]);
        return;
    }
    aos_cli_printf("set %04x compensate value %d\r\n", mac, rssi);
    snd_buf[0] = TYPE_RSSI_COMPENSATE;
    snd_buf[1] = (mac >> 8) & 0xff;
    snd_buf[2] = (mac >> 0) & 0xff;
    snd_buf[3] = (int8_t)rssi;
    snd_buf[4] = '\x0';
    send_message(snd_buf, 5);
}

static struct cli_command light_control_cmds[3] = {
    {
        .name = "rssi_threshold",
        .help = "rssi_threshold value",
        .function = handle_set_rssi_threshold
    },
    {
        .name = "rssi_compensate",
        .help = "rssi_compensate mac value",
        .function = handle_set_rssi_compensate
    }
};


void light_switch(uint8_t value);
static void wifimonitor_wifi_mgnt_frame_callback(uint8_t *buffer, int length, char rssi);

static void periodic_light_control(void *arg);
static void message_receive_worker(void *arg);
static void subscribed_event_cb(input_event_t *eventinfo, void *priv_data);
void light_init(void)
{
    int8_t rssi;
    int    rssi_len = 1;
    gpio_light.port = 8;
    gpio_light.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_light);
    hal_gpio_output_low(&gpio_light);

    gpio_led.port = 12;
    gpio_led.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_led);
    hal_gpio_output_high(&gpio_led);
    light_status = 0;

    /* init light control listen phones */
    slist_init(&phone_list);
    add_phone((uint8_t *)"\x14\x48\x8b\x06\xfb\xb8", 0);
    add_phone((uint8_t *)"\x3c\x47\x11\xc0\xc5\xe8", 7);
#if NEIGHBOR_COOPERATE
    slist_init(&neighbor_lights);
#endif
    if (aos_kv_get("rssi_threshold", &rssi, &rssi_len) == 0) {
        rssi_threshold = rssi;
    } else {
        rssi_threshold = DEFAULT_THRESHOLD;
    }
    LOGD(MODULE_NAME, "rssi_threshold = %d", rssi_threshold);
    aos_cli_register_commands(&light_control_cmds[0],
            sizeof(light_control_cmds) / sizeof(light_control_cmds[0]));
    aos_register_event_filter(EV_MESH, subscribed_event_cb, NULL);
    aos_task_new("lightworker", message_receive_worker, NULL, 4096);
    aos_post_delayed_action(LIGHT_CONTROL_PERIOD, periodic_light_control, NULL);
}

void light_switch(uint8_t value)
{
    if (value == light_status)
        return;
    if (value) {
        hal_gpio_output_high(&gpio_light);
        hal_gpio_output_low(&gpio_led);
        LOGD(MODULE_NAME, "switch light on");
    } else {
        hal_gpio_output_low(&gpio_light);
        hal_gpio_output_high(&gpio_led);
        LOGD(MODULE_NAME, "switch light off");
    }
    light_status = value;
}

static int send_message(void *buf, int len)
{
    int ret = -1;
    struct sockaddr_in addr;
    uint8_t ip4_addr[4];
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(LIGHT_CONTROL_PORT);
    ip4_addr[0] = 10;
    ip4_addr[1] = 0;
    ip4_addr[2] = 255;
    ip4_addr[3] = 255;
    memcpy(&addr.sin_addr, ip4_addr, sizeof(addr.sin_addr));
    ret = sendto(light_sockfd, buf, len, MSG_DONTWAIT, (struct sockaddr *)&addr, sizeof(addr));
    return ret;
}

static void receive_message(int sockfd)
{
    uint8_t buf[128];
    struct  sockaddr_in src_addr;
    size_t  slen = sizeof(src_addr);
    int     len = sizeof(buf);

    len = recvfrom(sockfd, (void *)buf, len, MSG_DONTWAIT, (struct sockaddr *)&src_addr, &slen);
    if (len <= 0) {
        return;
    }

    /* process message */
    uint8_t type = buf[0];
    int8_t  rssi;
    if (type == TYPE_RSSI_THRESHOLD) {
        rssi = buf[1];
        if (rssi == rssi_threshold)
            return;
        rssi_threshold = rssi;
        aos_kv_set("rssi_threshold", &rssi, 1, 1);
        LOGD(MODULE_NAME, "set rssi_threshold = %d", rssi);
    } else if (type == TYPE_RSSI_COMPENSATE) {
        const mac_address_t *mac = umesh_get_mac_address(MEDIA_TYPE_DFL);
        phone_t *phone;
        rssi = buf[3];
        if (memcmp(&mac->addr[4], &buf[1], 2) == 0) {
            rssi_adj = rssi;
            aos_kv_set("rssi_adjust", &rssi, 1, 1);
            LOGD(MODULE_NAME, "set rssi_adjust = %d", rssi);
            return;
        }
        slist_for_each_entry(&phone_list, phone, phone_t, next) {
            if (memcmp(&phone->mac[4], &buf[1], 2) != 0)
                continue;
            phone->rssi_compensate = rssi;
            LOGD(MODULE_NAME, "set phone %02x:%02x rssi_compensate = %d",
                    phone->mac[4], phone->mac[5], rssi);
        }
    }
#if NEIGHBOR_COOPERATE
    else if (type == TYPE_RSSI_DATA) {
        char     src_ip[4], src_mac[6];
        uint16_t src_sid;
        int8_t   rssi, trend;
        memcpy(src_ip, &src_addr.sin_addr.s_addr, sizeof(src_ip));
        src_sid = ((uint16_t)src_ip[2] << 8) | src_ip[3];
        /* LOGD(MODULE_NAME, "received %d bytes from %04x", len, src_sid); */
        memcpy(src_mac, buf, sizeof(src_mac));
        if (get_neighbor_light(src_mac) == NULL)
            return;

        rssi = buf[6]; trend = buf[7];
        LOGD(MODULE_NAME, "mac:%02x:%02x:%02x:%02x:%02x:%02x, rssi:%d, trend:%d",
                src_mac[0], src_mac[1], src_mac[2], src_mac[3],
                src_mac[4], src_mac[5], rssi, trend);
        if (rssi > rssi_threshold) {
            LOGD(MODULE_NAME, "cooperate with %04x to swith light on", src_sid);
            light_switch(1);
            if (rssi > max_rssi) max_rssi = rssi;
        }
    }
#endif
}

static void message_receive_worker(void *arg)
{
    int sockfd;
    fd_set rfds;
    struct timeval timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    while (1) {
        sockfd = light_sockfd;

        if (sockfd < 0) {
            aos_msleep(100);
            continue;
        }

        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);

        int ret = lwip_select(sockfd + 1, &rfds, NULL, NULL, &timeout);
        if (ret < 0) {
            if (errno != EINTR) {
                LOGE(MODULE_NAME, "select error %d", errno);
                continue;
            }
        }

        if (sockfd != light_sockfd) {
            close(sockfd);
            continue;
        }

        if (!FD_ISSET(sockfd, &rfds)) {
            continue;
        }

        receive_message(sockfd);
    }

    LOGE(MODULE_NAME, "return");
}

static void subscribed_event_cb(input_event_t *eventinfo, void *priv_data)
{
    if (eventinfo->type != EV_MESH) {
        return;
    }

    if (eventinfo->code == CODE_MESH_CONNECTED) {
        int8_t rssi;
        int    rssi_len = 1;
        const mac_address_t *mac = umesh_get_mac_address(MEDIA_TYPE_DFL);

        if (aos_kv_get("rssi_adjust", &rssi, &rssi_len) == 0){
            rssi_adj = rssi;
        } else {
            if (memcmp(mac->addr, LIGHT1_MAC, 6) == 0) {
                rssi_adj = 3;
            } else if (memcmp(mac->addr, LIGHT2_MAC, 6) == 0) {
                rssi_adj = 0;
            } else if (memcmp(mac->addr, LIGHT3_MAC, 6) == 0) {
                rssi_adj = 3;
            } else if (memcmp(mac->addr, LIGHT4_MAC, 6) == 0) {
                rssi_adj = 0;
            } else if (memcmp(mac->addr, LIGHT5_MAC, 6) == 0) {
                rssi_adj = 1;
            } else if (memcmp(mac->addr, LIGHT6_MAC, 6) == 0) {
                rssi_adj = 2;
            } else if (memcmp(mac->addr, LIGHT7_MAC, 6) == 0) {
                rssi_adj = -1;
            } else if (memcmp(mac->addr, LIGHT8_MAC, 6) == 0) {
                rssi_adj = 0;
            }
        }
        int sockfd;
        int ret;

        sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sockfd < 0) {
            LOGE(MODULE_NAME, "error open socket %d", errno);
            return;
        }

        if (light_sockfd >= 0) {
            light_sockfd = -1;
        }

        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(LIGHT_CONTROL_PORT);
        ret = bind(sockfd, (const struct sockaddr *)&addr, sizeof(addr));
        if (ret < 0) {
            LOGE(MODULE_NAME, "error bind socket %d", errno);
            close(sockfd);
            return;
        }
        light_sockfd = sockfd;
        LOGD(MODULE_NAME, "socket up");

#if NEIGHBOR_COOPERATE
        if (memcmp(mac->addr, LIGHT1_MAC, 6) == 0) {
            add_neighbor_light(LIGHT2_MAC);
        } else if (memcmp(mac->addr, LIGHT2_MAC, 6) == 0) {
            add_neighbor_light(LIGHT1_MAC);
            add_neighbor_light(LIGHT3_MAC);
        } else if (memcmp(mac->addr, LIGHT3_MAC, 6) == 0) {
            add_neighbor_light(LIGHT2_MAC);
            add_neighbor_light(LIGHT4_MAC);
        } else if (memcmp(mac->addr, LIGHT4_MAC, 6) == 0) {
            add_neighbor_light(LIGHT3_MAC);
            add_neighbor_light(LIGHT5_MAC);
        } else if (memcmp(mac->addr, LIGHT5_MAC, 6) == 0) {
            add_neighbor_light(LIGHT4_MAC);
            add_neighbor_light(LIGHT6_MAC);
        } else if (memcmp(mac->addr, LIGHT6_MAC, 6) == 0) {
            add_neighbor_light(LIGHT5_MAC);
            add_neighbor_light(LIGHT7_MAC);
        } else if (memcmp(mac->addr, LIGHT7_MAC, 6) == 0) {
            add_neighbor_light(LIGHT6_MAC);
            add_neighbor_light(LIGHT8_MAC);
        } else if (memcmp(mac->addr, LIGHT8_MAC, 6) == 0) {
            add_neighbor_light(LIGHT7_MAC);
        }
#endif
    } else if (eventinfo->code == CODE_MESH_DISCONNECTED) {
        light_sockfd = -1;
        LOGD(MODULE_NAME, "socket down");
    }
}

static void periodic_light_control(void *arg)
{
    aos_post_delayed_action(LIGHT_CONTROL_PERIOD, periodic_light_control, NULL);

    phone_t *phone;
    uint32_t now = aos_now_ms();
    slist_for_each_entry(&phone_list, phone, phone_t, next) {
        if ( phone->rssi_prv != -127 && (now - phone->heard_time) > LIGHT_CONTROL_PERIOD) {
            phone->rssi_tmp = -127;
            phone->rssi_prv = -127;
            phone->rssi_trend = RSSI_NONEXIST;
            LOGD(MODULE_NAME, "intereseted phone left");
        }
    }

    if (max_rssi < rssi_threshold) {
        light_switch(0);
    }
    max_rssi = -127;
}

static void rssi_filtering(void *arg)
{
    phone_t *phone = (phone_t *)arg;
    int8_t diff = phone->rssi_tmp - phone->rssi_prv;
    if (diff < 0) diff = -diff;
    if (diff < 3) {
        phone->rssi_trend = RSSI_STABLE;
    } else if (phone->rssi_tmp > phone->rssi_prv) {
        phone->rssi_trend = RSSI_INCREASING;
    } else {
        phone->rssi_trend = RSSI_DECREASING;
    }
    phone->rssi_prv = phone->rssi_tmp;
    phone->rssi_tmp = -127;

    const char *trend;
    switch (phone->rssi_trend) {
        case RSSI_NONEXIST:
            trend="nonexist";
            break;
        case RSSI_INCREASING:
            trend="increasing";
            break;
        case RSSI_DECREASING:
            trend="decreasing";
            break;
        case RSSI_STABLE:
            trend="stable";
            break;
        default:
            trend="unknown";
            break;
    }
    LOGD(MODULE_NAME, "rssi:%d, trend:%s", phone->rssi_prv, trend);
#if NEIGHBOR_COOPERATE
    const mac_address_t *mac_addr = umesh_get_mac_address(MEDIA_TYPE_DFL);
    snd_buf[0] = TYPE_RSSI_DATA;
    memcpy(&snd_buf[1], mac_addr->addr, 6);
    snd_buf[7] = phone->rssi_prv;
    snd_buf[8] = phone->rssi_trend;
    snd_buf[9] = '\x0';
    send_message(snd_buf, 10);
#endif
}

void rssi_light_control(uint8_t *mac, int8_t rssi)
{
    phone_t *phone;
    rssi += rssi_adj;
    if (rssi < RSSI_PROC_THRESHOLD)
        return;
    slist_for_each_entry(&phone_list, phone, phone_t, next) {
        if (memcmp(mac, phone->mac, 6) == 0) {
            phone->heard_time = aos_now_ms();

            if (rssi > phone->rssi_tmp)
                phone->rssi_tmp = rssi;
            aos_cancel_delayed_action(-1, rssi_filtering, phone);
            aos_post_delayed_action(500, rssi_filtering, phone);

            if (rssi >= rssi_threshold)
                light_switch(1);

            if (rssi > max_rssi)
                max_rssi = rssi;
        }
    }
}

