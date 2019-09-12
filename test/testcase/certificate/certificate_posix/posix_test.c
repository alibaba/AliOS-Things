/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifdef AOS_POSIX

#include <aos/kernel.h>
#include <pthread.h>
#include <semaphore.h>
#include "cutest/cut.h"

#ifndef SYSINFO_ARCH
#define SYSINFO_ARCH        "posix_test"
#endif
#ifndef SYSINFO_MCU
#define SYSINFO_MCU         "posix_test"
#endif
#ifndef SYSINFO_DEVICE_NAME
#define SYSINFO_DEVICE_NAME "posix_test"
#endif
#ifndef SYSINFO_KERNEL_VERSION
#define SYSINFO_KERNEL_VERSION "unknown"
#endif
#ifndef SYSINFO_APP_VERSION
#define SYSINFO_APP_VERSION "posix_test"
#endif
#define SYSINFO_KERNEL      "AOS"

/* posix test */
#define TEST_CONFIG_PTHREAD_ENABLED                  (1)
#define TEST_CONFIG_SEM_ENABLED                      (1)
#define TEST_CONFIG_MUX_ENABLED                      (1)
#define TEST_CONFIG_COND_ENABLED					 (1)


// #if (TEST_CONFIG_MM_ENABLED > 0)
// #define TEST_CONFIG_MALLOC_MAX_SIZE             (1024)
// #define TEST_CONFIG_MALLOC_FREE_TIMES           (100000)
// #endif

// /* task test */
// #define TEST_CONFIG_TASK_ENABLED                (1)
// #if (TEST_CONFIG_TASK_ENABLED > 0)
// #ifndef TEST_CONFIG_STACK_SIZE
// #define TEST_CONFIG_STACK_SIZE                  (256)
// #endif
// #define TEST_CONFIG_MAX_TASK_COUNT              (10)
// #define TEST_CONFIG_CREATE_TASK_TIMES           (10000)
// #endif

// /* task communication test */
// #define TEST_CONFIG_TASK_COMM_ENABLED           (1)
// #if (TEST_CONFIG_TASK_COMM_ENABLED > 0)
// #ifndef TEST_CONFIG_STACK_SIZE
// #define TEST_CONFIG_STACK_SIZE                  (256)
// #endif
// #define TEST_CONFIG_SYNC_TIMES                  (100000)
// #define TEST_CONFIG_QUEUE_BUF_SIZE              (32)
// #endif

// /* timer test */
// #define TEST_CONFIG_TIMER_ENABLED               (1)

// /* kv test */
// #ifndef TEST_CONFIG_KV_ENABLED
// #define TEST_CONFIG_KV_ENABLED                  (1)
// #endif
// #if (TEST_CONFIG_KV_ENABLED > 0)
// #define TEST_CONFIG_KV_TIMES                    (10000)
// #endif

static unsigned int g_var = 0;
static aos_sem_t    g_sem_taskexit_sync;
static aos_mutex_t  g_mutex;
static aos_sem_t    g_sem;
static aos_queue_t  g_queue;
static aos_timer_t  g_timer;

// static char         queue_buf[TEST_CONFIG_QUEUE_BUF_SIZE];
// static aos_queue_t  g_queue1;
// static char         queue1_buf[TEST_CONFIG_QUEUE_BUF_SIZE];
// static aos_queue_t  g_queue2;
// static char         queue2_buf[TEST_CONFIG_QUEUE_BUF_SIZE];
// static aos_queue_t  g_queue3;
// static char         queue3_buf[TEST_CONFIG_QUEUE_BUF_SIZE];

static int dump_test_config(void)
{
#define _PARSE(x) #x
#define PARSE(x) _PARSE(x)
#define PRINT_CONFIG(x) printf("\33[0;35m%s=%s\33[0m\r\n", #x, PARSE(x))
    if (strlen(SYSINFO_ARCH)==0 || strlen(SYSINFO_MCU) == 0 || strlen(SYSINFO_DEVICE_NAME)==0) {
        printf("Please set your device info first!\r\n");
        return -1;
    }
    else {
        PRINT_CONFIG(SYSINFO_ARCH);
        PRINT_CONFIG(SYSINFO_MCU);
        PRINT_CONFIG(SYSINFO_DEVICE_NAME);
        PRINT_CONFIG(SYSINFO_KERNEL);
        PRINT_CONFIG(SYSINFO_KERNEL_VERSION);
        PRINT_CONFIG(SYSINFO_APP_VERSION);
    }

    PRINT_CONFIG(TEST_CONFIG_MM_ENABLED);
#if (TEST_CONFIG_MM_ENABLED > 0)
    PRINT_CONFIG(TEST_CONFIG_MALLOC_MAX_SIZE);
    PRINT_CONFIG(TEST_CONFIG_MALLOC_FREE_TIMES);
#endif

    PRINT_CONFIG(TEST_CONFIG_TASK_ENABLED);
#if (TEST_CONFIG_TASK_ENABLED > 0)
    PRINT_CONFIG(TEST_CONFIG_MAX_TASK_COUNT);
    PRINT_CONFIG(TEST_CONFIG_CREATE_TASK_TIMES);
#endif

    PRINT_CONFIG(TEST_CONFIG_TASK_COMM_ENABLED);
#if (TEST_CONFIG_TASK_COMM_ENABLED > 0)
    PRINT_CONFIG(TEST_CONFIG_SYNC_TIMES);
#endif

    PRINT_CONFIG(TEST_CONFIG_TIMER_ENABLED);

    PRINT_CONFIG(TEST_CONFIG_KV_ENABLED);
#if (TEST_CONFIG_KV_ENABLED > 0)
    PRINT_CONFIG(TEST_CONFIG_KV_TIMES);
#endif

    return 0;
}

#if (TEST_CONFIG_PTHREAD_ENABLED > 0)

/* posix_1_101 Steps:
 * 1.  Create a thread using pthread_create()
 * 2.  Compare the thread ID of 'main' to the thread ID of the newly created
 *     thread. They should be different but process IDs are the same.
 */

static int pid = 0;
static void *a_thread_func_posix_1_101()
{
	// pid = getpid();
	// printf("child in\n");
	krhino_task_sleep(10);
	// printf("child done\n");
	pthread_exit(NULL);
	return NULL;
}

CASE(test_pthread, posix_1_101)
{
	pthread_t main_th, new_th;
	int ret, ppid;

	ret = pthread_create(&new_th, NULL, a_thread_func_posix_1_101, NULL);
    ASSERT_EQ(ret, 0);

	main_th = pthread_self();

	// printf("test task pri is %d\n", main_th->prio);
	// printf("test task name is %s\n", main_th->task_name);

    ASSERT_NE(new_th, main_th);

	// printf("join in\n");
	ret = pthread_join(new_th, NULL);
    ASSERT_EQ(ret, 0);
	// printf("join done\n");
}



/* posix_1_102 Steps:
 * 1.  Create a thread using pthread_create()
 * 2.  Cancel that thread with pthread_cancel()
 * 3.  If that thread doesn't exist, then it pthread_cancel() will return
 *     an error code.  This would mean that pthread_create() did not create
 *     a thread successfully.
 */

static void *a_thread_func_posix_1_102()
{
	krhino_task_sleep(10);

	/* Shouldn't reach here.  If we do, then the pthread_cancel()
	 * function did not succeed. */
	printf("Could not send cancel request correctly\n");
    ASSERT_EQ(1, 0);
	return NULL;
}
CASE(test_pthread, posix_1_102)
{
    pthread_t new_th;
	int ret;

	ret = pthread_create(&new_th, NULL, a_thread_func_posix_1_102, NULL);
	// if (ret) {
	// 	fprintf(stderr, "pthread_create(): %s\n", strerror(ret));
	// 	return PTS_UNRESOLVED;
	// }
    ASSERT_EQ(ret, 0);

	/* Try to cancel the newly created thread.  If an error is returned,
	 * then the thread wasn't created successfully. */
	ret = pthread_cancel(new_th);
	// if (ret) {
	// 	printf("Test FAILED: A new thread wasn't created: %s\n",
	// 	       strerror(ret));
	// 	return PTS_FAIL;
	// }
    ASSERT_EQ(ret, 0);

	krhino_task_sleep(10);
}

 /* Steps:
 * 1.  Create a thread using pthread_create() and passing 'NULL' for 'attr'.
 * 2.  Check to see if the thread is joinable, since that is the default.
 * 3.  We do this by calling pthread_join() and pthread_detach().  If
 *     they fail, then the thread is not joinable, and the test fails.
 */

void *a_thread_func_posix_1_103()
{
	return NULL;
}
CASE(test_pthread, posix_1_103)
{
    pthread_t new_th;
	int ret;

	/* Create a new thread.  The default attribute should be that
	 * it is joinable. */
	ret = pthread_create(&new_th, NULL, a_thread_func_posix_1_103, NULL);
	// if (ret) {
	// 	fprintf(stderr, "ptread_create(): %s\n", strerror(ret));
	// 	return PTS_UNRESOLVED;
	// }
    ASSERT_EQ(ret, 0);
	/* The new thread should be able to be joined. */
	ret = pthread_join(new_th, NULL);
	// if (ret) {
	// 	printf("Test FAILED (pthread_join(): %s)\n", strerror(ret));
	// 	return PTS_FAIL;
	// }
    ASSERT_EQ(ret, 0);

	// krhino_task_sleep(100);
}

/* Steps:
 * 1.  Create 5 separete threads using pthread_create() passing to it a single int 'arg'.
 * 2.  Use that passed int argument in the thread function start routine  and make sure no
 *     errors occur.
 */

static void *a_thread_func_posix_1_104(void *num)
{
	printf("arg = %li\n", (long)num);
	krhino_task_sleep(10);
	return num;
}
CASE(test_pthread, posix_1_104)
{
    pthread_t new_th;
	long i;
	void *res;
	int ret;

	for (i = 1; i < 6; i++) {
		ret = pthread_create(&new_th, NULL, a_thread_func_posix_1_104, (void *)i);

		// if (ret) {
		// 	printf(stderr, "pthread_create(): %s\n",
		// 	        strerror(ret));
		// 	return PTS_FAIL;
		// }
        ASSERT_EQ(ret, 0);

		pthread_join(new_th, &res);
		krhino_task_sleep(10);

		if ((long)res != i) {
			printf("Test FAILED: Returned value did not match %li != %li\n", (long)res, i);
            ASSERT_FAIL();
			//return PTS_FAIL;
		}
	}

}
/* Steps:
 * 1.  Create a thread using pthread_create(), passing to it all valid values.
 * 2.  If the return code was not EGAIN, EPERM or EINVAL, it should return 0.
 */

static void *a_thread_func_posix_1_105()
{
	return NULL;
}

CASE(test_pthread, posix_1_105)
{
    pthread_t new_th;
	int ret;

	ret = pthread_create(&new_th, NULL, a_thread_func_posix_1_105, NULL);
	// if (ret != 0) {
	// 	printf("Test FAILED: pthread_create(): %d %s\n",
	// 	       ret, strerror(ret));
	// 	return PTS_FAIL;
	// }
    ASSERT_EQ(ret, 0);
}

/* Steps:
 * 1.  Create a new thread.  Have it sleep for 3 seconds.
 * 2.  The main() thread should wait for the new thread to finish
 *     execution before exiting out.
 *
 */

static int end_exec;

static void *a_thread_func_posix_1_201()
{
	int i;

	printf("Wait for 3 seconds for thread to finish execution:\n");
	for (i = 1; i < 4; i++) {
		printf("Waited (%d) second\n", i);
		krhino_task_sleep(1);
	}

	/* Indicate that the thread has ended execution. */
	end_exec = 1;

	pthread_exit(0);
	return NULL;
}

CASE(test_pthread, posix_1_201)
{
    pthread_t new_th;
    int ret = 0;

	/* Initialize flag */
	end_exec = 0;

	/* Create a new thread. */
    ret = pthread_create(&new_th, NULL, a_thread_func_posix_1_201, NULL);
	// if (pthread_create(&new_th, NULL, a_thread_func, NULL) != 0) {
	// 	perror("Error creating thread\n");
	// 	return PTS_UNRESOLVED;
	// }
    ASSERT_EQ(ret, 0);

	/* Wait for thread to return */
    ret = pthread_join(new_th, NULL) != 0;
	// if (pthread_join(new_th, NULL) != 0) {
	// 	perror("Error in pthread_join()\n");
	// 	return PTS_UNRESOLVED;
	// }
    ASSERT_EQ(ret, 0);

	// if (end_exec == 0) {
	// 	printf("Test FAILED: When using pthread_join(), "
	// 	       "main() did not wait for thread to finish "
	// 	       "execution before continuing.\n");
	// 	return PTS_FAIL;
	// }
    ASSERT_EQ(end_exec, 1);
}

/* Steps:
 * 1.  Create a new thread.  Have it return a return code on pthread_exit();
 * 2.  Call pthread_join() in main(), and pass to it 'value_ptr'.
 * 3.  Check to see of the value_ptr and the value returned by pthread_exit()
 *     are the same;
 *
 */
#define RETURN_CODE ((void *)100)

#define INTHREAD 0		/* Control going to or is already for Thread */
#define INMAIN 1		/* Control going to or is already for CASE */

/* Manual semaphore used to indicate when the thread has been created. */
static int sem;

static void *a_thread_func_posix_1_202()
{
	sem = INMAIN;
	pthread_exit(RETURN_CODE);
	return NULL;
}

CASE(test_pthread, posix_1_202)
{
    pthread_t new_th;
	void *value_ptr;
    int ret = 0;

	value_ptr = 0;
	sem = INTHREAD;

    ret = pthread_create(&new_th, NULL, a_thread_func_posix_1_202, NULL);
	// if (pthread_create(&new_th, NULL, a_thread_func_posix_1_202, NULL) != 0) {
	// 	perror("Error creating thread\n");
	// 	return PTS_UNRESOLVED;
	// }
    ASSERT_EQ(ret , 0);

	/* Make sure the thread was created before we join it. */
	while (sem == INTHREAD)
		krhino_task_sleep(1);

	/* Wait for thread to return */
    ret = pthread_join(new_th, &value_ptr);
	// if (pthread_join(new_th, &value_ptr) != 0) {
	// 	perror("Error in pthread_join()\n");
	// 	return PTS_UNRESOLVED;
	// }
    ASSERT_EQ(ret, 0);

	/*
	 * Check to make sure that 'value_ptr' that was passed to
	 * pthread_join() and the pthread_exit() return code that
	 * was used in the thread function are the same.
	 */
	// if (value_ptr != RETURN_CODE) {
	// 	printf("Test FAILED: pthread_join() did not return the "
	// 	       "pthread_exit value of the thread in 'value_ptr'.\n");
	// 	return PTS_FAIL;
	// }
    ASSERT_EQ(value_ptr, RETURN_CODE);

}

/* Steps:
 * 1.  Create a new thread.
 * 2.  Send a cancel request to it from main, then use pthread_join to
 *     wait for it to end.
 * 3.  The thread will sleep for 3 seconds, then call test_cancel() to
 *     cancel execution.
 * 4.  When this happens, the cleanup handler should be called.
 * 5.  Main will test that when pthread_join allows main to continue
 *     with the process that the thread has ended execution.  If the
 *     cleanup_handler was not called, then the test fails.
 */

#define TIMEOUT 10
static int cleanup_flag;

/*
 * Cleanup function that the thread executes when it is canceled.  So if
 * cleanup_flag is 1, it means that the thread was canceled.
 */
static void a_cleanup_func()
{
	cleanup_flag = 1;
}

static void *a_thread_func_posix_1_203()
{
	int err;

	err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    ASSERT_EQ(err, 0);
	if (err != 0) {
		ASSERT_FAIL();
		// printf(stderr, "pthread_setcancelstate: %s", strerror(err));
		// goto thread_exit_failure;
	}


	err = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    ASSERT_EQ(err, 0);
    if (err != 0) {
		ASSERT_FAIL();
		// printf(stderr, "pthread_setcanceltype: %s", strerror(err));
		// goto thread_exit_failure;
	}

	/* Set up the cleanup handler */
	pthread_cleanup_push(a_cleanup_func, NULL);

	/* Wait for a timeout period for the cancel request to be sent. */
	// krhino_task_sleep(TIMEOUT);
	pthread_exit(NULL);

	/* Should not get here, but just in case pthread_testcancel() didn't
	 * work -- the cancel request timed out. */
	pthread_cleanup_pop(0);

	// ASSERT_EQ(cleanup_flag, 1);

// thread_exit_failure:
// 	cleanup_flag = -1;

	// return NULL;
}
CASE(test_pthread, posix_1_203)
{
    pthread_t new_th;

	int err;

	err = pthread_create(&new_th, NULL, a_thread_func_posix_1_203, NULL);
	// if (err != 0) {
	// 	fprintf(stderr, "pthread_create: %s\n", strerror(err));
	// 	return PTS_UNRESOLVED;
	// }
    ASSERT_EQ(err, 0);

	/* Remove potential for race */
	krhino_task_sleep(TIMEOUT / 2);

	err = pthread_cancel(new_th);
    ASSERT_EQ(err, 0);
	// if (err != 0) {
	// 	fprintf(stderr, "pthread_cancel: %s\n", strerror(err));
	// 	return PTS_UNRESOLVED;
	// }

	err = pthread_join(new_th, NULL);
    ASSERT_EQ(err, 0);
	// if (err != 0) {
	// 	fprintf(stderr, "pthread_join: %s\n", strerror(err));
	// 	return PTS_UNRESOLVED;
	// }

    ASSERT_EQ(cleanup_flag, 1);
	if (cleanup_flag == 0) {
		printf("Test FAILED: Thread did not finish execution when "
		       "pthread_join returned.\n");
		//return PTS_FAIL;
	}

	if (cleanup_flag == -1) {
		printf("Error in pthread_testcancel.  Cancel request timed "
		       "out.\n");
		//return PTS_UNRESOLVED;
	}
}

