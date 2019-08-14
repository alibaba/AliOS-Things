/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "module_crypto.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "be_common.h"
#include "be_jse_module.h"

#include "mbedtls/aes.h"
#include "mbedtls/md5.h"

#ifdef DEBUG_CRYPTO
#define DEBUG_HEX_DUMP(str, buff, len) hexdump(str, buff, len)
#else
#define DEBUG_HEX_DUMP(str, buff, len)
#endif

/*************************************************************************************
*Function:    module_crypto_md5
*Description: js native addon for CRYPTO.md5
*Called by:   js api
*Input:       CRYPTO.md5(input),
              input is a buffer array which to be hashed
*Output:      return NULL when hash fail , return hashed buffer when successed
**************************************************************************************/

static be_jse_symbol_t *module_crypto_md5()
{
    be_jse_symbol_t *arg0  = NULL;
    be_jse_symbol_t *item  = NULL;
    be_jse_symbol_t *array = NULL;
    char *src;
    char *dest;
#ifdef BE_OS_AOS
    MD5_CTX s_ctx;
#else
    mbedtls_md5_context s_ctx;
#endif

    uint8_t result[16] = {0};
    int i              = 0;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);

    if (!arg0 || !symbol_is_array(arg0)) {
        jse_error("%s arg invalid\n\r", __FUNCTION__);
        goto done;
    }

    int len = get_symbol_array_length(arg0);

    src = jse_calloc(1, len);
    if (src == NULL) goto done;

    for (i = 0; i < len; ++i) {
        item = get_symbol_array_item(arg0, i);
        if (!item || !symbol_is_int(item)) {
            symbol_unlock(item);
            goto done;
        }
        src[i] = (uint8_t)get_symbol_value_int(item);
        symbol_unlock(item);
    }

    DEBUG_HEX_DUMP("md5 src", src, len);

#ifdef BE_OS_AOS
    MD5_Init(&s_ctx);
    MD5_Update(&s_ctx, src, len);
    MD5_Final(result, &s_ctx);
#else
    utils_md5(src, len, result);
#endif

    array = new_symbol(BE_SYM_ARRAY);
    for (i = 0; i < 16; ++i) {
        be_jse_symbol_t *idx = new_int_symbol(result[i]);
        symbol_array_push(array, idx);
        symbol_unlock(idx);
    }

    DEBUG_HEX_DUMP("md5 dest", src, len);
done:
    jse_free(src);
    symbol_unlock(arg0);
    return (array == NULL) ? new_symbol(BE_SYM_NULL) : array;
}

#ifdef BE_OS_AOS

static be_osal_aes_type_t convert_str_to_aes_type(char *str)
{
    if (strcasecmp(str, "ecb") == 0) {
        return BE_AES_ECB;
    } else if (strcasecmp(str, "cbc") == 0) {
        return BE_AES_CBC;
    } else if (strcasecmp(str, "ctr") == 0) {
        return BE_AES_CTR;
    } else if (strcasecmp(str, "cts") == 0) {
        return BE_AES_CTS;
    } else if (strcasecmp(str, "xts") == 0) {
        return BE_AES_XTS;
    } else {
        return BE_AES_MAX_TYPE;
    }
}

static be_osal_aes_padding_t convert_str_to_aes_pad(char *str)
{
    if (strcasecmp(str, "nopad") == 0) {
        return BE_SYM_NOPAD;
    } else if (strcasecmp(str, "pck5") == 0) {
        return BE_SYM_PKCS5_PAD;
    } else if (strcasecmp(str, "pck7") == 0) {
        return BE_SYM_PKCS5_PAD; /* todo:pck7 is the same with pck5 by aes */
    } else if (strcasecmp(str, "zero") == 0) {
        return BE_SYM_ZERO_PAD;
    } else {
        return BE_SYM_MAX_PAD;
    }
}

/*************************************************************************************
*Function:    module_crypto_encrypt
*Description: js native addon for CRYPTO.encrypt
*Called by:   js api
*Input:       CRYPTO.encrypt("aes","cbc","pck5",input),
              input is a object like this:{"key":array, "iv":array,
"content":array} *Output:      return NULL when encrypted fail , return encryped
buffer when successed
**************************************************************************************/

