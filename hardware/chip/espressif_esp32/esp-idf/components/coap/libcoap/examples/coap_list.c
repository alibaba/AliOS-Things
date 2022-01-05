/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 * -*- */

/* coap_list.c -- CoAP list structures
 *
 * Copyright (C) 2010,2011,2015 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see README for terms of
 * use.
 */

/*
 * examples/coap_list.[ch] are DEPRECATED.  You should be using
 * struct coap_optlist_t instead with the following functions which are a part
 * of libcoap.
 *
 * coap_new_optlist()
 * coap_insert_optlist()
 * coap_delete_optlist()
 * coap_add_optlist_pdu()
 *
 * See 'man coap_pdu_setup' for further information.
 *
 * examples/coap_list.[ch] files will be removed in a future release
 * They are left here to support building backward compatability of old versions
 * of coap-client
 */

/* #include "coap_config.h" */

#include <stdio.h>
#include <string.h>

#include <coap2/libcoap.h>
#include <coap2/debug.h>
#include <coap2/mem.h>
#include "coap_list.h"


int
coap_insert(coap_list_t **head, coap_list_t *node) {
  if (!node) {
    coap_log(LOG_WARNING, "cannot create option Proxy-Uri\n");
  } else {
    /* must append at the list end to avoid re-ordering of
     * options during sort */
    LL_APPEND((*head), node);
  }

  return node != NULL;
}

int
coap_delete(coap_list_t *node) {
  if (node) {
    coap_free(node);
  }
  return 1;
}

void
coap_delete_list(coap_list_t *queue) {
  coap_list_t *elt, *tmp;

  if (!queue)
    return;

  LL_FOREACH_SAFE(queue, elt, tmp) {
    coap_delete(elt);
  }
}

