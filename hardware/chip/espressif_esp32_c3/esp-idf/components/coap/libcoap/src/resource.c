/* resource.c -- generic resource handling
 *
 * Copyright (C) 2010--2021 Olaf Bergmann <bergmann@tzi.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap3/coap_internal.h"

#include <stdio.h>
#include <errno.h>

#ifdef COAP_EPOLL_SUPPORT
#include <sys/epoll.h>
#include <sys/timerfd.h>
#endif /* COAP_EPOLL_SUPPORT */

#if defined(WITH_LWIP)
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
                coap_opt_t *query_filter COAP_UNUSED) {
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

static coap_str_const_t null_path_value = {0, (const uint8_t*)""};
static coap_str_const_t *null_path = &null_path_value;

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
    r->observe = 2;
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

static const uint8_t coap_proxy_resource_uri[] =
                       "- Proxy URI -";

coap_resource_t *
coap_resource_proxy_uri_init(coap_method_handler_t handler,
                      size_t host_name_count, const char *host_name_list[]) {
  coap_resource_t *r;

  if (host_name_count == 0) {
    coap_log(LOG_ERR,
          "coap_resource_proxy_uri_init: Must have one or more host names defined\n");
    return NULL;
  }
  r = (coap_resource_t *)coap_malloc_type(COAP_RESOURCE, sizeof(coap_resource_t));
  if (r) {
    size_t i;
    memset(r, 0, sizeof(coap_resource_t));
    r->is_proxy_uri = 1;
    /* Something unlikely to be used, but it shows up in the logs */
    r->uri_path = coap_new_str_const(coap_proxy_resource_uri, sizeof(coap_proxy_resource_uri)-1);
    /* Preset all the handlers */
    for (i = 0; i < (sizeof(r->handler) / sizeof(r->handler[0])); i++) {
      r->handler[i] = handler;
    }
    if (host_name_count) {
      r->proxy_name_list = coap_malloc(host_name_count *
                                       sizeof(coap_str_const_t*));
      if (r->proxy_name_list) {
        for (i = 0; i < host_name_count; i++) {
          r->proxy_name_list[i] =
              coap_new_str_const((const uint8_t*)host_name_list[i],
                                 strlen(host_name_list[i]));
          if (!r->proxy_name_list[i]) {
            coap_log(LOG_ERR,
                     "coap_resource_proxy_uri_init: unable to add host name\n");
            if (i == 0) {
              coap_free(r->proxy_name_list);
              r->proxy_name_list = NULL;
            }
            break;
          }
        }
        r->proxy_name_count = i;
      }
    }
  } else {
    coap_log(LOG_DEBUG, "coap_resource_proxy_uri_init: no memory left\n");
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

coap_str_const_t *
coap_attr_get_value(coap_attr_t *attr) {
  if (attr)
    return attr->value;
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

#ifdef WITH_LWIP
  memp_free(MEMP_COAP_RESOURCEATTR, attr);
#endif
#ifndef WITH_LWIP
  coap_free_type(COAP_RESOURCEATTR, attr);
#endif
}

typedef enum coap_deleting_resource_t {
  COAP_DELETING_RESOURCE,
  COAP_NOT_DELETING_RESOURCE
} coap_deleting_resource_t;

static void coap_notify_observers(coap_context_t *context, coap_resource_t *r,
                                  coap_deleting_resource_t deleting);

static void
coap_free_resource(coap_resource_t *resource) {
  coap_attr_t *attr, *tmp;
  coap_subscription_t *obs, *otmp;

  assert(resource);

  coap_resource_notify_observers(resource, NULL);
  coap_notify_observers(resource->context, resource, COAP_DELETING_RESOURCE);

  if (resource->context->release_userdata && resource->user_data)
    resource->context->release_userdata(resource->user_data);

  /* delete registered attributes */
  LL_FOREACH_SAFE(resource->link_attr, attr, tmp) coap_delete_attr(attr);

  /* Either the application provided or libcoap copied - need to delete it */
  coap_delete_str_const(resource->uri_path);

  /* free all elements from resource->subscribers */
  LL_FOREACH_SAFE( resource->subscribers, obs, otmp ) {
    coap_session_release( obs->session );
    coap_delete_pdu(obs->pdu);
    coap_delete_cache_key(obs->cache_key);
    COAP_FREE_TYPE( subscription, obs );
  }
  if (resource->proxy_name_count && resource->proxy_name_list) {
    size_t i;

    for (i = 0; i < resource->proxy_name_count; i++) {
      coap_delete_str_const(resource->proxy_name_list[i]);
    }
    coap_free(resource->proxy_name_list);
  }

#ifdef WITH_LWIP
  memp_free(MEMP_COAP_RESOURCE, resource);
#endif
#ifndef WITH_LWIP
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
  else if (resource->is_proxy_uri) {
    if (context->proxy_uri_resource)
      coap_free_resource(context->proxy_uri_resource);
    context->proxy_uri_resource = resource;
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
  assert(resource->context == NULL);
  resource->context = context;
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
  if (resource->is_proxy_uri && (context->proxy_uri_resource == resource)) {
    coap_free_resource(context->proxy_uri_resource);
    context->proxy_uri_resource = NULL;
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
  if (context->proxy_uri_resource) {
    coap_free_resource(context->proxy_uri_resource);
    context->proxy_uri_resource = NULL;
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
                      coap_request_t method,
                      coap_method_handler_t handler) {
  assert(resource);
  assert(method > 0 && (size_t)(method-1) < sizeof(resource->handler)/sizeof(coap_method_handler_t));
  resource->handler[method-1] = handler;
}

coap_subscription_t *
coap_find_observer(coap_resource_t *resource, coap_session_t *session,
                     const coap_binary_t *token) {
  coap_subscription_t *s;

  assert(resource);
  assert(session);

  LL_FOREACH(resource->subscribers, s) {
    if (s->session == session
        && (!token || (token->length == s->pdu->token_length
                       && memcmp(token->s, s->pdu->token, token->length) == 0)))
      return s;
  }

  return NULL;
}

static coap_subscription_t *
coap_find_observer_cache_key(coap_resource_t *resource, coap_session_t *session,
                     const coap_cache_key_t *cache_key) {
  coap_subscription_t *s;

  assert(resource);
  assert(session);

  LL_FOREACH(resource->subscribers, s) {
    if (s->session == session
        && (memcmp(cache_key, s->cache_key, sizeof(coap_cache_key_t)) == 0))
      return s;
  }

  return NULL;
}

coap_subscription_t *
coap_add_observer(coap_resource_t *resource,
                  coap_session_t *session,
                  const coap_binary_t *token,
                  const coap_pdu_t *request) {
  coap_subscription_t *s;
  coap_cache_key_t *cache_key = NULL;
  size_t len;
  const uint8_t *data;
/* https://tools.ietf.org/html/rfc7641#section-3.6 */
static const uint16_t cache_ignore_options[] = { COAP_OPTION_ETAG };

  assert( session );

  /* Check if there is already a subscription for this peer. */
  s = coap_find_observer(resource, session, token);
  if (!s) {
    /*
     * Cannot allow a duplicate to be created for the same query as application
     * may not be cleaning up duplicates.  If duplicate found, then original
     * observer is deleted and a new one created with the new token
     */
    cache_key = coap_cache_derive_key_w_ignore(session, request,
                                               COAP_CACHE_IS_SESSION_BASED,
                                               cache_ignore_options,
                  sizeof(cache_ignore_options)/sizeof(cache_ignore_options[0]));
    if (cache_key) {
      s = coap_find_observer_cache_key(resource, session, cache_key);
      if (s) {
        /* Delete old entry with old token */
        coap_binary_t tmp_token = { s->pdu->token_length, s->pdu->token };
        coap_delete_observer(resource, session, &tmp_token);
        s = NULL;
      }
    }
  }

  /* We are done if subscription was found. */
  if (s) {
    return s;
  }

  /* Create a new subscription */
  s = COAP_MALLOC_TYPE(subscription);

  if (!s) {
    coap_delete_cache_key(cache_key);
    return NULL;
  }

  coap_subscription_init(s);
  s->pdu = coap_pdu_duplicate(request, session, request->token_length,
                              request->token, NULL);
  if (s->pdu == NULL) {
    coap_delete_cache_key(cache_key);
    COAP_FREE_TYPE(subscription, s);
    return NULL;
  }
  if (coap_get_data(request, &len, &data)) {
    coap_add_data(s->pdu, len, data);
  }
  if (cache_key == NULL) {
    cache_key = coap_cache_derive_key_w_ignore(session, request,
                                               COAP_CACHE_IS_SESSION_BASED,
                                               cache_ignore_options,
                  sizeof(cache_ignore_options)/sizeof(cache_ignore_options[0]));
    if (cache_key == NULL) {
      coap_delete_pdu(s->pdu);
      coap_delete_cache_key(cache_key);
      COAP_FREE_TYPE(subscription, s);
      return NULL;
    }
  }
  s->cache_key = cache_key;
  s->session = coap_session_reference(session);

  /* add subscriber to resource */
  LL_PREPEND(resource->subscribers, s);

  coap_log(LOG_DEBUG, "create new subscription %p key 0x%02x%02x%02x%02x\n",
           (void*)s, s->cache_key->key[0], s->cache_key->key[1],
           s->cache_key->key[2], s->cache_key->key[3]);

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
    for ( i = 0; i < s->pdu->token_length; i++ )
      snprintf( &outbuf[2 * i], 3, "%02x", s->pdu->token[i] );
    coap_log(LOG_DEBUG,
             "removed subscription %p with token '%s' key 0x%02x%02x%02x%02x\n",
             (void*)s, outbuf, s->cache_key->key[0], s->cache_key->key[1],
             s->cache_key->key[2], s-> cache_key->key[3]);
  }

  if (resource->subscribers && s) {
    LL_DELETE(resource->subscribers, s);
    coap_session_release( session );
    coap_delete_pdu(s->pdu);
    coap_delete_cache_key(s->cache_key);
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
        coap_delete_pdu(s->pdu);
        coap_delete_cache_key(s->cache_key);
        COAP_FREE_TYPE(subscription, s);
      }
    }
  }
}

static void
coap_notify_observers(coap_context_t *context, coap_resource_t *r,
                      coap_deleting_resource_t deleting) {
  coap_method_handler_t h;
  coap_subscription_t *obs;
  coap_binary_t token;
  coap_pdu_t *response;
  uint8_t buf[4];
  coap_string_t *query;
  coap_block_t block;

  if (r->observable && (r->dirty || r->partiallydirty)) {
    r->partiallydirty = 0;

    LL_FOREACH(r->subscribers, obs) {
      if (r->dirty == 0 && obs->dirty == 0) {
        /*
         * running this resource due to partiallydirty, but this observation's
         * notification was already enqueued
         */
        context->observe_pending = 1;
        continue;
      }
      if (obs->session->con_active >= COAP_DEFAULT_NSTART &&
          ((r->flags & COAP_RESOURCE_FLAGS_NOTIFY_CON) ||
           (obs->non_cnt >= COAP_OBS_MAX_NON))) {
        r->partiallydirty = 1;
        obs->dirty = 1;
        context->observe_pending = 1;
        continue;
      }

      coap_mid_t mid = COAP_INVALID_MID;
      obs->dirty = 0;
      /* initialize response */
      response = coap_pdu_init(COAP_MESSAGE_CON, 0, 0, coap_session_max_pdu_size(obs->session));
      if (!response) {
        obs->dirty = 1;
        r->partiallydirty = 1;
        context->observe_pending = 1;
        coap_log(LOG_DEBUG,
                 "coap_check_notify: pdu init failed, resource stays "
                 "partially dirty\n");
        continue;
      }

      if (!coap_add_token(response, obs->pdu->token_length, obs->pdu->token)) {
        obs->dirty = 1;
        r->partiallydirty = 1;
        context->observe_pending = 1;
        coap_log(LOG_DEBUG,
                 "coap_check_notify: cannot add token, resource stays "
                 "partially dirty\n");
        coap_delete_pdu(response);
        continue;
      }

      token.length = obs->pdu->token_length;
      token.s = obs->pdu->token;

      obs->pdu->mid = response->mid = coap_new_message_id(obs->session);
      if ((r->flags & COAP_RESOURCE_FLAGS_NOTIFY_CON) == 0 &&
          ((r->flags & COAP_RESOURCE_FLAGS_NOTIFY_NON_ALWAYS) ||
           obs->non_cnt < COAP_OBS_MAX_NON)) {
        response->type = COAP_MESSAGE_NON;
      } else {
        response->type = COAP_MESSAGE_CON;
      }
      switch (deleting) {
      case COAP_NOT_DELETING_RESOURCE:
        /* fill with observer-specific data */
        coap_add_option(response, COAP_OPTION_OBSERVE,
                        coap_encode_var_safe(buf, sizeof (buf),
                                             r->observe),
                        buf);
        if (coap_get_block(obs->pdu, COAP_OPTION_BLOCK2, &block)) {
          /* Will get updated later (e.g. M bit) if appropriate */
          coap_add_option(response, COAP_OPTION_BLOCK2,
                          coap_encode_var_safe(buf, sizeof(buf),
                                               ((0 << 4) |
                                                (0 << 3) |
                                                block.szx)),
                          buf);
        }

        h = r->handler[obs->pdu->code - 1];
        assert(h);      /* we do not allow subscriptions if no
                         * GET/FETCH handler is defined */
        query = coap_get_query(obs->pdu);
        coap_log(LOG_DEBUG, "Observe PDU presented to app.\n");
        coap_show_pdu(LOG_DEBUG, obs->pdu);
        h(r, obs->session, obs->pdu, query, response);
        /* Check if lg_xmit generated and update PDU code if so */
        coap_check_code_lg_xmit(obs->session, response, r, query);
        coap_delete_string(query);
        if (COAP_RESPONSE_CLASS(response->code) != 2) {
          coap_remove_option(response, COAP_OPTION_OBSERVE);
        }
        if (COAP_RESPONSE_CLASS(response->code) > 2) {
          coap_delete_observer(r, obs->session, &token);
        }
        break;
      case COAP_DELETING_RESOURCE:
      default:
        /* Don't worry if it does not get there */
        response->type = COAP_MESSAGE_NON;
        response->code = COAP_RESPONSE_CODE(404);
        break;
      }

      if (response->type == COAP_MESSAGE_CON ||
          (r->flags & COAP_RESOURCE_FLAGS_NOTIFY_NON_ALWAYS)) {
        obs->non_cnt = 0;
      } else {
        obs->non_cnt++;
      }

      mid = coap_send_internal( obs->session, response );

      if (COAP_INVALID_MID == mid) {
        coap_log(LOG_DEBUG,
                 "coap_check_notify: sending failed, resource stays "
                 "partially dirty\n");
        obs->dirty = 1;
        r->partiallydirty = 1;
        context->observe_pending = 1;
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
coap_resource_notify_observers(coap_resource_t *r,
                               const coap_string_t *query COAP_UNUSED) {
  if (!r->observable)
    return 0;
  if ( !r->subscribers )
    return 0;
  r->dirty = 1;

  /* Increment value for next Observe use. Observe value must be < 2^24 */
  r->observe = (r->observe + 1) & 0xFFFFFF;

  assert(r->context);
  r->context->observe_pending = 1;
#ifdef COAP_EPOLL_SUPPORT
  if (r->context->eptimerfd != -1) {
    /* Need to immediately trigger any epoll_wait() */
    struct itimerspec new_value;
    int ret;

    memset(&new_value, 0, sizeof(new_value));
    new_value.it_value.tv_nsec = 1; /* small that is not zero */
    ret = timerfd_settime(r->context->eptimerfd, 0, &new_value, NULL);
    if (ret == -1) {
      coap_log(LOG_ERR,
                "%s: timerfd_settime failed: %s (%d)\n",
                "coap_resource_notify_observers",
                coap_socket_strerror(), errno);
    }
  }
#endif /* COAP_EPOLL_SUPPORT */
  return 1;
}

void
coap_resource_set_mode(coap_resource_t *resource, int mode) {
  resource->flags = (resource->flags &
    ~(COAP_RESOURCE_FLAGS_NOTIFY_CON|COAP_RESOURCE_FLAGS_NOTIFY_NON)) |
    (mode & (COAP_RESOURCE_FLAGS_NOTIFY_CON|COAP_RESOURCE_FLAGS_NOTIFY_NON));
}

void
coap_resource_set_userdata(coap_resource_t *resource, void *data) {
  resource->user_data = data;
}

void *
coap_resource_get_userdata(coap_resource_t *resource) {
  return resource->user_data;
}

void
coap_resource_release_userdata_handler(coap_context_t *context,
                          coap_resource_release_userdata_handler_t callback) {
  context->release_userdata = callback;
}

void
coap_resource_set_get_observable(coap_resource_t *resource, int mode) {
  resource->observable = mode ? 1 : 0;
}

coap_str_const_t*
coap_resource_get_uri_path(coap_resource_t *resource) {
  if (resource)
    return resource->uri_path;
  return NULL;
}

void
coap_check_notify(coap_context_t *context) {

  if (context->observe_pending) {
    context->observe_pending = 0;
    RESOURCES_ITER(context->resources, r) {
      coap_notify_observers(context, r, COAP_NOT_DELETING_RESOURCE);
    }
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
        token->length == obs->pdu->token_length &&
        memcmp(token->s, obs->pdu->token, token->length) == 0) {

      /* count failed notifies and remove when
       * COAP_MAX_FAILED_NOTIFY is reached */
      if (obs->fail_cnt < COAP_OBS_MAX_FAIL)
        obs->fail_cnt++;
      else {
        LL_DELETE(resource->subscribers, obs);
        obs->fail_cnt = 0;

        if (LOG_DEBUG <= coap_get_log_level()) {
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 40
#endif
          unsigned char addr[INET6_ADDRSTRLEN+8];

          if (coap_print_addr(&obs->session->addr_info.remote,
                              addr, INET6_ADDRSTRLEN+8))
            coap_log(LOG_DEBUG, "** removed observer %s\n", addr);
        }
        coap_cancel_all_messages(context, obs->session,
                                 obs->pdu->token, obs->pdu->token_length);
        coap_session_release( obs->session );
        coap_delete_pdu(obs->pdu);
        coap_delete_cache_key(obs->cache_key);
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
