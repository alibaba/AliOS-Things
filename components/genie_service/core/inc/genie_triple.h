/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __GENIE_TRIPLE_H__
#define __GENIE_TRIPLE_H__

#define GENIE_TRIPLE_PID_SIZE 4
#define GENIE_TRIPLE_KEY_SIZE 16
#define GENIE_TRIPLE_MAC_SIZE 6
#define GENIE_TRIPLE_CRC_LEN 2
#define GENIE_SIZE_TRI_TRUPLE (GENIE_TRIPLE_PID_SIZE + GENIE_TRIPLE_KEY_SIZE + GENIE_TRIPLE_MAC_SIZE + GENIE_TRIPLE_CRC_LEN)

#define GENIE_TRIPLE_CRC_MAGIC (0xA5A6)

typedef struct _genie_triple_s
{
    uint32_t pid;
    uint8_t key[GENIE_TRIPLE_KEY_SIZE];
    uint8_t mac[GENIE_TRIPLE_MAC_SIZE];
} genie_triple_t;

/**
 * @brief writes the trituple info into flash.
 * @param[in] p_pid: product ID
 * @param[in] p_mac: device address
 * @param[in] p_key: secret key
 * @return the status of the writing operation, 0 for success.
 */
genie_storage_status_e genie_triple_write(uint32_t *p_pid, uint8_t *p_mac, uint8_t *p_key);

/**
 * @brief get the trituple info from flash.
 * @param[in] p_pid
 * @param[in] p_mac
 * @param[in] p_key
 * @return the status of the reading operation, 0 for success.
 */
genie_storage_status_e genie_triple_read(uint32_t *p_pid, uint8_t *p_mac, uint8_t *p_key);

/**
 * @brief load the trituple info from flash.
 * @return 0 for success, otherwise failed.
 */
int8_t genie_triple_init(void);

genie_triple_t *genie_triple_get(void);
uint8_t *genie_triple_get_mac(void);

#endif