/* posix_1_204 steps are:
* -> create a thread blocked on a mutex.
* -> create another thread which tries and join the first thread.
* -> cancel the 2nd thread.
* -> unblock the semaphore then join the 1st thread
* The test fails if the main thread is unable to join the 1st thread.
*/
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

/* 1st thread function */
void *threaded_posix_1_204(void *arg)
{
	int ret = 0;

	/* Try and lock the mutex, then exit */
	ret = pthread_mutex_lock(&mtx);
    ASSERT_EQ(ret, 0);
	// if (ret != 0)
	// 	UNRESOLVED(ret, "Failed to lock mutex");

	ret = pthread_mutex_unlock(&mtx);
    ASSERT_EQ(ret, 0);
	// if (ret != 0)
	// 	UNRESOLVED(ret, "Failed to unlock mutex");

	return NULL;
}

/* Canceled thread */
void *joiner_func_posix_1_204(void *arg)
{
	(void)pthread_join(*(pthread_t *) arg, NULL);

	// FAILED("The joiner thread was not canceled");

	/* please the compiler */
	return NULL;
}
CASE(test_pthread, posix_1_204)
{
    int ret = 0;
	pthread_t child;
	pthread_t joiner;

    /* Lock the mutex */
    ret = pthread_mutex_lock(&mtx);
    ASSERT_EQ(ret, 0);
    // if (ret != 0)
    //     UNRESOLVED(ret, "failed to lock the mutex");

    ret = pthread_create(&child, NULL, threaded_posix_1_204, NULL);

    // switch (scenarii[sc].result) {
    //     /* Operation was expected to succeed */
    // case 0:

    //     if (ret != 0)
    //         UNRESOLVED(ret, "Failed to create this thread");

    //     break;
    //     /* Operation was expected to fail */
    // case 1:

    //     if (ret == 0)
    //         UNRESOLVED(-1, "An error was expected "
    //                 "but the thread creation succeeded");

    //     break;
    //     /* We did not know the expected result */
    // case 2:
    // default:
    // }

    /* The new thread is running */
    if (ret == 0) {
        /* Now create the joiner thread */
        ret = pthread_create(&joiner, NULL, joiner_func_posix_1_204, &child);
        ASSERT_EQ(ret, 0);
        // if (ret != 0)
        //     UNRESOLVED(ret, "Failed to create the joiner thread");

        /* Let it enter pthread_join */
        sched_yield();

        /* Cancel the joiner thread */
        ret = pthread_cancel(joiner);
        ASSERT_EQ(ret, 0);
        // if (ret != 0)
        //     UNRESOLVED(ret, "Failed to cancel the thread");

        /* Join the canceled thread */
        ret = pthread_join(joiner, NULL);
        ASSERT_EQ(ret, 0);
        // if (ret != 0)
        //     UNRESOLVED(ret, "Failed to join the canceled thread");

        /* Unblock the child thread */
        ret = pthread_mutex_unlock(&mtx);
        ASSERT_EQ(ret, 0);
        // if (ret != 0)
        //     UNRESOLVED(ret, "Failed to unlock the mutex");

        /* Check the first thread is still joinable */
        ret = pthread_join(child, NULL);
        ASSERT_EQ(ret, 0);
        // if (ret != 0) {
        //     output("Error returned: %d\n");
        //     FAILED("The thread is no more joinable");
        // }

    } else {
        ret = pthread_mutex_unlock(&mtx);
        ASSERT_EQ(ret, 0);
        // if (ret != 0)
        //     UNRESOLVED(ret, "Failed to unlock the mutex");
    }
}

/* Steps:
 * 1.  Create a new thread.
 * 2.  Join that thread to main.  If the return code is not 0,
 *     or the other valid error codes of EINVAL, ESRCH or EDEADLK,
 *     then the test fails.
 */
static void *a_thread_func_posix_1_205()
{
	pthread_exit(0);
	return NULL;
}
CASE(test_pthread, posix_1_205)
{
    pthread_t new_th;
	int ret;
    ret = pthread_create(&new_th, NULL, a_thread_func_posix_1_205, NULL);
    ASSERT_EQ(ret, 0);
	// if (pthread_create(&new_th, NULL, a_thread_func_posix_1_205, NULL) != 0) {
	// 	perror("Error creating thread\n");
	// 	return PTS_UNRESOLVED;
	// }

	ret = pthread_join(new_th, NULL);
	if (ret != 0) {
		//if ((ret != EINVAL) && (ret != ESRCH) && (ret != EDEADLK)) {
			printf("Test FAILED: Invalid return code %d.\n", ret);
			ASSERT_FAIL();
            //return PTS_FAIL;
		//}
	}
}

/* Steps:
 * 1.  Create a new thread.
 * 2.  Call pthread_join() in main. This means that the thread should
 *     have ended execution.
 * 3.  Call pthread_join() again, it should give the error code ESRCH.
 *
 */

static void *a_thread_func_posix_1_206()
{
	pthread_exit(0);
	return NULL;
}

CASE(test_pthread, posix_1_206)
{
    pthread_t new_th;
	int ret;

	if (pthread_create(&new_th, NULL, a_thread_func_posix_1_206, NULL) != 0) {
        ASSERT_FAIL();
		// perror("Error creating thread\n");
		// return PTS_UNRESOLVED;
	}

	ret = pthread_join(new_th, NULL);

	if (ret != 0) {
		ASSERT_FAIL();
        // perror("Error in pthread_join()\n");
		// return PTS_UNRESOLVED;
	}

	/*
	 * Now that the thread has returned, try to join it again.
	 * It should give the error code of ESRCH.
	 */
	ret = pthread_join(new_th, NULL);
	//if (ret != ESRCH) {
		printf("Test FAILED: Return code should be ESRCH, but is: "
		       "%d instead.\n", ret);
		ASSERT_FAIL();
        // return PTS_FAIL;
	//}
}

/* Steps:
 * 1.  Create a new thread.
 * 2.  Call pthread_join(itself) in new thread.
 * 3.  return EDEADLK.
 */

static void *a_thread_func_posix_1_207()
{
	pthread_exit(0);
	return NULL;
}

CASE(test_pthread, posix_1_207)
{
    pthread_t new_th;
	int ret;

	if (pthread_create(&new_th, NULL, a_thread_func_posix_1_207, NULL) != 0) {
		printf("pthread_create fail\n");
		ASSERT_FAIL();
		// perror("Error creating thread\n");
		// return PTS_UNRESOLVED;
	}

    krhino_task_sleep(10);

    ret = pthread_join(new_th, NULL);

    if (ret != -1) {//ESRCH
		printf("join a exit pthread shouldn't be success\n");
		// ASSERT_FAIL();
        // perror("Error in pthread_join()\n");
		// return PTS_UNRESOLVED;
	}

    new_th = pthread_self();

	ret = pthread_join(new_th, NULL);

	if (ret != -1) {//EDEADLK
		printf("join itself shouldn't be success\n");
		ASSERT_FAIL();
        // perror("Error in pthread_join()\n");
		// return PTS_UNRESOLVED;
	}

}

/* STEPS:
 * 1. Create a joinable thread
 * 2. Detach that thread with pthread_detach()
 * 3. Try and join the thread to main() using pthread_join()
 * 4. An error should return from the pthread_join() function saying that the
 *    thread is detched.  The test passes.
 * 5. Else, if pthread_join is successful, the test fails.
 */

void *a_thread_func_posix_1_301()
{
	//pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	/* If the thread wasn't canceled in 10 seconds, time out */
	krhino_task_sleep(10);

	//perror("Thread couldn't be canceled (at cleanup time), timing out\n");
	pthread_exit(0);
	return NULL;
}

CASE(test_pthread, posix_1_301)
{
    //pthread_attr_t new_attr;
	pthread_t new_th;
	int ret;

	// /* Initialize attribute */
	// if (pthread_attr_init(&new_attr) != 0) {
	// 	perror("Cannot initialize attribute object\n");
	// 	return PTS_UNRESOLVED;
	// }

	// /* Set the attribute object to be joinable */
	// if (pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_JOINABLE) !=
	//     0) {
	// 	perror("Error in pthread_attr_setdetachstate()\n");
	// 	return PTS_UNRESOLVED;
	// }

	/* Create the thread */
	if (pthread_create(&new_th, NULL, a_thread_func_posix_1_301, NULL) != 0) {
		ASSERT_FAIL();
        // perror("Error creating thread\n");
		// return PTS_UNRESOLVED;
	}

	/* Detach the thread. */
	if (pthread_detach(new_th) != 0) {
		ASSERT_FAIL();
        // printf("Error detaching thread\n");
		// return PTS_FAIL;
	}

	/* Now try and join it.  This should fail. */
	ret = pthread_join(new_th, NULL);

	/* Cleanup: Cancel the thread */
	//pthread_cancel(new_th);

    krhino_task_sleep(10);

	if (ret == 0) {
		ASSERT_FAIL();
        // printf("Test FAILED\n");
		// return PTS_FAIL;
	} else if (ret == -1) {//EINVAL
		// printf("Test PASSED\n");
		//return PTS_PASS;
	} else {
		ASSERT_FAIL();
        // perror("Error in pthread_join\n");
		// return PTS_UNRESOLVED;
	}
}

/* STEPS:
 * 1.Create a joinable thread
 * 2.Detach that thread
 * 3.Verify that the thread did not terminate because of this
 *
 */

void *a_thread_func_posix_1_302()
{

	//pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	/* If the thread wasn't canceled in 10 seconds, time out */
	krhino_task_sleep(10);

	//perror("Thread couldn't be canceled (at cleanup time), timing out\n");
	pthread_exit(0);
	return NULL;
}


CASE(test_pthread, posix_1_302)
{
    //pthread_attr_t new_attr;
	pthread_t new_th;
	int ret;

	// /* Initialize attribute */
	// if (pthread_attr_init(&new_attr) != 0) {
	// 	perror("Cannot initialize attribute object\n");
	// 	return PTS_UNRESOLVED;
	// }

	// /* Set the attribute object to be joinable */
	// if (pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_JOINABLE) !=
	//     0) {
	// 	perror("Error in pthread_attr_setdetachstate()\n");
	// 	return PTS_UNRESOLVED;
	// }

	/* Create the thread */
	if (pthread_create(&new_th, NULL, a_thread_func_posix_1_302, NULL) != 0) {
		ASSERT_FAIL();
        // perror("Error creating thread\n");
		// return PTS_UNRESOLVED;
	}

	/* Detach the thread. */
	if (pthread_detach(new_th) != 0) {
		ASSERT_FAIL();
        // printf("Error detaching thread\n");
		// return PTS_FAIL;
	}

    krhino_task_sleep(10);

	/* Verify that it hasn't terminated the thread */
	ret = pthread_cancel(new_th);

	if (ret != 0) {
		if (ret == -1) {//ESRCH
			ASSERT_FAIL();
            printf("Test FAILED\n");
			//return PTS_FAIL;
		}
        ASSERT_FAIL();
		//perror("Error canceling thread\n");
		//return PTS_UNRESOLVED;
	}
}

/* The steps are:
 * -> Create a thread
 * -> detach the thread outside the thread
 * -> wait for the thread to post a semaphore.
 * The test fails if the semaphore is not posted within a certain duration.
 */

sem_t sem_sync;

void *threaded_posix_1_303(void *arg)
{
	int ret = 0;

	if (arg != NULL) {
		ret = pthread_detach(pthread_self());
		if (ret != 0) {
			ASSERT_FAIL();
            //UNRESOLVED(ret, "Failed to detach the thread");
		}
	}
	/* Wait for this semaphore which indicates that pthread_detach has been called */
	do {
		ret = sem_wait(&sem_sync);
	}
	while (ret == 1);    // && (errno == EINTR));

	if (ret == 1) {
		ASSERT_FAIL();
        //UNRESOLVED(errno, "Failed to wait for the semaphore");
	}

	/* Post the semaphore to indicate the main thread we're alive */
	do {
		ret = sem_post(sem_sync);
	}
	while (ret == 1); //&& (errno == EINTR));
	if (ret == 1) {
		ASSERT_FAIL();
        //UNRESOLVED(errno, "Failed to post the semaphore");
	}

	return arg;
}
CASE(test_pthread, posix_1_303)
{
    int ret = 0;
    //pthread_attr_t attr;
	pthread_t child;
	struct timespec ts;
	sys_time_t t;

    ret = sem_init(&sem_sync, 0, 0);

    ret = pthread_create(&child, NULL, threaded_posix_1_303, NULL);
    ASSERT_EQ(ret, 0);

    ret = pthread_detach(child);

    do {
        ret = sem_post(&sem_sync);
    }
    while (ret == 1);// && (errno == EINTR));
    if (ret == 1) {
        ASSERT_FAIL();
        //UNRESOLVED(errno,"Failed to post the semaphore");
    }

    /* Just wait for the thread to terminate */
    // ret = clock_gettime(CLOCK_REALTIME, &ts);
	t = krhino_sys_time_get();
    if (t == 0) {
        ASSERT_FAIL();
        //UNRESOLVED(ret, "Failed to get time");
    }
	ts.tv_sec = (time_t)(t / 1000);
    ts.tv_sec += TIMEOUT;
	ts.tv_nsec = 0;

    do {
        ret = sem_timedwait(sem_sync, &ts);
    }
    while (ret == 1);// && (errno == EINTR));
    if (ret == 1) {
        // if (errno == -1) { //ETIMEDOUT
        //     ASSERT_FAIL();
        //     //FAILED("pthread_detach made the thread terminate");
        // }
        /* else */
        ASSERT_FAIL();
        //UNRESOLVED(errno,"Failed to wait for the semaphore");
    }

    /* Let the thread an additionnal row to cleanup */
    //sched_yield();

    ret = sem_destroy(&sem_sync);
	if (ret != 0) {
		ASSERT_FAIL();
        //UNRESOLVED(ret, "Failed to destroy the semaphore");
	}
}


/* The steps are:
 * -> Create a thread
 * -> detach the thread inside the thread
 * -> wait for the thread to post a semaphore.
 * The test fails if the semaphore is not posted within a certain duration.
 */



sem_t sem_sync_304;

void *threaded_posix_1_304(void *arg)
{
	int ret = 0;

	if (arg != NULL) {
		printf("child pthread_detach\n");
		ret = pthread_detach(pthread_self());
		if (ret != 0) {
			ASSERT_FAIL();
            //UNRESOLVED(ret, "Failed to detach the thread");
		}
	}
	/* Wait for this semaphore which indicates that pthread_detach has been called */
	do {
		printf("child sem_wait\n");
		ret = sem_wait(&sem_sync_304);
	}
	while (ret == 1);    // && (errno == EINTR));

	if (ret == 1) {
		ASSERT_FAIL();
        //UNRESOLVED(errno, "Failed to wait for the semaphore");
	}

	/* Post the semaphore to indicate the main thread we're alive */
	do {
		printf("child sem_post\n");
		ret = sem_post(sem_sync_304);
	}
	while (ret == 1); //&& (errno == EINTR));
	if (ret == 1) {
		ASSERT_FAIL();
        //UNRESOLVED(errno, "Failed to post the semaphore");
	}

	return arg;
}

