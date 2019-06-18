/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v20.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    domedambrosio - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
 *    Simon Bernard - Please refer to git log
 *    Toby Jaffey - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *
 *******************************************************************************/

/*
 Copyright (c) 2013, 2014 Intel Corporation

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
     * Neither the name of Intel Corporation nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.

 David Navarro <david.navarro@intel.com>

*/

/*
Contains code snippets which are:

 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.

*/


#include "internals.h"

#include <stdlib.h>
#include <string.h>

#include <stdio.h>


static void handle_reset(lwm2m_context_t * contextP,
                         void * fromSessionH,
                         lwm2m_coap_packet_t * message)
{
#ifdef LWM2M_CLIENT_MODE
    lwm2m_log(LOG_DEBUG, "Entering\n");
    observe_cancel(contextP, message->mid, fromSessionH);
#endif
}

static uint8_t handle_request(lwm2m_context_t * contextP,
                              void * fromSessionH,
                              lwm2m_coap_packet_t * message,
                              lwm2m_coap_packet_t * response)
{
    lwm2m_uri_t * uriP;
    uint8_t result = COAP_IGNORE;

    lwm2m_log(LOG_DEBUG, "Entering\n");

#ifdef LWM2M_CLIENT_MODE
    uriP = uri_decode(contextP->altPath, message->uri_path);
#else
    uriP = uri_decode(NULL, message->uri_path);
#endif

    if (uriP == NULL) return COAP_400_BAD_REQUEST;

    switch(uriP->flag & LWM2M_URI_MASK_TYPE)
    {
#ifdef LWM2M_CLIENT_MODE
    case LWM2M_URI_FLAG_DM:
    {
        lwm2m_server_t * serverP;

        serverP = utils_findServer(contextP, fromSessionH);
        if (serverP != NULL)
        {
            result = dm_handleRequest(contextP, uriP, serverP, message, response);
        }
#ifdef LWM2M_BOOTSTRAP
        else
        {
            serverP = utils_findBootstrapServer(contextP, fromSessionH);
            if (serverP != NULL)
            {
                result = bootstrap_handleCommand(contextP, uriP, serverP, message, response);
            }
        }
#endif
    }
    break;

#ifdef LWM2M_BOOTSTRAP
    case LWM2M_URI_FLAG_DELETE_ALL:
        if (COAP_DELETE != message->code)
        {
            result = COAP_400_BAD_REQUEST;
        }
        else
        {
            result = bootstrap_handleDeleteAll(contextP, fromSessionH);
        }
        break;

    case LWM2M_URI_FLAG_BOOTSTRAP:
        if (message->code == COAP_POST)
        {
            result = bootstrap_handleFinish(contextP, fromSessionH);
        }
        break;
#endif
#endif

#ifdef LWM2M_SERVER_MODE
    case LWM2M_URI_FLAG_REGISTRATION:
        result = registration_handleRequest(contextP, uriP, fromSessionH, message, response);
        break;
#endif
#ifdef LWM2M_BOOTSTRAP_SERVER_MODE
    case LWM2M_URI_FLAG_BOOTSTRAP:
        result = bootstrap_handleRequest(contextP, uriP, fromSessionH, message, response);
        break;
#endif
    default:
        result = COAP_IGNORE;
        break;
    }

    coap_set_status_code(response, result);

    if (COAP_IGNORE < result && result < COAP_400_BAD_REQUEST)
    {
        result = NO_ERROR;
    }

    lwm2m_free(uriP);
    return result;
}

/* This function is an adaptation of function coap_receive() from Erbium's er-coap-13-engine.c.
 * Erbium is Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 */
