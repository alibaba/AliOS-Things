NAME := bt_mesh

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_COMPONENTS-y += network.bluetooth.bt.bt_common network.bluetooth.bt_mesh.port

$(NAME)_INCLUDES-y += ./inc/ \
                      ./inc/api/mesh/

$(NAME)_COMPONENTS-y += yloop

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
                     ./src/transport.c

GLOBAL_INCLUDES-y += ./inc/ \
                     ./inc/api/

GLOBAL_DEFINES-y += CONFIG_BT_MESH
