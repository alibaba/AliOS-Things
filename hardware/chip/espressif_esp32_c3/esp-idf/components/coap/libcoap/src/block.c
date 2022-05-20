/* block.c -- block transfer
 *
 * Copyright (C) 2010--2012,2015-2019 Olaf Bergmann <bergmann@tzi.org> and others
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap3/coap_internal.h"

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

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
coap_get_block(const coap_pdu_t *pdu, coap_option_num_t number,
               coap_block_t *block) {
  coap_opt_iterator_t opt_iter;
  coap_opt_t *option;

  assert(block);
  memset(block, 0, sizeof(coap_block_t));

  if (pdu && (option = coap_check_option(pdu, number, &opt_iter)) != NULL) {
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
coap_write_block_opt(coap_block_t *block, coap_option_num_t number,
                     coap_pdu_t *pdu, size_t data_length) {
  size_t start, want, avail;
  unsigned char buf[4];

  assert(pdu);

  start = block->num << (block->szx + 4);
  if (block->num != 0 && data_length <= start) {
    coap_log(LOG_DEBUG, "illegal block requested\n");
    return -2;
  }

  assert(pdu->max_size > 0);
  avail = pdu->max_size - pdu->used_size - pdu->hdr_size;
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
                 "not enough space, even the smallest block does not fit\n");
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
  coap_insert_option(pdu, number, coap_encode_var_safe(buf, sizeof(buf),
                                                       ((block->num << 4) |
                                                       (block->m << 3) |
                                                       block->szx)),
                  buf);

  return 1;
}

int
coap_add_block(coap_pdu_t *pdu, size_t len, const uint8_t *data,
               unsigned int block_num, unsigned char block_szx) {
  unsigned int start;
  start = block_num << (block_szx + 4);

  if (len <= start)
    return 0;

  return coap_add_data(pdu,
                       min(len - start, ((size_t)1 << (block_szx + 4))),
                       data + start);
}

/*
 * Note that the COAP_OPTION_ have to be added in the correct order
 */
