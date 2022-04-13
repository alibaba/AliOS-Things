/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#if !defined(NEW_NV_RECORD_ENALBED)

#ifndef NVRECORD_BLE_H
#define NVRECORD_BLE_H

#define BLE_RECORD_NUM 5
#define section_name_ble_nvds "blenvds"
#define NV_RECORD_BLE_KEY "ble_key"
#define NVRAM_BLE_INVALID (0xfae66666)

#define BLE_ADDR_SIZE 6
#define BLE_ENC_RANDOM_SIZE 8
#define BLE_LTK_SIZE 16
#define BLE_IRK_SIZE 16

#define BLE_STATIC_ADDR 0
#define BLE_RANDOM_ADDR 1

typedef struct _BleDeviceRecord {
    uint8_t peer_bleAddr[BLE_ADDR_SIZE];
    uint16_t EDIV;
    uint8_t RANDOM[BLE_ENC_RANDOM_SIZE];
    uint8_t LTK[BLE_LTK_SIZE];
    uint8_t IRK[BLE_IRK_SIZE];
    uint8_t bonded;

} BleDeviceinfo;

struct Ble_nvrecord {
    uint8_t loc_irk[BLE_IRK_SIZE];
    uint8_t saved_list_num;
    BleDeviceinfo ble_nv[BLE_RECORD_NUM];
};

#ifdef __cplusplus
extern "C" {
#endif

int nv_record_blerec_add(const BleDeviceinfo *param_rec);
bool nv_record_ble_record_find_ltk_through_static_bd_addr(uint8_t *pBdAddr, uint8_t *ltk);
bool nv_record_ble_record_Once_a_device_has_been_bonded(void);
void nv_record_ble_delete_entry(uint8_t *pBdAddr);
uint8_t nv_record_ble_fill_irk(uint8_t *ltkToFill);
void nv_record_blerec_init(void);
void nv_record_blerec_get_local_irk(uint8_t *pIrk);
bool nv_record_blerec_get_bd_addr_from_irk(uint8_t *pBdAddr, uint8_t *pIrk);

#ifdef __cplusplus
}
#endif
#endif

#endif  // #if !defined(NEW_NV_RECORD_ENALBED)
