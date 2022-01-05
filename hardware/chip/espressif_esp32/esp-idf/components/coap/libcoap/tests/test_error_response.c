/* libcoap unit tests
 *
 * Copyright (C) 2013,2015 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap_config.h"
#include "test_error_response.h"

#include <coap.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

coap_pdu_t *pdu;              /* Holds the request PDU for most tests */
coap_opt_filter_t opts;              /* option filter used for generating responses */

/************************************************************************
 ** PDU decoder
 ************************************************************************/

/* FIXME: handle COAP_ERROR_PHRASE_LENGTH == 0 */

static void
t_error_response1(void) {
  uint8_t teststr[] = {
    0x60, 0x80, 0x12, 0x34, 0xff, 'B', 'a', 'd',
    ' ', 'R', 'e', 'q', 'u', 'e', 's', 't'
  };
  coap_pdu_t *response;

  coap_pdu_clear(pdu, pdu->max_size);
  pdu->type = COAP_MESSAGE_CON;
  pdu->tid = 0x1234;

  /* result = coap_add_token(pdu, 5, (unsigned char *)"token"); */
  coap_option_filter_clear(opts);
  response = coap_new_error_response(pdu, COAP_RESPONSE_CODE(400), opts);

  CU_ASSERT_PTR_NOT_NULL(response);

  CU_ASSERT(response->used_size == sizeof(teststr) - 4);
  CU_ASSERT(response->type == COAP_MESSAGE_ACK);
  CU_ASSERT(response->token_length == 0);
  CU_ASSERT(response->code == 0x80);
  CU_ASSERT(response->tid == 0x1234);
  CU_ASSERT(coap_pdu_encode_header(response, COAP_PROTO_UDP) == 4);
  CU_ASSERT(memcmp(response->token - 4, teststr, sizeof(teststr)) == 0);
  coap_delete_pdu(response);
}

static void
t_error_response2(void) {
  uint8_t teststr[] = {
    0x55, 0x84, 0x12, 0x34, 't', 'o', 'k', 'e',
    'n', 0xff, 'N', 'o', 't', ' ', 'F', 'o',
    'u', 'n', 'd'
  };
  coap_pdu_t *response;

  coap_pdu_clear(pdu, pdu->max_size);
  pdu->type = COAP_MESSAGE_NON;
  pdu->tid = 0x1234;
  coap_add_token(pdu, 5, (const uint8_t *)"token");
  coap_add_option(pdu, COAP_OPTION_URI_HOST, 4, (const uint8_t *)"time");

  coap_option_filter_clear(opts);
  response = coap_new_error_response(pdu, COAP_RESPONSE_CODE(404), opts);

  CU_ASSERT_PTR_NOT_NULL(response);

  CU_ASSERT(response->used_size == sizeof(teststr) - 4);
  CU_ASSERT(response->type == COAP_MESSAGE_NON);
  CU_ASSERT(response->token_length == 5);
  CU_ASSERT(response->code == 0x84);
  CU_ASSERT(coap_pdu_encode_header(response, COAP_PROTO_UDP) == 4);
  CU_ASSERT(memcmp(response->token - 4, teststr, sizeof(teststr)) == 0);
  coap_delete_pdu(response);
}

static void
t_error_response3(void) {
  const uint8_t code = COAP_RESPONSE_CODE(402);
  uint8_t teststr[] = {
    0x65, code, 0x00, 0x00, 't', 'o', 'k', 'e',
    'n', 0x90, 0xff, 'B', 'a', 'd', ' ', 'O',
    'p', 't', 'i', 'o', 'n'
  };
  coap_pdu_t *response;

  coap_pdu_clear(pdu, pdu->max_size);
  pdu->type = COAP_MESSAGE_CON;
  coap_add_token(pdu, 5, (const uint8_t *)"token");
  /* coap_add_option(pdu, COAP_OPTION_URI_HOST, 4, (const uint8_t *)"time"); */

  /* unknown critical option 9 */
  coap_add_option(pdu, 9, 0, NULL);

  coap_option_filter_clear(opts);
  coap_option_setb(opts, 9);
  response = coap_new_error_response(pdu, code, opts);

  CU_ASSERT_PTR_NOT_NULL(response);

  CU_ASSERT(response->used_size == sizeof(teststr) - 4);
  CU_ASSERT(response->type == COAP_MESSAGE_ACK);
  CU_ASSERT(response->token_length == 5);
  CU_ASSERT(response->code == code);
  CU_ASSERT(coap_pdu_encode_header(response, COAP_PROTO_UDP) == 4);
  CU_ASSERT(memcmp(response->token - 4, teststr, sizeof(teststr)) == 0);
  coap_delete_pdu(response);
}