CASE(test_pthread, posix_1_304)//pthread_create(&child, &attr, threaded_posix_1_304, &ret); fail
{
    int ret = 0;
    pthread_attr_t attr;
	pthread_t child;
	struct timespec ts;
	sys_time_t t;

    ret = sem_init(&sem_sync_304, 0, 0);

    attr.stacksize                 = 2048;
    attr.schedparam.sched_priority = 20;
    attr.detachstate               = PTHREAD_CREATE_DETACHED;
	printf("pthread_create\n");
    ret = pthread_create(&child, &attr, threaded_posix_1_304, &ret);
    ASSERT_EQ(ret, 0);

    //ret = pthread_detach(childD);
	printf("sem_post\n");
    do {
        ret = sem_post(sem_sync_304);
    }
    while (ret == 1);// && (errno == EINTR));
    if (ret == 1) {
        ASSERT_FAIL();
        //UNRESOLVED(errno,"Failed to post the semaphore");
    }
	printf("krhino_sys_time_get\n");
    /* Just wait for the thread to terminate */
    // ret = clock_gettime(CLOCK_REALTIME, &ts);
    t = krhino_sys_time_get();
    if (t == 0) {
        ASSERT_FAIL();
        //UNRESOLVED(ret, "Failed to get time");
    }
	ts.tv_sec = (time_t)(t / 1000);
    ts.tv_sec += TIMEOUT;
	ts.tv_nsec = 0;

    do {
		printf("sem_timedwait %d s.\n", ts.tv_sec);
        ret = sem_timedwait(sem_sync_304, &ts);
		printf("got the sem.\n");
    }
    while (ret == 1);// && (errno == EINTR));
    if (ret == 1) {
        // if (errno == -1) { //ETIMEDOUT
        //     ASSERT_FAIL();
        //     //FAILED("pthread_detach made the thread terminate");
        // }
        /* else */
        ASSERT_FAIL();
        //UNRESOLVED(errno,"Failed to wait for the semaphore");
    }

    /* Let the thread an additionnal row to cleanup */
    //sched_yield();

    ret = sem_destroy(&sem_sync_304);
	if (ret != 0) {
		ASSERT_FAIL();
        //UNRESOLVED(ret, "Failed to destroy the semaphore");
	}

}

/* STEPS:
 * 1. Create a thread.
 * 2.Wait 'till the thread exits.
 * 3.Try and detach this thread.
 * 4.Check the return value and make sure it is ESRCH
 *
 */

void *a_thread_func_posix_1_305()
{
	pthread_exit(0);
	return NULL;
}

CASE(test_pthread, posix_1_305)
{
    pthread_t new_th;
	int ret;

	/* Create the thread */
	if (pthread_create(&new_th, NULL, a_thread_func_posix_1_305, NULL) != 0) {
		ASSERT_FAIL();
        // perror("Error creating thread\n");
		// return PTS_UNRESOLVED;
	}

	/* Wait 'till the thread returns.
	 * The thread could have ended by the time we try to join, so
	 * don't worry about it, just so long as other errors don't
	 * occur. The point is to make sure the thread has ended execution. */
	if (pthread_join(new_th, NULL) == -1) {//EDEADLK
		ASSERT_FAIL();
        //perror("Error joining thread\n");
		//return PTS_UNRESOLVED;
	}

	/* Detach the non-existant thread. */
	ret = pthread_detach(new_th);

	/* Check return value of pthread_detach() */
	if (ret != -1) { //ESRCH
		printf
		    ("Test FAILED: Incorrect return code: %d instead of ESRCH\n",
		     ret);
		ASSERT_FAIL();
        //return PTS_FAIL;

	}
}

/* Steps:
 * 1.  Create a new thread.  Have it return a return code on pthread_exit();
 * 2.  Call pthread_join() in main(), and pass to it 'value_ptr'.
 * 3.  Check to see of the value_ptr and the value returned by pthread_exit() are the same;
 *
 */

#define RETURN_CODE 100		/* Set a random return code number. This shall be the return code of the
				   thread when using pthread_exit(). */
#define INTHREAD 0		/* Control going to or is already for Thread */
#define INMAIN 1		/* Control going to or is already for Main */

static int sem;			/* Manual semaphore used to indicate when the thread has been created. */

/* Thread's function. */
void *a_thread_func_posix_1_401()
{
	sem = INMAIN;
	pthread_exit((void *)RETURN_CODE);
	return NULL;
}
CASE(test_pthread, posix_1_401)
{
    pthread_t new_th;
	int *value_ptr;

	/* Initializing variables. */
	value_ptr = 0;
	sem = INTHREAD;

	/* Create a new thread. */
	if (pthread_create(&new_th, NULL, a_thread_func_posix_1_401, NULL) != 0) {
		ASSERT_FAIL();
        // perror("Error creating thread\n");
		// return PTS_UNRESOLVED;
	}

	/* Make sure the thread was created before we join it. */
	while (sem == INTHREAD)
		krhino_task_sleep(1);

	/* Wait for thread to return */
	if (pthread_join(new_th, (void *)&value_ptr) != 0) {
		ASSERT_FAIL();
        // perror("Error in pthread_join()\n");
		// return PTS_UNRESOLVED;
	}

	/* Check to make sure that 'value_ptr' that was passed to pthread_join() and the
	 * pthread_exit() return code that was used in the thread function are the same. */
	if ((long)value_ptr != RETURN_CODE) {
		printf
		    ("Test FAILED: pthread_exit() could not pass the return value of the thread in 'value_ptr' to pthread_join().\n");
		ASSERT_FAIL();
        //return PTS_FAIL;
	}
}


/* Steps:
 * 1.  Create a new thread.
 * 2.  The thread will call pthread_exit().
 * 3.  When this happens, the cleanup handler should be called.
 * 4.  Main will test that when pthread_join allows main to continue with the process that
 *     the thread has ended execution.  If the cleanup_handler was not called, then the
 *     test fails.
 *
 */

int i[3], j;

/* Cleanup function that the thread executes when it is canceled.  So if
 * cleanup_flag is 1, it means that the thread was canceled. */
void a_cleanup_func1()
{
	i[j] = 1;
	j++;
	return;
}

/* Cleanup function that the thread executes when it is canceled.  So if
 * cleanup_flag is 1, it means that the thread was canceled. */
void a_cleanup_func2()
{
	i[j] = 2;
	j++;
	return;
}

/* Cleanup function that the thread executes when it is canceled.  So if
 * cleanup_flag is 1, it means that the thread was canceled. */
void a_cleanup_func3()
{
	i[j] = 3;
	j++;
	return;
}

/* Thread's function. */
void *a_thread_func_posix_1_402()
{
	/* Set up 3 cleanup handlers */
	pthread_cleanup_push(a_cleanup_func1, NULL);
	pthread_cleanup_push(a_cleanup_func2, NULL);
	pthread_cleanup_push(a_cleanup_func3, NULL);

	/* Terminate the thread here. */
	pthread_exit(0);

	/* Need these here for it to compile nicely.  We never reach here though. */
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);
	return NULL;
}

CASE(test_pthread, posix_1_402)
{
    pthread_t new_th;

	/* Initialize integer array. */
	for (j = 0; j < 3; j++)
		i[j] = 0;

	/* Initialize counter. */
	j = 0;

	/* Create a new thread. */
	if (pthread_create(&new_th, NULL, a_thread_func_posix_1_402, NULL) != 0) {
		ASSERT_FAIL();
        // perror("Error creating thread\n");
		// return PTS_UNRESOLVED;
	}

	/* Wait for thread to return */
	if (pthread_join(new_th, NULL) != 0) {
		ASSERT_FAIL();
        // perror("Error in pthread_join()\n");
		// return PTS_UNRESOLVED;
	}
	if (i[0] == 0) {
		printf("Test FAILED: Did not execute cleanup handlers.\n");
		ASSERT_FAIL();
	}
	ASSERT_EQ(i[0], 3);
	ASSERT_EQ(i[1], 2);
	ASSERT_EQ(i[2], 1);
	// /* Check to make sure that the cleanup handlers were executed in order. */
	// if (i[0] == 3) {
	// 	if (i[1] == 2) {
	// 		if (i[2] == 1) {
	// 			printf("Test PASSED\n");
	// 			//return PTS_PASS;

	// 		}
	// 		printf
	// 		    ("Test FAILED: Did not execute cleanup handlers in order.\n");
	// 		ASSERT_FAIL();
    //         //return PTS_FAIL;
	// 	}
	// 	printf
	// 	    ("Test FAILED: Did not execute cleanup handlers in order.\n");
	// 	ASSERT_FAIL();
    //     //return PTS_FAIL;
	// } else if (i[0] == 0) {
	// 	printf("Test FAILED: Did not execute cleanup handlers.\n");
	// 	ASSERT_FAIL();
    //     //return PTS_FAIL;
	// }
}

/* Steps:
 * 1.  Create a new thread.
 * 2.  The function that the thread calls will call pthread_self() and store
 *     the return value (which is the thread ID of the thread calling it) into
 *     a global variable.
 * 3.  Call pthread_equal and verify that the thread IDs are the same.
 * 4.  Verify that the new thread ID is not the same as main thread ID.
 *
 */
pthread_t new_th2;		/* Global thread to hold the value of when pthread_self
				   returns from the thread function. */

void *a_thread_func_posix_1_501()
{
	new_th2 = pthread_self();
	pthread_exit(0);
	return NULL;
}
CASE(test_pthread, posix_1_501)
{
    pthread_t new_th1;

	/* Create a new thread. */
	if (pthread_create(&new_th1, NULL, a_thread_func_posix_1_501, NULL) != 0) {
		ASSERT_FAIL();
        // perror("Error creating thread\n");
		// return PTS_UNRESOLVED;
	}

	/* Wait for thread to return */
	if (pthread_join(new_th1, NULL) != 0) {
		ASSERT_FAIL();
        // perror("Error in pthread_join()\n");
		// return PTS_UNRESOLVED;
	}

	/* Call pthread_equal() and pass to it the new thread ID in both
	 * parameters.  It should return a non-zero value, indicating that
	 * both thread IDs are equal, and therefore refer to the same
	 * thread. */
	if (pthread_equal(new_th1, new_th2) == 0) {
		ASSERT_FAIL();
        printf("Test FAILED\n");
		//return PTS_FAIL;
	}
	if (pthread_equal(new_th1, pthread_self()) != 0) {
		ASSERT_FAIL();
        printf("Test FAILED -- 2 threads have the same ID\n");
		//return PTS_FAIL;
	}
}

/* STEPS:
 * 1. Create a thread
 * 2. The thread will push a cleanup handler routine, then call pthread_cleanup_pop, setting
 *    'execution' to a non-zero value (meaning the cleanup handler should be executed)
 * 3. Verify that the cleanup handler was called.
 *
 */

#define CLEANUP_NOTCALLED 0
#define CLEANUP_CALLED 1

#define INTHREAD 0
#define INMAIN 1

static int sem1;			/* Manual semaphore */
static int cleanup_flag;

/* Cleanup handler */
void a_cleanup_func_posix_1_601(void *flag_val)
{
	cleanup_flag = (long)flag_val;
	return;
}

/* Function that the thread executes upon its creation */
void *a_thread_func_posix_1_601()
{
	pthread_cleanup_push(a_cleanup_func_posix_1_601, (void *)CLEANUP_CALLED);
	pthread_cleanup_pop(1);

	/* Tell main that the thread has called the pop function */
	sem1 = INMAIN;

	/* Wait for main to say it's ok to continue (as it checks to make sure that
	 * the cleanup handler was called */
	while (sem1 == INMAIN)
		krhino_task_sleep(1);

	pthread_exit(0);
	return NULL;
}
CASE(test_pthread, posix_1_601)
{
    pthread_t new_th;

	/* Initializing values */
	sem1 = INTHREAD;
	cleanup_flag = CLEANUP_NOTCALLED;

	/* Create a new thread. */
	if (pthread_create(&new_th, NULL, a_thread_func_posix_1_601, NULL) != 0) {
		ASSERT_FAIL();
        // perror("Error creating thread\n");
		// return PTS_UNRESOLVED;
	}

	/* Wait for thread to call the pthread_cleanup_pop */
	while (sem1 == INTHREAD)
		krhino_task_sleep(1);

	/* Check to verify that the cleanup handler was called */
	if (cleanup_flag != CLEANUP_CALLED) {
		printf("Test FAILED: Cleanup handler was not called\n");
		ASSERT_FAIL();
        // return PTS_FAIL;
	}

	/* Tell thread it can keep going now */
	sem1 = INTHREAD;

	/* Wait for thread to end execution */
	if (pthread_join(new_th, NULL) != 0) {
		ASSERT_FAIL();
        // perror("Error in pthread_join()\n");
		// return PTS_UNRESOLVED;
	}

}

/* STEPS:
 * 1. Create a thread
 * 2. The thread will push a cleanup handler routine, then call pthread_cleanup_pop, setting
 *    'execution' to a zero value (meaning the cleanup handler shouldn't be executed)
 * 3. Verify that the cleanup handler was not called.
 *
 */

void *a_thread_func_posix_1_602()
{
	pthread_cleanup_push(a_cleanup_func_posix_1_601, (void *)CLEANUP_CALLED);
	pthread_cleanup_pop(0);

	/* Tell main that the thread has called the pop function */
	sem1 = INMAIN;

	/* Wait for main to say it's ok to continue (as it checks to make sure that
	 * the cleanup handler was called */
	while (sem1 == INMAIN)
		krhino_task_sleep(1);

	pthread_exit(0);
	return NULL;
}
CASE(test_pthread, posix_1_602)
{
    pthread_t new_th;

	/* Initializing values */
	sem1 = INTHREAD;
	cleanup_flag = CLEANUP_NOTCALLED;

	/* Create a new thread. */
	if (pthread_create(&new_th, NULL, a_thread_func_posix_1_602, NULL) != 0) {
		ASSERT_FAIL();
        // perror("Error creating thread\n");
		// return PTS_UNRESOLVED;
	}

	/* Wait for thread to call the pthread_cleanup_pop */
	while (sem1 == INTHREAD)
		krhino_task_sleep(1);

	/* Check to verify that the cleanup handler wasn't called */
	if (cleanup_flag == CLEANUP_CALLED) {
		printf("Test FAILED: Cleanup was incorrectly called\n");
		ASSERT_FAIL();
        // return PTS_FAIL;
	}

	/* Tell thread it can keep going now */
	sem1 = INTHREAD;

	/* Wait for thread to end execution */
	if (pthread_join(new_th, NULL) != 0) {
		ASSERT_FAIL();
        // perror("Error in pthread_join()\n");
		// return PTS_UNRESOLVED;
	}
}

/* STEPS:
 * 1. Create a thread
 * 2. The thread will push a cleanup handler routine and then exit befor the cleanup_pop
 *    routine is reached.
 * 3. Verify that the cleanup handler was called.
 *
 */
//int cleanup_flag;

/* Cleanup handler */
void a_cleanup_func_posix_1_603(void *flag_val)
{
	cleanup_flag = (long)flag_val;
	return;
}

/* Function that the thread executes upon its creation */
void *a_thread_func_posix_1_603()
{
	pthread_cleanup_push(a_cleanup_func_posix_1_603, (void *)CLEANUP_CALLED);

	pthread_exit(0);
	pthread_cleanup_pop(0);
	return NULL;
}
CASE(test_pthread, posix_1_603)
{
    pthread_t new_th;

	/* Initializing values */
	cleanup_flag = CLEANUP_NOTCALLED;

	/* Create a new thread. */
	if (pthread_create(&new_th, NULL, a_thread_func_posix_1_603, NULL) != 0) {
		ASSERT_FAIL();
        // perror("Error creating thread\n");
		// return PTS_UNRESOLVED;
	}

	/* Wait for thread to end execution */
	if (pthread_join(new_th, NULL) != 0) {
		ASSERT_FAIL();
        // perror("Error in pthread_join()\n");
		// return PTS_UNRESOLVED;
	}

	/* Check to verify that the cleanup handler was called */
	if (cleanup_flag != CLEANUP_CALLED) {
		printf("Test FAILED: Cleanup handler not called upon exit\n");
		ASSERT_FAIL();
        //return PTS_FAIL;
	}
}

#endif /* TEST_CONFIG_PTHREAD_ENABLED */

/* posix pthread test suite */
SUITE(test_pthread) = {
#if (TEST_CONFIG_PTHREAD_ENABLED > 0)
    ADD_CASE(test_pthread, posix_1_101),
    ADD_CASE(test_pthread, posix_1_102),
    ADD_CASE(test_pthread, posix_1_103),
    ADD_CASE(test_pthread, posix_1_104),
    ADD_CASE(test_pthread, posix_1_105),
    ADD_CASE(test_pthread, posix_1_201),
    ADD_CASE(test_pthread, posix_1_202),
    ADD_CASE(test_pthread, posix_1_203),
    ADD_CASE(test_pthread, posix_1_204),
    ADD_CASE(test_pthread, posix_1_205),
    ADD_CASE(test_pthread, posix_1_206),
    ADD_CASE(test_pthread, posix_1_207),
    ADD_CASE(test_pthread, posix_1_301),
    ADD_CASE(test_pthread, posix_1_302),
    ADD_CASE(test_pthread, posix_1_303),
    //ADD_CASE(test_pthread, posix_1_304),//pthread_create(&child, &attr, threaded_posix_1_304, &ret); fail
    ADD_CASE(test_pthread, posix_1_305),
    ADD_CASE(test_pthread, posix_1_401),
    ADD_CASE(test_pthread, posix_1_402),
    ADD_CASE(test_pthread, posix_1_501),
    ADD_CASE(test_pthread, posix_1_601),
    ADD_CASE(test_pthread, posix_1_602),
    ADD_CASE(test_pthread, posix_1_603),
#endif
    ADD_CASE_NULL
};


