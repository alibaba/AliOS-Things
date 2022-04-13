NAME := netmgr_rpc

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY :=

$(NAME)_INCLUDES += ./

ifeq ($(AOS_COMP_NETMGR),)
$(NAME)_SOURCES := netmgr_rpc_client.c
else
$(NAME)_SOURCES := netmgr_rpc_server.c
endif

$(NAME)_CFLAGS += -Werror

GLOBAL_INCLUDES += ./include
GLOBAL_INCLUDES += ../network/netmgr
