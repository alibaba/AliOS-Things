/**
 * Copyright (C) 2016 The YunOS Project. All rights reserved.
 **/

#include "ali_crypto.h"

#if CONFIG_CRYPT_MBED
#include "mbed_crypto.h"
#endif

#if CONFIG_CRYPT_MBED
ali_crypto_result mbed_crypto_init(void)
{
    ali_crypto_result ret = ALI_CRYPTO_SUCCESS;
#if defined(MBEDTLS_THREADING_ALT)
     //mbedtls_threading_set_alt(OSA_mutex_init, OSA_mutex_free, OSA_mutex_lock,
     //                           OSA_mutex_unlock);
#endif
    /* TODO */
    return ret;
}

void mbed_crypto_cleanup(void)
{
    /* TODO */
#if defined(MBEDTLS_THREADING_ALT)
    //mbedtls_threading_free_alt();
#endif
    return;
}
#endif

ali_crypto_result ali_crypto_init(void)
{
    ali_crypto_result ret = ALI_CRYPTO_SUCCESS;

#if CONFIG_CRYPT_MBED
    ret = mbed_crypto_init();
#endif

    return ret;
}
void ali_crypto_cleanup(void)
{
#if CONFIG_CRYPT_MBED
    mbed_crypto_cleanup();
#endif

    return;
}

