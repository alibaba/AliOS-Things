/* uri.c -- helper functions for URI treatment
 *
 * Copyright (C) 2010--2012,2015-2016 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap_config.h"

#if defined(HAVE_ASSERT_H) && !defined(assert)
# include <assert.h>
#endif

#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "libcoap.h"
#include "mem.h"
#include "coap_debug.h"
#include "pdu.h"
#include "option.h"
#include "uri.h"

/**
 * A length-safe version of strchr(). This function returns a pointer
 * to the first occurrence of @p c  in @p s, or @c NULL if not found.
 *
 * @param s   The string to search for @p c.
 * @param len The length of @p s.
 * @param c   The character to search.
 *
 * @return A pointer to the first occurence of @p c, or @c NULL
 * if not found.
 */
COAP_STATIC_INLINE const uint8_t *
strnchr(const uint8_t *s, size_t len, unsigned char c) {
  while (len && *s++ != c)
    --len;

  return len ? s : NULL;
}

#define ISEQUAL_CI(a,b) \
  ((a) == (b) || (islower(b) && ((a) == ((b) - 0x20))))

int
coap_split_uri(const uint8_t *str_var, size_t len, coap_uri_t *uri) {
  const uint8_t *p, *q;
  int res = 0;

  if (!str_var || !uri)
    return -1;

  memset(uri, 0, sizeof(coap_uri_t));
  uri->port = COAP_DEFAULT_PORT;

  /* search for scheme */
  p = str_var;
  if (*p == '/') {
    q = p;
    goto path;
  }

  q = (const uint8_t *)COAP_DEFAULT_SCHEME;
  while (len && *q && ISEQUAL_CI(*p, *q)) {
    ++p; ++q; --len;
  }

  /* If q does not point to the string end marker '\0', the schema
   * identifier is wrong. */
  if (*q) {
    res = -1;
    goto error;
  }

  /* There might be an additional 's', indicating the secure version: */
  if (len && (*p == 's')) {
    ++p; --len;
    uri->scheme = COAP_URI_SCHEME_COAPS;
    uri->port = COAPS_DEFAULT_PORT;
  } else {
    uri->scheme = COAP_URI_SCHEME_COAP;
  }

  /* There might be and addition "+tcp", indicating reliable transport: */
  if (len>=4 && p[0] == '+' && p[1] == 't' && p[2] == 'c' && p[3] == 'p' ) {
    p += 4;
    len -= 4;
    if (uri->scheme == COAP_URI_SCHEME_COAPS)
      uri->scheme = COAP_URI_SCHEME_COAPS_TCP;
    else
      uri->scheme = COAP_URI_SCHEME_COAP_TCP;
  }
  q = (const uint8_t *)"://";
  while (len && *q && *p == *q) {
    ++p; ++q; --len;
  }

  if (*q) {
    res = -2;
    goto error;
  }

  /* p points to beginning of Uri-Host */
  q = p;
  if (len && *p == '[') {        /* IPv6 address reference */
    ++p;

    while (len && *q != ']') {
      ++q; --len;
    }

    if (!len || *q != ']' || p == q) {
      res = -3;
      goto error;
    }

    COAP_SET_STR(&uri->host, q - p, p);
    ++q; --len;
  } else {                        /* IPv4 address or FQDN */
    while (len && *q != ':' && *q != '/' && *q != '?') {
      ++q;
      --len;
    }

    if (p == q) {
      res = -3;
      goto error;
    }

    COAP_SET_STR(&uri->host, q - p, p);
  }

  /* check for Uri-Port */
  if (len && *q == ':') {
    p = ++q;
    --len;

    while (len && isdigit(*q)) {
      ++q;
      --len;
    }

    if (p < q) {                /* explicit port number given */
      int uri_port = 0;

      while (p < q)
              uri_port = uri_port * 10 + (*p++ - '0');

      /* check if port number is in allowed range */
      if (uri_port > 65535) {
              res = -4;
              goto error;
      }

      uri->port = (uint16_t)uri_port;
    }
  }

 path:                 /* at this point, p must point to an absolute path */

  if (!len)
    goto end;

  if (*q == '/') {
    p = ++q;
    --len;

    while (len && *q != '?') {
      ++q;
      --len;
    }

    if (p < q) {
      COAP_SET_STR(&uri->path, q - p, p);
      p = q;
    }
  }

  /* Uri_Query */
  if (len && *p == '?') {
    ++p;
    --len;
    COAP_SET_STR(&uri->query, len, p);
    len = 0;
  }

  end:
  return len ? -1 : 0;

  error:
  return res;
}

