NAME := bt_mesh

$(NAME)_TYPE := kernel
GLOBAL_INCLUDES += .

$(NAME)_INCLUDES += ../../common/tinycrypt/include/ \
                    ../../include/bluetooth/mesh/

ifeq ($(bt_tmall), 1)
GLOBAL_DEFINES += CONFIG_BT_MESH_TMALL_PROFILE
endif

$(NAME)_COMPONENTS += yloop

$(NAME)_SOURCES := main.c \
                   adv.c \
                   beacon.c \
                   net.c \
                   transport.c \
                   crypto.c \
                   access.c \
                   cfg_srv.c \
                   health_srv.c \
                   cfg_cli.c \
                   proxy.c \
                   prov.c
