/* mem.c -- CoAP memory handling
 *
 * Copyright (C) 2014--2015,2019--2020 Olaf Bergmann <bergmann@tzi.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */


#include "coap3/coap_internal.h"

#if defined(RIOT_VERSION) && defined(MODULE_MEMARRAY)
#include <memarray.h>

#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#include <session.h>
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION

#include "coap_session.h"
#include "net.h"
#include "pdu.h"
#include "resource.h"

/**
 * The maximum size of a string on platforms that allocate fixed-size
 * memory blocks.
 */
#ifndef COAP_MAX_STRING_SIZE
#define COAP_MAX_STRING_SIZE     (64U)
#endif /* COAP_MAX_STRING_SIZE */

/**
 * The maximum number of strings on platforms that allocate
 * fixed-size memory blocks.
 */
#ifndef COAP_MAX_STRINGS
#define COAP_MAX_STRINGS         (16U)
#endif /* COAP_MAX_STRINGS */

/**
 * The maximum number of endpoints on platforms that allocate
 * fixed-size memory blocks.
 */
#ifndef COAP_MAX_ENDPOINTS
#define COAP_MAX_ENDPOINTS          (4U)
#endif /* COAP_MAX_ENDPOINTS */

/**
 * The maximum number of resources on platforms that allocate
 * fixed-size memory blocks.
 */
#ifndef COAP_MAX_RESOURCES
#define COAP_MAX_RESOURCES          (8U)
#endif /* COAP_MAX_RESOURCES */

/**
 * The maximum number of attributes on platforms that allocate
 * fixed-size memory blocks.  Default is #COAP_MAX_RESOURCES * 4.
 */
#ifndef COAP_MAX_ATTRIBUTES
#define COAP_MAX_ATTRIBUTES             \
  ((COAP_MAX_RESOURCES) * 4U)
#endif /* COAP_MAX_ATTRIBUTE_STRINGS */

/**
 * The maximum number of a strings that are used for attribute names
 * and values on platforms that allocate fixed-size memory blocks.
 * Default is #COAP_MAX_ATTRIBUTES, i.e. every attribute can have a
 * dynamic value.
 */
#ifndef COAP_MAX_ATTRIBUTE_STRINGS
#define COAP_MAX_ATTRIBUTE_STRINGS    (COAP_MAX_ATTRIBUTES)
#endif /* COAP_MAX_ATTRIBUTE_STRINGS */

/**
 * The maximum size of attribute names or values and values on
 * platforms that allocate fixed-size memory blocks.
 */
#ifndef COAP_MAX_ATTRIBUTE_SIZE
#define COAP_MAX_ATTRIBUTE_SIZE       (16U)
#endif /* COAP_MAX_ATTRIBUTE_SIZE */

/**
 * The maximum number of processed packets on platforms that allocate
 * fixed-size memory blocks.
 */
#ifndef COAP_MAX_PACKETS
#define COAP_MAX_PACKETS            (4U)
#endif /* COAP_MAX_PACKETS */

/**
 * The maximum number of nodes in retransmission queue on platforms
 * that allocate fixed-size memory blocks. The default value is
 * #COAP_MAX_ENDPOINTS * #COAP_MAX_PACKETS.
 */
#ifndef COAP_MAX_NODES
#define COAP_MAX_NODES               \
  ((COAP_MAX_ENDPOINTS) * (COAP_MAX_PACKETS))
#endif /* COAP_MAX_NODES */

/**
 * The maximum number of CoAP contexts on platforms that allocate
 * fixed-size memory blocks. Default is 1.
 */
#ifndef COAP_MAX_CONTEXTS
#define COAP_MAX_CONTEXTS           (1U)
#endif /* COAP_MAX_CONTEXTS */

/**
 * The maximum number of CoAP PDUs processed in parallel on platforms
 * that allocate fixed-size memory blocks. Default is
 * #COAP_MAX_ENDPOINTS * 4.
 */
#ifndef COAP_MAX_PDUS
#define COAP_MAX_PDUS               ((COAP_MAX_ENDPOINTS) * 4U)
#endif /* COAP_MAX_PDUS */

/**
 * The maximum number of DTLS sessions on platforms that allocate
 * fixed-size memory blocks.
 */
#ifndef COAP_MAX_DTLS_SESSIONS
#define COAP_MAX_DTLS_SESSIONS      (2U)
#endif /* COAP_MAX_CONTEXTS */

/**
 * The maximum number of DTLS sessions on platforms that allocate
 * fixed-size memory blocks. Default is #COAP_MAX_ENDPOINTS.
 */
#ifndef COAP_MAX_SESSIONS
#define COAP_MAX_SESSIONS           (COAP_MAX_ENDPOINTS)
#endif /* COAP_MAX_CONTEXTS */

