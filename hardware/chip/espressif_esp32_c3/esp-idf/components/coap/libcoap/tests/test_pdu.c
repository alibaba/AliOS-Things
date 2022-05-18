/* libcoap unit tests
 *
 * Copyright (C) 2012,2015 Olaf Bergmann <bergmann@tzi.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "test_common.h"
#include "test_pdu.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

coap_pdu_t *pdu;              /* Holds the parsed PDU for most tests */

/************************************************************************
 ** PDU decoder
 ************************************************************************/

static void
t_parse_pdu1(void) {
  uint8_t teststr[] = {  0x40, 0x01, 0x93, 0x34 };
  int result;

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), pdu);
  CU_ASSERT(result > 0);

  CU_ASSERT(pdu->used_size == sizeof(teststr) - 4);
  CU_ASSERT(pdu->type == COAP_MESSAGE_CON);
  CU_ASSERT(pdu->token_length == 0);
  CU_ASSERT(pdu->code == COAP_REQUEST_CODE_GET);
  CU_ASSERT(pdu->mid == 0x9334);
  CU_ASSERT_PTR_NULL(pdu->data);
}

static void
t_parse_pdu2(void) {
  uint8_t teststr[] = {  0x55, 0x69, 0x12, 0x34, 't', 'o', 'k', 'e', 'n' };
  int result;

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), pdu);
  CU_ASSERT(result > 0);

  CU_ASSERT(pdu->used_size == sizeof(teststr) - 4);
  CU_ASSERT(pdu->type == COAP_MESSAGE_NON);
  CU_ASSERT(pdu->token_length == 5);
  CU_ASSERT(pdu->code == 0x69);
  CU_ASSERT(pdu->mid == 0x1234);
  CU_ASSERT(memcmp(pdu->token, teststr + 4, 5) == 0);
  CU_ASSERT_PTR_NULL(pdu->data);
}

static void
t_parse_pdu3(void) {
  uint8_t teststr[] = {  0x53, 0x69, 0x12, 0x34, 't', 'o', 'k', 'e', 'n' };
  int result;

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), pdu);
  CU_ASSERT(result == 0);
}

static void
t_parse_pdu4(void) {
  /* illegal token length */
  uint8_t teststr[] = {  0x59, 0x69, 0x12, 0x34,
                      't', 'o', 'k', 'e', 'n', '1', '2', '3', '4' };
  int result;

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), pdu);
  CU_ASSERT(result == 0);

  teststr[0] = 0x5f;

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), pdu);
  CU_ASSERT(result == 0);
}

static void
t_parse_pdu5(void) {
  /* PDU with options */
  uint8_t teststr[] = {  0x55, 0x73, 0x12, 0x34, 't', 'o', 'k', 'e',
                      'n',  0x00, 0xc1, 0x00
  };
  int result;

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), pdu);
  CU_ASSERT(result > 0);

  CU_ASSERT(pdu->used_size == sizeof(teststr) - 4);
  CU_ASSERT(pdu->type == COAP_MESSAGE_NON);
  CU_ASSERT(pdu->token_length == 5);
  CU_ASSERT(pdu->code == 0x73);
  CU_ASSERT(pdu->mid == 0x1234);
  CU_ASSERT(memcmp(pdu->token, teststr + 4, 5) == 0);
  CU_ASSERT_PTR_NULL(pdu->data);

  /* FIXME: check options */
}

static void
t_parse_pdu6(void) {
  /* PDU with options that exceed the PDU */
  uint8_t teststr[] = {  0x55, 0x73, 0x12, 0x34, 't', 'o', 'k', 'e',
                      'n',  0x00, 0xc1, 0x00, 0xae, 0xf0, 0x03
  };
  int result;

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), pdu);
  CU_ASSERT(result == 0);
}

static void
t_parse_pdu7(void) {
  /* PDU with options and payload */
  uint8_t teststr[] = {  0x55, 0x73, 0x12, 0x34, 't', 'o', 'k', 'e',
                      'n',  0x00, 0xc1, 0x00, 0xff, 'p', 'a', 'y',
                      'l', 'o', 'a', 'd'
  };
  int result;

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), pdu);
  CU_ASSERT(result > 0);

  CU_ASSERT(pdu->used_size == sizeof(teststr) - 4);
  CU_ASSERT(pdu->type == COAP_MESSAGE_NON);
  CU_ASSERT(pdu->token_length == 5);
  CU_ASSERT(pdu->code == 0x73);
  CU_ASSERT(pdu->mid == 0x1234);
  CU_ASSERT(memcmp(pdu->token, teststr + 4, 5) == 0);

  /* FIXME: check options */

  CU_ASSERT(pdu->data == pdu->token + 9);
  CU_ASSERT(memcmp(pdu->data, teststr + 13, 7) == 0);
}

static void
t_parse_pdu8(void) {
  /* PDU without options but with payload */
  uint8_t teststr[] = {  0x50, 0x73, 0x12, 0x34,
                      0xff, 'p', 'a', 'y', 'l', 'o', 'a',
                      'd'
  };
  int result;

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), pdu);
  CU_ASSERT(result > 0);

  CU_ASSERT(pdu->used_size == sizeof(teststr) - 4);
  CU_ASSERT(pdu->type == COAP_MESSAGE_NON);
  CU_ASSERT(pdu->token_length == 0);
  CU_ASSERT(pdu->code == 0x73);
  CU_ASSERT(pdu->mid == 0x1234);

  /* FIXME: check options */

  CU_ASSERT(pdu->data == pdu->token + 1);
  CU_ASSERT(memcmp(pdu->data, teststr + 5, 7) == 0);
}

static void
t_parse_pdu9(void) {
  /* PDU without options and payload but with payload start marker */
  uint8_t teststr[] = {  0x70, 0x00, 0x12, 0x34, 0xff };
  int result;

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), pdu);
  CU_ASSERT(result == 0);
}

static void
t_parse_pdu10(void) {
  /* PDU without payload but with options and payload start marker */
  uint8_t teststr[] = {  0x53, 0x73, 0x12, 0x34, 't', 'o', 'k',
                      0x31, 'a', 0xc1, 0x00, 0xff
  };
  int result;

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), pdu);
  CU_ASSERT(result == 0);
}

static void
t_parse_pdu11(void) {
  uint8_t teststr[] = {  0x60, 0x00, 0x12, 0x34 };
  int result;

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), pdu);
  CU_ASSERT(result > 0);

  CU_ASSERT(pdu->used_size == sizeof(teststr) - 4);
  CU_ASSERT(pdu->type == COAP_MESSAGE_ACK);
  CU_ASSERT(pdu->token_length == 0);
  CU_ASSERT(pdu->code == 0);
  CU_ASSERT(pdu->mid == 0x1234);
}

static void
t_parse_pdu12(void) {
  /* RST */
  uint8_t teststr[] = {  0x70, 0x00, 0x12, 0x34 };
  int result;

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), pdu);
  CU_ASSERT(result > 0);

  CU_ASSERT(pdu->used_size == sizeof(teststr) - 4);
  CU_ASSERT(pdu->type == COAP_MESSAGE_RST);
  CU_ASSERT(pdu->token_length == 0);
  CU_ASSERT(pdu->code == 0);
  CU_ASSERT(pdu->mid == 0x1234);
}