void lwm2m_handle_packet(void* handler,
                         uint8_t * buffer,
                         int length,
                         void * fromSessionH)
{
    uint8_t coap_error_code = NO_ERROR;
    static lwm2m_coap_packet_t message[1];
    static lwm2m_coap_packet_t response[1];
    lwm2m_context_t * contextP = (lwm2m_context_t *) handler;

    lwm2m_log(LOG_DEBUG, "Entering\n");
    coap_error_code = coap_parse_message(message, buffer, (uint16_t)length);
    if (coap_error_code == NO_ERROR)
    {
        lwm2m_log(LOG_DEBUG, "Parsed: ver %u, type %u, tkl %u, code %u.%.2u, mid %u, Content type: %d\n",
                message->version, message->type, message->token_len, message->code >> 5, message->code & 0x1F, message->mid, message->content_type);
        lwm2m_log(LOG_DEBUG, "Payload: %.*s\n", message->payload_len, message->payload);
        if (message->code >= COAP_GET && message->code <= COAP_DELETE)
        {
            uint32_t block_num = 0;
            uint16_t block_size = REST_MAX_CHUNK_SIZE;
            uint32_t block_offset = 0;
            int64_t new_offset = 0;

            /* prepare response */
            if (message->type == COAP_TYPE_CON)
            {
                /* Reliable CON requests are answered with an ACK. */
                coap_init_message(response, COAP_TYPE_ACK, COAP_205_CONTENT, message->mid);
            }
            else
            {
                /* Unreliable NON requests are answered with a NON as well. */
                coap_init_message(response, COAP_TYPE_NON, COAP_205_CONTENT, contextP->nextMID++);
            }

            /* mirror token */
            if (message->token_len)
            {
                coap_set_header_token(response, message->token, message->token_len);
            }

            /* get offset for blockwise transfers */
            if (coap_get_header_block2(message, &block_num, NULL, &block_size, &block_offset))
            {
                lwm2m_log(LOG_DEBUG, "Blockwise: block request %u (%u/%u) @ %u bytes\n", block_num, block_size, REST_MAX_CHUNK_SIZE, block_offset);
                block_size = MIN(block_size, REST_MAX_CHUNK_SIZE);
                new_offset = block_offset;
            }

            /* handle block1 option */
            if (IS_OPTION(message, COAP_OPTION_BLOCK1))
            {
#ifdef LWM2M_CLIENT_MODE
                // get server
                lwm2m_server_t * serverP;
                serverP = utils_findServer(contextP, fromSessionH);
#ifdef LWM2M_BOOTSTRAP
                if (serverP == NULL)
                {
                    serverP = utils_findBootstrapServer(contextP, fromSessionH);
                }
#endif
                if (serverP == NULL)
                {
                    coap_error_code = COAP_500_INTERNAL_SERVER_ERROR;
                }
                else
                {
                    uint32_t block1_num;
                    uint8_t  block1_more;
                    uint16_t block1_size;
                    uint8_t * complete_buffer = NULL;
                    size_t complete_buffer_size;

                    // parse block1 header
                    coap_get_header_block1(message, &block1_num, &block1_more, &block1_size, NULL);
                    lwm2m_log(LOG_DEBUG, "Blockwise: block1 request NUM %u (SZX %u/ SZX Max%u) MORE %u\n", block1_num, block1_size, REST_MAX_CHUNK_SIZE, block1_more);

                    // handle block 1
                    coap_error_code = coap_block1_handler(&serverP->block1Data, message->mid, message->payload, message->payload_len, block1_size, block1_num, block1_more, &complete_buffer, &complete_buffer_size);

                    // if payload is complete, replace it in the coap message.
                    if (coap_error_code == NO_ERROR)
                    {
                        message->payload = complete_buffer;
                        message->payload_len = complete_buffer_size;
                    }
                    else if (coap_error_code == COAP_231_CONTINUE)
                    {
                        block1_size = MIN(block1_size, REST_MAX_CHUNK_SIZE);
                        coap_set_header_block1(response,block1_num, block1_more,block1_size);
                    }
                }
#else
                coap_error_code = COAP_501_NOT_IMPLEMENTED;
#endif
            }
            if (coap_error_code == NO_ERROR)
            {
                coap_error_code = handle_request(contextP, fromSessionH, message, response);
            }
            if (coap_error_code==NO_ERROR)
            {
                /* Save original payload pointer for later freeing. Payload in response may be updated. */
                uint8_t *payload = response->payload;
                if ( IS_OPTION(message, COAP_OPTION_BLOCK2) )
                {
                    /* unchanged new_offset indicates that resource is unaware of blockwise transfer */
                    if (new_offset==block_offset)
                    {
                        lwm2m_log(LOG_DEBUG, "Blockwise: unaware resource with payload length %u/%u\n", response->payload_len, block_size);
                        if (block_offset >= response->payload_len)
                        {
                            lwm2m_log(LOG_DEBUG, "handle_incoming_data(): block_offset >= response->payload_len\n");

                            response->code = COAP_402_BAD_OPTION;
                            coap_set_payload(response, "BlockOutOfScope", 15); /* a const char str[] and sizeof(str) produces larger code size */
                        }
                        else
                        {
                            coap_set_header_block2(response, block_num, response->payload_len - block_offset > block_size, block_size);
                            coap_set_payload(response, response->payload+block_offset, MIN(response->payload_len - block_offset, block_size));
                        } /* if (valid offset) */
                    }
                    else
                    {
                        /* resource provides chunk-wise data */
                        lwm2m_log(LOG_DEBUG, "Blockwise: blockwise resource, new offset %d\n", (int) new_offset);
                        coap_set_header_block2(response, block_num, new_offset!=-1 || response->payload_len > block_size, block_size);
                        if (response->payload_len > block_size) coap_set_payload(response, response->payload, block_size);
                    } /* if (resource aware of blockwise) */
                }
                else if (new_offset!=0)
                {
                    lwm2m_log(LOG_DEBUG, "Blockwise: no block option for blockwise resource, using block size %u\n", REST_MAX_CHUNK_SIZE);

                    coap_set_header_block2(response, 0, new_offset!=-1, REST_MAX_CHUNK_SIZE);
                    coap_set_payload(response, response->payload, MIN(response->payload_len, REST_MAX_CHUNK_SIZE));
                } /* if (blockwise request) */

                coap_error_code = message_send(contextP, response, fromSessionH);

                lwm2m_free(payload);
                response->payload = NULL;
                response->payload_len = 0;
            }
            else if (coap_error_code != COAP_IGNORE)
            {
                if (1 == coap_set_status_code(response, coap_error_code))
                {
                    coap_error_code = message_send(contextP, response, fromSessionH);
                }
            }
        }
        else
        {
            /* Responses */
            switch (message->type)
            {
            case COAP_TYPE_NON:
            case COAP_TYPE_CON:
                {
                    bool done = transaction_handleResponse(contextP, fromSessionH, message, response);

#ifdef LWM2M_SERVER_MODE
                    if (!done && IS_OPTION(message, COAP_OPTION_OBSERVE) &&
                        ((message->code == COAP_204_CHANGED) || (message->code == COAP_205_CONTENT)))
                    {
                        done = observe_handleNotify(contextP, fromSessionH, message, response);
                    }
#endif
                    if (!done && message->type == COAP_TYPE_CON )
                    {
                        coap_init_message(response, COAP_TYPE_ACK, 0, message->mid);
                        coap_error_code = message_send(contextP, response, fromSessionH);
                    }
                }
                break;

            case COAP_TYPE_RST:
                /* Cancel possible subscriptions. */
                handle_reset(contextP, fromSessionH, message);
                transaction_handleResponse(contextP, fromSessionH, message, NULL);
                break;

            case COAP_TYPE_ACK:
                transaction_handleResponse(contextP, fromSessionH, message, NULL);
                break;

            default:
                break;
            }
        } /* Request or Response */
        coap_free_header(message);
    } /* if (parsed correctly) */
    else
    {
        lwm2m_log(LOG_DEBUG, "Message parsing failed %u.%2u\n", coap_error_code >> 5, coap_error_code & 0x1F);
    }

    if (coap_error_code != NO_ERROR && coap_error_code != COAP_IGNORE)
    {
        lwm2m_log(LOG_DEBUG, "ERROR %u: %s\n", coap_error_code, coap_error_message);

        /* Set to sendable error code. */
        if (coap_error_code >= 192)
        {
            coap_error_code = COAP_500_INTERNAL_SERVER_ERROR;
        }
        /* Reuse input buffer for error message. */
        coap_init_message(message, COAP_TYPE_ACK, coap_error_code, message->mid);
        coap_set_payload(message, coap_error_message, strlen(coap_error_message));
        message_send(contextP, message, fromSessionH);
    }
}


