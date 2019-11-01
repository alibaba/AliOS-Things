NAME := mesh_hal_ref_impl

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := BLE Mesh HAL reference implementation

$(NAME)_SOURCES := ./mesh_hal_ble.c \
                   ./mesh_hal_sec.c

$(NAME)_INCLUDES += ../include/
ifeq ($(bt_mesh_standalone_deploy),1)
$(NAME)_SOURCES += ./mesh_hal_os.c
$(NAME)_INCLUDES += .
endif

ifneq ($(bt_mesh_standalone_deploy),1)
$(NAME)_COMPONENTS += bt_host
$(NAME)_INCLUDES += ../bt_host/host/ \
                    ../bt_host/include/
endif
