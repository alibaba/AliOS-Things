NAME := mesh_hal_ref_impl

$(NAME)_COMPONENTS := 

$(NAME)_SOURCES := ./mesh_hal_ble.c \
                   ./mesh_hal_sec.c

ifeq ($(bt_mesh_standalone_deploy),1)
$(NAME)_SOURCES += ./mesh_hal_os.c
GLOBAL_INCLUDES += .
endif

ifneq ($(bt_mesh_standalone_deploy),1)
$(NAME)_COMPONENTS += bluetooth.bt_host
GLOBAL_INCLUDES += ../bt_host/host/ \
                   ../bt_host/include/
endif
