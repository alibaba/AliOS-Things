/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
//#include <aos/aos.h>
#include <aos/kv.h>
#include "aos/hal/flash.h"
#include <hal/hal.h>
#include "crc16.h"
#include "bt_crypto.h"

#include "genie_service.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_FLASH)
#include "common/log.h"

static uint8_t g_enc_key[16] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static genie_storage_status_e _genie_storage_encrypt(uint8_t *p_buff, uint16_t size)
{
    uint8_t data_temp[16];
    uint8_t enc_temp[16];
    uint16_t i = 0;

    while (i < (size >> 4))
    {
        memcpy(data_temp, p_buff + (i << 4), 16);
        BT_DBG("data: %s", bt_hex(data_temp, 16));
        bt_mesh_aes_encrypt(g_enc_key, data_temp, enc_temp);
        memcpy(p_buff + (i << 4), enc_temp, 16);
        i++;
    }

    return 0;
}

static genie_storage_status_e _genie_storage_decrypt(uint8_t *p_buff, uint16_t size)
{
    uint8_t data_temp[16];
    uint8_t dec_temp[16];
    uint16_t i = 0;

    while (i < (size >> 4))
    {
        memcpy(data_temp, p_buff + (i << 4), 16);
        bt_mesh_aes_decrypt(g_enc_key, data_temp, dec_temp);
        BT_DBG("dec: %s", bt_hex(dec_temp, 16));
        memcpy(p_buff + (i << 4), dec_temp, 16);
        i++;
    }

    return 0;
}

#ifdef PROJECT_SECRET_KEY
static void _ginie_flash_get_enc_key(uint8_t project_key[16])
{
    BT_DBG("project key: %s", bt_hex(project_key, 16));
    //TODO:need find suitable key
    bt_mesh_aes_encrypt(project_key, project_key, g_enc_key);
    BT_DBG("enc key: %s", bt_hex(g_enc_key, 16));
}
#endif

genie_storage_status_e genie_storage_init(void)
{
    static uint8_t flash_already_inited = 0;
    genie_storage_status_e ret = GENIE_STORAGE_SUCCESS;

    if (flash_already_inited == 1)
    {
        return GENIE_STORAGE_SUCCESS;
    }

    flash_already_inited = 1;

#ifdef PROJECT_SECRET_KEY
    char key_char[] = PROJECT_SECRET_KEY;
    uint8_t prj_key[16];

    memset(prj_key, 0, 16);
    stringtohex(key_char, prj_key, 16);

    _ginie_flash_get_enc_key(prj_key);
#endif

    return ret;
}

static int genie_kv_read(int key_index, uint8_t *value, uint16_t value_size)
{
    int read_len = 0;
    char key[10] = {0};

    read_len = value_size;
    snprintf(key, sizeof(key), "%s_%d", "key", key_index);

    return aos_kv_get(key, value, &read_len);
}

static genie_storage_status_e _genie_storage_read(uint16_t key_index, uint8_t *p_buff, uint16_t size)
{
    int retval = 0;

    retval = genie_kv_read(key_index, p_buff, size);
    if (retval != 0)
    {
        //GENIE_LOG_ERR("func:%s index(%d) failed(%d)\n", __func__, key_index, retval);
        return GENIE_STORAGE_READ_FAIL;
    }
    else
    {
        return GENIE_STORAGE_SUCCESS;
    }
}

static int genie_kv_write(int key_index, uint8_t *value, uint16_t value_size)
{
    char key[10] = {0};

    snprintf(key, sizeof(key), "%s_%d", "key", key_index);

    return aos_kv_set(key, value, (int)value_size, 0);
}

