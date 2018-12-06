NAME := posix

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := posix for alios things

$(NAME)_SOURCES := pthread.c
$(NAME)_SOURCES += pthread_attr.c
$(NAME)_SOURCES += pthread_cond.c
$(NAME)_SOURCES += pthread_mutex.c
$(NAME)_SOURCES += pthread_sched.c
$(NAME)_SOURCES += pthread_tsd.c
$(NAME)_SOURCES += posix_timer.c
$(NAME)_SOURCES += posix_semaphore.c
$(NAME)_SOURCES += posix_mqueue.c
$(NAME)_SOURCES += posix_dirent.c
$(NAME)_SOURCES += posix_prctl.c
$(NAME)_SOURCES += posix_init.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),armcc)
GLOBAL_DEFINES += __BSD_VISIBLE
endif

GLOBAL_INCLUDES += include
GLOBAL_DEFINES  += AOS_POSIX
