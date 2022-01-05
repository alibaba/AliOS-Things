/* libcoap unit tests
 *
 * Copyright (C) 2012,2015 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap_config.h"
#include "test_options.h"

#include <coap.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************************************************************
 ** decoder tests
 ************************************************************************/

static void
t_parse_option1(void) {
  /* delta == 0, length == 0, value == 0 */
  coap_str_const_t teststr = {  1, (const uint8_t *)"" };

  size_t result;
  coap_option_t option;

  /* result = coap_opt_parse(teststr.s, teststr.s + teststr.length, &option); */
  result = coap_opt_parse(teststr.s, teststr.length, &option);
  CU_ASSERT(result == 1);
  CU_ASSERT(option.delta == 0);
  CU_ASSERT(option.length == 0);
  /* FIXME: value? */
}

static void
t_parse_option2(void) {
  /* delta == 12, length == 1, value == 0 */
  coap_str_const_t teststr = {  2, (const uint8_t *)"\xc1" };

  size_t result;
  coap_option_t option;

  result = coap_opt_parse(teststr.s, teststr.length, &option);
  CU_ASSERT(result == 2);
  CU_ASSERT(option.delta == 12);
  CU_ASSERT(option.length == 1);
  CU_ASSERT(option.value == teststr.s + 1);
}

static void
t_parse_option3(void) {
  /* delta == 3, length == 12, value == 0 */
  coap_str_const_t teststr = { 13, (const uint8_t *)"\x3c\x00\x01\x02\x03\x04"
                                       "\x05\x06\x07\x08\x09\x0a\x0b" };

  size_t result;
  coap_option_t option;

  result = coap_opt_parse(teststr.s, teststr.length, &option);
  CU_ASSERT(result == 13);
  CU_ASSERT(option.delta == 3);
  CU_ASSERT(option.length == 12);
  CU_ASSERT(option.value == teststr.s + 1);
  /* CU_ASSERT(memcmp(option.value, teststr.s + 1, 12) == 0); */
}

static void
t_parse_option4(void) {
  /* delta == 15, length == 3, value == 0 */
  coap_str_const_t teststr = {  2, (const uint8_t *)"\xf3" };

  size_t result;
  coap_option_t option;

  result = coap_opt_parse(teststr.s, teststr.length, &option);
  CU_ASSERT(result == 0);
}

static void
t_parse_option5(void) {
  /* delta == 3, length == 15, value == 0 */
  coap_str_const_t teststr = {  2, (const uint8_t *)"\x3f" };

  size_t result;
  coap_option_t option;

  result = coap_opt_parse(teststr.s, teststr.length, &option);
  CU_ASSERT(result == 0);
}

static void
t_parse_option6(void) {
  /* delta == 15, length == 15 */
  coap_str_const_t teststr = {  1, (const uint8_t *)"\xff" };

  size_t result;
  coap_option_t option;

  result = coap_opt_parse(teststr.s, teststr.length, &option);
  CU_ASSERT(result == 0);
}

static void
t_parse_option7(void) {
  /* delta == 20, length == 0 */
  coap_str_const_t teststr = {  2, (const uint8_t *)"\xd0\x07" };

  size_t result;
  coap_option_t option;

  result = coap_opt_parse(teststr.s, teststr.length, &option);
  CU_ASSERT(result == 2);
  CU_ASSERT(option.delta == 20);
  CU_ASSERT(option.length == 0);
}

static void
t_parse_option8(void) {
  /* delta == 780, length == 0 */
  coap_str_const_t teststr = {  3, (const uint8_t *)"\xe0\x01\xff" };

  size_t result;
  coap_option_t option;

  result = coap_opt_parse(teststr.s, teststr.length, &option);
  CU_ASSERT(result == 3);
  CU_ASSERT(option.delta == 780);
  CU_ASSERT(option.length == 0);
}

static void
t_parse_option9(void) {
  /* delta == 65535, length == 0 */
  coap_str_const_t teststr = {  3, (const uint8_t *)"\xe0\xfe\xf2" };

  size_t result;
  coap_option_t option;

  result = coap_opt_parse(teststr.s, teststr.length, &option);
  CU_ASSERT(result == 3);
  CU_ASSERT(option.delta == 65535);
}

static void
t_parse_option10(void) {
  /* delta > 65535 (illegal), length == 0 */
  coap_str_const_t teststr = {  3, (const uint8_t *)"\xe0\xff\xff" };

  size_t result;
  coap_option_t option;

  result = coap_opt_parse(teststr.s, teststr.length, &option);
  CU_ASSERT(result == 0);
}

