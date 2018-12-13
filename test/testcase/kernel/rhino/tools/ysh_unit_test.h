/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef YSH_TEST_H
#define YSH_TEST_H

#define YSH_VAL_CHK(value) do {if ((int)(value) == 0) \
        { \
            test_case_critical_enter(); \
            test_case_check_err++;  \
            test_case_critical_exit(); \
            printf("ysh test is [FAIL %d], func %s, line %d\n", \
                   ++test_case_check_err, __FUNCTION__, __LINE__); \
        }  \
    } while (0)

void ysh_cmd_test(void);
#endif /* YSH_TEST_H */
