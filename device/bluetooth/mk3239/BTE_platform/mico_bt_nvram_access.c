/*****************************************************************************
**
**  Name:          mico_bt_nvram_access.c
**
**  Description:   general method of store/retrieve informations with searching key
**                     handling linkkey storage
**
******************************************************************************/

#include "common.h"
#include "mico_system.h"
#include "StringUtils.h"

#define EXPORT_API

#include "buildcfg.h"
#include "bt_types.h"   /* This must be defined AFTER buildcfg.h */
#include "bt_trace.h"

#include "mico_bt_dev.h"

#include "mico_bt_nvram_access.h"

/** NVRAM entry for bonded device */
#pragma pack(1)
typedef struct {
    mico_bt_device_address_t    bd_addr;        /**< Device address */
    uint8_t                     addr_type;      /**< BLE_ADDR_PUBLIC or BLE_ADDR_RANDOM */
    uint8_t                     device_type;    /**< BT_DEVICE_TYPE_BREDR or BT_DEVICE_TYPE_BLE */
    uint16_t                    length;         /**< Length of key_blobs (link key information) */
    uint8_t                     key_blobs[1];   /**< Link keys (actual length specified by 'length' field) */
} mico_bt_nvram_access_entry_t;
#pragma pack()

#if defined(BTM_INTERNAL_LINKKEY_STORAGE_INCLUDED) && (BTM_INTERNAL_LINKKEY_STORAGE_INCLUDED == TRUE)


/**
 * Function     mico_bt_nvram_access_init
 *
 * initialize nvram control block
 *
 * @param[in]  max_num_bonded_devices : maximum limit of number of bonded devices
 *
 * @return  void
 */
void mico_bt_nvram_access_init()
{
    return;
}


/**
 * Function     mico_bt_nvram_access_get_bonded_devices
 *
 * get lists of bonded devices stored in nvram
 *
 * @param[in]  paired_device_list : array for getting bd address of bonded devices
 * @param[in/out] p_num_devices :  list size of paired_device_list/total number of bonded devices stored
 *
 * @return      MICO_BT_SUCCESS or ERROR
 */
mico_bt_result_t mico_bt_nvram_access_get_bonded_devices(mico_bt_dev_bonded_device_info_t bonded_device_list[],
                                                         uint16_t *p_num_devices)
{
    int index, list_size = 0;
    mico_bt_nvram_access_entry_t *p_entry;
    char key[32];
    int buflen = sizeof(mico_bt_nvram_access_entry_t) + MICO_BT_DCT_MAX_KEYBLOBS;

    *p_num_devices = 0;
    p_entry = (mico_bt_nvram_access_entry_t *)malloc(buflen);
    if (p_entry == NULL) {
        return -1;
    }
    for (index = 0 ; index < MICO_BT_DCT_MAX_DEVICES ; index++) {
        sprintf(key, "bt_bounded_dev%d", index);
        if (aos_kv_get(key, p_entry, buflen) != 0) {
            continue;
        }

        memcpy(bonded_device_list[list_size].bd_addr, p_entry->bd_addr, sizeof(p_entry->bd_addr));
        bonded_device_list[list_size].addr_type = p_entry->addr_type;
        bonded_device_list[list_size].device_type = p_entry->device_type;
        list_size++;
    }

    *p_num_devices = list_size;
    APPL_TRACE_DEBUG2("%s num bonded devices : %d", __FUNCTION__, *p_num_devices );

    free(p_entry);
    return MICO_BT_SUCCESS;
}



/**
 * Function     mico_bt_nvram_access_save_bonded_device_key
 *
 *  save link key information of bonded device
 *
 * @param[in]  bd_addr : bd_addr of bonded device
 * @param[in]  p_keyblobs : key blobs including key header, link keys and key length
 * @param[in]  key_len :  total length of p_keyblobs
 *
 * @return      MICO_BT_SUCCESS or ERROR
 */
mico_bt_result_t mico_bt_nvram_access_save_bonded_device_key(mico_bt_device_address_t bd_addr,
                                                             mico_bt_ble_address_type_t addr_type, uint8_t device_type, uint8_t *p_keyblobs, uint16_t key_len)

