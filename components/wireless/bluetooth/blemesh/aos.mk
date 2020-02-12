NAME := bt_mesh

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := BLE Mesh stack.

$(NAME)_COMPONENTS :=
$(NAME)_INCLUDES := ./include/
GLOBAL_INCLUDES += ../../../../include/wireless/bluetooth/blemesh

ifeq ($(bt_mesh_standalone_deploy),y)
$(NAME)_COMPONENTS += mesh_util
else
$(NAME)_COMPONENTS += bt_common
endif

$(NAME)_COMPONENTS += mesh_hal_ref_impl

$(NAME)_SOURCES := ./src/access.c \
                   ./src/adv.c \
                   ./src/beacon.c \
                   ./src/crypto.c \
                   ./src/cfg_srv.c \
                   ./src/health_srv.c \
                   ./src/main.c \
                   ./src/net.c \
                   ./src/prov.c \
                   ./src/proxy.c \
                   ./src/transport.c

$(NAME)_SOURCES-$(CONFIG_BT_MESH_FRIEND) += ./src/friend.c
$(NAME)_SOURCES-$(CONFIG_BT_MESH_LOW_POWER) += ./src/lpn.c
$(NAME)_SOURCES-$(CONFIG_BT_MESH_CFG_CLI) += ./src/cfg_cli.c
$(NAME)_SOURCES-$(CONFIG_BT_MESH_HEALTH_CLI) += ./src/health_cli.c
$(NAME)_SOURCES-$(CONFIG_BT_MESH_SHELL) += ./src/shell.c

#GLOBAL_DEFINES-$(AOS_COMP_BT_MESH) += CONFIG_BT_MESH

# Define your BT version here
#   0 - 1.0b
#   1 - 1.1
#   2 - 1.2
#   3 - 2.0
#   4 - 2.1
#   5 - 3.0
#   6 - 4.0
#   7 - 4.1
#   8 - 4.2
#   9 - 5.0
GLOBAL_CFLAGS-y += -DBT_HCI_VERSION=8
