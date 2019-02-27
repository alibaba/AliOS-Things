/**
 * Copyright (C) 2017 The YunOS IoT Project. All rights reserved.
 */

#include "itls/config.h"
#include "itls/ssl.h"
#include "itls/ssl_internal.h"
#include "itls/ssl_auth.h"
#include "itls/debug.h"
#include "itls/platform.h"

#include "id2_client.h"

#if defined(MBEDTLS_SSL_PROTO_ITLS)

#if defined(CONFIG_KEY_OTP_ENABLED)
static uint32_t otp_time = 0;
#endif

static unsigned char *mbedtls_hex_to_str(uint8_t *data, size_t len)
{
    size_t i;
    uint8_t lo, hi;
    unsigned char *str = NULL;

    str = (unsigned char *)mbedtls_calloc(1, 2 * len + 1);
    if (str == NULL) {
        return NULL;
    }

    for (i = 0; i < len; i++) {
        hi = (data[i] >> 4) & 0xF;
        lo = (data[i] >> 0) & 0xF;

        if (hi <= 9) {
            str[2*i] = hi + ('0' - 0);
        } else {
            str[2*i] = hi + ('A' - 10);
        }

        if (lo <= 9) {
            str[2*i + 1] = lo + ('0' - 0);
        } else {
            str[2*i + 1] = lo + ('A' - 10);
        }
    }

    str[2 * len] = '\0';

    return str;
}

int mbedtls_write_auth_extra_ext(
        mbedtls_ssl_context *ssl, unsigned char *buf, size_t *olen)
{
    int ret = 0;
    unsigned char *p = buf;
    const unsigned char *end = ssl->out_msg + MBEDTLS_SSL_MAX_CONTENT_LEN;
    unsigned char *auth_extra = buf + 4;
    size_t auth_extra_len = ssl->conf->auth_extra_len;

    if (end < p || (size_t)(end - p) < 4 + auth_extra_len) {
        SSL_DBG_LOG("buffer too small\n");
        return -1;
    }

    /* if no auth extra data, exit directly */
    if (ssl->conf->auth_extra == NULL || ssl->conf->auth_extra_len == 0) {
        SSL_DBG_LOG("no auth extra data\n");
        *olen = 0;
        return ret;
    }

    *p++ = (unsigned char)((MBEDTLS_TLS_EXT_AUTH_EXTRA >> 8) & 0xFF);
    *p++ = (unsigned char)((MBEDTLS_TLS_EXT_AUTH_EXTRA) & 0xFF);

    *p++ = (unsigned char)((auth_extra_len >> 8) & 0xFF);
    *p++ = (unsigned char)((auth_extra_len) & 0xFF);

    memcpy(auth_extra, ssl->conf->auth_extra, auth_extra_len);

#if defined(MBEDTLS_DEBUG_C)
    MBEDTLS_SSL_DEBUG_BUF(3, "client hello, auth extra", auth_extra, auth_extra_len);
#endif

    *olen = 4 + auth_extra_len;

    return ret;
}

/*
 * struct {
 *     uint32_t key_group;
 *     opaque key_id<0..2^8-1>;
 * } Key_ID_Extension;
 */
