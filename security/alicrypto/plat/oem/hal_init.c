/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/

#include "ali_crypto.h"
#include "impl_crypto.h"
#include "hal_crypto.h"
#include "sal_crypto.h"



ali_crypto_result hal_crypto_init()
{
    return ALI_CRYPTO_SUCCESS;
}

void hal_crypto_cleanup()
{

}


//AES start
#ifdef ALI_CRYPTO_AES
ALI_CRYPTO_AES_IMPL_START(hal)
#ifdef ALI_CRYPTO_AES_ECB
ALI_CRYPTO_AES_IMPL(AES_ECB,    
        sal_aes_get_ctx_size, 
        sal_aes_init, 
        sal_aes_process, 
        sal_aes_finish, 
        sal_aes_reset)
#endif
#ifdef ALI_CRYPTO_AES_CBC
ALI_CRYPTO_AES_IMPL(AES_CBC,    
        sal_aes_get_ctx_size, 
        sal_aes_init, 
        sal_aes_process, 
        sal_aes_finish, 
        sal_aes_reset)
#endif
#ifdef ALI_CRYPTO_AES_CTR
ALI_CRYPTO_AES_IMPL(AES_CTR,    
        sal_aes_get_ctx_size, 
        sal_aes_init, 
        sal_aes_process, 
        sal_aes_finish, 
        sal_aes_reset)
#endif
#ifdef ALI_CRYPTO_AES_CFB
ALI_CRYPTO_AES_IMPL(AES_CFB8,   
        sal_aes_get_ctx_size, 
        sal_aes_init, 
        sal_aes_process, 
        sal_aes_finish, 
        sal_aes_reset)
        
ALI_CRYPTO_AES_IMPL(AES_CFB128, 
        sal_aes_get_ctx_size, 
        sal_aes_init, 
        sal_aes_process, 
        sal_aes_finish, 
        sal_aes_reset)
#endif
ALI_CRYPTO_AES_IMPL_END()
ALI_CRYPTO_AES_GET_IMPL(hal)
#endif
//AES end


//SM4 start
#ifdef ALI_CRYPTO_SM4
ALI_CRYPTO_SM4_IMPL_START(hal)
#ifdef ALI_CRYPTO_SM4_ECB
ALI_CRYPTO_SM4_IMPL(SM4_ECB,
        sal_sm4_get_ctx_size,
        sal_sm4_init,
        sal_sm4_process,
        sal_sm4_finish,
        sal_sm4_reset)
#endif
#ifdef ALI_CRYPTO_SM4_CBC
ALI_CRYPTO_SM4_IMPL(SM4_CBC,
        sal_sm4_get_ctx_size,
        sal_sm4_init,
        sal_sm4_process,
        sal_sm4_finish,
        sal_sm4_reset)
#endif
ALI_CRYPTO_SM4_IMPL_END()
ALI_CRYPTO_SM4_GET_IMPL(hal)
#endif
//SM4 end

//HASH start
#ifdef ALI_CRYPTO_HASH
ALI_CRYPTO_HASH_IMPL_START(hal)
#ifdef ALI_CRYPTO_HASH_SHA1
ALI_CRYPTO_HASH_IMPL(SHA1,   
        sal_hash_get_ctx_size, 
        sal_hash_init, 
        sal_hash_update, 
        sal_hash_final, 
        sal_hash_reset)
#endif
#ifdef ALI_CRYPTO_HASH_SHA256
ALI_CRYPTO_HASH_IMPL(SHA224, 
        sal_hash_get_ctx_size, 
        sal_hash_init, 
        sal_hash_update, 
        sal_hash_final, 
        sal_hash_reset)
ALI_CRYPTO_HASH_IMPL(SHA256, 
        sal_hash_get_ctx_size, 
        sal_hash_init, 
        sal_hash_update, 
        sal_hash_final, 
        sal_hash_reset)
#endif
#ifdef ALI_CRYPTO_HASH_MD5
ALI_CRYPTO_HASH_IMPL(MD5,    
        sal_hash_get_ctx_size, 
        sal_hash_init, 
        sal_hash_update, 
        sal_hash_final, 
        sal_hash_reset)
