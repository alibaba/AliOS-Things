NAME := uspace_rpc

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Enable Remote Process Call.

$(NAME)_SOURCES := rpc.c
#$(NAME)_SOURCES := rpc.c

$(NAME)_COMPONENTS += uspace_parcel

$(NAME)_INCLUDES += ./

GLOBAL_INCLUDES += ./include