#if (TEST_CONFIG_SEM_ENABLED > 0)

/*
The following test case initializes an unnamed semaphore with a value of 1,
and then check the value of the semaphore.
*/

CASE(test_sem, posix_2_101)
{
    sem_t mysemp;
	int sts;
	int val;

	sts = sem_init(&mysemp, 0, 1);

	if (sem_getvalue(&mysemp, &val) == -1) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_getvalue");
		// return PTS_UNRESOLVED;
	}

	if ((sts == -1) && (val != 1)) {
        ASSERT_FAIL();
		// puts("TEST FAILED");
		// return PTS_FAIL;
	} else {
		// puts("TEST PASSED");
		sem_destroy(&mysemp);
		// return PTS_PASS;
	}
}

/*
 * unamed semaphore is used in subsequent of sem_wait.
*/

CASE(test_sem, posix_2_102)
{
    sem_t mysemp;
	int val;

	if (sem_init(&mysemp, 0, 1) == -1) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_init");
		// return PTS_UNRESOLVED;
	}

	if (sem_trywait(&mysemp) == -1) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "trywait");
		// return PTS_UNRESOLVED;
	}

	if (sem_getvalue(&mysemp, &val) < 0) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_getvalue");
		// return PTS_UNRESOLVED;
	}

	if (val <= 0) {
		//puts("TEST PASSED");
		sem_destroy(&mysemp);
		//return PTS_PASS;
	} else {
		ASSERT_FAIL();
        // puts("TEST FAILED");
		// return PTS_FAIL;
	}
}

/*
 * unamed semaphore is used in subsequent of sem_post.
*/
CASE(test_sem, posix_2_103)
{
    sem_t mysemp;
	int val;

	if (sem_init(&mysemp, 0, 0) == -1) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_init");
		// return PTS_UNRESOLVED;
	}

	if (sem_post(&mysemp) == -1) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "trywait");
		// return PTS_UNRESOLVED;
	}

	if (sem_getvalue(&mysemp, &val) < 0) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_getvalue");
		// return PTS_UNRESOLVED;
	}

	if (val == 1) {
		//puts("TEST PASSED");
		sem_destroy(&mysemp);
		// return PTS_PASS;
	} else {
		ASSERT_FAIL();
        // puts("TEST FAILED");
		// return PTS_FAIL;
	}
}

/*
 *  This test case illustrate the semaphore is shared between processes when
 *  pshared value is non-zero.
 */

sem_t psem, csem;
int n;
static void *producer_posix_2_104(void *arg)
{
	int i, cnt;
	cnt = (long)arg;
	for (i = 0; i < cnt; i++) {
		sem_wait(&psem);
		n++;
		sem_post(&csem);
	}
	return NULL;
}

static void *consumer_posix_2_104(void *arg)
{
	int i, cnt;
	cnt = (long)arg;
	for (i = 0; i < cnt; i++) {
		sem_wait(&csem);
		sem_post(&psem);
	}
	return NULL;
}

CASE(test_sem, posix_2_104)
{
    pthread_t prod, cons;
	// void *producer(void *);
	// void *consumer(void *);
	long cnt = 3;

	n = 0;
	if (sem_init(&csem, 0, 0) < 0) {
		ASSERT_FAIL();
        // perror("sem_init");
		// return PTS_UNRESOLVED;
	}
	if (sem_init(&psem, 0, 1) < 0) {
		ASSERT_FAIL();
        // perror("sem_init");
		// return PTS_UNRESOLVED;
	}
	if (pthread_create(&prod, NULL, producer_posix_2_104, (void *)cnt) != 0) {
		ASSERT_FAIL();
        // perror("pthread_create");
		// return PTS_UNRESOLVED;
	}
	if (pthread_create(&cons, NULL, consumer_posix_2_104, (void *)cnt) != 0) {
		ASSERT_FAIL();
        // perror("pthread_create");
		// return PTS_UNRESOLVED;
	}

	if ((pthread_join(prod, NULL) == 0) && (pthread_join(cons, NULL) == 0)) {
		// puts("TEST PASS");
		pthread_exit(NULL);
		if(sem_destroy(&psem) != 0)
            ASSERT_FAIL();
		if(sem_destroy(&csem) != 0)
            ASSERT_FAIL();
		// return PTS_PASS;
	} else {
		ASSERT_FAIL();
        // puts("TEST FAILED");
		// return PTS_FAIL;
	}
}

/*
 * un-successful unamed semaphore initialization return -1, otherwise zero
 * on successful completion.
*/

CASE(test_sem, posix_2_105)
{
    sem_t mysemp;

	if (sem_init(&mysemp, 0, 1) == 1) {
		ASSERT_FAIL();
        // puts("TEST FAILED");
		// return PTS_FAIL;
	} else {
		//puts("TEST PASSED");
		if(sem_destroy(&mysemp) != 0)
            ASSERT_FAIL();
		//return PTS_PASS;
	}
}

/*
 * sem_init shall fail if the valueargument exceeds SEM_VALUE_MAX.
 */

CASE(test_sem, posix_2_106)
{
    sem_t mysemp;
	int counter = 32767;//SEM_VALUE_MAX;

	// if (SEM_VALUE_MAX >= INT_MAX) {
	// 	//puts("Test skipped");
	// 	//return PTS_PASS;
	// }

	++counter;

	if (sem_init(&mysemp, 0, counter) != 0) {//EINVAL
		//puts("TEST PASSED");
		//return PTS_PASS;
	} else {
		printf("sem_init SEM_VALUE_MAX counter failed\n");
		ASSERT_FAIL();
        // puts("TEST FAILED");
		// return PTS_FAIL;
	}
}

/*
   sem_open test case that attempts to open a new semaphoree,
   close the semaphore and verify that open_sem returns 0.
 */

CASE(test_sem, posix_2_201)
{
    sem_t mysemp;
	//char semname[50];

	//printf(semname, "/" FUNCTION "_" TEST "_%d", getpid());

	// mysemp = sem_open(semname, O_CREAT, 0777, 1);
    sem_init(&mysemp, 0, 1);
	if (mysemp == NULL) {
		ASSERT_FAIL();
        // puts("TEST FAILED");
		// return PTS_FAIL;
	} else {
		// puts("TEST PASSED");
		sem_close(mysemp);
		sem_destroy(mysemp);
		//sem_unlink(semname);
		// return PTS_PASS;
	}
}
/*
 * sem_post shall unlock the locked semaphore and increment the semaphore
 * value by one.
 */
CASE(test_sem, posix_2_301)
{
    sem_t mysemp;
	// char semname[28];
	int val;

	//sprintf(semname, "/" FUNCTION "_" TEST "_%d", getpid());

	/* Initial value of Semaphore is 0 */
	// mysemp = sem_open(semname, O_CREAT, 0777, 0);
	// printf("sem_init\n");
    sem_init(&mysemp, 0, 0);
	if (mysemp == NULL) {
		printf("== NULL\n");
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_open");
		// return PTS_UNRESOLVED;
	}

	if (sem_post(mysemp) == 1) {
		printf("sem_post\n");
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_post");
		// return PTS_UNRESOLVED;
	}

	if (sem_getvalue(mysemp, &val) == -1) {
		printf("sem_getvalue\n");
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_getvalue");
		// return PTS_UNRESOLVED;

		/* Checking if the value of the Semaphore incremented by one */
	} else if (val == 1) {
		//puts("TEST PASSED");
		sem_close(mysemp);
		// sem_unlink(semname);
		sem_destroy(mysemp);
		//return PTS_PASS;
	} else {
		printf("val fail\n");
		ASSERT_FAIL();
        // puts("TEST FAILED");
		// return PTS_FAIL;
	}
}

/*
 * sem_post shall increment the value of the semaphore when its unlocked
 * and in positive value
 */

CASE(test_sem, posix_2_302)
{
    sem_t mysemp;
	char semname[28];
	int val;

	//sprintf(semname, "/" FUNCTION "_" TEST "_%d", getpid());

	/* Initial value of Semaphore is unlocked */
    sem_init(&mysemp, 0 , 2);
	//mysemp = sem_open(semname, O_CREAT, 0777, 2);
	if (mysemp == NULL) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_open");
		// return PTS_UNRESOLVED;
	}

	if (sem_post(mysemp) == -1) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_post");
		// return PTS_UNRESOLVED;
	}

	if (sem_getvalue(mysemp, &val) == -1) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_getvalue");
		// return PTS_UNRESOLVED;

		/* Checking if the value of the Semaphore incremented by one */
	} else if (val == 3) {
		//puts("TEST PASSED");
		sem_close(mysemp);
		sem_unlink(semname);
		sem_destroy(mysemp);
		//return PTS_PASS;
	} else {
		ASSERT_FAIL();
        // puts("TEST FAILED");
		// return PTS_FAIL;
	}
}


/*
 * This test case verifies the semaphore value is 0, then shows a successful
 * call to release the unlock from mysemp.
 */

CASE(test_sem, posix_2_303)
{
    sem_t mysemp;
	char semname[28];

	//sprintf(semname, "/" FUNCTION "_" TEST "_%d", getpid());

	/* Initial value of Semaphore is 1 */
    sem_init(&mysemp, 0 , 1);
	//mysemp = sem_open(semname, O_CREAT, 0777, 1);
	if (mysemp == NULL) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_open");
		// return PTS_UNRESOLVED;
	}

	if (sem_wait(mysemp) == 1) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_post");
		// return PTS_UNRESOLVED;
	}

	if (sem_post(mysemp) == 0) {
		//puts("TEST PASSED");
		sem_close(mysemp);
		sem_unlink(semname);
		sem_destroy(mysemp);
		//return PTS_PASS;
	} else {
		ASSERT_FAIL();
        // puts("TEST FAILED: value of sem_post is not zero");
		// return PTS_FAIL;
	}
}

/*
 * sem_post() can be called from within timer handler
 *
 * 1) Set up a timer.
 * 2) Set up a semaphore.
 * 3) in timer recall handler.
 * 4) Call sem_post() from within the handler for timer
 * 5) If sempost() succeeds (verified if val is incremented by 1),
 *    test passes.  Otherwise, failure.
 */
static sem_t gsemp;
static aos_timer_t  g_timer;
void timer_handler_posix_2_304(void *arg1, void* arg2)
{
	if (sem_post(gsemp) != 0) {
		ASSERT_FAIL();
		printf("sem_post in timer failed\n");
        // perror(ERROR_PREFIX "sem_post");
		// exit(PTS_UNRESOLVED);
	}
}

CASE(test_sem, posix_2_304)
{
    char semname[28];
	// struct sigaction act;
	int val, ret;

	// sprintf(semname, "/" FUNCTION "_" TEST "_%d", getpid());

	//gsemp = sem_open(semname, O_CREAT, 0777, SEMINITVAL);
    ret = sem_init(&gsemp, 0 , 0);
	ASSERT_EQ(ret, 0);

	// act.sa_handler = handler;
	// act.sa_flags = 0;

	// if (sigemptyset(&act.sa_mask) == -1) {
	// 	perror("Error calling sigemptyset\n");
	// 	return PTS_UNRESOLVED;
	// }
	// if (sigaction(SIGALRM, &act, 0) == -1) {
	// 	perror("Error calling sigaction\n");
	// 	return PTS_UNRESOLVED;
	// }

    ret = aos_timer_new(&g_timer, timer_handler_posix_2_304, NULL, 200, 0);

	ASSERT_NOT_NULL(gsemp);
	// if (gsemp == NULL) {
	// 	ASSERT_FAIL();
    //     // perror(ERROR_PREFIX "sem_open");
	// 	// return PTS_UNRESOLVED;
	// }

	// alarm(1);
	krhino_task_sleep(300);

	/* Checking if the value of the Semaphore incremented by one */
	if (sem_getvalue(gsemp, &val) != 0) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_getvalue");
		// return PTS_UNRESOLVED;
	}
	ASSERT_EQ(val, 1);
	// if (val != 1) {
    //     ASSERT_FAIL();
	// 	// printf("TEST FAILED\n");
	// 	// return PTS_FAIL;
	// }

	// printf("TEST PASSED\n");
	sem_close(gsemp);
	sem_unlink(semname);
	sem_destroy(gsemp);
	// return PTS_PASS;
}

/*
 * sem_timedwait shall lock the unlocked semaphore and decrement the
 * semaphore * value by one.
 */

CASE(test_sem, posix_2_401)
{
    sem_t mysemp;
	struct timespec ts;
	int val, sts;

	if (sem_init(&mysemp, 0, 1) == -1) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_init");
		// return PTS_UNRESOLVED;
	}

	ts.tv_sec = time(NULL);
	ts.tv_nsec = 0;

	/* Lock Semaphore */
	sts = sem_timedwait(&mysemp, &ts);
	if (sts == -1) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_timedwait");
		// return PTS_UNRESOLVED;
	}

	/* Value of Semaphore */
	if (sem_getvalue(&mysemp, &val) == -1) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_getvalue");
		// return PTS_UNRESOLVED;
	}

	/* Checking if the value of the Semaphore decremented by one */
	if (val == 0 && sts == 0) {
		//puts("TEST PASSED");
		sem_destroy(&mysemp);
		// return PTS_PASS;
	} else {
		ASSERT_FAIL();
        // puts("TEST FAILED");
		// return PTS_FAIL;
	}
}

/* sem_timedwait will return successfully when sem_post
 * will unlock the semaphore from another process.
 */

static void *pth_wait_posix_2_402()
{
    struct timespec ts;
    ts.tv_sec = time(NULL) + 2;
    ts.tv_nsec = 0;

    if (sem_timedwait(&gsemp, &ts) == -1) {
        ASSERT_FAIL();
        // puts("TEST FAILED");
        // return PTS_FAIL;
    } else {
        //puts("TEST PASSED");
        sem_destroy(&gsemp);
        //return PTS_PASS;
    }
    pthread_exit(0);
}

static void *pth_post_posix_2_402()
{
    int i;
    krhino_task_sleep(1);
    if (sem_post(&gsemp) == -1) {
        ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_post");
        // return PTS_FAIL;
    }
    // if (wait(&i) == -1) {
    //     ASSERT_FAIL();
    //     // perror("Error waiting for child to exit");
    //     // return PTS_UNRESOLVED;
    // }

    // if (!WEXITSTATUS(i)) {
    //     ASSERT_FAIL();
    //     // return PTS_FAIL;
    // }
    // puts("TEST PASSED");
    sem_destroy(&gsemp);
    // return PTS_PASS;
    pthread_exit(0);
}

CASE(test_sem, posix_2_402)
{
    pthread_t pth_wait, pth_post;
    int ret;

    sem_init(gsemp, 0 , 0);

    ret = pthread_create(&pth_wait, NULL, pth_wait_posix_2_402, NULL);
    ASSERT_EQ(ret, 0);

    ret = pthread_create(&pth_post, NULL, pth_post_posix_2_402, NULL);
    ASSERT_EQ(ret, 0);

    ret = pthread_join(pth_wait, NULL);
    ASSERT_EQ(ret, 0);

    ret = pthread_join(pth_post, NULL);
    ASSERT_EQ(ret, 0);

}

/*
 * This test case will verify that sem_timedwait will wait one second to
 * unlock the locked semaphore, and then when the semaphore fails to
 * unlock should return -1, and leave the semaphore status unchanged
 * by doing sem_post on the sempahore and check the current value of
 * the semaphore.
 */

CASE(test_sem, posix_2_403)
{
    sem_t mysemp;
	struct timespec ts;
	int sts, val;

	if (sem_init(&mysemp, 0, 0) != 0) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_init");
		// return PTS_UNRESOLVED;
	}

	// ts.tv_sec = (krhino_sys_time_get()/1000) + 1;
	// ts.tv_sec = 3;
	ts.tv_sec = time(NULL);
	ts.tv_nsec = 0;
	ts.tv_sec += 10;
	printf("sem_timedwait for %d second\n", ts.tv_sec);
	/* Try to lock Semaphore */
	sts = sem_timedwait(mysemp, &ts);

	if (sem_post(mysemp) != 0) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_post");
		// return PTS_UNRESOLVED;
	}

	if (sem_getvalue(mysemp, &val) != 0) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_getvalue");
		// return PTS_UNRESOLVED;
	}

	if ((val == 1) && (sts != 0)) {
		// puts("TEST PASSED");
		sem_destroy(mysemp);
		// return PTS_PASS;
	} else {
		ASSERT_FAIL();
        // puts("TEST FAILED");
		// return PTS_FAIL;
	}
}

