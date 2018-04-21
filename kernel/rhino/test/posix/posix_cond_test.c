#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static pthread_t thread;
static pthread_cond_t cond;
static pthread_mutex_t mutex;
static int flag = 1;

extern int gettimeofday(struct timeval *tv, void *tzp);

static void *demo_task1(void *arg)
{
    struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&mutex);
    while (flag)
        {
        printf("demo_task1\n");
        gettimeofday(&now, NULL);
        outtime.tv_sec = now.tv_sec + 5;
        outtime.tv_nsec = now.tv_usec * 1000;
        pthread_cond_timedwait(&cond, &mutex, &outtime);
        }
    
    pthread_mutex_unlock(&mutex);
    printf("demo_task1 exit\n");

    return NULL;
}

static void *demo_task2(void *arg)
{
    krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND*10);
    printf("Now terminate the thread!\n");
    flag = 0;
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

static void *init_task(void *arg)
{
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    
    pthread_create(&thread, NULL, demo_task1, NULL);
    pthread_create(&thread, NULL, demo_task2, NULL);

    return NULL;
}


void posix_test_case4(void)
{    
    pthread_create(&thread, NULL, init_task, NULL);
}