static be_jse_symbol_t *module_crypto_encrypt()
{
    be_jse_symbol_t *arg0  = NULL;
    be_jse_symbol_t *arg1  = NULL;
    be_jse_symbol_t *arg2  = NULL;
    be_jse_symbol_t *arg3  = NULL;
    be_jse_symbol_t *array = NULL;
    be_osal_aes_type_t type;
    be_osal_aes_padding_t padding;
    char crypte_mode[8] = {0};
    char aes_type[8]    = {0};
    char aes_padding[8] = {0};
    be_aes_crypt_in_t aes_input;
    int i      = 0;
    char *key  = NULL;
    char *iv   = NULL;
    char *dest = NULL;

    be_jse_handle_function(0, &arg0, &arg1, &arg2, &arg3);

    if (!arg0 || !symbol_is_string(arg0) || !arg1 || !symbol_is_string(arg1) ||
        !arg2 || !symbol_is_string(arg2) || !arg3 || !symbol_is_object(arg3)) {
        goto done;
    }

    symbol_to_str(arg0, crypte_mode, sizeof(crypte_mode));
    symbol_to_str(arg1, aes_type, sizeof(aes_type));
    symbol_to_str(arg2, aes_padding, sizeof(aes_padding));

    if (strcasecmp(crypte_mode, "aes") != 0) {
        jse_error("not supprted crypte_mode\n\r");
        goto done; /* now just support aes encryption */
    }

    type    = convert_str_to_aes_type(aes_type);
    padding = convert_str_to_aes_pad(aes_padding);

    if ((type == BE_AES_MAX_TYPE) || (padding == BE_SYM_MAX_PAD)) {
        jse_error("not support this type or padding with aes\n\r");
        goto done;
    }

    be_jse_symbol_t *keySymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg3), "key", false));
    be_jse_symbol_t *ivSymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg3), "iv", false));
    be_jse_symbol_t *payloadSymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg3), "content", false));

    char *key_array     = be_array_symbol_to_buffer(keySymbol);
    char *iv_array      = be_array_symbol_to_buffer(ivSymbol);
    char *payload_array = be_array_symbol_to_buffer(payloadSymbol);

    aes_input.iv      = iv_array;
    aes_input.key     = key_array;
    aes_input.key_len = get_symbol_array_length(keySymbol);
    aes_input.packet  = payload_array;
    aes_input.pkt_len = get_symbol_array_length(payloadSymbol);
    aes_input.type    = type;
    aes_input.padding = padding;

    DEBUG_HEX_DUMP("key", aes_input.key, aes_input.key_len);
    DEBUG_HEX_DUMP("iv", aes_input.iv, 16);
    DEBUG_HEX_DUMP("packet", aes_input.packet, aes_input.pkt_len);

    int len = be_osal_aes_encrypt(&aes_input, &dest);

    if (len > 0) {
        DEBUG_HEX_DUMP("encrypted:", dest, len);
        array = new_symbol(BE_SYM_ARRAY);
        for (i = 0; i < len; ++i) {
            be_jse_symbol_t *idx = new_int_symbol(dest[i]);
            symbol_array_push(array, idx);
            symbol_unlock(idx);
        }
    }

done:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    symbol_unlock(arg2);
    symbol_unlock(arg3);
    symbol_unlock(keySymbol);
    symbol_unlock(ivSymbol);
    symbol_unlock(payloadSymbol);
    jse_free(key_array);
    jse_free(iv_array);
    jse_free(payload_array);
    return (array == NULL) ? new_symbol(BE_SYM_NULL) : array;
}

/*************************************************************************************
*Function:    module_crypto_decrypt
*Description: js native addon for CRYPTO.decrypt
*Called by:   js api
*Input:       CRYPTO.decrypt("aes","cbc","pck5",input),
              input is a object like this:{"key":array, "iv":array,
"content":array} *Output:      return NULL when decrypted fail , return decryped
buffer when successed
**************************************************************************************/