static genie_storage_status_e _genie_storage_write(uint16_t key_index, uint8_t *p_buff, uint16_t size)
{
    int retval = 0;

    if (p_buff == NULL || 0 == size)
    {
        GENIE_LOG_ERR("p_buff is null!!!");
        return GENIE_STORAGE_DATA_INVALID;
    }

    retval = genie_kv_write(key_index, p_buff, size);
    if (retval != 0)
    {
        //GENIE_LOG_ERR("func:%s index(%d) failed(%d)\n", __func__, key_index, retval);
        return GENIE_STORAGE_WRITE_FAIL;
    }

    return GENIE_STORAGE_SUCCESS;
}

static uint16_t _genie_storage_get_reliable_size(uint16_t data_size)
{
    if ((data_size & 0x0F) == 0)
    {
        return data_size;
    }
    else
    {
        return ((data_size >> 4) + 1) << 4;
    }
}

genie_storage_status_e genie_storage_read_reliable(uint16_t index, uint8_t *p_data, uint16_t data_size)
{
    genie_storage_status_e ret = GENIE_STORAGE_SUCCESS;
    uint16_t buff_size = _genie_storage_get_reliable_size(data_size);
    uint8_t *p_buff = NULL;

    if (p_data == NULL)
    {
        GENIE_LOG_ERR("p_data is null!!!");
        return GENIE_STORAGE_DATA_INVALID;
    }

    p_buff = hal_malloc(buff_size);
    if (p_buff == NULL)
    {
        GENIE_LOG_ERR("p_buff is null!!!");
        return GENIE_STORAGE_DATA_INVALID;
    }

    if (GFI_MESH_TRITUPLE == index)
    {
        if (0 != hal_flash_read_triples(p_buff, buff_size))
        {
            GENIE_LOG_ERR("read triples fail");
            ret = GENIE_STORAGE_READ_FAIL;
        }
    }
    else
    {
        ret = _genie_storage_read(index, p_buff, buff_size);
    }

    if (ret == 0)
    {
        _genie_storage_decrypt(p_buff, buff_size);
        memcpy(p_data, p_buff, data_size);
    }
    hal_free(p_buff);

    return ret;
}

genie_storage_status_e genie_storage_write_reliable(uint16_t index, uint8_t *p_data, uint16_t data_size)
{
    uint8_t *p_buff = NULL;
    genie_storage_status_e ret = GENIE_STORAGE_SUCCESS;
    uint16_t buff_size = _genie_storage_get_reliable_size(data_size);

    if (p_data == NULL)
    {
        GENIE_LOG_ERR("p_data is null!!!");
        return GENIE_STORAGE_DATA_INVALID;
    }

    p_buff = hal_malloc(buff_size);
    if (p_buff == NULL)
    {
        GENIE_LOG_ERR("p_buff is null!!!");
        return GENIE_STORAGE_DATA_INVALID;
    }

    memset(p_buff, 0, buff_size);
    memcpy(p_buff, p_data, data_size);
    GENIE_LOG_DBG("data:%d buff:%d", data_size, buff_size);
    _genie_storage_encrypt(p_buff, buff_size);

    if (GFI_MESH_TRITUPLE == index)
    {
        if (0 != hal_flash_write_triples(p_buff, buff_size))
        {
            GENIE_LOG_ERR("write triples fail");
            ret = GENIE_STORAGE_WRITE_FAIL;
        }
    }
    else
    {
        ret = _genie_storage_write(index, p_buff, buff_size);
    }

    hal_free(p_buff);

    return ret;
}

genie_storage_status_e genie_storage_delete_reliable(uint16_t index)
{
    char key[10] = {0};

    snprintf(key, sizeof(key), "%s_%d", "key", index);

    return aos_kv_del(key);
}

genie_storage_status_e genie_storage_read_userdata(uint16_t index, uint8_t *p_buff, uint16_t size)
{
    return _genie_storage_read(index, p_buff, size);
}

genie_storage_status_e genie_storage_write_userdata(uint16_t index, uint8_t *p_buff, uint16_t size)
{
    return _genie_storage_write(index, p_buff, size);
}