static void
t_parse_option11(void) {
  /* illegal delta value (option too short) */
  coap_str_const_t teststr = {  1, (const uint8_t *)"\xd0" };

  size_t result;
  coap_option_t option;

  result = coap_opt_parse(teststr.s, teststr.length, &option);
  CU_ASSERT(result == 0);
}

static void
t_parse_option12(void) {
  /* delta == 280, length == 500 */
  coap_str_const_t teststr = {  3, (const uint8_t *)"\xee\xff\x0b" };

  size_t result;
  coap_option_t option;

  result = coap_opt_parse(teststr.s, teststr.length, &option);
  CU_ASSERT(result == 0);
}

static void
t_parse_option13(void) {
  /* delta == 280, length == 500 */
  unsigned char _data[505];
  coap_string_t teststr = {  sizeof(_data), _data };
  teststr.s[0] = 0xee;
  teststr.s[1] = 0x00;
  teststr.s[2] = 0x0b;
  teststr.s[3] = 0x00;
  teststr.s[4] = 0xe7;

  size_t result;
  coap_option_t option;

  result = coap_opt_parse(teststr.s, teststr.length, &option);
  CU_ASSERT(result == sizeof(_data));
  CU_ASSERT(option.delta == 280);
  CU_ASSERT(option.length == 500);
  CU_ASSERT(option.value == &_data[5]);
}

static void
t_parse_option14(void) {
  /* delta == 268, length == 65535 */
  unsigned char *data;
  unsigned int length = 4 + 65535;

  data = (unsigned char *)malloc(length);
  if (!data) {
    CU_FAIL("internal error in test framework -- insufficient memory\n");
    return;
  }

  data[0] = 0xde;
  data[1] = 0xff;
  data[2] = 0xfe;
  data[3] = 0xf2;

  size_t result;
  coap_option_t option;

  result = coap_opt_parse(data, length, &option);
  CU_ASSERT(result == length);
  CU_ASSERT(option.delta == 268);
  CU_ASSERT(option.length == 65535);
  CU_ASSERT(option.value == &data[4]);
  free(data);
}

/************************************************************************
 ** encoder tests
 ************************************************************************/

static void
t_encode_option1(void) {
  char teststr[] = { 0x00 };
  unsigned char buf[40];
  size_t result;

  result = coap_opt_setheader((coap_opt_t *)buf, sizeof(buf), 0, 0);
  CU_ASSERT(result == sizeof(teststr));

  CU_ASSERT(memcmp(buf, teststr, result) == 0);
}

static void
t_encode_option2(void) {
  uint8_t teststr[] = { 0x5d, 0xff };
  unsigned char buf[40];
  size_t result;

  result = coap_opt_setheader((coap_opt_t *)buf, sizeof(buf), 5, 268);
  CU_ASSERT(result == sizeof(teststr));

  CU_ASSERT(memcmp(buf, teststr, result) == 0);
}

static void
t_encode_option3(void) {
  uint8_t teststr[] = { 0xd1, 0x01 };
  unsigned char buf[40];
  size_t result;

  result = coap_opt_setheader((coap_opt_t *)buf, sizeof(buf), 14, 1);
  CU_ASSERT(result == sizeof(teststr));

  CU_ASSERT(memcmp(buf, teststr, result) == 0);
}

static void
t_encode_option4(void) {
  uint8_t teststr[] = { 0xdd, 0xff, 0xab };
  unsigned char buf[40];
  size_t result;

  result = coap_opt_setheader((coap_opt_t *)buf, sizeof(buf), 268, 184);
  CU_ASSERT(result == sizeof(teststr));

  CU_ASSERT(memcmp(buf, teststr, result) == 0);
}

static void
t_encode_option5(void) {
  uint8_t teststr[] = { 0xed, 0x13, 0x00, 0xff };
  unsigned char buf[40];
  size_t result;

  result = coap_opt_setheader((coap_opt_t *)buf, sizeof(buf), 5133, 268);
  CU_ASSERT(result == sizeof(teststr));

  CU_ASSERT(memcmp(buf, teststr, result) == 0);
}

