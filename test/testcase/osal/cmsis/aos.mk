NAME := cmsis_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := test code of cmsis
$(NAME)_COMPONENTS += osal_cmsis

$(NAME)_SOURCES := cmsis_test_main.c     \
                   thread_test.c     \
                   msgqueue_test.c     \
                   mutex_test.c     \
                   sem_test.c     \
                   timer_test.c

$(NAME)_CFLAGS += -Wall -Werror

