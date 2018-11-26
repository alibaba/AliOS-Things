NAME := realtime

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_COMPONENTS += rhino

GLOBAL_INCLUDES += include common

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

$(NAME)_SOURCES := common/auxiliary.c \
                   common/global_object.c \
                   sched/sched_yield.c \
                   sched/sched_preempt.c \
                   sched/sched_resume_active.c \
                   memory/memory_blk.c \
                   memory/memory_tlf.c \
                   sync/sync_sem.c \
                   sync/sync_mutex.c \
                   sync/sync_mutex_shuffling.c \
                   sync/sync_sem_shuffling.c \
                   sync/sync_sem_rrwp_task.c \
                   sync/sync_sem_active.c \
                   sync/sync_mutex_active.c \
                   interrupt/interrupt_respond.c \
                   interrupt/interrupt_wakeuptask.c \
                   interrupt/interrupt_preempt.c \
                   message/message_queue_rrwp_task.c \
                   message/message_queue.c \
                   test_realtime_main.c
