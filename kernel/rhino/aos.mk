NAME := rhino

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := rhino core code

GLOBAL_INCLUDES += include ./

$(NAME)_COMPONENTS += lib_rbtree

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

$(NAME)_SOURCES := k_err.c          \
                   k_mm.c           \
                   k_mm_debug.c     \
                   k_ringbuf.c      \
                   k_stats.c        \
                   k_task_sem.c     \
                   k_timer.c        \
                   k_buf_queue.c    \
                   k_event.c        \
                   k_mm_blk.c       \
                   k_mutex.c        \
                   k_pend.c         \
                   k_sched.c        \
                   k_sys.c          \
                   k_tick.c         \
                   k_workqueue.c    \
                   k_dyn_mem_proc.c \
                   k_idle.c         \
                   k_obj.c          \
                   k_queue.c        \
                   k_sem.c          \
                   k_task.c         \
                   k_time.c         \
                   k_spin_lock.c    \
                   k_cfs.c

GLOBAL_DEFINES += AOS_COMP_RHINO