/* This tests case will open a locked semaphore.  The time will tick 5 times
 * until the absolute time passes.  The sempahore will unlock, then the
 * sem_timedwait call will immediately lock again.
 */
CASE(test_sem, posix_2_404)
{
    struct timespec ts;
	sem_t mysemp;
	int i = 0;
	int val;

	if (sem_init(&mysemp, 0, 0) == -1) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_init");
		// return PTS_UNRESOLVED;
	}

	ts.tv_sec = time(NULL);
	ts.tv_nsec = 0;

	while (sem_timedwait(&mysemp, &ts) == -1) {
		ts.tv_sec += 1;
//              printf("%s \n", asctime(localtime(&ts.tv_sec)));
		i++;
//              printf("i=%d\n",i);
		if (i == 5) {
			sem_post(&mysemp);
		}
	}

	/* Value of Semaphore */
	if (sem_getvalue(&mysemp, &val) == -1) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_getvalue");
		// return PTS_UNRESOLVED;
	}

	/* Checking if the value of the Semaphore after lock & unlock */
	if (val == 0) {
		// puts("TEST PASSED: Sem unlocked after 5 timeouts");
		sem_destroy(&mysemp);
		// return PTS_PASS;
	} else {
		ASSERT_FAIL();
        // puts("TEST FAILED");
		// return PTS_FAIL;
	}
}

/*
 * The process would be blocked, and the timeout parameter is
 * secified in nanoseconds field value less than zero.  Should
 * return ERROR (EINVAL).
 */

CASE(test_sem, posix_2_405)
{
    sem_t mysemp;
	struct timespec ts;
	int sts;

	if (sem_init(&mysemp, 0, 0) != 0) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_init");
		// return PTS_UNRESOLVED;
	}
	ts.tv_sec = time(NULL);
	ts.tv_nsec = -3;

	sts = sem_timedwait(&mysemp, &ts);

	if (sts != 0) {
		// puts("TEST PASSED");
		sem_destroy(&mysemp);
		// return PTS_PASS;
	} else {
		ASSERT_FAIL();
		printf("sem_timedwait -3ns pass, this is wrong..\n");
        // puts("TEST FAILED");
		// return PTS_FAIL;
	}
}

/*
 * Under no circumstance shall the function fail with a timeout if
 * the semaphore can be locked immediately. The validity of the
 * abs_timeout need not be checked if the semaphore can be locked
 * immediately.
 */

CASE(test_sem, posix_2_406)
{
    sem_t mysemp[2];
	struct timespec ts[2];
	int val[2], sts[2];
	int i;

	for (i = 0; i < 2; i++) {
		if (sem_init(&mysemp[i], 0, 1) == -1) {
			ASSERT_FAIL();
            // perror(ERROR_PREFIX "sem_init");
			// return PTS_UNRESOLVED;
		}
		if (i == 0) {
			ts[i].tv_sec = time(NULL) + 2;
			ts[i].tv_nsec = 0;
		} else if (i == 1) {
			ts[i].tv_sec = time(NULL) - 2;
			ts[i].tv_nsec = 0;
		}
		/* Lock Semaphore */
		sts[i] = sem_timedwait(&mysemp[i], &ts[i]);
		if (sts[i] == -1) {
			ASSERT_FAIL();
            // perror(ERROR_PREFIX "sem_timedwait");
			// return PTS_UNRESOLVED;
		}

		/* Value of Semaphore */
		if (sem_getvalue(&mysemp[i], &val[i]) == -1) {
			ASSERT_FAIL();
            // perror(ERROR_PREFIX "sem_getvalue");
			// return PTS_UNRESOLVED;
		}

		/* Checking if the value of the Semaphore decremented by one */
		if ((val[i] == 0) && (sts[i] == 0)) {
			// puts("TEST PASSED");
			sem_destroy(&mysemp[i]);
			// return PTS_PASS;
		} else {
			// puts("TEST FAILED");
            ASSERT_FAIL();
			sem_destroy(&mysemp[i]);
			// return PTS_FAIL;
		}
	}
}

/*
 * sem_wait shall lock the unlocked semaphore and decrement the semaphore
 * value by one.
 */

CASE(test_sem, posix_2_501)
{
    sem_t mysemp;
	char semname[28];
	int val;

	// sprintf(semname, "/" FUNCTION "_" TEST "_%d", getpid());

	/* Initial value of Semaphore is 1 */
	// mysemp = sem_open(semname, O_CREAT, 0777, 1);
	// if (mysemp == SEM_FAILED || mysemp == NULL) {
	// 	perror(ERROR_PREFIX "sem_open");
	// 	return PTS_UNRESOLVED;
	// }

    if (sem_init(&mysemp, 0, 1) == 1)
        ASSERT_FAIL();

	/* Lock Semaphore */
	if (sem_wait(mysemp) != 0) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_wait");
		// return PTS_UNRESOLVED;
	}

    /* Try to Lock Semaphore */
    if (sem_trywait(mysemp) == 0) {
        ASSERT_FAIL();
    // perror(ERROR_PREFIX "sem_wait");
    // return PTS_UNRESOLVED;
    }

	/* Value of Semaphore */
	if (sem_getvalue(mysemp, &val) != 0) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_getvalue");
		// return PTS_UNRESOLVED;

	/* Checking if the value of the Semaphore decremented by one */
	} else if (val == 0) {
		// puts("TEST PASSED");
		sem_close(mysemp);
		sem_unlink(semname);
		sem_destroy(mysemp);
		// return PTS_PASS;
	} else {
		ASSERT_FAIL();
        // puts("TEST FAILED");
		// return PTS_FAIL;
	}
}

/*
 * Keep calling sem_wait until the sempahore is locked.  That is
 * decrementing the semaphore value by one until its zero or locked.
 */

CASE(test_sem, posix_2_502)
{
    sem_t mysemp;
	char semname[28];
	int value = 10;
	int val;

	// sprintf(semname, "/" FUNCTION "_" TEST "_%d", getpid());

	/* Initial value of Semaphore is 10 */
	// mysemp = sem_open(semname, O_CREAT, 0777, value);
	// if (mysemp == SEM_FAILED || mysemp == NULL) {
	// 	perror(ERROR_PREFIX "sem_open");
	// 	return PTS_UNRESOLVED;
	// }
    if (sem_init(&mysemp, 0, value) != 0)
	{
		printf("sem_init fail\n");
        ASSERT_FAIL();
	}


    if (sem_trywait(mysemp) != 0) {
		printf("sem_trywait fail\n");
        ASSERT_FAIL();
    }else{
		value--;
	}

	while (value) {		// checking the value if zero yet.
		if (sem_wait(mysemp) != 0) {
			printf("sem_wait many times fail\n");
			ASSERT_FAIL();
            // perror(ERROR_PREFIX "sem_getvalue");
			// return PTS_UNRESOLVED;
		} else {
			value--;
		}
	}

	if (sem_getvalue(mysemp, &val) != 0) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_getvalue");
		// return PTS_UNRESOLVED;
	} else if (val == 0) {
		// puts("TEST PASSED");
		sem_unlink(semname);
		sem_close(mysemp);
        sem_destroy(mysemp);
		// return PTS_PASS;
	} else {
		printf("val != 0, fail\n");
		ASSERT_FAIL();
        // puts("TEST FAILED: Semaphore is not locked");
		// return PTS_FAIL;
	}
}

/* The steps are:
* -> Initialize a semaphore to 0 count
* -> Register a timer to wait the semaphore
* -> save current time
* -> sem_wait
* -> Verify either errno is EINTR or no error occured.
* -> Verify 1 sec has elapsed.
* The test fails if the call did not block.
*/

void timer_handler_posix_2_503(void *arg1, void* arg2)
{
	if (sem_wait(gsemp) != 1) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_post");
		// exit(PTS_UNRESOLVED);
	}
}
CASE(test_sem, posix_2_503)
{
    int ret;
    sem_init(gsemp, 0 , 0);
    ret = aos_timer_new(&g_timer, timer_handler_posix_2_503, NULL, 200, 0);
    krhino_task_sleep(300);

}

/*
 * This test case will call sem_getvalue to update the location referenced
 * by the semaphpre without effecting the state of the semaphore.  The
 * updated value represents the actual semaphore value when it was called.
 */

CASE(test_sem, posix_2_601)
{
    // char semname[NAME_MAX - 4];
	sem_t mysemp;
	int val;

	// snprintf(semname, sizeof(semname), "/" FUNCTION "_" TEST "_%d", getpid());

	// mysemp = sem_open(semname, O_CREAT, 0777, 1);
    sem_init(&mysemp, 0 , 1);
	if (mysemp == NULL) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_open");
		// return PTS_UNRESOLVED;
	}

	if (sem_getvalue(mysemp, &val) != 0) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_getvalue");
		// return PTS_UNRESOLVED;
	}

	/*
	   printf("Current value is: %d\n", val);
	 */

	if (val == 1) {
		// puts("TEST PASSED");
		sem_close(mysemp);
		// sem_unlink(semname);
		// return PTS_PASS;
	} else {
		ASSERT_FAIL();
        // puts("TEST FAILED");
		// return PTS_FAIL;
	}

    if (sem_trywait(mysemp) != 0) {
        ASSERT_FAIL();
    }

    if (sem_getvalue(mysemp, &val) != 0) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_getvalue");
		// return PTS_UNRESOLVED;
	}
    if (val == 0) {
		// puts("TEST PASSED");
		sem_close(mysemp);
		sem_destroy(mysemp);
		// sem_unlink(semname);
		// return PTS_PASS;
	} else {
		ASSERT_FAIL();
        // puts("TEST FAILED");
		// return PTS_FAIL;
	}
}

/*
 * Test case vrifies sem_destroy shall destroy on intialized semaphore
 * upon which no threads are currently blocked.
 */

sem_t psem, csem;
int n;

void *producer_posix_2_701(void *arg)
{
	int i, cnt;
	cnt = (long)arg;
	for (i = 0; i < cnt; i++) {
		sem_wait(&psem);
		n++;
		sem_post(&csem);
	}
	return NULL;
}

void *consumer_posix_2_701(void *arg)
{
	int i, cnt;
	cnt = (long)arg;
	for (i = 0; i < cnt; i++) {
		sem_wait(&csem);
		sem_post(&psem);
	}
	return NULL;
}

CASE(test_sem, posix_2_701)
{
    pthread_t prod, cons;
	long cnt = 3;

	n = 0;
	if (sem_init(&csem, 0, 0) < 0) {
		ASSERT_FAIL();
        // perror("sem_init");
		// return PTS_UNRESOLVED;
	}
	if (sem_init(&psem, 0, 1) < 0) {
		ASSERT_FAIL();
        // perror("sem_init");
	}
	if (pthread_create(&prod, NULL, producer_posix_2_701, (void *)cnt) != 0) {
		ASSERT_FAIL();
        // perror("sem_init");
		// return PTS_UNRESOLVED;
	}
	if (pthread_create(&cons, NULL, consumer_posix_2_701, (void *)cnt) != 0) {
		ASSERT_FAIL();
        // perror("sem_init");
		// return PTS_UNRESOLVED;
	}

	if (pthread_join(prod, NULL) == 0 && pthread_join(cons, NULL) == 0) {
		// puts("TEST PASS");
		pthread_exit(NULL);
		if ((sem_destroy(&psem) == 0) && sem_destroy(&csem) == 0) {
			// return PTS_PASS;
		} else {
			ASSERT_FAIL();
            // perror("sem_init");
		    // return PTS_UNRESOLVED;
		}
	}
}

/*
 * This test case verify the unamed semaphore is destroyed by calling
 * sem_destroy to return 0 on successful call.
*/
CASE(test_sem, posix_2_702)
{
    sem_t mysemp;

	if (sem_init(&mysemp, 0, 0) == -1) {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_init");
		// return PTS_UNRESOLVED;
	}

	if (sem_destroy(&mysemp) == 0) {
		// puts("TEST PASSED");
		// return PTS_PASS;
	} else {
		ASSERT_FAIL();
        // perror(ERROR_PREFIX "sem_init");
		// return PTS_UNRESOLVED;
	}
}


#endif /* TEST_CONFIG_SEM_ENABLED */

/* posix sem test suite */
SUITE(test_sem) = {
#if (TEST_CONFIG_SEM_ENABLED > 0)
    ADD_CASE(test_sem, posix_2_101),
    ADD_CASE(test_sem, posix_2_102),
    ADD_CASE(test_sem, posix_2_103),
    ADD_CASE(test_sem, posix_2_104),
    ADD_CASE(test_sem, posix_2_105),
    ADD_CASE(test_sem, posix_2_106),
    ADD_CASE(test_sem, posix_2_201),
    ADD_CASE(test_sem, posix_2_301),
    ADD_CASE(test_sem, posix_2_302),
    ADD_CASE(test_sem, posix_2_303),
    ADD_CASE(test_sem, posix_2_304),
    ADD_CASE(test_sem, posix_2_401),
    ADD_CASE(test_sem, posix_2_402),
    ADD_CASE(test_sem, posix_2_403),
    ADD_CASE(test_sem, posix_2_404),
    ADD_CASE(test_sem, posix_2_405),
    ADD_CASE(test_sem, posix_2_406),
    ADD_CASE(test_sem, posix_2_501),
    ADD_CASE(test_sem, posix_2_502),
    ADD_CASE(test_sem, posix_2_503),
    ADD_CASE(test_sem, posix_2_601),
#endif
    ADD_CASE_NULL
};


#if (TEST_CONFIG_MUX_ENABLED > 0)

/* Test that pthread_mutex_init()
 *   initializes a mutex referenced by 'mutex' with attributes specified
 *   by 'attr'.  If 'attr' is NULL, the default mutex attributes are used.
 *   The effect shall be the same as passing the address of a default
 *   mutex attributes.
 */
CASE(test_mux, posix_3_101)
{
    pthread_mutexattr_t mta;
	pthread_mutex_t mutex1, mutex2;
	int rc;

	/* Initialize a mutex attributes object */
	if ((rc = pthread_mutexattr_init(&mta)) != 0) {
		ASSERT_FAIL();
        // fprintf(stderr, "Error at pthread_mutexattr_init(), rc=%d\n", rc);
		// return PTS_UNRESOLVED;
	}

	/* Initialize mutex1 with the default mutex attributes */
	if ((rc = pthread_mutex_init(&mutex1, &mta)) != 0) {
		ASSERT_FAIL();
        // fprintf(stderr, "Fail to initialize mutex1, rc=%d\n", rc);
		// printf("Test FAILED\n");
		// return PTS_FAIL;
	}

	/* Initialize mutex2 with NULL attributes */
	if ((rc = pthread_mutex_init(&mutex2, NULL)) != 0) {
		ASSERT_FAIL();
        // fprintf(stderr, "Fail to initialize mutex2, rc=%d\n", rc);
		// printf("Test FAILED\n");
		// return PTS_FAIL;
	}
}

CASE(test_mux, posix_3_102)
{
    pthread_mutex_t mutex;
	int rc;

	/* Initialize a mutex object */
	if ((rc = pthread_mutex_init(&mutex, NULL)) != 0) {
        ASSERT_FAIL();
		// fprintf(stderr, "Fail to initialize mutex, rc=%d\n", rc);
		// printf("Test FAILED\n");
		// return PTS_FAIL;
	}

	/* Acquire the mutex object using pthread_mutex_lock */
	if ((rc = pthread_mutex_lock(&mutex)) != 0) {
        ASSERT_FAIL();
		// fprintf(stderr, "Fail to lock the mutex, rc=%d\n", rc);
		// printf("Test FAILED\n");
		// return PTS_FAIL;
	}
	// fprintf(stderr, "Main: hold the mutex for a while\n");
	krhino_task_sleep(1);

	/* Release the mutex object using pthread_mutex_unlock */
	if ((rc = pthread_mutex_unlock(&mutex)) != 0) {
        ASSERT_FAIL();
		// fprintf(stderr, "Fail to unlock the mutex, rc=%d\n", rc);
		// return PTS_UNRESOLVED;
	}

	/* Destory the mutex object */
	if ((rc = pthread_mutex_destroy(&mutex)) != 0) {
        ASSERT_FAIL();
		// fprintf(stderr, "Fail to destory the mutex, rc=%d\n", rc);
		// return PTS_UNRESOLVED;
	}
}


 /* Test that pthread_mutex_init()
 *   Upon successful initialization, the state of the mutex becomes
 *   initialized and unlocked.
 *
 */

