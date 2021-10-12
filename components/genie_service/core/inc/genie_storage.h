/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __GENIE_STORAGE_H__
#define __GENIE_STORAGE_H__

#include <bluetooth/bluetooth.h>

#include <api/mesh.h>
#include "net.h"
//#include "flash.h"

#define GENIE_KV_SEQ_KEY "seq"
#define GENIE_SEQ_MAGIC_NUMBER (0xA8)
#define GENIE_SEQ_SAVE_INTERVAL (100)

enum {
    GENIE_STORAGE_INDEX_START = 0,
    GFI_MESH_PARA,
    GFI_MESH_TRITUPLE,
    GFI_MESH_SADDR,
    GFI_MESH_SEQ,
    GFI_MESH_RESET_CNT,
    GFI_MESH_POWERUP,
    GFI_MESH_HB,
    GFI_MESH_DEVKEY,
    GFI_MESH_NETKEY,
    GFI_MESH_APPKEY,
    GFI_OTA_INDICAT,
    GFI_OTA_IMAGE_ID,
    GFI_USERDATA_START,
#ifdef CONFIG_BT_MESH_CTRL_RELAY
    GFI_MESH_CTRL_RELAY,
#endif
};

typedef enum
{
    GENIE_STORAGE_SUCCESS = 0,
    GENIE_STORAGE_INIT_FAIL,
    GENIE_STORAGE_MALLOC_FAIL,
    GENIE_STORAGE_EARSE_FAIL,
    GENIE_STORAGE_DATA_INVALID,
    GENIE_STORAGE_ERASE_FAIL,
    GENIE_STORAGE_READ_FAIL,
    GENIE_STORAGE_WRITE_FAIL,
    GENIE_STORAGE_DELETE_FAIL,
    GENIE_STORAGE_INVALID_INDEX,
    GENIE_STORAGE_SIZE_ERROR,
    GENIE_STORAGE_CHECK_CRC_FAIL
} genie_storage_status_e;

typedef struct{
    uint16_t dst;
    uint8_t count;
    uint8_t period;
    uint8_t ttl;
    uint16_t feat;
    uint16_t net_idx;
} mesh_hb_para_t;

typedef struct{
    uint16_t net_index;
    uint8_t flag;
    uint32_t ivi;
    uint8_t key[16];
} mesh_netkey_para_t;

typedef struct{
    uint16_t net_index;
    uint16_t key_index;
    uint8_t flag;
    uint8_t key[16];
} mesh_appkey_para_t;

#ifdef CONFIG_BT_MESH_CTRL_RELAY
typedef struct
{
    uint8_t enable;
    uint8_t trd_n;
    uint8_t rssi;
    uint8_t sta_period;
    uint8_t chk_period;
    uint8_t req_period;
} mesh_ctrl_relay_para_t;
#endif

/**
 * @brief iniatialize the flash for further operation
 * @return the status of iniatializetion, 0 for successed.
 */
genie_storage_status_e genie_storage_init(void);

/**
 * @brief read the date from flash and decrypt it.
 * @param[in] index refers to the flash partition to be read.
 * @param[out] p_buff: the data.
 * @param[in] data_size: the size of the data.
 * @return the status of reading operation.
 */
genie_storage_status_e genie_storage_read_reliable(uint16_t index, uint8_t *p_buff, uint16_t data_size);

/**
 * @brief encrypt the data and write it into flash
 * @param[in] index refers to the flash partition to be read.
 * @param[out] p_buff: the data.
 * @param[in] data_size: the size of the data.
 * @return the status of writing operation.
 */
genie_storage_status_e genie_storage_write_reliable(uint16_t index, uint8_t *p_buff, uint16_t data_size);

/**
 * @brief delete the contents saved at the partition refered by index.
 * @param[in] index refers to the partition
 * @return the status of the operation.
 */
genie_storage_status_e genie_storage_delete_reliable(uint16_t index);

/**
 * @brief read the user raw date from flash
 * @param[in] index refers to the flash partition to be read.
 * @param[out] p_buff: the data.
 * @param[in] data_size: the size of the data.
 * @return the status of reading operation.
 */
