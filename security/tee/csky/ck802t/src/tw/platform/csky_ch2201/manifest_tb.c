#include "tee_types.h"
#include "tee_dbg.h"
#include "tee_trm.h"

extern void uart_print_uint8(unsigned char data);
extern void uart_print_uint32(unsigned int data);
extern void uart_print_string(char *data);

extern uint32_t g_ree_entry;
extern uint32_t g_mft_idx_base;

typedef struct
{
    uint16_t index;
    uint16_t reserve;
    uint32_t image_size;
    uint8_t  image_name[8];
    uint64_t static_addr;
    uint64_t loading_addr;
} image_info;

typedef struct
{
    uint16_t encrypted : 1;
    uint16_t reserve : 15;
} tb_flag;

typedef struct
{
    uint32_t   magic;
    uint16_t   version;
    tb_flag    flag;
    uint16_t   image_num;
    uint16_t   digest_sch;
    uint16_t   signature_sch;
    uint16_t   size;
    image_info images[0];
} manifest_tb;

static int32_t _get_key_meta_info(mft_idx_info_t *mft_idx, uint32_t key_type,
                                  mft_key_meta_t *key_meta)
{
    uint32_t i;

    TEE_ASSERT(mft_idx && key_meta);

    for (i = 0; i < mft_idx->key_num; i++) {
        if (mft_idx->key_metas[i].type == key_type) {
            memcpy(key_meta, &mft_idx->key_metas[i], sizeof(mft_key_meta_t));
            return 0;
        }
    }
    return -1;
}

static int32_t _get_key_addr(mft_pos_info_t *pos_info, uint32_t *key_addr)
{
    uint32_t bank_num;
    uint32_t bit_idx;
    uint32_t byte_idx;

    if (pos_info == NULL || key_addr == NULL) {
        return -1;
    }

    if (pos_info->type == POS_INFO_TYPE_FUSE) {
        bank_num = pos_info->info >> 16;
        bit_idx  = pos_info->info & 0xffff;
        byte_idx = bit_idx >> 3;

        *key_addr = bank_num * PLATFORM_OTP_BANK_SIZE + byte_idx +
                    PLATFORM_OTP_SECURE_SECTOR_START;
    } else {
        return -1;
    }

    return 0;
}

static int get_trustroot_pubkey(manifest_tb *tb, char *name,
                                uint32_t *ree_entry)
{
    uint16_t    image_num = tb->image_num;
    uint32_t    image_off = sizeof(manifest_tb);
    image_info *image_addr;

    while (image_num--) {
        image_addr = (image_info *)((uint32_t)tb + image_off);
        if (strcmp((char *)(image_addr->image_name), name) == 0) {
            *ree_entry = image_addr->static_addr;
            break;
        }
        image_off += sizeof(image_info);
    }

    return 0;
}

int hal_get_ree_entry(void)
{
    int             ret;
    uint32_t        key_addr;
    mft_idx_info_t *mft_idx;
    mft_key_meta_t  key_meta;

    mft_idx = (mft_idx_info_t *)g_mft_idx_base;

    if (mft_idx->magic != MANIFEST_IDX_MAGIC) {
        uart_print_string("_trm_get_key_data: bad mft idx \n");
        return -1;
    }
    ret = _get_key_meta_info(mft_idx, KEY_TYPE_MNF_TB_ADDR, &key_meta);
    if (ret < 0) {
        uart_print_string("_trm_get_key_data: fail to get key meta\n");
        return -1;
    }
    ret = _get_key_addr(&key_meta.pos_info, &key_addr);
    if (ret < 0) {
        uart_print_string("_trm_get_key_data: get key addr fail\n");
        return -1;
    }

    get_trustroot_pubkey((manifest_tb *)(*(uint32_t *)key_addr), "ree",
                         &g_ree_entry);

    return 1;
}
