/*
* Copyright (c) 2014, Stephen Robinson
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1. Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*/
#include <string.h>
#include "mqtt_msg.h"
#include "mqtt_config.h"
#include "platform.h"

#define MQTT_MAX_FIXED_HEADER_SIZE 5
#define MQTT_3_1_VARIABLE_HEADER_SIZE 12
#define MQTT_3_1_1_VARIABLE_HEADER_SIZE 10

enum mqtt_connect_flag {
    MQTT_CONNECT_FLAG_USERNAME = 1 << 7,
    MQTT_CONNECT_FLAG_PASSWORD = 1 << 6,
    MQTT_CONNECT_FLAG_WILL_RETAIN = 1 << 5,
    MQTT_CONNECT_FLAG_WILL = 1 << 2,
    MQTT_CONNECT_FLAG_CLEAN_SESSION = 1 << 1
};

static int append_string(mqtt_connection_t *connection, const char *string, int len)
{
    if (connection->message.length + len + 2 > connection->buffer_length) {
        return -1;
    }

    connection->buffer[connection->message.length++] = len >> 8;
    connection->buffer[connection->message.length++] = len & 0xff;
    memcpy(connection->buffer + connection->message.length, string, len);
    connection->message.length += len;

    return len + 2;
}

static uint16_t append_message_id(mqtt_connection_t *connection, uint16_t message_id)
{
    // If message_id is zero then we should assign one, otherwise
    // we'll use the one supplied by the caller
    while (message_id == 0) {
#if MQTT_MSG_ID_INCREMENTAL
        message_id = ++connection->last_message_id;
#else
        message_id = platform_random(65535);
#endif
    }

    if (connection->message.length + 2 > connection->buffer_length) {
        return 0;
    }

    connection->buffer[connection->message.length++] = message_id >> 8;
    connection->buffer[connection->message.length++] = message_id & 0xff;

    return message_id;
}

static int init_message(mqtt_connection_t *connection)
{
    connection->message.length = MQTT_MAX_FIXED_HEADER_SIZE;
    return MQTT_MAX_FIXED_HEADER_SIZE;
}

static mqtt_message_t *fail_message(mqtt_connection_t *connection)
{
    connection->message.data = connection->buffer;
    connection->message.length = 0;
    return &connection->message;
}

static mqtt_message_t *fini_message(mqtt_connection_t *connection, int type, int dup, int qos, int retain)
{
    int message_length = connection->message.length - MQTT_MAX_FIXED_HEADER_SIZE;
    int total_length = message_length;
    int encoded_length = 0;
    uint8_t encoded_lens[4] = {0};
    // Check if we have fragmented message and update total_len
    if (connection->message.fragmented_msg_total_length) {
        total_length = connection->message.fragmented_msg_total_length - MQTT_MAX_FIXED_HEADER_SIZE;
    }

    // Encode MQTT message length
    int len_bytes = 0; // size of encoded message length
    do {
        encoded_length = total_length % 128;
        total_length /= 128;
        if (total_length > 0) {
            encoded_length |= 0x80;
        }
        encoded_lens[len_bytes] = encoded_length;
        len_bytes++;
    } while (total_length > 0);

    // Sanity check for MQTT header
    if (len_bytes + 1 > MQTT_MAX_FIXED_HEADER_SIZE) {
        return fail_message(connection);
    }

    // Save the header bytes
    connection->message.length = message_length + len_bytes + 1; // msg len + encoded_size len + type (1 byte)
    int offs = MQTT_MAX_FIXED_HEADER_SIZE - 1 - len_bytes;
    connection->message.data = connection->buffer + offs;
    connection->message.fragmented_msg_data_offset -= offs;
    // type byte
    connection->buffer[offs++] =  ((type & 0x0f) << 4) | ((dup & 1) << 3) | ((qos & 3) << 1) | (retain & 1);
    // length bytes
    for (int j = 0; j < len_bytes; j++) {
        connection->buffer[offs++] = encoded_lens[j];
    }

    return &connection->message;
}

void mqtt_msg_init(mqtt_connection_t *connection, uint8_t *buffer, size_t buffer_length)
{
    memset(connection, 0, sizeof(mqtt_connection_t));
    connection->buffer = buffer;
    connection->buffer_length = buffer_length;
}

size_t mqtt_get_total_length(const uint8_t *buffer, size_t length, int *fixed_size_len)
{
    int i;
    size_t totlen = 0;

    for (i = 1; i < length; ++i) {
        totlen += (buffer[i] & 0x7f) << (7 * (i - 1));
        if ((buffer[i] & 0x80) == 0) {
            ++i;
            break;
        }
    }
    totlen += i;
    if (fixed_size_len) {
        *fixed_size_len = i;
    }

    return totlen;
}