CASE(test_mux, posix_3_201)
{
	pthread_mutexattr_t mta;
	pthread_mutex_t mutex1, mutex2;
	pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
	int rc;

	/* Initialize a mutex attributes object */
	if ((rc = pthread_mutexattr_init(&mta)) != 0) {
		printf("pthread_mutexattr_init(&mta) fail\n");
		ASSERT_FAIL();
		// fprintf(stderr, "Error at pthread_mutexattr_init(), rc=%d\n", rc);
		// return PTS_UNRESOLVED;
	}

	/* Initialize mutex1 with the default mutex attributes */
	if ((rc = pthread_mutex_init(&mutex1, &mta)) != 0) {
		printf("pthread_mutex_init(&mutex1, &mta) fail\n");
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to initialize mutex1, rc=%d\n", rc);
		// return PTS_UNRESOLVED;
	}

	/* Initialize mutex2 with NULL attributes */
	if ((rc = pthread_mutex_init(&mutex2, NULL)) != 0) {
		printf("pthread_mutex_init(&mutex2, NULL) fail\n");
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to initialize mutex2, rc=%d\n", rc);
		// return PTS_UNRESOLVED;
	}

	/* Destroy the mutex attributes object */
	if ((rc = pthread_mutexattr_destroy(&mta)) != 0) {
		printf("pthread_mutexattr_destroy(&mta) fail\n");
		ASSERT_FAIL();
		// fprintf(stderr, "Error at pthread_mutexattr_destroy(), rc=%d\n", rc);
		// return PTS_UNRESOLVED;
	}

	/* Destroy mutex1 */
	if ((rc = pthread_mutex_destroy(&mutex1)) != 0) {
		printf("pthread_mutex_destroy(&mutex1) fail\n");
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to destroy mutex1, rc=%d\n", rc);
		// printf("Test FAILED\n");
		// return PTS_FAIL;
	}

	/* Destroy mutex2 */
	if ((rc = pthread_mutex_destroy(&mutex2)) != 0) {
		printf("pthread_mutex_destroy(&mutex2) fail\n");
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to destroy mutex2, rc=%d\n", rc);
		// printf("Test FAILED\n");
		// return PTS_FAIL;
	}

	/* Destroy mutex3 */
	if ((rc = pthread_mutex_destroy(&mutex3)) != 0) {
		printf("pthread_mutex_destroy(&mutex3)PTHREAD_MUTEX_INITIALIZER fail\n");
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to destroy mutex3, rc=%d\n", rc);
		// printf("Test FAILED\n");
		// return PTS_FAIL;
	}
}

/* Test pthread_mutex_destroy() that
 *   a destroyed mutex object can be reinitialized using pthread_mutex_init()
 */
CASE(test_mux, posix_3_202)
{
	pthread_mutex_t mutex;

	/* Initialize a mutex attributes object */
	if (pthread_mutex_init(&mutex, NULL) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Cannot initialize mutex object\n");
		// return PTS_UNRESOLVED;
	}

	/* Destroy the mutex attributes object */
	if (pthread_mutex_destroy(&mutex) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Cannot destroy the mutex object\n");
		// return PTS_UNRESOLVED;
	}

	/* Initialize the mutex attributes object again.  This shouldn't result in an error. */
	if (pthread_mutex_init(&mutex, NULL) != 0) {
		ASSERT_FAIL();
		// printf("Test FAILED\n");
		// return PTS_FAIL;
	} else {
		// printf("Test PASSED\n");
		// return PTS_PASS;
	}
}

/* Test that pthread_mutex_destroy()
 * 	It shall be safe to destroy an initialized mutex that is unlocked.
 */

CASE(test_mux, posix_3_203)
{
	pthread_mutex_t mutex;
	int rc;

	/* Initialize mutex with the default mutex attributes */
	if ((rc = pthread_mutex_init(&mutex, NULL)) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to initialize mutex, rc=%d\n", rc);
		// return PTS_UNRESOLVED;
	}

	/* Lock mutex */
	if ((rc = pthread_mutex_lock(&mutex)) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Error at pthread_mutex_lock(), rc=%d\n", rc);
		// return PTS_UNRESOLVED;
	}
	krhino_task_sleep(1);
	/* Unlock */
	if ((rc = pthread_mutex_unlock(&mutex)) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Error at pthread_mutex_unlock(), rc=%d\n", rc);
		// return PTS_UNRESOLVED;
	}
	/* Destroy mutex after it is unlocked */
	if ((rc = pthread_mutex_destroy(&mutex)) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr,"Fail to destroy mutex after being unlocked, rc=%d\n", rc);
		// printf("Test FAILED\n");
		// return PTS_FAIL;
	}
}

/* Steps:
 *   -- Initialize a mutex to protect a global variable 'value'
 *   -- Create N threads. Each is looped M times to acquire the mutex,
 *      increase the value, and then release the mutex.
 *   -- Check if the value has increased properly (M*N); a broken mutex
 *      implementation may cause lost augments.
 *
 */

#define    THREAD_NUM  	5
#define    LOOPS     	4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int value;			/* value protected by mutex */


void *f1_posix_3_301(void *parm)
{
	int i, tmp;
	int rc = 0;
	pthread_t self = pthread_self();

	/* Loopd M times to acquire the mutex, increase the value,
	   and then release the mutex. */

	for (i = 0; i < LOOPS; ++i) {
		//printf("thread name : %s, loop time %d\n", self->task_name, i);
		rc = pthread_mutex_lock(&mutex);
		if (rc != 0) {
			printf("pthread_mutex_lock a PTHREAD_MUTEX_INITIALIZER failed\n");
			ASSERT_FAIL();
			// fprintf(stderr, "Error on pthread_mutex_lock(), rc=%d\n", rc);
			// return (void *)(PTS_FAIL);
		}

		tmp = value;
		tmp = tmp + 1;
		// fprisntf(stderr, "Thread(0x%p) holds the mutex\n", (void *)self);
		krhino_task_sleep(10);	/* delay the increasement operation */
		value = tmp;

		rc = pthread_mutex_unlock(&mutex);
		if (rc != 0) {
			printf("pthread_mutex_unlock a PTHREAD_MUTEX_INITIALIZER failed\n");
			ASSERT_FAIL();
			// fprintf(stderr, "Error on pthread_mutex_unlock(), rc=%d\n", rc);
			// return (void *)(PTS_UNRESOLVED);
		}
		krhino_task_sleep(10);
	}
	pthread_exit(0);
	return (void *)(0);
}
CASE(test_mux, posix_3_301)
{
	int i, rc;
	// pthread_attr_t pta;
	pthread_t threads[THREAD_NUM];
	//pthread_t           self = pthread_self();

	// pthread_attr_init(&pta);
	// pthread_attr_setdetachstate(&pta, PTHREAD_CREATE_JOINABLE);

	//pthread_mutex_init(&mutex, NULL);

	/* Create threads */
	// fprintf(stderr, "Creating %d threads\n", THREAD_NUM);
	for (i = 0; i < THREAD_NUM; ++i)
		rc = pthread_create(&threads[i], NULL, f1_posix_3_301, NULL);

	/* Wait to join all threads */
	for (i = 0; i < THREAD_NUM; ++i)
		pthread_join(threads[i], NULL);
	// pthread_attr_destroy(&pta);
	pthread_mutex_destroy(&mutex);

	/* Check if the final value is as expected */
	if (value != (THREAD_NUM) * LOOPS) {
		printf("value %d != (THREAD_NUM) * LOOPS) %d\n", value, (THREAD_NUM) * LOOPS);
		ASSERT_FAIL();
		// fprintf(stderr, "Using %d threads and each loops %d times\n", THREAD_NUM, LOOPS);
		// fprintf(stderr, "Final value must be %d instead of %d\n", (THREAD_NUM) * LOOPS, value);
		// printf("Test FAILED\n");
		// return PTS_FAIL;
	}
}


/* The steps are:
 * ->Create a mutex with recursive attribute
 * ->Create a threads
 * ->Parent locks the mutex twice, unlocks once.
 * ->Child attempts to lock the mutex.
 * ->Parent unlocks the mutex.
 * ->Parent unlocks the mutex (shall fail)
 * ->Child unlocks the mutex.
 */

// pthread_mutex_t mtx;
sem_t sem_posix_3_302;

/** child thread function **/
void *threaded_posix_3_302(void *arg)
{
	int ret;
	/* Try to lock the mutex once. The call must fail here. */
	ret = pthread_mutex_trylock(&mtx);
	if (ret == 0) {
		ASSERT_FAIL();
		// FAILED("Child first trylock succeeded");
	}

	/* Free the parent thread and lock the mutex (must success) */
	if ((ret = sem_post(&sem_posix_3_302))) {
		ASSERT_FAIL();
		// UNRESOLVED(errno, "1st post sem in child failed");
	}

	if ((ret = pthread_mutex_lock(&mtx))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Child lock failed");
	}

	/* Wait for the parent to let us go on */
	if ((ret = sem_post(&sem_posix_3_302))) {
		ASSERT_FAIL();
		// UNRESOLVED(errno, "2nd post sem in child failed");
	}

	/* Unlock and exit */
	if ((ret = pthread_mutex_unlock(&mtx))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Unlock in child failed");
	}

	return NULL;
}

CASE(test_mux, posix_3_302)
{
	int ret;
	int i;
	pthread_mutexattr_t ma;
	pthread_t child;

	// output_init();

	/* Initialize the semaphore */
	if ((ret = sem_init(&sem_posix_3_302, 0, 0))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Sem init failed");
	}

	/* We initialize the recursive mutex */
	// if ((ret = pthread_mutexattr_init(&ma))) {
	// 	UNRESOLVED(ret, "Mutex attribute init failed");
	// }

	// if ((ret = pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_RECURSIVE))) {
	// 	UNRESOLVED(ret, "Set type RECURSIVE failed");
	// }

	if ((ret = pthread_mutex_init(&mtx, &ma))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Recursive mutex init failed");
	}

	if ((ret = pthread_mutexattr_destroy(&ma))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Mutex attribute destroy failed");
	}

	/* -- The mutex is now ready for testing -- */

	/* First, we lock it twice and unlock once */
	if ((ret = pthread_mutex_lock(&mtx))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "First lock failed");
	}

	if ((ret = pthread_mutex_lock(&mtx))) {
		ASSERT_FAIL();
		// FAILED("Second lock failed");
	}

	if ((ret = pthread_mutex_unlock(&mtx))) {
		ASSERT_FAIL();
		// FAILED("First unlock failed");
	}
	/* Here this thread owns the mutex and the internal count is "1" */

	/* We create the child thread */
	if ((ret = pthread_create(&child, NULL, threaded_posix_3_302, NULL))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Unable to create child thread");
	}

	/* then wait for child to be ready */
	if ((ret = sem_wait(&sem_posix_3_302))) {
		ASSERT_FAIL();
		// UNRESOLVED(errno, "Wait sem in child failed");
	}

	/* We can now unlock the mutex */
	if ((ret = pthread_mutex_unlock(&mtx))) {
		ASSERT_FAIL();
		// FAILED("Second unlock failed");
	}

	/* We wait for the child to lock the mutex */
	if ((ret = sem_wait(&sem_posix_3_302))) {
		ASSERT_FAIL();
		// UNRESOLVED(errno, "Wait sem in child failed");
	}

	/* Then, try to unlock the mutex (owned by the child or unlocked) */
	ret = pthread_mutex_unlock(&mtx);
	if (ret == 0) {
		ASSERT_FAIL();
		// FAILED("Unlock of unowned mutex succeeds");
	}

	/* Everything seems OK here */
	if ((ret = pthread_join(child, NULL))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Child join failed");
	}

	/* Simple loop to double-check */

	for (i = 0; i < 50; i++) {
		if ((ret = pthread_mutex_lock(&mtx))) {
			ASSERT_FAIL();
			// FAILED("Lock failed in loop");
		}
	}
	for (i = 0; i < 50; i++) {
		if ((ret = pthread_mutex_unlock(&mtx))) {
			ASSERT_FAIL();
			// FAILED("Unlock failed in loop");
		}
	}

	ret = pthread_mutex_unlock(&mtx);
	if (ret == 0) {
		ASSERT_FAIL();
		// FAILED("Unlock succeeds after the loop");
	}

	/* The test passed, we destroy the mutex */
	if ((ret = pthread_mutex_destroy(&mtx))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Final mutex destroy failed");
	}
}

/* Steps:
 *   -- Initilize a mutex object
 *   -- Create a secondary thread and have it lock the mutex
 *   -- Within the main thread, try to lock the mutex using
 	pthread_mutex_trylock() and EBUSY is expected
 *   -- Have the secondary thread unlock the mutex
 *   -- Within the main thread, try to lock the mutex again
 	and expect a successful locking.
 *
 */

pthread_mutex_t mutex_3_401 = PTHREAD_MUTEX_INITIALIZER;
int t1_start = 0;
int t1_pause = 1;

void *func_posix_3_401(void *parm)
{
	int rc;
	printf("f\n");
	if ((rc = pthread_mutex_lock(&mutex_3_401)) != 0) {
		printf("u\n");
		// fprintf(stderr, "Error at pthread_mutex_lock(), rc=%d\n", rc);
		pthread_exit((void *)1);
	}
	printf("t1_start = 1\n");
	t1_start = 1;

	while (t1_pause)
		krhino_task_sleep(1);
	printf("child pthread_mutex_unlock\n");
	if ((rc = pthread_mutex_unlock(&mutex_3_401)) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Error at pthread_mutex_unlock(), rc=%d\n", rc);
		// pthread_exit((void *)PTS_UNRESOLVED);
	}

	pthread_exit(0);
	return (void *)(0);
}

CASE(test_mux, posix_3_401)
{
	int i, rc;
	pthread_t t1;

	/* Create a secondary thread and wait until it has locked the mutex */
	pthread_create(&t1, NULL, func_posix_3_401, NULL);
	printf("while (!t1_start)\n");
	while (!t1_start)
		krhino_task_sleep(1);
	printf("father pthread_mutex_trylock\n");
	/* Trylock the mutex and expect it returns EBUSY */
	rc = pthread_mutex_trylock(&mutex_3_401);
	if (rc != 1) {//EBUSY
		ASSERT_FAIL();
		// fprintf(stderr, "Expected %d(EBUSY), got %d\n", EBUSY, rc);
		// printf("Test FAILED\n");
		// return PTS_FAIL;
	}

	/* Allow the secondary thread to go ahead */
	t1_pause = 0;

	/* Trylock the mutex for N times */
	for (i = 0; i < 5; i++) {
		rc = pthread_mutex_trylock(&mutex_3_401);
		if (rc == 0) {
			pthread_mutex_unlock(&mutex_3_401);
			break;
		} else if (rc == -1) {//EBUSY
			krhino_task_sleep(1);
			continue;
		} else {
			ASSERT_FAIL();
			// fprintf(stderr, "Unexpected error code(%d) for pthread_mutex_lock()\n", rc);
			// return PTS_UNRESOLVED;
		}
	}

	/* Clean up */
	pthread_join(t1, NULL);
	pthread_mutex_destroy(&mutex_3_401);

	if (i >= 5) {
		ASSERT_FAIL();
		// fprintf(stderr, "Have tried %d times but failed to get the mutex\n", i);
		// return PTS_UNRESOLVED;
	}
}

/* Steps:
 *   -- Initilize a mutex object
 *   -- Get the mutex using pthread_mutex_lock()
 *   -- Release the mutex using pthread_mutex_unlock()
 *   -- Try to get the mutex using pthread_mutex_trylock()
 *   -- Release the mutex using pthread_mutex_unlock()
 *
 */

CASE(test_mux, posix_3_501)
{
	int rc;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	/* Get the mutex using pthread_mutex_lock() */
	if ((rc = pthread_mutex_lock(&mutex)) != 0) {
		printf("lock PTHREAD_MUTEX_INITIALIZER fail\n");
		ASSERT_FAIL();
		// fprintf(stderr, "Error at pthread_mutex_lock(), rc=%d\n", rc);
		// return PTS_UNRESOLVED;
	}

	/* Release the mutex using pthread_mutex_unlock() */
	if ((rc = pthread_mutex_unlock(&mutex)) != 0) {
		printf("unlock PTHREAD_MUTEX_INITIALIZER fail\n");
		ASSERT_FAIL();
		// printf("Test FAILED\n");
		// return PTS_FAIL;
	}

	/* Get the mutex using pthread_mutex_trylock() */
	if ((rc = pthread_mutex_trylock(&mutex)) != 0) {
		printf("trylock PTHREAD_MUTEX_INITIALIZER fail\n");
		ASSERT_FAIL();
		// printf("Test FAILED\n");
		// return PTS_FAIL;
	}

	/* Release the mutex using pthread_mutex_unlock() */
	if ((rc = pthread_mutex_unlock(&mutex)) != 0) {
		ASSERT_FAIL();
		// printf("Test FAILED\n");
		// return PTS_FAIL;
	}

}