/**
 * Calculates decimal value from hexadecimal ASCII character given in
 * @p c. The caller must ensure that @p c actually represents a valid
 * heaxdecimal character, e.g. with isxdigit(3).
 *
 * @hideinitializer
 */
#define hexchar_to_dec(c) ((c) & 0x40 ? ((c) & 0x0F) + 9 : ((c) & 0x0F))

/**
 * Decodes percent-encoded characters while copying the string @p seg
 * of size @p length to @p buf. The caller of this function must
 * ensure that the percent-encodings are correct (i.e. the character
 * '%' is always followed by two hex digits. and that @p buf provides
 * sufficient space to hold the result. This function is supposed to
 * be called by make_decoded_option() only.
 *
 * @param seg     The segment to decode and copy.
 * @param length  Length of @p seg.
 * @param buf     The result buffer.
 */
static void
decode_segment(const uint8_t *seg, size_t length, unsigned char *buf) {

  while (length--) {

    if (*seg == '%') {
      *buf = (hexchar_to_dec(seg[1]) << 4) + hexchar_to_dec(seg[2]);

      seg += 2; length -= 2;
    } else {
      *buf = *seg;
    }

    ++buf; ++seg;
  }
}

/**
 * Runs through the given path (or query) segment and checks if
 * percent-encodings are correct. This function returns @c 0 on success
 * and @c -1 on error.
 */
static int
check_segment(const uint8_t *s, size_t length, size_t *segment_size) {
  size_t n = 0;

  while (length) {
    if (*s == '%') {
      if (length < 2 || !(isxdigit(s[1]) && isxdigit(s[2])))
              return -1;

      s += 2;
      length -= 2;
    }

    ++s; ++n; --length;
  }

  *segment_size = n;

  return 0;
}

/**
 * Writes a coap option from given string @p s to @p buf. @p s should
 * point to a (percent-encoded) path or query segment of a coap_uri_t
 * object.  The created option will have type @c 0, and the length
 * parameter will be set according to the size of the decoded string.
 * On success, this function returns @c 0 and sets @p optionsize to the option's
 * size. On error the function returns a value less than zero. This function
 * must be called from coap_split_path_impl() only.
 *
 * @param s           The string to decode.
 * @param length      The size of the percent-encoded string @p s.
 * @param buf         The buffer to store the new coap option.
 * @param buflen      The maximum size of @p buf.
 * @param optionsize  The option's size.
 *
 * @return @c 0 on success and @c -1 on error.
 *
 * @bug This function does not split segments that are bigger than 270
 * bytes.
 */
static int
make_decoded_option(const uint8_t *s, size_t length,
                    unsigned char *buf, size_t buflen, size_t* optionsize) {
  int res;
  size_t segmentlen;
  size_t written;

  if (!buflen) {
    coap_log(LOG_DEBUG, "make_decoded_option(): buflen is 0!\n");
    return -1;
  }

  res = check_segment(s, length, &segmentlen);
  if (res < 0)
    return -1;

  /* write option header using delta 0 and length res */
  written = coap_opt_setheader(buf, buflen, 0, segmentlen);

  assert(written <= buflen);

  if (!written)                        /* encoding error */
    return -1;

  buf += written;                /* advance past option type/length */
  buflen -= written;

  if (buflen < segmentlen) {
    coap_log(LOG_DEBUG, "buffer too small for option\n");
    return -1;
  }

  decode_segment(s, length, buf);

  *optionsize = written + segmentlen;

  return 0;
}


#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

typedef void (*segment_handler_t)(const uint8_t *, size_t, void *);

/**
 * Checks if path segment @p s consists of one or two dots.
 */
