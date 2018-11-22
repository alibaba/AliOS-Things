#include "rec_verify_fw.h"
#include "nbpatch.h"
#include "rec_md5.h"

typedef struct _fw_info{
    unsigned int   image_magic;
    unsigned int   image_size;
    unsigned char  image_md5_value[16];
    unsigned char  image_reserver[2];
    unsigned short image_crc16;
}rec_fw_info;

static int rec_get_fw_store_md5(unsigned long fw_tag, unsigned int fw_len, unsigned char store_md5_value[16])
{
    int ret = 0;
    unsigned int off_set = 0;
    int real_read_size = 0;
    rec_fw_info fw_info;
    off_set = fw_len - sizeof(rec_fw_info);
    real_read_size = nbpatch_read(fw_tag, (const unsigned char*)&fw_info, off_set, sizeof(rec_fw_info),2);
    if (real_read_size != sizeof(rec_fw_info)) {
        ret = -1;
        rec_printf("rec md5 read flash failed!");
    }
    rec_memcpy(store_md5_value, fw_info.image_md5_value, 16);
    return ret;
}
static int rec_get_fw_md5(unsigned long fw_tag, unsigned int fw_len, unsigned char md5_value[16])
{
    int                   ret     = 0;
    int                   fw_info_size = 0;
    volatile unsigned int off_set = 0;
    unsigned int          read_size    = 0;
    int                   real_read_size = 0;
    unsigned char        *rd_buf       = NULL;
    rec_md5_context      *rec_md5_ctx  = NULL;
    int                   bin_size     = 0;

    fw_info_size = sizeof(rec_fw_info);
    if(fw_len <= fw_info_size) {
        ret = -1;
        rec_printf("recovery fw verify input parameters error!\n");
        return ret;
    }

    rd_buf = rec_malloc(512);
    if (rd_buf == NULL) {
        ret = -1;
        rec_printf("rec verify fw malloc failed\n");
        return ret;
    }
    rec_md5_ctx = rec_malloc(sizeof(rec_md5_context));

    if (rec_md5_ctx == NULL) {
        ret = -1;
        rec_printf("recovery malloc md5 ctx failed!\n");
        rec_free(rd_buf);
        return ret;
    }

    rec_md5_init(rec_md5_ctx);
    rec_md5_starts(rec_md5_ctx);

    off_set = 0;
    bin_size = fw_len - fw_info_size;

    while (off_set < bin_size) {
        (bin_size - off_set >= 512) ? (read_size = 512)
                                    : (read_size = bin_size - off_set);
        real_read_size = nbpatch_read(fw_tag, rd_buf, off_set, read_size,2);
        if (real_read_size < 0) {
            ret = -1;
            rec_printf("rec md5 read flash failed!");
            goto REC_HASH_FAILED;
        }
        off_set += real_read_size;
        rec_md5_update(rec_md5_ctx, (const unsigned char *)rd_buf, real_read_size);
    }
    rec_md5_finish(rec_md5_ctx, md5_value);

REC_HASH_FAILED:
    rec_free(rd_buf);
    rec_free(rec_md5_ctx);
    return ret;
}

int rec_verify_firmware(unsigned long fw_tag, unsigned int fw_len)
{
    unsigned char rec_fw_store_md5[16];
    unsigned char rec_fw_real_md5[16];
    rec_memset(rec_fw_store_md5, 0x00, 16);
    rec_memset(rec_fw_real_md5, 0x00, 16);
    if(rec_get_fw_md5(fw_tag, fw_len, rec_fw_real_md5) < 0) {
        rec_printf("rec get fw md5 failed\n");
        return -1;
    }
    if(rec_get_fw_store_md5(fw_tag, fw_len, rec_fw_store_md5) < 0) {
        rec_printf("rec get fw md5 failed\n");
        return -1;
    }
    if(rec_memcmp(rec_fw_real_md5, rec_fw_store_md5, sizeof(rec_fw_store_md5)) == 0) {
        rec_printf("rec md5 verify successfully\n");
        return 0;
    }
    return -1;

}


