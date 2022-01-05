/* libcoap unit tests
 *
 * Copyright (C) 2013--2015 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap_config.h"
#include "test_wellknown.h"

#include <coap.h>

#include <assert.h>
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_PDU_SIZE 120
#define TEST_URI_LEN    4

coap_context_t *ctx;           /* Holds the coap context for most tests */
coap_pdu_t *pdu;           /* Holds the parsed PDU for most tests */
coap_session_t *session;   /* Holds a reference-counted session object
                            * that is passed to coap_wellknown_response(). */

static void
t_wellknown1(void) {
  coap_print_status_t result;
  coap_resource_t *r;
  unsigned char buf[40];
  size_t buflen, offset, ofs;

  char teststr[] = {  /* </>;title="some attribute";ct=0 (31 chars) */
    '<', '/', '>', ';', 't', 'i', 't', 'l',
    'e', '=', '"', 's', 'o', 'm', 'e', ' ',
    'a', 't', 't', 'r', 'i', 'b', 'u', 't',
    'e', '"', ';', 'c', 't', '=', '0'
  };

  r = coap_resource_init(NULL, 0);

  coap_add_attr(r, coap_make_str_const("ct"), coap_make_str_const("0"), 0);
  coap_add_attr(r, coap_make_str_const("title"), coap_make_str_const("\"some attribute\""), 0);

  coap_add_resource(ctx, r);

  for (offset = 0; offset < sizeof(teststr); offset++) {
    ofs = offset;
    buflen = sizeof(buf);

    result = coap_print_link(r, buf, &buflen, &ofs);

    CU_ASSERT(result == sizeof(teststr) - offset);
    CU_ASSERT(buflen == sizeof(teststr));
    CU_ASSERT(memcmp(buf, teststr + offset, sizeof(teststr) - offset) == 0);
  }

  /* offset points behind teststr */
  ofs = offset;
  buflen = sizeof(buf);
  result = coap_print_link(r, buf, &buflen, &ofs);

  CU_ASSERT(result == 0);
    CU_ASSERT(buflen == sizeof(teststr));

  /* offset exceeds buffer */
  buflen = sizeof(buf);
  ofs = buflen;
  result = coap_print_link(r, buf, &buflen, &ofs);

  CU_ASSERT(result == 0);
  CU_ASSERT(buflen == sizeof(teststr));
}

static void
t_wellknown2(void) {
  coap_print_status_t result;
  coap_resource_t *r;
  unsigned char buf[10];        /* smaller than teststr */
  size_t buflen, offset, ofs;

  char teststr[] = {  /* ,</abcd>;if="one";obs (21 chars) */
    '<', '/', 'a', 'b', 'c', 'd', '>', ';',
    'i', 'f', '=', '"', 'o', 'n', 'e', '"',
    ';', 'o', 'b', 's'
  };

  r = coap_resource_init(coap_make_str_const("abcd"), 0);
  r->observable = 1;
  coap_add_attr(r, coap_make_str_const("if"), coap_make_str_const("\"one\""), 0);

  coap_add_resource(ctx, r);

  for (offset = 0; offset < sizeof(teststr) - sizeof(buf); offset++) {
    ofs = offset;
    buflen = sizeof(buf);

    result = coap_print_link(r, buf, &buflen, &ofs);

    CU_ASSERT(result == (COAP_PRINT_STATUS_TRUNC | sizeof(buf)));
    CU_ASSERT(buflen == sizeof(teststr));
    CU_ASSERT(ofs == 0);
    CU_ASSERT(memcmp(buf, teststr + offset, sizeof(buf)) == 0);
  }

  /* from here on, the resource description fits into buf */
  for (; offset < sizeof(teststr); offset++) {
    ofs = offset;
    buflen = sizeof(buf);
    result = coap_print_link(r, buf, &buflen, &ofs);

    CU_ASSERT(result == sizeof(teststr) - offset);
    CU_ASSERT(buflen == sizeof(teststr));
    CU_ASSERT(ofs == 0);
    CU_ASSERT(memcmp(buf, teststr + offset,
                     COAP_PRINT_OUTPUT_LENGTH(result)) == 0);
  }

  /* offset exceeds buffer */
  buflen = sizeof(buf);
  ofs = offset;
  result = coap_print_link(r, buf, &buflen, &ofs);
  CU_ASSERT(result == 0);
  CU_ASSERT(buflen == sizeof(teststr));
  CU_ASSERT(ofs == offset - sizeof(teststr));
}