genie_storage_status_e genie_storage_delete_userdata(uint16_t index)
{
    char key[10] = {0};

    snprintf(key, sizeof(key), "%s_%d", "key", index);

    return aos_kv_del(key);
}

genie_storage_status_e genie_storage_write_seq(uint32_t *p_seq, bool force_write)
{
    int ret = -1;
    uint32_t seq_write = 0;

    /*Don't save seq number too frequently for save energy*/
    /*Save seq number 1\2\3 for provision*/
    if ((force_write == false) && (*p_seq > 0x03) && (*p_seq % GENIE_SEQ_SAVE_INTERVAL) != 0)
    {
        //GENIE_LOG_INFO("no save seq(%d)", *p_seq);
        return GENIE_STORAGE_SUCCESS;
    }

    seq_write = (*p_seq & 0x00FFFFFF) | (GENIE_SEQ_MAGIC_NUMBER << 24);
    ret = aos_kv_set(GENIE_KV_SEQ_KEY, &seq_write, sizeof(uint32_t), 0);
    if (ret != 0)
    {
        GENIE_LOG_ERR("func:%s (%d)failed\n", __func__, ret);
        return GENIE_STORAGE_WRITE_FAIL;
    }
    else
    {
        return GENIE_STORAGE_SUCCESS;
    }
}

genie_storage_status_e genie_storage_read_seq(uint32_t *p_seq)
{
    int ret = -1;
    uint32_t seq_read;
    int seq_len = sizeof(uint32_t);

    ret = aos_kv_get(GENIE_KV_SEQ_KEY, &seq_read, &seq_len);
    if ((seq_read >> 24) != GENIE_SEQ_MAGIC_NUMBER)
    {
        return GENIE_STORAGE_DATA_INVALID;
    }

    *p_seq = seq_read & 0x00FFFFFF;
    *p_seq += GENIE_SEQ_SAVE_INTERVAL;

    genie_storage_write_seq(p_seq, true); //update seq in kv

    return ret;
}

genie_storage_status_e genie_storage_delete_seq(void)
{
    int ret = -1;

    ret = aos_kv_del(GENIE_KV_SEQ_KEY);
    if (ret != 0)
    {
        GENIE_LOG_ERR("func:%s (%d)failed\n", __func__, ret);
        return GENIE_STORAGE_DELETE_FAIL;
    }
    else
    {
        return GENIE_STORAGE_SUCCESS;
    }
}

genie_storage_status_e genie_storage_write_sub(uint16_t *p_sub)
{
    uint16_t group_addr_crc = 0;
    uint16_t crc_magic = 0xA5A6;
    uint8_t data[CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2 + 2] = {0};

    memcpy(data, p_sub, CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2);
    group_addr_crc = util_crc16_compute(data, CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2, &crc_magic);
    memcpy(data + CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2, (unsigned char *)&group_addr_crc, 2);

    return hal_flash_write_group_addr(data, CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2 + 2);
}

genie_storage_status_e genie_storage_read_sub(uint16_t *p_sub)
{
    uint16_t group_addr_crc = 0;
    uint16_t group_addr_read_crc = 0;
    uint16_t crc_magic = 0xA5A6;
    uint8_t *p_crc = NULL;
    uint8_t data[CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2 + 2] = {0};

    if (0 != hal_flash_read_group_addr(data, CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2 + 2))
    {
        GENIE_LOG_ERR("read group addr fail");
        return GENIE_STORAGE_READ_FAIL;
    }

    group_addr_crc = util_crc16_compute(data, CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2, &crc_magic);

    p_crc = data + CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2;
    group_addr_read_crc = p_crc[0] | (p_crc[1] << 8);

    if (group_addr_read_crc != group_addr_crc)
    {
        return GENIE_STORAGE_READ_FAIL;
    }

    memcpy(p_sub, data, CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2);

    return GENIE_STORAGE_SUCCESS;
}

