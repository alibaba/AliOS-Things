/* resource.c -- generic resource handling
 *
 * Copyright (C) 2010--2015 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include <stdio.h>
#include "coap_config.h"
#include "coap.h"
#include "coap_debug.h"
#include "coap_mem.h"
#include "coap_net.h"
#include "resource.h"
#include "subscribe.h"
#include "utlist.h"

#if defined(WITH_LWIP_LIBCOAP)
/* mem.h is only needed for the string free calls for
 * COAP_ATTR_FLAGS_RELEASE_NAME / COAP_ATTR_FLAGS_RELEASE_VALUE /
 * COAP_RESOURCE_FLAGS_RELEASE_URI. not sure what those lines should actually
 * do on lwip. */

#include <lwip/memp.h>

#define COAP_MALLOC_TYPE(Type) \
  ((coap_##Type##_t *)memp_malloc(MEMP_COAP_##Type))
#define COAP_FREE_TYPE(Type, Object) memp_free(MEMP_COAP_##Type, Object)

#elif defined(WITH_CONTIKI)
#include "memb.h"

#define COAP_MALLOC_TYPE(Type) \
  ((coap_##Type##_t *)memb_alloc(&(Type##_storage)))
#define COAP_FREE_TYPE(Type, Object) memb_free(&(Type##_storage), (Object))

MEMB(subscription_storage, coap_subscription_t, COAP_MAX_SUBSCRIBERS);

void
coap_resources_init() {
  memb_init(&subscription_storage);
}

COAP_STATIC_INLINE coap_subscription_t *
coap_malloc_subscription() {
  return memb_alloc(&subscription_storage);
}

COAP_STATIC_INLINE void
coap_free_subscription(coap_subscription_t *subscription) {
  memb_free(&subscription_storage, subscription);
}

#else
#define COAP_MALLOC_TYPE(Type) \
  ((coap_##Type##_t *)coap_malloc(sizeof(coap_##Type##_t)))
#define COAP_FREE_TYPE(Type, Object) coap_free(Object)
#endif

#define COAP_PRINT_STATUS_MAX (~COAP_PRINT_STATUS_MASK)

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

/* Helper functions for conditional output of character sequences into
 * a given buffer. The first Offset characters are skipped.
 */

/**
 * Adds Char to Buf if Offset is zero. Otherwise, Char is not written
 * and Offset is decremented.
 */
#define PRINT_WITH_OFFSET(Buf,Offset,Char)                \
  if ((Offset) == 0) {                                        \
    (*(Buf)++) = (Char);                                \
  } else {                                                \
    (Offset)--;                                                \
  }                                                        \

/**
 * Adds Char to Buf if Offset is zero and Buf is less than Bufend.
 */
#define PRINT_COND_WITH_OFFSET(Buf,Bufend,Offset,Char,Result) {                \
    if ((Buf) < (Bufend)) {                                                \
      PRINT_WITH_OFFSET(Buf,Offset,Char);                                \
    }                                                                        \
    (Result)++;                                                                \
  }

/**
 * Copies at most Length characters of Str to Buf. The first Offset
 * characters are skipped. Output may be truncated to Bufend - Buf
 * characters.
 */
#define COPY_COND_WITH_OFFSET(Buf,Bufend,Offset,Str,Length,Result) {        \
    size_t i;                                                                \
    for (i = 0; i < (Length); i++) {                                        \
      PRINT_COND_WITH_OFFSET((Buf), (Bufend), (Offset), (Str)[i], (Result)); \
    }                                                                        \
  }

#ifndef WITHOUT_QUERY_FILTER
static int
match(const coap_str_const_t *text, const coap_str_const_t *pattern, int match_prefix,
  int match_substring
) {
  assert(text); assert(pattern);

  if (text->length < pattern->length)
    return 0;

  if (match_substring) {
    const uint8_t *next_token = text->s;
    size_t remaining_length = text->length;
    while (remaining_length) {
      size_t token_length;
      const uint8_t *token = next_token;
      next_token = (unsigned char *)memchr(token, ' ', remaining_length);

      if (next_token) {
        token_length = next_token - token;
        remaining_length -= (token_length + 1);
        next_token++;
      } else {
        token_length = remaining_length;
        remaining_length = 0;
      }

      if ((match_prefix || pattern->length == token_length) &&
            memcmp(token, pattern->s, pattern->length) == 0)
        return 1;
    }
    return 0;
  }

  return (match_prefix || pattern->length == text->length) &&
    memcmp(text->s, pattern->s, pattern->length) == 0;
}
#endif

/**
 * Prints the names of all known resources to @p buf. This function
 * sets @p buflen to the number of bytes actually written and returns
 * @c 1 on succes. On error, the value in @p buflen is undefined and
 * the return value will be @c 0.
 *
 * @param context The context with the resource map.
 * @param buf     The buffer to write the result.
 * @param buflen  Must be initialized to the maximum length of @p buf and will be
 *                set to the length of the well-known response on return.
 * @param offset  The offset in bytes where the output shall start and is
 *                shifted accordingly with the characters that have been
 *                processed. This parameter is used to support the block
 *                option.
 * @param query_filter A filter query according to <a href="http://tools.ietf.org/html/draft-ietf-core-link-format-11#section-4.1">Link Format</a>
 *
 * @return COAP_PRINT_STATUS_ERROR on error. Otherwise, the lower 28 bits are
 *         set to the number of bytes that have actually been written to
 *         @p buf. COAP_PRINT_STATUS_TRUNC is set when the output has been
 *         truncated.
 */
#if defined(__GNUC__) && defined(WITHOUT_QUERY_FILTER)
coap_print_status_t
coap_print_wellknown(coap_context_t *context, unsigned char *buf, size_t *buflen,
                size_t offset,
                coap_opt_t *query_filter __attribute__ ((unused))) {
#else /* not a GCC */
coap_print_status_t
coap_print_wellknown(coap_context_t *context, unsigned char *buf, size_t *buflen,
                size_t offset, coap_opt_t *query_filter) {
#endif /* GCC */
  size_t output_length = 0;
  unsigned char *p = buf;
  const uint8_t *bufend = buf + *buflen;
  size_t left, written = 0;
  coap_print_status_t result;
  const size_t old_offset = offset;
  int subsequent_resource = 0;
#ifndef WITHOUT_QUERY_FILTER
  coap_str_const_t resource_param = { 0, NULL }, query_pattern = { 0, NULL };
  int flags = 0; /* MATCH_SUBSTRING, MATCH_PREFIX, MATCH_URI */
#define MATCH_URI       0x01
#define MATCH_PREFIX    0x02
#define MATCH_SUBSTRING 0x04
  static const coap_str_const_t _rt_attributes[] = {
    {2, (const uint8_t *)"rt"},
    {2, (const uint8_t *)"if"},
    {3, (const uint8_t *)"rel"},
    {0, NULL}};
#endif /* WITHOUT_QUERY_FILTER */

#ifndef WITHOUT_QUERY_FILTER
  /* split query filter, if any */
  if (query_filter) {
    resource_param.s = coap_opt_value(query_filter);
    while (resource_param.length < coap_opt_length(query_filter)
           && resource_param.s[resource_param.length] != '=')
      resource_param.length++;

    if (resource_param.length < coap_opt_length(query_filter)) {
      const coap_str_const_t *rt_attributes;
      if (resource_param.length == 4 &&
          memcmp(resource_param.s, "href", 4) == 0)
        flags |= MATCH_URI;

      for (rt_attributes = _rt_attributes; rt_attributes->s; rt_attributes++) {
        if (resource_param.length == rt_attributes->length &&
            memcmp(resource_param.s, rt_attributes->s, rt_attributes->length) == 0) {
          flags |= MATCH_SUBSTRING;
          break;
        }
      }

      /* rest is query-pattern */
      query_pattern.s =
        coap_opt_value(query_filter) + resource_param.length + 1;

      assert((resource_param.length + 1) <= coap_opt_length(query_filter));
      query_pattern.length =
        coap_opt_length(query_filter) - (resource_param.length + 1);

     if ((query_pattern.s[0] == '/') && ((flags & MATCH_URI) == MATCH_URI)) {
       query_pattern.s++;
       query_pattern.length--;
      }

      if (query_pattern.length &&
          query_pattern.s[query_pattern.length-1] == '*') {
        query_pattern.length--;
        flags |= MATCH_PREFIX;
      }
    }
  }
#endif /* WITHOUT_QUERY_FILTER */

  RESOURCES_ITER(context->resources, r) {

#ifndef WITHOUT_QUERY_FILTER
    if (resource_param.length) { /* there is a query filter */

      if (flags & MATCH_URI) {        /* match resource URI */
        if (!match(r->uri_path, &query_pattern, (flags & MATCH_PREFIX) != 0,
            (flags & MATCH_SUBSTRING) != 0))
          continue;
      } else {                        /* match attribute */
        coap_attr_t *attr;
        coap_str_const_t unquoted_val;
        attr = coap_find_attr(r, &resource_param);
        if (!attr || !attr->value) continue;
        unquoted_val = *attr->value;
        if (attr->value->s[0] == '"') {          /* if attribute has a quoted value, remove double quotes */
          unquoted_val.length -= 2;
          unquoted_val.s += 1;
        }
        if (!(match(&unquoted_val, &query_pattern,
                    (flags & MATCH_PREFIX) != 0,
                    (flags & MATCH_SUBSTRING) != 0)))
          continue;
      }
    }
#endif /* WITHOUT_QUERY_FILTER */

    if (!subsequent_resource) {        /* this is the first resource  */
      subsequent_resource = 1;
    } else {
      PRINT_COND_WITH_OFFSET(p, bufend, offset, ',', written);
    }

    left = bufend - p; /* calculate available space */
    result = coap_print_link(r, p, &left, &offset);

    if (result & COAP_PRINT_STATUS_ERROR) {
      break;
    }

    /* coap_print_link() returns the number of characters that
     * where actually written to p. Now advance to its end. */
    p += COAP_PRINT_OUTPUT_LENGTH(result);
    written += left;
  }

  *buflen = written;
  output_length = p - buf;

  if (output_length > COAP_PRINT_STATUS_MAX) {
    return COAP_PRINT_STATUS_ERROR;
  }

  result = (coap_print_status_t)output_length;

  if (result + old_offset - offset < *buflen) {
    result |= COAP_PRINT_STATUS_TRUNC;
  }
  return result;
}

static coap_str_const_t *null_path = coap_make_str_const("");

coap_resource_t *
coap_resource_init(coap_str_const_t *uri_path, int flags) {
  coap_resource_t *r;

  r = (coap_resource_t *)coap_malloc_type(COAP_RESOURCE, sizeof(coap_resource_t));
  if (r) {
    memset(r, 0, sizeof(coap_resource_t));

    if (!(flags & COAP_RESOURCE_FLAGS_RELEASE_URI)) {
      /* Need to take a copy if caller is not providing a release request */
      if (uri_path)
        uri_path = coap_new_str_const(uri_path->s, uri_path->length);
      else
        uri_path = coap_new_str_const(null_path->s, null_path->length);
    }
    else if (!uri_path) {
      /* Do not expecte this, but ... */
      uri_path = coap_new_str_const(null_path->s, null_path->length);
    }

    if (uri_path)
      r->uri_path = uri_path;

    r->flags = flags;
  } else {
    coap_log(LOG_DEBUG, "coap_resource_init: no memory left\n");
  }

  return r;
}

static const uint8_t coap_unknown_resource_uri[] =
                       "- Unknown -";

coap_resource_t *
coap_resource_unknown_init(coap_method_handler_t put_handler) {
  coap_resource_t *r;

  r = (coap_resource_t *)coap_malloc_type(COAP_RESOURCE, sizeof(coap_resource_t));
  if (r) {
    memset(r, 0, sizeof(coap_resource_t));
    r->is_unknown = 1;
    /* Something unlikely to be used, but it shows up in the logs */
    r->uri_path = coap_new_str_const(coap_unknown_resource_uri, sizeof(coap_unknown_resource_uri)-1);
    coap_register_handler(r, COAP_REQUEST_PUT, put_handler);
  } else {
    coap_log(LOG_DEBUG, "coap_resource_unknown_init: no memory left\n");
  }

  return r;
}

coap_attr_t *
coap_add_attr(coap_resource_t *resource,
              coap_str_const_t *name,
              coap_str_const_t *val,
              int flags) {
  coap_attr_t *attr;

  if (!resource || !name)
    return NULL;

  attr = (coap_attr_t *)coap_malloc_type(COAP_RESOURCEATTR, sizeof(coap_attr_t));

  if (attr) {
    if (!(flags & COAP_ATTR_FLAGS_RELEASE_NAME)) {
      /* Need to take a copy if caller is not providing a release request */
      name = coap_new_str_const(name->s, name->length);
    }
    attr->name = name;
    if (val) {
      if (!(flags & COAP_ATTR_FLAGS_RELEASE_VALUE)) {
        /* Need to take a copy if caller is not providing a release request */
        val = coap_new_str_const(val->s, val->length);
      }
    }
    attr->value = val;

    attr->flags = flags;

    /* add attribute to resource list */
    LL_PREPEND(resource->link_attr, attr);
  } else {
    coap_log(LOG_DEBUG, "coap_add_attr: no memory left\n");
  }

  return attr;
}

coap_attr_t *
coap_find_attr(coap_resource_t *resource,
               coap_str_const_t *name) {
  coap_attr_t *attr;

  if (!resource || !name)
    return NULL;

  LL_FOREACH(resource->link_attr, attr) {
    if (attr->name->length == name->length &&
        memcmp(attr->name->s, name->s, name->length) == 0)
      return attr;
  }

  return NULL;
}

void
coap_delete_attr(coap_attr_t *attr) {
  if (!attr)
    return;
  coap_delete_str_const(attr->name);
  if (attr->value) {
    coap_delete_str_const(attr->value);
  }

#ifdef WITH_LWIP_LIBCOAP
  memp_free(MEMP_COAP_RESOURCEATTR, attr);
#endif
#ifndef WITH_LWIP_LIBCOAP
  coap_free_type(COAP_RESOURCEATTR, attr);
#endif
}

static void
coap_free_resource(coap_resource_t *resource) {
  coap_attr_t *attr, *tmp;
  coap_subscription_t *obs, *otmp;

  assert(resource);

  /* delete registered attributes */
  LL_FOREACH_SAFE(resource->link_attr, attr, tmp) coap_delete_attr(attr);

  /* Either the application provided or libcoap copied - need to delete it */
  coap_delete_str_const(resource->uri_path);

  /* free all elements from resource->subscribers */
  LL_FOREACH_SAFE( resource->subscribers, obs, otmp ) {
    coap_session_release( obs->session );
    if (obs->query)
      coap_delete_string(obs->query);
    COAP_FREE_TYPE( subscription, obs );
  }

#ifdef WITH_LWIP_LIBCOAP
  memp_free(MEMP_COAP_RESOURCE, resource);
#endif
#ifndef WITH_LWIP_LIBCOAP
  coap_free_type(COAP_RESOURCE, resource);
#endif /* WITH_CONTIKI */
}

void
coap_add_resource(coap_context_t *context, coap_resource_t *resource) {
  if (resource->is_unknown) {
    if (context->unknown_resource)
      coap_free_resource(context->unknown_resource);
    context->unknown_resource = resource;
  }
  else {
    coap_resource_t *r = coap_get_resource_from_uri_path(context,
                                                         resource->uri_path);

    if (r) {
      coap_log(LOG_WARNING,
        "coap_add_resource: Duplicate uri_path '%*.*s', old resource deleted\n",
              (int)resource->uri_path->length, (int)resource->uri_path->length,
              resource->uri_path->s);
      coap_delete_resource(context, r);
    }
    RESOURCES_ADD(context->resources, resource);
  }
}

int
coap_delete_resource(coap_context_t *context, coap_resource_t *resource) {
  if (!context || !resource)
    return 0;

  if (resource->is_unknown && (context->unknown_resource == resource)) {
    coap_free_resource(context->unknown_resource);
    context->unknown_resource = NULL;
    return 1;
  }

  /* remove resource from list */
  RESOURCES_DELETE(context->resources, resource);

  /* and free its allocated memory */
  coap_free_resource(resource);

  return 1;
}

void
coap_delete_all_resources(coap_context_t *context) {
  coap_resource_t *res;
  coap_resource_t *rtmp;

  /* Cannot call RESOURCES_ITER because coap_free_resource() releases
   * the allocated storage. */

  HASH_ITER(hh, context->resources, res, rtmp) {
    HASH_DELETE(hh, context->resources, res);
    coap_free_resource(res);
  }

  context->resources = NULL;

  if (context->unknown_resource) {
    coap_free_resource(context->unknown_resource);
    context->unknown_resource = NULL;
  }
}

coap_resource_t *
coap_get_resource_from_uri_path(coap_context_t *context, coap_str_const_t *uri_path) {
  coap_resource_t *result;

  RESOURCES_FIND(context->resources, uri_path, result);

  return result;
}

coap_print_status_t
coap_print_link(const coap_resource_t *resource,
                unsigned char *buf, size_t *len, size_t *offset) {
  unsigned char *p = buf;
  const uint8_t *bufend = buf + *len;
  coap_attr_t *attr;
  coap_print_status_t result = 0;
  size_t output_length = 0;
  const size_t old_offset = *offset;

  *len = 0;
  PRINT_COND_WITH_OFFSET(p, bufend, *offset, '<', *len);
  PRINT_COND_WITH_OFFSET(p, bufend, *offset, '/', *len);

  COPY_COND_WITH_OFFSET(p, bufend, *offset,
                        resource->uri_path->s, resource->uri_path->length, *len);

  PRINT_COND_WITH_OFFSET(p, bufend, *offset, '>', *len);

  LL_FOREACH(resource->link_attr, attr) {

    PRINT_COND_WITH_OFFSET(p, bufend, *offset, ';', *len);

    COPY_COND_WITH_OFFSET(p, bufend, *offset,
                          attr->name->s, attr->name->length, *len);

    if (attr->value && attr->value->s) {
      PRINT_COND_WITH_OFFSET(p, bufend, *offset, '=', *len);

      COPY_COND_WITH_OFFSET(p, bufend, *offset,
                            attr->value->s, attr->value->length, *len);
    }

  }
  if (resource->observable) {
    COPY_COND_WITH_OFFSET(p, bufend, *offset, ";obs", 4, *len);
  }

  output_length = p - buf;

  if (output_length > COAP_PRINT_STATUS_MAX) {
    return COAP_PRINT_STATUS_ERROR;
  }

  result = (coap_print_status_t)output_length;

  if (result + old_offset - *offset < *len) {
    result |= COAP_PRINT_STATUS_TRUNC;
  }

  return result;
}

void
coap_register_handler(coap_resource_t *resource,
                      unsigned char method,
                      coap_method_handler_t handler) {
  assert(resource);
  assert(method > 0 && (size_t)(method-1) < sizeof(resource->handler)/sizeof(coap_method_handler_t));
  resource->handler[method-1] = handler;
}

#ifndef WITHOUT_OBSERVE
coap_subscription_t *
coap_find_observer(coap_resource_t *resource, coap_session_t *session,
                     const coap_binary_t *token) {
  coap_subscription_t *s;

  assert(resource);
  assert(session);

  LL_FOREACH(resource->subscribers, s) {
    if (s->session == session
        && (!token || (token->length == s->token_length
                       && memcmp(token->s, s->token, token->length) == 0)))
      return s;
  }

  return NULL;
}

static coap_subscription_t *
coap_find_observer_query(coap_resource_t *resource, coap_session_t *session,
                     const coap_string_t *query) {
  coap_subscription_t *s;

  assert(resource);
  assert(session);

  LL_FOREACH(resource->subscribers, s) {
    if (s->session == session
        && ((!query && !s->query)
             || (query && s->query && coap_string_equal(query, s->query))))
      return s;
  }

  return NULL;
}

coap_subscription_t *
coap_add_observer(coap_resource_t *resource,
                  coap_session_t *session,
                  const coap_binary_t *token,
                  coap_string_t *query,
                  int has_block2,
                  coap_block_t block2) {
  coap_subscription_t *s;

  assert( session );

  /* Check if there is already a subscription for this peer. */
  s = coap_find_observer(resource, session, token);
  if (!s) {
    /*
     * Cannot allow a duplicate to be created for the same query as application
     * may not be cleaning up duplicates.  If duplicate found, then original
     * observer is deleted and a new one created with the new token
     */
    s = coap_find_observer_query(resource, session, query);
    if (s) {
      /* Delete old entry with old token */
      coap_binary_t tmp_token = { s->token_length, s->token };
      coap_delete_observer(resource, session, &tmp_token);
      s = NULL;
    }
  }

  /* We are done if subscription was found. */
  if (s) {
    if (s->query)
      coap_delete_string(s->query);
    s->query = query;
    return s;
  }

  /* s points to a different subscription, so we have to create
   * another one. */
  s = COAP_MALLOC_TYPE(subscription);

  if (!s) {
    if (query)
      coap_delete_string(query);
    return NULL;
  }

  coap_subscription_init(s);
  s->session = coap_session_reference( session );

  if (token && token->length) {
    s->token_length = token->length;
    memcpy(s->token, token->s, min(s->token_length, 8));
  }

  s->query = query;

  s->has_block2 = has_block2;
  s->block2 = block2;

  /* add subscriber to resource */
  LL_PREPEND(resource->subscribers, s);

  coap_log(LOG_DEBUG, "create new subscription\n");

  return s;
}

void
coap_touch_observer(coap_context_t *context, coap_session_t *session,
                    const coap_binary_t *token) {
  coap_subscription_t *s;

  RESOURCES_ITER(context->resources, r) {
    s = coap_find_observer(r, session, token);
    if (s) {
      s->fail_cnt = 0;
    }
  }
}

int
coap_delete_observer(coap_resource_t *resource, coap_session_t *session,
                     const coap_binary_t *token) {
  coap_subscription_t *s;

  s = coap_find_observer(resource, session, token);

  if ( s && coap_get_log_level() >= LOG_DEBUG ) {
    char outbuf[2 * 8 + 1] = "";
    unsigned int i;
    for ( i = 0; i < s->token_length; i++ )
      snprintf( &outbuf[2 * i], 3, "%02x", s->token[i] );
    coap_log(LOG_DEBUG, "removed observer tid %s\n", outbuf);
  }

  if (resource->subscribers && s) {
    LL_DELETE(resource->subscribers, s);
    coap_session_release( session );
    if (s->query)
      coap_delete_string(s->query);
    COAP_FREE_TYPE(subscription,s);
  }

  return s != NULL;
}

void
coap_delete_observers(coap_context_t *context, coap_session_t *session) {
  RESOURCES_ITER(context->resources, resource) {
    coap_subscription_t *s, *tmp;
    LL_FOREACH_SAFE(resource->subscribers, s, tmp) {
      if (s->session == session) {
        LL_DELETE(resource->subscribers, s);
        coap_session_release(session);
        if (s->query)
          coap_delete_string(s->query);
        COAP_FREE_TYPE(subscription, s);
      }
    }
  }
}

static void
coap_notify_observers(coap_context_t *context, coap_resource_t *r) {
  coap_method_handler_t h;
  coap_subscription_t *obs;
  coap_binary_t token;
  coap_pdu_t *response;

  if (r->observable && (r->dirty || r->partiallydirty)) {
    r->partiallydirty = 0;

    /* retrieve GET handler, prepare response */
    h = r->handler[COAP_REQUEST_GET - 1];
    assert(h);                /* we do not allow subscriptions if no
                         * GET handler is defined */

    LL_FOREACH(r->subscribers, obs) {
      if (r->dirty == 0 && obs->dirty == 0)
        /*
         * running this resource due to partiallydirty, but this observation's
         * notification was already enqueued
         */
        continue;
      if (obs->session->con_active >= COAP_DEFAULT_NSTART &&
          ((r->flags & COAP_RESOURCE_FLAGS_NOTIFY_CON) ||
           (obs->non_cnt >= COAP_OBS_MAX_NON)))
        continue;

      coap_tid_t tid = COAP_INVALID_TID;
      obs->dirty = 0;
      /* initialize response */
      response = coap_pdu_init(COAP_MESSAGE_CON, 0, 0, coap_session_max_pdu_size(obs->session));
      if (!response) {
        obs->dirty = 1;
        r->partiallydirty = 1;
        coap_log(LOG_DEBUG,
                 "coap_check_notify: pdu init failed, resource stays "
                 "partially dirty\n");
        continue;
      }

      if (!coap_add_token(response, obs->token_length, obs->token)) {
        obs->dirty = 1;
        r->partiallydirty = 1;
        coap_log(LOG_DEBUG,
                 "coap_check_notify: cannot add token, resource stays "
                 "partially dirty\n");
        coap_delete_pdu(response);
        continue;
      }

      token.length = obs->token_length;
      token.s = obs->token;

      response->tid = coap_new_message_id(obs->session);
      if ((r->flags & COAP_RESOURCE_FLAGS_NOTIFY_CON) == 0
          && obs->non_cnt < COAP_OBS_MAX_NON) {
        response->type = COAP_MESSAGE_NON;
      } else {
        response->type = COAP_MESSAGE_CON;
      }
      /* fill with observer-specific data */
      h(context, r, obs->session, NULL, &token, obs->query, response);

      /* TODO: do not send response and remove observer when
       *  COAP_RESPONSE_CLASS(response->hdr->code) > 2
       */
      if (response->type == COAP_MESSAGE_CON) {
        obs->non_cnt = 0;
      } else {
        obs->non_cnt++;
      }

      tid = coap_send( obs->session, response );

      if (COAP_INVALID_TID == tid) {
        coap_log(LOG_DEBUG,
                 "coap_check_notify: sending failed, resource stays "
                 "partially dirty\n");
        obs->dirty = 1;
        r->partiallydirty = 1;
      }

    }
  }
  r->dirty = 0;
}

int
coap_resource_set_dirty(coap_resource_t *r, const coap_string_t *query) {
  return coap_resource_notify_observers(r, query);
}

int
coap_resource_notify_observers(coap_resource_t *r, const coap_string_t *query) {
  if (!r->observable)
    return 0;
  if (query) {
    coap_subscription_t *obs;
    int found = 0;
    LL_FOREACH(r->subscribers, obs) {
      if (obs->query
       && obs->query->length==query->length
       && memcmp(obs->query->s, query->s, query->length)==0 ) {
        found = 1;
        if (!r->dirty && !obs->dirty) {
          obs->dirty = 1;
          r->partiallydirty = 1;
        }
      }
    }
    if (!found)
      return 0;
  } else {
    if ( !r->subscribers )
      return 0;
    r->dirty = 1;
  }

  /* Increment value for next Observe use. Observe value must be < 2^24 */
  r->observe = (r->observe + 1) & 0xFFFFFF;

  return 1;
}

void
coap_check_notify(coap_context_t *context) {

  RESOURCES_ITER(context->resources, r) {
    coap_notify_observers(context, r);
  }
}

/**
 * Checks the failure counter for (peer, token) and removes peer from
 * the list of observers for the given resource when COAP_OBS_MAX_FAIL
 * is reached.
 *
 * @param context  The CoAP context to use
 * @param resource The resource to check for (peer, token)
 * @param session  The observer's session
 * @param token    The token that has been used for subscription.
 */
static void
coap_remove_failed_observers(coap_context_t *context,
                             coap_resource_t *resource,
                             coap_session_t *session,
                             const coap_binary_t *token) {
  coap_subscription_t *obs, *otmp;

  LL_FOREACH_SAFE(resource->subscribers, obs, otmp) {
    if ( obs->session == session &&
        token->length == obs->token_length &&
        memcmp(token->s, obs->token, token->length) == 0) {

      /* count failed notifies and remove when
       * COAP_MAX_FAILED_NOTIFY is reached */
      if (obs->fail_cnt < COAP_OBS_MAX_FAIL)
        obs->fail_cnt++;
      else {
        LL_DELETE(resource->subscribers, obs);
        obs->fail_cnt = 0;

#ifndef NDEBUG
        if (LOG_DEBUG <= coap_get_log_level()) {
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 40
#endif
          unsigned char addr[INET6_ADDRSTRLEN+8];

          if (coap_print_addr(&obs->session->remote_addr, addr, INET6_ADDRSTRLEN+8))
            coap_log(LOG_DEBUG, "** removed observer %s\n", addr);
        }
#endif
        coap_cancel_all_messages(context, obs->session,
                                 obs->token, obs->token_length);
        coap_session_release( obs->session );
        if (obs->query)
          coap_delete_string(obs->query);
        COAP_FREE_TYPE(subscription, obs);
      }
      break;                        /* break loop if observer was found */
    }
  }
}

void
coap_handle_failed_notify(coap_context_t *context,
                          coap_session_t *session,
                          const coap_binary_t *token) {

  RESOURCES_ITER(context->resources, r) {
        coap_remove_failed_observers(context, r, session, token);
  }
}
#endif /* WITHOUT_NOTIFY */
