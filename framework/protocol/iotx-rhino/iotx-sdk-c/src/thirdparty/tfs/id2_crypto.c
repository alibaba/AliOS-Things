/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifdef MQTT_ID2_AUTH


#include <stdint.h>
#include <stddef.h>
#include "iot_export.h"
#include "sdk-impl_internal.h"
#include "tfs.h"
#include "utils_hmac.h"
#include "tfs_debug.h"

#define HASH_SHA1_LEN           (20)
#define HMAC_MD5_LEN            (16)
#define MQTT_MAX_TOPIC_LEN      (64)
#define MQTT_MS(A, OFFSET) ((A << (sizeof(A) - (OFFSET))*8) >> (sizeof(A) - 1) * 8)

void writeUint16(unsigned char **pptr, unsigned short anUint)
{
    if (!pptr) {
        return;
    }

    **pptr = MQTT_MS(anUint, 2);
    (*pptr)++;
    **pptr = MQTT_MS(anUint, 1);
    (*pptr)++;
}

/**
 * Writes an integer as 4 bytes to an output buffer.
 * @param pptr pointer to the output buffer - incremented by the number of bytes used & returned
 * @param anInt the integer to write
 */
void writeUint32(unsigned char **pptr, unsigned int anUint)
{
    if (!pptr) {
        return;
    }

    **pptr = MQTT_MS(anUint, 4);
    (*pptr)++;
    **pptr = MQTT_MS(anUint, 3);
    (*pptr)++;
    **pptr = MQTT_MS(anUint, 2);
    (*pptr)++;
    **pptr = MQTT_MS(anUint, 1);
    (*pptr)++;
}

typedef struct _MQTT_ReplayFender {
    uint32_t        sequence;
    uint16_t        hmac_len;
#ifdef HASH_SHA1
    char            hmac_str[HASH_SHA1_LEN * 2];
#else
    char            hmac_str[HMAC_MD5_LEN * 2];
#endif
} __attribute__((packed)) MQTT_ReplayFender;

static int _fill_replay_fender(
            MQTT_ReplayFender *f,
            const char *topic,
            uint16_t msg_id,
            int enc_payloadlen)
{
    char                hmac_source[MQTT_MAX_TOPIC_LEN + 32] = {0};
    int                 hmac_srclen = 0;
    iotx_conn_info_pt   conn;

    if (!f || !topic) {
        return -1;
    }

    if (!(conn = iotx_conn_info_get())) {
        return -1;
    }

    uint8_t *ptr = (uint8_t *) & (f->sequence);
    writeUint32(&ptr, (uint32_t)msg_id);
    ptr = (uint8_t *) & (f->hmac_len);
    writeUint16(&ptr, HMAC_MD5_LEN * 2);

    HAL_Snprintf((char *)hmac_source, sizeof(hmac_source),
                 "%s%d%d",
                 topic, msg_id, enc_payloadlen);
    hmac_srclen = strlen(hmac_source);
    tfs_debug("hmac_source(%d) = '%s'", hmac_srclen, hmac_source);

    memset(f->hmac_str, 0, sizeof(f->hmac_str));
#ifdef HASH_SHA1
    uint8_t             sha1_hexbuf[HASH_SHA1_LEN] = {0};

    utils_sha1(hmac_source, hmac_srclen, sha1_hexbuf);
    LITE_hexbuf_convert(sha1_hexbuf, f->hmac_str, HASH_SHA1_LEN, 0);
    tfs_debug("sha1_str(%d) = '%s'", HASH_SHA1_LEN * 2, f->hmac_str);
#else
    utils_hmac_md5((const char *)hmac_source,
                   hmac_srclen,
                   f->hmac_str,
                   (const char *)conn->aeskey_hex,
                   (int)sizeof(conn->aeskey_hex));

    HEXDUMP_DEBUG(f->hmac_str, sizeof(f->hmac_str));
    /* _to_lower(f->hmac_str, sizeof(f->hmac_str)); */
    HEXDUMP_DEBUG(f->hmac_str, sizeof(f->hmac_str));
#endif

    return 0;
}