static void
t_parse_pdu13(void) {
  /* RST with content */
  uint8_t teststr[] = {  0x70, 0x00, 0x12, 0x34,
                      0xff, 'c', 'o', 'n', 't', 'e', 'n', 't'
  };
  int result;

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), pdu);
  CU_ASSERT(result == 0);
}

static void
t_parse_pdu14(void) {
  /* ACK with content */
  uint8_t teststr[] = {  0x60, 0x00, 0x12, 0x34,
                      0xff, 'c', 'o', 'n', 't', 'e', 'n', 't'
  };
  int result;

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), pdu);
  CU_ASSERT(result == 0);
}

/*
 * To test Issue #199 which reads one byte past the end of teststr[]
 * before fix to coap_opt_parse() as delta is two byte value and only
 * one byte left
 * Credit to OSS-Fuzz for finding this, work done by Bhargava Shastry
 */
static void
t_parse_pdu15(void) {
  int result;
  uint8_t teststr[] = {
  64,  91,  91,  91, 139,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  91,  91, 224, 224, 224,
 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224,
 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224,   1,   0,   0,   0,
   0,   0,   0,   0, 224, 192, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224,
 224, 224, 224, 224, 224, 224, 224, 224, 228, 224, 224, 224, 224, 224, 224, 224,
 224, 224, 224, 224, 224, 224, 224, 224, 224,  91,  91,  91,  91,  91,  91,  91,
  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,
  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,
  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,
  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,
  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,
  91,  91,  91,  91,  91,  91,  91, 224, 224, 224, 224, 224, 224, 224, 224, 224,
 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224,
 224, 224, 224, 224, 224, 224,   1,   0,   0,   0,   0,   0,   0,   0, 224, 224,
 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224,
 224, 224, 224, 224, 224};

  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  CU_ASSERT(pdu->data == NULL);

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), pdu);
  CU_ASSERT(result == 0);
}

static void log_handler(coap_log_t level, const char *message)
{
  (void)level;
  (void)message;
}

/*
 * To test Issue #214 which allows the token size to be set larger than the
 * decoded PDU in coap_pdu_parse_header().  This then causes coap_show_pdu()
 * to access invalid memory.
 * Credit to OSS-Fuzz for finding this, work done by Bhargava Shastry
 */
static void
t_parse_pdu16(void) {
  int result;
  coap_pdu_t *testpdu;
  uint8_t teststr[] = { 0x5a, 0x0a, 0x5b, 0x5b };

  testpdu = coap_pdu_init(0, 0, 0, sizeof(teststr));
  CU_ASSERT(testpdu != NULL);

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), testpdu);
  CU_ASSERT(result == 0);

  coap_set_show_pdu_output(0);
  coap_set_log_handler(log_handler);
  coap_show_pdu(LOG_ERR, testpdu);        /* display PDU */
  coap_set_log_handler(NULL);

  coap_delete_pdu(testpdu);
}

static void
t_parse_pdu17(void) {
  uint8_t teststr[512] = {  0x40, 0x01, 0x93, 0x34 };
  size_t idx;
  int result;

  /* 245 * option delta 268 > 65535, causing a overflow in the option
   * number */
  for (idx = 4; idx < sizeof(teststr) - 4; idx += 2) {
    teststr[idx] = 0xd0;     /* 1 byte option delta follows */
    teststr[idx + 1] = 0xff; /* option delta 268 */
  }

  result = coap_pdu_parse(COAP_PROTO_UDP, teststr, sizeof(teststr), pdu);
  CU_ASSERT(result == 0);
}

/************************************************************************
 ** PDU encoder
 ************************************************************************/

static void
t_encode_pdu1(void) {
  uint8_t teststr[] = { 0x45, 0x01, 0x12, 0x34, 't', 'o', 'k', 'e', 'n' };
  int result;

  coap_pdu_clear(pdu, pdu->max_size);
  pdu->type = COAP_MESSAGE_CON;
  pdu->code = COAP_REQUEST_CODE_GET;
  pdu->mid = 0x1234;

  result = coap_add_token(pdu, 5, (const uint8_t *)"token");

  CU_ASSERT(result == 1);
  CU_ASSERT(pdu->used_size == 5);
  CU_ASSERT_PTR_NULL(pdu->data);
  CU_ASSERT(coap_pdu_encode_header(pdu, COAP_PROTO_UDP) == 4);
  CU_ASSERT(memcmp(pdu->token - 4, teststr, sizeof(teststr)) == 0);
}

static void
t_encode_pdu2(void) {
  size_t old_max = pdu->max_size;
  int result;

  coap_pdu_clear(pdu, 3);        /* set very small PDU size */

  pdu->type = COAP_MESSAGE_CON;
  pdu->code = COAP_REQUEST_CODE_GET;
  pdu->mid = 0x1234;

  result = coap_add_token(pdu, 5, (const uint8_t *)"token");

  CU_ASSERT(result == 0);

  coap_pdu_clear(pdu, old_max);        /* restore PDU size */
}

static void
t_encode_pdu3(void) {
  int result;

  result = coap_add_token(pdu, 9, (const uint8_t *)"123456789");

  CU_ASSERT(result == 0);
}

static void
t_encode_pdu4(void) {
  /* PDU with options */
  uint8_t teststr[] = { 0x60, 0x99, 0x12, 0x34, 0x3d, 0x05, 0x66, 0x61,
                     0x6e, 0x63, 0x79, 0x70, 0x72, 0x6f, 0x78, 0x79,
                     0x2e, 0x63, 0x6f, 0x61, 0x70, 0x2e, 0x6d, 0x65,
                     0x84, 0x70, 0x61, 0x74, 0x68, 0x00, 0xe8, 0x1e,
                     0x28, 0x66, 0x61, 0x6e, 0x63, 0x79, 0x6f, 0x70,
                     0x74
  };
  int result;

  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  pdu->type = COAP_MESSAGE_ACK;
  pdu->code = 0x99;
  pdu->mid = 0x1234;

  CU_ASSERT(pdu->used_size == 0);

  result = coap_add_option(pdu, COAP_OPTION_URI_HOST,
       18, (const uint8_t *)"fancyproxy.coap.me");

  CU_ASSERT(result == 20);
  CU_ASSERT(pdu->max_opt == 3);
  CU_ASSERT(pdu->used_size == 20);
  CU_ASSERT_PTR_NULL(pdu->data);

  result = coap_add_option(pdu, COAP_OPTION_URI_PATH,
                           4, (const uint8_t *)"path");

  CU_ASSERT(result == 5);
  CU_ASSERT(pdu->max_opt == 11);
  CU_ASSERT(pdu->used_size == 25);
  CU_ASSERT_PTR_NULL(pdu->data);

  result = coap_add_option(pdu, COAP_OPTION_URI_PATH, 0, NULL);

  CU_ASSERT(result == 1);
  CU_ASSERT(pdu->max_opt == 11);
  CU_ASSERT(pdu->used_size == 26);
  CU_ASSERT_PTR_NULL(pdu->data);

  result = coap_add_option(pdu, 8000, 8, (const uint8_t *)"fancyopt");

  CU_ASSERT(result == 11);
  CU_ASSERT(pdu->max_opt == 8000);
  CU_ASSERT(pdu->used_size == 37);
  CU_ASSERT_PTR_NULL(pdu->data);

  CU_ASSERT(coap_pdu_encode_header(pdu, COAP_PROTO_UDP) == 4);
  CU_ASSERT(memcmp(pdu->token - 4, teststr, sizeof(teststr)) == 0);
}

