#include <stdio.h>
#include "lega_cm4.h"
#include "lega_common.h"
#include "lega_peripheral_reg.h"
#include "sns_silib.h"
#include "crys_rnd.h"
#include "lega_aes.h"

int lega_aes_init(lega_aes_dev_t *aes_crypt)
{
    //C310 clock enable
#ifdef LEGA_A0V2
    REG_WR(SYS_REG_BASE_C310_CLK, 0x1);
    delay(50);
#endif
    return SaSi_LibInit((CRYS_RND_Context_t *)aes_crypt->p_workspace, (CRYS_RND_WorkBuff_t *)(aes_crypt->p_workspace + sizeof(CRYS_RND_Context_t)));
}

int lega_aes_crypt(lega_aes_dev_t *aes_crypt)
{
    int ret = 0;
    uint32_t block_index;
    uint8_t *dataInBuff;
    uint8_t *dataOutBuff;
    SaSiAesUserContext_t ContextID;
    SaSiAesUserKeyData_t keyData;
    size_t outSize = 0;

    /*In case of non Linux platform set data pointers to test's vectors */
    dataInBuff = aes_crypt->input_data;
    dataOutBuff = aes_crypt->output_data;

    /*Call non-integrated APIs - first SaSi_AesInit*/
    ret = SaSi_AesInit(&ContextID, aes_crypt->enc_dec_mode, aes_crypt->operation_mode, SASI_AES_PADDING_NONE);
    if (ret != SA_SILIB_RET_OK)
    {
        return ret;
    }

    if ((aes_crypt->operation_mode == SASI_AES_MODE_CBC)
        || (aes_crypt->operation_mode == SASI_AES_MODE_CTR)
        || (aes_crypt->operation_mode == SASI_AES_MODE_CBC_MAC))
    {
        ret = SaSi_AesSetIv(&ContextID, aes_crypt->iv_counter);
        if (ret != SA_SILIB_RET_OK)
        {
           return ret;
        }
    }

    keyData.pKey = aes_crypt->key_data;
    keyData.keySize = aes_crypt->key_size;
    ret = SaSi_AesSetKey(&ContextID, SASI_AES_USER_KEY, &keyData, sizeof(keyData));
    if (ret != SA_SILIB_RET_OK)
    {
        return ret;
    }
    /*Call CRYS_AES_Block for each 16 byte block data of the input buffer*/
    for (block_index = 0; block_index<aes_crypt->data_size/SASI_AES_BLOCK_SIZE_IN_BYTES-1; block_index++)
    {
        ret = SaSi_AesBlock(&ContextID,
            dataInBuff + (block_index*SASI_AES_BLOCK_SIZE_IN_BYTES),
            SASI_AES_BLOCK_SIZE_IN_BYTES,
            dataOutBuff + (block_index*SASI_AES_BLOCK_SIZE_IN_BYTES));

        if (ret != SA_SILIB_RET_OK)
        {
            return ret;
        }
    }
    outSize = aes_crypt->data_size - (block_index*SASI_AES_BLOCK_SIZE_IN_BYTES);
    /*Call CRYS_AES_Finish for the last block (the size may be 0)*/
    if (aes_crypt->operation_mode == SASI_AES_MODE_XCBC_MAC ||
        aes_crypt->operation_mode == SASI_AES_MODE_CMAC)
    {
        /*For XCBC_MAC and CMAC modes we should pass to SaSi_AesFinish the begining of the output buffer*/
        ret = SaSi_AesFinish(&ContextID,
            outSize,
            dataInBuff + (block_index*SASI_AES_BLOCK_SIZE_IN_BYTES),
            outSize,
            dataOutBuff,
            &outSize);
    }
    else
    {
        /*For the rest of the modes we should pass to SaSi_AesFinish the relevant offset of output buffer*/
        ret = SaSi_AesFinish(&ContextID,
            outSize,
            dataInBuff + (block_index*SASI_AES_BLOCK_SIZE_IN_BYTES),
            outSize,
            dataOutBuff + (block_index*SASI_AES_BLOCK_SIZE_IN_BYTES),
            &outSize);
    }

    if (ret != SA_SILIB_RET_OK)
    {
        return ret;
    }
    return ret;
}

int lega_aes_finalize(lega_aes_dev_t *aes_crypt)
{
    return SaSi_LibFini((CRYS_RND_Context_t *)aes_crypt->p_workspace);
}