static void
t_error_response4(void) {
  const uint8_t code = COAP_RESPONSE_CODE(402);
  unsigned char optval[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b
  };
  uint8_t teststr[] = {
    0x65, code, 0x00, 0x00,  't',  'o',  'k',  'e',
     'n', 0x9c, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
    0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0xff,  'B',
     'a',  'd',  ' ',  'O',  'p',  't',  'i',  'o',
     'n'
  };
  coap_pdu_t *response;

  coap_pdu_clear(pdu, pdu->max_size);
  pdu->type = COAP_MESSAGE_CON;
  coap_add_token(pdu, 5, (const uint8_t *)"token");
  /* coap_add_option(pdu, COAP_OPTION_URI_HOST, 4, (const uint8_t *)"time"); */

  /* unknown critical option 9 */
  coap_add_option(pdu, 9, sizeof(optval), optval);

  coap_option_filter_clear(opts);
  coap_option_setb(opts, 9);
  response = coap_new_error_response(pdu, code, opts);

  CU_ASSERT_PTR_NOT_NULL(response);

  CU_ASSERT(response->used_size == sizeof(teststr) - 4);
  CU_ASSERT(response->type == COAP_MESSAGE_ACK);
  CU_ASSERT(response->token_length == 5);
  CU_ASSERT(response->code == code);
  CU_ASSERT(coap_pdu_encode_header(response, COAP_PROTO_UDP) == 4);
  CU_ASSERT(memcmp(response->token - 4, teststr, sizeof(teststr)) == 0);
  coap_delete_pdu(response);
}

static void
t_error_response5(void) {
  const uint8_t code = COAP_RESPONSE_CODE(402);
  unsigned char optval[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12
  };
  uint8_t teststr[] = {
    0x65, code, 0x00, 0x00,  't',  'o',  'k',  'e',
     'n', 0x9d, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
    0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0xff,  'B',
     'a',  'd',  ' ',  'O',  'p',  't',  'i',  'o',
     'n'
  };
  coap_pdu_t *response;

  coap_pdu_clear(pdu, pdu->max_size);
  pdu->type = COAP_MESSAGE_CON;
  coap_add_token(pdu, 5, (const uint8_t *)"token");
  /* coap_add_option(pdu, COAP_OPTION_URI_HOST, 4, (const uint8_t *)"time"); */

  /* unknown critical option 9 */
  coap_add_option(pdu, 9, sizeof(optval), optval);

  coap_option_filter_clear(opts);
  coap_option_setb(opts, 9);
  response = coap_new_error_response(pdu, code, opts);

  CU_ASSERT_PTR_NOT_NULL(response);

  CU_ASSERT(response->used_size == sizeof(teststr) - 4);
  CU_ASSERT(response->type == COAP_MESSAGE_ACK);
  CU_ASSERT(response->token_length == 5);
  CU_ASSERT(response->code == code);
  CU_ASSERT(coap_pdu_encode_header(response, COAP_PROTO_UDP) == 4);
  CU_ASSERT(memcmp(response->token - 4, teststr, sizeof(teststr)) == 0);
  coap_delete_pdu(response);
}

static void
t_error_response6(void) {
  const uint8_t code = COAP_RESPONSE_CODE(402);
  unsigned char optval[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12
  };
  uint8_t teststr[] = {
    0x65, code, 0x00, 0x00,  't',  'o',  'k',  'e',
     'n', 0xdd, 0x0a, 0x06, 0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0xff,
     'B',  'a',  'd',  ' ',  'O',  'p',  't',  'i',
     'o',  'n'
  };
  coap_pdu_t *response;

  coap_pdu_clear(pdu, pdu->max_size);
  pdu->type = COAP_MESSAGE_CON;
  coap_add_token(pdu, 5, (const uint8_t *)"token");
  /* coap_add_option(pdu, COAP_OPTION_URI_HOST, 4, (const uint8_t *)"time"); */

  /* unknown critical option 23 */
  coap_add_option(pdu, 23, sizeof(optval), optval);

  coap_option_filter_clear(opts);
  coap_option_setb(opts, 23);
  response = coap_new_error_response(pdu, code, opts);

  CU_ASSERT_PTR_NOT_NULL(response);

  CU_ASSERT(response->used_size == sizeof(teststr) - 4);
  CU_ASSERT(response->type == COAP_MESSAGE_ACK);
  CU_ASSERT(response->token_length == 5);
  CU_ASSERT(response->code == code);
  CU_ASSERT(coap_pdu_encode_header(response, COAP_PROTO_UDP) == 4);
  CU_ASSERT(memcmp(response->token - 4, teststr, sizeof(teststr)) == 0);
  coap_delete_pdu(response);
}