uint8_t message_send(lwm2m_context_t * contextP,
                     lwm2m_coap_packet_t * message,
                     void * sessionH)
{
    uint8_t result = COAP_500_INTERNAL_SERVER_ERROR;
    uint8_t * pktBuffer;
    size_t pktBufferLen = 0;
    size_t allocLen;

    lwm2m_log(LOG_DEBUG, "Entering\n");
    allocLen = coap_serialize_get_size(message);
    lwm2m_log(LOG_DEBUG, "Size to allocate: %d\n", allocLen);
    if (allocLen == 0) return COAP_500_INTERNAL_SERVER_ERROR;

    pktBuffer = (uint8_t *)lwm2m_malloc(allocLen);
    if (pktBuffer != NULL)
    {
#ifdef LWM2M_LIBCOAP_ENABLED
        pktBufferLen = coap_serialize_message(message, pktBuffer, allocLen);
#else
        pktBufferLen = coap_serialize_message(message, pktBuffer);
#endif
        lwm2m_log(LOG_DEBUG, "coap_serialize_message() returned %d\n", pktBufferLen);
        if (0 != pktBufferLen)
        {
            result = lwm2m_buffer_send(sessionH, pktBuffer, pktBufferLen, contextP->userData);
        }
        lwm2m_free(pktBuffer);
    }

    return result;
}

