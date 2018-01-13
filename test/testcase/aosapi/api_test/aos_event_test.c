/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <k_api.h>
#include <aos/kernel.h>

#include <yunit.h>
#include <errno.h>

#define EVENT_INIT_FLAG     0x0F0F0F0F
#define EVENT_FLAG_1        0x00000001
#define EVENT_FLAG_2        0x01000F01
#define EVENT_FLAG_3        0x000000F0

static aos_event_t  event;
#ifdef MULTITASK_UNDER_SMP
static aos_task_t   task1;
static aos_task_t   task2;
static aos_sem_t    sem;
#endif /* MULTITASK_UNDER_SMP */

static void CASE_aosapi_kernel_event_new_free()
{
    kstat_t  ret;

    /* create event */

    ret = aos_event_new(&event, EVENT_INIT_FLAG);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /* destory event */

    aos_event_free(&event);
    YUNIT_ASSERT_MSG(event.hdl==NULL, "ret=%d", 0);
}

static void CASE_aosapi_kernel_event_op_and()
{
    kstat_t  ret;
    uint32_t actl_flags;

    /* create event */
    
    ret = aos_event_new(&event, EVENT_INIT_FLAG);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /*the event now should has event flag value as EVENT_INIT_FLAG(0x0F0F0F0F) */

    /* try to get flag EVENT_FLAG_1(0x00000001) with AND operation should success */
    ret = aos_event_get(&event, EVENT_FLAG_1, RHINO_AND, &actl_flags,AOS_NO_WAIT);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /* try to get flag EVENT_FLAG_2(0x01000F01) with AND operation should success */
    ret = aos_event_get(&event, EVENT_FLAG_2, RHINO_AND, &actl_flags, AOS_NO_WAIT);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /* try to get flag EVENT_FLAG_3(0x000000F0) with AND operation should fail */
    ret = aos_event_get(&event, EVENT_FLAG_3, RHINO_AND, &actl_flags, AOS_NO_WAIT);
    YUNIT_ASSERT_MSG(ret!=(RHINO_SUCCESS), "ret=%d", ret);    

    /* destory event */

    aos_event_free(&event);
    YUNIT_ASSERT_MSG(event.hdl==NULL, "ret=%d", 0);
}

static void CASE_aosapi_kernel_event_op_andclear()
{
    kstat_t  ret;
    uint32_t actl_flags;

    /* create event */
    
    ret = aos_event_new(&event, EVENT_INIT_FLAG);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /*the event now should has event flag value as EVENT_INIT_FLAG(0x0F0F0F0F) */

    /* try to get flag EVENT_FLAG_1(0x00000001) with AND_CLEAR operation should success */
    ret = aos_event_get(&event, EVENT_FLAG_1, RHINO_AND_CLEAR, &actl_flags,AOS_NO_WAIT);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /* now the event should has flag 0x0F0F0F0E */

    /* 
     * try to get flag EVENT_FLAG_2(0x01000F01) with AND_CLEAR operation should fail,
     * because (0x0F0F0F0E & 0x01000F01) != 0x01000F01
     */
    ret = aos_event_get(&event, EVENT_FLAG_2, RHINO_AND, &actl_flags, AOS_NO_WAIT);
    YUNIT_ASSERT_MSG(ret!=(RHINO_SUCCESS), "ret=%d", ret);

    /* now the event should still has flag 0x0F0F0F0E */

    /* try to get flag EVENT_FLAG_3(0x000000F0) with AND operation should fail */
    ret = aos_event_get(&event, EVENT_FLAG_3, RHINO_AND, &actl_flags, AOS_NO_WAIT);
    YUNIT_ASSERT_MSG(ret!=(RHINO_SUCCESS), "ret=%d", ret);    

    /* destory event */

    aos_event_free(&event);
    YUNIT_ASSERT_MSG(event.hdl==NULL, "ret=%d", 0);
}

static void CASE_aosapi_kernel_event_op_or()
{
    kstat_t  ret;
    uint32_t actl_flags;

    /* create event */
    
    ret = aos_event_new(&event, EVENT_INIT_FLAG);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /*set event flag with OR operation */
    ret = aos_event_set(&event, EVENT_FLAG_1, RHINO_OR);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /*the event now should has event flag value as EVENT_INIT_FLAG|EVENT_FLAG_1 = 0x0F0F0F0F */

    /* try to get flag EVENT_FLAG_1(0x00000001) with OR operation should success */
    ret = aos_event_get(&event, EVENT_FLAG_1, RHINO_OR, &actl_flags, AOS_NO_WAIT);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /* try to get flag EVENT_FLAG_2(0x01000F01) with OR operation should success */
    ret = aos_event_get(&event, EVENT_FLAG_2, RHINO_OR, &actl_flags, AOS_NO_WAIT);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /* try to get flag EVENT_FLAG_3(0x000000F0) with OR operation should fail */
    ret = aos_event_get(&event, EVENT_FLAG_3, RHINO_OR, &actl_flags, AOS_NO_WAIT);
    YUNIT_ASSERT_MSG(ret!=(RHINO_SUCCESS), "ret=%d", ret);    

    /* destory event */

    aos_event_free(&event);
    YUNIT_ASSERT_MSG(event.hdl==NULL, "ret=%d", 0);
}