static void
t_encode_option6(void) {
  uint8_t teststr[] = { 0xee, 0xfe, 0xf2, 0xfe, 0xf2 };
  unsigned char buf[40];
  size_t result;

  result = coap_opt_setheader((coap_opt_t *)buf, sizeof(buf), 65535, 65535);
  CU_ASSERT(result == sizeof(teststr));

  CU_ASSERT(memcmp(buf, teststr, result) == 0);
}

static void
t_encode_option7(void) {
  uint8_t teststr[] = { 0x35, 'v', 'a', 'l', 'u', 'e' };
  const size_t valoff = 1;
  unsigned char buf[40];
  size_t result;

  result = coap_opt_encode((coap_opt_t *)buf, sizeof(buf), 3,
                           (unsigned char *)teststr + valoff,
                           sizeof(teststr) - valoff);

  CU_ASSERT(result == sizeof(teststr));

  CU_ASSERT(memcmp(buf, teststr, result) == 0);
}

static void
t_encode_option8(void) {
  /* value does not fit in message buffer */
  unsigned char buf[40];
  size_t result;

  result = coap_opt_encode((coap_opt_t *)buf, 8, 15,
                           (const uint8_t *)"something", 9);

  CU_ASSERT(result == 0);

  result = coap_opt_encode((coap_opt_t *)buf, 1, 15,
                           (const uint8_t *)"something", 9);

  CU_ASSERT(result == 0);
}

static void
t_encode_option9(void) {
  uint8_t teststr[] = { 0xe1, 0x00, 0x00 };
  unsigned char buf[40] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  size_t result;

  result = coap_opt_setheader((coap_opt_t *)buf, sizeof(buf), 269, 1);
  CU_ASSERT(result == sizeof(teststr));

  CU_ASSERT(memcmp(buf, teststr, result) == 0);
}

/************************************************************************
 ** accessor tests
 ************************************************************************/

static void
t_access_option1(void) {
  const uint8_t teststr[] = { 0x12, 'a', 'b' };

  CU_ASSERT(coap_opt_delta((const coap_opt_t *)teststr) == 1);
  CU_ASSERT(coap_opt_length((const coap_opt_t *)teststr) == 2);
  CU_ASSERT_PTR_EQUAL_C(coap_opt_value((const coap_opt_t *)teststr), teststr + 1);
  CU_ASSERT(coap_opt_size((const coap_opt_t *)teststr) == sizeof(teststr));
}

static void
t_access_option2(void) {
  const uint8_t teststr[] = { 0xe2, 0x18, 0xfd, 'a', 'b' };

  CU_ASSERT(coap_opt_delta((const coap_opt_t *)teststr) == 6666);
  CU_ASSERT(coap_opt_length((const coap_opt_t *)teststr) == 2);
  CU_ASSERT_PTR_EQUAL_C(coap_opt_value((const coap_opt_t *)teststr), teststr + 3);
  CU_ASSERT(coap_opt_size((const coap_opt_t *)teststr) == sizeof(teststr));
}

static void
t_access_option3(void) {
  const uint8_t teststr[] = { 0xed, 0x18, 0x0a, 0x00, 'a', 'b', 'c', 'd',
                           'e',  'f',  'g',  'h',  'i', 'j', 'k', 'l',
                           'm'
  };

  CU_ASSERT(coap_opt_delta((const coap_opt_t *)teststr) == 6423);
  CU_ASSERT(coap_opt_length((const coap_opt_t *)teststr) == 13);
  CU_ASSERT_PTR_EQUAL_C(coap_opt_value((const coap_opt_t *)teststr), teststr + 4);
  CU_ASSERT(coap_opt_size((const coap_opt_t *)teststr) == sizeof(teststr));
}

static void
t_access_option4(void) {
  const uint8_t teststr[] = { 0xde, 0xff, 0xfe, 0xf2, 'a', 'b', 'c' };

  CU_ASSERT(coap_opt_delta((const coap_opt_t *)teststr) == 268);
  CU_ASSERT(coap_opt_length((const coap_opt_t *)teststr) == 65535);
  CU_ASSERT_PTR_EQUAL_C(coap_opt_value((const coap_opt_t *)teststr), teststr + 4);
  CU_ASSERT(coap_opt_size((const coap_opt_t *)teststr) == 65535 + 4);
}

static void
t_access_option5(void) {
  const uint8_t teststr[] = { 0xee, 0xfe, 0xf2, 0x00, 0xdd, 'a', 'b', 'c' };

  CU_ASSERT(coap_opt_delta((const coap_opt_t *)teststr) == 65535);
  CU_ASSERT(coap_opt_length((const coap_opt_t *)teststr) == 490);
  CU_ASSERT_PTR_EQUAL_C(coap_opt_value((const coap_opt_t *)teststr), teststr + 5);
  CU_ASSERT(coap_opt_size((const coap_opt_t *)teststr) == 495);
}

