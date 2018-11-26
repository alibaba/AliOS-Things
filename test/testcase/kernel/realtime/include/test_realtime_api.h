/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef TEST_REALTIME_API_H
#define TEST_REALTIME_API_H

#ifdef __cplusplus
extern "C" {
#endif

/* test case */
void test_realtime_intrpt_respond(void);
void test_realtime_intrpt_wakeuptask(void);
void test_realtime_intrpt_preempt(void);
void test_realtime_sched_yield(void);
void test_realtime_sched_preempt(void);
void test_realtime_sched_resume_active(void);
void test_realtime_sync_mutex(void);
void test_realtime_sync_sem(void);
void test_realtime_sync_sem_rrwp_task(void);
void test_realtime_shuffling_sem(void);
void test_realtime_shuffling_mutex(void);
void test_realtime_sem_active(void);
void test_realtime_mutex_active(void);
void test_realtime_message_queue(void);
void test_realtime_message_rrwp_task(void);
void test_realtime_memory_blk_alloc(void);
void test_realtime_memory_tlf_alloc(void);

/* test entry */
void test_realtime_main(uint32_t id); /* 0 for all test case*/
void test_realtime_max_disintrpt(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_REALTIME_API_H */
