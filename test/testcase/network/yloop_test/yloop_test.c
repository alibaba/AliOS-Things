/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>

#include <aos/kernel.h>
#include <aos/aos.h>

#include <yunit.h>
#include <yts.h>

#define TASK1 "task1"
#define TASK2 "task2"

struct task_cookie {
    aos_loop_t loop;
    int flag;
    const char *name;
};

#define T_ACTION2_FLAG      0x01
#define T_WORK_DONE_FLAG    0x02
#define T_WORK_FLAG         0x04
#define T_ACTION_TERM_FLAG  0x08
#define T_FILTER_ALL_FLAG   0x10
#define T_FILTER_ONE_FLAG   0x20
#define T_ALL 0x3f

#define T_FILTER_FLAG       0x01
static uint32_t done_flag;
static uint32_t unexpected_flag;

#define TYPE_TEST_1 (EV_USER + 1)
#define TYPE_TEST_2 (EV_USER + 2)

static void filter_all(input_event_t *event, void *private_data)
{
    done_flag |= T_FILTER_ALL_FLAG;
}

static void filter_one(input_event_t *event, void *private_data)
{
    if (event->type != TYPE_TEST_1)
        unexpected_flag |= T_FILTER_FLAG;
    else
        done_flag |= T_FILTER_ONE_FLAG;
}

static void app_delayed_action(void *arg)
{
    struct task_cookie *cookie = arg;

    YUNIT_ASSERT(strcmp(aos_task_name(), cookie->name) == 0);
    if (cookie->flag == 0) {
        aos_post_delayed_action(1000, app_delayed_action, arg);
    }
    else if (cookie->flag == 1) {
        aos_schedule_call(app_delayed_action, arg);
    }
    else {
        aos_loop_exit();
    }
    cookie->flag ++;
}

static void app_delayed_action2(void *arg)
{
    struct task_cookie *cookie = arg;

    YUNIT_ASSERT(strcmp(aos_task_name(), cookie->name) == 0);

    done_flag |= T_ACTION2_FLAG;
}

static struct task_cookie cookie1 = {
    .name = TASK1,
};

static struct task_cookie cookie2 = {
    .name = TASK2,
};

static void work_done(void *arg)
{
    struct task_cookie *cookie = arg;
    YUNIT_ASSERT(strcmp(aos_task_name(), cookie->name) == 0);
    aos_loop_exit();

    done_flag |= T_WORK_DONE_FLAG;
}

static void work_not_exec(void *arg)
{
    YUNIT_ASSERT(0);
}

static void mywork(void *arg)
{
    struct task_cookie *cookie = arg;
    YUNIT_ASSERT(strcmp(aos_task_name(), cookie->name) != 0);

    done_flag |= T_WORK_FLAG;
}

static void app_main_entry(void *arg)
{
    struct task_cookie *cookie = arg;
    YUNIT_ASSERT(strcmp(aos_task_name(), cookie->name) == 0);

    cookie->loop = aos_current_loop();

    aos_post_delayed_action(1000, app_delayed_action, cookie);

    aos_loop_run();
    aos_loop_run();

    YUNIT_ASSERT(aos_loop_schedule_work(0, NULL, NULL, NULL, NULL) == 0);
    YUNIT_ASSERT(aos_loop_schedule_work(0, mywork, arg, NULL, arg) != 0);
    YUNIT_ASSERT(aos_loop_schedule_work(1000, mywork, arg, work_done, arg) != 0);

    void *w = aos_loop_schedule_work(500, work_not_exec, NULL, NULL, NULL);
    YUNIT_ASSERT(w != 0);
    aos_cancel_work(w, work_not_exec, NULL);
    aos_cancel_work(NULL, work_not_exec, NULL);

    aos_loop_run();
    aos_task_exit(0);
}

static void action_after_terminated(void *arg)
{
    printf("%s:%d - %s\n", __func__, __LINE__, aos_task_name());
    aos_loop_exit();

    done_flag |= T_ACTION_TERM_FLAG;
}

static void app_second_entry(void *arg)
{
    struct task_cookie *cookie = arg;
    YUNIT_ASSERT(strcmp(aos_task_name(), cookie->name) == 0);
    int i;

    for (i=0;i<1000;i++) {
        aos_loop_init();
        aos_loop_destroy();
        aos_loop_destroy();
    }

    aos_loop_init();

    cookie->loop = aos_current_loop();
    aos_post_delayed_action(1000, app_delayed_action, cookie);

    aos_loop_run();

    YUNIT_ASSERT(cookie1.loop != NULL);
    aos_loop_schedule_call(cookie1.loop, app_delayed_action2, &cookie1);
    aos_loop_destroy();

    aos_schedule_call(action_after_terminated, NULL);

    aos_task_exit(0);
}

static void test_simple_case(void)
{
    int ret;

    ret = aos_register_event_filter(EV_ALL, NULL, NULL);
    YUNIT_ASSERT(ret < 0);
    ret = aos_schedule_call(NULL, NULL);
    YUNIT_ASSERT(ret < 0);
    ret = aos_post_delayed_action(1000, NULL, NULL);
    YUNIT_ASSERT(ret < 0);

    aos_register_event_filter(EV_ALL, filter_all, NULL);
    aos_register_event_filter(TYPE_TEST_1, filter_one, NULL);

    aos_task_new(TASK1, app_main_entry, &cookie1, 8192);
    aos_task_new(TASK2, app_second_entry, &cookie2, 8192);

    aos_post_event(TYPE_TEST_1, 0, 0);
    aos_post_event(TYPE_TEST_2, 0, 0);

    check_cond_wait(done_flag == T_ALL, 10);
    YUNIT_ASSERT(unexpected_flag == 0);

    YUNIT_ASSERT(0 == aos_unregister_event_filter(EV_ALL, filter_all, NULL));
    YUNIT_ASSERT(0 == aos_unregister_event_filter(TYPE_TEST_1, filter_one, NULL));
}

static int init(void)
{
    return 0;
}

static int cleanup(void)
{
    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

static yunit_test_case_t aos_yloop_testcases[] = {
    { "simple", test_simple_case },
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "yloop", init, cleanup, setup, teardown, aos_yloop_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_yloop(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_yloop);