static void
t_encode_pdu5(void) {
  /* PDU with token and options */
  uint8_t teststr[] = { 0x68, 0x84, 0x12, 0x34, '1',  '2',  '3',  '4',
                     '5',  '6',  '7',  '8',  0x18, 0x41, 0x42, 0x43,
                     0x44, 0x45, 0x46, 0x47, 0x48, 0xd1, 0x03, 0x12
  };
  int result;

  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  pdu->type = COAP_MESSAGE_ACK;
  pdu->code = COAP_RESPONSE_CODE(404);
  pdu->mid = 0x1234;

  CU_ASSERT(pdu->used_size == 0);

  result = coap_add_token(pdu, 8, (const uint8_t *)"12345678");

  CU_ASSERT(pdu->used_size == 8);

  result = coap_add_option(pdu, COAP_OPTION_IF_MATCH,
                           8, (const uint8_t *)"ABCDEFGH");

  CU_ASSERT(result == 9);
  CU_ASSERT(pdu->max_opt == 1);
  CU_ASSERT(pdu->used_size == 17);
  CU_ASSERT_PTR_NULL(pdu->data);

  result = coap_add_option(pdu, COAP_OPTION_ACCEPT,
                           1, (const uint8_t *)"\x12");

  CU_ASSERT(result == 3);
  CU_ASSERT(pdu->max_opt == 17);
  CU_ASSERT(pdu->used_size == 20);
  CU_ASSERT_PTR_NULL(pdu->data);

  CU_ASSERT(coap_pdu_encode_header(pdu, COAP_PROTO_UDP) == 4);
  CU_ASSERT(memcmp(pdu->token - 4, teststr, sizeof(teststr)) == 0);
}

static void
t_encode_pdu6(void) {
  /* PDU with data */
  uint8_t teststr[] = { 0x50, 0x02, 0x12, 0x34, 0xff, '1',  '2',  '3',
                     '4', '5',  '6',  '7',  '8'
  };
  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  pdu->type = COAP_MESSAGE_NON;
  pdu->code = COAP_REQUEST_CODE_POST;
  pdu->mid = 0x1234;

  CU_ASSERT(pdu->used_size == 0);
  CU_ASSERT_PTR_NULL(pdu->data);

  coap_add_data(pdu, 8, (const uint8_t *)"12345678");

  CU_ASSERT(pdu->used_size == 9);
  CU_ASSERT(coap_pdu_encode_header(pdu, COAP_PROTO_UDP) == 4);
  CU_ASSERT(memcmp(pdu->token - 4, teststr, sizeof(teststr)) == 0);
}

static void
t_encode_pdu7(void) {
  /* PDU with empty data */
  uint8_t teststr[] = { 0x40, 0x43, 0x12, 0x34 };
  int result;
  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  pdu->type = COAP_MESSAGE_CON;
  pdu->code = COAP_RESPONSE_CODE(203);
  pdu->mid = 0x1234;

  CU_ASSERT(pdu->used_size == 0);

  result = coap_add_data(pdu, 0, NULL);

  CU_ASSERT(result > 0);
  CU_ASSERT(pdu->used_size == 0);
  CU_ASSERT_PTR_NULL(pdu->data);

  CU_ASSERT(coap_pdu_encode_header(pdu, COAP_PROTO_UDP) == 4);
  CU_ASSERT(memcmp(pdu->token - 4, teststr, sizeof(teststr)) == 0);
}

static void
t_encode_pdu8(void) {
  /* PDU with token and data */
  uint8_t teststr[] = { 0x42, 0x43, 0x12, 0x34, 0x00, 0x01, 0xff, 0x00 };
  int result;
  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  pdu->type = COAP_MESSAGE_CON;
  pdu->code = COAP_RESPONSE_CODE(203);
  pdu->mid = 0x1234;

  CU_ASSERT(pdu->used_size == 0);

  result = coap_add_token(pdu, 2, (const uint8_t *)"\x00\x01");

  CU_ASSERT(result > 0);

  result = coap_add_data(pdu, 1, (const uint8_t *)"\0");

  CU_ASSERT(result > 0);
  CU_ASSERT(pdu->used_size == 4);
  CU_ASSERT(pdu->data == pdu->token + 3);

  CU_ASSERT(coap_pdu_encode_header(pdu, COAP_PROTO_UDP) == 4);
  CU_ASSERT(memcmp(pdu->token - 4, teststr, sizeof(teststr)) == 0);
}

static void
t_encode_pdu9(void) {
  /* PDU with options and data */
  uint8_t teststr[] = { 0x60, 0x44, 0x12, 0x34, 0x48, 's',  'o',  'm',
                     'e',  'e',  't',  'a',  'g',  0x10, 0xdd, 0x11,
                     0x04, 's',  'o',  'm',  'e',  'r',  'a',  't',
                     'h',  'e',  'r',  'l',  'o',  'n',  'g',  'u',
                     'r',  'i',  0xff, 'd',  'a',  't',  'a'
  };
  int result;

  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  pdu->type = COAP_MESSAGE_ACK;
  pdu->code = COAP_RESPONSE_CODE(204);
  pdu->mid = 0x1234;

  CU_ASSERT(pdu->used_size == 0);

  result = coap_add_option(pdu, COAP_OPTION_ETAG, 8, (const uint8_t *)"someetag");

  CU_ASSERT(result == 9);
  CU_ASSERT(pdu->max_opt == 4);
  CU_ASSERT(pdu->used_size == 9);
  CU_ASSERT_PTR_NULL(pdu->data);

  result = coap_add_option(pdu, COAP_OPTION_IF_NONE_MATCH, 0, NULL);

  CU_ASSERT(result == 1);
  CU_ASSERT(pdu->max_opt == 5);
  CU_ASSERT(pdu->used_size == 10);
  CU_ASSERT_PTR_NULL(pdu->data);

  result = coap_add_option(pdu, COAP_OPTION_PROXY_URI,
                           17, (const uint8_t *)"someratherlonguri");

  CU_ASSERT(result == 20);
  CU_ASSERT(pdu->max_opt == 35);
  CU_ASSERT(pdu->used_size == 30);
  CU_ASSERT_PTR_NULL(pdu->data);

  result = coap_add_data(pdu, 4, (const uint8_t *)"data");

  CU_ASSERT(result > 0);
  CU_ASSERT(pdu->used_size == 35);
  CU_ASSERT(pdu->data == pdu->token + 31);

  CU_ASSERT(coap_pdu_encode_header(pdu, COAP_PROTO_UDP) == 4);
  CU_ASSERT(memcmp(pdu->token - 4, teststr, sizeof(teststr)) == 0);
}