genie_storage_status_e genie_storage_write_para(struct bt_mesh_net *mesh_data)
{
    return genie_storage_write_reliable(GFI_MESH_PARA, (uint8_t *)mesh_data, sizeof(struct bt_mesh_net));
}

genie_storage_status_e genie_storage_read_para(struct bt_mesh_net *mesh_data)
{
    return genie_storage_read_reliable(GFI_MESH_PARA, (uint8_t *)mesh_data, sizeof(struct bt_mesh_net));
}

genie_storage_status_e genie_storage_write_devkey(uint8_t *p_devkey)
{
    return genie_storage_write_reliable(GFI_MESH_DEVKEY, p_devkey, 16);
}

genie_storage_status_e genie_storage_read_devkey(uint8_t *p_devkey)
{
    return genie_storage_read_reliable(GFI_MESH_DEVKEY, p_devkey, 16);
}

genie_storage_status_e genie_storage_write_netkey(mesh_netkey_para_t *p_netkey)
{
    return genie_storage_write_reliable(GFI_MESH_NETKEY, (uint8_t *)p_netkey, sizeof(mesh_netkey_para_t));
}

genie_storage_status_e genie_storage_read_netkey(mesh_netkey_para_t *p_netkey)
{
    return genie_storage_read_reliable(GFI_MESH_NETKEY, (uint8_t *)p_netkey, sizeof(mesh_netkey_para_t));
}

genie_storage_status_e genie_storage_write_appkey(mesh_appkey_para_t *p_appkey)
{
    return genie_storage_write_reliable(GFI_MESH_APPKEY, (uint8_t *)p_appkey, sizeof(mesh_appkey_para_t));
}

genie_storage_status_e genie_storage_read_appkey(mesh_appkey_para_t *p_appkey)
{
    return genie_storage_read_reliable(GFI_MESH_APPKEY, (uint8_t *)p_appkey, sizeof(mesh_appkey_para_t));
}

genie_storage_status_e genie_storage_write_addr(uint16_t *p_addr)
{
    BT_DBG("0x%04x", *p_addr);

    return genie_storage_write_reliable(GFI_MESH_SADDR, (uint8_t *)p_addr, 2);
}

genie_storage_status_e genie_storage_read_addr(uint16_t *p_addr)
{
    return genie_storage_read_reliable(GFI_MESH_SADDR, (uint8_t *)p_addr, 2);
}

genie_storage_status_e genie_storage_write_hb(mesh_hb_para_t *p_para)
{
    return genie_storage_write_reliable(GFI_MESH_HB, (uint8_t *)p_para, sizeof(mesh_hb_para_t));
}

genie_storage_status_e genie_storage_read_hb(mesh_hb_para_t *p_para)
{
    return genie_storage_read_reliable(GFI_MESH_HB, (uint8_t *)p_para, sizeof(mesh_hb_para_t));
}

#ifdef CONFIG_BT_MESH_CTRL_RELAY
genie_storage_status_e genie_storage_write_ctrl_relay(mesh_ctrl_relay_para_t *p_para)
{
    return genie_storage_write_reliable(GFI_MESH_CTRL_RELAY, (uint8_t *)p_para, sizeof(mesh_ctrl_relay_para_t));
}

genie_storage_status_e genie_storage_read_ctrl_relay(mesh_ctrl_relay_para_t *p_para)
{
    return genie_storage_read_reliable(GFI_MESH_CTRL_RELAY, (uint8_t *)p_para, sizeof(mesh_ctrl_relay_para_t));
}
#endif

genie_storage_status_e genie_storage_reset_system(void)
{
    int ret = 0;

    ret = aos_kv_reset();
    if (ret != 0)
    {
        GENIE_LOG_ERR("func:%s (%d)failed", __func__, ret);
        return GENIE_STORAGE_EARSE_FAIL;
    }

    return GENIE_STORAGE_SUCCESS;
}
