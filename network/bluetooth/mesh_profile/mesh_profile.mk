NAME := mesh_profile

$(NAME)_MBINS_TYPE := kernel

$(NAME)_SOURCES := mesh_profile.c

$(NAME)_INCLUDES := ./ \
                   ../../bluetooth/bt/include \
                   ../../bluetooth/bt/common/tinycrypt/include

GLOBAL_INCLUDES := ./
