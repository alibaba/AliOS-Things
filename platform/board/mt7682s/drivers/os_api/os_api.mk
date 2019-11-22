NAME := os_api

$(NAME)_INCLUDES += inc	\
					test	
GLOBAL_INCLUDES +=	freertos_aos
GLOBAL_INCLUDES +=	inc
$(NAME)_SOURCES += \
                    src/os_task.c      \
                    src/os_mutex.c     \
                    src/os_queue.c     \
                    src/os_sem.c \
                    src/os_timer.c      \
					src/os_event.c	\
					src/os_mem.c
#test source files
$(NAME)_SOURCES += \
					test/os_timer_test.c	\
					test/os_task_test.c	\
					test/os_mutex_test.c	\
					test/os_sem_test.c	\
					test/os_queue_test.c	\
					test/os_event_test.c	\
					test/os_test.c
#freertos api
$(NAME)_SOURCES += freertos_aos/freertos_to_aos.c

