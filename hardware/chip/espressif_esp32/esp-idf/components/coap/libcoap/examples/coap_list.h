/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 * -*- */

/* coap_list.h -- CoAP list structures
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
 * They are left here to support building backward compatibility of old versions
 * of coap-client
 */

#ifndef COAP_LIST_H_
#define COAP_LIST_H_

#include <coap2/utlist.h>

typedef struct coap_list_t {
  struct coap_list_t *next;
  char data[];
} coap_list_t;

/**
 * Adds node to given queue, ordered by specified order function. Returns 1
 * when insert was successful, 0 otherwise.
 */
int coap_insert(coap_list_t **queue, coap_list_t *node);

/* destroys specified node */
int coap_delete(coap_list_t *node);

/* removes all items from given queue and frees the allocated storage */
void coap_delete_list(coap_list_t *queue);

#endif /* COAP_LIST_H_ */
