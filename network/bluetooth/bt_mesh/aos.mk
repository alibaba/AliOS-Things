NAME := bt_mesh

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := BLE Mesh stack.

$(NAME)_COMPONENTS += bt_common bt_port

$(NAME)_INCLUDES += ./inc/ \
                      ./inc/api/mesh

$(NAME)_SOURCES := ./src/access.c \
                     ./src/adv.c \
                     ./src/beacon.c \
                     ./src/crypto.c \
                     ./src/cfg_srv.c \
                     ./src/health_srv.c \
                     ./src/main.c \
                     ./src/net.c \
                     ./src/prov.c \
                     ./src/transport.c

$(NAME)_SOURCES-$(CONFIG_BT_MESH_FRIEND) += ./src/friend.c
$(NAME)_SOURCES-$(CONFIG_BT_MESH_LOW_POWER) += ./src/lpn.c
$(NAME)_SOURCES-$(CONFIG_BT_MESH_PROXY) += ./src/proxy.c

$(NAME)_SOURCES-$(CONFIG_BT_MESH_CFG_CLI) += ./src/cfg_cli.c
$(NAME)_SOURCES-$(CONFIG_BT_MESH_HEALTH_CLI) += ./src/health_cli.c

$(NAME)_SOURCES-$(CONFIG_BT_MESH_SHELL) += ./src/shell.c

GLOBAL_INCLUDES += ./inc/ \
                   ./inc/api

GLOBAL_DEFINES += CONFIG_BT_MESH
