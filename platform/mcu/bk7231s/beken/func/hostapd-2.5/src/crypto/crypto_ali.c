/*
 * WPA Supplicant / wrapper functions for libalicrypto
 *
 */

#include "includes.h"

#include "common.h"
#include "crypto.h"
#include "ali_crypto.h"

void des_encrypt(const u8 *clear, const u8 *key, u8 *cypher)
{
	size_t size;
    void *context;

    ali_des_get_ctx_size(DES_ECB, &size);
    context = os_malloc(size);
    if (context == NULL)
        return;
    ali_des_init(DES_ECB, true, key, 64,
                 NULL, context);
    ali_des_process(clear, cypher, 8, context);
    os_free(context);
}


int md5_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
	size_t size;
    void *context;
    int i;

    ali_hash_get_ctx_size(MD5, &size);

    context = os_malloc(size);
    if (context == NULL) {
        return -1;
    }
    os_memset(context, 0, size);

    ali_hash_init(MD5, context);
    for (i = 0; i < num_elem; i++) {
	    ali_hash_update(addr[i], len[i], context);
    }
    ali_hash_final(mac, context);
    
    os_free(context);
	return 0;
}


int sha1_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
	size_t size;
    void *context;
    int i;

    ali_hash_get_ctx_size(SHA1, &size);

    context = os_malloc(size);
    if (context == NULL) {
        return -1;
    }
    os_memset(context, 0, size);

    ali_hash_init(SHA1, context);
    for (i = 0; i < num_elem; i++) {
	    ali_hash_update(addr[i], len[i], context);
    }
    ali_hash_final(mac, context);
    
    os_free(context);
	return 0;
}

int hmac_sha1_vector(const u8 *key, size_t key_len, size_t num_elem,
		     const u8 *addr[], const size_t *len, u8 *mac)
{
	size_t size;
    void *context;
    int i;
    
    ali_hmac_get_ctx_size(SHA1, &size);
    context = os_malloc(size);
    if (context == NULL) {
        return -1;
    }
    os_memset(context, 0, size);

    ali_hmac_init(SHA1, key, key_len, context);
    for (i = 0; i < num_elem; i++) {
	    ali_hmac_update(addr[i], len[i], context);
    }
    ali_hmac_final(mac, context);
    
    os_free(context);

    return 0;
}


int hmac_sha1(const u8 *key, size_t key_len, const u8 *data, size_t data_len,
	       u8 *mac)
{
	return hmac_sha1_vector(key, key_len, 1, &data, &data_len, mac);
}

int hmac_md5_vector(const u8 *key, size_t key_len, size_t num_elem,
		     const u8 *addr[], const size_t *len, u8 *mac)
{
	size_t size;
    void *context;
    int i;

    ali_hmac_get_ctx_size(MD5, &size);

    context = os_malloc(size);
    if (context == NULL) {
        return -1;
    }
    os_memset(context, 0, size);

    ali_hmac_init(MD5, key, key_len, context);
    for (i = 0; i < num_elem; i++) {
	    ali_hmac_update(addr[i], len[i], context);
    }
    ali_hmac_final(mac, context);
    
    os_free(context);
    return 0;
}


int hmac_md5(const u8 *key, size_t key_len, const u8 *data, size_t data_len,
	       u8 *mac)
{
	return hmac_md5_vector(key, key_len, 1, &data, &data_len, mac);
}

void * aes_encrypt_init(const u8 *key, size_t len)
{
    size_t size;
    void *context;
    int ret;

    ali_aes_get_ctx_size(AES_ECB, &size);

    context = os_malloc(size);
    if (context == NULL) {
        return NULL;
    }
    os_memset(context, 0, size);
    
	ret = ali_aes_init(AES_ECB, true, key, NULL,
                 len, NULL, context);

    if (ret != 0) {
        os_free(context);
        return NULL;
    }
	return context;
}

void aes_encrypt(void *ctx, const u8 *plain, u8 *crypt)
{
	ali_aes_process(plain, crypt, 16, ctx);
}

void aes_encrypt_deinit(void *ctx)
{
	os_free(ctx);
}

void * aes_decrypt_init(const u8 *key, size_t len)
{
	size_t size;
    void *context;
    int ret;

    ali_aes_get_ctx_size(AES_ECB, &size);

    context = os_malloc(size);
    if (context == NULL) {
        return NULL;
    }
    os_memset(context, 0, size);
    
	ret = ali_aes_init(AES_ECB, false, key, NULL,
                 len, NULL, context);

    if (ret != 0) {
        os_free(context);
        return NULL;
    }
	return context;
}

void aes_decrypt(void *ctx, const u8 *crypt, u8 *plain)
{
	ali_aes_process(crypt, plain, 16, ctx);
}

void aes_decrypt_deinit(void *ctx)
{
	os_free(ctx);
}