#ifdef LWM2M_LIBCOAP_ENABLED
void
coap_add_multi_option(multi_option_t **dst, uint8_t *option, size_t option_len, uint8_t is_static)
{
  multi_option_t *opt = (multi_option_t *)lwm2m_malloc(sizeof(multi_option_t));

  if (opt)
  {
    opt->next = NULL;
    opt->len = (uint8_t)option_len;
    if (is_static)
    {
      opt->data = option;
      opt->is_static = 1;
    }
    else
    {
        opt->is_static = 0;
        opt->data = (uint8_t *)lwm2m_malloc(option_len);
        if (opt->data == NULL)
        {
            lwm2m_free(opt);
            return;
        }
        memcpy(opt->data, option, option_len);
    }

    if (*dst)
    {
      multi_option_t * i = *dst;
      while (i->next)
      {
        i = i->next;
      }
      i->next = opt;
    }
    else
    {
      *dst = opt;
    }
  }
}

void
free_multi_option(multi_option_t *dst)
{
  if (dst)
  {
    multi_option_t *n = dst->next;
    dst->next = NULL;
    if (dst->is_static == 0)
    {
        lwm2m_free(dst->data);
    }
    lwm2m_free(dst);
    free_multi_option(n);
  }
}

char * coap_get_multi_option_as_string(multi_option_t * option)
{
    size_t len = 0;
    multi_option_t * opt;
    char * output;

    for (opt = option; opt != NULL; opt = opt->next)
    {
       len += opt->len + 1;     // for separator
    }

    output = lwm2m_malloc(len + 1); // for String terminator
    if (output != NULL)
    {
        size_t i = 0;

        for (opt = option; opt != NULL; opt = opt->next)
        {
            output[i] = '/';
            i += 1;

            memmove(output + i, opt->data, opt->len);
            i += opt->len;
        }
        output[i] = 0;
    }

    return output;
}

void
coap_free_header(void *packet)
{
    lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

    free_multi_option(coap_pkt->uri_path);
    free_multi_option(coap_pkt->uri_query);
    free_multi_option(coap_pkt->location_path);
    coap_pkt->uri_path = NULL;
    coap_pkt->uri_query = NULL;
    coap_pkt->location_path = NULL;
}