/**
 * The maximum number of optlist entries on platforms that allocate
 * fixed-size memory blocks.
 */
#ifndef COAP_MAX_OPTIONS
#define COAP_MAX_OPTIONS            (16U)
#endif /* COAP_MAX_CONTEXTS */

/**
 * The maximum size of option values on platforms that allocate
 * fixed-size memory blocks.
 */
#ifndef COAP_MAX_OPTION_SIZE
#define COAP_MAX_OPTION_SIZE        (16U)
#endif /* COAP_MAX_OPTION_SIZE */

/**
 * The maximum number of cache-key entries that allocate
 * fixed-size memory blocks.
 */
#ifndef COAP_MAX_CACHE_KEYS
#define COAP_MAX_CACHE_KEYS        (2U)
#endif /* COAP_MAX_CACHE_KEYS */

/**
 * The maximum number of cache-entry entries that allocate
 * fixed-size memory blocks.
 */
#ifndef COAP_MAX_CACHE_ENTRIES
#define COAP_MAX_CACHE_ENTRIES        (2U)
#endif /* COAP_MAX_CACHE_ENTRIES */

/* The memstr is the storage for holding coap_string_t structure
 * together with its contents. */
union memstr_t {
  coap_string_t s;
  char buf[sizeof(coap_string_t) + COAP_MAX_STRING_SIZE];
};

/* The attrstr is the storage for holding coap_string_t structures to
 * serve as attribute names or values. As these are typically short,
 * they are stored in a different arena than generic strings. */
union attrstr_t {
  coap_string_t s;
  char buf[sizeof(coap_string_t) + COAP_MAX_ATTRIBUTE_SIZE];
};

static union memstr_t string_storage_data[COAP_MAX_STRINGS];
static memarray_t string_storage;

static coap_endpoint_t endpoint_storage_data[COAP_MAX_ENDPOINTS];
static memarray_t endpoint_storage;

static union attrstr_t attr_storage_data[COAP_MAX_ATTRIBUTE_STRINGS];
static memarray_t attr_storage;

static coap_attr_t resattr_storage_data[COAP_MAX_ATTRIBUTES];
static memarray_t resattr_storage;

static coap_packet_t pkt_storage_data[COAP_MAX_PACKETS];
static memarray_t pkt_storage;

static coap_queue_t node_storage_data[COAP_MAX_NODES];
static memarray_t node_storage;

static coap_context_t context_storage_data[COAP_MAX_CONTEXTS];
static memarray_t context_storage;

static coap_pdu_t pdu_storage_data[COAP_MAX_PDUS];
static memarray_t pdu_storage;

/* The pdubuf is the storage for holding the (assembled) PDU data in a
 * coap_pdu_t structure. */
union pdubuf_t {
  void *p; /* try to convince the compiler to word-align this structure  */
  char buf[COAP_DEFAULT_MAX_PDU_RX_SIZE];
};

static union pdubuf_t pdubuf_storage_data[COAP_MAX_PDUS];
static memarray_t pdubuf_storage;

static coap_resource_t resource_storage_data[COAP_MAX_RESOURCES];
static memarray_t resource_storage;

#ifdef HAVE_LIBTINYDTLS
static session_t dtls_storage_data[COAP_MAX_DTLS_SESSIONS];
static memarray_t dtls_storage;
#endif /* HAVE_LIBTINYDTLS */

static coap_session_t session_storage_data[COAP_MAX_SESSIONS];
static memarray_t session_storage;

/* The optbuf_t is the storage for holding optlist nodes. */
struct optbuf_t {
  coap_optlist_t optlist;
  char optbuf[COAP_MAX_OPTION_SIZE];
};
static struct optbuf_t option_storage_data[COAP_MAX_OPTIONS];
static memarray_t option_storage;

static coap_cache_key_t cache_key_storage_data[COAP_MAX_CACHE_KEYS];
static memarray_t cache_key_storage;

static coap_cache_entry_t cache_entry_storage_data[COAP_MAX_CACHE_ENTRIES];
static memarray_t cache_entry_storage;