static void
t_error_response7(void) {
  const uint8_t code = COAP_RESPONSE_CODE(402);
  unsigned char optval[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12
  };
  uint8_t teststr[] = {
    0x65, code, 0x00, 0x00,  't',  'o',  'k',  'e',
     'n', 0xdd, 0x0a, 0x06, 0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0xff,
     'B',  'a',  'd',  ' ',  'O',  'p',  't',  'i',
     'o',  'n'
  };
  coap_pdu_t *response;

  coap_pdu_clear(pdu, pdu->max_size);
  pdu->type = COAP_MESSAGE_CON;
  coap_add_token(pdu, 5, (const uint8_t *)"token");
  /* known option 11 */
  coap_add_option(pdu, 11, 4, (const uint8_t *)"time");

  /* unknown critical option 23 */
  coap_add_option(pdu, 23, sizeof(optval), optval);

  coap_option_filter_clear(opts);
  coap_option_setb(opts, 23);
  response = coap_new_error_response(pdu, code, opts);

  CU_ASSERT_PTR_NOT_NULL(response);

  CU_ASSERT(response->used_size == sizeof(teststr) - 4);
  CU_ASSERT(response->type == COAP_MESSAGE_ACK);
  CU_ASSERT(response->token_length == 5);
  CU_ASSERT(response->code == code);
  CU_ASSERT(coap_pdu_encode_header(response, COAP_PROTO_UDP) == 4);
  CU_ASSERT(memcmp(response->token - 4, teststr, sizeof(teststr)) == 0);
  coap_delete_pdu(response);
}

static void
t_error_response8(void) {
  const uint8_t code = COAP_RESPONSE_CODE(503);
  uint8_t teststr[] = {
    0x65, code, 0x00, 0x00,  't',  'o',  'k',  'e',
     'n', 0xe0, 0x02, 0xdc, 0xd0, 0x00, 0xff,  'S',
     'e',  'r',  'v',  'i',  'c',  'e',  ' ',  'U',
     'n',  'a',  'v',  'a',  'i',  'l',  'a',  'b',
     'l',  'e'
  };
  coap_pdu_t *response;

  coap_pdu_clear(pdu, pdu->max_size);
  pdu->type = COAP_MESSAGE_CON;
  coap_add_token(pdu, 5, (const uint8_t *)"token");
  /* known option 1000 */
  coap_add_option(pdu, 1000, 0, NULL);

  /* unknown options 1001 and 1014 */
  coap_add_option(pdu, 1001, 0, NULL);
  coap_add_option(pdu, 1014, 0, NULL);

  /* known option 2000 */
  coap_add_option(pdu, 2000, 0, NULL);

  coap_option_filter_clear(opts);
  coap_option_setb(opts, 1001);
  coap_option_setb(opts, 1014);
  response = coap_new_error_response(pdu, code, opts);

  CU_ASSERT_PTR_NOT_NULL(response);

  CU_ASSERT(response->used_size == sizeof(teststr) - 4);
  CU_ASSERT(response->type == COAP_MESSAGE_ACK);
  CU_ASSERT(response->token_length == 5);
  CU_ASSERT(response->code == code);
  CU_ASSERT(coap_pdu_encode_header(response, COAP_PROTO_UDP) == 4);
  CU_ASSERT(memcmp(response->token - 4, teststr, sizeof(teststr)) == 0);
  coap_delete_pdu(response);
}

static int
t_error_response_tests_create(void) {
  pdu = coap_pdu_init(0, 0, 0, COAP_DEFAULT_MTU);

  return pdu == NULL;
}

static int
t_error_response_tests_remove(void) {
  coap_delete_pdu(pdu);
  return 0;
}

CU_pSuite
t_init_error_response_tests(void) {
  CU_pSuite suite[1];

  suite[0] = CU_add_suite("error response generator",
                          t_error_response_tests_create,
                          t_error_response_tests_remove);
  if (!suite[0]) {                        /* signal error */
    fprintf(stderr, "W: cannot add error response generator test suite (%s)\n",
            CU_get_error_msg());

    return NULL;
  }

#define ERROR_RESPONSE_TEST(s,t)                                        \
  if (!CU_ADD_TEST(s,t)) {                                                \
    fprintf(stderr, "W: cannot add error response generator test (%s)\n", \
            CU_get_error_msg());                                        \
  }

  ERROR_RESPONSE_TEST(suite[0], t_error_response1);
  ERROR_RESPONSE_TEST(suite[0], t_error_response2);
  ERROR_RESPONSE_TEST(suite[0], t_error_response3);
  ERROR_RESPONSE_TEST(suite[0], t_error_response4);
  ERROR_RESPONSE_TEST(suite[0], t_error_response5);
  ERROR_RESPONSE_TEST(suite[0], t_error_response6);
  ERROR_RESPONSE_TEST(suite[0], t_error_response7);
  ERROR_RESPONSE_TEST(suite[0], t_error_response8);

  return suite[0];
}