static void
t_access_option6(void) {
  coap_log_t level = coap_get_log_level();
  const uint8_t teststr[] = { 0xf2, 'a', 'b' };

  coap_set_log_level(LOG_CRIT);
  CU_ASSERT(coap_opt_delta((const coap_opt_t *)teststr) == 0);
  coap_set_log_level(level);
  CU_ASSERT(coap_opt_length((const coap_opt_t *)teststr) == 0);
  CU_ASSERT_PTR_EQUAL_C(coap_opt_value((const coap_opt_t *)teststr), NULL);
  CU_ASSERT(coap_opt_size((const coap_opt_t *)teststr) == 0);
}

static void
t_access_option7(void) {
  const uint8_t teststr[] = { 0x2f, 'a', 'b' };

  CU_ASSERT(coap_opt_delta((const coap_opt_t *)teststr) == 2);
  CU_ASSERT(coap_opt_length((const coap_opt_t *)teststr) == 0);
  CU_ASSERT_PTR_EQUAL_C(coap_opt_value((const coap_opt_t *)teststr), NULL);
  CU_ASSERT(coap_opt_size((const coap_opt_t *)teststr) == 0);
}

/************************************************************************
 ** accessor tests
 ************************************************************************/

#define TEST_MAX_SIZE 1000

#ifdef _MSC_VER
#  define ALIGNED(x)
#else
#  define ALIGNED(x) __attribute__ ((aligned (x)))
#endif

static void
t_iterate_option1(void) {
  /* CoAP PDU without token, options, or data */
  uint8_t teststr[] ALIGNED(8) = {
    0x00, 0x00, 0x00, 0x00
  };

  coap_pdu_t pdu = {
    .max_size = TEST_MAX_SIZE,
    .token = teststr,
    .used_size = 0
  };
  coap_opt_iterator_t oi, *result;
  coap_opt_t *option;

  result = coap_option_iterator_init(&pdu, &oi, COAP_OPT_ALL);

  CU_ASSERT(result == NULL);
  CU_ASSERT(oi.bad == 1);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 1);
  CU_ASSERT(option == NULL);
}

static void
t_iterate_option2(void) {
  /* CoAP PDU with token but without options and data */
  uint8_t teststr[3] ALIGNED(8) = {
    't', 'o', 'k'
  };

  coap_pdu_t pdu = {
    .max_size = TEST_MAX_SIZE,
    .token_length = 3,
    .token = teststr,
    .used_size = sizeof(teststr)
  };
  coap_opt_iterator_t oi, *result;
  coap_opt_t *option;

  result = coap_option_iterator_init(&pdu, &oi, COAP_OPT_ALL);

  CU_ASSERT(result == NULL);
  CU_ASSERT(oi.bad == 1);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 1);
  CU_ASSERT(option == NULL);
}

static void
t_iterate_option3(void) {
  /* CoAP PDU with token and options */
  uint8_t teststr[] ALIGNED(8) = {
    't', 'o', 'k', 0x13,
    'o',  'p',  't',  0x00, 0xd1, 0x10, 'x'
  };

  coap_pdu_t pdu = {
    .max_size = TEST_MAX_SIZE,
    .token_length = 3,
    .token = teststr,
    .used_size = sizeof(teststr)
  };
  coap_opt_iterator_t oi, *result;
  coap_opt_t *option;

  result = coap_option_iterator_init(&pdu, &oi, COAP_OPT_ALL);

  CU_ASSERT_PTR_EQUAL(result, &oi);
  CU_ASSERT(oi.bad == 0);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 0);
  CU_ASSERT(oi.type == 1);
  CU_ASSERT_PTR_EQUAL(option, teststr + 3);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 0);
  CU_ASSERT(oi.type == 1);
  CU_ASSERT_PTR_EQUAL(option, teststr + 7);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 0);
  CU_ASSERT(oi.type == 30);
  CU_ASSERT_PTR_EQUAL(option, teststr + 8);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 1);
  CU_ASSERT_PTR_EQUAL(option, NULL);
}