bool mqtt_header_complete(uint8_t *buffer, size_t buffer_length)
{
    uint16_t i;
    uint16_t topiclen;

    for (i = 1; i < MQTT_MAX_FIXED_HEADER_SIZE; ++i) {
        if (i >= buffer_length) {
            return false;
        }
        if ((buffer[i] & 0x80) == 0) {
            ++i;
            break;
        }
    }
    // i is now the length of the fixed header

    if (i + 2 >= buffer_length) {
        return false;
    }
    topiclen = buffer[i++] << 8;
    topiclen |= buffer[i++];

    i += topiclen;

    if (mqtt_get_qos(buffer) > 0) {
        i += 2;
    }
    // i is now the length of the fixed + variable header
    return buffer_length >= i;
}

char *mqtt_get_publish_topic(uint8_t *buffer, size_t *length)
{
    int i;
    int totlen = 0;
    int topiclen;

    for (i = 1; i < *length; ++i) {
        totlen += (buffer[i] & 0x7f) << (7 * (i - 1));
        if ((buffer[i] & 0x80) == 0) {
            ++i;
            break;
        }
    }
    totlen += i;

    if (i + 2 >= *length) {
        return NULL;
    }
    topiclen = buffer[i++] << 8;
    topiclen |= buffer[i++];

    if (i + topiclen > *length) {
        return NULL;
    }

    *length = topiclen;
    return (char *)(buffer + i);
}

char *mqtt_get_publish_data(uint8_t *buffer, size_t *length)
{
    int i;
    int totlen = 0;
    int topiclen;
    int blength = *length;
    *length = 0;

    for (i = 1; i < blength; ++i) {
        totlen += (buffer[i] & 0x7f) << (7 * (i - 1));
        if ((buffer[i] & 0x80) == 0) {
            ++i;
            break;
        }
    }
    totlen += i;

    if (i + 2 >= blength) {
        return NULL;
    }
    topiclen = buffer[i++] << 8;
    topiclen |= buffer[i++];

    if (i + topiclen >= blength) {
        return NULL;
    }

    i += topiclen;

    if (mqtt_get_qos(buffer) > 0) {
        if (i + 2 >= blength) {
            return NULL;
        }
        i += 2;
    }

    if (totlen < i) {
        return NULL;
    }

    if (totlen <= blength) {
        *length = totlen - i;
    } else {
        *length = blength - i;
    }
    return (char *)(buffer + i);
}

uint16_t mqtt_get_id(uint8_t *buffer, size_t length)
{
    if (length < 1) {
        return 0;
    }

    switch (mqtt_get_type(buffer)) {
    case MQTT_MSG_TYPE_PUBLISH: {
        int i;
        int topiclen;

        for (i = 1; i < length; ++i) {
            if ((buffer[i] & 0x80) == 0) {
                ++i;
                break;
            }
        }

        if (i + 2 >= length) {
            return 0;
        }
        topiclen = buffer[i++] << 8;
        topiclen |= buffer[i++];

        if (i + topiclen > length) {
            return 0;
        }
        i += topiclen;

        if (mqtt_get_qos(buffer) > 0) {
            if (i + 2 > length) {
                return 0;
            }
            //i += 2;
        } else {
            return 0;
        }

        return (buffer[i] << 8) | buffer[i + 1];
    }
    case MQTT_MSG_TYPE_PUBACK:
    case MQTT_MSG_TYPE_PUBREC:
    case MQTT_MSG_TYPE_PUBREL:
    case MQTT_MSG_TYPE_PUBCOMP:
    case MQTT_MSG_TYPE_SUBACK:
    case MQTT_MSG_TYPE_UNSUBACK:
    case MQTT_MSG_TYPE_SUBSCRIBE:
    case MQTT_MSG_TYPE_UNSUBSCRIBE: {
        // This requires the remaining length to be encoded in 1 byte,
        // which it should be.
        if (length >= 4 && (buffer[1] & 0x80) == 0) {
            return (buffer[2] << 8) | buffer[3];
        } else {
            return 0;
        }
    }

    default:
        return 0;
    }
}

