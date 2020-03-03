NAME := aos_api_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := An example to show how to use the aos kernel API
$(NAME)_SOURCES := maintask.c \
                   aos_api_main.c \
                   task_app/task_new.c \
                   task_app/task_new_ext.c \
                   task_app/task_key.c \
                   timer_app/timer_new.c \
                   timer_app/timer_new_ext.c \
                   timer_app/timer_change.c \
                   work_app/work_new.c \
                   work_app/work_cancel.c \
                   work_app/workqueue_new.c \
                   mem_app/mem_alloc.c \
                   mem_app/mem_realloc.c \
                   sem_app.c \
                   mutex_app.c \
                   queue_app.c \
                   time_app.c

GLOBAL_DEFINES += AOS_NO_WIFI
GLOBAL_DEFINES += RHINO_CONFIG_WORKQUEUE

ifneq ($(HOST_MCU_FAMILY),mcu_esp8266)
$(NAME)_COMPONENTS  += cli
GLOBAL_DEFINES += CLI_CONFIG_STACK_SIZE=3072
else
GLOBAL_DEFINES += ESP8266_CHIPSET
endif

