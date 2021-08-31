ifneq ($(GEN_SDK_LIB_WW),1)
obj-y := userapi/

ifeq ($(TILE_DATAPATH_ENABLED), 1)
obj-y += tile
endif

ifeq ($(THIRDPARTY_LIB),)
THIRDPARTY_LIB := demo
endif

obj-y += $(foreach lib,$(THIRDPARTY_LIB),$(lib)_lib/)


else

cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

LIB_NAME := libww
ifneq ($(SDK),1)

src_obj := userapi/

ifeq ($(TILE_DATAPATH_ENABLED), 1)
src_obj += tile
endif

ifeq ($(THIRDPARTY_LIB),)
THIRDPARTY_LIB := demo
endif

src_obj += $(foreach lib,$(THIRDPARTY_LIB),$(lib)_lib/)

$(LIB_NAME)-y := $(src_obj)
endif

obj-y := $(LIB_NAME).a
endif

ifeq ($(BES_BLE_MESH_ENABLE),1)
obj-y += bluetooth/
endif