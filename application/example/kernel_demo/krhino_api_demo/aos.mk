NAME := krhino_api_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := An example to show how to use the krhino kernel API
$(NAME)_SOURCES := maintask.c \
                   krhino_api_main.c \
                   task_app/task_new.c \
                   task_app/task_asyndelete.c \
                   bufqueue_app.c \
                   mutex_app.c \
                   queue_app.c \
                   timer_app.c \
                   sem_app.c \
                   work_app.c \
                   event_app.c

GLOBAL_DEFINES += AOS_NO_WIFI
GLOBAL_DEFINES += RHINO_CONFIG_WORKQUEUE

ifneq ($(HOST_MCU_FAMILY),mcu_esp8266)
$(NAME)_COMPONENTS  += cli
GLOBAL_DEFINES += CLI_CONFIG_STACK_SIZE=3072
else
GLOBAL_DEFINES += ESP8266_CHIPSET
endif
