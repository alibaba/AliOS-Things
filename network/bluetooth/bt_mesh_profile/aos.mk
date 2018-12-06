NAME := bt_mesh_profile

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Tmall BLE Mesh profile implementation.

$(NAME)_COMPONENTS-y := bt_mesh

$(NAME)_SOURCES-y := bt_mesh_profile.c

GLOBAL_INCLUDES-y := ./