/* Steps:
 *   -- Initialize a mutex to protect a global variable 'value'
 *   -- Create N threads. Each is looped M times to acquire the mutex,
        increase the value, and then release the mutex.
 *   -- Check if the value has increased properly (M*N); a broken mutex
        implementation may cause lost augments.
 *
 */


#define    THREAD_NUM  	4
#define    LOOPS     	3


pthread_mutex_t mutex_3_502 = PTHREAD_MUTEX_INITIALIZER;
int value;			/* value protected by mutex */


void *func_posix_3_502(void *parm)
{
	int i, tmp;
	int rc = 0;
	pthread_t self = pthread_self();

	/* Loopd M times to acquire the mutex, increase the value,
	   and then release the mutex. */

	for (i = 0; i < LOOPS; ++i) {
		rc = pthread_mutex_lock(&mutex_3_502);
		if (rc != 0) {
			ASSERT_FAIL();
			// fprintf(stderr,"Error on pthread_mutex_lock(), rc=%d\n", rc);
			// return (void *)(PTS_UNRESOLVED);
		}
		printf("pthread_mutex_lock\n");
		tmp = value;
		tmp = tmp + 1;
		// fprintf(stderr, "Thread(0x%p) holds the mutex\n", (void *)self);
		krhino_task_sleep(10);	/* delay the increasement operation */
		value = tmp;

		rc = pthread_mutex_unlock(&mutex_3_502);
		if (rc != 0) {
			ASSERT_FAIL();
			// fprintf(stderr,"Error on pthread_mutex_unlock(), rc=%d\n", rc);
			// return (void *)(PTS_UNRESOLVED);
		}
		printf("pthread_mutex_unlock\n");
		krhino_task_sleep(1);
	}
	pthread_exit(0);
	return (void *)(0);
}

CASE(test_mux, posix_3_502)
{
	int i, rc;
	pthread_t threads[THREAD_NUM];

	/* Create threads */
	// fprintf(stderr, "Creating %d threads\n", THREAD_NUM);
	printf("pthread_create\n");
	for (i = 0; i < THREAD_NUM; ++i)
		rc = pthread_create(&threads[i], NULL, func_posix_3_502, NULL);
	printf("pthread_join\n");
	/* Wait to join all threads */
	for (i = 0; i < THREAD_NUM; ++i)
		pthread_join(threads[i], NULL);
	pthread_mutex_destroy(&mutex_3_502);
	printf("end\n");

	/* Check if the final value is as expected */
	if (value != (THREAD_NUM) * LOOPS) {
		printf("value %d != (THREAD_NUM) * LOOPS) %d\n", value, (THREAD_NUM) * LOOPS);
		ASSERT_FAIL();
		// fprintf(stderr, "Using %d threads and each loops %d times\n",THREAD_NUM, LOOPS);
		// fprintf(stderr, "Final value must be %d instead of %d\n",(THREAD_NUM) * LOOPS, value);
		// return PTS_UNRESOLVED;
	}
}




/* The steps are:
 * ->Create a mutex with recursive attribute
 * ->Create a threads
 * ->Parent locks the mutex twice, unlocks once.
 * ->Child attempts to lock the mutex.
 * ->Parent unlocks the mutex.
 * ->Parent unlocks the mutex (shall fail)
 * ->Child unlocks the mutex.
 */

pthread_mutex_t mtx_A, mtx_B;
sem_t sem_A, sem_B;

/** child thread function **/
void *threaded_posix_3_503_A(void *arg)
{
	int ret;

	if (ret = sem_wait(sem_A)) {
		ASSERT_FAIL();
		// UNRESOLVED(errno, "1st post sem in child failed");
	}

	/* Try to lock the mutex once. The call must fail here. */
	ret = pthread_mutex_lock(&mtx_A);
	if (ret != 0) {
		ASSERT_FAIL();
		// FAILED("Child first trylock succeeded");
	}

	if (ret = sem_wait(sem_A)) {
		ASSERT_FAIL();
		// UNRESOLVED(errno, "1st post sem in child failed");
	}

	ret = pthread_mutex_lock(&mtx_B);
	if (ret == 0) {
		ASSERT_FAIL();
		// FAILED("Child first trylock succeeded");
	}

	return NULL;
}

/** child thread function **/
void *threaded_posix_3_503_B(void *arg)
{
	int ret;

	if (ret = sem_wait(sem_B)) {
		ASSERT_FAIL();
		// UNRESOLVED(errno, "1st post sem in child failed");
	}

	/* Try to lock the mutex once. The call must fail here. */
	ret = pthread_mutex_lock(&mtx_B);
	if (ret != 0) {
		ASSERT_FAIL();
		// FAILED("Child first trylock succeeded");
	}

	if (ret = sem_wait(sem_B)) {
		ASSERT_FAIL();
		// UNRESOLVED(errno, "1st post sem in child failed");
	}

	ret = pthread_mutex_lock(&mtx_A);
	if (ret == 0) {
		ASSERT_FAIL();
		// FAILED("Child first trylock succeeded");
	}

	return NULL;
}

CASE(test_mux, posix_3_503)
{
	int ret;
	int i;
	pthread_mutexattr_t ma;
	pthread_t child_A, child_B;

	// output_init();

	/* Initialize the semaphore */
	if ((ret = sem_init(&sem_A, 0, 0))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Sem init failed");
	}

	if ((ret = sem_init(&sem_B, 0, 0))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Sem init failed");
	}

	if ((ret = pthread_mutex_init(&mtx_A, &ma))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Recursive mutex init failed");
	}

	if ((ret = pthread_mutex_init(&mtx_B, &ma))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Recursive mutex init failed");
	}

	if ((ret = pthread_mutexattr_destroy(&ma))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Mutex attribute destroy failed");
	}

	/* Here this thread owns the mutex and the internal count is "1" */

	/* We create the child thread */

	pthread_attr_t attr;

	memset(&attr, 0, sizeof(attr));

	attr.stacksize 				 	= 2048;
    attr.schedparam.sched_priority 	= 20;
    attr.detachstate               	= PTHREAD_CREATE_JOINABLE;

	if ((ret = pthread_create(&child_A, &attr, threaded_posix_3_503_A, NULL))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Unable to create child thread");
	}

	attr.stacksize 				 	= 2048;
    attr.schedparam.sched_priority 	= 30;
    attr.detachstate               	= PTHREAD_CREATE_JOINABLE;

	if ((ret = pthread_create(&child_B, &attr, threaded_posix_3_503_B, NULL))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Unable to create child thread");
	}

	/* -- The mutex is now ready for testing -- */

	/* First, we make A&B run */

	if ((ret = sem_post(&sem_A))) {
		ASSERT_FAIL();
		// UNRESOLVED(errno, "sem in child failed");
	}

	if ((ret = sem_post(&sem_B))) {
		ASSERT_FAIL();
		// UNRESOLVED(errno, "sem in child failed");
	}

	printf("child_A's b_prio is %d, now is %d.\n ", child_A->b_prio, child_A->prio);
	printf("child_B's b_prio is %d, now is %d.\n ", child_B->b_prio, child_B->prio);


	/* The test passed, we destroy the mutex */
	if ((ret = pthread_mutex_destroy(&mtx_A))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Final mutex destroy failed");
	}
	if ((ret = pthread_mutex_destroy(&mtx_B))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Final mutex destroy failed");
	}
	if ((ret = sem_destroy(&sem_A))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Final mutex destroy failed");
	}
	if ((ret = sem_destroy(&sem_B))) {
		ASSERT_FAIL();
		// UNRESOLVED(ret, "Final mutex destroy failed");
	}
}


#endif /* TEST_CONFIG_MUX_ENABLED */

/* posix mux test suite */
SUITE(test_mux) = {
#if (TEST_CONFIG_MUX_ENABLED > 0)
    ADD_CASE(test_mux, posix_3_101),
    ADD_CASE(test_mux, posix_3_102),
    ADD_CASE(test_mux, posix_3_201),
    ADD_CASE(test_mux, posix_3_202),
    ADD_CASE(test_mux, posix_3_203),
    ADD_CASE(test_mux, posix_3_301),
    ADD_CASE(test_mux, posix_3_302),
    //ADD_CASE(test_mux, posix_3_401),//PTHREAD_MUTEX_INITIALIZER fail
    ADD_CASE(test_mux, posix_3_501),
    ADD_CASE(test_mux, posix_3_502),
#endif
    ADD_CASE_NULL
};


#if (TEST_CONFIG_COND_ENABLED > 0)

/* Test that pthread_cond_init()
 *   shall initialize the condition variable referenced by cond with attributes
 *   referenced by attr. If attr is NULL, the default condition variable
 *   attributes shall be used; the effect is the same as passing the address
 *   of a default condition variable attributes object.
 */
CASE(test_cond, posix_4_101)
{
	pthread_condattr_t condattr;
	pthread_cond_t cond1;
	pthread_cond_t cond2;
	int rc;
	char *f;
	// int status = PTS_UNRESOLVED;

	// f = "pthread_condattr_init()";
	// rc = pthread_condattr_init(&condattr);
	// if (rc)
	// 	goto done;

	// status = PTS_FAIL;
	f = "pthread_cond_init() - condattr";
	rc = pthread_cond_init(&cond1, &condattr);
	if (rc)
		ASSERT_FAIL();

	f = "pthread_cond_init() - NULL";
	rc = pthread_cond_init(&cond2, NULL);
	if (rc)
		ASSERT_FAIL();
}

/* Test that pthread_cond_destroy()
 *   shall destroy the condition variable referenced by 'cond';
 *   the condition variable object in effect becomes uninitialized.
 *
 */
CASE(test_cond, posix_4_102)
{
	pthread_cond_t cond1, cond2;
	pthread_cond_t cond3; //= PTHREAD_COND_INITIALIZER;

	pthread_condattr_t condattr;
	int rc;

	/* Initialize a condition variable attribute object */
	if ((rc = pthread_condattr_init(&condattr)) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Error at pthread_condattr_init(), rc=%d\n",rc);
		// return PTS_UNRESOLVED;
	}

	/* Initialize cond1 with the default condition variable attribute */
	if ((rc = pthread_cond_init(&cond1, &condattr)) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to initialize cond1, rc=%d\n", rc);
		// return PTS_UNRESOLVED;
	}

	/* Initialize cond2 with NULL attributes */
	if ((rc = pthread_cond_init(&cond2, NULL)) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to initialize cond2, rc=%d\n", rc);
		// return PTS_UNRESOLVED;
	}

	/* Destroy the condition variable attribute object */
	if ((rc = pthread_condattr_destroy(&condattr)) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Error at pthread_condattr_destroy(), rc=%d\n",rc);
		// return PTS_UNRESOLVED;
	}

	/* Destroy cond1 */
	if ((rc = pthread_cond_destroy(&cond1)) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to destroy cond1, rc=%d\n", rc);
		// printf("Test FAILED\n");
		// return PTS_FAIL;
	}

	/* Destroy cond2 */
	if ((rc = pthread_cond_destroy(&cond2)) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to destroy cond2, rc=%d\n", rc);
		// printf("Test FAILED\n");
		// return PTS_FAIL;
	}

	/* Destroy cond3 */
	if ((rc = pthread_cond_destroy(&cond3)) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to destroy cond3, rc=%d\n", rc);
		// printf("Test FAILED\n");
		// return PTS_FAIL;
	}
}

 /* Test that pthread_cond_broadcast()
 *   shall unblock all threads currently blocked on the specified condition
 *   variable cond.
 */

#define THREAD_NUM  3

struct testdata {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} td;

static int start_num = 0;
static int waken_num = 0;

static void *thr_func_posix_4_201(void *arg)
{
	int rc;
	pthread_t self = pthread_self();

	if (pthread_mutex_lock(&td.mutex) != 0) {
		printf("pthread_mutex_lock td.mutex in child failed\n");
		ASSERT_FAIL();
		// fprintf(stderr, "[Thread 0x%p] failed to acquire the mutex\n", (void *)self);
		// exit(PTS_UNRESOLVED);
	}
	start_num++;
	printf("start_num++\n");
	// fprintf(stderr, "[Thread 0x%p] started and locked the mutex\n",(void *)self);

	// fprintf(stderr, "[Thread 0x%p] is waiting for the cond\n",(void *)self);
	rc = pthread_cond_wait(&td.cond, &td.mutex);
	if (rc != 0) {
		printf("pthread_cond_wait td.cond in child failed\n");
		ASSERT_FAIL();
		// fprintf(stderr, "pthread_cond_wait return %d\n", rc);
		// exit(PTS_UNRESOLVED);
	}

	if (pthread_mutex_trylock(&td.mutex) == 0) {
		printf("pthread_mutex_trylock shouldn't be successed\n");
		ASSERT_FAIL();
		// fprintf(stderr, "[Thread 0x%p] should not be able to lock the mutex again\n", (void *)self);
		// printf("Test FAILED\n");
		// exit(PTS_FAIL);
	}

	waken_num++;
	printf("waken_num++\n");
	// fprintf(stderr, "[Thread 0x%p] was wakened and acquired the mutex again\n", (void *)self);

	if (pthread_mutex_unlock(&td.mutex) != 0) {
		printf("pthread_mutex_unlock td.mutex in child failed\n");
		ASSERT_FAIL();
		// fprintf(stderr, "[Thread 0x%p] failed to release the mutex\n",(void *)self);
		// exit(PTS_UNRESOLVED);
	}
	// fprintf(stderr, "[Thread 0x%p] released the mutex\n", (void *)self);
	// return NULL;
}


CASE(test_cond, posix_4_201)
{
	int i, rc;
	pthread_t thread[THREAD_NUM];

	if (pthread_mutex_init(&td.mutex, NULL) != 0) {
		printf("pthread_mutex_init td.mutex failed\n");
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to initialize mutex\n");
		// return PTS_UNRESOLVED;
	}
	if (pthread_cond_init(&td.cond, NULL) != 0) {
		printf("pthread_cond_init td.cond failed\n");
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to initialize cond\n");
		// return PTS_UNRESOLVED;
	}

	for (i = 0; i < THREAD_NUM; i++) {
		if (pthread_create(&thread[i], NULL, thr_func_posix_4_201, NULL) != 0) {
			printf("pthread_create THREAD_NUM failed\n");
			ASSERT_FAIL();
			// fprintf(stderr, "Fail to create thread[%d]\n", i);
			// return PTS_UNRESOLVED;
		}
	}

	while (start_num < THREAD_NUM)
		krhino_task_sleep(100);

	/*
	 * Acquire the mutex to make sure that all waiters are currently
	 * blocked on pthread_cond_wait
	 */
	if (pthread_mutex_lock(&td.mutex) != 0) {
		printf("pthread_mutex_lock td.mutex in parent failed\n");
		ASSERT_FAIL();
		// fprintf(stderr, "Main: Fail to acquire mutex\n");
		// return PTS_UNRESOLVED;
	}
	if (pthread_mutex_unlock(&td.mutex) != 0) {
		printf("pthread_mutex_unlock td.mutex in parent failed\n");
		ASSERT_FAIL();
		// fprintf(stderr, "Main: Fail to release mutex\n");
		// return PTS_UNRESOLVED;
	}

	/* broadcast and check if all waiters are wakened */
	// fprintf(stderr, "[Main thread] broadcast the condition\n");
	rc = pthread_cond_broadcast(&td.cond);
	if (rc != 0) {
		printf("pthread_cond_broadcast td.cond in parent failed\n");
		ASSERT_FAIL();
		// fprintf(stderr, "[Main thread] failed to broadcast the condition\n");
		// return PTS_UNRESOLVED;
	}
	krhino_task_sleep(100);
	if (waken_num < THREAD_NUM) {
		// fprintf(stderr, "[Main thread] Not all waiters were wakened\n");
		printf("Test FAILED, waken num is %d\n", waken_num);
		for (i = 0; i < THREAD_NUM; i++)
			pthread_cancel(thread[i]);
		ASSERT_FAIL();
		// exit(PTS_FAIL);
	}
	// fprintf(stderr, "[Main thread] all waiters were wakened\n");

	/* join all secondary threads */
	for (i = 0; i < THREAD_NUM; i++) {
		if (pthread_join(thread[i], NULL) != 0) {
			printf("join all secondary threads failed\n");
			ASSERT_FAIL();
			// fprintf(stderr, "Fail to join thread[%d]\n", i);
			// return PTS_UNRESOLVED;
		}
	}
}

/* Test that pthread_cond_broadcast()
 *   When each thread unblocked as a result of pthread_cond_broadcast()
 *   returns from its call to pthread_cond_timedwait(), the thread shall
 *   own the mutex with which it called pthread_cond_timedwait().
 */

