NAME := bt_mesh

$(NAME)_TYPE := kernel
GLOBAL_INCLUDES += .

$(NAME)_INCLUDES += ../../core/tinycrypt/include/ \
                    ../../include/bluetooth/mesh/

$(NAME)_COMPONENTS += yloop

$(NAME)_SOURCES := main.c \
                   adv.c \
                   beacon.c \
                   net.c \
                   transport.c \
                   crypto.c \
                   access.c \
                   cfg_srv.c \
                   cfg_cli.c \
                   health_srv.c \
                   proxy.c \
                   prov.c
