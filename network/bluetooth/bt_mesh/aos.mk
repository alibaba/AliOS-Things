NAME := bt_mesh

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := BLE Mesh stack.

$(NAME)_COMPONENTS-y += bt_common bt_mesh_port

$(NAME)_INCLUDES-y += ./inc/ \
                      ./inc/api/mesh

$(NAME)_SOURCES-y := ./src/access.c \
                     ./src/adv.c \
                     ./src/beacon.c \
                     ./src/crypto.c \
                     ./src/cfg_srv.c \
                     ./src/cfg_cli.c \
                     ./src/health_srv.c \
                     ./src/health_cli.c \
                     ./src/main.c \
                     ./src/net.c \
                     ./src/prov.c \
                     ./src/proxy.c \
                     ./src/transport.c \
                     ./src/lpn.c \
                     ./src/friend.c \
                     ./src/shell.c

GLOBAL_INCLUDES-y += ./inc/ \
                     ./inc/api

GLOBAL_DEFINES-y += CONFIG_BT_MESH
