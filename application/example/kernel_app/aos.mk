NAME := kernel_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := kernel_app
$(NAME)_SOURCES := kernel_app.c task_app.c bufqueue_app.c

GLOBAL_DEFINES += AOS_NO_WIFI