/*-----------------------------------------------------------------------------------*/
size_t coap_serialize_get_size(void *packet)
{
    lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;
    size_t length = 0;

    length = COAP_HEADER_LEN + coap_pkt->payload_len + coap_pkt->token_len;

    if (IS_OPTION(coap_pkt, COAP_OPTION_IF_MATCH))
    {
        length += COAP_MAX_OPTION_HEADER_LEN + coap_pkt->if_match_len;
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_URI_HOST))
    {
        length += COAP_MAX_OPTION_HEADER_LEN + coap_pkt->uri_host_len;
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_ETAG))
    {
        length += COAP_MAX_OPTION_HEADER_LEN + coap_pkt->etag_len;
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_IF_NONE_MATCH))
    {
        // can be stored in extended fields
        length += COAP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_OBSERVE))
    {
        // can be stored in extended fields
        length += COAP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_URI_PORT))
    {
        // can be stored in extended fields
        length += COAP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_LOCATION_PATH))
    {
        multi_option_t * optP;

        for (optP = coap_pkt->location_path ; optP != NULL ; optP = optP->next)
        {
            length += COAP_MAX_OPTION_HEADER_LEN + optP->len;
        }
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_URI_PATH))
    {
        multi_option_t * optP;

        for (optP = coap_pkt->uri_path ; optP != NULL ; optP = optP->next)
        {
            length += COAP_MAX_OPTION_HEADER_LEN + optP->len;
        }
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_CONTENT_TYPE))
    {
        // can be stored in extended fields
        length += COAP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_MAX_AGE))
    {
        // can be stored in extended fields
        length += COAP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_URI_QUERY))
    {
        multi_option_t * optP;

        for (optP = coap_pkt->uri_query ; optP != NULL ; optP = optP->next)
        {
            length += COAP_MAX_OPTION_HEADER_LEN + optP->len;
        }
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_ACCEPT))
    {
        length += coap_pkt->accept_num * COAP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_LOCATION_QUERY))
    {
        length += COAP_MAX_OPTION_HEADER_LEN + coap_pkt->location_query_len;
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_BLOCK2))
    {
        // can be stored in extended fields
        length += COAP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_BLOCK1))
    {
        // can be stored in extended fields
        length += COAP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_SIZE))
    {
        // can be stored in extended fields
        length += COAP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_PROXY_URI))
    {
        length += COAP_MAX_OPTION_HEADER_LEN + coap_pkt->proxy_uri_len;
    }

    if (coap_pkt->payload_len)
    {
        // Account for the payload marker.
        length += 1;
    }

    return length;
}

/*-----------------------------------------------------------------------------------*/
/*- MEASSAGE PROCESSING -------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
void
coap_init_message(void *packet, coap_message_type_t type, uint8_t code, uint16_t mid)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  /* Important thing */
  memset(coap_pkt, 0, sizeof(lwm2m_coap_packet_t));

  coap_pkt->type = type;
  coap_pkt->code = code;
  coap_pkt->mid = mid;
}


/*-----------------------------------------------------------------------------------*/
static
int
coap_get_variable(const uint8_t *buffer, size_t length, const char *name, const char **output)
{
  const uint8_t *start = NULL;
  const uint8_t *end = NULL;
  const uint8_t *value_end = NULL;
  size_t name_len = 0;

  /*initialize the output buffer first*/
  *output = 0;

  name_len = strlen(name);
  end = buffer + length;

  for (start = buffer; start + name_len < end; ++start){
    if ((start == buffer || start[-1] == '&') && start[name_len] == '=' &&
        strncmp(name, (char *)start, name_len)==0) {

      /* Point start to variable value */
      start += name_len + 1;

      /* Point end to the end of the value */
      value_end = (const uint8_t *) memchr(start, '&', end - start);
      if (value_end == NULL) {
        value_end = end;
      }

      *output = (char *)start;

      return (value_end - start);
    }
  }

  return 0;
}

/*-----------------------------------------------------------------------------------*/
/*- REST FRAMEWORK FUNCTIONS --------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
int
coap_get_query_variable(void *packet, const char *name, const char **output)
{
/*
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (IS_OPTION(coap_pkt, COAP_OPTION_URI_QUERY)) {
    return coap_get_variable(coap_pkt->uri_query, coap_pkt->uri_query_len, name, output);
  }
*/
  return 0;
}

int
coap_get_post_variable(void *packet, const char *name, const char **output)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (coap_pkt->payload_len) {
    return coap_get_variable(coap_pkt->payload, coap_pkt->payload_len, name, output);
  }
  return 0;
}
/*-----------------------------------------------------------------------------------*/
int
coap_set_status_code(void *packet, unsigned int code)
{
  if (code <= 0xFF)
  {
    ((lwm2m_coap_packet_t *)packet)->code = (uint8_t) code;
    return 1;
  }
  else
  {
    return 0;
  }
}
/*-----------------------------------------------------------------------------------*/
/*- HEADER OPTION GETTERS AND SETTERS -----------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
unsigned int
coap_get_header_content_type(void *packet)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_CONTENT_TYPE)) return -1;

  return coap_pkt->content_type;
}

int
coap_set_header_content_type(void *packet, unsigned int content_type)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  coap_pkt->content_type = (coap_content_type_t) content_type;
  SET_OPTION(coap_pkt, COAP_OPTION_CONTENT_TYPE);
  return 1;
}
/*-----------------------------------------------------------------------------------*/
int
coap_get_header_accept(void *packet, const uint16_t **accept)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_ACCEPT)) return 0;

  *accept = coap_pkt->accept;
  return coap_pkt->accept_num;
}

