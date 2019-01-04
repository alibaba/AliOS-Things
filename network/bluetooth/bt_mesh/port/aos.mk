NAME := port

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := This is an AliOS Things based implementation of BLE Mesh HAL API.

$(NAME)_COMPONENTS-y := bt_host

$(NAME)_SOURCES-y := ./adv/bt_mesh_adv.c \
                     ./crypto/bt_mesh_crypto.c \
                     ./gatt/bt_mesh_gatt.c \

GLOBAL_INCLUDES-y +=  ./adv/ \
                      ./crypto/ \
                      ./gatt/ \
                      ../../bt_host/host/ \
                      ../../bt_host/include/