static void
t_iterate_option4(void) {
  /* CoAP PDU with token, options, and data */
  uint8_t teststr[] ALIGNED(8) = {
    't', 'o', 'k', 0x13,
    'o',  'p',  't',  0x00, 0xd1, 0x10, 'x', 0xff,
    'd',  'a',  't',  'a'
  };

  coap_pdu_t pdu = {
    .max_size = TEST_MAX_SIZE,
    .token_length = 3,
    .token = teststr,
    .used_size = sizeof(teststr)
  };
  coap_opt_iterator_t oi, *result;
  coap_opt_t *option;

  result = coap_option_iterator_init(&pdu, &oi, COAP_OPT_ALL);

  CU_ASSERT_PTR_EQUAL(result, &oi);
  CU_ASSERT(oi.bad == 0);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 0);
  CU_ASSERT(oi.type == 1);
  CU_ASSERT_PTR_EQUAL(option, teststr + 3);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 0);
  CU_ASSERT(oi.type == 1);
  CU_ASSERT_PTR_EQUAL(option, teststr + 7);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 0);
  CU_ASSERT(oi.type == 30);
  CU_ASSERT_PTR_EQUAL(option, teststr + 8);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 1);
  CU_ASSERT_PTR_EQUAL(option, NULL);
}

static void
t_iterate_option5(void) {
  /* CoAP PDU with malformed option */
  uint8_t teststr[] ALIGNED(8) = {
    0x52, 'o', 'p', 0xee,
    0x12, 0x03, 0x00
  };

  coap_pdu_t pdu = {
    .max_size = TEST_MAX_SIZE,
    .token_length = 0,
    .token = teststr,
    .used_size = sizeof(teststr)
  };
  coap_opt_iterator_t oi, *result;
  coap_opt_t *option;

  result = coap_option_iterator_init(&pdu, &oi, COAP_OPT_ALL);

  CU_ASSERT_PTR_EQUAL(result, &oi);
  CU_ASSERT(oi.bad == 0);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 0);
  CU_ASSERT(oi.type == 5);
  CU_ASSERT_PTR_EQUAL(option, teststr);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 1);
  CU_ASSERT_PTR_EQUAL(option, NULL);
}

static void
t_iterate_option6(void) {
  /* option filter */
  /* CoAP PDU with token, options, and data */
  uint8_t teststr[] ALIGNED(8) = {
    0x80, 0x20, 0x00, 0x00,
    0xc0, 0x00
  };

  coap_pdu_t pdu = {
    .max_size = TEST_MAX_SIZE,
    .token_length = 0,
    .token = teststr,
    .used_size = sizeof(teststr)
  };
  coap_opt_iterator_t oi, *result;
  coap_opt_t *option;
  coap_opt_filter_t filter;

  coap_option_filter_clear(filter);
  coap_option_setb(filter, 10);        /* option nr 10 only */
  result = coap_option_iterator_init(&pdu, &oi, filter);

  CU_ASSERT_PTR_EQUAL(result, &oi);
  CU_ASSERT(oi.bad == 0);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 0);
  CU_ASSERT(oi.type == 10);
  CU_ASSERT_PTR_EQUAL(option, teststr + 1);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 0);
  CU_ASSERT(oi.type == 10);
  CU_ASSERT_PTR_EQUAL(option, teststr + 2);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 0);
  CU_ASSERT(oi.type == 10);
  CU_ASSERT_PTR_EQUAL(option, teststr + 3);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 1);
  CU_ASSERT_PTR_EQUAL(option, NULL);
}

static void
t_iterate_option7(void) {
  /* option filter */
  uint8_t teststr[] ALIGNED(8) = {
    0x80, 0x20, 0x00, 0x00,
    0xc0, 0x00, 0x10, 0x10, 0x00
  };

  coap_pdu_t pdu = {
    .max_size = TEST_MAX_SIZE,
    .token_length = 0,
    .token = teststr,
    .used_size = sizeof(teststr)
  };
  coap_opt_iterator_t oi, *result;
  coap_opt_t *option;
  coap_opt_filter_t filter;

  /* search options nr 8 and 22 */
  coap_option_filter_clear(filter);
  coap_option_setb(filter, 8);
  coap_option_setb(filter, 22);
  result = coap_option_iterator_init(&pdu, &oi, filter);

  CU_ASSERT_PTR_EQUAL(result, &oi);
  CU_ASSERT(oi.bad == 0);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 0);
  CU_ASSERT(oi.type == 8);
  CU_ASSERT_PTR_EQUAL(option, teststr);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 0);
  CU_ASSERT(oi.type == 22);
  CU_ASSERT_PTR_EQUAL(option, teststr + 4);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 0);
  CU_ASSERT(oi.type == 22);
  CU_ASSERT_PTR_EQUAL(option, teststr + 5);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 1);
  CU_ASSERT_PTR_EQUAL(option, NULL);
}

