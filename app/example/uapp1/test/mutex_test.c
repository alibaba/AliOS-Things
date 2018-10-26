#include <k_api.h>
#include <hal/hal.h>

#define LOOP_ROUND 50
#define STACK_SIZE 0x400

static char stack[STACK_SIZE];

static ktask_t mutex_task_obj;
static kmutex_t test_mutex;

static bool loop_exit;
static bool del_mutex;
static int mutex_cnt;

static void second_task_run(void *arg)
{
    while (1) {
        if (loop_exit)
            break;

        krhino_mutex_lock(&test_mutex, RHINO_WAIT_FOREVER);
        mutex_cnt++;
        krhino_task_sleep(5);
        krhino_mutex_unlock(&test_mutex);
    }

    del_mutex = 1;

    while (1) {
        krhino_task_sleep(1000);
    }
}

int mutex_test(void)
{
    kstat_t stat;

    loop_exit = 0;
    del_mutex = 0;
    mutex_cnt = 0;

    krhino_mutex_create(&test_mutex, "test_mutex");

    stat = krhino_utask_create(&mutex_task_obj, "mutex_test_task", 0,
                        AOS_DEFAULT_APP_PRI,
                        (tick_t)0, stack,
                        STACK_SIZE,
                        STACK_SIZE,
                        (task_entry_t)second_task_run, 1);

    if (stat != RHINO_SUCCESS) {
        printf("%s create mutex_test task failed, ret code %d\r\n",
                __func__, stat);
        goto out;
    }

    for (int i = 0; i < LOOP_ROUND; i++) {
        krhino_mutex_lock(&test_mutex, RHINO_WAIT_FOREVER);
        krhino_task_sleep(2);
        krhino_mutex_unlock(&test_mutex);
    }

    loop_exit = 1;

    while (!del_mutex) {
        krhino_task_sleep(1);
    }

out:
    krhino_mutex_del(&test_mutex);

    return (mutex_cnt == LOOP_ROUND)? 0: -1;
}

