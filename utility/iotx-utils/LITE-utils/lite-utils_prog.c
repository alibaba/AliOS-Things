/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "lite-utils_internal.h"

int main(void)
{
    unittest_string_utils();
    unittest_json_token();
    unittest_json_parser();

    return 0;
}