genie_storage_status_e genie_storage_read_userdata(uint16_t index, uint8_t *p_buff, uint16_t data_size);

/**
 * @brief write the date into flash
 * @param[in] index refers to the flash partition to be read.
 * @param[out] p_buff: the data.
 * @param[in] data_size: the size of the data.
 * @return the status of writing operation.
 */
genie_storage_status_e genie_storage_write_userdata(uint16_t index, uint8_t *p_buff, uint16_t data_size);

/**
 * @brief delete the contents saved at the partition refered by index.
 * @param[in] index refers to the partition
 * @return the status of the operation.
 */
genie_storage_status_e genie_storage_delete_userdata(uint16_t index);

genie_storage_status_e genie_triple_write(uint32_t *p_pid, uint8_t *p_mac,  uint8_t *p_key);
genie_storage_status_e genie_triple_read(uint32_t *p_pid, uint8_t *p_mac,  uint8_t *p_key);

/**
 * @brief save the subscription address to flash
 * @param[in] p_sub: the list of subscription address
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_write_sub(uint16_t *p_sub);

/**
 * @brief read the subscription address from flash
 * @param[out] p_sub: the list of subscription address
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_read_sub(uint16_t *p_sub);

/**
 * @brief save the parameters for mesh to flash
 * @param[in] mesh_data: the parameters for mesh
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_write_para(struct bt_mesh_net *mesh_data);

/**
 * @brief read the parameters for mesh from flash
 * @param[in] mesh_data: the parameters for mesh
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_read_para(struct bt_mesh_net *mesh_data);

/**
 * @brief save the address of peer mesh devices to flash
 * @param[in] p_addr: address
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_write_addr(uint16_t *p_addr);

/**
 * @brief read the address of peer mesh devices from flash
 * @param[out] p_addr: address
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_read_addr(uint16_t *p_addr);

/**
 * @brief write the heartbeat parameters to flash
 * @param[in] p_para: heartbeat parameters
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_write_hb(mesh_hb_para_t *p_para);

/**
 * @brief read the heartbeat parameters from flash
 * @param[out] p_para: heartbeat parameters
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_read_hb(mesh_hb_para_t *p_para);

#ifdef CONFIG_BT_MESH_CTRL_RELAY
/**
 * @brief write the ctrl relay parameters to flash
 * @param[in] p_para: ctrl relay parameters
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_write_ctrl_relay(mesh_ctrl_relay_para_t *p_para);

/**
 * @brief read the ctrl relay parameters from flash
 * @param[out] p_para: ctrl relay parameters
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_read_ctrl_relay(mesh_ctrl_relay_para_t *p_para);
#endif

/**
 * @brief save the devices key to flash
 * @param[in] status: devcie key
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_write_devkey(uint8_t *p_devkey);

/**
 * @brief read the devices key from flash
 * @param[out] status: devcie key
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_read_devkey(uint8_t *p_devkey);

/**
 * @brief save the net key to flash
 * @param[in] status: net key
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_write_netkey(mesh_netkey_para_t *p_netkey);

/**
 * @brief read the net key from flash
 * @param[out] status: net key
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_read_netkey(mesh_netkey_para_t *p_netkey);

/**
 * @brief save the app key to flash
 * @param[in] status: app key
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_write_appkey(mesh_appkey_para_t *p_appkey);

/**
 * @brief read the app key from flash
 * @param[out] status: app key
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_read_appkey(mesh_appkey_para_t *p_appkey);

/**
 * @brief save the seq number to flash
 * @param[in] status: seq number
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_write_seq(uint32_t *p_seq, bool force_write);

/**
 * @brief read the seq number from flash
 * @param[out] status: seq number
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_read_seq(uint32_t *p_seq);

/**
 * @brief delete the seq number from flash
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_delete_seq(void);

/**
 * @brief erase everyting on flash except the trituple info
 * @return the status of operation, 0 means successed.
 */
genie_storage_status_e genie_storage_reset_system(void);

#endif/*--_GENIE_STORAGE_H_--*/
