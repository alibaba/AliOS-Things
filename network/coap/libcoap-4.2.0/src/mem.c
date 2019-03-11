/* mem.c -- CoAP memory handling
 *
 * Copyright (C) 2014--2015 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */


#include "coap_config.h"
#include "libcoap.h"
#include "mem.h"
#include "coap_debug.h"

#ifdef HAVE_ASSERT_H
#include <assert.h>
#else /* HAVE_ASSERT_H */
#define assert(...)
#endif /* HAVE_ASSERT_H */

#ifdef HAVE_MALLOC
#include <stdlib.h>

void
coap_memory_init(void) {
}

#ifdef __GNUC__
#define UNUSED_PARAM __attribute__((unused))
#else
#define UNUSED_PARAM
#endif /* __GNUC__ */

void *
coap_malloc_type(coap_memory_tag_t type, size_t size) {
  (void)type;
  return malloc(size);
}

void
coap_free_type(coap_memory_tag_t type, void *p) {
  (void)type;
  free(p);
}

#else /* HAVE_MALLOC */

#ifdef WITH_CONTIKI

/**
 * The maximum size of a string on platforms that allocate fixed-size
 * memory blocks.
 */
#ifndef COAP_MAX_STRING_SIZE
#define COAP_MAX_STRING_SIZE 64
#endif /* COAP_MAX_STRING_SIZE */

/**
 * The maximum number of a strings on platforms that allocate
 * fixed-size memory blocks.
 */
#ifndef COAP_MAX_STRINGS
#define COAP_MAX_STRINGS      10
#endif /* COAP_MAX_STRINGS */

struct coap_stringbuf_t {
  char data[COAP_MAX_STRING_SIZE];
};

#include "coap_config.h"
#include "net.h"
#include "pdu.h"
#include "coap_io.h"
#include "resource.h"
#include "coap_session.h"

#define COAP_MAX_PACKET_SIZE (sizeof(coap_packet_t) + COAP_RXBUFFER_SIZE)
#ifndef COAP_MAX_PACKETS
#define COAP_MAX_PACKETS     2
#endif /* COAP_MAX_PACKETS */

typedef union {
  coap_pdu_t packet; /* try to convince the compiler to word-align this structure  */
  char buf[COAP_MAX_PACKET_SIZE];
} coap_packetbuf_t;

MEMB(string_storage, struct coap_stringbuf_t, COAP_MAX_STRINGS);
MEMB(packet_storage, coap_packetbuf_t, COAP_MAX_PACKETS);
MEMB(session_storage, coap_session_t, COAP_MAX_SESSIONS);
MEMB(node_storage, coap_queue_t, COAP_PDU_MAXCNT);
MEMB(pdu_storage, coap_pdu_t, COAP_PDU_MAXCNT);
MEMB(pdu_buf_storage, coap_packetbuf_t, COAP_PDU_MAXCNT);
MEMB(resource_storage, coap_resource_t, COAP_MAX_RESOURCES);
MEMB(attribute_storage, coap_attr_t, COAP_MAX_ATTRIBUTES);

static struct memb *
get_container(coap_memory_tag_t type) {
  switch(type) {
  case COAP_PACKET: return &packet_storage;
  case COAP_NODE:   return &node_storage;
  case COAP_SESSION: return &session_storage;
  case COAP_PDU:     return &pdu_storage;
  case COAP_PDU_BUF: return &pdu_buf_storage;
  case COAP_RESOURCE: return &resource_storage;
  case COAP_RESOURCEATTR: return &attribute_storage;
  default:
    return &string_storage;
  }
}

void
coap_memory_init(void) {
  memb_init(&string_storage);
  memb_init(&packet_storage);
  memb_init(&node_storage);
  memb_init(&session_storage);
  memb_init(&pdu_storage);
  memb_init(&pdu_buf_storage);
  memb_init(&resource_storage);
  memb_init(&attribute_storage);
}

void *
coap_malloc_type(coap_memory_tag_t type, size_t size) {
  struct memb *container =  get_container(type);
  void *ptr;

  assert(container);

  if (size > container->size) {
    coap_log(LOG_WARNING,
             "coap_malloc_type: Requested memory exceeds maximum object "
             "size (type %d, size %d, max %d)\n",
             type, (int)size, container->size);
    return NULL;
  }

  ptr = memb_alloc(container);
  if (!ptr)
    coap_log(LOG_WARNING,
             "coap_malloc_type: Failure (no free blocks) for type %d\n",
             type);
  return ptr;
}

void
coap_free_type(coap_memory_tag_t type, void *object) {
  memb_free(get_container(type), object);
}
#endif /* WITH_CONTIKI */

#endif /* HAVE_MALLOC */
