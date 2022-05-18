/* coap_cache.c -- Caching of CoAP requests
*
* Copyright (C) 2020 Olaf Bergmann <bergmann@tzi.org>
*
 * SPDX-License-Identifier: BSD-2-Clause
 *
* This file is part of the CoAP library libcoap. Please see
* README for terms of use.
*/

#include "coap3/coap_internal.h"

/* Determines if the given option_type denotes an option type that can
 * be used as CacheKey. Options that can be cache keys are not Unsafe
 * and not marked explicitly as NoCacheKey. */
static int
is_cache_key(uint16_t option_type, size_t cache_ignore_count,
             const uint16_t *cache_ignore_options) {
  size_t i;

  /* https://tools.ietf.org/html/rfc7252#section-5.4.6 Nocachekey definition */
  if ((option_type & 0x1e) == 0x1c)
    return 0;
  /*
   * https://tools.ietf.org/html/rfc7641#section-2 Observe is not a
   * part of the cache-key.
   */
  if (option_type == COAP_OPTION_OBSERVE)
    return 0;

  /* Check for option user has defined as not part of cache-key */
  for (i = 0; i < cache_ignore_count; i++) {
    if (cache_ignore_options[i] == option_type) {
      return 0;
    }
  }

  return 1;
}

int
coap_cache_ignore_options(coap_context_t *ctx,
                          const uint16_t *options,
                          size_t count) {
  if (ctx->cache_ignore_options) {
    coap_free(ctx->cache_ignore_options);
  }
  if (count) {
    assert(options);
    ctx->cache_ignore_options = coap_malloc(count * sizeof(options[0]));
    if (ctx->cache_ignore_options) {
      memcpy(ctx->cache_ignore_options, options, count * sizeof(options[0]));
      ctx->cache_ignore_count = count;
    }
    else {
      coap_log(LOG_WARNING, "Unable to create cache_ignore_options\n");
      return 0;
    }
  }
  else {
    ctx->cache_ignore_options = NULL;
    ctx->cache_ignore_count = count;
  }
  return 1;
}

coap_cache_key_t *
coap_cache_derive_key_w_ignore(const coap_session_t *session,
                               const coap_pdu_t *pdu,
                               coap_cache_session_based_t session_based,
                               const uint16_t *cache_ignore_options,
                               size_t cache_ignore_count) {
  coap_opt_t *option;
  coap_opt_iterator_t opt_iter;
  coap_digest_ctx_t *dctx;
  coap_digest_t digest;
  coap_cache_key_t *cache_key;

  if (!coap_option_iterator_init(pdu, &opt_iter, COAP_OPT_ALL)) {
    return NULL;
  }

  dctx = coap_digest_setup();
  if (!dctx)
    return NULL;

  if (session_based == COAP_CACHE_IS_SESSION_BASED) {
    /* Include the session ptr */
    if (!coap_digest_update(dctx, (const uint8_t*)session, sizeof(session))) {
      coap_digest_free(dctx);
      return NULL;
    }
  }
  while ((option = coap_option_next(&opt_iter))) {
    if (is_cache_key(opt_iter.number, cache_ignore_count,
                     cache_ignore_options)) {
      if (!coap_digest_update(dctx, option, coap_opt_size(option))) {
        coap_digest_free(dctx);
        return NULL;
      }
    }
  }

  /* The body of a FETCH payload is part of the cache key,
   * see https://tools.ietf.org/html/rfc8132#section-2 */
  if (pdu->code == COAP_REQUEST_CODE_FETCH) {
    size_t len;
    const uint8_t *data;
    if (coap_get_data(pdu, &len, &data)) {
      if (!coap_digest_update(dctx, data, len)) {
        coap_digest_free(dctx);
        return NULL;
      }
    }
  }

  if (!coap_digest_final(dctx, &digest)) {
    /* coap_digest_final() is guaranteed to free off dctx no matter what */
    return NULL;
  }
  cache_key = coap_malloc_type(COAP_CACHE_KEY, sizeof(coap_cache_key_t));
  if (cache_key) {
    memcpy(cache_key->key, digest.key, sizeof(cache_key->key));
  }
  return cache_key;
}

coap_cache_key_t *
coap_cache_derive_key(const coap_session_t *session,
                      const coap_pdu_t *pdu,
                      coap_cache_session_based_t session_based) {
  return coap_cache_derive_key_w_ignore(session, pdu, session_based,
                                        session->context->cache_ignore_options,
                                        session->context->cache_ignore_count);
}

void
coap_delete_cache_key(coap_cache_key_t *cache_key) {
  coap_free_type(COAP_CACHE_KEY, cache_key);
}

