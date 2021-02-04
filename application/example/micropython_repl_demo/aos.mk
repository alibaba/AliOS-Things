NAME := repl_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := repl_demo
$(NAME)_SOURCES := maintask.c appdemo.c

$(NAME)_INCLUDES += ../../../components/haascv
$(NAME)_COMPONENTS += osal_aos
$(NAME)_COMPONENTS += littlefs
$(NAME)_COMPONENTS += haascv

GLOBAL_DEFINES += AOS_NO_WIFI