static be_jse_symbol_t *module_crypto_decrypt()
{
    be_jse_symbol_t *arg0  = NULL;
    be_jse_symbol_t *arg1  = NULL;
    be_jse_symbol_t *arg2  = NULL;
    be_jse_symbol_t *arg3  = NULL;
    be_jse_symbol_t *array = NULL;
    be_osal_aes_type_t type;
    be_osal_aes_padding_t padding;
    char crypte_mode[8] = {0};
    char aes_type[8]    = {0};
    char aes_padding[8] = {0};
    be_aes_crypt_in_t aes_input;
    int i      = 0;
    char *key  = NULL;
    char *iv   = NULL;
    char *dest = NULL;

    be_jse_handle_function(0, &arg0, &arg1, &arg2, &arg3);

    if (!arg0 || !symbol_is_string(arg0) || !arg1 || !symbol_is_string(arg1) ||
        !arg2 || !symbol_is_string(arg2) || !arg3 || !symbol_is_object(arg3)) {
        goto done;
    }

    symbol_to_str(arg0, crypte_mode, sizeof(crypte_mode));
    symbol_to_str(arg1, aes_type, sizeof(aes_type));
    symbol_to_str(arg2, aes_padding, sizeof(aes_padding));

    if (strcasecmp(crypte_mode, "aes") != 0) {
        jse_error("not supprted crypte_mode\n\r");
        goto done; /* now just support aes encryption */
    }

    type    = convert_str_to_aes_type(aes_type);
    padding = convert_str_to_aes_pad(aes_padding);

    if ((type == BE_AES_MAX_TYPE) || (padding == BE_SYM_MAX_PAD)) {
        jse_warn("not support this type or padding with aes\n\r");
        goto done;
    }

    be_jse_symbol_t *keySymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg3), "key", false));
    be_jse_symbol_t *ivSymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg3), "iv", false));
    be_jse_symbol_t *payloadSymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg3), "content", false));

    char *key_array     = be_array_symbol_to_buffer(keySymbol);
    char *iv_array      = be_array_symbol_to_buffer(ivSymbol);
    char *payload_array = be_array_symbol_to_buffer(payloadSymbol);

    aes_input.iv      = iv_array;
    aes_input.key     = key_array;
    aes_input.key_len = get_symbol_array_length(keySymbol);
    aes_input.packet  = payload_array;
    aes_input.pkt_len = get_symbol_array_length(payloadSymbol);
    aes_input.type    = type;
    aes_input.padding = padding;

    DEBUG_HEX_DUMP("key", aes_input.key, aes_input.key_len);
    DEBUG_HEX_DUMP("iv", aes_input.iv, 16);
    DEBUG_HEX_DUMP("packet", aes_input.packet, aes_input.pkt_len);

    int len = be_osal_aes_decrypt(&aes_input, &dest);

    if (len > 0) {
        DEBUG_HEX_DUMP("decrypted:", dest, len);
        array = new_symbol(BE_SYM_ARRAY);
        for (i = 0; i < len; ++i) {
            be_jse_symbol_t *idx = new_int_symbol(dest[i]);
            symbol_array_push(array, idx);
            symbol_unlock(idx);
        }
    }

done:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    symbol_unlock(arg2);
    symbol_unlock(arg3);
    symbol_unlock(keySymbol);
    symbol_unlock(ivSymbol);
    symbol_unlock(payloadSymbol);
    jse_free(key_array);
    jse_free(iv_array);
    jse_free(payload_array);
    return (array == NULL) ? new_symbol(BE_SYM_NULL) : array;
}

#else

/*************************************************************************************
 * Function:    module_crypto_encrypt
 * Description: js native addon for CRYPTO.encrypt
 * Called by:   js api
 * Input:       CRYPTO.encrypt("aes","cbc","pck5",input),
 *            input is a object like this:{"key":array, "iv":array,
 *"content":array} Output:      return NULL when encrypted fail , return
 *encryped buffer when successed
 **************************************************************************************/