int
coap_set_header_accept(void *packet, uint16_t accept)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (coap_pkt->accept_num < COAP_MAX_ACCEPT_NUM)
  {
    coap_pkt->accept[coap_pkt->accept_num] = accept;
    coap_pkt->accept_num += 1;

    SET_OPTION(coap_pkt, COAP_OPTION_ACCEPT);
  }
  return coap_pkt->accept_num;
}
/*-----------------------------------------------------------------------------------*/
int
coap_get_header_max_age(void *packet, uint32_t *age)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_MAX_AGE)) {
    *age = COAP_DEFAULT_MAX_AGE;
  } else {
    *age = coap_pkt->max_age;
  }
  return 1;
}

int
coap_set_header_max_age(void *packet, uint32_t age)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  coap_pkt->max_age = age;
  SET_OPTION(coap_pkt, COAP_OPTION_MAX_AGE);
  return 1;
}
/*-----------------------------------------------------------------------------------*/
int
coap_get_header_etag(void *packet, const uint8_t **etag)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_ETAG)) return 0;

  *etag = coap_pkt->etag;
  return coap_pkt->etag_len;
}

int
coap_set_header_etag(void *packet, const uint8_t *etag, size_t etag_len)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  coap_pkt->etag_len = (uint8_t)(MIN(COAP_ETAG_LEN, etag_len));
  memcpy(coap_pkt->etag, etag, coap_pkt->etag_len);

  SET_OPTION(coap_pkt, COAP_OPTION_ETAG);
  return coap_pkt->etag_len;
}
/*-----------------------------------------------------------------------------------*/
/*FIXME support multiple ETags */
int
coap_get_header_if_match(void *packet, const uint8_t **etag)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_IF_MATCH)) return 0;

  *etag = coap_pkt->if_match;
  return coap_pkt->if_match_len;
}

int
coap_set_header_if_match(void *packet, const uint8_t *etag, size_t etag_len)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  coap_pkt->if_match_len = (uint8_t)(MIN(COAP_ETAG_LEN, etag_len));
  memcpy(coap_pkt->if_match, etag, coap_pkt->if_match_len);

  SET_OPTION(coap_pkt, COAP_OPTION_IF_MATCH);
  return coap_pkt->if_match_len;
}
/*-----------------------------------------------------------------------------------*/
int
coap_get_header_if_none_match(void *packet)
{
  return IS_OPTION((lwm2m_coap_packet_t *)packet, COAP_OPTION_IF_NONE_MATCH) ? 1 : 0;
}

int
coap_set_header_if_none_match(void *packet)
{
  SET_OPTION((lwm2m_coap_packet_t *)packet, COAP_OPTION_IF_NONE_MATCH);
  return 1;
}
/*-----------------------------------------------------------------------------------*/
int
coap_get_header_token(void *packet, const uint8_t **token)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_TOKEN)) return 0;

  *token = coap_pkt->token;
  return coap_pkt->token_len;
}

int
coap_set_header_token(void *packet, const uint8_t *token, size_t token_len)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  coap_pkt->token_len = (uint8_t)(MIN(COAP_TOKEN_LEN, token_len));
  memcpy(coap_pkt->token, token, coap_pkt->token_len);

  SET_OPTION(coap_pkt, COAP_OPTION_TOKEN);
  return coap_pkt->token_len;
}
/*-----------------------------------------------------------------------------------*/
int
coap_get_header_proxy_uri(void *packet, const char **uri)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_PROXY_URI)) return 0;

  *uri = (const char *)coap_pkt->proxy_uri;
  return coap_pkt->proxy_uri_len;
}

