/* str.c -- strings to be used in the CoAP library
 *
 * Copyright (C) 2010,2011 Olaf Bergmann <bergmann@tzi.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap3/coap_internal.h"

#include <stdio.h>

coap_string_t *coap_new_string(size_t size) {
  coap_string_t *s;
#ifdef WITH_LWIP
  if (size >= MEMP_LEN_COAPSTRING) {
    coap_log(LOG_CRIT,
             "coap_new_string: size too large (%zu +1 > MEMP_LEN_COAPSTRING)\n",
             size);
    return NULL;
  }
#endif /* WITH_LWIP */
  assert(size+1 != 0);
  s = (coap_string_t *)coap_malloc_type(COAP_STRING,
                                        sizeof(coap_string_t) + size + 1);
  if ( !s ) {
    coap_log(LOG_CRIT, "coap_new_string: malloc: failed\n");
    return NULL;
  }

  memset(s, 0, sizeof(coap_string_t));
  s->s = ((unsigned char *)s) + sizeof(coap_string_t);
  s->s[size] = '\000';
  s->length = size;
  return s;
}

void coap_delete_string(coap_string_t *s) {
  coap_free_type(COAP_STRING, s);
}

coap_str_const_t *coap_new_str_const(const uint8_t *data, size_t size) {
  coap_string_t *s = coap_new_string(size);
  if (!s)
    return NULL;
  memcpy (s->s, data, size);
  s->length = size;
  return (coap_str_const_t *)s;
}

void coap_delete_str_const(coap_str_const_t *s) {
  coap_free_type(COAP_STRING, s);
}

coap_str_const_t *coap_make_str_const(const char *string)
{
  static int ofs = 0;
  static coap_str_const_t var[COAP_MAX_STR_CONST_FUNC];
  if (++ofs == COAP_MAX_STR_CONST_FUNC) ofs = 0;
  var[ofs].length = strlen(string);
  var[ofs].s = (const uint8_t *)string;
  return &var[ofs];
}

coap_binary_t *coap_new_binary(size_t size) {
  return (coap_binary_t *)coap_new_string(size);
}

coap_binary_t *coap_resize_binary(coap_binary_t *s, size_t size) {
#if defined(RIOT_VERSION) || defined(WITH_CONTIKI) || defined(WITH_LWIP)
  /* Unlikely to work as strings will not be large enough */
  coap_binary_t *new = coap_new_binary(size);
  if (new) {
    memcpy(new->s, s->s, s->length);
    coap_delete_binary(s);
  }
#else /* ! RIOT_VERSION && ! WITH_CONTIKI && ! WITH_LWIP */
  coap_binary_t *new = coap_realloc_type(COAP_STRING,
                                         s,
                                         sizeof(coap_binary_t) + size);
#endif /* ! RIOT_VERSION && ! WITH_CONTIKI && ! WITH_LWIP */
  if (new) {
    new->length = size;
    new->s = ((unsigned char *)new) + sizeof(coap_string_t);
  }
  return new;
}

void coap_delete_binary(coap_binary_t *s) {
  coap_free_type(COAP_STRING, s);
}

coap_bin_const_t *coap_new_bin_const(const uint8_t *data, size_t size) {
  coap_string_t *s = coap_new_string(size);
  if (!s)
    return NULL;
  memcpy (s->s, data, size);
  s->length = size;
  return (coap_bin_const_t *)s;
}

void coap_delete_bin_const(coap_bin_const_t *s) {
  coap_free_type(COAP_STRING, s);
}