static be_jse_symbol_t *module_crypto_encrypt()
{
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    be_jse_symbol_t *arg2 = NULL;
    be_jse_symbol_t *arg3 = NULL;
    be_jse_symbol_t *array = NULL;
    char crypte_mode[8] = {0};
    char aes_type[8] = {0};
    char aes_padding[8] = {0};
    int i = 0;
    char *key = NULL;
    char *iv = NULL;
    unsigned char *input = NULL;
    unsigned char *dest = NULL;

    be_jse_handle_function(0, &arg0, &arg1, &arg2, &arg3);

    if (!arg0 || !symbol_is_string(arg0) || !arg1 || !symbol_is_string(arg1) ||
        !arg2 || !symbol_is_string(arg2) || !arg3 || !symbol_is_object(arg3)) {
        goto done;
    }

    symbol_to_str(arg0, crypte_mode, sizeof(crypte_mode));
    symbol_to_str(arg1, aes_type, sizeof(aes_type));
    symbol_to_str(arg2, aes_padding, sizeof(aes_padding));

    if (strcasecmp(crypte_mode, "aes") != 0) {
        jse_error("not supprted crypte_mode\n\r");
        goto done; /* now just support aes encryption */
    }

    if (strcasecmp(aes_type, "cbc") != 0) {
        jse_warn("aes type only cbc supported\n");
        goto done;
    }

    be_jse_symbol_t *keySymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg3), "key", false));
    be_jse_symbol_t *ivSymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg3), "iv", false));
    be_jse_symbol_t *payloadSymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg3), "content", false));

    unsigned char *key_array =
        (unsigned char *)be_array_symbol_to_buffer(keySymbol);
    int key_array_len = get_symbol_array_length(keySymbol);
    unsigned char *iv_array =
        (unsigned char *)be_array_symbol_to_buffer(ivSymbol);
    int iv_array_len = get_symbol_array_length(ivSymbol);
    char *payload_array = be_array_symbol_to_buffer(payloadSymbol);
    int payload_array_len = get_symbol_array_length(payloadSymbol);
    int padding_len;
    int payload_len;

    assert(key_array_len == 16);
    assert(iv_array_len == 16);

    DEBUG_HEX_DUMP("key", key_array, key_array_len);
    DEBUG_HEX_DUMP("iv", iv_array, iv_array_len);
    DEBUG_HEX_DUMP("packet", payload_array, payload_array_len);

    /* PKCS#7 padding */
    if (payload_array_len & 15)
        padding_len = 16 - (payload_array_len & 15);
    else
        padding_len = 16;
    payload_len = payload_array_len + padding_len;
    input = (unsigned char *)jse_malloc(payload_len);
    memcpy(input, payload_array, payload_array_len);
    for (i = 0; i < padding_len; i++)
        input[payload_array_len + i] = padding_len;

    /* save the encrypto data */
    dest = (unsigned char *)jse_malloc(payload_len);

    /* using mbedtls */
    mbedtls_aes_context aes;
    mbedtls_aes_setkey_enc(&aes, key_array, 16 * 8);
    int err = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, payload_len,
                                    iv_array, input, dest);

    if (!err) {
        DEBUG_HEX_DUMP("encrypted:", dest, payload_len);
        array = new_symbol(BE_SYM_ARRAY);
        for (i = 0; i < payload_len; ++i) {
            be_jse_symbol_t *idx = new_int_symbol(dest[i]);
            symbol_array_push(array, idx);
            symbol_unlock(idx);
        }
    } else {
        jse_warn("mbedtls_aes_crypt_cbc MBEDTLS_AES_ENCRYPT failed, err: %d\n",
                 err);
    }

done:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    symbol_unlock(arg2);
    symbol_unlock(arg3);
    symbol_unlock(keySymbol);
    symbol_unlock(ivSymbol);
    symbol_unlock(payloadSymbol);
    jse_free(key_array);
    jse_free(iv_array);
    jse_free(payload_array);
    if (input) jse_free(input);
    if (dest) jse_free(dest);
    return (array == NULL) ? new_symbol(BE_SYM_NULL) : array;
}

/*************************************************************************************
 * Function:    module_crypto_decrypt
 * Description: js native addon for CRYPTO.decrypt
 * Called by:   js api
 * Input:       CRYPTO.decrypt("aes","cbc","pck5",input),
 *            input is a object like this:{"key":array, "iv":array,
 *"content":array} Output:      return NULL when decrypted fail , return
 *decryped buffer when successed
 **************************************************************************************/

