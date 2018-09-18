/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <aos/aos.h>

#include "include/real_time_test.h"

#define TEST_MAIN_STACKSIZE 256

#define TEST_MAIN_NAME "rt_main"
#define TEST_MAIN_PRI  (TEST_TASK_PRIORITY - 3)

static ktask_t     test_main_tcb;
static cpu_stack_t test_main_stack[TEST_MAIN_STACKSIZE];
static ksem_t      wait_main_end;

static intrpt_callback_t test_case[] = { 
#if ENABLE_INTRPT_RESPOND_WAKEUP 
    rttest_intrpt_respond,        /* t1 */
#else
    NULL,
#endif
#if ENABLE_INTRPT_PREEMPT   
    rttest_intrpt_preempt,        /* t2 */
#else
    NULL,
#endif

#if ENABLE_INTRPT_RESPOND_WAKEUP 
    rttest_intrpt_wakeuptask,     /* t3 */
#else
    NULL,
#endif

    rttest_schedule_yield,         /* t4 */
    rttest_schedule_preempt,       /* t5 */
    rttest_schedule_resume_active, /* t6 */
    rttest_sync_mutex,             /* t7 */
    rttest_sync_sem,               /* t8 */ 
    rttest_shuffling_sem,          /* t9 */
    rttest_shuffling_mutex,        /* t10 */
    rttest_sem_active,             /* t11 */
    rttest_mutex_active,           /* t12 */
    rttest_sync_sem_rrwp_task,     /* t13 */

    rttest_message_queue,          /* t14 */
    rttest_message_rrwp_task,      /* t15 */

    rttest_memory_blk_alloc,       /* t16 t18*/
    rttest_memory_tlf_alloc,       /* t17 t19*/
};

uint32_t id_to_index[] = {0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,15,16};

static void rttest_entry(void * arg)
{
    uint32_t id = (uint32_t)arg;
    uint32_t index;
    
    if(id >= sizeof(id_to_index)/sizeof(id_to_index[0])) {
        printf("the test id is too large\n");
        krhino_sem_give(&wait_main_end);
        return;
    }

    index = id_to_index[id];

    rttest_aux_measure_waste();
    
    if (id == 0) {
        rttest_aux_show_sysconfig();
        rttest_aux_show_result_header();
        for (uint32_t i=0; i< sizeof(test_case)/sizeof(intrpt_callback_t); i++) {
            if(test_case[i] != NULL) {
                test_case[i]();
            }
        }
        rttest_aux_show_result_end();
        rttest_max_disintrpt();
    }
    else {
        rttest_aux_show_result_header();
        if(test_case[index] != NULL) {
            test_case[index]();
        }
        rttest_aux_show_result_end();
    }

    krhino_sem_give(&wait_main_end);
}

void rttest_main(uint32_t id)
{
    krhino_sem_create(&wait_main_end, "main_sync", 0);

    krhino_task_create(&test_main_tcb, TEST_MAIN_NAME, id, TEST_MAIN_PRI, 50,
                      test_main_stack, TEST_MAIN_STACKSIZE, rttest_entry, 1);
    
    krhino_sem_take(&wait_main_end, RHINO_WAIT_FOREVER);

    krhino_task_del(&test_main_tcb);
    krhino_sem_del(&wait_main_end);
}

uint32_t rttest_func(char *pcWriteBuffer, int xWriteBufferLen, int argc,
                      char **argv)
{
    if (argc == 2 && 0 == strcmp(argv[1], "all")) {
        rttest_main(0);
    } else if (argc == 2 && (('t' == argv[1][0]) ||  ('T' == argv[1][0]))
               && (argv[1][1] >= '0' && argv[1][1] <= '9')) {
        uint32_t id = atoi(&argv[1][1]);
        rttest_main(id);
    } else if (argc == 2 && 0 == strcmp(argv[1], "disint")) {
        rttest_max_disintrpt();
    } else {
        printf("rttest help:\n");
        printf("    rttest all     : run all test case\n");
        printf("    rttest tn      : run test case n\n");
        printf("    rttest disint  : show max intrpt disable time\n");
    }
    return 0;
}

static void rttest_cmd(char *buf, int len, int argc, char **argv)
{
    rttest_func(buf, len, argc, argv);
}

static struct cli_command rttest_cli_cmd[] = {
    { "rttest", "run real time test case", rttest_cmd },
};

void rttest_cli_init(void)
{
    aos_cli_register_commands(&rttest_cli_cmd[0],
                              sizeof(rttest_cli_cmd) / sizeof(struct cli_command));
}