static void
t_wellknown3(void) {
  coap_print_status_t result;
  int j;
  coap_resource_t *r;
  static char uris[2 * 1024];
  unsigned char *uribuf = (unsigned char *)uris;
  unsigned char buf[40];
  size_t buflen = sizeof(buf);
  size_t offset;
  const uint16_t num_resources = (sizeof(uris) / TEST_URI_LEN) - 1;

  /* ,</0000> (TEST_URI_LEN + 4 chars) */
  for (j = 0; j < num_resources; j++) {
    int len = snprintf((char *)uribuf, TEST_URI_LEN + 1,
                       "%0*d", TEST_URI_LEN, j);
    coap_str_const_t uri_path = {.length = len, .s = uribuf};
    r = coap_resource_init(&uri_path, 0);
    coap_add_resource(ctx, r);
    uribuf += TEST_URI_LEN;
  }

  /* the following test assumes that the first two resources from
   * t_wellknown1() and t_wellknown2() need more than buflen
   * characters. Otherwise, CU_ASSERT(result > 0) will fail.
   */
  offset = num_resources * (TEST_URI_LEN + 4);
  result = coap_print_wellknown(ctx, buf, &buflen, offset, NULL);
  CU_ASSERT((result & COAP_PRINT_STATUS_ERROR) == 0 );
  CU_ASSERT(COAP_PRINT_OUTPUT_LENGTH(result) > 0);
}

/* Create wellknown response for request without Block-option. */
static void
t_wellknown4(void) {
  coap_pdu_t *response;
  coap_block_t block;

  response = coap_wellknown_response(ctx, session, pdu);

  CU_ASSERT_PTR_NOT_NULL(response);

  CU_ASSERT(coap_get_block(response, COAP_OPTION_BLOCK2, &block) != 0);

  CU_ASSERT(block.num == 0);
  CU_ASSERT(block.m == 1);
  CU_ASSERT(1 << (block.szx + 4)
    == response->token + response->used_size - response->data);

  coap_delete_pdu(response);
}

/* Create wellknown response for request with Block2-option and an szx
 * value smaller than COAP_MAX_BLOCK_SZX.
 */
static void
t_wellknown5(void) {
  coap_pdu_t *response;
  coap_block_t inblock = { .num = 1, .m = 0, .szx = 1 };
  coap_block_t block;
  unsigned char buf[3];

  if (!coap_add_option(pdu, COAP_OPTION_BLOCK2,
                       coap_encode_var_safe(buf, sizeof(buf),
                                            ((inblock.num << 4) |
                                             (inblock.m << 3) |
                                             inblock.szx)), buf)) {
    CU_FAIL("cannot add Block2 option");
    return;
  }

  response = coap_wellknown_response(ctx, session, pdu);

  CU_ASSERT_PTR_NOT_NULL(response);

  CU_ASSERT(coap_get_block(response, COAP_OPTION_BLOCK2, &block) != 0);

  CU_ASSERT(block.num == inblock.num);
  CU_ASSERT(block.m == 1);
  CU_ASSERT(1 << (block.szx + 4)
    == response->token + response->used_size - response->data);

  coap_delete_pdu(response);
}

