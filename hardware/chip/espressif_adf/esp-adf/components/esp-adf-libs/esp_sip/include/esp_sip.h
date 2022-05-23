/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _ESP_SIP_H
#define _ESP_SIP_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sip_* sip_handle_t;

/**
 * @brief SIP audio codec type
 */
typedef enum {
    SIP_ACODEC_G711A,
    SIP_ACODEC_G711U,
    SIP_ACODEC_OPUS,
    SIP_ACODEC_SPEEX,
} sip_audio_codec_t;

/**
 * @brief SIP session state
 */
typedef enum {
    SIP_STATE_NONE          = 0x00,
    SIP_STATE_CONNECTED     = 0x01,
    SIP_STATE_REGISTERED    = 0x02,
    SIP_STATE_CALLING       = 0x04,
    SIP_STATE_SESS_PROGRESS = 0x08,
    SIP_STATE_RINGING       = 0x10,
    SIP_STATE_ON_CALL       = 0x20,
    SIP_STATE_CALL_OWNER    = 0x40,
} sip_state_t;

/**
 * @brief SIP session event
 */
typedef enum {
    SIP_EVENT_NONE = 0,
    SIP_EVENT_REGISTERED,
    SIP_EVENT_RINGING,
    SIP_EVENT_INVITING,
    SIP_EVENT_HANGUP,
    SIP_EVENT_BUSY,
    SIP_EVENT_UNREGISTERED,
    SIP_EVENT_AUDIO_SESSION_BEGIN,
    SIP_EVENT_READ_AUDIO_DATA,
    SIP_EVENT_WRITE_AUDIO_DATA,
    SIP_EVENT_AUDIO_SESSION_END,
    SIP_EVENT_REQUEST_NETWORK_STATUS,
    SIP_EVENT_REQUEST_NETWORK_IP,
    SIP_EVENT_READ_DTMF,
} sip_event_t;

/**
 * @brief SIP Messages header info
 */
typedef struct {
    char *via;         /*!< SIP Messages via fields  */
    char *from;        /*!< SIP Messages from fields  */
    char *to;          /*!< SIP Messages to fields  */
    char *contact;     /*!< SIP Messages contact fields  */
} sip_messages_info_t;

/**
 * @brief SIP session event message
 */
typedef struct {
    sip_event_t type;           /*!< SIP session event type */
    void *data;                 /*!< RTP TX/RX data */
    int data_len;               /*!< Length of rtp data */
} sip_event_msg_t;

typedef int (*sip_event_handle)(sip_event_msg_t *event);

/**
 * @brief SIP session configurations
 */
typedef struct {
    sip_event_handle            event_handler;       /*!< SIP session event handler */
    const char                  *uri;                /*!< Transport://user:pass@server:port/path */
    const char                  *cert_pem;           /*!< SSL server certification, PEM format as string, if the client requires to verify server */
    const char                  *client_cert_pem;    /*!< SSL client certification, PEM format as string, if the server requires to verify client */
    const char                  *client_key_pem;     /*!< SSL client key, PEM format as string, if the server requires to verify client */
    bool                        send_options;        /*!< Send 'OPTIONS' messages to Server for keep NAT hole opened*/
    sip_audio_codec_t           acodec_type;         /*!< Audio codec type */
} sip_config_t;

/**
 * @brief      Intialize SIP Service
 *
 * @param[in]  config   The SIP configuration
 *
 * @return     The sip handle
 */
sip_handle_t esp_sip_init(sip_config_t *config);

/**
 * @brief      Start sip task and register the device
 *
 * @param[in]  sip  The sip handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t esp_sip_start(sip_handle_t sip);

/**
 * @brief      Stop sip task
 *
 * @param[in]  sip  The sip handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t esp_sip_stop(sip_handle_t sip);

/**
 * @brief      Destroy SIP Service
 *
 * @param[in]  sip  The sip handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t esp_sip_destroy(sip_handle_t sip);

/**
 * @brief       Answer the sip Session
 *
 * @param[in]  sip      The sip handle
 * @param[in]  accept   accept the session or not
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t esp_sip_uas_answer(sip_handle_t sip, bool accept);

/**
 * @brief      Initialize a sip session
 *
 * @param[in]  sip          The sip handle
 * @param[in]  extension    Remote extension ID
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t esp_sip_uac_invite(sip_handle_t sip, const char *extension);

/**
 * @brief      Hang up
 *
 * @param[in]  sip  The sip handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t esp_sip_uac_bye(sip_handle_t sip);

/**
 * @brief      Cancel the sip session
 *
 * @param[in]  sip  The sip handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t esp_sip_uac_cancel(sip_handle_t sip);

/**
 * @brief      Get the current session state
 *
 * @param[in]  sip  The sip handle
 *
 * @return     SIP session state
 */
sip_state_t esp_sip_get_state(sip_handle_t sip);

/**
 * @brief      Send DTMF event ( Only support out band method (RFC2833) )
 *
 * @param[in]  sip          The sip handle
 * @param[in]  dtmf_event   DTMF event ID (0-15)
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t esp_sip_send_dtmf(sip_handle_t sip, const uint8_t dtmf_event);

/**
 * @brief      Set custom invite info
 *
 * @param[in]  sip              The sip handle
 * @param[in]  sip_set_info     Set Via, From, To, Contact fields
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t esp_sip_set_invite_info(sip_handle_t sip, const sip_messages_info_t *sip_set_info);

/**
 * @brief      Read incomming sip message
 *
 * @param[in]  sip                      The sip handle
 * @param[in]  sip_messages_info_t      Read Via, From, To, Contact fields, we will copy to user buffer.
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t esp_sip_read_incomming_messages(sip_handle_t sip, sip_messages_info_t *sip_read_info);

#ifdef __cplusplus
}
#endif

#endif