COAP_STATIC_INLINE int
dots(const uint8_t *s, size_t len) {
  return len && *s == '.' && (len == 1 || (len == 2 && *(s+1) == '.'));
}

/**
 * Splits the given string into segments. You should call one of the
 * macros coap_split_path() or coap_split_query() instead.
 *
 * @param s      The URI string to be tokenized.
 * @param length The length of @p s.
 * @param h      A handler that is called with every token.
 * @param data   Opaque data that is passed to @p h when called.
 *
 * @return The number of characters that have been parsed from @p s.
 */
static size_t
coap_split_path_impl(const uint8_t *s, size_t length,
                     segment_handler_t h, void *data) {

  const uint8_t *p, *q;

  p = q = s;
  while (length > 0 && !strnchr((const uint8_t *)"?#", 2, *q)) {
    if (*q == '/') {                /* start new segment */

      if (!dots(p, q - p)) {
        h(p, q - p, data);
      }

      p = q + 1;
    }

    q++;
    length--;
  }

  /* write last segment */
  if (!dots(p, q - p)) {
    h(p, q - p, data);
  }

  return q - s;
}

struct cnt_str {
  coap_string_t buf;
  int n;
};

static void
write_option(const uint8_t *s, size_t len, void *data) {
  struct cnt_str *state = (struct cnt_str *)data;
  int res;
  size_t optionsize;
  assert(state);

  res = make_decoded_option(s, len, state->buf.s, state->buf.length, &optionsize);
  if (res == 0) {
    state->buf.s += optionsize;
    state->buf.length -= optionsize;
    state->n++;
  }
}

int
coap_split_path(const uint8_t *s, size_t length,
                unsigned char *buf, size_t *buflen) {
  struct cnt_str tmp = { { *buflen, buf }, 0 };

  coap_split_path_impl(s, length, write_option, &tmp);

  *buflen = *buflen - tmp.buf.length;

  return tmp.n;
}

int
coap_split_query(const uint8_t *s, size_t length,
                unsigned char *buf, size_t *buflen) {
  struct cnt_str tmp = { { *buflen, buf }, 0 };
  const uint8_t *p;

  p = s;
  while (length > 0 && *s != '#') {
    if (*s == '&') {                /* start new query element */
      write_option(p, s - p, &tmp);
      p = s + 1;
    }

    s++;
    length--;
  }

  /* write last query element */
  write_option(p, s - p, &tmp);

  *buflen = *buflen - tmp.buf.length;
  return tmp.n;
}

#define URI_DATA(uriobj) ((unsigned char *)(uriobj) + sizeof(coap_uri_t))

coap_uri_t *
coap_new_uri(const uint8_t *uri, unsigned int length) {
  unsigned char *result;

  result = (unsigned char*)coap_malloc(length + 1 + sizeof(coap_uri_t));

  if (!result)
    return NULL;

  memcpy(URI_DATA(result), uri, length);
  URI_DATA(result)[length] = '\0'; /* make it zero-terminated */

  if (coap_split_uri(URI_DATA(result), length, (coap_uri_t *)result) < 0) {
    coap_free(result);
    return NULL;
  }
  return (coap_uri_t *)result;
}

coap_uri_t *
coap_clone_uri(const coap_uri_t *uri) {
  coap_uri_t *result;
  uint8_t *p;

  if ( !uri )
    return  NULL;

  result = (coap_uri_t *)coap_malloc( uri->query.length + uri->host.length +
                                      uri->path.length + sizeof(coap_uri_t) + 1);

  if ( !result )
    return NULL;

  memset( result, 0, sizeof(coap_uri_t) );

  result->port = uri->port;

  if ( uri->host.length ) {
    result->host.s = p = URI_DATA(result);
    result->host.length = uri->host.length;

    memcpy(p, uri->host.s, uri->host.length);
  }

  if ( uri->path.length ) {
    result->path.s = p = URI_DATA(result) + uri->host.length;
    result->path.length = uri->path.length;

    memcpy(p, uri->path.s, uri->path.length);
  }

  if ( uri->query.length ) {
    result->query.s = p = URI_DATA(result) + uri->host.length + uri->path.length;
    result->query.length = uri->query.length;

    memcpy (p, uri->query.s, uri->query.length);
  }

  return result;
}

