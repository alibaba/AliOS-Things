/* encode.c -- encoding and decoding of CoAP data types
 *
 * Copyright (C) 2010,2011 Olaf Bergmann <bergmann@tzi.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap3/coap_internal.h"

/* Carsten suggested this when fls() is not available: */
#ifndef HAVE_FLS
int coap_fls(unsigned int i) {
  return coap_flsll(i);
}
#endif

#ifndef HAVE_FLSLL
int coap_flsll(long long i)
{
  int n;
  for (n = 0; i; n++)
    i >>= 1;
  return n;
}
#endif

unsigned int
coap_decode_var_bytes(const uint8_t *buf, size_t len) {
  unsigned int i, n = 0;
  for (i = 0; i < len; ++i)
    n = (n << 8) + buf[i];

  return n;
}

unsigned int
coap_encode_var_safe(uint8_t *buf, size_t length, unsigned int val) {
  unsigned int n, i;

  for (n = 0, i = val; i && n < sizeof(val); ++n)
    i >>= 8;

  if (n > length) {
    assert (n <= length);
    return 0;
  }
  i = n;
  while (i--) {
    buf[i] = val & 0xff;
    val >>= 8;
  }

  return n;
}

uint64_t
coap_decode_var_bytes8(const uint8_t *buf, size_t len) {
  unsigned int i;
  uint64_t n = 0;
  for (i = 0; i < len; ++i)
    n = (n << 8) + buf[i];

  return n;
}

unsigned int
coap_encode_var_safe8(uint8_t *buf, size_t length, uint64_t val) {
  unsigned int n, i;
  uint64_t tval = val;

  for (n = 0; tval && n < sizeof(val); ++n)
    tval >>= 8;

  if (n > length) {
    assert (n <= length);
    return 0;
  }
  i = n;
  while (i--) {
    buf[i] = val & 0xff;
    val >>= 8;
  }

  return n;
}
