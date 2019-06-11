
#include <stdlib.h>

#include <string.h>
#include <stdio.h>

#include "lwm2m_coap.h"
#include "coap.h"

/*-----------------------------------------------------------------------------------*/
/*- Variables -----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/

coap_status_t coap_error_code = NO_ERROR;
const char *coap_error_message = "";

/*-----------------------------------------------------------------------------------*/
static
void
coap_merge_multi_option(uint8_t **dst, size_t *dst_len, uint8_t *option, size_t option_len, char separator)
{
  /* Merge multiple options. */
  if (*dst_len > 0)
  {
    /* dst already contains an option: concatenate */
    (*dst)[*dst_len] = separator;
    *dst_len += 1;

    /* memmove handles 2-byte option headers */
    memmove((*dst)+(*dst_len), option, option_len);

    *dst_len += option_len;
  }
  else
  {
    /* dst is empty: set to option */
    *dst = option;
    *dst_len = option_len;
  }
}

/*-----------------------------------------------------------------------------------*/
size_t
coap_serialize_message(void *packet, uint8_t *buffer, uint16_t buffer_len)
{
    lwm2m_coap_packet_t *coap_pkt = NULL;
    coap_pdu_t *pdu = NULL;
    unsigned char buf[4];
    size_t ret;

    if (!packet || !buffer) {
        return 0;
    }

    coap_pkt = (lwm2m_coap_packet_t *)packet;

    /* Initialize */
    coap_pkt->version = 1;

    pdu = coap_pdu_init(0, 0, 0, buffer_len);
    /* set header fields */
    pdu->type = coap_pkt->type;
    pdu->code = coap_pkt->code;
    pdu->tid = coap_pkt->mid;
    
     /* set Token */
    if (!coap_add_token(pdu, coap_pkt->token_len, coap_pkt->token)) {
        coap_log(LOG_ERR, "cannot add token to request\n");
        goto err;
    }
    
    /* FIXME: proto */
    if (!coap_pdu_encode_header(pdu, COAP_PROTO_DTLS)) {
       goto err;
    }

    /* add options */
    if (IS_OPTION(coap_pkt, COAP_OPTION_IF_MATCH)) {
        coap_add_option(pdu, COAP_OPTION_IF_MATCH, coap_pkt->if_match_len,
                         (const uint8_t *)coap_pkt->if_match);
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_URI_HOST)) {
        coap_add_option(pdu, COAP_OPTION_URI_HOST, coap_pkt->uri_host_len,
                        (const uint8_t *)coap_pkt->uri_host);
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_ETAG)) {
        coap_add_option(pdu, COAP_OPTION_ETAG, coap_pkt->etag_len,
                        (const uint8_t *)coap_pkt->etag);
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_IF_NONE_MATCH)) {
        coap_add_option(pdu, COAP_OPTION_IF_NONE_MATCH, 0, NULL);
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_OBSERVE)) {
        coap_add_option(pdu, COAP_OPTION_OBSERVE,
                        coap_encode_var_safe(buf, sizeof(buf),
                                             coap_pkt->observe),
                        buf); 
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_URI_PORT)) {
        coap_add_option(pdu, COAP_OPTION_URI_PORT,
                        coap_encode_var_safe(buf, sizeof(buf),
                                             coap_pkt->uri_port),
                        buf);
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_LOCATION_PATH)) {
        multi_option_t * optP;

        for (optP = coap_pkt->location_path ; optP != NULL ; optP = optP->next) {
            coap_add_option(pdu, COAP_OPTION_LOCATION_PATH, optP->len,
                            (const uint8_t *)(optP->data));
        }
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_URI_PATH)) {
        multi_option_t * optP;
        for (optP = coap_pkt->uri_path ; optP != NULL ; optP = optP->next) {
            coap_add_option(pdu, COAP_OPTION_URI_PATH, optP->len,
                            (const uint8_t *)(optP->data));
        }
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_CONTENT_TYPE)) {
        coap_add_option(pdu, COAP_OPTION_CONTENT_TYPE,
                        coap_encode_var_safe(buf, sizeof(buf),
                                             coap_pkt->content_type),
                        buf);
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_MAX_AGE)) {
        coap_add_option(pdu, COAP_OPTION_MAX_AGE, 
                        coap_encode_var_safe(buf, sizeof(buf),
                                             coap_pkt->max_age),
                        buf);
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_URI_QUERY)) {
        multi_option_t * optP;

        for (optP = coap_pkt->uri_query ; optP != NULL ; optP = optP->next) {
            coap_add_option(pdu, COAP_OPTION_URI_QUERY, optP->len,
                            (const uint8_t *)(optP->data));
        }
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_ACCEPT)) {
        int i;

        for (i = 0; i < coap_pkt->accept_num; i++) {
            coap_add_option(pdu, COAP_OPTION_ACCEPT, 
                            coap_encode_var_safe(buf, sizeof(buf),
                                                 coap_pkt->accept[i]),
                            buf);
        }
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_LOCATION_QUERY))  {
        coap_add_option(pdu, COAP_OPTION_LOCATION_QUERY, coap_pkt->location_query_len,
                        (const uint8_t *)coap_pkt->location_query);
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_BLOCK2)) {
        coap_block_t block2;
        block2.num = coap_pkt->block2_num;
        block2.m = coap_pkt->block2_more;
        block2.szx = coap_pkt->block2_size;
        coap_write_block_opt(&block2, COAP_OPTION_BLOCK2, pdu,
                             coap_pkt->payload_len);
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_BLOCK1)) {
        coap_block_t block1;
        block1.num = coap_pkt->block1_num;
        block1.m = coap_pkt->block1_more;
        block1.szx = coap_pkt->block1_size;
        coap_write_block_opt(&block1, COAP_OPTION_BLOCK1, pdu,
                             coap_pkt->payload_len);
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_SIZE)) {
        coap_add_option(pdu, COAP_OPTION_SIZE,
                        coap_encode_var_safe(buf, sizeof(buf),
                                             coap_pkt->size),
                        buf);
    }
    if (IS_OPTION(coap_pkt, COAP_OPTION_PROXY_URI)) {
        coap_add_option(pdu, COAP_OPTION_PROXY_URI, coap_pkt->proxy_uri_len,
                        (const uint8_t *)coap_pkt->proxy_uri);
    }
  
    /* Pack payload */
    if (coap_pkt->payload_len) {
        coap_add_data(pdu, coap_pkt->payload_len, coap_pkt->payload);
    }

    memcpy(buffer, pdu->token - pdu->hdr_size, pdu->used_size + pdu->hdr_size);                                

    ret = pdu->used_size + pdu->hdr_size; /* packet length */
    coap_delete_pdu(pdu);
    return ret;