#endif
#ifdef ALI_CRYPTO_HASH_SM3
ALI_CRYPTO_HASH_IMPL(SM3,
        sal_hash_get_ctx_size,
        sal_hash_init,
        sal_hash_update,
        sal_hash_final,
        sal_hash_reset)
#endif
ALI_CRYPTO_HASH_IMPL_END()
ALI_CRYPTO_HASH_GET_IMPL(hal)
#endif
//HASH end

//HMAC start
#ifdef ALI_CRYPTO_HMAC
ALI_CRYPTO_HMAC_IMPL_START(hal)
#ifdef ALI_CRYPTO_HASH_SHA1
ALI_CRYPTO_HMAC_IMPL(SHA1,   
        sal_hmac_get_ctx_size, 
        sal_hmac_init, 
        sal_hmac_update, 
        sal_hmac_final, 
        sal_hmac_reset)
#endif
#ifdef ALI_CRYPTO_HASH_SHA256
ALI_CRYPTO_HMAC_IMPL(SHA224, 
        sal_hmac_get_ctx_size, 
        sal_hmac_init, 
        sal_hmac_update, 
        sal_hmac_final, 
        sal_hmac_reset)
ALI_CRYPTO_HMAC_IMPL(SHA256, 
        sal_hmac_get_ctx_size, 
        sal_hmac_init, 
        sal_hmac_update, 
        sal_hmac_final, 
        sal_hmac_reset)
#endif
#ifdef ALI_CRYPTO_HASH_MD5
ALI_CRYPTO_HMAC_IMPL(MD5,    
        sal_hmac_get_ctx_size, 
        sal_hmac_init, 
        sal_hmac_update, 
        sal_hmac_final, 
        sal_hmac_reset)
#endif
#ifdef ALI_CRYPTO_HASH_SM3
ALI_CRYPTO_HMAC_IMPL(SM3,
        sal_hmac_get_ctx_size,
        sal_hmac_init,
        sal_hmac_update,
        sal_hmac_final,
        sal_hmac_reset)
#endif
ALI_CRYPTO_HMAC_IMPL_END()
ALI_CRYPTO_HMAC_GET_IMPL(hal)
#endif
//HMAC end

//RSA start
#ifdef ALI_CRYPTO_RSA
ALI_CRYPTO_RSA_IMPL_START(hal)
ALI_CRYPTO_RSA_IMPL(
        sal_rsa_get_keypair_size,
        sal_rsa_get_pubkey_size,    
        sal_rsa_init_keypair,       
        sal_rsa_init_pubkey,        
        sal_rsa_get_key_attr,       
        sal_rsa_gen_keypair,        
        sal_rsa_public_encrypt,     
        sal_rsa_private_decrypt,    
        sal_rsa_sign,               
        sal_rsa_verify)
ALI_CRYPTO_RSA_IMPL_END()
ALI_CRYPTO_RSA_GET_IMPL(hal)
#endif
//RSA end

//SM2 start
#ifdef ALI_CRYPTO_SM2
ALI_CRYPTO_SM2_IMPL_START(hal)
ALI_CRYPTO_SM2_IMPL(
        sal_sm2_get_keypair_size,
        sal_sm2_get_pubkey_size,
        sal_sm2_init_keypair,
        sal_sm2_init_pubkey,
        sal_sm2_gen_keypair,
        sal_sm2_sign,
        sal_sm2_verify,
        sal_sm2_encrypt,
        sal_sm2_decrypt,
        sal_sm2dh_derive_secret)
ALI_CRYPTO_SM2_IMPL_END()
ALI_CRYPTO_SM2_GET_IMPL(hal)
#endif
//SM2 end

//RAND start
ALI_CRYPTO_RAND_IMPL_START(hal)
ALI_CRYPTO_RAND_IMPL(sal_rand_gen, sal_seed)
ALI_CRYPTO_RAND_IMPL_END()
ALI_CRYPTO_RAND_GET_IMPL(hal)
//RAND end