#define INIT_STORAGE(Storage, Count)  \
  memarray_init(&(Storage ## _storage), (Storage ## _storage_data), sizeof(Storage ## _storage_data[0]), (Count));

#define STORAGE_PTR(Storage)  (&(Storage ## _storage))

void
coap_memory_init(void) {
  INIT_STORAGE(string, COAP_MAX_STRINGS);
  INIT_STORAGE(endpoint, COAP_MAX_ENDPOINTS);
  INIT_STORAGE(attr, COAP_MAX_ATTRIBUTE_STRINGS);
  INIT_STORAGE(pkt, COAP_MAX_PACKETS);
  INIT_STORAGE(node, COAP_MAX_NODES);
  INIT_STORAGE(context, COAP_MAX_CONTEXTS);
  INIT_STORAGE(pdu, COAP_MAX_PDUS);
  INIT_STORAGE(pdubuf, COAP_MAX_PDUS);
  INIT_STORAGE(resource, COAP_MAX_RESOURCES);
  INIT_STORAGE(resattr, COAP_MAX_ATTRIBUTES);
#ifdef HAVE_LIBTINYDTLS
  INIT_STORAGE(dtls, COAP_MAX_DTLS_SESSIONS);
#endif
  INIT_STORAGE(session, COAP_MAX_SESSIONS);
  INIT_STORAGE(option, COAP_MAX_OPTIONS);
  INIT_STORAGE(cache_key, COAP_MAX_CACHE_KEYS);
  INIT_STORAGE(cache_entry, COAP_MAX_CACHE_ENTRIES);
}

static memarray_t *
get_container(coap_memory_tag_t type) {
  switch(type) {
  case COAP_ATTRIBUTE_NAME:
    /* fall through */
  case COAP_ATTRIBUTE_VALUE: return &attr_storage;
  case COAP_PACKET:          return &pkt_storage;
  case COAP_NODE:            return &node_storage;
  case COAP_CONTEXT:         return STORAGE_PTR(context);
  case COAP_ENDPOINT:        return &endpoint_storage;
  case COAP_PDU:             return &pdu_storage;
  case COAP_PDU_BUF:         return &pdubuf_storage;
  case COAP_RESOURCE:        return &resource_storage;
  case COAP_RESOURCEATTR:    return &resattr_storage;
#ifdef HAVE_LIBTINYDTLS
  case COAP_DTLS_SESSION:    return &dtls_storage;
#endif
  case COAP_SESSION:         return &session_storage;
  case COAP_OPTLIST:         return &option_storage;
  case COAP_CACHE_KEY:       return &cache_key_storage;
  case COAP_CACHE_ENTRY:     return &cache_key_entry;
  case COAP_STRING:
    /* fall through */
  default:
    return &string_storage;
  }
}

void *
coap_malloc_type(coap_memory_tag_t type, size_t size) {
  memarray_t *container = get_container(type);
  void *ptr;
  assert(container);

  if (size > container->size) {
    coap_log(LOG_WARNING,
             "coap_malloc_type: Requested memory exceeds maximum object "
             "size (type %d, size %zu, max %d)\n",
             type, size, container->size);
    return NULL;
  }

  ptr = memarray_alloc(container);
  if (!ptr)
    coap_log(LOG_WARNING,
             "coap_malloc_type: Failure (no free blocks) for type %d\n",
             type);
  return ptr;
}

void
coap_free_type(coap_memory_tag_t type, void *object) {
  if (object != NULL)
    memarray_free(get_container(type), object);
}
#else /* ! RIOT_VERSION */

#ifdef HAVE_MALLOC
#include <stdlib.h>

void
coap_memory_init(void) {
}

void *
coap_malloc_type(coap_memory_tag_t type, size_t size) {
  (void)type;
  return malloc(size);
}

void *
coap_realloc_type(coap_memory_tag_t type, void* p, size_t size) {
  (void)type;
  return realloc(p, size);
}

void
coap_free_type(coap_memory_tag_t type, void *p) {
  (void)type;
  free(p);
}

#else /* ! HAVE_MALLOC */

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
MEMB(cache_key_storage, coap_cache_key_t, COAP_MAX_CACHE_KEYS);
MEMB(cache_entry_storage, coap_cache_entry_t, COAP_MAX_CACHE_ENTRIES);
MEMB(lg_xmit_storage, coap_lg_xmit_t, COAP_MAX_LG_XMIT);
MEMB(lg_crcv_storage, coap_lg_crcv_t, COAP_MAX_LG_CRCV);
MEMB(lg_srcv_storage, coap_lg_srcv_t, COAP_MAX_LG_SRCV);

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
  case COAP_CACHE_KEY:    return &cache_key_storage;
  case COAP_CACHE_ENTRY:  return &cache_entry_storage;
  case COAP_LG_XMIT: return &lg_xmit_storage;
  case COAP_LG_CRCV: return &lg_crcv_storage;
  case COAP_LG_SRCV: return &lg_srcv_storage;
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
  memb_init(&cache_key_storage);
  memb_init(&cache_entry_storage);
  memb_init(&lg_xmit_storage);
  memb_init(&lg_crcv_storage);
  memb_init(&lg_srcv_storage);
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

#endif /* ! HAVE_MALLOC */

#endif /* ! RIOT_VERSION */
