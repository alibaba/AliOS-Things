NAME := udata_local_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := uData local demo

$(NAME)_SOURCES    :=   udata_local_demo.c
$(NAME)_COMPONENTS := cli udata

$(NAME)_INCLUDES += ./