int mbedtls_write_key_id_ext(
        mbedtls_ssl_context *ssl, unsigned char *buf, size_t *olen)
{
    int ret = 0;
    unsigned char *p = buf;
    const unsigned char *end = ssl->out_msg + MBEDTLS_SSL_MAX_CONTENT_LEN;
    uint32_t key_group;
    unsigned char *key_id = buf + 4;
    uint32_t key_id_len;
#if defined(CONFIG_KEY_OTP_ENABLED)
    bool is_prov;
#endif

    if (end < p || (size_t)(end - p) < 8) {
        SSL_DBG_LOG("buffer too small\n");
        return -1;
    }

    *p++ = (unsigned char)((MBEDTLS_TLS_EXT_KEY_ID >> 8) & 0xFF);
    *p++ = (unsigned char)((MBEDTLS_TLS_EXT_KEY_ID) & 0xFF);

    key_id_len = (uint32_t)(end - p - 8);

#if defined(MBEDTLS_SSL_PROTO_ITLS_TEST)
    if (ssl->conf->type == ITLS_TEST_VALUE_KEY_GROUP)
    {
        SSL_DBG_LOG("iTLS TEST - Client Hello - Invalid Key Group(0x%x)!!!\n\n", ssl->conf->key_group);

        key_group = ssl->conf->key_group;

        key_id[0] = (unsigned char)( ( key_group       ) & 0xFF );
        key_id[1] = (unsigned char)( ( key_group >> 8  ) & 0xFF );
        key_id[2] = (unsigned char)( ( key_group >> 16 ) & 0xFF );
        key_id[3] = (unsigned char)( ( key_group >> 24 ) & 0xFF );
    }
    else
#endif /* MBEDTLS_SSL_PROTO_ITLS_TEST */
    {
        key_group = MBEDTLS_KEY_GROUP_ALIBABA_ID2;

        key_id[0] = (unsigned char)( ( key_group       ) & 0xFF );
        key_id[1] = (unsigned char)( ( key_group >> 8  ) & 0xFF );
        key_id[2] = (unsigned char)( ( key_group >> 16 ) & 0xFF );
        key_id[3] = (unsigned char)( ( key_group >> 24 ) & 0xFF );
    }

#if defined(MBEDTLS_SSL_PROTO_ITLS_TEST)
    if (ssl->conf->type == ITLS_TEST_DATA_KEY_ID)
    {
         SSL_DBG_LOG("iTLS ABT TEST - Client Hello - Invalid Key ID!!!\n\n");

         memcpy(key_id + 4, ssl->conf->key_id, ssl->conf->key_id_len);
         key_id_len = ssl->conf->key_id_len;
    }
    else
#endif /* MBEDTLS_SSL_PROTO_ITLS_TEST */
    {
#if defined(CONFIG_KEY_OTP_ENABLED)
        ret = id2_client_get_prov_stat(&is_prov);
        if (ret != 0) {
            SSL_DBG_LOG("id2 client get prov stat fail, %d\n", ret);
            return -1;
        }

        if (key_id_len < ID2_ID_LEN) {
            SSL_DBG_LOG("key id short buffer, %d\n", (int)key_id_len);
            return -1;
        } else {
            key_id_len = ID2_ID_LEN;
        }

        ssl->handshake->key_otp = 0;

        /* key is not provisioned, set otp flag to true */
        if (!is_prov) {
            SSL_DBG_LOG("key is not provisioned, need to get key first!\n");
            if (!otp_time++) {
                ssl->handshake->key_otp = 1;
                memset(key_id + 4, 'F', key_id_len);
            } else {
                SSL_DBG_LOG("key provisioning (time:%d) exceed the allowed times!\n", (int)otp_time);
                return -1;
            }
        }
        else
#endif /* CONFIG_KEY_OTP_ENABLED */
        {
            ret = id2_client_get_id(key_id + 4, &key_id_len);
            if (ret != 0) {
                SSL_DBG_LOG("id2 client get id fail, %d\n", ret);
                return -1;
            }
        }
    }

    if (key_id_len > MBEDTLS_KEY_ID_MAX_LEN) {
        SSL_DBG_LOG("bad key id len, %d\n", (int)key_id_len);
        return -1;
    }

    /* add key group length */
    key_id_len += 4;

    *p++ = (unsigned char)((key_id_len >> 8) & 0xFF);
    *p++ = (unsigned char)((key_id_len) & 0xFF);

#if defined(MBEDTLS_DEBUG_C)
    MBEDTLS_SSL_DEBUG_BUF(3, "client hello, key id", key_id, key_id_len);
#endif

    *olen = 4 + key_id_len;

    return ret;
}

int mbedtls_write_auth_code_ext(
        mbedtls_ssl_context *ssl, unsigned char *buf, size_t *olen)
{
    int ret = 0;
    unsigned char *p = buf;
    const unsigned char *end = ssl->out_msg + MBEDTLS_SSL_MAX_CONTENT_LEN;
    unsigned char *auth_code = buf + 4;
    uint32_t auth_code_len;

    if (end < p || (size_t)(end - p) < 4) {
        SSL_DBG_LOG("buffer too small\n");
        return -1;
    }

    *p++ = (unsigned char)((MBEDTLS_TLS_EXT_AUTH_CODE >> 8) & 0xFF);
    *p++ = (unsigned char)((MBEDTLS_TLS_EXT_AUTH_CODE) & 0xFF);

    auth_code_len = (uint32_t)(end - p - 4);

#if defined(CONFIG_KEY_OTP_ENABLED)
    if (ssl->handshake->key_otp) {
        ret = id2_client_get_otp_auth_code(
                  ssl->conf->auth_token, ssl->conf->auth_token_len,
                  auth_code, &auth_code_len);
        if (ret != 0) {
            SSL_DBG_LOG("id2_clien _get_otp_auth_code fail, %d\n", ret);
            return -1;
        }
    }
    else
#endif /* CONFIG_KEY_OTP_ENABLED */
    {
        if (ssl->handshake->challenge != NULL &&
            ssl->handshake->challenge_len != 0) {
            /* challenge len is set to fixed length */
            if (ssl->handshake->challenge_len != 32) {
                SSL_DBG_LOG("bad challenge len: %d", ssl->handshake->challenge_len);
                return -1;
            }

#if defined(MBEDTLS_SSL_PROTO_ITLS_TEST)
            if (ssl->conf->type == ITLS_TEST_DATA_AUTH_CODE)
            {
                SSL_DBG_LOG("iTLS ABT TEST - Client Hello - Invalid Auth Code!!!\n\n");

                auth_code_len = ssl->conf->auth_code_len;
                if ((uint32_t)(end - p) < 4 + auth_code_len) {
                    SSL_DBG_LOG("buffer too small\n");
                    return -1;
                }

                memcpy(auth_code, ssl->conf->auth_code, auth_code_len);
            }
            else
#endif /* MBEDTLS_SSL_PROTO_ITLS_TEST */
            {
                ret = id2_client_get_challenge_auth_code(
                              (char *)ssl->handshake->challenge,
                              NULL, 0, auth_code, &auth_code_len);
                if (ret != 0) {
                    SSL_DBG_LOG("id2_client_get_challenge_auth_code fail, %d\n", ret);
                    return -1;
                }
            }
        } else {
            auth_code_len = 0;
        }
    }

#if defined(MBEDTLS_DEBUG_C)
    MBEDTLS_SSL_DEBUG_BUF(3, "client hello, auth code", auth_code, auth_code_len);
#endif

    *p++ = (unsigned char)((auth_code_len >> 8) & 0xFF);
    *p++ = (unsigned char)((auth_code_len) & 0xFF);

    *olen = 4 + auth_code_len;

    return ret;
}

