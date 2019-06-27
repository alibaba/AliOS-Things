#ifndef _LEGA_AES_H_
#define _LEGA_AES_H_
#include "ssi_aes_defs.h"
#include "ssi_aes.h"

typedef struct {
    char *p_workspace;
    uint8_t key_data[SASI_AES_KEY_MAX_SIZE_IN_BYTES];
    uint8_t key_size;
    SaSiAesIv_t iv_counter;
    uint8_t *input_data;
    uint8_t *output_data;
    uint16_t data_size;
    SaSiAesOperationMode_t operation_mode;
    SaSiAesEncryptMode_t enc_dec_mode;
    uint32_t output_datasize;
} lega_aes_dev_t;

int lega_aes_init(lega_aes_dev_t *aes_crypt);

int lega_aes_crypt(lega_aes_dev_t *aes_crypt);

int lega_aes_finalize(lega_aes_dev_t *aes_crypt);

#endif //_LEGA_AES_H_