COAP_STATIC_INLINE int
is_unescaped_in_path(const uint8_t c) {
  return ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' )
      || ( c >= '0' && c <= '9' ) || c == '-' || c == '.' || c == '_'
      || c == '~' || c == '!' || c == '$' || c == '\'' || c == '('
      || c == ')' || c == '*' || c == '+' || c == ',' || c == ';' || c=='='
      || c==':' || c=='@' || c == '&';
}

COAP_STATIC_INLINE int
is_unescaped_in_query(const uint8_t c) {
  return is_unescaped_in_path(c) || c=='/' || c=='?';
}

coap_string_t *coap_get_query(const coap_pdu_t *request) {
  coap_opt_iterator_t opt_iter;
  coap_opt_filter_t f;
  coap_opt_t *q;
  coap_string_t *query = NULL;
  size_t length = 0;
  static const uint8_t hex[] = "0123456789ABCDEF";

  coap_option_filter_clear(f);
  coap_option_filter_set(f, COAP_OPTION_URI_QUERY);
  coap_option_iterator_init(request, &opt_iter, f);
  while ((q = coap_option_next(&opt_iter))) {
    uint16_t seg_len = coap_opt_length(q), i;
    const uint8_t *seg= coap_opt_value(q);
    for (i = 0; i < seg_len; i++) {
      if (is_unescaped_in_query(seg[i]))
        length += 1;
      else
        length += 3;
    }
    length += 1;
  }
  if (length > 0)
    length -= 1;
  if (length > 0) {
    query = coap_new_string(length);
    if (query) {
      query->length = length;
      unsigned char *s = query->s;
      coap_option_iterator_init(request, &opt_iter, f);
      while ((q = coap_option_next(&opt_iter))) {
        if (s != query->s)
          *s++ = '&';
        uint16_t seg_len = coap_opt_length(q), i;
        const uint8_t *seg= coap_opt_value(q);
        for (i = 0; i < seg_len; i++) {
          if (is_unescaped_in_query(seg[i])) {
            *s++ = seg[i];
          } else {
            *s++ = '%';
            *s++ = hex[seg[i]>>4];
            *s++ = hex[seg[i]&0x0F];
          }
        }
      }
    }
  }
  return query;
}

coap_string_t *coap_get_uri_path(const coap_pdu_t *request) {
  coap_opt_iterator_t opt_iter;
  coap_opt_filter_t f;
  coap_opt_t *q;
  coap_string_t *uri_path = NULL;
  size_t length = 0;
  static const uint8_t hex[] = "0123456789ABCDEF";

  coap_option_filter_clear(f);
  coap_option_filter_set(f, COAP_OPTION_URI_PATH);
  coap_option_iterator_init(request, &opt_iter, f);
  while ((q = coap_option_next(&opt_iter))) {
    uint16_t seg_len = coap_opt_length(q), i;
    const uint8_t *seg= coap_opt_value(q);
    for (i = 0; i < seg_len; i++) {
      if (is_unescaped_in_path(seg[i]))
        length += 1;
      else
        length += 3;
    }
    /* bump for the leading "/" */
    length += 1;
  }
  /* The first entry does not have a leading "/" */
  if (length > 0)
    length -= 1;

  /* if 0, either no URI_PATH Option, or the first one was empty */
  uri_path = coap_new_string(length);
  if (uri_path) {
    uri_path->length = length;
    unsigned char *s = uri_path->s;
    int n = 0;
    coap_option_iterator_init(request, &opt_iter, f);
    while ((q = coap_option_next(&opt_iter))) {
      if (n++) {
        *s++ = '/';
      }
      uint16_t seg_len = coap_opt_length(q), i;
      const uint8_t *seg= coap_opt_value(q);
      for (i = 0; i < seg_len; i++) {
        if (is_unescaped_in_path(seg[i])) {
          *s++ = seg[i];
        } else {
          *s++ = '%';
          *s++ = hex[seg[i]>>4];
          *s++ = hex[seg[i]&0x0F];
        }
      }
    }
  }
  return uri_path;
}

