/* main.c - Application main entry point */

/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aos/kernel.h>
#include <aos/cli.h>
#include <misc/printk.h>
#include <aos/hal/gpio.h>
#include "tmall_model.h"

/* use LED1 on nrf52832 pca10040 dev board for demo */
#define LED1_PIN 17

static gpio_dev_t gpio_led;

#ifdef CONFIG_PRINT_HEAP
static void print_heap()
{
    extern k_mm_head *g_kmm_head;
    int               free = g_kmm_head->free_size;
    LOG("============free heap size =%d==========", free);
}
#endif

static void prov_complete(u16_t net_idx, u16_t addr)
{
    printk("Provisioning completed!\n");
    printk("Net ID: %u\n", net_idx);
    printk("Unicast addr: 0x%04x\n", addr);
}

static void prov_reset(void)
{
    printk("reset provisioning\n");
}

/* UUID for identifying the unprovisioned node */
/* sample value generated from IoT cloud for testing */
#if 0
static const uint8_t pid[4] = { 0x00, 0x02, 0xa4, 0xee }; // product id:173294
static const uint8_t mac_addr[6] = { 0xaa, 0xbb, 0xcc, 0x00, 0x00, 0x01 }; // device name
static const char secret[] = "zd6sCqghXY37ySE6gmryIVkzkqruV6Vz"; // device secret
#else
/* provided by tmall team for testing */
static const uint8_t pid[4] = { 0x00, 0x00, 0x00, 0x02 };
static const uint8_t mac_addr[6] = { 0x78, 0xda, 0x07, 0x11, 0x27, 0xff };
static const char secret[] = "9c98d67f6bd3be48d36bd8ed29a95bab";
#endif

/* product key */
static const char pkey[] = "b1HaDlKgwoV"; 
static uint8_t dev_uuid[16] = { 0x00 };
static uint8_t digest[32] = { 0x00 };

static void blemesh_tmall_profile(void)
{
    int ret;

    printk("Initializing tmall ble mesh profile...\n");

    /* calculate digest of static oob info */
    bt_mesh_profile_calculate_digest(digest, pid, mac_addr, secret);
    printk("digest: %s\n", bt_hex(digest, 16));

    /* construct uuid broadcasted in unprovisioned beacon */
    bt_mesh_profile_construct_uuid(dev_uuid, pid, mac_addr);
    printk("dev_uuid: %s\n", bt_hex(dev_uuid, 16));

    /* configure mesh profile prov */
    ret = bt_mesh_profile_prov_init(dev_uuid, digest, 16,
                                    prov_complete, prov_reset);
    if (ret != 0) {
        printk("fail to initialize mesh profile provisioning capability\n");
    }

    /* configure mesh profile composition data page 0 */
    ret = bt_mesh_profile_composition_data_init();
    if (ret != 0) {
        printk("fail to configure composistion data\n");
    }

    /* configure mesh profile supported elements and models */
    ret = bt_mesh_profile_model_init();
    if (ret != 0) {
        printk("fail to initialize mesh profile supported elements and models\n");
    }

    /* start mesh profile */
    ret = bt_mesh_profile_start();
    if (ret != 0) {
        printk("fail to initialize mesh profile\n");
    }
}

static void app_delayed_action(void *arg)
{
    /* int LED1 */
    gpio_led.port = LED1_PIN;
    gpio_led.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_led);
    hal_gpio_output_high(&gpio_led);

    /* init tmall ble mesh profile */
    blemesh_tmall_profile();
}

#ifdef AOS_COMP_CLI
static void handle_set_mac(char *pwbuf, int blen, int argc, char **argv);

static struct cli_command tmall_cmds[] = {
    {
        .name     = "set_mac",
        .help     = "set_mac <MAC address in xx:xx:xx:xx:xx:xx format>",
        .function = handle_set_mac
    },
    /* Add more commands here per necessary. */
};

static uint8_t char2u8(char *c)
{
    uint8_t ret = 0;

    if (isdigit(*c)) {
        ret = *c - '0';
    } else if (*c >= 'A' && *c <= 'F') {
        ret = *c - 'A' + 10;
    } else if (*c >= 'a' && *c <= 'f') {
        ret = *c - 'a' + 10;
    }

    return ret;
}

static void handle_set_mac(char *pwbuf, int blen, int argc, char **argv)
{
    char *p;
    uint8_t mac[6] = {0}, i;

    if (argc < 2) {
        printf("Invalid argument.\r\n");
        printf("Usage: %s\r\n", tmall_cmds[0].help);
        return;
    }

    for (p = argv[1], i = 0; *p != '\0'; p += 3, i += 3) {
        if (!isxdigit(*p) || !isxdigit(*(p+1))) {
            printf("Invalid format, MAC not set!!!\r\n");
            return;
        }
        mac[i / 3] = ((char2u8(p) & 0x0f) << 4) | (char2u8(p+1) & 0x0f);
    }
}
#endif

int application_start(int argc, char **argv)
{
#ifdef CONFIG_PRINT_HEAP
    printf("After kernel init\r\n");
    print_heap();
#endif
#ifdef AOS_COMP_CLI
    aos_cli_register_commands(&tmall_cmds[0], sizeof(tmall_cmds) / sizeof(tmall_cmds[0]));
#endif
    aos_post_delayed_action(1000, app_delayed_action, NULL);
    ble_storage_init();
#ifdef CONFIG_PRINT_HEAP
    printf("After storage init\r\n");
    print_heap();
#endif
    aos_loop_run();
    return 0;
}