static void
t_encode_pdu10(void) {
  /* PDU with token, options and data */
  uint8_t teststr[] = { 0x62, 0x44, 0x12, 0x34, 0x00, 0x00, 0x8d, 0xf2,
                     'c',  'o',  'a',  'p',  ':',  '/',  '/',  'e',
                     'x',  'a',  'm',  'p',  'l',  'e',  '.',  'c',
                     'o',  'm',  '/',  '1',  '2',  '3',  '4',  '5',
                     '/',  '%',  '3',  'F',  'x',  'y',  'z',  '/',
                     '3',  '0',  '4',  '8',  '2',  '3',  '4',  '2',
                     '3',  '4',  '/',  '2',  '3',  '4',  '0',  '2',
                     '3',  '4',  '8',  '2',  '3',  '4',  '/',  '2',
                     '3',  '9',  '0',  '8',  '4',  '2',  '3',  '4',
                     '-',  '2',  '3',  '/',  '%',  'A',  'B',  '%',
                     '3',  '0',  '%',  'a',  'f',  '/',  '+',  '1',
                     '2',  '3',  '/',  'h',  'f',  'k',  's',  'd',
                     'h',  '/',  '2',  '3',  '4',  '8',  '0',  '-',
                     '2',  '3',  '4',  '-',  '9',  '8',  '2',  '3',
                     '5',  '/',  '1',  '2',  '0',  '4',  '/',  '2',
                     '4',  '3',  '5',  '4',  '6',  '3',  '4',  '5',
                     '3',  '4',  '5',  '2',  '4',  '3',  '/',  '0',
                     '1',  '9',  '8',  's',  'd',  'n',  '3',  '-',
                     'a',  '-',  '3',  '/',  '/',  '/',  'a',  'f',
                     'f',  '0',  '9',  '3',  '4',  '/',  '9',  '7',
                     'u',  '2',  '1',  '4',  '1',  '/',  '0',  '0',
                     '0',  '2',  '/',  '3',  '9',  '3',  '2',  '4',
                     '2',  '3',  '5',  '3',  '2',  '/',  '5',  '6',
                     '2',  '3',  '4',  '0',  '2',  '3',  '/',  '-',
                     '-',  '-',  '-',  '/',  '=',  '1',  '2',  '3',
                     '4',  '=',  '/',  '0',  '9',  '8',  '1',  '4',
                     '1',  '-',  '9',  '5',  '6',  '4',  '6',  '4',
                     '3',  '/',  '2',  '1',  '9',  '7',  '0',  '-',
                     '-',  '-',  '-',  '-',  '/',  '8',  '2',  '3',
                     '6',  '4',  '9',  '2',  '3',  '4',  '7',  '2',
                     'w',  'e',  'r',  'e',  'r',  'e',  'w',  'r',
                     '0',  '-',  '9',  '2',  '1',  '-',  '3',  '9',
                     '1',  '2',  '3',  '-',  '3',  '4',  '/',  0x0d,
                     0x01, '/',  '/',  '4',  '9',  '2',  '4',  '0',
                     '3',  '-',  '-',  '0',  '9',  '8',  '/',  0xc1,
                     '*',  0xff, 'd',  'a',  't',  'a'
  };
  int result;

  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  pdu->type = COAP_MESSAGE_ACK;
  pdu->code = COAP_RESPONSE_CODE(204);
  pdu->mid = 0x1234;

  CU_ASSERT(pdu->used_size == 0);

  result = coap_add_token(pdu, 2, (const uint8_t *)"\0\0");

  CU_ASSERT(result > 0);
  result = coap_add_option(pdu, COAP_OPTION_LOCATION_PATH, 255,
                           (const uint8_t *)"coap://example.com/12345/%3Fxyz/3048234234/23402348234/239084234-23/%AB%30%af/+123/hfksdh/23480-234-98235/1204/243546345345243/0198sdn3-a-3///aff0934/97u2141/0002/3932423532/56234023/----/=1234=/098141-9564643/21970-----/82364923472wererewr0-921-39123-34/");

  CU_ASSERT(result == 257);
  CU_ASSERT(pdu->max_opt == 8);
  CU_ASSERT(pdu->used_size == 259);
  CU_ASSERT_PTR_NULL(pdu->data);

  result = coap_add_option(pdu, COAP_OPTION_LOCATION_PATH, 14,
                           (const uint8_t *)"//492403--098/");

  CU_ASSERT(result == 16);
  CU_ASSERT(pdu->max_opt == 8);
  CU_ASSERT(pdu->used_size == 275);
  CU_ASSERT_PTR_NULL(pdu->data);

  result = coap_add_option(pdu, COAP_OPTION_LOCATION_QUERY,
                           1, (const uint8_t *)"*");

  CU_ASSERT(result == 2);
  CU_ASSERT(pdu->max_opt == 20);
  CU_ASSERT(pdu->used_size == 277);
  CU_ASSERT_PTR_NULL(pdu->data);

  result = coap_add_data(pdu, 4, (const uint8_t *)"data");

  CU_ASSERT(result > 0);
  CU_ASSERT(pdu->used_size == 282);
  CU_ASSERT(pdu->data == pdu->token + 278);

  CU_ASSERT(coap_pdu_encode_header(pdu, COAP_PROTO_UDP) == 4);
  CU_ASSERT(memcmp(pdu->token - 4, teststr, sizeof(teststr)) == 0);
}

static void
t_encode_pdu11(void) {
  coap_log_t level = coap_get_log_level();
  /* data too long for PDU */
  size_t old_max = pdu->max_size;
  int result;

  coap_pdu_clear(pdu, 8);        /* clear PDU, with small maximum */

  CU_ASSERT(pdu->data == NULL);
  coap_set_log_level(LOG_CRIT);
  result = coap_add_data(pdu, 10, (const uint8_t *)"0123456789");
  coap_set_log_level(level);

  CU_ASSERT(result == 0);
  CU_ASSERT(pdu->data == NULL);
  CU_ASSERT(pdu->used_size == 0);

  pdu->max_size = old_max;
}

static void
t_encode_pdu12(void) {
  coap_optlist_t *optlist = NULL;
  int n;
  uint8_t opt_num[] = { 50, 51, 52, 53, 54, 55, 56, 57, 58, 59 };
  uint8_t opt_val[] = { 50, 51, 52, 53, 54, 55, 56, 57, 58, 59 };
  uint8_t opt_srt[] = { 50, 51, 52, 53, 54, 55, 56, 57, 58, 59 };
  coap_opt_iterator_t oi;
  coap_opt_t *option;

  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  CU_ASSERT(pdu->data == NULL);

  for (n = 0; n < (int)sizeof(opt_num); n++) {
    coap_insert_optlist(&optlist, coap_new_optlist(opt_num[n],
                     sizeof(opt_val[n]), &opt_val[n]));
  }
  coap_add_optlist_pdu(pdu, &optlist);

  /* Check options in pdu are in right order */
  coap_option_iterator_init(pdu, &oi, COAP_OPT_ALL);
  for (n = 0; n < (int)sizeof(opt_num); n++) {
    option = coap_option_next(&oi);
    CU_ASSERT(oi.bad == 0);
    CU_ASSERT(option != NULL);
    CU_ASSERT(coap_opt_length(option) == 1);
    CU_ASSERT(*coap_opt_value(option) == opt_srt[n]);
  }
  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 1);
  CU_ASSERT(option == NULL);
  coap_delete_optlist(optlist);
}

