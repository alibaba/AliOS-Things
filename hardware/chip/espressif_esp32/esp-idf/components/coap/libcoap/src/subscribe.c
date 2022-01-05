/* subscribe.c -- subscription handling for CoAP
 *                see draft-ietf-coap-observe-16
 *
 * Copyright (C) 2010--2013,2015 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap_config.h"
#include "coap.h"

#if defined(HAVE_ASSERT_H) && !defined(assert)
# include <assert.h>
#endif

#include "subscribe.h"

void
coap_subscription_init(coap_subscription_t *s) {
  assert(s);
  memset(s, 0, sizeof(coap_subscription_t));
}