coap_cache_entry_t *
coap_new_cache_entry(coap_session_t *session, const coap_pdu_t *pdu,
               coap_cache_record_pdu_t record_pdu,
               coap_cache_session_based_t session_based,
               unsigned int idle_timeout) {
  coap_cache_entry_t *entry = coap_malloc_type(COAP_CACHE_ENTRY,
                                               sizeof(coap_cache_entry_t));
  if (!entry) {
    return NULL;
  }

  memset(entry, 0, sizeof(coap_cache_entry_t));
  entry->session = session;
  if (record_pdu == COAP_CACHE_RECORD_PDU) {
    entry->pdu = coap_pdu_init(pdu->type, pdu->code, pdu->mid, pdu->alloc_size);
    if (entry->pdu) {
      if (!coap_pdu_resize(entry->pdu, pdu->alloc_size)) {
        coap_delete_pdu(entry->pdu);
        coap_free_type(COAP_CACHE_ENTRY, entry);
        return NULL;
      }
      /* Need to get the appropriate data across */
      memcpy(entry->pdu, pdu, offsetof(coap_pdu_t, token));
      memcpy(entry->pdu->token, pdu->token, pdu->used_size);
      /* And adjust all the pointers etc. */
      entry->pdu->data = entry->pdu->token + (pdu->data - pdu->token);
    }
  }
  entry->cache_key = coap_cache_derive_key(session, pdu, session_based);
  if (!entry->cache_key) {
    coap_free_type(COAP_CACHE_ENTRY, entry);
    return NULL;
  }
  entry->idle_timeout = idle_timeout;
  if (idle_timeout > 0) {
    coap_ticks(&entry->expire_ticks);
    entry->expire_ticks += idle_timeout * COAP_TICKS_PER_SECOND;
  }

  HASH_ADD(hh, session->context->cache, cache_key[0], sizeof(coap_cache_key_t), entry);
  return entry;
}

coap_cache_entry_t *
coap_cache_get_by_key(coap_context_t *ctx, const coap_cache_key_t *cache_key) {
  coap_cache_entry_t *cache_entry = NULL;

  assert(cache_key);
  if (cache_key) {
    HASH_FIND(hh, ctx->cache, cache_key, sizeof(coap_cache_key_t), cache_entry);
  }
  if (cache_entry && cache_entry->idle_timeout > 0) {
    coap_ticks(&cache_entry->expire_ticks);
    cache_entry->expire_ticks += cache_entry->idle_timeout * COAP_TICKS_PER_SECOND;
  }
  return cache_entry;
}

coap_cache_entry_t *
coap_cache_get_by_pdu(coap_session_t *session,
                      const coap_pdu_t *request,
                      coap_cache_session_based_t session_based) {
  coap_cache_key_t *cache_key = coap_cache_derive_key(session, request, session_based);
  coap_cache_entry_t *cache_entry;

  if (!cache_key)
    return NULL;

  cache_entry = coap_cache_get_by_key(session->context, cache_key);
  coap_delete_cache_key(cache_key);
  if (cache_entry && cache_entry->idle_timeout > 0) {
    coap_ticks(&cache_entry->expire_ticks);
    cache_entry->expire_ticks += cache_entry->idle_timeout * COAP_TICKS_PER_SECOND;
  }
  return cache_entry;
}

void
coap_delete_cache_entry(coap_context_t *ctx, coap_cache_entry_t *cache_entry) {

  assert(cache_entry);

  if (cache_entry) {
    HASH_DELETE(hh, ctx->cache, cache_entry);
  }
  if (cache_entry->pdu) {
    coap_delete_pdu(cache_entry->pdu);
  }
  coap_delete_cache_key(cache_entry->cache_key);
  if (cache_entry->callback && cache_entry->app_data) {
    cache_entry->callback(cache_entry->app_data);
  }
  coap_free_type(COAP_CACHE_ENTRY, cache_entry);
}

const coap_pdu_t *
coap_cache_get_pdu(const coap_cache_entry_t *cache_entry) {
        return cache_entry->pdu;
}

void
coap_cache_set_app_data(coap_cache_entry_t *cache_entry,
                        void *data,
                        coap_cache_app_data_free_callback_t callback) {
  cache_entry->app_data = data;
  cache_entry->callback = callback;
}

void *
coap_cache_get_app_data(const coap_cache_entry_t *cache_entry) {
  return cache_entry->app_data;
}

void
coap_expire_cache_entries(coap_context_t *ctx) {
  coap_tick_t now;
  coap_cache_entry_t *cp, *ctmp;

  coap_ticks(&now);
  HASH_ITER(hh, ctx->cache, cp, ctmp) {
    if (cp->idle_timeout > 0) {
      if (cp->expire_ticks <= now) {
        coap_delete_cache_entry(ctx, cp);
      }
    }
  }
}