static void
t_iterate_option8(void) {
  /* option filter */
  uint8_t teststr[] ALIGNED(8) = {
    0x80, 0x20, 0x00, 0x00,
    0xc0, 0x00, 0x10, 0x10, 0x00
  };

  coap_pdu_t pdu = {
    .max_size = TEST_MAX_SIZE,
    .token_length = 0,
    .token = teststr,
    .used_size = sizeof(teststr)
  };
  coap_opt_iterator_t oi, *result;
  coap_opt_t *option;
  coap_opt_filter_t filter;

  /* search option nr 36 */
  coap_option_filter_clear(filter);
  coap_option_setb(filter, 36);
  result = coap_option_iterator_init(&pdu, &oi, filter);

  CU_ASSERT_PTR_EQUAL(result, &oi);
  CU_ASSERT(oi.bad == 0);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 1);
  CU_ASSERT_PTR_EQUAL(option, NULL);
}

static void
t_iterate_option9(void) {
  /* options filter: option number too large for filter */
  uint8_t teststr[] ALIGNED(8) = {
    0x80, 0x20, 0x00, 0x00,
    0xc0, 0x00, 0x10, 0x10, 0x00
  };

  coap_pdu_t pdu = {
    .max_size = TEST_MAX_SIZE,
    .token_length = 0,
    .token = teststr,
    .used_size = sizeof(teststr)
  };
  coap_opt_iterator_t oi, *result;
  coap_opt_t *option;
  coap_opt_filter_t filter;

  /* search option nr 100 */
  coap_option_filter_clear(filter);
  coap_option_setb(filter, 100);
  result = coap_option_iterator_init(&pdu, &oi, filter);

  CU_ASSERT_PTR_EQUAL(result, &oi);
  CU_ASSERT(oi.bad == 0);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 1);
  CU_ASSERT_PTR_EQUAL(option, NULL);
}

static void
t_iterate_option10(void) {
  /* options filter: option numbers in PDU exceed filter size */
  uint8_t teststr[] ALIGNED(8) = {
    0x80, 0x20, 0x00, 0x00,
    0xd0, 0x26, 0xe0, 0x10, 0x00
  };

  coap_pdu_t pdu = {
    .max_size = TEST_MAX_SIZE,
    .token_length = 0,
    .token = teststr,
    .used_size = sizeof(teststr)
  };
  coap_opt_iterator_t oi, *result;
  coap_opt_t *option;
  coap_opt_filter_t filter;

  /* search option nr 61 */
  coap_option_filter_clear(filter);
  coap_option_setb(filter, 61);
  result = coap_option_iterator_init(&pdu, &oi, filter);

  CU_ASSERT_PTR_EQUAL(result, &oi);
  CU_ASSERT(oi.bad == 0);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 0);
  CU_ASSERT_PTR_EQUAL(option, teststr + 4);

  option = coap_option_next(&oi);
  CU_ASSERT(oi.bad == 1);
  CU_ASSERT_PTR_EQUAL(option, NULL);
}

/************************************************************************
 ** filter tests
 ************************************************************************/

static void
t_filter_option1(void) {
  coap_opt_filter_t filter;

  coap_option_filter_clear(filter);

  CU_ASSERT(coap_option_filter_set(filter, 0) == 1);
  CU_ASSERT(coap_option_filter_set(filter, 37) == 1);
  CU_ASSERT(coap_option_filter_set(filter, 37) == 1);
  CU_ASSERT(coap_option_filter_set(filter, 43) == 1);
  CU_ASSERT(coap_option_filter_set(filter, 290) == 1);
  CU_ASSERT(coap_option_filter_set(filter, 65535) == 1);

  CU_ASSERT(coap_option_filter_get(filter, 0) == 1);
  CU_ASSERT(coap_option_filter_get(filter, 37) == 1);
  CU_ASSERT(coap_option_filter_get(filter, 43) == 1);
  CU_ASSERT(coap_option_filter_get(filter, 290) == 1);
  CU_ASSERT(coap_option_filter_get(filter, 65535) == 1);

  CU_ASSERT(coap_option_filter_unset(filter, 37) == 1);

  CU_ASSERT(coap_option_filter_get(filter, 0) == 1);
  CU_ASSERT(coap_option_filter_get(filter, 43) == 1);
  CU_ASSERT(coap_option_filter_get(filter, 290) == 1);
  CU_ASSERT(coap_option_filter_get(filter, 65535) == 1);

  CU_ASSERT(coap_option_filter_get(filter, 37) == 0);
  CU_ASSERT(coap_option_filter_get(filter, 89) == 0);
}

