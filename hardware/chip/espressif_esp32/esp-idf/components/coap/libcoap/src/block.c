/* block.c -- block transfer
 *
 * Copyright (C) 2010--2012,2015-2019 Olaf Bergmann <bergmann@tzi.org> and others
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap_config.h"

#if defined(HAVE_ASSERT_H) && !defined(assert)
# include <assert.h>
#endif

#include "libcoap.h"
#include "coap_debug.h"
#include "block.h"
#include "resource.h"
#include "coap_hashkey.h"

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef WITHOUT_BLOCK
unsigned int
coap_opt_block_num(const coap_opt_t *block_opt) {
  unsigned int num = 0;
  uint16_t len;

  len = coap_opt_length(block_opt);

  if (len == 0) {
    return 0;
  }

  if (len > 1) {
    num = coap_decode_var_bytes(coap_opt_value(block_opt),
                                coap_opt_length(block_opt) - 1);
  }

  return (num << 4) | ((*COAP_OPT_BLOCK_LAST(block_opt) & 0xF0) >> 4);
}

int
coap_get_block(coap_pdu_t *pdu, uint16_t type, coap_block_t *block) {
  coap_opt_iterator_t opt_iter;
  coap_opt_t *option;

  assert(block);
  memset(block, 0, sizeof(coap_block_t));

  if (pdu && (option = coap_check_option(pdu, type, &opt_iter)) != NULL) {
    unsigned int num;

    block->szx = COAP_OPT_BLOCK_SZX(option);
    if (COAP_OPT_BLOCK_MORE(option))
      block->m = 1;

    /* The block number is at most 20 bits, so values above 2^20 - 1
     * are illegal. */
    num = coap_opt_block_num(option);
    if (num > 0xFFFFF) {
      return 0;
    }
    block->num = num;
    return 1;
  }

  return 0;
}

int
coap_write_block_opt(coap_block_t *block, uint16_t type,
                     coap_pdu_t *pdu, size_t data_length) {
  size_t start, want, avail;
  unsigned char buf[4];

  assert(pdu);

  start = block->num << (block->szx + 4);
  if (data_length <= start) {
    coap_log(LOG_DEBUG, "illegal block requested\n");
    return -2;
  }

  assert(pdu->max_size > 0);
  avail = pdu->max_size - pdu->used_size - 4;
  want = (size_t)1 << (block->szx + 4);

  /* check if entire block fits in message */
  if (want <= avail) {
    block->m = want < data_length - start;
  } else {
    /* Sender has requested a block that is larger than the remaining
     * space in pdu. This is ok if the remaining data fits into the pdu
     * anyway. The block size needs to be adjusted only if there is more
     * data left that cannot be delivered in this message. */

    if (data_length - start <= avail) {

      /* it's the final block and everything fits in the message */
      block->m = 0;
    } else {
      unsigned int szx;
      int newBlockSize;

      /* we need to decrease the block size */
      if (avail < 16) {         /* bad luck, this is the smallest block size */
        coap_log(LOG_DEBUG,
                 "not enough space, even the smallest block does not fit");
        return -3;
      }
      newBlockSize = coap_flsll((long long)avail) - 5;
      coap_log(LOG_DEBUG,
               "decrease block size for %zu to %d\n", avail, newBlockSize);
      szx = block->szx;
      block->szx = newBlockSize;
      block->m = 1;
      block->num <<= szx - block->szx;
    }
  }

  /* to re-encode the block option */
  coap_add_option(pdu, type, coap_encode_var_safe(buf, sizeof(buf),
                                                  ((block->num << 4) |
                                                   (block->m << 3) |
                                                   block->szx)),
                  buf);

  return 1;
}

int
coap_add_block(coap_pdu_t *pdu, unsigned int len, const uint8_t *data,
               unsigned int block_num, unsigned char block_szx) {
  unsigned int start;
  start = block_num << (block_szx + 4);

  if (len <= start)
    return 0;

  return coap_add_data(pdu,
                       min(len - start, (1U << (block_szx + 4))),
                       data + start);
}

/*
 * Note that the COAP_OPTION_ have to be added in the correct order
 */
void
coap_add_data_blocked_response(coap_resource_t *resource,
                       coap_session_t *session,
                       coap_pdu_t *request,
                       coap_pdu_t *response,
                       const coap_binary_t *token,
                       uint16_t media_type,
                       int maxage,
                       size_t length,
                       const uint8_t* data
) {
  coap_key_t etag;
  unsigned char buf[4];
  coap_block_t block2 = { 0, 0, 0 };
  int block2_requested = 0;
  coap_subscription_t *subscription = coap_find_observer(resource, session, token);

  /*
   * Need to check that a valid block is getting asked for so that the
   * correct options are put into the PDU.
   */
  if (request) {
    if (coap_get_block(request, COAP_OPTION_BLOCK2, &block2)) {
      block2_requested = 1;
      if (length <= (block2.num << (block2.szx + 4))) {
        coap_log(LOG_DEBUG, "Illegal block requested (%d > last = %zu)\n",
                 block2.num,
                 length >> (block2.szx + 4));
        response->code = COAP_RESPONSE_CODE(400);
        goto error;
      }
    }
  }
  else if (subscription && subscription->has_block2) {
    block2 = subscription->block2;
    block2.num = 0;
    block2_requested = 1;
  }
  response->code = COAP_RESPONSE_CODE(205);

  /* add etag for the resource */
  memset(etag, 0, sizeof(etag));
  coap_hash(data, length, etag);
  coap_add_option(response, COAP_OPTION_ETAG, sizeof(etag), etag);

  if ((block2.num == 0) && subscription) {
    coap_add_option(response, COAP_OPTION_OBSERVE,
                    coap_encode_var_safe(buf, sizeof (buf),
                                         resource->observe),
                    buf);
  }

  coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
                  coap_encode_var_safe(buf, sizeof(buf),
                                       media_type),
                  buf);

  if (maxage >= 0) {
    coap_add_option(response,
                    COAP_OPTION_MAXAGE,
                    coap_encode_var_safe(buf, sizeof(buf), maxage), buf);
  }

  if (block2_requested) {
    int res;

    res = coap_write_block_opt(&block2, COAP_OPTION_BLOCK2, response,
                               length);

    switch (res) {
    case -2:                        /* illegal block (caught above) */
        response->code = COAP_RESPONSE_CODE(400);
        goto error;
    case -1:                        /* should really not happen */
        assert(0);
        /* fall through if assert is a no-op */
    case -3:                        /* cannot handle request */
        response->code = COAP_RESPONSE_CODE(500);
        goto error;
    default:                        /* everything is good */
        ;
    }

    coap_add_option(response,
                    COAP_OPTION_SIZE2,
                    coap_encode_var_safe(buf, sizeof(buf), length),
                    buf);

    coap_add_block(response, length, data,
                   block2.num, block2.szx);
    return;
  }

  /*
   * BLOCK2 not requested
   */
  if (!coap_add_data(response, length, data)) {
    /* set initial block size, will be lowered by
     * coap_write_block_opt) automatically */
    block2.num = 0;
    block2.szx = 6;
    coap_write_block_opt(&block2, COAP_OPTION_BLOCK2, response,
                         length);

    coap_add_option(response,
                    COAP_OPTION_SIZE2,
                    coap_encode_var_safe(buf, sizeof(buf), length),
                    buf);

    coap_add_block(response, length, data,
                   block2.num, block2.szx);
  }
  return;

error:
  coap_add_data(response,
                strlen(coap_response_phrase(response->code)),
                (const unsigned char *)coap_response_phrase(response->code));
}

#endif /* WITHOUT_BLOCK  */