mqtt_message_t *mqtt_msg_connect(mqtt_connection_t *connection, mqtt_connect_info_t *info)
{

    init_message(connection);

    int header_len;
    if (info->protocol_ver == MQTT_PROTOCOL_V_3_1) {
        header_len = MQTT_3_1_VARIABLE_HEADER_SIZE;
    } else {
        header_len = MQTT_3_1_1_VARIABLE_HEADER_SIZE;
    }

    if (connection->message.length + header_len > connection->buffer_length) {
        return fail_message(connection);
    }
    char* variable_header = (void *)(connection->buffer + connection->message.length);
    connection->message.length += header_len;

    int header_idx = 0;
    variable_header[header_idx++] = 0;                              // Variable header length MSB

    if (info->protocol_ver == MQTT_PROTOCOL_V_3_1) {
        variable_header[header_idx++] = 6;                          // Variable header length LSB
        memcpy(&variable_header[header_idx], "MQIsdp", 6);          // Protocol name
        header_idx = header_idx + 6;
        variable_header[header_idx++] = 3;                          // Protocol version
    } else {
        /* Defaults to protocol version 3.1.1 values */
        variable_header[header_idx++] = 4;                          // Variable header length LSB
        memcpy(&variable_header[header_idx], "MQTT", 4);            // Protocol name
        header_idx = header_idx + 4;
        variable_header[header_idx++] = 4;                          // Protocol version
    }

    int flags_offset = header_idx;
    variable_header[header_idx++] = 0;                              // Flags
    variable_header[header_idx++] = info->keepalive >> 8;           // Keep-alive MSB
    variable_header[header_idx] = info->keepalive & 0xff;         // Keep-alive LSB

    if (info->clean_session) {
        variable_header[flags_offset] |= MQTT_CONNECT_FLAG_CLEAN_SESSION;
    }

    if (info->client_id != NULL && info->client_id[0] != '\0') {
        if (append_string(connection, info->client_id, strlen(info->client_id)) < 0) {
            return fail_message(connection);
        }
    } else {
        return fail_message(connection);
    }

    if (info->will_topic != NULL && info->will_topic[0] != '\0') {
        if (append_string(connection, info->will_topic, strlen(info->will_topic)) < 0) {
            return fail_message(connection);
        }

        if (append_string(connection, info->will_message, info->will_length) < 0) {
            return fail_message(connection);
        }

        variable_header[flags_offset] |= MQTT_CONNECT_FLAG_WILL;
        if (info->will_retain) {
            variable_header[flags_offset] |= MQTT_CONNECT_FLAG_WILL_RETAIN;
        }
        variable_header[flags_offset] |= (info->will_qos & 3) << 3;
    }

    if (info->username != NULL && info->username[0] != '\0') {
        if (append_string(connection, info->username, strlen(info->username)) < 0) {
            return fail_message(connection);
        }

        variable_header[flags_offset] |= MQTT_CONNECT_FLAG_USERNAME;
    }

    if (info->password != NULL && info->password[0] != '\0') {
        if (info->username == NULL || info->username[0] == '\0') {
            /* In case if password is set without username, we need to set a zero length username.
             * (otherwise we violate: MQTT-3.1.2-22: If the User Name Flag is set to 0 then the Password Flag MUST be set to 0.)
             */
            if (append_string(connection, "", 0) < 0) {
                return fail_message(connection);
            }

            variable_header[flags_offset] |= MQTT_CONNECT_FLAG_USERNAME;
        }

        if (append_string(connection, info->password, strlen(info->password)) < 0) {
            return fail_message(connection);
        }

        variable_header[flags_offset] |= MQTT_CONNECT_FLAG_PASSWORD;
    }

    return fini_message(connection, MQTT_MSG_TYPE_CONNECT, 0, 0, 0);
}

mqtt_message_t *mqtt_msg_publish(mqtt_connection_t *connection, const char *topic, const char *data, int data_length, int qos, int retain, uint16_t *message_id)
{
    init_message(connection);

    if (topic == NULL || topic[0] == '\0') {
        return fail_message(connection);
    }

    if (append_string(connection, topic, strlen(topic)) < 0) {
        return fail_message(connection);
    }

    if (data == NULL && data_length > 0) {
       return fail_message(connection);
    }

    if (qos > 0) {
        if ((*message_id = append_message_id(connection, 0)) == 0) {
            return fail_message(connection);
        }
    } else {
        *message_id = 0;
    }

    if (connection->message.length + data_length > connection->buffer_length) {
        // Not enough size in buffer -> fragment this message
        connection->message.fragmented_msg_data_offset = connection->message.length;
        memcpy(connection->buffer + connection->message.length, data, connection->buffer_length - connection->message.length);
        connection->message.length = connection->buffer_length;
        connection->message.fragmented_msg_total_length = data_length + connection->message.fragmented_msg_data_offset;
    } else {
        if (data != NULL)
        {
            memcpy(connection->buffer + connection->message.length, data, data_length);
            connection->message.length += data_length;
        }
        connection->message.fragmented_msg_total_length = 0;
    }
    return fini_message(connection, MQTT_MSG_TYPE_PUBLISH, 0, qos, retain);
}

mqtt_message_t *mqtt_msg_puback(mqtt_connection_t *connection, uint16_t message_id)
{
    init_message(connection);
    if (append_message_id(connection, message_id) == 0) {
        return fail_message(connection);
    }
    return fini_message(connection, MQTT_MSG_TYPE_PUBACK, 0, 0, 0);
}

