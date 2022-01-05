/* libcoap unit tests
 *
 * Copyright (C) 2012 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include <CUnit/CUnit.h>

/** Asserts that const pointers actual == expected.
 *  Reports failure with no other action.
 */
#define CU_ASSERT_PTR_EQUAL_C(actual, expected) \
  { CU_assertImplementation(((const void*)(actual) == (const void*)(expected)), __LINE__, ("CU_ASSERT_PTR_EQUAL(" #actual "," #expected ")"), __FILE__, "", CU_FALSE); }

CU_pSuite t_init_option_tests(void);
