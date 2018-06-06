NAME := cmsis

GLOBAL_INCLUDES += .

$(NAME)_SOURCES := \
                    cmsis_os.c      \
                    test/main.c     \
                    test/thread_test.c     \
                    test/msgqueue_test.c     \
                    test/mutex_test.c     \
                    test/sem_test.c     \
                    test/timer_test.c