err:
    coap_delete_pdu(pdu);
    return 0;
}

static uint8_t coap_get_version(coap_pdu_t *pdu)
{
    uint8_t *hdr = pdu->token - pdu->hdr_size;
    /* default COAP_PROTO_UDP COAP_PROTO_DTLS */
    return hdr[0] >> 6;
}

/*-----------------------------------------------------------------------------------*/
coap_status_t
coap_parse_message(void *packet, uint8_t *data, uint16_t data_len)
{
    int ret = NO_ERROR;
    lwm2m_coap_packet_t *coap_pkt = NULL;

    coap_pdu_t *pdu = NULL;
    coap_opt_iterator_t opt_iter;
    coap_opt_t *option = NULL;

    if (!packet || !data) {
        return -1;
    }

    coap_pkt = (lwm2m_coap_packet_t *)packet;

     /* Initialize packet */
    memset(coap_pkt, 0, sizeof(lwm2m_coap_packet_t));

    /* pointer to packet bytes */
    coap_pkt->buffer = data;

    pdu = coap_pdu_init(0, 0, 0, data_len);
    if (!pdu) {
        ret = BAD_REQUEST_4_00;
        goto err;
    }

    /* FIXME: proto */
    if (!coap_pdu_parse(COAP_PROTO_DTLS, data, data_len, pdu)) {
        coap_log(LOG_WARNING, "discard malformed PDU\n");
        ret = BAD_REQUEST_4_00;
        goto err;
    }

    /* parse header fields */
    coap_pkt->version = coap_get_version(pdu);
    coap_pkt->type = pdu->type;
    coap_pkt->token_len = pdu->token_length;
    coap_pkt->code = pdu->code;
    coap_pkt->mid = pdu->tid;

    if (pdu->data) {
        coap_pkt->payload_len = pdu->used_size - (pdu->data - pdu->token);
        coap_pkt->payload = data + pdu->hdr_size + (pdu->data - pdu->token);
    } else {
        coap_pkt->payload_len  = 0;
        coap_pkt->payload  = NULL;
    }

    if (coap_pkt->token_len != 0) {
        memcpy(coap_pkt->token, pdu->token, coap_pkt->token_len);
        SET_OPTION(coap_pkt, COAP_OPTION_TOKEN);
    }

    coap_option_iterator_init(pdu, &opt_iter, COAP_OPT_ALL);

    while ((option = coap_option_next(&opt_iter))) {
        SET_OPTION(coap_pkt, opt_iter.type);

        switch(opt_iter.type) {
            case COAP_OPTION_CONTENT_TYPE:
                coap_pkt->content_type = coap_decode_var_bytes(coap_opt_value(option),
                                                               coap_opt_length(option));
                break;
            case COAP_OPTION_MAX_AGE:
                coap_pkt->max_age = coap_decode_var_bytes(coap_opt_value(option),
                                                          coap_opt_length(option));

                break;
            case COAP_OPTION_ETAG:
                coap_pkt->etag_len = (uint8_t)(MIN(COAP_ETAG_LEN, coap_opt_length(option)));
                memcpy(coap_pkt->etag, coap_opt_value(option), coap_pkt->etag_len);
                break;
            case COAP_OPTION_ACCEPT:
              if (coap_pkt->accept_num < COAP_MAX_ACCEPT_NUM) {
                  coap_pkt->accept[coap_pkt->accept_num] = coap_decode_var_bytes(coap_opt_value(option),
                                                                                 coap_opt_length(option));
                  coap_pkt->accept_num += 1;
              }
              break;
            case COAP_OPTION_IF_MATCH:
              coap_pkt->if_match_len = (uint8_t)(MIN(COAP_ETAG_LEN, coap_opt_length(option)));
              memcpy(coap_pkt->if_match, coap_opt_value(option), coap_pkt->if_match_len);
              break;
            case COAP_OPTION_IF_NONE_MATCH:
                coap_pkt->if_none_match = 1;
                break;
            case COAP_OPTION_URI_HOST:
                coap_pkt->uri_host = coap_opt_value(option);
                coap_pkt->uri_host_len = coap_opt_length(option);
                break;
            case COAP_OPTION_URI_PORT:
                coap_pkt->uri_port = coap_decode_var_bytes(coap_opt_value(option),
                                                           coap_opt_length(option));
                break;
            case COAP_OPTION_URI_PATH:
                coap_add_multi_option( &(coap_pkt->uri_path), coap_opt_value(option),  coap_opt_length(option), 1);
                break;
            case COAP_OPTION_URI_QUERY:
                coap_add_multi_option( &(coap_pkt->uri_query), coap_opt_value(option),  coap_opt_length(option), 1);
                break;
            case COAP_OPTION_LOCATION_PATH:
                coap_add_multi_option( &(coap_pkt->location_path), coap_opt_value(option),  coap_opt_length(option), 1);
                break;
            case COAP_OPTION_LOCATION_QUERY:
                coap_merge_multi_option( &(coap_pkt->location_query), &(coap_pkt->location_query_len), coap_opt_value(option), coap_opt_length(option), '&');
                break;
            case COAP_OPTION_PROXY_URI:
              /*FIXME check for own end-point */
              coap_pkt->proxy_uri = coap_opt_value(option);
              coap_pkt->proxy_uri_len = coap_opt_length(option);
              /*TODO length > 270 not implemented (actually not required) */
              coap_error_message = "This is a constrained server (Contiki)";
              return PROXYING_NOT_SUPPORTED_5_05;
              break;
            case COAP_OPTION_OBSERVE:
                coap_pkt->observe = coap_decode_var_bytes(coap_opt_value(option),
                                                          coap_opt_length(option));
                break;
            case COAP_OPTION_BLOCK2:
              coap_pkt->block2_num = coap_decode_var_bytes(coap_opt_value(option),
                                                           coap_opt_length(option));
              coap_pkt->block2_more = (coap_pkt->block2_num & 0x08)>>3;
              coap_pkt->block2_size = 16 << (coap_pkt->block2_num & 0x07);
              coap_pkt->block2_offset = (coap_pkt->block2_num & ~0x0000000F)<<(coap_pkt->block2_num & 0x07);
              coap_pkt->block2_num >>= 4;
              break;
            case COAP_OPTION_BLOCK1:
              coap_pkt->block1_num = coap_decode_var_bytes(coap_opt_value(option),
                                                           coap_opt_length(option));
              coap_pkt->block1_more = (coap_pkt->block1_num & 0x08)>>3;
              coap_pkt->block1_size = 16 << (coap_pkt->block1_num & 0x07);
              coap_pkt->block1_offset = (coap_pkt->block1_num & ~0x0000000F)<<(coap_pkt->block1_num & 0x07);
              coap_pkt->block1_num >>= 4;
              break;
            case COAP_OPTION_SIZE:
              coap_pkt->size = coap_decode_var_bytes(coap_opt_value(option),
                                                     coap_opt_length(option));
              break;
            default:
              /* Check if critical (odd) */
              if (opt_iter.type & 1) {
                  ret = BAD_OPTION_4_02;
                  coap_error_message = "Unsupported critical option";
                  goto err;
              }
        }
    }

    coap_delete_pdu(pdu);
    return ret;

err:
    coap_delete_pdu(pdu);
    return ret;
}