static void
t_filter_option2(void) {
  coap_opt_filter_t filter;
  int s;

  coap_option_filter_clear(filter);

  /* fill all COAP_OPT_FILTER_SHORT slots */
  for (s = 0; s < COAP_OPT_FILTER_SHORT; s++) {
    CU_ASSERT(coap_option_filter_set(filter, s));
  }

  /* adding a short option type must fail */
  CU_ASSERT(coap_option_filter_set(filter, COAP_OPT_FILTER_SHORT) == 0);

  /* adding a long option type must succeed */
  CU_ASSERT(coap_option_filter_set(filter, 256) == 1);
}

static void
t_filter_option3(void) {
  coap_opt_filter_t filter;
  int l;

  coap_option_filter_clear(filter);

  /* set COAP_OPT_FILTER_LONG long filters */
  for (l = 0; l < COAP_OPT_FILTER_LONG; l++) {
    CU_ASSERT(coap_option_filter_set(filter, 256 + l) == 1);
  }

  /* the next must fail and must not be found */
  CU_ASSERT(coap_option_filter_set(filter, 256 + COAP_OPT_FILTER_LONG) == 0);
  CU_ASSERT(coap_option_filter_get(filter, 256 + COAP_OPT_FILTER_LONG) == 0);

  /* remove one item */
  CU_ASSERT(coap_option_filter_unset(filter, 256) == 1);
  CU_ASSERT(coap_option_filter_get(filter, 256) == 0);

  /* now, storing a new filter must succeed */
  CU_ASSERT(coap_option_filter_set(filter, 256 + COAP_OPT_FILTER_LONG) == 1);
  CU_ASSERT(coap_option_filter_get(filter, 256 + COAP_OPT_FILTER_LONG) == 1);

  /* and all other items must be available as well */
  for (l = 0; l < COAP_OPT_FILTER_LONG; l++) {
    CU_ASSERT(coap_option_filter_get(filter, 256 + l + 1) == 1);
  }

  /* set COAP_OPT_FILTER_SHORT short filters */
  for (l = 0; l < COAP_OPT_FILTER_SHORT; l++) {
    CU_ASSERT(coap_option_filter_set(filter, l) == 1);
  }

  /* the next must fail and must not be found */
  CU_ASSERT(coap_option_filter_set(filter, COAP_OPT_FILTER_SHORT) == 0);
  CU_ASSERT(coap_option_filter_get(filter, COAP_OPT_FILTER_SHORT) == 0);

  /* remove one item */
  CU_ASSERT(coap_option_filter_unset(filter, 0) == 1);
  CU_ASSERT(coap_option_filter_get(filter, 0) == 0);

  /* now, storing a new filter must succeed */
  CU_ASSERT(coap_option_filter_set(filter, COAP_OPT_FILTER_SHORT) == 1);
  CU_ASSERT(coap_option_filter_get(filter, COAP_OPT_FILTER_SHORT) == 1);

  /* and all other items must be available as well */
  for (l = 0; l < COAP_OPT_FILTER_SHORT; l++) {
    CU_ASSERT(coap_option_filter_get(filter, l + 1) == 1);
  }
}

/************************************************************************
 ** initialization
 ************************************************************************/

