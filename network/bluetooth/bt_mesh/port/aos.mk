NAME := bt_mesh_port

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_COMPONENTS-y := network.bluetooth.bt

$(NAME)_SOURCES-y := ./adv/bt_mesh_adv.c \
                   ./crypto/bt_mesh_crypto.c \
                   ./gatt/bt_mesh_gatt.c \

GLOBAL_INCLUDES-y +=  ./adv/ \
                    ./crypto/ \
                    ./gatt/ \
                    ../../bt/host/ \
                    ../../bt/include/