int iotx_mqtt_id2_payload_encrypt(char *topic, iotx_mqtt_topic_info_pt topic_msg)
{
    MQTT_ReplayFender       fender;
    int                     ret = -1;
    int                     out_buf_len = 0;
    uint8_t                *out_buf = NULL;
    uint8_t                 iv[16] = {0};
    int                     enc_len = 0;
    iotx_conn_info_pt       conn;

    if (!topic || !topic_msg) {
        return -1;
    }

    if (!(conn = iotx_conn_info_get())) {
        return -1;
    }

    out_buf_len = (topic_msg->payload_len / 16) * 16;
    if (topic_msg->payload_len % 16 != 0) {
        out_buf_len += 16;
    }

    /* get fender (fender is between mqtt header and mqtt message) */
    memset(&fender, 0, sizeof(MQTT_ReplayFender));
    _fill_replay_fender(&fender,
                        topic,
                        topic_msg->packet_id,
                        out_buf_len);

    out_buf_len += sizeof(MQTT_ReplayFender);
    out_buf = (uint8_t *)LITE_malloc(out_buf_len + 1);
    memset(out_buf, 0, out_buf_len + 1);
    memcpy(out_buf, &fender, sizeof(MQTT_ReplayFender));

    /* encode MQTT message */
    ret = tfs_aes128_cbc_enc(conn->aeskey_hex,
                             iv,
                             topic_msg->payload_len,
                             (const uint8_t *)topic_msg->payload,
                             &enc_len,
                             out_buf + sizeof(MQTT_ReplayFender),
                             TFS_AES_ZERO_PADDING);
    tfs_debug("rc = tfs_aes128_cbc_enc() = %d, enc_len = %d", ret, enc_len);

    if (ret != 0) {
        tfs_err("tfs_aes128_cbc_enc error!");
        ret = -1;
        goto exit;
    }
    memset((void *)topic_msg->payload, 0, topic_msg->payload_len);
    memcpy((void *)topic_msg->payload, out_buf, out_buf_len);
    topic_msg->payload_len = out_buf_len;

    HEXDUMP_DEBUG(topic_msg->payload, topic_msg->payload_len);

exit:
    LITE_free(out_buf);

    return ret;
}

int iotx_mqtt_id2_payload_decrypt(iotx_mqtt_topic_info_pt topic_msg)
{
    int                 ret = -1;
    uint8_t             iv[16] = {0};
    int32_t             dec_len = 0;
    int                 offset = sizeof(MQTT_ReplayFender);
    uint8_t            *dec_out = NULL;
    iotx_conn_info_pt   conn = NULL;

    conn = iotx_conn_info_get();
    if (!conn) {
        return -1;
    }
    dec_out = LITE_malloc(topic_msg->payload_len);
    if (!dec_out) {
        return -1;
    }

    memset(dec_out, 0, topic_msg->payload_len);
    ret = tfs_aes128_cbc_dec(conn->aeskey_hex,
                             iv,
                             topic_msg->payload_len - offset,
                             (const uint8_t *)topic_msg->payload + offset,
                             &dec_len,
                             dec_out,
                             TFS_AES_ZERO_PADDING);
    tfs_debug("rc = tfs_aes128_cbc_dec() = %d, dec_len = %d", ret, dec_len);

    if (ret != 0) {
        tfs_err("tfs_aes128_cbc_dec error!");
        LITE_free(dec_out);
        return -1;
    }

    memset((void *)topic_msg->payload, 0, topic_msg->payload_len);
    memcpy((void *)topic_msg->payload, dec_out, dec_len);
    topic_msg->payload_len = dec_len;

    tfs_debug("ID2 decrypt publish[%d]: %s", topic_msg->payload_len, topic_msg->payload);
    LITE_free(dec_out);

    return ret;
}
#endif

