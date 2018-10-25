NAME := bt_mesh

$(NAME)_TYPE := kernel

$(NAME)_INCLUDES += ./port/adv/ \
                    ./port/crypto/ \
                    ./port/gatt/ \
                    ./inc/ \
                    ./inc/api/mesh/ \
                    ../bt/common/tinycrypt/include/ \
                    ../bt/host/ \
                    ../bt/include/

$(NAME)_COMPONENTS += yloop

$(NAME)_SOURCES := ./port/adv/bt_mesh_adv.c \
                   ./port/crypto/bt_mesh_crypto.c \
                   ./port/gatt/bt_mesh_gatt.c \
                   ./src/access.c \
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

GLOBAL_INCLUDES += ./inc/ \
                   ./inc/api/ \
                   ./port/