mqtt_message_t *mqtt_msg_pubrec(mqtt_connection_t *connection, uint16_t message_id)
{
    init_message(connection);
    if (append_message_id(connection, message_id) == 0) {
        return fail_message(connection);
    }
    return fini_message(connection, MQTT_MSG_TYPE_PUBREC, 0, 0, 0);
}

mqtt_message_t *mqtt_msg_pubrel(mqtt_connection_t *connection, uint16_t message_id)
{
    init_message(connection);
    if (append_message_id(connection, message_id) == 0) {
        return fail_message(connection);
    }
    return fini_message(connection, MQTT_MSG_TYPE_PUBREL, 0, 1, 0);
}

mqtt_message_t *mqtt_msg_pubcomp(mqtt_connection_t *connection, uint16_t message_id)
{
    init_message(connection);
    if (append_message_id(connection, message_id) == 0) {
        return fail_message(connection);
    }
    return fini_message(connection, MQTT_MSG_TYPE_PUBCOMP, 0, 0, 0);
}

mqtt_message_t *mqtt_msg_subscribe(mqtt_connection_t *connection, const char *topic, int qos, uint16_t *message_id)
{
    init_message(connection);

    if (topic == NULL || topic[0] == '\0') {
        return fail_message(connection);
    }

    if ((*message_id = append_message_id(connection, 0)) == 0) {
        return fail_message(connection);
    }

    if (append_string(connection, topic, strlen(topic)) < 0) {
        return fail_message(connection);
    }

    if (connection->message.length + 1 > connection->buffer_length) {
        return fail_message(connection);
    }
    connection->buffer[connection->message.length++] = qos;

    return fini_message(connection, MQTT_MSG_TYPE_SUBSCRIBE, 0, 1, 0);
}

mqtt_message_t *mqtt_msg_unsubscribe(mqtt_connection_t *connection, const char *topic, uint16_t *message_id)
{
    init_message(connection);

    if (topic == NULL || topic[0] == '\0') {
        return fail_message(connection);
    }

    if ((*message_id = append_message_id(connection, 0)) == 0) {
        return fail_message(connection);
    }

    if (append_string(connection, topic, strlen(topic)) < 0) {
        return fail_message(connection);
    }

    return fini_message(connection, MQTT_MSG_TYPE_UNSUBSCRIBE, 0, 1, 0);
}

mqtt_message_t *mqtt_msg_pingreq(mqtt_connection_t *connection)
{
    init_message(connection);
    return fini_message(connection, MQTT_MSG_TYPE_PINGREQ, 0, 0, 0);
}

mqtt_message_t *mqtt_msg_pingresp(mqtt_connection_t *connection)
{
    init_message(connection);
    return fini_message(connection, MQTT_MSG_TYPE_PINGRESP, 0, 0, 0);
}

mqtt_message_t *mqtt_msg_disconnect(mqtt_connection_t *connection)
{
    init_message(connection);
    return fini_message(connection, MQTT_MSG_TYPE_DISCONNECT, 0, 0, 0);
}

/*
 * check flags: [MQTT-2.2.2-1], [MQTT-2.2.2-2]
 * returns 0 if flags are invalid, otherwise returns 1
 */
int mqtt_has_valid_msg_hdr(uint8_t *buffer, size_t length)
{
    int qos, dup;

    if (length < 1) {
        return 0;
    }
    switch (mqtt_get_type(buffer)) {
    case MQTT_MSG_TYPE_CONNECT:
    case MQTT_MSG_TYPE_CONNACK:
    case MQTT_MSG_TYPE_PUBACK:
    case MQTT_MSG_TYPE_PUBREC:
    case MQTT_MSG_TYPE_PUBCOMP:
    case MQTT_MSG_TYPE_SUBACK:
    case MQTT_MSG_TYPE_UNSUBACK:
    case MQTT_MSG_TYPE_PINGREQ:
    case MQTT_MSG_TYPE_PINGRESP:
    case MQTT_MSG_TYPE_DISCONNECT:
        return (buffer[0] & 0x0f) == 0;  /* all flag bits are 0 */
    case MQTT_MSG_TYPE_PUBREL:
    case MQTT_MSG_TYPE_SUBSCRIBE:
    case MQTT_MSG_TYPE_UNSUBSCRIBE:
        return (buffer[0] & 0x0f) == 0x02;  /* only bit 1 is set */
    case MQTT_MSG_TYPE_PUBLISH:
        qos = mqtt_get_qos(buffer);
        dup = mqtt_get_dup(buffer);
        /*
         * there is no qos=3  [MQTT-3.3.1-4]
         * dup flag must be set to 0 for all qos=0 messages [MQTT-3.3.1-2]
         */
        return (qos < 3) && ((qos > 0) || (dup == 0));
    default:
        return 0;
    }
}
