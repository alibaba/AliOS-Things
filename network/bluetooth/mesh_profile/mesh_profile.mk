NAME := mesh_profile

$(NAME)_MBINS_TYPE := kernel

$(NAME)_SOURCES := mesh_profile.c

GLOBAL_INCLUDES := ./ \
                   ../../../network/bluetooth/include \
                   ../../../network/bluetooth/core/include \
                   ../../../network/bluetooth/common/tinycrypt/include
