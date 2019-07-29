NAME := tmall_model

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Tmall BLE Mesh model implementation.

$(NAME)_COMPONENTS-y := bt_mesh

$(NAME)_SOURCES-y := tmall_model.c