int mbedtls_parse_hello_verify_ext(
        mbedtls_ssl_context *ssl, unsigned char *buf, size_t len)
{
    const unsigned char *ext = buf;
    unsigned int ext_id;
    unsigned int ext_size;
#if defined(CONFIG_KEY_OTP_ENABLED)
    int ret = 0;
#endif

#if defined(MBEDTLS_SSL_PROTO_ITLS_TEST)
    if (ssl->conf->type == ITLS_TEST_FLAGS_SRV_VERIFY) {
        MBEDTLS_SSL_DEBUG_MSG(1, ("TLS ABT TEST - Hello Verify - Parse Ext Failed!!!\n\n"));
        return -1;
    }
#endif /* MBEDTLS_SSL_PROTO_ITLS_TEST */

    if (len < 4) {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "no extension data for hello verify " ) );
        return -1;
    }

    ext_id = (ext[0] << 8) | ext[1];
    ext_size = (ext[2] << 8) | ext[3];

    if (ext_size + 4 > len) {
        MBEDTLS_SSL_DEBUG_MSG( 1,
            ( "extension length does not match incoming message size" ) );
        return - 1;
    }

    switch(ext_id) {
        case MBEDTLS_TLS_EXT_SRV_CHALLENGE: {
#if defined(MBEDTLS_DEBUG_C)
            MBEDTLS_SSL_DEBUG_BUF( 3, "challenge extension", ext + 4, ext_size );
#endif

            mbedtls_free(ssl->handshake->challenge);

            ssl->handshake->challenge = mbedtls_calloc(1, ext_size + 1);
            if (ssl->handshake->challenge == NULL ) {
                SSL_DBG_LOG("alloc failed (%d bytes)\n", ext_size);
                return - 1;
            } else {
                memset(ssl->handshake->challenge, 0, ext_size + 1);
            }

            memcpy(ssl->handshake->challenge, ext + 4, ext_size);
            ssl->handshake->challenge_len = ext_size;

            break;
        }

#if defined(CONFIG_KEY_OTP_ENABLED)
        case MBEDTLS_TLS_EXT_KEY_OTP_DATA: {
#if defined(MBEDTLS_DEBUG_C)
            MBEDTLS_SSL_DEBUG_BUF( 3, "otp_data extension", ext + 4, ext_size );
#endif

            if (ssl->handshake->key_otp == 0) {
                SSL_DBG_LOG("otp_data extension is not needed\n");
                return -1;
            }

            ret = id2_client_load_otp_data(ext + 4, ext_size);
            if (ret != 0) {
                SSL_DBG_LOG("id2_client_load_otp_data fail, %d\n", ret);
                return -1;
            }

            break;
        }
#endif

        default:
           SSL_DBG_LOG("unkown extension found: 0x%04x\n", ext_id);
           return -1;
    }

    return 0;
}

