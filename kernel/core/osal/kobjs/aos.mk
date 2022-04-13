NAME := kobjs

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := kernel object

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall
endif

GLOBAL_INCLUDES += include

$(NAME)_SOURCES+= obj_install.c
$(NAME)_SOURCES-$(AOS_KOBJ_SEM) += aos_sem.c
$(NAME)_SOURCES-$(AOS_KOBJ_EVENT) += aos_event.c
$(NAME)_SOURCES-$(AOS_KOBJ_MUTEX) += aos_mutex.c
$(NAME)_SOURCES-$(AOS_KOBJ_TIMER) += aos_timer.c
$(NAME)_SOURCES-$(AOS_KOBJ_QUEUE) += aos_queue.c