int
coap_set_header_proxy_uri(void *packet, const char *uri)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  coap_pkt->proxy_uri = (uint8_t *)uri;
  coap_pkt->proxy_uri_len = strlen(uri);

  SET_OPTION(coap_pkt, COAP_OPTION_PROXY_URI);
  return coap_pkt->proxy_uri_len;
}
/*-----------------------------------------------------------------------------------*/
int
coap_get_header_uri_host(void *packet, const char **host)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_URI_HOST)) return 0;

  *host = (char *)coap_pkt->uri_host;
  return coap_pkt->uri_host_len;
}

int
coap_set_header_uri_host(void *packet, const char *host)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  coap_pkt->uri_host = (uint8_t *)host;
  coap_pkt->uri_host_len = strlen(host);

  SET_OPTION(coap_pkt, COAP_OPTION_URI_HOST);
  return coap_pkt->uri_host_len;
}
/*-----------------------------------------------------------------------------------*/
int
coap_get_header_uri_path(void *packet, const char **path)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_URI_PATH)) return 0;

  *path = NULL; //coap_pkt->uri_path;
  return 0; //coap_pkt->uri_path_len;
}

int
coap_set_header_uri_path(void *packet, const char *path)
{
  lwm2m_coap_packet_t *coap_pkt = (lwm2m_coap_packet_t *) packet;
  int length = 0;

  free_multi_option(coap_pkt->uri_path);
  coap_pkt->uri_path = NULL;

  if (path[0]=='/') ++path;

  do
  {
      int i = 0;

      while (path[i] != 0 && path[i] != '/') i++;
      coap_add_multi_option(&(coap_pkt->uri_path), (uint8_t *)path, i, 0);

      if (path[i] == '/') i++;
      path += i;
      length += i;
  } while (path[0] != 0);

  SET_OPTION(coap_pkt, COAP_OPTION_URI_PATH);
  return length;
}

int
coap_set_header_uri_path_segment(void *packet, const char *segment)
{
  lwm2m_coap_packet_t *coap_pkt = (lwm2m_coap_packet_t *) packet;
  int length;

  if (segment == NULL || segment[0] == 0)
  {
      coap_add_multi_option(&(coap_pkt->uri_path), NULL, 0, 1);
      length = 0;
  }
  else
  {
      length = strlen(segment);
      coap_add_multi_option(&(coap_pkt->uri_path), (uint8_t *)segment, length, 0);
  }

  SET_OPTION(coap_pkt, COAP_OPTION_URI_PATH);
  return length;
}
/*-----------------------------------------------------------------------------------*/
int
coap_get_header_uri_query(void *packet, const char **query)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_URI_QUERY)) return 0;

  *query = NULL; //coap_pkt->uri_query;
  return 0; //coap_pkt->uri_query_len;
}

int
coap_set_header_uri_query(void *packet, const char *query)
{
    int length = 0;
    lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

    free_multi_option(coap_pkt->uri_query);
    coap_pkt->uri_query = NULL;

    if (query[0]=='?') ++query;

    do
    {
        int i = 0;

        while (query[i] != 0 && query[i] != '&') i++;
        coap_add_multi_option(&(coap_pkt->uri_query), (uint8_t *)query, i, 0);

        if (query[i] == '&') i++;
        query += i;
        length += i;
    } while (query[0] != 0);

    SET_OPTION(coap_pkt, COAP_OPTION_URI_QUERY);
    return length;
 }
/*-----------------------------------------------------------------------------------*/
int
coap_get_header_location_path(void *packet, const char **path)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_LOCATION_PATH)) return 0;

  *path = NULL; //coap_pkt->location_path;
  return 0; //coap_pkt->location_path_len;
}

int
coap_set_header_location_path(void *packet, const char *path)
{
    lwm2m_coap_packet_t *coap_pkt = (lwm2m_coap_packet_t *) packet;
    int length = 0;

    free_multi_option(coap_pkt->location_path);
    coap_pkt->location_path = NULL;

    if (path[0]=='/') ++path;

    do
    {
        int i = 0;

        while (path[i] != 0 && path[i] != '/') i++;
        coap_add_multi_option(&(coap_pkt->location_path), (uint8_t *)path, i, 0);

        if (path[i] == '/') i++;
        path += i;
        length += i;
    } while (path[0] != 0);

    SET_OPTION(coap_pkt, COAP_OPTION_LOCATION_PATH);
    return length;
}
/*-----------------------------------------------------------------------------------*/
int
coap_get_header_location_query(void *packet, const char **query)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_LOCATION_QUERY)) return 0;

  *query = (const char*)coap_pkt->location_query;
  return coap_pkt->location_query_len;
}