int mbedtls_parse_auth_code_ext(
        mbedtls_ssl_context *ssl, unsigned char *buf, size_t len)
{
    int ret = 0;
    unsigned char *rand_str = NULL;

#if defined(MBEDTLS_DEBUG_C)
    MBEDTLS_SSL_DEBUG_BUF(3, "server hello, auth code", buf, len);
#endif

    rand_str = mbedtls_hex_to_str(ssl->handshake->randbytes, 32);
    if (rand_str == NULL) {
        SSL_DBG_LOG("mbedtls_hex_to_str fail\n");
        ret = -1;
        goto _out;
    }

#if defined(MBEDTLS_DEBUG_C)
    MBEDTLS_SSL_DEBUG_BUF(3, "rand_str", rand_str, strlen((char *)rand_str));
#endif

#if defined(MBEDTLS_SSL_PROTO_ITLS_TEST)
    if (ssl->conf->type == ITLS_TEST_FLAGS_SRV_HELLO) {
         SSL_DBG_LOG("TLS ABT TEST - Server Hello - Verify iTLS Server Failed!!!\n\n");

         ret = -1;
         goto _out;
    }
#endif /* MBEDTLS_SSL_PROTO_ITLS_TEST */

    ret = id2_client_verify_server(
              buf, len, rand_str, strlen((char *)rand_str), NULL, 0);
    if (ret != 0) {
        SSL_DBG_LOG("  . Verify iTLS Server authCode Failed!\n");
        ret = -1;
        goto _out;
    } else {
        SSL_DBG_LOG("  . Verify iTLS Server authCode OK!\n");
    }

    ret = 0;

_out:
    if (ret < 0) {
        mbedtls_ssl_send_alert_message(ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                       MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE );
        ret = MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO;
    }

    if (rand_str != NULL) {
        mbedtls_free(rand_str);
    }

    return ret;
}

int mbedtls_parse_pre_master_secret_ext(
        mbedtls_ssl_context *ssl, unsigned char *buf, size_t len)
{
    int ret = 0;
    uint32_t pms_len;

#if defined(MBEDTLS_DEBUG_C)
    MBEDTLS_SSL_DEBUG_BUF( 3, "server hello, encrypted premaster secret", buf, len );
#endif

    pms_len = 48;
    ret = id2_client_decrypt(buf, len, ssl->handshake->premaster, &pms_len);
    if (ret != 0) {
        SSL_DBG_LOG("id2_client_decrypt fail, %d\n", ret);
        ret = -1;
        goto _out;
    }

    if (pms_len != 48) {
        SSL_DBG_LOG("invalid premaster secret len: %d\n", (int)pms_len);
        ret = -1;
        goto _out;
    } else {
        ssl->handshake->pmslen = pms_len;
    }

#if defined(MBEDTLS_DEBUG_C)
    MBEDTLS_SSL_DEBUG_BUF( 3, "premaster secret", ssl->handshake->premaster, pms_len);
#endif

    ret = 0;

_out:
    if (ret < 0) {
        mbedtls_ssl_send_alert_message(ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                       MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE);
        ret = MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO;
    }

    return ret;
}

#if defined(MBEDTLS_SSL_PROTO_ITLS_TEST)
int mbedtls_ssl_conf_set_data( mbedtls_ssl_config *conf,
                unsigned int type, const char *data, size_t data_len)
{
    SSL_DBG_LOG("ssl_conf_set_data - type: 0x%x\n", type);

    if (conf == NULL || data == NULL || data_len == 0) {
        SSL_DBG_LOG("invalid input args\n");
        return -1;
    }

    switch(type) {
        case ITLS_TEST_DATA_KEY_ID: {
            conf->key_id = (unsigned char *)data;
            conf->key_id_len = data_len;
            break;
        }
        case ITLS_TEST_DATA_AUTH_CODE: {
            conf->auth_code = (unsigned char *)data;
            conf->auth_code_len = data_len;
            break;
        }
        case ITLS_TEST_DATA_VERIFY_DATA: {
            conf->verify_data = (unsigned char *)data;
            conf->verify_data_len = data_len;
            break;
        }
        default: {
            SSL_DBG_LOG("invalid data type, 0x%x\n", type);
            return -1;
        }
    }

    conf->type = type;

    return 0;
}

int mbedtls_ssl_conf_set_value( mbedtls_ssl_config *conf,
                unsigned int type, unsigned int value)
{
    SSL_DBG_LOG("ssl_conf_set_data - type: 0x%x value: 0x%x\n", type, value);

    if (conf == NULL) {
        SSL_DBG_LOG("invalid input arg\n");
        return -1;
    }

    switch(type) {
        case ITLS_TEST_VALUE_VERSION: {
            conf->version = value;
            break;
        }
        case ITLS_TEST_VALUE_KEY_GROUP: {
            conf->key_group = value;
            break;
        }
        case ITLS_TEST_VALUE_CIPHER_SUITE: {
            conf->cipher_suite = value;
            break;
        }
        default: {
            SSL_DBG_LOG("invalid data type, 0x%x\n", type);
            return -1;
        }
    }

    conf->type = type;

    return 0;
}
#endif /* MBEDTLS_SSL_PROTO_ITLS_TEST */

#endif /* MBEDTLS_SSL_PROTO_ITLS */