{
    int index, list_size = 0, idle_index = -1, ret = MICO_BT_SUCCESS;
    mico_bt_nvram_access_entry_t *p_entry;
    char key[32];
    int buflen = sizeof(mico_bt_nvram_access_entry_t) + MICO_BT_DCT_MAX_KEYBLOBS;

    p_entry = (mico_bt_nvram_access_entry_t *)malloc(buflen);
    if (p_entry == NULL) {
        return -1;
    }

    for (index = 0 ; index < MICO_BT_DCT_MAX_DEVICES ; index++) {
        sprintf(key, "bt_bounded_dev%d", index);
        if (aos_kv_get(key, p_entry, buflen) != 0) {
            idle_index = index;
            continue;
        }

        if ( !memcmp(p_entry->bd_addr, bd_addr, sizeof(mico_bt_device_address_t) )) { // found
            if ( (key_len == p_entry->length) && (memcmp( p_entry->key_blobs, p_keyblobs, key_len ) == 0)) { // same key
                APPL_TRACE_DEBUG0( "Same key, write ignore..." );
            } else {
                p_entry->length = key_len;
                memcpy(p_entry->key_blobs, p_keyblobs, key_len);
                aos_kv_set(key, p_entry, buflen, 1);
                APPL_TRACE_DEBUG0( "Found and update" );
            }

            goto EXIT;
        }
    }

    if (idle_index > 0) {
        sprintf(key, "bt_bounded_dev%d", idle_index);
        memcpy(p_entry->bd_addr, bd_addr, MICO_BT_DCT_ADDR_FIELD);
        p_entry->addr_type = addr_type;
        p_entry->device_type = device_type;
        p_entry->length = key_len;
        memcpy(p_entry->key_blobs, p_keyblobs, key_len);
        aos_kv_set(key, p_entry, p_entry->length + sizeof(mico_bt_nvram_access_entry_t), 1);
        APPL_TRACE_DEBUG0( "Create new one" );
    } else {
        ret = -1;
        APPL_TRACE_DEBUG0( "Can't save more" );
        goto EXIT;
    }

EXIT:
    free(p_entry);
    return ret;
}


/**
 * Function     mico_bt_nvram_access_load_bonded_device_keys
 *
 *  loads stored key information for bonded device having @bd_addr
 *
 * @param[in]  bd_addr : bd_addr of bonded device
 * @param[out]  p_key_entry :  key information stored
 *
 * @return      MICO_BT_SUCCESS or ERROR
 */
mico_bt_result_t mico_bt_nvram_access_load_bonded_device_keys(mico_bt_device_address_t bd_addr,
                                                              mico_bt_nvram_access_entry_t *p_key_entry, uint8_t entry_max_length)
{
    int index, ret = MICO_BT_NO_RESOURCES;
    mico_bt_nvram_access_entry_t *p_entry;
    char key[32];
    int buflen = sizeof(mico_bt_nvram_access_entry_t) + MICO_BT_DCT_MAX_KEYBLOBS;

    p_entry = (mico_bt_nvram_access_entry_t *)malloc(buflen);
    if (p_entry == NULL) {
        return -1;
    }

    for (index = 0 ; index < MICO_BT_DCT_MAX_DEVICES ; index++) {
        sprintf(key, "bt_bounded_dev%d", index);
        if (aos_kv_get(key, p_entry, buflen) != 0) {
            continue;
        }

        if ( !memcmp(p_entry->bd_addr, bd_addr, sizeof(mico_bt_device_address_t) )) { // found
            if (entry_max_length < p_entry->length) {
                ret = MICO_BT_ILLEGAL_VALUE;
            } else {
                p_key_entry->addr_type = p_entry->addr_type;
                p_key_entry->device_type = p_entry->device_type;
                p_key_entry->length = p_entry->length;
                memcpy(p_key_entry->key_blobs, p_entry->key_blobs, p_entry->length);
                ret = MICO_BT_SUCCESS;
            }
        }
    }

EXIT:
    free(p_entry);
    return ret;
}



/**
 * Function     mico_bt_nvram_access_delete_bonded_device
 *
 * remove key information from storage by releasing bonding with remote device having @bd_addr
 *
 * @param[in]  bd_addr : bd_addr of bonded device to be removed

 * @return      MICO_BT_SUCCESS or ERROR
 */
mico_bt_result_t mico_bt_nvram_access_delete_bonded_device(mico_bt_device_address_t bd_addr)
{
    int index, ret = MICO_BT_NO_RESOURCES;
    mico_bt_nvram_access_entry_t *p_entry;
    char key[32];
    int buflen = sizeof(mico_bt_nvram_access_entry_t) + MICO_BT_DCT_MAX_KEYBLOBS;

    p_entry = (mico_bt_nvram_access_entry_t *)malloc(buflen);
    if (p_entry == NULL) {
        return -1;
    }

    for (index = 0 ; index < MICO_BT_DCT_MAX_DEVICES ; index++) {
        sprintf(key, "bt_bounded_dev%d", index);
        if (aos_kv_get(key, p_entry, buflen) != 0) {
            continue;
        }

        if ( !memcmp(p_entry->bd_addr, bd_addr, sizeof(mico_bt_device_address_t) )) { // found
            aos_kv_del(key);
            ret = MICO_BT_SUCCESS;
            goto EXIT;
        }
    }

EXIT:
    free(p_entry);
    return ret;

}



/**
 * Function     mico_bt_nvram_access_load_local_identity_keys
 *
 * load local identity keys including ir/irk/dhk stored in nvram
 *
 * @param[out]  p_lkeys: local identity key information
 *
 * @return      MICO_BT_SUCCESS or ERROR
 */