static void *thr_func_posix_4_202(void *arg)
{
	int rc;
	struct timespec timeout;
	struct timeval curtime;
	pthread_t self = pthread_self();

	if (pthread_mutex_lock(&td.mutex) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "[Thread 0x%p] failed to acquire the mutex\n", (void *)self);
		// exit(PTS_UNRESOLVED);
	}
	start_num++;
	// fprintf(stderr, "[Thread 0x%p] started and locked the mutex\n",(void *)self);

	if (gettimeofday(&curtime, NULL) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to get current time\n");
		// exit(PTS_UNRESOLVED);
	}
	timeout.tv_sec = curtime.tv_sec + TIMEOUT;
	timeout.tv_nsec = curtime.tv_usec * 1000;


	// fprintf(stderr, "[Thread 0x%p] is waiting for the cond\n",(void *)self);
	rc = pthread_cond_timedwait(&td.cond, &td.mutex, &timeout);
	if (rc != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "pthread_cond_wait return %d\n", rc);
		// exit(PTS_UNRESOLVED);
	}

	if (pthread_mutex_trylock(&td.mutex) == 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "[Thread 0x%p] should not be able to lock the mutex again\n", (void *)self);
		// printf("Test FAILED\n");
		// exit(PTS_FAIL);
	}

	waken_num++;
	// fprintf(stderr, "[Thread 0x%p] was wakened and acquired the mutex again\n", (void *)self);

	if (pthread_mutex_unlock(&td.mutex) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "[Thread 0x%p] failed to release the mutex\n",(void *)self);
		// exit(PTS_UNRESOLVED);
	}
	// fprintf(stderr, "[Thread 0x%p] released the mutex\n", (void *)self);
	// return NULL;
}
CASE(test_cond, posix_4_202)
{
	int i, rc;
	start_num = 0;
	waken_num = 0;
	pthread_t thread[THREAD_NUM];

	if (pthread_mutex_init(&td.mutex, NULL) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to initialize mutex\n");
		// return PTS_UNRESOLVED;
	}
	if (pthread_cond_init(&td.cond, NULL) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to initialize cond\n");
		// return PTS_UNRESOLVED;
	}

	for (i = 0; i < THREAD_NUM; i++) {
		if (pthread_create(&thread[i], NULL, thr_func_posix_4_202, NULL) != 0) {
			ASSERT_FAIL();
			// fprintf(stderr, "Fail to create thread[%d]\n", i);
			// return PTS_UNRESOLVED;
		}
	}

	while (start_num < THREAD_NUM)
		krhino_task_sleep(100);

	/*
	 * Acquire the mutex to make sure that all waiters are currently
	 * blocked on pthread_cond_wait
	 */
	if (pthread_mutex_lock(&td.mutex) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Main: Fail to acquire mutex\n");
		// return PTS_UNRESOLVED;
	}
	if (pthread_mutex_unlock(&td.mutex) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Main: Fail to release mutex\n");
		// return PTS_UNRESOLVED;
	}

	/* broadcast and check if all waiters are wakened */
	// fprintf(stderr, "[Main thread] broadcast the condition\n");
	rc = pthread_cond_broadcast(&td.cond);
	if (rc != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "[Main thread] failed to broadcast the condition\n");
		// return PTS_UNRESOLVED;
	}
	krhino_task_sleep(1);
	if (waken_num < THREAD_NUM) {
		// fprintf(stderr, "[Main thread] Not all waiters were wakened\n");
		printf("Test FAILED\n");
		for (i = 0; i < THREAD_NUM; i++)
			pthread_cancel(thread[i]);
		ASSERT_FAIL();
		// exit(PTS_FAIL);
	}
	// fprintf(stderr, "[Main thread] all waiters were wakened\n");

	/* join all secondary threads */
	for (i = 0; i < THREAD_NUM; i++) {
		if (pthread_join(thread[i], NULL) != 0) {
			ASSERT_FAIL();
			// fprintf(stderr, "Fail to join thread[%d]\n", i);
			// return PTS_UNRESOLVED;
		}
	}
}


/* Test that pthread_cond_signal()
 *   shall unblock at least one of the threads currently blocked on
 *   the specified condition variable cond.
 */

static void *thr_func_posix_4_203(void *arg)
{
	int rc;
	pthread_t self = pthread_self();

	if (pthread_mutex_lock(&td.mutex) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "[Thread 0x%p] failed to acquire the mutex\n", (void *)self);
		// exit(PTS_UNRESOLVED);
	}
	start_num++;
	// fprintf(stderr, "[Thread 0x%p] started and locked the mutex\n",(void *)self);

	// fprintf(stderr, "[Thread 0x%p] is waiting for the cond\n",(void *)self);
	rc = pthread_cond_wait(&td.cond, &td.mutex);
	if (rc != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "pthread_cond_wait return %d\n", rc);
		// exit(PTS_UNRESOLVED);
	}

	if (pthread_mutex_trylock(&td.mutex) == 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "[Thread 0x%p] should not be able to lock the mutex again\n", (void *)self);
		// printf("Test FAILED\n");
		// exit(PTS_FAIL);
	}

	waken_num++;
	// fprintf(stderr, "[Thread 0x%p] was wakened and acquired the mutex again\n", (void *)self);

	if (pthread_mutex_unlock(&td.mutex) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "[Thread 0x%p] failed to release the mutex\n",(void *)self);
		// exit(PTS_UNRESOLVED);
	}
	// fprintf(stderr, "[Thread 0x%p] released the mutex\n", (void *)self);
	// return NULL;
}

CASE(test_cond, posix_4_203)
{
	int i, rc;
	start_num = 0;
	waken_num = 0;
	pthread_t thread[THREAD_NUM];

	if (pthread_mutex_init(&td.mutex, NULL) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to initialize mutex\n");
		// return PTS_UNRESOLVED;
	}
	if (pthread_cond_init(&td.cond, NULL) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to initialize cond\n");
		// return PTS_UNRESOLVED;
	}

	for (i = 0; i < THREAD_NUM; i++) {
		if (pthread_create(&thread[i], NULL, thr_func_posix_4_203, NULL) != 0) {
			ASSERT_FAIL();
			// fprintf(stderr, "Fail to create thread[%d]\n", i);
			// return PTS_UNRESOLVED;
		}
	}

	while (start_num < THREAD_NUM)
		krhino_task_sleep(100);

	/*
	 * Acquire the mutex to make sure that all waiters are currently
	 * blocked on pthread_cond_wait
	 */
	if (pthread_mutex_lock(&td.mutex) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Main: Fail to acquire mutex\n");
		// return PTS_UNRESOLVED;
	}
	if (pthread_mutex_unlock(&td.mutex) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Main: Fail to release mutex\n");
		// return PTS_UNRESOLVED;
	}

	/* broadcast and check if all waiters are wakened */
	// fprintf(stderr, "[Main thread] broadcast the condition\n");
	rc = pthread_cond_signal(&td.cond);
	if (rc != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "[Main thread] failed to broadcast the condition\n");
		// return PTS_UNRESOLVED;
	}
	krhino_task_sleep(1);
	if (waken_num <= 0) {
		// fprintf(stderr, "[Main thread] Not all waiters were wakened\n");
		printf("Test FAILED\n");
		for (i = 0; i < THREAD_NUM; i++)
			pthread_cancel(thread[i]);
		ASSERT_FAIL();
		// exit(PTS_FAIL);
	}
	// fprintf(stderr, "[Main thread] all waiters were wakened\n");

	/* join all secondary threads */
	for (i = 0; i < THREAD_NUM; i++) {
		if (pthread_join(thread[i], NULL) != 0) {
			ASSERT_FAIL();
			// fprintf(stderr, "Fail to join thread[%d]\n", i);
			// return PTS_UNRESOLVED;
		}
	}
}

/* Test that pthread_cond_broadcast()
 *   When each thread unblocked as a result of pthread_cond_signal()
 *   returns from its call to pthread_cond_timedwait(), the thread shall
 *   own the mutex with which it called pthread_cond_timedwait().
 */

static void *thr_func_posix_4_204(void *arg)
{
	int rc;
	struct timespec timeout;
	struct timeval curtime;
	pthread_t self = pthread_self();

	if (pthread_mutex_lock(&td.mutex) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "[Thread 0x%p] failed to acquire the mutex\n", (void *)self);
		// exit(PTS_UNRESOLVED);
	}
	start_num++;
	// fprintf(stderr, "[Thread 0x%p] started and locked the mutex\n",(void *)self);

	if (gettimeofday(&curtime, NULL) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to get current time\n");
		// exit(PTS_UNRESOLVED);
	}
	timeout.tv_sec = curtime.tv_sec + TIMEOUT;
	timeout.tv_nsec = curtime.tv_usec * 1000;


	// fprintf(stderr, "[Thread 0x%p] is waiting for the cond\n",(void *)self);
	rc = pthread_cond_timedwait(&td.cond, &td.mutex, &timeout);
	if (rc != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "pthread_cond_wait return %d\n", rc);
		// exit(PTS_UNRESOLVED);
	}

	if (pthread_mutex_trylock(&td.mutex) == 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "[Thread 0x%p] should not be able to lock the mutex again\n", (void *)self);
		// printf("Test FAILED\n");
		// exit(PTS_FAIL);
	}

	waken_num++;
	// fprintf(stderr, "[Thread 0x%p] was wakened and acquired the mutex again\n", (void *)self);

	if (pthread_mutex_unlock(&td.mutex) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "[Thread 0x%p] failed to release the mutex\n",(void *)self);
		// exit(PTS_UNRESOLVED);
	}
	// fprintf(stderr, "[Thread 0x%p] released the mutex\n", (void *)self);
	// return NULL;
}
CASE(test_cond, posix_4_204)
{
	int i, rc;
	start_num = 0;
	waken_num = 0;
	pthread_t thread[THREAD_NUM];

	if (pthread_mutex_init(&td.mutex, NULL) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to initialize mutex\n");
		// return PTS_UNRESOLVED;
	}
	if (pthread_cond_init(&td.cond, NULL) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to initialize cond\n");
		// return PTS_UNRESOLVED;
	}

	for (i = 0; i < THREAD_NUM; i++) {
		if (pthread_create(&thread[i], NULL, thr_func_posix_4_204, NULL) != 0) {
			ASSERT_FAIL();
			// fprintf(stderr, "Fail to create thread[%d]\n", i);
			// return PTS_UNRESOLVED;
		}
	}

	while (start_num < THREAD_NUM)
		krhino_task_sleep(100);

	/*
	 * Acquire the mutex to make sure that all waiters are currently
	 * blocked on pthread_cond_wait
	 */
	if (pthread_mutex_lock(&td.mutex) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Main: Fail to acquire mutex\n");
		// return PTS_UNRESOLVED;
	}
	if (pthread_mutex_unlock(&td.mutex) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Main: Fail to release mutex\n");
		// return PTS_UNRESOLVED;
	}

	/* broadcast and check if all waiters are wakened */
	// fprintf(stderr, "[Main thread] broadcast the condition\n");
	rc = pthread_cond_signal(&td.cond);
	if (rc != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "[Main thread] failed to broadcast the condition\n");
		// return PTS_UNRESOLVED;
	}
	krhino_task_sleep(1);
	if (waken_num <= 0) {
		// fprintf(stderr, "[Main thread] Not all waiters were wakened\n");
		printf("Test FAILED\n");
		for (i = 0; i < THREAD_NUM; i++)
			pthread_cancel(thread[i]);
		ASSERT_FAIL();
		// exit(PTS_FAIL);
	}
	// fprintf(stderr, "[Main thread] all waiters were wakened\n");

	/* join all secondary threads */
	for (i = 0; i < THREAD_NUM; i++) {
		if (pthread_join(thread[i], NULL) != 0) {
			ASSERT_FAIL();
			// fprintf(stderr, "Fail to join thread[%d]\n", i);
			// return PTS_UNRESOLVED;
		}
	}
}


/* Test that pthread_cond_timedwait()
 *   shall block on a condition variable. It shall be called with mutex locked
 *   by the calling thread or undefined behavior results.
 */

static void *thr_func_posix_4_205(void *arg)
{
	int rc;
	struct timespec timeout;
	struct timeval curtime;

	pthread_t self = pthread_self();

	if (pthread_mutex_lock(&td.mutex) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "[Thread 0x%p] failed to acquire the mutex\n", (void *)self);
		// exit(PTS_UNRESOLVED);
	}
	start_num++;
	// fprintf(stderr, "[Thread 0x%p] started and locked the mutex\n",(void *)self);

	if (gettimeofday(&curtime, NULL) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to get current time\n");
		// exit(PTS_UNRESOLVED);
	}
	timeout.tv_sec = curtime.tv_sec + TIMEOUT;
	timeout.tv_nsec = curtime.tv_usec * 1000;
	timeout.tv_sec += 5;


	// fprintf(stderr, "[Thread 0x%p] is waiting for the cond\n",(void *)self);
	rc = pthread_cond_timedwait(&td.cond, &td.mutex, &timeout);
	if (rc != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "pthread_cond_wait return %d\n", rc);
		// exit(PTS_UNRESOLVED);
	}

	if (pthread_mutex_trylock(&td.mutex) == 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "[Thread 0x%p] should not be able to lock the mutex again\n", (void *)self);
		// printf("Test FAILED\n");
		// exit(PTS_FAIL);
	}

	waken_num++;
	// fprintf(stderr, "[Thread 0x%p] was wakened and acquired the mutex again\n", (void *)self);

	if (pthread_mutex_unlock(&td.mutex) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "[Thread 0x%p] failed to release the mutex\n",(void *)self);
		// exit(PTS_UNRESOLVED);
	}
	// fprintf(stderr, "[Thread 0x%p] released the mutex\n", (void *)self);
	// return NULL;
}
CASE(test_cond, posix_4_205)
{
	int i, rc;
	start_num = 0;
	waken_num = 0;
	pthread_t thread[THREAD_NUM];

	if (pthread_mutex_init(&td.mutex, NULL) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to initialize mutex\n");
		// return PTS_UNRESOLVED;
	}
	if (pthread_cond_init(&td.cond, NULL) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Fail to initialize cond\n");
		// return PTS_UNRESOLVED;
	}

	for (i = 0; i < THREAD_NUM; i++) {
		if (pthread_create(&thread[i], NULL, thr_func_posix_4_205, NULL) != 0) {
			ASSERT_FAIL();
			// fprintf(stderr, "Fail to create thread[%d]\n", i);
			// return PTS_UNRESOLVED;
		}
	}

	while (start_num < THREAD_NUM)
		krhino_task_sleep(100);

	/*
	 * Acquire the mutex to make sure that all waiters are currently
	 * blocked on pthread_cond_wait
	 */
	if (pthread_mutex_lock(&td.mutex) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Main: Fail to acquire mutex\n");
		// return PTS_UNRESOLVED;
	}
	if (pthread_mutex_unlock(&td.mutex) != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "Main: Fail to release mutex\n");
		// return PTS_UNRESOLVED;
	}

	/* broadcast and check if all waiters are wakened */
	// fprintf(stderr, "[Main thread] broadcast the condition\n");
	rc = pthread_cond_signal(&td.cond);
	if (rc != 0) {
		ASSERT_FAIL();
		// fprintf(stderr, "[Main thread] failed to broadcast the condition\n");
		// return PTS_UNRESOLVED;
	}
	krhino_task_sleep(1);
	if (waken_num <= 0) {
		// fprintf(stderr, "[Main thread] Not all waiters were wakened\n");
		printf("Test FAILED\n");
		for (i = 0; i < THREAD_NUM; i++)
			pthread_cancel(thread[i]);
		ASSERT_FAIL();
		// exit(PTS_FAIL);
	}
	// fprintf(stderr, "[Main thread] all waiters were wakened\n");

	/* join all secondary threads */
	for (i = 0; i < THREAD_NUM; i++) {
		if (pthread_join(thread[i], NULL) != 0) {
			ASSERT_FAIL();
			// fprintf(stderr, "Fail to join thread[%d]\n", i);
			// return PTS_UNRESOLVED;
		}
	}
}



#endif /* TEST_CONFIG_COND_ENABLED */

/* posix cond test suite */
SUITE(test_cond) = {
#if (TEST_CONFIG_COND_ENABLED > 0)
    ADD_CASE(test_cond, posix_4_101),
    ADD_CASE(test_cond, posix_4_102),
    ADD_CASE(test_cond, posix_4_201),
    ADD_CASE(test_cond, posix_4_202),
    ADD_CASE(test_cond, posix_4_203),
    ADD_CASE(test_cond, posix_4_204),
    ADD_CASE(test_cond, posix_4_205),
#endif
    ADD_CASE_NULL
};



void test_certificate_posix(void)
{
    if (0 == dump_test_config()) {
        printf("posix test start!\r\n");
        ADD_SUITE(test_pthread);
        ADD_SUITE(test_sem);
        ADD_SUITE(test_mux);
		ADD_SUITE(test_cond);
        cut_main(0, NULL);
        printf("posix test finished!\r\n");
    }
    else {
        printf("posix test error!\r\n");
    }
}
AOS_TESTCASE(test_certificate_posix);

#endif