static void
t_wellknown6(void) {
  coap_pdu_t *response;
  coap_block_t block = { .num = 0, .szx = 6 };
  unsigned char buf[TEST_PDU_SIZE];


  do {
    coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

    pdu->type = COAP_MESSAGE_NON;
    pdu->code = COAP_REQUEST_GET;
    pdu->tid = 0x1234;

    CU_ASSERT_PTR_NOT_NULL(pdu);

    if (!pdu || !coap_add_option(pdu, COAP_OPTION_BLOCK2,
                                 coap_encode_var_safe(buf, sizeof(buf),
                                       ((block.num << 4) | block.szx)), buf)) {
      CU_FAIL("cannot create request");
      return;
    }

    response = coap_wellknown_response(ctx, session, pdu);

    CU_ASSERT_PTR_NOT_NULL(response);

    /* coap_show_pdu(LOG_INFO, response); */

    CU_ASSERT(coap_get_block(response, COAP_OPTION_BLOCK2, &block) != 0);

    block.num++;
    coap_delete_pdu(response);
  } while (block.m == 1);
}

static int
t_wkc_tests_create(void) {
  coap_address_t addr;

  coap_address_init(&addr);

  addr.size = sizeof(struct sockaddr_in6);
  addr.addr.sin6.sin6_family = AF_INET6;
  addr.addr.sin6.sin6_addr = in6addr_any;
  addr.addr.sin6.sin6_port = htons(COAP_DEFAULT_PORT);

  ctx = coap_new_context(&addr);

  addr.addr.sin6.sin6_addr = in6addr_loopback;
  session = coap_new_client_session(ctx, NULL, &addr, COAP_PROTO_UDP);

  pdu = coap_pdu_init(0, 0, 0, TEST_PDU_SIZE);
#if 0
  /* add resources to coap context */
  if (ctx && pdu) {
    coap_resource_t *r;
    static char _buf[2 * 1024];
    unsigned char *buf = (unsigned char *)_buf;
    int i;

    /* </>;title="some attribute";ct=0 (31 chars) */
    r = coap_resource_init(NULL, 0, 0);

    coap_add_attr(r, coap_make_str_const("ct"), coap_make_str_const("0"), 0);
    coap_add_attr(r, coap_make_str_const("title"), coap_make_str_const("\"some attribute\""), 0);
    coap_add_resource(ctx, r);

    /* ,</abcd>;if="one";obs (21 chars) */
    r = coap_resource_init((const uint8_t *)"abcd", 4, 0);
    r->observable = 1;
    coap_add_attr(r, coap_make_str_const("if"), coap_make_str_const("\"one\""), 0);

    coap_add_resource(ctx, r);

    /* ,</0000> (TEST_URI_LEN + 4 chars) */
    for (i = 0; i < sizeof(_buf) / (TEST_URI_LEN + 4); i++) {
      int len = snprintf((char *)buf, TEST_URI_LEN + 1,
                         "%0*d", TEST_URI_LEN, i);
      r = coap_resource_init(buf, len, 0);
      coap_add_resource(ctx, r);
      buf += TEST_URI_LEN + 1;
    }

  }
#endif
  return ctx == NULL || pdu == NULL;
}

static int
t_wkc_tests_remove(void) {
  coap_delete_pdu(pdu);
  coap_free_context(ctx);
  return 0;
}

CU_pSuite
t_init_wellknown_tests(void) {
  CU_pSuite suite;

  suite = CU_add_suite(".well-known/core", t_wkc_tests_create, t_wkc_tests_remove);
  if (!suite) {                        /* signal error */
    fprintf(stderr, "W: cannot add .well-known/core test suite (%s)\n",
            CU_get_error_msg());

    return NULL;
  }

#define WKC_TEST(s,t)                                             \
  if (!CU_ADD_TEST(s,t)) {                                        \
    fprintf(stderr, "W: cannot add .well-known/core test (%s)\n", \
            CU_get_error_msg());                                  \
  }

  WKC_TEST(suite, t_wellknown1);
  WKC_TEST(suite, t_wellknown2);
  WKC_TEST(suite, t_wellknown3);
  WKC_TEST(suite, t_wellknown4);
  WKC_TEST(suite, t_wellknown5);
  WKC_TEST(suite, t_wellknown6);

  return suite;
}