mico_bt_result_t mico_bt_nvram_access_load_local_identity_keys(mico_bt_local_identity_keys_t *p_lkeys)
{
    int ret;

    ret = aos_kv_get("bt_local_key", p_lkeys, sizeof(mico_bt_local_identity_keys_t));

    if (ret != 0) {
        return MICO_BT_NO_RESOURCES;
    } else {
        return MICO_BT_SUCCESS;
    }
}


/**
 * Function     mico_bt_nvram_access_save_local_identity_keys
 *
 * save local identity keys including ir/irk/dhk to nvram
 *
 * @param[in]  p_lkeys : local identity key information
 *
 * @return      MICO_BT_SUCCESS or ERROR
 */
mico_bt_result_t mico_bt_nvram_access_save_local_identity_keys(mico_bt_local_identity_keys_t *p_lkeys)
{
    int ret;

    ret = aos_kv_set("bt_local_key", p_lkeys, sizeof(mico_bt_local_identity_keys_t), 1);

    if (ret != 0) {
        return MICO_BT_NO_RESOURCES;
    } else {
        return MICO_BT_SUCCESS;
    }
}


/**
 * Function     mico_bt_nvram_access_key_storage_available
 *
 * query if there are available spaces for storing key information
 * for device with @bd_addr with requested @req_size
 *
 * @param[in]  bd_addr : bd_addr of bonded device
 * @param[in]  req_size : requested size to be stored
 *
 * @return      TRUE if there is available space or FALSE
 */
BOOLEAN mico_bt_nvram_access_key_storage_available(mico_bt_device_address_t bd_addr, int req_size)
{
    int index, found = 0;
    mico_bt_nvram_access_entry_t *p_entry;
    char key[32];
    int buflen = sizeof(mico_bt_nvram_access_entry_t) + MICO_BT_DCT_MAX_KEYBLOBS;

    p_entry = (mico_bt_nvram_access_entry_t *)malloc(buflen);
    if (p_entry == NULL) {
        return -1;
    }
    for (index = 0 ; index < MICO_BT_DCT_MAX_DEVICES ; index++) {
        sprintf(key, "bt_bounded_dev%d", index);
        if (aos_kv_get(key, p_entry, buflen) != 0) {
            found = 1;
        }
    }

    free(p_entry);

    if (found == 0) {
        return FALSE;
    } else {
        return TRUE;
    }
}


/**
 * Function     mico_bt_nvram_access_enum_bonded_device_keys
 *
 *   load stored key information by enumeration
 *
 * @param[out]  p_index : index of stored key
 * @param[out]  p_key_entry : key information stored
 *
 * @return      MICO_BT_SUCCESS or ERROR
 */
mico_bt_result_t mico_bt_nvram_access_enum_bonded_device_keys(int8_t *p_index,
                                                              mico_bt_nvram_access_entry_t *p_key_entry, uint8_t entry_max_length)
{
    static int8_t enum_index = 0;
    char key[32];

    *p_index = enum_index;

    sprintf(key, "bt_bounded_dev%d", enum_index);
    enum_index++;
    if (enum_index == MICO_BT_DCT_MAX_KEYBLOBS) {
        enum_index = 0;
    }
    if (aos_kv_get(key, p_key_entry, entry_max_length + sizeof(*p_key_entry)) != 0) {
        return -1;
    }

    return MICO_BT_SUCCESS;
}

/**
 * Function     mico_bt_nvram_access_find_offset
 *
 *
 * @param[in]  *key_bdaddr : bd_addr of bonded device
 * @param[in]  start_offset : start offset where to start searching
 * @param[out] p_offset : stored offset  of bonded device of bd_addr
 *
 * @return      TRUE if found or FALSE
 */
mico_bool_t mico_bt_nvram_access_find_device( mico_bt_device_address_t key_bdaddr )
{
    int index, list_size = 0;
    mico_bt_nvram_access_entry_t *p_entry;
    char key[32];
    int buflen = sizeof(mico_bt_nvram_access_entry_t) + MICO_BT_DCT_MAX_KEYBLOBS;
    mico_bool_t found_keyblobs = FALSE;

    p_entry = (mico_bt_nvram_access_entry_t *)malloc(buflen);
    if (p_entry == NULL) {
        return -1;
    }
    for (index = 0 ; index < MICO_BT_DCT_MAX_DEVICES ; index++) {
        sprintf(key, "bt_bounded_dev%d", index);
        if (aos_kv_get(key, p_entry, buflen) != 0) {
            continue;
        }

        if ( !memcmp(p_entry->bd_addr, key_bdaddr, sizeof(mico_bt_device_address_t) )) { // found
            found_keyblobs = TRUE;
            break;
        }
    }

    free(p_entry);
    return found_keyblobs;
}

#endif /* #if defined(BTM_INTERNAL_LINKKEY_STORAGE_INCLUDED) && (BTM_INTERNAL_LINKKEY_STORAGE_INCLUDED == TRUE) */
