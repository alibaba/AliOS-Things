NAME := socket_rpc_client

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Socket rpc client implement

$(NAME)_SOURCES     := lwip_rpc_client.c
$(NAME)_SOURCES     += lwip_rpc_vfs.c

GLOBAL_INCLUDES     += ../include
GLOBAL_INCLUDES     += ../../include
