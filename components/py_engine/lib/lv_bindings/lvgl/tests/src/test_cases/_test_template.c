#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void);
void tearDown(void);

void test_func_1(void);

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

void test_func_1(void)
{
  TEST_ASSERT_EQUAL(actual, expected);
}

#endif