int
coap_set_header_location_query(void *packet, char *query)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  while (query[0]=='?') ++query;

  coap_pkt->location_query = (uint8_t *)query;
  coap_pkt->location_query_len = strlen(query);

  SET_OPTION(coap_pkt, COAP_OPTION_LOCATION_QUERY);
  return coap_pkt->location_query_len;
}
/*-----------------------------------------------------------------------------------*/
int
coap_get_header_observe(void *packet, uint32_t *observe)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_OBSERVE)) return 0;

  *observe = coap_pkt->observe;
  return 1;
}

int
coap_set_header_observe(void *packet, uint32_t observe)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  coap_pkt->observe = 0x00FFFFFF & observe;
  SET_OPTION(coap_pkt, COAP_OPTION_OBSERVE);
  return 1;
}
/*-----------------------------------------------------------------------------------*/
int
coap_get_header_block2(void *packet, uint32_t *num, uint8_t *more, uint16_t *size, uint32_t *offset)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_BLOCK2)) return 0;

  /* pointers may be NULL to get only specific block parameters */
  if (num!=NULL) *num = coap_pkt->block2_num;
  if (more!=NULL) *more = coap_pkt->block2_more;
  if (size!=NULL) *size = coap_pkt->block2_size;
  if (offset!=NULL) *offset = coap_pkt->block2_offset;

  return 1;
}

int
coap_set_header_block2(void *packet, uint32_t num, uint8_t more, uint16_t size)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (size<16) return 0;
  if (size>2048) return 0;
  if (num>0x0FFFFF) return 0;

  coap_pkt->block2_num = num;
  coap_pkt->block2_more = more ? 1 : 0;
  coap_pkt->block2_size = size;

  SET_OPTION(coap_pkt, COAP_OPTION_BLOCK2);
  return 1;
}
/*-----------------------------------------------------------------------------------*/
int
coap_get_header_block1(void *packet, uint32_t *num, uint8_t *more, uint16_t *size, uint32_t *offset)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_BLOCK1)) return 0;

  /* pointers may be NULL to get only specific block parameters */
  if (num!=NULL) *num = coap_pkt->block1_num;
  if (more!=NULL) *more = coap_pkt->block1_more;
  if (size!=NULL) *size = coap_pkt->block1_size;
  if (offset!=NULL) *offset = coap_pkt->block1_offset;

  return 1;
}

int
coap_set_header_block1(void *packet, uint32_t num, uint8_t more, uint16_t size)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (size<16) return 0;
  if (size>2048) return 0;
  if (num>0x0FFFFF) return 0;

  coap_pkt->block1_num = num;
  coap_pkt->block1_more = more;
  coap_pkt->block1_size = size;

  SET_OPTION(coap_pkt, COAP_OPTION_BLOCK1);
  return 1;
}
/*-----------------------------------------------------------------------------------*/
int
coap_get_header_size(void *packet, uint32_t *size)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_SIZE)) return 0;

  *size = coap_pkt->size;
  return 1;
}

int
coap_set_header_size(void *packet, uint32_t size)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  coap_pkt->size = size;
  SET_OPTION(coap_pkt, COAP_OPTION_SIZE);
  return 1;
}
/*-----------------------------------------------------------------------------------*/
/*- PAYLOAD -------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
int
coap_get_payload(void *packet, const uint8_t **payload)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  if (coap_pkt->payload) {
    *payload = coap_pkt->payload;
    return coap_pkt->payload_len;
  } else {
    *payload = NULL;
    return 0;
  }
}

int
coap_set_payload(void *packet, const void *payload, size_t length)
{
  lwm2m_coap_packet_t *const coap_pkt = (lwm2m_coap_packet_t *) packet;

  coap_pkt->payload = (uint8_t *) payload;
  coap_pkt->payload_len = (uint16_t)(length);

  return coap_pkt->payload_len;
}
#endif
/*-----------------------------------------------------------------------------------*/

