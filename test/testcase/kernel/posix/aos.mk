NAME := posix_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := test code of posix
$(NAME)_COMPONENTS += kernel.posix

$(NAME)_SOURCES := posix_cond_test.c   \
                   posix_mutex_test.c  \
                   posix_sem_test.c    \
                   posix_task_test.c   \
                   posix_test.c        \
                   posix_tsd_test.c    \
                   posix_dirent_test.c

$(NAME)_CFLAGS += -Wall -Werror

