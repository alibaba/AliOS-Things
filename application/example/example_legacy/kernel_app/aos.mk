NAME := kernel_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := kernel_app
$(NAME)_SOURCES := kernel_app.c task_app.c bufqueue_app.c mutex_app.c task_asyndelete.c \
                   queue_app.c timer_app.c sem_app.c work_app.c event_app.c

GLOBAL_DEFINES += AOS_NO_WIFI

