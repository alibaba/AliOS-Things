
GLOBAL_INCLUDES += espos/include
$(NAME)_SOURCES += \
                    espos/platform/rhino/espos_misc.c      \
                    espos/platform/rhino/espos_mutex.c     \
                    espos/platform/rhino/espos_queue.c     \
                    espos/platform/rhino/espos_scheduler.c \
                    espos/platform/rhino/espos_semaphore.c \
                    espos/platform/rhino/espos_task.c      \
                    espos/platform/rhino/espos_time.c      \
                    espos/platform/rhino/espos_timer.c
