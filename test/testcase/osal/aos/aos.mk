NAME := aos_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := test for aos api

$(NAME)_COMPONENTS += rhino osal_aos

$(NAME)_SOURCES += test_kernel.c
$(NAME)_SOURCES += aos_sys_test.c
$(NAME)_SOURCES += aos_task_test.c
$(NAME)_SOURCES += aos_mm_test.c
$(NAME)_SOURCES += aos_mutex_test.c
$(NAME)_SOURCES += aos_queue_test.c
$(NAME)_SOURCES += aos_sem_test.c
$(NAME)_SOURCES += aos_timer_test.c
$(NAME)_SOURCES += aos_workqueue_test.c

$(NAME)_CFLAGS += -Wall -Werror
