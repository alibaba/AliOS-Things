NAME := uapp1

$(NAME)_TYPE := app

$(NAME)_MBINS_TYPE := app

$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

ifeq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES := src/entry.c \
                   src/app.c \
                   src/ucli.c

$(NAME)_SOURCES += test/test.c \
                   test/task_test.c \
                   test/dynamic_mutex_test.c \
                   test/static_mutex_test.c \
                   test/dynamic_semaphore_test.c \
                   test/static_semaphore_test.c \
                   test/time_test.c \
                   test/timer_test.c \
                   test/buf_queue_test.c

$(NAME)_SOURCES += test/aos_uspace_test.c \
                   test/cut.c

$(NAME)_INCLUDES := include \
                    test
else
$(NAME)_SOURCES := src/app.c
endif

