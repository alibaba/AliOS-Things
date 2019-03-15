/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*test_case_t)(void);

typedef struct {
    const char  *name;
    test_case_t  fn;
} test_case_map_t;

extern test_case_map_t test_fw_map[];
extern void vc_port_printf(char *f, ...);
extern uint32_t test_case_success;
extern uint32_t test_case_fail;
extern uint32_t test_case_check_err;

void    next_test_case_notify(void);
void    next_test_case_wait(void);
uint8_t test_case_register(test_case_t *case_runner);
void    test_case_run(void);
void    test_case_unregister(void);
void    test_case_init(void);
void    test_case_cleanup(void);
void    test_case_critical_enter(void);
void    test_case_critical_exit(void);

#ifdef _MSC_VER
#define printf vc_port_printf
#else
#include <stdio.h>
#endif /* _MSC_VER */

#define PASS 0
#define FAIL 1

#define PRINT_PASS(s) printf("%-30s test [PASS %d]\n", s, (int)test_case_success)
#define PRINT_FAIL(s) printf("%-30s test [FAIL %d]\n", s, (int)test_case_fail)
#define PRINT_RESULT(s, f) do { if (f == PASS) { PRINT_PASS(s); } else { PRINT_FAIL(s); }} while (0)

#define TEST_FW_VAL_CHK(module_name,value) do {if ((int)(value) == 0) \
        { \
            test_case_critical_enter(); \
            test_case_check_err++;  \
            test_case_critical_exit(); \
            printf("%s test is [FAIL %d], func %s, line %d\n", \
                   module_name, (int)++test_case_check_err, __FUNCTION__, __LINE__); \
        }  \
    } while (0)


#ifdef __cplusplus
}
#endif
#endif /* TEST_FRAMEWORK_H */
