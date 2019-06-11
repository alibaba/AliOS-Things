/* str.c -- strings to be used in the CoAP library
 *
 * Copyright (C) 2010,2011 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap_config.h"

#include <stdio.h>

#include "libcoap.h"
#include "coap_debug.h"
#include "coap_mem.h"
#include "str.h"

coap_string_t *coap_new_string(size_t size) {
  coap_string_t *s =
            (coap_string_t *)coap_malloc_type(COAP_STRING, sizeof(coap_string_t) + size + 1);
  if ( !s ) {
#ifndef NDEBUG
    coap_log(LOG_CRIT, "coap_new_string: malloc\n");
#endif
    return NULL;
  }

  memset(s, 0, sizeof(coap_string_t));
  s->s = ((unsigned char *)s) + sizeof(coap_string_t);
  s->s[size] = '\000';
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

