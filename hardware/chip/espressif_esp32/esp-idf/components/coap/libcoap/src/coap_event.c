/*
 * coap_event.c -- libcoap Event API
 *
 * Copyright (C) 2016 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

#include "coap_event.h"
#include "net.h"

/*
 * This replaces coap_set_event_handler() so that handler registration is
 * consistent in the naming.
 */
void
coap_register_event_handler(struct coap_context_t *context,
                            coap_event_handler_t hnd) {
  context->handle_event = hnd;
}

void
coap_set_event_handler(struct coap_context_t *context,
                       coap_event_handler_t hnd) {
  context->handle_event = hnd;
}

void
coap_clear_event_handler(struct coap_context_t *context) {
  context->handle_event = NULL;
}