static void
t_encode_pdu13(void) {
  coap_optlist_t *optlist = NULL;
  int n;
  uint8_t opt_num[] = { 59, 58, 57, 56, 55, 54, 53, 52, 51, 50 };
  uint8_t opt_val[] = { 59, 58, 57, 56, 55, 54, 53, 52, 51, 50 };
  uint8_t opt_srt[] = { 50, 51, 52, 53, 54, 55, 56, 57, 58, 59 };
  coap_opt_iterator_t oi;
  coap_opt_t *option;

  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  CU_ASSERT(pdu->data == NULL);

  for (n = 0; n < (int)sizeof(opt_num); n++) {
    coap_insert_optlist(&optlist, coap_new_optlist(opt_num[n],
                     sizeof(opt_val[n]), &opt_val[n]));
  }
  coap_add_optlist_pdu(pdu, &optlist);

  /* Check options in pdu are in right order */
  coap_option_iterator_init(pdu, &oi, COAP_OPT_ALL);
  for (n = 0; n < (int)sizeof(opt_num); n++) {
    option = coap_option_next(&oi);
    CU_ASSERT(oi.bad == 0);
    CU_ASSERT(option != NULL);
    CU_ASSERT(coap_opt_length(option) == 1);
    CU_ASSERT(*coap_opt_value(option) == opt_srt[n]);
  }
  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 1);
  CU_ASSERT(option == NULL);
  coap_delete_optlist(optlist);
}

static void
t_encode_pdu14(void) {
  coap_optlist_t *optlist = NULL;
  int n;
  uint8_t opt_num[] = { 53, 52, 51, 50, 51, 52, 52, 51, 50, 50 };
  uint8_t opt_val[] = { 59, 56, 53, 50, 54, 57, 58, 55, 51, 52 };
  uint8_t opt_srt[] = { 50, 51, 52, 53, 54, 55, 56, 57, 58, 59 };
  coap_opt_iterator_t oi;
  coap_opt_t *option;

  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  CU_ASSERT(pdu->data == NULL);

  for (n = 0; n < (int)sizeof(opt_num); n++) {
    coap_insert_optlist(&optlist, coap_new_optlist(opt_num[n],
                     sizeof(opt_val[n]), &opt_val[n]));
  }
  coap_add_optlist_pdu(pdu, &optlist);

  /* Check options in pdu are in right order */
  coap_option_iterator_init(pdu, &oi, COAP_OPT_ALL);
  for (n = 0; n < (int)sizeof(opt_num); n++) {
    option = coap_option_next(&oi);
    CU_ASSERT(oi.bad == 0);
    CU_ASSERT(option != NULL);
    CU_ASSERT(coap_opt_length(option) == 1);
    CU_ASSERT(*coap_opt_value(option) == opt_srt[n]);
  }
  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 1);
  CU_ASSERT(option == NULL);
  coap_delete_optlist(optlist);
}

/* Check inserting options with random types get put into the PDU in the
   right order */
static void
t_encode_pdu15(void) {
  size_t n;
  uint16_t opt_num[] = { 300,  13,  10,   7,  11, 268, 269,  12,   8,   9 };
  uint8_t  opt_val[] = {  59,  56,  53,  50,  54,  57,  58,  55,  51,  52 };
  uint8_t  opt_srt[] = {  50,  51,  52,  53,  54,  55,  56,  57,  58,  59 };
  coap_opt_iterator_t oi;
  coap_opt_t *option;

  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  CU_ASSERT(pdu->data == NULL);

  for (n = 0; n < (sizeof(opt_num)/sizeof(opt_num[0])); n++) {
    coap_insert_option(pdu, opt_num[n],
                     sizeof(opt_val[n]), &opt_val[n]);
  }

  /* Check options in pdu are in right order */
  coap_option_iterator_init(pdu, &oi, COAP_OPT_ALL);
  for (n = 0; n < (sizeof(opt_num)/sizeof(opt_num[0])); n++) {
    option = coap_option_next(&oi);
    CU_ASSERT(oi.bad == 0);
    CU_ASSERT(option != NULL);
    CU_ASSERT(coap_opt_length(option) == 1);
    CU_ASSERT(*coap_opt_value(option) == opt_srt[n]);
  }
  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 1);
  CU_ASSERT(option == NULL);
}

/* Check changing value of options works */
static void
t_encode_pdu16(void) {
  size_t n;
  uint16_t opt_num[] = { 300,  10,   7 };
  uint8_t  opt_val[] = {  53,  51,  50 };
  uint8_t  data[] = { 'd', 'a', 't', 'a' };
  uint8_t  data1[] = { 0x71, 0x32, 0x31, 0x33, 0xe1, 0x00, 0x15, 0x35,
                       0xff, 0x64, 0x61, 0x74, 0x61 };
  uint8_t  data2[] = { 0x71, 0x32, 0x33, 0x01, 0x23, 0x45, 0xe1, 0x00,
                       0x15, 0x35, 0xff, 0x64, 0x61, 0x74, 0x61 };
  uint8_t  data3[] = { 0x70, 0x31, 0x33, 0xe1, 0x00, 0x15, 0x35, 0xff,
                       0x64, 0x61, 0x74, 0x61 };
  uint8_t  data4[] = { 0x71, 0x32, 0x31, 0x33, 0xe4, 0x00, 0x15, 0x06,
                       0x54, 0x32, 0x10, 0xff, 0x64, 0x61, 0x74, 0x61 };
  int new_val;
  unsigned char buf[4];

  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  CU_ASSERT(pdu->data == NULL);

  for (n = 0; n < (sizeof(opt_num)/sizeof(opt_num[0])); n++) {
    coap_add_option(pdu, opt_num[n],
                     sizeof(opt_val[n]), &opt_val[n]);
  }
  coap_add_data(pdu, sizeof(data), data);
  CU_ASSERT(memcmp(pdu->token, data1, pdu->used_size) == 0);
  /* Now update an option in the middle */
  new_val = 0x12345;
  coap_update_option(pdu, 10,
                     coap_encode_var_safe(buf, sizeof(buf), new_val), buf);
  CU_ASSERT(memcmp(pdu->token, data2, pdu->used_size) == 0);
  /* Shrink it back again */
  new_val = 51;
  coap_update_option(pdu, 10,
                     coap_encode_var_safe(buf, sizeof(buf), new_val), buf);
  CU_ASSERT(memcmp(pdu->token, data1, pdu->used_size) == 0);
  /* Now update an option at the start */
  new_val = 0;
  coap_update_option(pdu, 7,
                     coap_encode_var_safe(buf, sizeof(buf), new_val), buf);
  CU_ASSERT(memcmp(pdu->token, data3, pdu->used_size) == 0);
  /* put it back again */
  new_val = 50;
  coap_update_option(pdu, 7,
                     coap_encode_var_safe(buf, sizeof(buf), new_val), buf);
  CU_ASSERT(memcmp(pdu->token, data1, pdu->used_size) == 0);
  /* Now update an option at the end */
  new_val = 0x6543210;
  coap_update_option(pdu, 300,
                     coap_encode_var_safe(buf, sizeof(buf), new_val), buf);
  CU_ASSERT(memcmp(pdu->token, data4, pdu->used_size) == 0);
  /* put it back again */
  new_val = 53;
  coap_update_option(pdu, 300,
                     coap_encode_var_safe(buf, sizeof(buf), new_val), buf);
  CU_ASSERT(memcmp(pdu->token, data1, pdu->used_size) == 0);
}