static void CASE_aosapi_kernel_event_op_orclear()
{
    kstat_t  ret;
    uint32_t actl_flags;

    /* create event */
    
    ret = aos_event_new(&event, EVENT_INIT_FLAG);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /*the event now should has event flag value as EVENT_INIT_FLAG|EVENT_FLAG_1 = 0x0F0F0F0F */

    /* try to get flag EVENT_FLAG_1(0x00000001) with OR_CLEAR operation should success */
    ret = aos_event_get(&event, EVENT_FLAG_1, RHINO_OR_CLEAR, &actl_flags,AOS_NO_WAIT);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /* now the event should has flag 0x0F0F0F0E*/

    /*
     * try to get flag EVENT_FLAG_2(0x01000F01) with OR_CLEAR operation should success,
     * because (0x0F0F0F0E & 0x01000F01) > 0
     */
    ret = aos_event_get(&event, EVENT_FLAG_2, RHINO_OR_CLEAR, &actl_flags, AOS_NO_WAIT);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /* now the event should has flag 0x0E0F000E*/

    /* try to get flag EVENT_FLAG_3(0x000000F0) with OR operation should fail */
    ret = aos_event_get(&event, EVENT_FLAG_3, RHINO_OR, &actl_flags, AOS_NO_WAIT);
    YUNIT_ASSERT_MSG(ret!=(RHINO_SUCCESS), "ret=%d", ret);    

    /* destory event */

    aos_event_free(&event);
    YUNIT_ASSERT_MSG(event.hdl==NULL, "ret=%d", 0);
}

#ifdef MULTITASK_UNDER_SMP

static void event_test_task1_entry(void* arg)
{
    kstat_t  ret;
    uint32_t actl_flags;

    /* try to get flag EVENT_FLAG_3(0x000000F0) with OR operation should fail */
    ret = aos_event_get(&event, EVENT_FLAG_3, RHINO_OR, &actl_flags, AOS_NO_WAIT);
    YUNIT_ASSERT_MSG(ret!=(RHINO_SUCCESS), "ret=%d", ret);    

    /* 
     * try to get flag EVENT_FLAG_3(0x000000F0) with OR operation should wait here,
     * task2 will set the flags with 0x000000F0, and then task1 will continue
     */
    ret = aos_event_get(&event, EVENT_FLAG_3, RHINO_OR, &actl_flags, AOS_WAIT_FOREVER);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /* give the samphore to let main task continue*/
    aos_sem_signal(&sem);

    /* exit self */
    aos_task_exit(0);
}

static void event_test_task2_entry(void* arg)
{
    kstat_t  ret;
    
    /*set event flag(0x000000F0) with OR operation, this will un-block task1 */

    ret = aos_event_set(&event, EVENT_FLAG_3, RHINO_OR);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /* exit self */
    aos_task_exit(0);    
}

static void CASE_aosapi_kernel_event_op_multask(void)
{
    kstat_t  ret;

    /* create event */
    
    ret = aos_event_new(&event, EVENT_INIT_FLAG);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /*create a semphore which will be used for sync multi-task*/
    ret = aos_sem_new(&sem, 0);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);    

    /*the event now should has event flag value as EVENT_INIT_FLAG = 0x0F0F0F0F */

    /* create task1 */

    ret = aos_task_new_ext(&task1, "event_test_task1", event_test_task1_entry,
                           0, 4096, AOS_DEFAULT_APP_PRI);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /* create task2, which has a lower pri then task1, this make sure task1 will run first */

    ret = aos_task_new_ext(&task2, "event_test_task2", event_test_task2_entry,
                           0, 4096, AOS_DEFAULT_APP_PRI+1);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /* wait for task1 to give samphore */
    ret = aos_sem_wait(&sem, AOS_WAIT_FOREVER);
    YUNIT_ASSERT_MSG(ret==(RHINO_SUCCESS), "ret=%d", ret);

    /* destory samphore */
    aos_sem_free(&sem);
    
    /* destory event */

    aos_event_free(&event);
    YUNIT_ASSERT_MSG(event.hdl==NULL, "ret=%d", 0);
}

#endif /* MULTITASK_UNDER_SMP */

void aosapi_kernel_event_test_entry(yunit_test_suite_t *suite)
{
    yunit_add_test_case(suite, "kernel.event.new_free",     CASE_aosapi_kernel_event_new_free);
    yunit_add_test_case(suite, "kernel.event.op_and",       CASE_aosapi_kernel_event_op_and);
    yunit_add_test_case(suite, "kernel.event.op_andclear",  CASE_aosapi_kernel_event_op_andclear);
    yunit_add_test_case(suite, "kernel.event.op_or",        CASE_aosapi_kernel_event_op_or);
    yunit_add_test_case(suite, "kernel.event.op_orclear",   CASE_aosapi_kernel_event_op_orclear);
#ifdef MULTITASK_UNDER_SMP    
    yunit_add_test_case(suite, "kernel.event.op_multask",   CASE_aosapi_kernel_event_op_multask);
#endif
}


