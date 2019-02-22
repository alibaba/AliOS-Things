/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/

#include "ali_crypto.h"

#if CONFIG_CRYPT_ALI_ALGO
#include "sal_crypto.h"
#endif



#if CONFIG_CRYPT_ALI_ALGO
static ali_crypto_result ali_crypto_algo_init(void)
{
    ali_crypto_result ret = ALI_CRYPTO_SUCCESS;
#if defined(ALI_ALGO_THREADING_ALT)
    // osa_threading_set_alt(OSA_mutex_init, OSA_mutex_free, OSA_mutex_lock,
    //                           OSA_mutex_unlock);
#endif
    /* TODO */
    return ret;
}

static void ali_crypto_algo_cleanup(void)
{
    /* TODO */
#if defined(ALI_ALGO_THREADING_ALT)
    // osa_threading_free_alt();
#endif
    return;
}
#endif

ali_crypto_result ali_crypto_init(void)
{
    ali_crypto_result ret = ALI_CRYPTO_SUCCESS;

#if CONFIG_CRYPT_ALI_ALGO
    ret = ali_crypto_algo_init();
    if(ALI_CRYPTO_SUCCESS != ret) {
        return ret;
    }
#endif

    return ret;
}
void ali_crypto_cleanup(void)
{
#if CONFIG_CRYPT_ALI_ALGO
    ali_crypto_algo_cleanup();
#endif

    return;
}
