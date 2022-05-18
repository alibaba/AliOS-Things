/* libcoap unit tests
 *
 * Copyright (C) 2019 Olaf Bergmann <bergmann@tzi.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "test_common.h"
#include "test_encode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************************************************************
 ** decoder tests
 ************************************************************************/

static void
t_decode1(void) {
  const coap_binary_t teststr = {  0, NULL };
  unsigned int result;

  result = coap_decode_var_bytes(teststr.s, teststr.length);

  CU_ASSERT(result == 0);
}

static void
t_decode2(void) {
  uint8_t data[] = { 0x01 };
  coap_binary_t teststr = {  sizeof(data), data };
  unsigned int result;

  result = coap_decode_var_bytes8(teststr.s, teststr.length);

  CU_ASSERT(result == 1);
}

static void
t_decode3(void) {
  uint8_t data[] = { 0x01, 0x00, 0x00, 0x00 };
  coap_binary_t teststr = { sizeof(data), data };
  unsigned int result;

  result = coap_decode_var_bytes(teststr.s, teststr.length);

  CU_ASSERT(result == 0x01000000);
}

static void
t_decode4(void) {
  uint8_t data[] = { 0x05, 0x06, 0x07, 0x08 };
  coap_binary_t teststr = { sizeof(data), data };
  unsigned int result;

  result = coap_decode_var_bytes(teststr.s, teststr.length);

  CU_ASSERT(result == 0x05060708);
}

static void
t_decode5(void) {
  const coap_binary_t teststr = {  0, NULL };
  uint64_t result;

  result = coap_decode_var_bytes8(teststr.s, teststr.length);

  CU_ASSERT(result == 0);
}

static void
t_decode6(void) {
  uint8_t data[] = { 0x01 };
  coap_binary_t teststr = {  sizeof(data), data };
  uint64_t result;

  result = coap_decode_var_bytes8(teststr.s, teststr.length);

  CU_ASSERT(result == 1);
}

static void
t_decode7(void) {
  uint8_t data[] = { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  coap_binary_t teststr = { sizeof(data), data };
  uint64_t result;

  result = coap_decode_var_bytes8(teststr.s, teststr.length);

  CU_ASSERT(result == 0x0100000000000000);
}

static void
t_decode8(void) {
  uint8_t data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  coap_binary_t teststr = { sizeof(data), data };
  uint64_t result;

  result = coap_decode_var_bytes8(teststr.s, teststr.length);

  CU_ASSERT(result == 0x0102030405060708);
}

/************************************************************************
 ** encoder tests
 ************************************************************************/

static void
t_encode1(void) {
  uint8_t buf[16];
  uint8_t data[] = { 0x00 };
  unsigned int result;

  result = coap_encode_var_safe(buf, sizeof(buf), 0x00);

  CU_ASSERT(result == 0);

  CU_ASSERT(memcmp(buf, data, result) == 0);
}

static void
t_encode2(void) {
  uint8_t buf[16];
  uint8_t data[] = { 0x01 };
  unsigned int result;

  result = coap_encode_var_safe(buf, sizeof(buf), 0x01);

  CU_ASSERT(result == sizeof(data));

  CU_ASSERT(memcmp(buf, data, result) == 0);
}

static void
t_encode3(void) {
  uint8_t buf[16];
  uint8_t data[] = { 0x05, 0x06, 0x07, 0x08 };
  unsigned int result;

  result = coap_encode_var_safe8(buf, sizeof(buf), 0x05060708);

  CU_ASSERT(result == sizeof(data));

  CU_ASSERT(memcmp(buf, data, result) == 0);
}

static void
t_encode4(void) {
  unsigned int result = 0;
  /* This check will abort the program if configured with --enable-assert */
#ifdef NDEBUG
  uint8_t buf[16];
  result = coap_encode_var_safe(buf, 2, 0x01020304);
#endif /* NDEBUG */
  CU_ASSERT(result == 0);
}

static void
t_encode5(void) {
  uint8_t buf[16];
  uint8_t data[] = { 0x00 };
  unsigned int result;

  result = coap_encode_var_safe8(buf, sizeof(buf), 0x00);

  CU_ASSERT(result == 0);

  CU_ASSERT(memcmp(buf, data, result) == 0);
}

static void
t_encode6(void) {
  uint8_t buf[16];
  uint8_t data[] = { 0x01 };
  unsigned int result;

  result = coap_encode_var_safe8(buf, sizeof(buf), 0x01);

  CU_ASSERT(result == sizeof(data));

  CU_ASSERT(memcmp(buf, data, result) == 0);
}

static void
t_encode7(void) {
  uint8_t buf[16];
  uint8_t data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  unsigned int result;

  result = coap_encode_var_safe8(buf, sizeof(buf), 0x0102030405060708);

  CU_ASSERT(result == sizeof(data));

  CU_ASSERT(memcmp(buf, data, result) == 0);
}

static void
t_encode8(void) {
  unsigned int result = 0;
  /* This check will abort the program if configured with --enable-assert */
#ifdef NDEBUG
  uint8_t buf[16];
  result = coap_encode_var_safe8(buf, 2, 0x0102030405060708);
#endif /* NDEBUG */
  CU_ASSERT(result == 0);
}

/************************************************************************
 ** initialization
 ************************************************************************/

CU_pSuite
t_init_encode_tests(void) {
  CU_pSuite suite[5];

  suite[0] = CU_add_suite("byte value decoder", NULL, NULL);
  if (!suite[0]) {                        /* signal error */
    fprintf(stderr, "W: cannot add byte value decoder test suite (%s)\n",
            CU_get_error_msg());

    return NULL;
  }

#define DECODE_TEST(n,s)                                       \
  if (!CU_add_test(suite[0], s, t_decode##n)) {                \
    fprintf(stderr, "W: cannot add decoder test (%s)\n",       \
            CU_get_error_msg());                               \
  }

  DECODE_TEST(1, "decode value #1");
  DECODE_TEST(2, "decode value #2");
  DECODE_TEST(3, "decode value #3");
  DECODE_TEST(4, "decode value #4");
  DECODE_TEST(5, "decode value #5");
  DECODE_TEST(6, "decode value #6");
  DECODE_TEST(7, "decode value #7");
  DECODE_TEST(8, "decode value #8");

  if ((suite[1] = CU_add_suite("byte value encoder", NULL, NULL))) {
#define ENCODE_TEST(n,s)                                          \
    if (!CU_add_test(suite[1], s, t_encode##n)) {                 \
      fprintf(stderr, "W: cannot add encoder test (%s)\n",        \
              CU_get_error_msg());                                \
    }

    ENCODE_TEST(1, "encode value #1");
    ENCODE_TEST(2, "encode value #2");
    ENCODE_TEST(3, "encode value #3");
    ENCODE_TEST(4, "encode value #4");
    ENCODE_TEST(5, "encode value #5");
    ENCODE_TEST(6, "encode value #6");
    ENCODE_TEST(7, "encode value #7");
    ENCODE_TEST(8, "encode value #8");

  } else {
    fprintf(stderr, "W: cannot add byte value encoder test suite (%s)\n",
            CU_get_error_msg());
  }

  return suite[0];
}