CU_pSuite
t_init_option_tests(void) {
  CU_pSuite suite[5];

  suite[0] = CU_add_suite("option parser", NULL, NULL);
  if (!suite[0]) {                        /* signal error */
    fprintf(stderr, "W: cannot add option parser test suite (%s)\n",
            CU_get_error_msg());

    return NULL;
  }

#define OPTION_TEST(n,s)                                       \
  if (!CU_add_test(suite[0], s, t_parse_option##n)) {          \
    fprintf(stderr, "W: cannot add option parser test (%s)\n", \
            CU_get_error_msg());                               \
  }

  OPTION_TEST(1, "parse option #1");
  OPTION_TEST(2, "parse option #2");
  OPTION_TEST(3, "parse option #3");
  OPTION_TEST(4, "parse option #4");
  OPTION_TEST(5, "parse option #5");
  OPTION_TEST(6, "parse option #6");
  OPTION_TEST(7, "parse option #7");
  OPTION_TEST(8, "parse option #8");
  OPTION_TEST(9, "parse option #9");
  OPTION_TEST(10, "parse option #10");
  OPTION_TEST(11, "parse option #11");
  OPTION_TEST(12, "parse option #12");
  OPTION_TEST(13, "parse option #13");
  OPTION_TEST(14, "parse option #14");

  if ((suite[1] = CU_add_suite("option encoder", NULL, NULL))) {
#define OPTION_ENCODER_TEST(n,s)                                  \
    if (!CU_add_test(suite[1], s, t_encode_option##n)) {          \
      fprintf(stderr, "W: cannot add option encoder test (%s)\n", \
              CU_get_error_msg());                                \
    }

    OPTION_ENCODER_TEST(1, "encode option #1");
    OPTION_ENCODER_TEST(2, "encode option #2");
    OPTION_ENCODER_TEST(3, "encode option #3");
    OPTION_ENCODER_TEST(4, "encode option #4");
    OPTION_ENCODER_TEST(5, "encode option #5");
    OPTION_ENCODER_TEST(6, "encode option #6");
    OPTION_ENCODER_TEST(7, "encode option #7");
    OPTION_ENCODER_TEST(8, "encode option #8");
    OPTION_ENCODER_TEST(9, "encode option #9");

  } else {
    fprintf(stderr, "W: cannot add option encoder test suite (%s)\n",
            CU_get_error_msg());
  }

  if ((suite[2] = CU_add_suite("option accessors", NULL, NULL))) {
#define OPTION_ACCESSOR_TEST(n,s)                                           \
    if (!CU_add_test(suite[2], s, t_access_option##n)) {                    \
      fprintf(stderr, "W: cannot add option accessor function test (%s)\n", \
              CU_get_error_msg());                                          \
    }

    OPTION_ACCESSOR_TEST(1, "access option #1");
    OPTION_ACCESSOR_TEST(2, "access option #2");
    OPTION_ACCESSOR_TEST(3, "access option #3");
    OPTION_ACCESSOR_TEST(4, "access option #4");
    OPTION_ACCESSOR_TEST(5, "access option #5");
    OPTION_ACCESSOR_TEST(6, "access option #6");
    OPTION_ACCESSOR_TEST(7, "access option #7");

  } else {
    fprintf(stderr, "W: cannot add option acessor function test suite (%s)\n",
            CU_get_error_msg());
  }

  if ((suite[3] = CU_add_suite("option iterator", NULL, NULL))) {
#define OPTION_ITERATOR_TEST(n,s)                                  \
    if (!CU_add_test(suite[3], s, t_iterate_option##n)) {          \
      fprintf(stderr, "W: cannot add option iterator test (%s)\n", \
              CU_get_error_msg());                                 \
    }

    OPTION_ITERATOR_TEST(1, "option iterator #1");
    OPTION_ITERATOR_TEST(2, "option iterator #2");
    OPTION_ITERATOR_TEST(3, "option iterator #3");
    OPTION_ITERATOR_TEST(4, "option iterator #4");
    OPTION_ITERATOR_TEST(5, "option iterator #5");
    OPTION_ITERATOR_TEST(6, "option iterator #6");
    OPTION_ITERATOR_TEST(7, "option iterator #7");
    OPTION_ITERATOR_TEST(8, "option iterator #8");
    OPTION_ITERATOR_TEST(9, "option iterator #9");
    OPTION_ITERATOR_TEST(10, "option iterator #10");

  } else {
    fprintf(stderr, "W: cannot add option iterator test suite (%s)\n",
            CU_get_error_msg());
  }

  if ((suite[4] = CU_add_suite("option filter", NULL, NULL))) {
#define OPTION_FILTER_TEST(n,s)                                  \
    if (!CU_add_test(suite[4], s, t_filter_option##n)) {         \
      fprintf(stderr, "W: cannot add option filter test (%s)\n", \
              CU_get_error_msg());                               \
    }

    OPTION_FILTER_TEST(1, "option filter #1");
    OPTION_FILTER_TEST(2, "option filter #2");
    OPTION_FILTER_TEST(3, "option filter #3");

  } else {
    fprintf(stderr, "W: cannot add option filter test suite (%s)\n",
            CU_get_error_msg());
  }

  return suite[0];
}

