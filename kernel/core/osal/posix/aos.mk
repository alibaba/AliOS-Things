NAME := osal_posix_obsolete

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := kernel posix for alios things (obsolete)
# If component's header files under another directory, add RPM_INCLUDE_DIR to
# indicate where the header file folder is located.
RPM_INCLUDE_DIR    := posix

$(NAME)_SOURCES := pthread.c
$(NAME)_SOURCES += pthread_attr.c
$(NAME)_SOURCES += pthread_cond.c
$(NAME)_SOURCES += pthread_mutex.c
$(NAME)_SOURCES += pthread_sched.c
$(NAME)_SOURCES += pthread_tsd.c
$(NAME)_SOURCES += timer.c
$(NAME)_SOURCES += semaphore.c
$(NAME)_SOURCES += mqueue.c
$(NAME)_SOURCES += dirent.c
$(NAME)_SOURCES += prctl.c
$(NAME)_SOURCES += enviro.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall
else ifeq ($(COMPILER),armcc)
GLOBAL_DEFINES += __BSD_VISIBLE
endif

GLOBAL_DEFINES  += AOS_COMP_OSAL_POSIX
