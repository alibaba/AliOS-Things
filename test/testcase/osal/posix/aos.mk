NAME := posix_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := test code of posix
$(NAME)_COMPONENTS += osal_posix

$(NAME)_SOURCES := posix_cond_test.c          \
                   posix_mutex_test.c         \
                   posix_sem_test.c           \
                   posix_pthread_test.c       \
                   posix_pthread_attr_test.c  \
                   posix_pthread_sched_test.c \
                   posix_test.c               \
                   posix_tsd_test.c           \
                   posix_timer_test.c         \
                   posix_jump_test.c          \
                   posix_dirent_test.c

$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_COMPONENTS += kernel.fs.ramfs