/* Same as t_encode_pdu16, but without any data, but with a token */
static void
t_encode_pdu17(void) {
  size_t n;
  uint8_t  token[] = { 't' };
  uint16_t opt_num[] = { 300,  10,   7 };
  uint8_t  opt_val[] = {  53,  51,  50 };
  uint8_t  data1[] = { 0x74, 0x71, 0x32, 0x31, 0x33, 0xe1, 0x00, 0x15,
                       0x35 };
  uint8_t  data2[] = { 0x74, 0x71, 0x32, 0x33, 0x01, 0x23, 0x45, 0xe1,
                       0x00, 0x15, 0x35 };
  uint8_t  data3[] = { 0x74, 0x70, 0x31, 0x33, 0xe1, 0x00, 0x15, 0x35 };
  uint8_t  data4[] = { 0x74, 0x71, 0x32, 0x31, 0x33, 0xe4, 0x00, 0x15,
                       0x06, 0x54, 0x32, 0x10 };
  int new_val;
  unsigned char buf[4];

  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  CU_ASSERT(pdu->data == NULL);

  coap_add_token(pdu, sizeof(token), token);
  for (n = 0; n < (sizeof(opt_num)/sizeof(opt_num[0])); n++) {
    coap_add_option(pdu, opt_num[n],
                     sizeof(opt_val[n]), &opt_val[n]);
  }
  CU_ASSERT(memcmp(pdu->token, data1, pdu->used_size) == 0);
  /* Now update an option in the middle */
  new_val = 0x12345;
  coap_update_option(pdu, 10,
                     coap_encode_var_safe(buf, sizeof(buf), new_val), buf);
  CU_ASSERT(memcmp(pdu->token, data2, pdu->used_size) == 0);
  /* Shrink it back again */
  new_val = 51;
  coap_update_option(pdu, 10,
                     coap_encode_var_safe(buf, sizeof(buf), new_val), buf);
  CU_ASSERT(memcmp(pdu->token, data1, pdu->used_size) == 0);
  /* Now update an option at the start */
  new_val = 0;
  coap_update_option(pdu, 7,
                     coap_encode_var_safe(buf, sizeof(buf), new_val), buf);
  CU_ASSERT(memcmp(pdu->token, data3, pdu->used_size) == 0);
  /* put it back again */
  new_val = 50;
  coap_update_option(pdu, 7,
                     coap_encode_var_safe(buf, sizeof(buf), new_val), buf);
  CU_ASSERT(memcmp(pdu->token, data1, pdu->used_size) == 0);
  /* Now update an option at the end */
  new_val = 0x6543210;
  coap_update_option(pdu, 300,
                     coap_encode_var_safe(buf, sizeof(buf), new_val), buf);
  CU_ASSERT(memcmp(pdu->token, data4, pdu->used_size) == 0);
  /* put it back again */
  new_val = 53;
  coap_update_option(pdu, 300,
                     coap_encode_var_safe(buf, sizeof(buf), new_val), buf);
  CU_ASSERT(memcmp(pdu->token, data1, pdu->used_size) == 0);
}

static void
t_encode_pdu18(void) {
  /* PDU with token, options and data */
  uint8_t teststr[] = { 0x62, 0x44, 0x12, 0x34, 0x00, 0x00, 0x8d, 0xf2,
                     'c',  'o',  'a',  'p',  ':',  '/',  '/',  'e',
                     'x',  'a',  'm',  'p',  'l',  'e',  '.',  'c',
                     'o',  'm',  '/',  '1',  '2',  '3',  '4',  '5',
                     '/',  '%',  '3',  'F',  'x',  'y',  'z',  '/',
                     '3',  '0',  '4',  '8',  '2',  '3',  '4',  '2',
                     '3',  '4',  '/',  '2',  '3',  '4',  '0',  '2',
                     '3',  '4',  '8',  '2',  '3',  '4',  '/',  '2',
                     '3',  '9',  '0',  '8',  '4',  '2',  '3',  '4',
                     '-',  '2',  '3',  '/',  '%',  'A',  'B',  '%',
                     '3',  '0',  '%',  'a',  'f',  '/',  '+',  '1',
                     '2',  '3',  '/',  'h',  'f',  'k',  's',  'd',
                     'h',  '/',  '2',  '3',  '4',  '8',  '0',  '-',
                     '2',  '3',  '4',  '-',  '9',  '8',  '2',  '3',
                     '5',  '/',  '1',  '2',  '0',  '4',  '/',  '2',
                     '4',  '3',  '5',  '4',  '6',  '3',  '4',  '5',
                     '3',  '4',  '5',  '2',  '4',  '3',  '/',  '0',
                     '1',  '9',  '8',  's',  'd',  'n',  '3',  '-',
                     'a',  '-',  '3',  '/',  '/',  '/',  'a',  'f',
                     'f',  '0',  '9',  '3',  '4',  '/',  '9',  '7',
                     'u',  '2',  '1',  '4',  '1',  '/',  '0',  '0',
                     '0',  '2',  '/',  '3',  '9',  '3',  '2',  '4',
                     '2',  '3',  '5',  '3',  '2',  '/',  '5',  '6',
                     '2',  '3',  '4',  '0',  '2',  '3',  '/',  '-',
                     '-',  '-',  '-',  '/',  '=',  '1',  '2',  '3',
                     '4',  '=',  '/',  '0',  '9',  '8',  '1',  '4',
                     '1',  '-',  '9',  '5',  '6',  '4',  '6',  '4',
                     '3',  '/',  '2',  '1',  '9',  '7',  '0',  '-',
                     '-',  '-',  '-',  '-',  '/',  '8',  '2',  '3',
                     '6',  '4',  '9',  '2',  '3',  '4',  '7',  '2',
                     'w',  'e',  'r',  'e',  'r',  'e',  'w',  'r',
                     '0',  '-',  '9',  '2',  '1',  '-',  '3',  '9',
                     '1',  '2',  '3',  '-',  '3',  '4',  '/',  0x0d,
                     0x01, '/',  '/',  '4',  '9',  '2',  '4',  '0',
                     '3',  '-',  '-',  '0',  '9',  '8',  '/',  0xc1,
                     '*',  0xff, 'd',  'a',  't',  'a'
  };
  int result;

  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  pdu->type = COAP_MESSAGE_ACK;
  pdu->code = COAP_RESPONSE_CODE(204);
  pdu->mid = 0x1234;

  CU_ASSERT(pdu->used_size == 0);

  result = coap_add_token(pdu, 2, (const uint8_t *)"\0\0");

  CU_ASSERT(result > 0);
  result = coap_add_option(pdu, COAP_OPTION_LOCATION_PATH, 255,
                           (const uint8_t *)"coap://example.com/12345/%3Fxyz/3048234234/23402348234/239084234-23/%AB%30%af/+123/hfksdh/23480-234-98235/1204/243546345345243/0198sdn3-a-3///aff0934/97u2141/0002/3932423532/56234023/----/=1234=/098141-9564643/21970-----/82364923472wererewr0-921-39123-34/");

  CU_ASSERT(result == 257);
  CU_ASSERT(pdu->max_opt == 8);
  CU_ASSERT(pdu->used_size == 259);
  CU_ASSERT_PTR_NULL(pdu->data);

  result = coap_add_option(pdu, COAP_OPTION_LOCATION_QUERY,
                           1, (const uint8_t *)"*");

  CU_ASSERT(result == 2);
  CU_ASSERT(pdu->used_size == 261);
  CU_ASSERT_PTR_NULL(pdu->data);

  result = coap_insert_option(pdu, COAP_OPTION_LOCATION_PATH, 14,
                              (const uint8_t *)"//492403--098/");

  CU_ASSERT(result == 16);
  CU_ASSERT(pdu->used_size == 277);
  CU_ASSERT_PTR_NULL(pdu->data);

  result = coap_add_data(pdu, 4, (const uint8_t *)"data");

  CU_ASSERT(result > 0);
  CU_ASSERT(pdu->used_size == 282);
  CU_ASSERT(pdu->data == pdu->token + 278);

  CU_ASSERT(coap_pdu_encode_header(pdu, COAP_PROTO_UDP) == 4);
  CU_ASSERT(memcmp(pdu->token - 4, teststr, sizeof(teststr)) == 0);

}