static be_jse_symbol_t *module_crypto_decrypt()
{
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    be_jse_symbol_t *arg2 = NULL;
    be_jse_symbol_t *arg3 = NULL;
    be_jse_symbol_t *array = NULL;
    char crypte_mode[8] = {0};
    char aes_type[8] = {0};
    char aes_padding[8] = {0};
    int i = 0;
    char *key = NULL;
    char *iv = NULL;
    unsigned char *dest = NULL;

    be_jse_handle_function(0, &arg0, &arg1, &arg2, &arg3);

    if (!arg0 || !symbol_is_string(arg0) || !arg1 || !symbol_is_string(arg1) ||
        !arg2 || !symbol_is_string(arg2) || !arg3 || !symbol_is_object(arg3)) {
        goto done;
    }

    symbol_to_str(arg0, crypte_mode, sizeof(crypte_mode));
    symbol_to_str(arg1, aes_type, sizeof(aes_type));
    symbol_to_str(arg2, aes_padding, sizeof(aes_padding));

    if (strcasecmp(crypte_mode, "aes") != 0) {
        jse_error("not supprted crypte_mode\n");
        goto done; /* now just support aes encryption */
    }

    if (strcasecmp(aes_type, "cbc") != 0) {
        jse_warn("aes type only cbc supported\n");
        goto done;
    }

    be_jse_symbol_t *keySymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg3), "key", false));
    be_jse_symbol_t *ivSymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg3), "iv", false));
    be_jse_symbol_t *payloadSymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg3), "content", false));

    unsigned char *key_array =
        (unsigned char *)be_array_symbol_to_buffer(keySymbol);
    int key_array_len = get_symbol_array_length(keySymbol);
    unsigned char *iv_array =
        (unsigned char *)be_array_symbol_to_buffer(ivSymbol);
    int iv_array_len = get_symbol_array_length(ivSymbol);
    char *payload_array = be_array_symbol_to_buffer(payloadSymbol);
    int payload_array_len = get_symbol_array_length(payloadSymbol);

    assert(key_array_len == 16);
    assert(iv_array_len == 16);
    assert((payload_array_len ^ 16) == 0);

    DEBUG_HEX_DUMP("key", key_array, key_array_len);
    DEBUG_HEX_DUMP("iv", iv_array, iv_array_len);
    DEBUG_HEX_DUMP("packet", payload_array, payload_array_len);

    /* save encrypto data */
    dest = (unsigned char *)jse_malloc(payload_array_len);

    /* using mbedtls */
    mbedtls_aes_context aes;
    mbedtls_aes_setkey_dec(&aes, key_array, 16 * 8);
    int err = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT,
                                    payload_array_len, iv_array,
                                    (const unsigned char *)payload_array, dest);
    if (!err) {
        DEBUG_HEX_DUMP("decrypted:", dest, payload_array_len);
        /* remove the padding data */
        int padding_len = dest[payload_array_len - 1];
        array = new_symbol(BE_SYM_ARRAY);
        for (i = 0; i < payload_array_len - padding_len; ++i) {
            be_jse_symbol_t *idx = new_int_symbol(dest[i]);
            symbol_array_push(array, idx);
            symbol_unlock(idx);
        }
    } else {
        jse_warn("mbedtls_aes_crypt_cbc MBEDTLS_AES_DECRYPT failed, err: %d\n",
                 err);
    }

done:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    symbol_unlock(arg2);
    symbol_unlock(arg3);
    symbol_unlock(keySymbol);
    symbol_unlock(ivSymbol);
    symbol_unlock(payloadSymbol);
    jse_free(key_array);
    jse_free(iv_array);
    jse_free(payload_array);
    if (dest) jse_free(dest);
    return (array == NULL) ? new_symbol(BE_SYM_NULL) : array;
}

#endif

static be_jse_symbol_t *crypto_module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                                be_jse_symbol_t *var,
                                                const char *name)
{
    if (strcmp(name, "md5") == 0) return module_crypto_md5();
    if (strcmp(name, "decrypt") == 0) return module_crypto_decrypt();
    if (strcmp(name, "encrypt") == 0) return module_crypto_encrypt();

    return BE_JSE_FUNC_UNHANDLED;
}

void module_crypto_register(void)
{
    be_jse_module_load("CRYPTO", crypto_module_handle_cb);
}
