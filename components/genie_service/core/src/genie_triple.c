/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

//#include <aos/aos.h>

#include <misc/printk.h>
#include <misc/byteorder.h>
#include <tinycrypt/sha256.h>
#include <tinycrypt/constants.h>
//#include <port/mesh_hal_sec.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"
#include "crc16.h"
#include "genie_service.h"

static genie_triple_t genie_triple;

genie_storage_status_e genie_triple_write(uint32_t *p_pid, uint8_t *p_mac, uint8_t *p_key)
{
#if 0
    uint16_t triple_crc = 0;
    uint16_t triple_crc_magic = GENIE_TRIPLE_CRC_MAGIC;
    uint8_t data[GENIE_SIZE_TRI_TRUPLE];

    memcpy(data, p_pid, GENIE_TRIPLE_PID_SIZE);
    memcpy(data + GENIE_TRIPLE_PID_SIZE, p_key, GENIE_TRIPLE_KEY_SIZE);
    memcpy(data + GENIE_TRIPLE_PID_SIZE + GENIE_TRIPLE_KEY_SIZE, p_mac, GENIE_TRIPLE_MAC_SIZE);

    triple_crc = util_crc16_compute(data, GENIE_SIZE_TRI_TRUPLE - GENIE_TRIPLE_CRC_LEN, &triple_crc_magic);
    memcpy(data + GENIE_TRIPLE_PID_SIZE + GENIE_TRIPLE_KEY_SIZE + GENIE_TRIPLE_MAC_SIZE, (unsigned char *)&triple_crc, GENIE_TRIPLE_CRC_LEN);

    return genie_storage_write_reliable(GFI_MESH_TRITUPLE, data, GENIE_SIZE_TRI_TRUPLE);
#else
    int32_t ret;

    //uint32_t pid;
    //uint8_t mac[GENIE_TRIPLE_MAC_SIZE];
    //uint8_t device_secret[GENIE_TRIPLE_KEY_SIZE];

    if ( p_pid == NULL || p_mac == NULL || p_key == NULL) {
        GENIE_LOG_ERR("[%s]: input para error!\r\n", __func__);
        return -1;
    }

    ret = aos_kv_set("pid", p_pid, GENIE_TRIPLE_PID_SIZE, 1);
    if ( ret ){
        GENIE_LOG_ERR("[%s]: set 'product_key' fail! ret = %d\r\n", __func__, ret);
        return ret;
    }

#if 0
    printk("step 2, mac before save in kv : ");
    for (int i = 0; i < 6; i++)
    {
        printk("%02x", p_mac[i]);
    }
    printk("\n");
#endif

    ret = aos_kv_set("pro_mac", p_mac, GENIE_TRIPLE_MAC_SIZE + 1, 1);
    if ( ret ) {
        GENIE_LOG_ERR("[%s]: set 'device_name' fail! ret = %d\r\n", __func__, ret);
        return ret;
    }

    ret = aos_kv_set("pro_secret", p_key, GENIE_TRIPLE_KEY_SIZE + 1, 1);
    if ( ret ) {
        GENIE_LOG_ERR("[%s]: set 'device_secret' fail! ret = %d\r\n", __func__, ret);
        return ret;
    }

    return GENIE_STORAGE_SUCCESS;
#endif
}

genie_storage_status_e genie_triple_read(uint32_t *p_pid, uint8_t *p_mac, uint8_t *p_key)
{
#if 0
    uint16_t triple_crc = 0;
    uint16_t triple_read_crc = 0;
    uint16_t triple_crc_magic = GENIE_TRIPLE_CRC_MAGIC;
    unsigned char *p_data = NULL;
    genie_storage_status_e ret;
    uint8_t data[GENIE_SIZE_TRI_TRUPLE];

    ret = genie_storage_read_reliable(GFI_MESH_TRITUPLE, data, GENIE_SIZE_TRI_TRUPLE);
    if (GENIE_STORAGE_SUCCESS != ret)
    {
        GENIE_LOG_ERR("read triple fail:%d", ret);
        return ret;
    }

    triple_crc = util_crc16_compute(data, GENIE_SIZE_TRI_TRUPLE - GENIE_TRIPLE_CRC_LEN, &triple_crc_magic);

    p_data = data + GENIE_TRIPLE_PID_SIZE + GENIE_TRIPLE_KEY_SIZE + GENIE_TRIPLE_MAC_SIZE;

    triple_read_crc = p_data[0] | (p_data[1] << 8);
    if (triple_crc != triple_read_crc)
    {
        return GENIE_STORAGE_READ_FAIL;
    }

    memcpy(p_pid, data, GENIE_TRIPLE_PID_SIZE);
    memcpy(p_key, data + GENIE_TRIPLE_PID_SIZE, GENIE_TRIPLE_KEY_SIZE);
    memcpy(p_mac, data + GENIE_TRIPLE_PID_SIZE + GENIE_TRIPLE_KEY_SIZE, GENIE_TRIPLE_MAC_SIZE);

    return GENIE_STORAGE_SUCCESS;
#else
    int32_t ret;

    uint32_t pid;
    uint8_t mac[GENIE_TRIPLE_MAC_SIZE];
    uint8_t device_secret[GENIE_TRIPLE_KEY_SIZE];

    int pid_len = GENIE_TRIPLE_PID_SIZE;
    int mac_len = GENIE_TRIPLE_MAC_SIZE + 1;
    int secret_len = GENIE_TRIPLE_KEY_SIZE + 1;

    ret = aos_kv_get("pid", &pid, &pid_len);
    if (ret != 0) {
        printf("pid in kv is valid \r\n");
    }

    ret = aos_kv_get("pro_mac", mac, &mac_len);
    if ((ret != 0) || (mac_len != mac_len)) {
        printf("mac in kv is valid ,ret = %d, mac_len = %d \r\n", ret, mac_len);
    }

#if 0
    printk("step 3, mac get from kv : ");
    for (int i = 0; i < 6; i++)
    {
        printk("%02x", mac[i]);
    }
    printk("\n");
#endif

    ret = aos_kv_get("pro_secret", device_secret, &secret_len);
    if ((ret != 0) || (secret_len != secret_len)) {
        printf("secret in kv is valid ,ret = %d,  secret_len = %d\r\n", ret, secret_len);
    }

    memcpy(p_pid, &pid, GENIE_TRIPLE_PID_SIZE);
    memcpy(p_key, device_secret, GENIE_TRIPLE_KEY_SIZE);
    memcpy(p_mac, mac, GENIE_TRIPLE_MAC_SIZE);

#if 0
    printk("step 4, in p_mac : ");
    for (int i = 0; i < 6; i++)
    {
        printk("%02x", p_mac[i]);
    }
    printk("\n");
#endif

    return GENIE_STORAGE_SUCCESS;
#endif
}

int8_t genie_triple_init(void)
{
    genie_storage_status_e ret;

    memset(&genie_triple, 0, sizeof(genie_triple_t));

    ret = genie_triple_read(&genie_triple.pid, genie_triple.mac, genie_triple.key);
    if (ret != GENIE_STORAGE_SUCCESS)
    {
        GENIE_LOG_ERR("No genie triples,please burn them");
        return -1;
    }

    return 0;
}

genie_triple_t *genie_triple_get(void)
{
    return &genie_triple;
}

uint8_t *genie_triple_get_mac(void)
{
    return genie_triple.mac;
}