void
coap_add_data_blocked_response(const coap_pdu_t *request,
                       coap_pdu_t *response,
                       uint16_t media_type,
                       int maxage,
                       size_t length,
                       const uint8_t* data
) {
  coap_key_t etag;
  unsigned char buf[4];
  coap_block_t block2 = { 0, 0, 0 };
  int block2_requested = 0;

  /*
   * Need to check that a valid block is getting asked for so that the
   * correct options are put into the PDU.
   */
  if (request) {
    if (coap_get_block(request, COAP_OPTION_BLOCK2, &block2)) {
      block2_requested = 1;
      if (block2.num != 0 && length <= (block2.num << (block2.szx + 4))) {
        coap_log(LOG_DEBUG, "Illegal block requested (%d > last = %zu)\n",
                 block2.num,
                 length >> (block2.szx + 4));
        response->code = COAP_RESPONSE_CODE(400);
        goto error;
      }
    }
  }
  response->code = COAP_RESPONSE_CODE(205);

  /* add etag for the resource */
  memset(etag, 0, sizeof(etag));
  coap_hash(data, length, etag);
  coap_add_option(response, COAP_OPTION_ETAG, sizeof(etag), etag);

  coap_insert_option(response, COAP_OPTION_CONTENT_FORMAT,
                  coap_encode_var_safe(buf, sizeof(buf),
                                       media_type),
                  buf);

  if (maxage >= 0) {
    coap_insert_option(response,
                    COAP_OPTION_MAXAGE,
                    coap_encode_var_safe(buf, sizeof(buf), maxage), buf);
  }

  if (block2_requested) {
    int res;

    res = coap_write_block_opt(&block2, COAP_OPTION_BLOCK2, response, length);

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
                    coap_encode_var_safe8(buf, sizeof(buf), length),
                    buf);

    coap_add_block(response, length, data,
                   block2.num, block2.szx);
    return;
  }

  /*
   * BLOCK2 not requested
   */
  if (!coap_add_data(response, length, data)) {
    /*
     * Insufficient space to add in data - use block mode
     * set initial block size, will be lowered by
     * coap_write_block_opt() automatically
     */
    block2.num = 0;
    block2.szx = 6;
    coap_write_block_opt(&block2, COAP_OPTION_BLOCK2, response, length);

    coap_add_option(response,
                    COAP_OPTION_SIZE2,
                    coap_encode_var_safe8(buf, sizeof(buf), length),
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

void
coap_context_set_block_mode(coap_context_t *context,
                                  uint8_t block_mode) {
  context->block_mode = block_mode &= (COAP_BLOCK_USE_LIBCOAP |
                                       COAP_BLOCK_SINGLE_BODY);
  if (!(block_mode & COAP_BLOCK_USE_LIBCOAP))
    context->block_mode = 0;
}

/*
 * The block token match only matches on the bottom 32 bits
 * [The upper 32 bits are incremented as different payloads are sent]
 *
 */
COAP_STATIC_INLINE int
block_token_match(const uint8_t *a, size_t alen,
  const uint8_t *b, size_t blen) {
  size_t bias;
  if (blen < 4)
    return alen == blen && memcmp(a, b, blen) == 0;
  bias = blen - 4;
  return alen == blen && memcmp(a+bias, b+bias, 4) == 0;
}

COAP_STATIC_INLINE int
full_match(const uint8_t *a, size_t alen,
  const uint8_t *b, size_t blen) {
  return alen == blen && (alen == 0 || memcmp(a, b, alen) == 0);
}

int
coap_cancel_observe(coap_session_t *session, coap_binary_t *token,
                    coap_pdu_type_t type) {
  coap_lg_crcv_t *cq;

  assert(session);
  if (!session)
    return 0;

  if (!(session->block_mode & COAP_BLOCK_USE_LIBCOAP)) {
    coap_log(LOG_DEBUG,
             "** %s: coap_cancel_observe: COAP_BLOCK_USE_LIBCOAP not enabled\n",
             coap_session_str(session));
    return 0;
  }

  LL_FOREACH(session->lg_crcv, cq) {
    if (cq->observe_set) {
      if ((!token && !cq->app_token->length) || (token &&
          full_match(token->s, token->length, cq->app_token->s,
                     cq->app_token->length))) {
        uint8_t buf[4];
        coap_mid_t mid;
        size_t size;
        const uint8_t *data;
        coap_pdu_t * pdu = coap_pdu_duplicate(&cq->pdu,
                                              session,
                                              cq->base_token_length,
                                              cq->base_token,
                                              NULL);

        cq->observe_set = 0;
        if (pdu == NULL)
          return 0;
        /* Need to make sure that this is the correct type */
        pdu->type = type;

        if (coap_get_data(&cq->pdu, &size, &data)) {
          coap_add_data(pdu, size, data);
        }
        coap_update_option(pdu, COAP_OPTION_OBSERVE,
                           coap_encode_var_safe(buf, sizeof(buf),
                                                COAP_OBSERVE_CANCEL),
                           buf);
        mid = coap_send_internal(session, pdu);
        if (mid != COAP_INVALID_MID)
          return 1;
        break;
      }
    }
  }
  return 0;
}

int
coap_add_data_large_internal(coap_session_t *session,
                             coap_pdu_t *pdu,
                             coap_resource_t *resource,
                             const coap_string_t *query,
                             int maxage,
                             uint64_t etag,
                             size_t length,
                             const uint8_t *data,
                             coap_release_large_data_t release_func,
                             void *app_ptr) {

  ssize_t avail;
  coap_block_t block;
  size_t chunk;
  coap_lg_xmit_t *lg_xmit = NULL;
  uint8_t buf[8];
  int have_block_defined = 0;
  uint8_t blk_size;
  uint16_t option;

  assert(pdu);

  if (!(session->block_mode & COAP_BLOCK_USE_LIBCOAP)) {
    coap_log(LOG_DEBUG,
             "** %s: coap_add_data_large: COAP_BLOCK_USE_LIBCOAP not enabled\n",
             coap_session_str(session));
    goto add_data;
  }

/* Block NUM max 20 bits and block size is "2**(SZX + 4)" and SZX max of 6 */
#define MAX_BLK_LEN (((1 << 20) - 1) * (1 << (6 + 4)))

  if (length > MAX_BLK_LEN) {
    coap_log(LOG_WARNING,
             "Size of large buffer restricted to 0x%x bytes\n", MAX_BLK_LEN);
    length = MAX_BLK_LEN;
  }
  /* Determine the block size to use, adding in sensible options if needed */
  if (COAP_PDU_IS_REQUEST(pdu)) {
    coap_lg_xmit_t *q;

    option = COAP_OPTION_BLOCK1;

    /* See if this token is already in use for large bodies (unlikely) */
    LL_FOREACH_SAFE(session->lg_xmit, lg_xmit, q) {
      if (full_match(pdu->token, pdu->token_length,
                     lg_xmit->b.b1.app_token->s,
                     lg_xmit->b.b1.app_token->length)) {
        /* Unfortunately need to free this off as potential size change */
        LL_DELETE(session->lg_xmit, lg_xmit);
        coap_block_delete_lg_xmit(session, lg_xmit);
        lg_xmit = NULL;
        break;
      }
    }
  }
  else {
    /* Have to assume that it is a response even if code is 0.00 */
    coap_lg_xmit_t *q;
    coap_string_t empty = { 0, NULL};

    assert(resource);
    option = COAP_OPTION_BLOCK2;

    /* Check if resource+query is already in use for large bodies (unlikely) */
    LL_FOREACH_SAFE(session->lg_xmit, lg_xmit, q) {
      if (resource == lg_xmit->b.b2.resource &&
          coap_string_equal(query ? query : &empty,
                         lg_xmit->b.b2.query ? lg_xmit->b.b2.query : &empty)) {
        /* Unfortunately need to free this off as potential size change */
        LL_DELETE(session->lg_xmit, lg_xmit);
        coap_block_delete_lg_xmit(session, lg_xmit);
        lg_xmit = NULL;
        break;
      }
    }
  }

  avail = pdu->max_size - pdu->used_size - pdu->hdr_size;
  /* May need token of length 8, so account for this */
  avail -= (pdu->token_length <= 8) ? pdu->token_length <= 8 : 0;
  blk_size = coap_flsll((long long)avail) - 4 - 1;

  /* see if BLOCKx defined - if so update blk_size as given by app */
  if (coap_get_block(pdu, option, &block)) {
    if (block.szx < blk_size)
      blk_size = block.szx;
    have_block_defined = 1;
  }

  if (avail < 16 && ((ssize_t)length > avail || have_block_defined)) {
    /* bad luck, this is the smallest block size */
    coap_log(LOG_DEBUG,
                "not enough space, even the smallest block does not fit\n");
    goto fail;
  }

  chunk = (size_t)1 << (blk_size + 4);
  if (have_block_defined && block.num != 0) {
    /* App is defining a single block to send */
    size_t rem;

    pdu->body_data = data;
    pdu->body_length = length;
    coap_log(LOG_DEBUG, "PDU presented by app\n");
    coap_show_pdu(LOG_DEBUG, pdu);
    pdu->body_data = NULL;
    pdu->body_length = 0;
    if (length >= block.num * chunk) {
      rem = chunk;
      if (chunk > length - block.num * chunk)
        rem = length - block.num * chunk;
      if (!coap_add_data(pdu, rem, &data[block.num * chunk]))
        goto fail;
    }
    if (release_func)
      release_func(session, app_ptr);
  }
  else if ((have_block_defined && length > chunk) || (ssize_t)length > avail) {
    /* Only add in lg_xmit if more than one block needs to be handled */
    uint64_t token;
    size_t rem;

    lg_xmit = coap_malloc_type(COAP_LG_XMIT, sizeof(coap_lg_xmit_t));
    if (!lg_xmit)
      goto fail;

    coap_log(LOG_DEBUG, "** %s: lg_xmit %p initialized\n",
             coap_session_str(session), (void*)lg_xmit);
    /* Set up for displaying all the data in the pdu */
    pdu->body_data = data;
    pdu->body_length = length;
    coap_log(LOG_DEBUG, "PDU presented by app\n");
    coap_show_pdu(LOG_DEBUG, pdu);
    pdu->body_data = NULL;
    pdu->body_length = 0;
    /* Update lg_xmit with large data information */
    lg_xmit->blk_size = blk_size;
    lg_xmit->option = option;
    lg_xmit->data = data;
    lg_xmit->length = length;
    lg_xmit->offset = 0;
    lg_xmit->release_func = release_func;
    lg_xmit->last_payload = 0;
    lg_xmit->last_used = 0;
    lg_xmit->app_ptr = app_ptr;
    if (COAP_PDU_IS_REQUEST(pdu)) {
      /* Need to keep original token for updating response PDUs */
      lg_xmit->b.b1.app_token = coap_new_binary(pdu->token_length);
      if (!lg_xmit->b.b1.app_token)
        goto fail;
      memcpy(lg_xmit->b.b1.app_token->s, pdu->token, pdu->token_length);
      /*
       * Need to set up new token for use during transmits
       */
      lg_xmit->b.b1.count = 1;
      token = ((++session->tx_token) & 0xffffffff) +
              ((uint64_t)lg_xmit->b.b1.count << 32);
      memset(lg_xmit->b.b1.token, 0, sizeof(lg_xmit->b.b1.token));
      lg_xmit->b.b1.token_length = coap_encode_var_safe8(lg_xmit->b.b1.token,
                                                         sizeof(token), token);
      /*
       * Token will be updated in pdu later as original pdu may be needed in
       * coap_send()
       */
      coap_update_option(pdu,
                         COAP_OPTION_SIZE1,
                         coap_encode_var_safe(buf, sizeof(buf),
                                              (unsigned int)length),
                         buf);
    }
    else {
      /*
       * resource+query match is used for BLOCK2 large body transmissions
       * token match is used for BLOCK1 large body transmissions
       */
      lg_xmit->b.b2.resource = resource;
      if (query) {
        lg_xmit->b.b2.query = coap_new_string(query->length);
        if (lg_xmit->b.b2.query) {
          memcpy(lg_xmit->b.b2.query->s, query->s, query->length);
        }
      }
      else {
        lg_xmit->b.b2.query = NULL;
      }
      lg_xmit->b.b2.etag = etag;
      if (maxage >= 0) {
        coap_tick_t now;

        coap_ticks(&now);
        lg_xmit->b.b2.maxage_expire = coap_ticks_to_rt(now) + maxage;
      }
      else {
        lg_xmit->b.b2.maxage_expire = 0;
      }
      coap_update_option(pdu,
                         COAP_OPTION_SIZE2,
                         coap_encode_var_safe(buf, sizeof(buf),
                                              (unsigned int)length),
                         buf);
      if (etag == 0) {
        if (++session->context->etag == 0)
          ++session->context->etag;
        etag = session->context->etag;
      }
      coap_update_option(pdu,
                         COAP_OPTION_ETAG,
                         coap_encode_var_safe8(buf, sizeof(buf), etag),
                         buf);
    }

    /* Add in with requested block num, more bit and block size */
    block.m = ((block.num + 1) * chunk) < lg_xmit->length;
    coap_update_option(pdu,
                       lg_xmit->option,
                       coap_encode_var_safe(buf, sizeof(buf),
                        (block.num << 4) | (block.m << 3) | lg_xmit->blk_size),
                       buf);

    /* Set up skeletal PDU to use as a basis for all the subsequent blocks */
    memcpy(&lg_xmit->pdu, pdu, sizeof(lg_xmit->pdu));
    lg_xmit->pdu.token = coap_malloc_type(COAP_PDU_BUF,
                           8 + lg_xmit->pdu.used_size + lg_xmit->pdu.hdr_size);
    if (!lg_xmit->pdu.token)
      goto fail;

    lg_xmit->pdu.alloc_size = 8 + lg_xmit->pdu.used_size +
                              lg_xmit->pdu.hdr_size;
    lg_xmit->pdu.token += lg_xmit->pdu.hdr_size;
    memcpy(lg_xmit->pdu.token, pdu->token, lg_xmit->pdu.used_size);
    if (pdu->data)
      lg_xmit->pdu.data = lg_xmit->pdu.token + (pdu->data - pdu->token);

    /* Check we still have space after adding in some options */
    avail = pdu->max_size - pdu->used_size - pdu->hdr_size;
    /* May need token of length 8, so account for this */
    avail -= (pdu->token_length <= 8) ? pdu->token_length <= 8 : 0;
    if (avail < (ssize_t)chunk) {
      /* chunk size change down */
      if (avail < 16) {
        coap_log(LOG_DEBUG,
                "not enough space, even the smallest block does not fit\n");
        goto fail;
      }
      blk_size = coap_flsll((long long)avail) - 4 - 1;
      block.num = block.num << (lg_xmit->blk_size - blk_size);
      lg_xmit->blk_size = blk_size;
      chunk = (size_t)1 << (lg_xmit->blk_size + 4);
      coap_update_option(pdu,
                  lg_xmit->option,
                  coap_encode_var_safe(buf, sizeof(buf),
                        (block.num << 4) | (block.m << 3) | lg_xmit->blk_size),
                   buf);
    }

    rem = chunk;
    if (chunk > lg_xmit->length - block.num * chunk)
      rem = lg_xmit->length - block.num * chunk;
    if (!coap_add_data(pdu, rem, &data[block.num * chunk]))
      goto fail;

    lg_xmit->last_block = -1;

    /* Link the new lg_xmit in */
    LL_PREPEND(session->lg_xmit,lg_xmit);
  }
  else {
    /* No need to use blocks */
    if (have_block_defined) {
      coap_update_option(pdu,
                  option,
                  coap_encode_var_safe(buf, sizeof(buf),
                     (0 << 4) | (0 << 3) | blk_size), buf);
    }
add_data:
    if (!coap_add_data(pdu, length, data))
      goto fail;

    if (release_func)
      release_func(session, app_ptr);
  }
  return 1;

fail:
  if (lg_xmit) {
    coap_block_delete_lg_xmit(session, lg_xmit);
  }
  if (release_func)
    release_func(session, app_ptr);
  return 0;
}

int
coap_add_data_large_request(coap_session_t *session,
                            coap_pdu_t *pdu,
                            size_t length,
                            const uint8_t *data,
                            coap_release_large_data_t release_func,
                            void *app_ptr) {
  return coap_add_data_large_internal(session, pdu, NULL, NULL, -1,
                                 0, length, data, release_func, app_ptr);
}

int
coap_add_data_large_response(coap_resource_t *resource,
                             coap_session_t *session,
                             const coap_pdu_t *request,
                             coap_pdu_t *response,
                             const coap_string_t *query,
                             uint16_t media_type,
                             int maxage,
                             uint64_t etag,
                             size_t length,
                             const uint8_t *data,
                             coap_release_large_data_t release_func,
                             void *app_ptr
) {
  unsigned char buf[4];
  coap_block_t block = { 0, 0, 0 };
  int block_requested = 0;
  uint16_t block_opt = COAP_OPTION_BLOCK2;

  /*
   * Need to check that a valid block is getting asked for so that the
   * correct options are put into the PDU.
   */
  if (request) {
    if (coap_get_block(request, COAP_OPTION_BLOCK2, &block)) {
      block_requested = 1;
      if (block.num != 0 && length <= (block.num << (block.szx + 4))) {
        coap_log(LOG_DEBUG, "Illegal block requested (%d > last = %zu)\n",
                 block.num,
                 length >> (block.szx + 4));
        response->code = COAP_RESPONSE_CODE(400);
        goto error;
      }
    }
  }

  coap_insert_option(response, COAP_OPTION_CONTENT_TYPE,
                     coap_encode_var_safe(buf, sizeof(buf),
                                          media_type),
                     buf);

  if (maxage >= 0) {
    coap_insert_option(response,
                       COAP_OPTION_MAXAGE,
                       coap_encode_var_safe(buf, sizeof(buf), maxage), buf);
  }

  if (block_requested) {
    int res;

    res = coap_write_block_opt(&block, block_opt, response,
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

    if (!coap_add_data_large_internal(session, response, resource, query,
                                      maxage, etag, length, data,
                                      release_func, app_ptr)) {
      response->code = COAP_RESPONSE_CODE(500);
      goto error;
    }

    return 1;
  }

  /*
   * BLOCK2 not requested
   */
  if (!coap_add_data_large_internal(session, response, resource, query, maxage,
                                    etag, length, data, release_func,
                                    app_ptr)) {
    response->code = COAP_RESPONSE_CODE(400);
    goto error;
  }

  return 1;

error:
  coap_add_data(response,
                strlen(coap_response_phrase(response->code)),
                (const unsigned char *)coap_response_phrase(response->code));
  return 0;
}

coap_tick_t
coap_block_check_lg_xmit_timeouts(coap_session_t *session, coap_tick_t now) {
  coap_lg_xmit_t *p;
  coap_lg_xmit_t *q;
  coap_tick_t partial_timeout = 8 * COAP_TICKS_PER_SECOND;
  coap_tick_t tim_rem = -1;

  LL_FOREACH_SAFE(session->lg_xmit, p, q) {
    if (p->last_used == 0) {
      continue;
    }
    if (p->last_used + partial_timeout <= now) {
      /* Expire this entry */
      LL_DELETE(session->lg_xmit, p);
      coap_block_delete_lg_xmit(session, p);
    }
    else {
      /* Delay until the lg_xmit needs to expire */
      if (tim_rem > p->last_used + partial_timeout - now)
        tim_rem = p->last_used + partial_timeout - now;
    }
  }
  return tim_rem;
}

coap_tick_t
coap_block_check_lg_crcv_timeouts(coap_session_t *session, coap_tick_t now) {
  coap_lg_crcv_t *p;
  coap_lg_crcv_t *q;
  coap_tick_t partial_timeout = COAP_EXCHANGE_LIFETIME(session);
  coap_tick_t tim_rem = -1;

  LL_FOREACH_SAFE(session->lg_crcv, p, q) {
    if (!p->observe_set && p->last_used &&
        p->last_used + partial_timeout <= now) {
      /* Expire this entry */
      LL_DELETE(session->lg_crcv, p);
      coap_block_delete_lg_crcv(session, p);
    }
    else if (!p->observe_set && p->last_used) {
      /* Delay until the lg_crcv needs to expire */
      if (tim_rem > p->last_used + partial_timeout - now)
        tim_rem = p->last_used + partial_timeout - now;
    }
  }
  return tim_rem;
}

static int
check_if_received_block(coap_rblock_t *rec_blocks, uint32_t block_num) {
  uint32_t i;

  for (i = 0; i < rec_blocks->used; i++) {
    if (block_num < rec_blocks->range[i].begin)
      return 0;
    if (block_num <= rec_blocks->range[i].end)
      return 1;
  }
  return 0;
}

static int
check_all_blocks_in(coap_rblock_t *rec_blocks, size_t total_blocks) {
  uint32_t i;
  uint32_t block = 0;

  for (i = 0; i < rec_blocks->used; i++) {
    if (block < rec_blocks->range[i].begin)
      return 0;
    if (block < rec_blocks->range[i].end)
      block = rec_blocks->range[i].end;
  }
  /* total_blocks counts from 1 */
  if (block + 1 < total_blocks)
    return 0;

  return 1;
}

coap_tick_t
coap_block_check_lg_srcv_timeouts(coap_session_t *session, coap_tick_t now) {
  coap_lg_srcv_t *p;
  coap_lg_srcv_t *q;
  coap_tick_t partial_timeout = COAP_EXCHANGE_LIFETIME(session);
  coap_tick_t tim_rem = -1;

  LL_FOREACH_SAFE(session->lg_srcv, p, q) {
    if (p->last_used && p->last_used + partial_timeout <= now) {
      /* Expire this entry */
      LL_DELETE(session->lg_srcv, p);
      coap_block_delete_lg_srcv(session, p);
    }
    else if (p->last_used) {
      /* Delay until the lg_srcv needs to expire */
      if (tim_rem > p->last_used + partial_timeout - now)
        tim_rem = p->last_used + partial_timeout - now;
    }
  }
  return tim_rem;
}

coap_lg_crcv_t *
coap_block_new_lg_crcv(coap_session_t *session, coap_pdu_t *pdu) {
  coap_lg_crcv_t *lg_crcv;

  lg_crcv = coap_malloc_type(COAP_LG_CRCV, sizeof(coap_lg_crcv_t));

  if (lg_crcv == NULL)
    return NULL;

  coap_log(LOG_DEBUG, "** %s: lg_crcv %p initialized\n",
           coap_session_str(session), (void*)lg_crcv);
  memset(lg_crcv, 0, sizeof(coap_lg_crcv_t));
  lg_crcv->initial = 1;
  /* Set up skeletal PDU to use as a basis for all the subsequent blocks */
  memcpy(&lg_crcv->pdu, pdu, sizeof(lg_crcv->pdu));
  lg_crcv->pdu.token = coap_malloc_type(COAP_PDU_BUF,
                            lg_crcv->pdu.alloc_size + lg_crcv->pdu.hdr_size);
  if (!lg_crcv->pdu.token) {
    coap_block_delete_lg_crcv(session, lg_crcv);
    return NULL;
  }
  lg_crcv->pdu.token += lg_crcv->pdu.hdr_size;
  memcpy(lg_crcv->pdu.token, pdu->token, lg_crcv->pdu.used_size);
  if (lg_crcv->pdu.data)
    lg_crcv->pdu.data = lg_crcv->pdu.token + (pdu->data - pdu->token);
  /* Check that there is space for increased token + option change */
  if (lg_crcv->pdu.max_size < lg_crcv->pdu.used_size + 9)
    lg_crcv->pdu.max_size = lg_crcv->pdu.used_size + 9;

  assert(pdu->token_length <= 8);
  lg_crcv->token_length = min(pdu->token_length, 8);
  memset(lg_crcv->token, 0, sizeof(lg_crcv->token));
  memcpy(lg_crcv->token, pdu->token, lg_crcv->token_length);

  /* Need to keep original token for handling observe responses */
  memset(lg_crcv->base_token, 0, sizeof(lg_crcv->base_token));
  memcpy(lg_crcv->base_token, pdu->token, lg_crcv->token_length);
  lg_crcv->base_token_length = lg_crcv->token_length;

  /* Need to keep original token for updating response PDUs */
  lg_crcv->app_token = coap_new_binary(lg_crcv->token_length);
  if (!lg_crcv->app_token) {
    coap_block_delete_lg_crcv(session, lg_crcv);
    return NULL;
  }
  memcpy(lg_crcv->app_token->s, pdu->token, lg_crcv->token_length);
  /* In case it is there - must not be in continuing request PDUs */
  coap_remove_option(&lg_crcv->pdu, COAP_OPTION_BLOCK1);

  return lg_crcv;
}

void
coap_block_delete_lg_crcv(coap_session_t *session,
                               coap_lg_crcv_t *lg_crcv) {
  if (lg_crcv == NULL)
    return;

  if (lg_crcv->pdu.token)
    coap_free_type(COAP_PDU_BUF, lg_crcv->pdu.token - lg_crcv->pdu.hdr_size);
  coap_free_type(COAP_STRING, lg_crcv->body_data);
  coap_log(LOG_DEBUG, "** %s: lg_crcv %p released\n",
           coap_session_str(session), (void*)lg_crcv);
  coap_delete_binary(lg_crcv->app_token);
  coap_free_type(COAP_LG_CRCV, lg_crcv);
}

void
coap_block_delete_lg_srcv(coap_session_t *session,
                               coap_lg_srcv_t *lg_srcv) {
  if (lg_srcv == NULL)
    return;

  coap_delete_str_const(lg_srcv->uri_path);
  coap_free_type(COAP_STRING, lg_srcv->body_data);
  coap_log(LOG_DEBUG, "** %s: lg_srcv %p released\n",
         coap_session_str(session), (void*)lg_srcv);
  coap_free_type(COAP_LG_SRCV, lg_srcv);
}

void
coap_block_delete_lg_xmit(coap_session_t *session,
                               coap_lg_xmit_t *lg_xmit) {
  if (lg_xmit == NULL)
    return;

  if (lg_xmit->release_func) {
    lg_xmit->release_func(session, lg_xmit->app_ptr);
  }
  if (lg_xmit->pdu.token) {
    coap_free_type(COAP_PDU_BUF, lg_xmit->pdu.token - lg_xmit->pdu.hdr_size);
  }
  if (COAP_PDU_IS_REQUEST(&lg_xmit->pdu))
    coap_delete_binary(lg_xmit->b.b1.app_token);
  else
    coap_delete_string(lg_xmit->b.b2.query);

  coap_log(LOG_DEBUG, "** %s: lg_xmit %p released\n",
           coap_session_str(session), (void*)lg_xmit);
  coap_free_type(COAP_LG_XMIT, lg_xmit);
}

static int
add_block_send(uint32_t num, uint32_t *out_blocks,
                          uint32_t *count, uint32_t max_count) {
  uint32_t i;

  for (i = 0; i < *count && *count < max_count; i++) {
    if (num == out_blocks[i])
      return 0;
    else if (num < out_blocks[i]) {
      if (*count - i > 1)
        memmove(&out_blocks[i], &out_blocks[i+1], *count - i -1);
      out_blocks[i] = num;
      (*count)++;
      return 1;
    }
  }
  if (*count < max_count) {
    out_blocks[i] = num;
    (*count)++;
    return 1;
  }
  return 0;
}

/*
 * Need to see if this is a request for the next block of a large body
 * transfer.  If so, need to initiate the response with the next blocks
 * and not trouble the application.
 *
 * If additional responses needed, then these are expicitly sent out and
 * 'response' is updated to be the last response to be sent.
 *
 * This is set up using coap_add_data_large_response()
 *
 * Server is sending a large data response to GET / observe (BLOCK2)
 *
 * Return: 0 Call application handler
 *         1 Do not call application handler - just send the built response
 */
int
coap_handle_request_send_block(coap_session_t *session,
                               coap_pdu_t *pdu,
                               coap_pdu_t *response,
                               coap_resource_t *resource,
                               coap_string_t *query) {
  coap_lg_xmit_t *p;
  coap_block_t block;
  uint16_t block_opt = 0;
  uint32_t out_blocks[1];
  const char *error_phrase;

  if (coap_get_block(pdu, COAP_OPTION_BLOCK2, &block)) {
    block_opt = COAP_OPTION_BLOCK2;
  }
  LL_FOREACH(session->lg_xmit, p) {
    size_t chunk;
    coap_opt_iterator_t opt_iter;
    coap_opt_iterator_t opt_b_iter;
    coap_opt_t *option;
    uint32_t request_cnt, i;
    coap_opt_t *etag_opt = NULL;
    coap_pdu_t *out_pdu = response;
    static coap_string_t empty = { 0, NULL};

    if (COAP_PDU_IS_REQUEST(&p->pdu) || resource != p->b.b2.resource ||
        !coap_string_equal(query ? query : &empty,
                           p->b.b2.query ? p->b.b2.query : &empty)) {
      /* try out the next one */
      continue;
    }
    etag_opt = coap_check_option(pdu, COAP_OPTION_ETAG, &opt_iter);
    if (etag_opt) {
      uint64_t etag = coap_decode_var_bytes8(coap_opt_value(etag_opt),
                                            coap_opt_length(etag_opt));
      if (etag != p->b.b2.etag) {
        /* try out the next one */
        continue;
      }
      out_pdu->code = COAP_RESPONSE_CODE(203);
      return 1;
    }
    else {
      out_pdu->code = p->pdu.code;
    }

    /* lg_xmit (response) found */

    chunk = (size_t)1 << (p->blk_size + 4);
    if (block_opt) {
      coap_log(LOG_DEBUG,
               "found Block option, block size is %zu, block nr. %u, M %d\n",
               (size_t)1 << (block.szx + 4), block.num, block.m);
      if (block.szx != p->blk_size) {
        if ((p->offset + chunk) % ((size_t)1 << (block.szx + 4)) == 0) {
          /*
           * Recompute the block number of the previous packet given
           * the new block size
           */
          block.num = (uint32_t)(((p->offset + chunk) >> (block.szx + 4)) - 1);
          p->blk_size = block.szx;
          chunk = (size_t)1 << (p->blk_size + 4);
          p->offset = block.num * chunk;
          coap_log(LOG_DEBUG,
                   "new Block size is %u, block number %u completed\n",
                   1 << (block.szx + 4), block.num);
        } else {
          coap_log(LOG_DEBUG,
                   "ignoring request to increase Block size, "
                   "next block is not aligned on requested block size "
                   "boundary. (%zu x %u mod %u = %zu (which is not 0)\n",
                   p->offset/chunk + 1, (1 << (p->blk_size + 4)),
                   (1 << (block.szx + 4)),
                   (p->offset + chunk) % ((size_t)1 << (block.szx + 4)));
        }
      }
    }

    request_cnt = 0;
    coap_option_iterator_init(pdu, &opt_b_iter, COAP_OPT_ALL);
    while ((option = coap_option_next(&opt_b_iter))) {
      unsigned int num;
      if (opt_b_iter.number != p->option)
        continue;
      num = coap_opt_block_num(option);
      if (num > 0xFFFFF) /* 20 bits max for num */
        continue;
      if (block.szx != COAP_OPT_BLOCK_SZX(option)) {
        coap_add_data(response,
                      sizeof("Changing blocksize during request invalid")-1,
                 (const uint8_t *)"Changing blocksize during request invalid");
        response->code = COAP_RESPONSE_CODE(400);
        return 1;
      }
      add_block_send(num, out_blocks, &request_cnt, 1);
      break;
    }
    if (request_cnt == 0) {
      /* Block2 not found - give them the first block */
      block.szx = p->blk_size;
      p->offset = 0;
      out_blocks[0] = 0;
      request_cnt = 1;
    }

    for (i = 0; i < request_cnt; i++) {
      uint8_t buf[8];

      block.num = out_blocks[i];
      p->offset = block.num * chunk;

      if (i + 1 < request_cnt) {
        /* Need to set up a copy of the pdu to send */
        coap_opt_filter_t drop_options;

        memset(&drop_options, 0, sizeof(coap_opt_filter_t));
        if (block.num != 0)
          coap_option_filter_set(&drop_options, COAP_OPTION_OBSERVE);
        out_pdu = coap_pdu_duplicate(&p->pdu, session, pdu->token_length,
                                     pdu->token, &drop_options);
        if (!out_pdu) {
          goto internal_issue;
        }
      }
      else {
        /*
         * Copy the options across and then fix the block option
         *
         * Need to drop Observe option if BLOCK2 and block.num != 0
         */
        coap_option_iterator_init(&p->pdu, &opt_iter, COAP_OPT_ALL);
        while ((option = coap_option_next(&opt_iter))) {
          if (opt_iter.number == COAP_OPTION_OBSERVE && block.num != 0)
            continue;
          if (!coap_add_option(response, opt_iter.number,
                               coap_opt_length(option),
                               coap_opt_value(option))) {
            goto internal_issue;
          }
        }
        out_pdu = response;
      }
      if (pdu->type == COAP_MESSAGE_NON)
        out_pdu->type = COAP_MESSAGE_NON;
      if (!coap_update_option(out_pdu, p->option,
          coap_encode_var_safe(buf,
                          sizeof(buf),
                          (block.num << 4) |
                           ((p->offset + chunk < p->length) << 3) |
                           block.szx),
                          buf)) {
        goto internal_issue;
      }
      if (!(p->offset + chunk < p->length)) {
        /* Last block - keep in cache for 4 * ACK_TIMOUT */
        coap_ticks(&p->last_used);
      }
      if (p->b.b2.maxage_expire) {
        coap_tick_t now;
        coap_time_t rem;

        coap_ticks(&now);
        rem = coap_ticks_to_rt(now);
        if (p->b.b2.maxage_expire > rem) {
          rem = p->b.b2.maxage_expire - rem;
        }
        else {
          rem = 0;
          /* Entry needs to be expired */
          coap_ticks(&p->last_used);
        }
        if (!coap_update_option(out_pdu, COAP_OPTION_MAXAGE,
                                coap_encode_var_safe8(buf,
                                                      sizeof(buf),
                                                      rem),
                                buf)) {
          goto internal_issue;
        }
      }

      if (!etag_opt && !coap_add_block(out_pdu,
                                       p->length,
                                       p->data,
                                       block.num,
                                       block.szx)) {
        goto internal_issue;
      }
      if (i + 1 < request_cnt) {
        coap_send_internal(session, out_pdu);
      }
    }
    goto skip_app_handler;

internal_issue:
    response->code = COAP_RESPONSE_CODE(500);
    error_phrase = coap_response_phrase(response->code);
    coap_add_data(response, strlen(error_phrase),
                  (const uint8_t *)error_phrase);
    /* Keep in cache for 4 * ACK_TIMOUT */
    coap_ticks(&p->last_used);
    goto skip_app_handler;
  } /* end of LL_FOREACH() */
  return 0;

skip_app_handler:
  return 1;
}

static int
update_received_blocks(coap_rblock_t *rec_blocks, uint32_t block_num) {
  uint32_t i;

  /* Reset as there is activity */
  rec_blocks->retry = 0;

  for (i = 0; i < rec_blocks->used; i++) {
    if (block_num >= rec_blocks->range[i].begin &&
        block_num <= rec_blocks->range[i].end)
      break;

    if (block_num < rec_blocks->range[i].begin) {
      if (block_num + 1 == rec_blocks->range[i].begin) {
        rec_blocks->range[i].begin = block_num;
      }
      else {
        /* Need to insert a new range */
        if (rec_blocks->used == COAP_RBLOCK_CNT -1)
          /* Too many losses */
          return 0;
        memmove(&rec_blocks->range[i+1], &rec_blocks->range[i],
                (rec_blocks->used - i) * sizeof (rec_blocks->range[0]));
        rec_blocks->range[i].begin = rec_blocks->range[i].end = block_num;
        rec_blocks->used++;
      }
      break;
    }
    if (block_num == rec_blocks->range[i].end + 1) {
      rec_blocks->range[i].end = block_num;
      if (i + 1 < rec_blocks->used) {
        if (rec_blocks->range[i+1].begin == block_num + 1) {
          /* Merge the 2 ranges */
          rec_blocks->range[i].end = rec_blocks->range[i+1].end;
          if (i+2 < rec_blocks->used) {
            memmove (&rec_blocks->range[i+1], &rec_blocks->range[i+2],
                   (rec_blocks->used - (i+2)) * sizeof (rec_blocks->range[0]));
          }
          rec_blocks->used--;
        }
      }
      break;
    }
  }
  if (i == rec_blocks->used) {
    if (rec_blocks->used == COAP_RBLOCK_CNT -1)
      /* Too many losses */
      return 0;
    rec_blocks->range[i].begin = rec_blocks->range[i].end = block_num;
    rec_blocks->used++;
  }
  coap_ticks(&rec_blocks->last_seen);
  return 1;
}

/*
 * Need to check if this is a large PUT / POST using multiple blocks
 *
 * Server receiving PUT/POST etc. of a large amount of data (BLOCK1)
 *
 * Return: 0 Call application handler
 *         1 Do not call application handler - just send the built response
 */
int
coap_handle_request_put_block(coap_context_t *context,
                              coap_session_t *session,
                              coap_pdu_t *pdu,
                              coap_pdu_t *response,
                              coap_resource_t *resource,
                              coap_string_t *uri_path,
                              coap_opt_t *observe,
                              coap_string_t *query,
                              coap_method_handler_t h,
                              int *added_block) {
  size_t length = 0;
  const uint8_t *data = NULL;
  size_t offset = 0;
  size_t total = 0;
  coap_block_t block;
  coap_opt_iterator_t opt_iter;
  uint16_t block_option = 0;

  coap_get_data_large(pdu, &length, &data, &offset, &total);
  pdu->body_offset = 0;
  pdu->body_total = length;

  if (coap_get_block(pdu, COAP_OPTION_BLOCK1, &block)) {
    block_option = COAP_OPTION_BLOCK1;
  }
  if (block_option) {
    coap_lg_srcv_t *p;
    coap_opt_t *size_opt = coap_check_option(pdu,
                                             COAP_OPTION_SIZE1,
                                             &opt_iter);
    coap_opt_t *fmt_opt = coap_check_option(pdu,
                                            COAP_OPTION_CONTENT_FORMAT,
                                            &opt_iter);
    uint16_t fmt = fmt_opt ? coap_decode_var_bytes(coap_opt_value(fmt_opt),
                                        coap_opt_length(fmt_opt)) :
                             COAP_MEDIATYPE_TEXT_PLAIN;

    total = size_opt ? coap_decode_var_bytes(coap_opt_value(size_opt),
                                        coap_opt_length(size_opt)) : 0;
    offset = block.num << (block.szx + 4);

    LL_FOREACH(session->lg_srcv, p) {
      if (resource == p->resource) {
        break;
      }
      if ((p->resource == context->unknown_resource ||
           resource == context->proxy_uri_resource) &&
          coap_string_equal(uri_path, p->uri_path))
        break;
    }
    if (!p && block.num != 0) {
      /* random access - no need to track */
      pdu->body_data = data;
      pdu->body_length = length;
      pdu->body_offset = offset;
      pdu->body_total = length + offset + (block.m ? 1 : 0);
    }
    /* Do not do this if this is a single block */
    else if (!p && !(offset == 0 && block.m == 0)) {
      p = coap_malloc_type(COAP_LG_SRCV, sizeof(coap_lg_srcv_t));
      if (p == NULL) {
        coap_add_data(response, sizeof("Memory issue")-1,
                      (const uint8_t *)"Memory issue");
        response->code = COAP_RESPONSE_CODE(500);
        goto skip_app_handler;
      }
      coap_log(LOG_DEBUG, "** %s: lg_srcv %p initialized\n",
               coap_session_str(session), (void*)p);
      memset(p, 0, sizeof(coap_lg_srcv_t));
      p->resource = resource;
      if (resource == context->unknown_resource ||
          resource == context->proxy_uri_resource)
        p->uri_path = coap_new_str_const(uri_path->s, uri_path->length);
      p->content_format = fmt;
      p->total_len = total;
      p->amount_so_far = length;
      p->szx = block.szx;
      p->block_option = block_option;
      if (observe) {
        p->observe_length = min(coap_opt_length(observe), 3);
        memcpy(p->observe, coap_opt_value(observe), p->observe_length);
        p->observe_set = 1;
      }
      p->body_data = NULL;
      LL_PREPEND(session->lg_srcv, p);
    }
    if (p) {
      if (fmt != p->content_format) {
        coap_add_data(response, sizeof("Content-Format mismatch")-1,
                      (const uint8_t *)"Content-Format mismatch");
        response->code = COAP_RESPONSE_CODE(408);
        goto free_lg_recv;
      }
      p->last_mid = pdu->mid;
      p->last_type = pdu->type;
      memcpy(p->last_token, pdu->token, pdu->token_length);
      p->last_token_length = pdu->token_length;
      if (session->block_mode & (COAP_BLOCK_SINGLE_BODY)) {
        size_t chunk = (size_t)1 << (block.szx + 4);
        if (!check_if_received_block(&p->rec_blocks, block.num)) {
          /* Update list of blocks received */
          if (!update_received_blocks(&p->rec_blocks, block.num)) {
            coap_handle_event(context, COAP_EVENT_PARTIAL_BLOCK, session);
            coap_add_data(response, sizeof("Too many missing blocks")-1,
                          (const uint8_t *)"Too many missing blocks");
            response->code = COAP_RESPONSE_CODE(408);
            goto free_lg_recv;
          }
          /* Update saved data */
          p->body_data = coap_block_build_body(p->body_data, length, data,
                                               offset, p->total_len);
          if (!p->body_data)
            goto call_app_handler;

        }
        if (!check_all_blocks_in(&p->rec_blocks,
                                (uint32_t)(p->total_len + chunk -1)/chunk)) {
          /* Not all the payloads of the body have arrived */
          if (block.m) {
            uint8_t buf[4];

            /* Ask for the next block */
            coap_insert_option(response, block_option,
                             coap_encode_var_safe(buf, sizeof(buf),
                               (block.num << 4) |
                               (block.m << 3) |
                               block.szx),
                             buf);
            response->code = COAP_RESPONSE_CODE(231);
            goto skip_app_handler;
          }
          goto skip_app_handler;
        }

        /*
         * Remove the BLOCK1 option as passing all of the data to
         * application layer. Add back in observe option if appropriate.
         * Adjust all other information.
         */
        if (p->observe_set) {
          coap_update_option(pdu, COAP_OPTION_OBSERVE,
                             p->observe_length, p->observe);
        }
        coap_remove_option(pdu, block_option);
        pdu->body_data = p->body_data->s;
        pdu->body_length = p->total_len;
        pdu->body_offset = 0;
        pdu->body_total = p->total_len;
        coap_log(LOG_DEBUG, "Server app version of updated PDU\n");
        coap_show_pdu(LOG_DEBUG, pdu);
        /* Need to do this here as we need to free off p */
        h(resource, session, pdu, query, response);
        /* Check if lg_xmit generated and update PDU code if so */
        coap_check_code_lg_xmit(session, response, resource, query);
        /* Last chunk - free off shortly */
        coap_ticks(&p->last_used);
        goto skip_app_handler;
      }
      else {
        /* No need to update body_data and body_length as a single PDU */
        pdu->body_offset = offset;
        /* Exact match if last block */
        if (block.m) {
          uint8_t buf[4];

          if (total > offset + length + block.m)
            pdu->body_total = total;
          else
            pdu->body_total = offset + length + block.m;

          coap_insert_option(response, block_option,
                           coap_encode_var_safe(buf, sizeof(buf),
                             (block.num << 4) |
                             (block.m << 3) |
                             block.szx),
                           buf);
          h(resource, session, pdu, query, response);
          /* Check if lg_xmit generated and update PDU code if so */
          coap_check_code_lg_xmit(session, response, resource, query);
          if (COAP_RESPONSE_CLASS(response->code) == 2) {
            /* Just in case, as there are more to go */
            response->code = COAP_RESPONSE_CODE(231);
          }
          *added_block = 1;
          goto skip_app_handler;
        }
        else {
          pdu->body_total = offset + length + block.m;
        }
      }

      if (block.m == 0) {
        /* Last chunk - free off all */
        coap_ticks(&p->last_used);
      }
      goto call_app_handler;

free_lg_recv:
      LL_DELETE(session->lg_srcv, p);
      coap_block_delete_lg_srcv(session, p);
      goto skip_app_handler;
    }
  }
call_app_handler:
  return 0;

skip_app_handler:
  return 1;
}

/*
 * Need to see if this is a response to a large body request transfer. If so,
 * need to initiate the request containing the next block and not trouble the
 * application.  Note that Token must unique per request/response.
 *
 * Client receives large data acknowledgement from server (BLOCK1)
 *
 * This is set up using coap_add_data_large_request()
 *
 * Client is sending a large data request using GET etc.
 *
 * Return: 0 Call application handler
 *         1 Do not call application handler - just send the built response
 */
int
coap_handle_response_send_block(coap_session_t *session, coap_pdu_t *rcvd) {
  coap_lg_xmit_t *p;
  coap_lg_xmit_t *q;

  LL_FOREACH_SAFE(session->lg_xmit, p, q) {
    if (COAP_PDU_IS_REQUEST(&p->pdu) &&
        !block_token_match(rcvd->token, rcvd->token_length,
                    p->b.b1.token, p->b.b1.token_length)) {
    }
    /* lg_xmit found */
    size_t chunk = (size_t)1 << (p->blk_size + 4);
    coap_block_t block;

    if (COAP_RESPONSE_CLASS(rcvd->code) == 2 &&
        coap_get_block(rcvd, p->option, &block)) {
      coap_log(LOG_DEBUG,
               "found Block option, block size is %u, block nr. %u\n",
               1 << (block.szx + 4), block.num);
      if (block.szx != p->blk_size) {
        if ((p->offset + chunk) % ((size_t)1 << (block.szx + 4)) == 0) {
          /*
           * Recompute the block number of the previous packet given the
           * new block size
           */
          block.num = (uint32_t)(((p->offset + chunk) >> (block.szx + 4)) - 1);
          p->blk_size = block.szx;
          chunk = (size_t)1 << (p->blk_size + 4);
          p->offset = block.num * chunk;
          coap_log(LOG_DEBUG,
                   "new Block size is %u, block number %u completed\n",
                   1 << (block.szx + 4), block.num);
        } else {
          coap_log(LOG_DEBUG, "ignoring request to increase Block size, "
             "next block is not aligned on requested block size boundary. "
             "(%zu x %u mod %u = %zu != 0)\n",
             p->offset/chunk + 1, (1 << (p->blk_size + 4)),
             (1 << (block.szx + 4)),
             (p->offset + chunk) % ((size_t)1 << (block.szx + 4)));
        }
      }
      if (p->last_block == (int)block.num) {
        /*
         * Duplicate BLOCK ACK
         *
         * RFCs not clear here, but on a lossy connection, there could
         * be multiple BLOCK ACKs, causing the client to retransmit the
         * same block multiple times, or the server retransmitting the
         * same ACK.
         *
         * Once a block has been ACKd, there is no need to retransmit it.
         */
        return 1;
      }
      p->last_block = block.num;
      p->offset = (block.num + 1) * chunk;
      if (p->offset < p->length) {
        /* Build the next PDU request based off the skeletal PDU */
        uint8_t buf[8];
        coap_pdu_t *pdu;
        uint64_t token = coap_decode_var_bytes8(p->pdu.token,
                                                p->pdu.token_length);
        uint8_t ltoken[8];
        size_t ltoken_length;

        token = (token & 0xffffffff) + ((uint64_t)(++p->b.b1.count) << 32);
        ltoken_length = coap_encode_var_safe8(ltoken, sizeof(token), token);
        pdu = coap_pdu_duplicate(&p->pdu, session, ltoken_length, ltoken, NULL);
        if (!pdu)
          goto fail_body;

        block.num++;
        coap_update_option(pdu, p->option,
                           coap_encode_var_safe(buf, sizeof(buf),
                             (block.num << 4) |
                             ((p->offset + chunk < p->length) << 3) |
                             block.szx),
                           buf);

        if (!coap_add_block(pdu,
                            p->length,
                            p->data,
                            block.num,
                            block.szx))
          goto fail_body;
        if (coap_send_internal(session, pdu) == COAP_INVALID_MID)
          goto fail_body;
        return 1;
      }
    }
fail_body:
    /* need to put back original token into rcvd */
    if (p->b.b1.app_token)
      coap_update_token(rcvd, p->b.b1.app_token->length,
                        p->b.b1.app_token->s);
    coap_log(LOG_DEBUG, "PDU given to app\n");
    coap_show_pdu(LOG_DEBUG, rcvd);

    LL_DELETE(session->lg_xmit, p);
    coap_block_delete_lg_xmit(session, p);
    /*
     * There may be a block response after doing the large request
     * https://tools.ietf.org/html/rfc7959#section-3.3
     */
    break;
  } /* end of LL_FOREACH_SAFE */
  return 0;
}

/*
 * Re-assemble payloads into a body
 */
coap_binary_t *
coap_block_build_body(coap_binary_t *body_data, size_t length,
                      const uint8_t *data, size_t offset, size_t total)
{
  if (data == NULL)
    return NULL;
  if (body_data == NULL && total) {
    body_data = coap_new_binary(total);
  }
  if (body_data == NULL)
    return NULL;

  /* Update saved data */
  if (offset + length <= total && body_data->length >= total) {
    memcpy(&body_data->s[offset], data, length);
  }
  else {
    /*
     * total may be inaccurate as per
     * https://tools.ietf.org/html/rfc7959#section-4
     * o In a request carrying a Block1 Option, to indicate the current
     *   estimate the client has of the total size of the resource
     *   representation, measured in bytes ("size indication").
     * o In a response carrying a Block2 Option, to indicate the current
     *   estimate the server has of the total size of the resource
     *   representation, measured in bytes ("size indication").
     */
    coap_binary_t *new = coap_resize_binary(body_data, offset + length);

    if (new) {
      body_data = new;
      memcpy(&body_data->s[offset], data, length);
    }
    else {
      coap_delete_binary(body_data);
      return NULL;
    }
  }
  return body_data;
}

/*
 * Need to see if this is a large body response to a request. If so,
 * need to initiate the request for the next block and not trouble the
 * application.  Note that Token must unique per request/response.
 *
 * This is set up using coap_send()
 * Client receives large data from server (BLOCK2)
 *
 * Return: 0 Call application handler
 *         1 Do not call application handler - just sent the next request
 */
int
coap_handle_response_get_block(coap_context_t *context,
                        coap_session_t *session,
                        coap_pdu_t *sent,
                        coap_pdu_t *rcvd,
                        coap_recurse_t recursive) {
  coap_lg_crcv_t *p;
  int app_has_response = 0;
  coap_block_t block = {0, 0, 0};
  int have_block = 0;
  uint16_t block_opt = 0;
  size_t offset;

  LL_FOREACH(session->lg_crcv, p) {
    size_t chunk = 0;
    uint8_t buf[8];
    coap_opt_iterator_t opt_iter;

    if (!full_match(rcvd->token, rcvd->token_length,
                     p->token, p->token_length)) {
      /* try out the next one */
      continue;
    }

    /* lg_crcv found */

    if (COAP_RESPONSE_CLASS(rcvd->code) == 2) {
      size_t length;
      const uint8_t *data;
      coap_opt_t *size_opt = coap_check_option(rcvd, COAP_OPTION_SIZE2,
                                               &opt_iter);
      size_t size2 = size_opt ?
                  coap_decode_var_bytes(coap_opt_value(size_opt),
                                        coap_opt_length(size_opt)) : 0;

      coap_get_data(rcvd, &length, &data);
      rcvd->body_offset = 0;
      rcvd->body_total = length;
      if (coap_get_block(rcvd, COAP_OPTION_BLOCK2, &block)) {
        have_block = 1;
        block_opt = COAP_OPTION_BLOCK2;
      }
      if (have_block) {
        coap_opt_t *fmt_opt = coap_check_option(rcvd,
                                            COAP_OPTION_CONTENT_FORMAT,
                                            &opt_iter);
        uint16_t fmt = fmt_opt ?
                         coap_decode_var_bytes(coap_opt_value(fmt_opt),
                                        coap_opt_length(fmt_opt)) :
                         COAP_MEDIATYPE_TEXT_PLAIN;
        coap_opt_t *etag_opt = coap_check_option(rcvd,
                                                 COAP_OPTION_ETAG,
                                                 &opt_iter);
        /* Possibility that Size2 not sent, or is too small */
        chunk = (size_t)1 << (block.szx + 4);
        offset = block.num * chunk;
        if (size2 < (offset + length)) {
          if (block.m)
            size2 = offset + length + 1;
          else
            size2 = offset + length;
        }

        if (p->initial) {
          p->initial = 0;
          if (etag_opt) {
            p->etag_length = coap_opt_length(etag_opt);
            memcpy(p->etag, coap_opt_value(etag_opt), p->etag_length);
            p->etag_set = 1;
          }
          else {
            p->etag_set = 0;
          }
          p->total_len = size2;
          p->content_format = fmt;
          p->szx = block.szx;
          p->block_option = block_opt;
          p->last_type = rcvd->type;
          p->rec_blocks.used = 0;
        }
        if (p->total_len < size2)
          p->total_len = size2;

        if (etag_opt) {
          if (!full_match(coap_opt_value(etag_opt),
                                coap_opt_length(etag_opt),
                                p->etag, p->etag_length)) {
            /* body of data has changed - need to restart request */
            size_t len;
            coap_pdu_t *pdu;

            coap_log(LOG_WARNING,
                 "Data body updated during receipt - new request started\n");
            if (!(session->block_mode & COAP_BLOCK_SINGLE_BODY))
              coap_handle_event(context, COAP_EVENT_PARTIAL_BLOCK, session);

            p->initial = 1;
            coap_free_type(COAP_STRING, p->body_data);
            p->body_data = NULL;

            coap_session_new_token(session, &len, buf);
            pdu = coap_pdu_duplicate(&p->pdu, session, len, buf, NULL);
            if (!pdu)
              goto fail_resp;

            memcpy(p->token, pdu->token, pdu->token_length);
            p->token_length = pdu->token_length;

            coap_update_option(pdu, block_opt,
                               coap_encode_var_safe(buf, sizeof(buf),
                                      (0 << 4) | (0 << 3) | block.szx),
                               buf);

            if (coap_send_internal(session, pdu) == COAP_INVALID_MID)
              goto fail_resp;

            goto skip_app_handler;
          }
        }
        else if (p->etag_set) {
          /* Cannot handle this change in ETag to not being there */
          coap_log(LOG_WARNING, "Not all blocks have ETag option\n");
          session->block_mode &= ~(COAP_BLOCK_SINGLE_BODY);
          goto block_mode;
        }

        if (fmt != p->content_format) {
          coap_log(LOG_WARNING, "Content-Format option mismatch\n");
          session->block_mode &= ~(COAP_BLOCK_SINGLE_BODY);
          goto block_mode;
        }
        coap_log(LOG_DEBUG,
                 "found Block option, block size is %u, block nr. %u\n",
                 1 << (block.szx + 4), block.num);
        if (block.num == 0) {
          coap_opt_t *obs_opt = coap_check_option(rcvd,
                                                  COAP_OPTION_OBSERVE,
                                                  &opt_iter);
          if (obs_opt) {
            p->observe_length = min(coap_opt_length(obs_opt), 3);
            memcpy(p->observe, coap_opt_value(obs_opt), p->observe_length);
            p->observe_set = 1;
          }
          else {
            p->observe_set = 0;
          }
        }
        if (!check_if_received_block(&p->rec_blocks, block.num)) {
          /* Update list of blocks received */
          if (!update_received_blocks(&p->rec_blocks, block.num)) {
            coap_handle_event(context, COAP_EVENT_PARTIAL_BLOCK, session);
            goto fail_resp;
          }

          if (session->block_mode & (COAP_BLOCK_SINGLE_BODY)) {
            p->body_data = coap_block_build_body(p->body_data, length, data,
                                                 offset, size2);
            if (p->body_data == NULL) {
              /* Need to do it block by block */
              session->block_mode &= ~(COAP_BLOCK_SINGLE_BODY);
              goto block_mode;
            }
          }
          if (!check_all_blocks_in(&p->rec_blocks,
                                   (size2 + chunk -1) / chunk)) {
            /* Not all the payloads of the body have arrived */
            size_t len;
            coap_pdu_t *pdu;

            if (block.m) {
              block.m = 0;

              /* Ask for the next block */
              coap_session_new_token(session, &len, buf);
              pdu = coap_pdu_duplicate(&p->pdu, session, len, buf, NULL);
              if (!pdu)
                goto fail_resp;

              memcpy(p->token, pdu->token, pdu->token_length);
              p->token_length = pdu->token_length;

              /* Only sent with the first block */
              coap_remove_option(pdu, COAP_OPTION_OBSERVE);

              coap_update_option(pdu, block_opt,
                                 coap_encode_var_safe(buf, sizeof(buf),
                                   ((block.num + 1) << 4) |
                                    (block.m << 3) | block.szx),
                                 buf);

              if (coap_send_internal(session, pdu) == COAP_INVALID_MID)
                goto fail_resp;
            }
            if (session->block_mode & (COAP_BLOCK_SINGLE_BODY))
              goto skip_app_handler;

            /* need to put back original token into rcvd */
            coap_update_token(rcvd, p->app_token->length, p->app_token->s);
            rcvd->body_offset = block.num*chunk;
            rcvd->body_total = size2;
            goto call_app_handler;
          }
          /* need to put back original token into rcvd */
          coap_update_token(rcvd, p->app_token->length, p->app_token->s);
          if (session->block_mode & (COAP_BLOCK_SINGLE_BODY)) {
            /* Pretend that there is no block */
            coap_remove_option(rcvd, block_opt);
            if (p->observe_set) {
              coap_update_option(rcvd, COAP_OPTION_OBSERVE,
                                 p->observe_length, p->observe);
            }
            rcvd->body_data = p->body_data->s;
            rcvd->body_length = block.num*chunk + length;
            rcvd->body_offset = 0;
            rcvd->body_total = rcvd->body_length;
          }
          else {
            rcvd->body_offset = block.num*chunk;
            rcvd->body_total = size2;
          }
          if (context->response_handler) {
            if (session->block_mode &
                  (COAP_BLOCK_SINGLE_BODY)) {
              coap_log(LOG_DEBUG, "Client app version of updated PDU\n");
              coap_show_pdu(LOG_DEBUG, rcvd);
            }
            context->response_handler(session, sent, rcvd, rcvd->mid);
          }
          app_has_response = 1;
          /* Set up for the next data body if observing */
          p->initial = 1;
          memcpy(p->token, p->base_token, p->base_token_length);
          p->token_length = p->base_token_length;
          if (p->body_data) {
            coap_free_type(COAP_STRING, p->body_data);
            p->body_data = NULL;
          }
          else {
            goto skip_app_handler;
          }
        }
        else {
block_mode:
          /* need to put back original token into rcvd */
          coap_update_token(rcvd, p->app_token->length, p->app_token->s);
          rcvd->body_offset = block.num*chunk;
          /* slightly oversize if there is more data */
          if (block.m) {
            if(size2 > block.num*chunk + length + block.m)
              rcvd->body_total = size2;
            else
              rcvd->body_total = block.num*chunk + length + block.m;
          }
          else {
            rcvd->body_total = block.num*chunk + length;
            /* Set up for the next data body if observing */
            p->initial = 1;
            memcpy(p->token, p->base_token, p->base_token_length);
            p->token_length = p->base_token_length;
          }
          if (context->response_handler) {
            coap_log(LOG_DEBUG, "Client app version of updated PDU\n");
            coap_show_pdu(LOG_DEBUG, rcvd);
            context->response_handler(session, sent, rcvd, rcvd->mid);
          }
          app_has_response = 1;
        }
      }
      else {
        coap_opt_t *obs_opt = coap_check_option(rcvd,
                                                COAP_OPTION_OBSERVE,
                                                &opt_iter);
        if (obs_opt) {
          p->observe_length = min(coap_opt_length(obs_opt), 3);
          memcpy(p->observe, coap_opt_value(obs_opt), p->observe_length);
          p->observe_set = 1;
        }
        else {
          p->observe_set = 0;
        }
      }
    }
    if (!block.m && !p->observe_set) {
fail_resp:
      /* lg_crcv no longer required - cache it */
      coap_ticks(&p->last_used);
    }
    /* need to put back original token into rcvd */
    coap_update_token(rcvd, p->app_token->length, p->app_token->s);
    break;
  } /* LL_FOREACH() */

  /* Check if receiving a block response and if blocks can be set up */
  if (recursive == COAP_RECURSE_OK && !p) {
    if (!sent) {
      if (coap_get_block(rcvd, COAP_OPTION_BLOCK2, &block)) {
        coap_log(LOG_DEBUG, "** %s: large body receive internal issue\n",
                 coap_session_str(session));
      }
    }
    else if (COAP_RESPONSE_CLASS(rcvd->code) == 2) {
      if (coap_get_block(rcvd, COAP_OPTION_BLOCK2, &block)) {
        have_block = 1;
        block_opt = COAP_OPTION_BLOCK2;
        if (block.num != 0) {
          /* Assume random access and just give the single response to app */
          size_t length;
          const uint8_t *data;
          size_t chunk = (size_t)1 << (block.szx + 4);

          coap_get_data(rcvd, &length, &data);
          rcvd->body_offset = block.num*chunk;
          rcvd->body_total = block.num*chunk + length + (block.m ? 1 : 0);
          return 0;
        }
      }
      if (have_block) {
        coap_lg_crcv_t *lg_crcv = coap_block_new_lg_crcv(session, sent);

        if (lg_crcv) {
          LL_PREPEND(session->lg_crcv, lg_crcv);
          return coap_handle_response_get_block(context, session, sent, rcvd,
                                                COAP_RECURSE_NO);
        }
      }
    }
  }
  return app_has_response;

call_app_handler:
  return 0;

skip_app_handler:
  return 1;
}

/* Check if lg_xmit generated and update PDU code if so */
void
coap_check_code_lg_xmit(coap_session_t *session, coap_pdu_t *response,
                        coap_resource_t *resource, coap_string_t *query) {
  coap_lg_xmit_t *lg_xmit;
  coap_string_t empty = { 0, NULL};

  if (response->code == 0)
    return;
  LL_FOREACH(session->lg_xmit, lg_xmit) {
    if (!COAP_PDU_IS_REQUEST(&lg_xmit->pdu) &&
        lg_xmit->b.b2.resource == resource &&
        coap_string_equal(query ? query : &empty,
                   lg_xmit->b.b2.query ? lg_xmit->b.b2.query : &empty)) {
      /* lg_xmit found */
      if (lg_xmit->pdu.code == 0) {
        lg_xmit->pdu.code = response->code;
        return;
      }
    }
  }
}
