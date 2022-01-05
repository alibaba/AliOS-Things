/* coap_hashkey.c -- definition of hash key type and helper functions
 *
 * Copyright (C) 2010,2011 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap_hashkey.h"

void
coap_hash_impl(const unsigned char *s, unsigned int len, coap_key_t h) {
  size_t j;

  while (len--) {
    j = sizeof(coap_key_t)-1;

    while (j) {
      h[j] = ((h[j] << 7) | (h[j-1] >> 1)) + h[j];
      --j;
    }

    h[0] = (h[0] << 7) + h[0] + *s++;
  }
}