/* Remove an option (no data) */
static void
t_encode_pdu19(void) {
  size_t n;
  uint8_t  token[] = { 't' };
  uint16_t opt_num[] = { 300,   7,  21,  25 };
  uint8_t  opt_val[] = {  54,  50,  52,  53 };
  uint8_t  data1[] = { 0x74, 0x71, 0x32, 0xd1, 0x01, 0x34, 0x41, 0x35,
                       0xe1, 0x00, 0x06, 0x36 };
  uint8_t  data2[] = { 0x74, 0x71, 0x32, 0xd1, 0x05, 0x35, 0xe1, 0x00,
                       0x06, 0x36 };
  uint8_t  data3[] = { 0x74, 0xd1, 0x0c, 0x35, 0xe1, 0x00, 0x06, 0x36 };
  uint8_t  data4[] = { 0x74, 0xd1, 0x0c, 0x35 };
  uint8_t  data5[] = { 0x74 };
  uint8_t  data6[] = { 0x74, 0xd1, 0x0c, 0x0a };
  int new_val;
  unsigned char buf[4];

  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  CU_ASSERT(pdu->data == NULL);

  coap_add_token(pdu, sizeof(token), token);
  for (n = 0; n < (sizeof(opt_num)/sizeof(opt_num[0])); n++) {
    coap_add_option(pdu, opt_num[n],
                     sizeof(opt_val[n]), &opt_val[n]);
  }
  CU_ASSERT(memcmp(pdu->token, data1, pdu->used_size) == 0);

  /* Now remove an option in the middle */
  coap_remove_option(pdu, 21);
  CU_ASSERT(memcmp(pdu->token, data2, pdu->used_size) == 0);

  /* Now remove an option from the start */
  coap_remove_option(pdu, 7);
  CU_ASSERT(memcmp(pdu->token, data3, pdu->used_size) == 0);

  /* Now remove an option from the end */
  coap_remove_option(pdu, 300);
  CU_ASSERT(memcmp(pdu->token, data4, pdu->used_size) == 0);

  /* Now remove the final option */
  coap_remove_option(pdu, 25);
  CU_ASSERT(memcmp(pdu->token, data5, pdu->used_size) == 0);

  /* Now insert an option */
  new_val = 10;
  coap_update_option(pdu, 25,
                     coap_encode_var_safe(buf, sizeof(buf), new_val), buf);
  CU_ASSERT(memcmp(pdu->token, data6, pdu->used_size) == 0);
}

/* Remove an option (with data) */
static void
t_encode_pdu20(void) {
  size_t n;
  uint8_t  token[] = { 't' };
  uint16_t opt_num[] = { 300,   7,  21,  25 };
  uint8_t  opt_val[] = {  54,  50,  52,  53 };
  uint8_t  data[] = { 'd', 'a', 't', 'a' };
  uint8_t  data1[] = { 0x74, 0x71, 0x32, 0xd1, 0x01, 0x34, 0x41, 0x35,
                       0xe1, 0x00, 0x06, 0x36, 0xff, 0x64, 0x61, 0x74,
                       0x61 };
  uint8_t  data2[] = { 0x74, 0x71, 0x32, 0xd1, 0x05, 0x35, 0xe1, 0x00,
                       0x06, 0x36, 0xff, 0x64, 0x61, 0x74, 0x61 };
  uint8_t  data3[] = { 0x74, 0xd1, 0x0c, 0x35, 0xe1, 0x00, 0x06, 0x36,
                       0xff, 0x64, 0x61, 0x74, 0x61 };
  uint8_t  data4[] = { 0x74, 0xd1, 0x0c, 0x35, 0xff, 0x64, 0x61, 0x74,
                       0x61 };
  uint8_t  data5[] = { 0x74, 0xff, 0x64, 0x61, 0x74, 0x61 };
  uint8_t  data6[] = { 0x74, 0xd1, 0x0c, 0x0a, 0xff, 0x64, 0x61, 0x74,
                       0x61 };
  int new_val;
  unsigned char buf[4];

  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  CU_ASSERT(pdu->data == NULL);

  coap_add_token(pdu, sizeof(token), token);
  for (n = 0; n < (sizeof(opt_num)/sizeof(opt_num[0])); n++) {
    coap_add_option(pdu, opt_num[n],
                     sizeof(opt_val[n]), &opt_val[n]);
  }
  coap_add_data(pdu, sizeof(data), data);
  CU_ASSERT(memcmp(pdu->token, data1, pdu->used_size) == 0);

  /* Now remove an option in the middle */
  coap_remove_option(pdu, 21);
  CU_ASSERT(memcmp(pdu->token, data2, pdu->used_size) == 0);

  /* Now remove an option from the start */
  coap_remove_option(pdu, 7);
  CU_ASSERT(memcmp(pdu->token, data3, pdu->used_size) == 0);

  /* Now remove an option from the end */
  coap_remove_option(pdu, 300);
  CU_ASSERT(memcmp(pdu->token, data4, pdu->used_size) == 0);

  /* Now remove the final option */
  coap_remove_option(pdu, 25);
  CU_ASSERT(memcmp(pdu->token, data5, pdu->used_size) == 0);

  /* Now insert an option */
  new_val = 10;
  coap_update_option(pdu, 25,
                     coap_encode_var_safe(buf, sizeof(buf), new_val), buf);
  CU_ASSERT(memcmp(pdu->token, data6, pdu->used_size) == 0);
}

/* Update token */
static void
t_encode_pdu21(void) {
  size_t n;
  uint8_t  token[] = { 't' };
  uint8_t  new_token[] = { 't', 'o', 'k', 'e', 'n' };
  uint16_t opt_num[] = { 300,  10,   7,  21,  25 };
  uint8_t  opt_val[] = {  54,  51,  50,  52,  53 };
  uint8_t  data[] = { 'd', 'a', 't', 'a' };
  uint8_t  data1[] = { 0x74, 0x71, 0x32, 0x31, 0x33, 0xb1, 0x34, 0x41,
                       0x35, 0xe1, 0x00, 0x06, 0x36, 0xff, 0x64, 0x61,
                       0x74, 0x61 };
  uint8_t  data2[] = { 0x74, 0x6f, 0x6b, 0x65, 0x6e, 0x71, 0x32, 0x31,
                       0x33, 0xb1, 0x34, 0x41, 0x35, 0xe1, 0x00, 0x06,
                       0x36, 0xff, 0x64, 0x61, 0x74, 0x61 };
  uint8_t  data3[] = { 0x71, 0x32, 0x31, 0x33, 0xb1, 0x34, 0x41, 0x35,
                       0xe1, 0x00, 0x06, 0x36, 0xff, 0x64, 0x61, 0x74,
                       0x61 };

  coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

  CU_ASSERT(pdu->data == NULL);

  coap_add_token(pdu, sizeof(token), token);
  for (n = 0; n < (sizeof(opt_num)/sizeof(opt_num[0])); n++) {
    coap_add_option(pdu, opt_num[n],
                     sizeof(opt_val[n]), &opt_val[n]);
  }
  coap_add_data(pdu, sizeof(data), data);
  CU_ASSERT(memcmp(pdu->token, data1, pdu->used_size) == 0);

  /* Now update token */
  coap_update_token(pdu, sizeof(new_token), new_token);
  CU_ASSERT(memcmp(pdu->token, data2, pdu->used_size) == 0);

  /* Now restore token */
  coap_update_token(pdu, sizeof(token), token);
  CU_ASSERT(memcmp(pdu->token, data1, pdu->used_size) == 0);

  /* Now set token to zero length */
  coap_update_token(pdu, 0, NULL);
  CU_ASSERT(memcmp(pdu->token, data3, pdu->used_size) == 0);
}

/* insert option before (large) final one */
static void
t_encode_pdu22(void) {
  size_t n;
  uint8_t  token[] = { 't' };
  uint8_t  buf[4];
  uint16_t opt_num[] = { 28,  28,    28,      28 };
  uint32_t opt_val[] = { 0x1, 0x100, 0x10000, 0x1000000 };
  uint8_t data1[][8] = {
                        { 0x74, 0xd1, 0x0f, 0x01 },
                        { 0x74, 0xd2, 0x0f, 0x01, 0x00 },
                        { 0x74, 0xd3, 0x0f, 0x01, 0x00, 0x00 },
                        { 0x74, 0xd4, 0x0f, 0x01, 0x00, 0x00, 0x00 }};
  uint8_t  data2[][16] = {
                        { 0x74, 0xd3, 0x0a, 0xff, 0xff, 0xf6, 0x51, 0x01 },
                        { 0x74, 0xd3, 0x0a, 0xff, 0xff, 0xf6, 0x52, 0x01,
                          0x00 },
                        { 0x74, 0xd3, 0x0a, 0xff, 0xff, 0xf6, 0x53, 0x01,
                          0x00, 0x00 },
                        { 0x74, 0xd3, 0x0a, 0xff, 0xff, 0xf6, 0x54, 0x01,
                          0x00, 0x00, 0x00 }};

  for (n = 0; n < (sizeof(opt_num)/sizeof(opt_num[0])); n++) {
    coap_pdu_clear(pdu, pdu->max_size);        /* clear PDU */

    CU_ASSERT(pdu->data == NULL);

    coap_add_token(pdu, sizeof(token), token);
    coap_add_option(pdu, opt_num[n],
                    coap_encode_var_safe(buf, sizeof(buf), opt_val[n]), buf);
    CU_ASSERT(memcmp(pdu->token, data1[n], pdu->used_size) == 0);

    /* Now insert option */
    coap_insert_option(pdu, 23,
                       coap_encode_var_safe(buf, sizeof(buf), 0xfffff6), buf);
    CU_ASSERT(memcmp(pdu->token, data2[n], pdu->used_size) == 0);
  }
}


static int
t_pdu_tests_create(void) {
  pdu = coap_pdu_init(0, 0, 0, COAP_DEFAULT_MTU);

  return pdu == NULL;
}

static int
t_pdu_tests_remove(void) {
  coap_delete_pdu(pdu);
  return 0;
}

CU_pSuite
t_init_pdu_tests(void) {
  CU_pSuite suite[2];

  suite[0] = CU_add_suite("pdu parser", t_pdu_tests_create, t_pdu_tests_remove);
  if (!suite[0]) {                        /* signal error */
    fprintf(stderr, "W: cannot add pdu parser test suite (%s)\n",
            CU_get_error_msg());

    return NULL;
  }

#define PDU_TEST(s,t)                                                      \
  if (!CU_ADD_TEST(s,t)) {                                              \
    fprintf(stderr, "W: cannot add pdu parser test (%s)\n",              \
            CU_get_error_msg());                                      \
  }

  PDU_TEST(suite[0], t_parse_pdu1);
  PDU_TEST(suite[0], t_parse_pdu2);
  PDU_TEST(suite[0], t_parse_pdu3);
  PDU_TEST(suite[0], t_parse_pdu4);
  PDU_TEST(suite[0], t_parse_pdu5);
  PDU_TEST(suite[0], t_parse_pdu6);
  PDU_TEST(suite[0], t_parse_pdu7);
  PDU_TEST(suite[0], t_parse_pdu8);
  PDU_TEST(suite[0], t_parse_pdu9);
  PDU_TEST(suite[0], t_parse_pdu10);
  PDU_TEST(suite[0], t_parse_pdu11);
  PDU_TEST(suite[0], t_parse_pdu12);
  PDU_TEST(suite[0], t_parse_pdu13);
  PDU_TEST(suite[0], t_parse_pdu14);
  PDU_TEST(suite[0], t_parse_pdu15);
  PDU_TEST(suite[0], t_parse_pdu16);
  PDU_TEST(suite[0], t_parse_pdu17);

  suite[1] = CU_add_suite("pdu encoder", t_pdu_tests_create, t_pdu_tests_remove);
  if (suite[1]) {
#define PDU_ENCODER_TEST(s,t)                                                      \
  if (!CU_ADD_TEST(s,t)) {                                              \
    fprintf(stderr, "W: cannot add pdu encoder test (%s)\n",              \
            CU_get_error_msg());                                      \
  }
    PDU_ENCODER_TEST(suite[1], t_encode_pdu1);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu2);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu3);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu4);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu5);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu6);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu7);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu8);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu9);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu10);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu11);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu12);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu13);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu14);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu15);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu16);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu17);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu18);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu19);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu20);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu21);
    PDU_ENCODER_TEST(suite[1], t_encode_pdu22);

  } else                         /* signal error */
    fprintf(stderr, "W: cannot add pdu parser test suite (%s)\n",
            CU_get_error_msg());

  return suite[0];
}

