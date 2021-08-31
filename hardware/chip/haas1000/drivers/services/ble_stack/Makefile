cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)common/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hl/src/gap/gapc/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hl/src/gap/gapm/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hl/src/gap/smpc/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hl/src/gatt/attc/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hl/src/gatt/attm/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hl/src/gatt/atts/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hl/src/gatt/gattc/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hl/src/gatt/gattm/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hl/src/l2c/l2cc/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hl/src/l2c/l2cm/*.c))
#obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hl/src/prf/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hl/src/rwble_hl/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hci/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ke/src/*.c))
#obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)dbg/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ble_ip/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)util/*.c))

BTBLE_LIB_NAME := $(CHIP)_libble_stack

ifeq ($(HFP_1_6_ENABLE), 1)
BTBLE_LIB_NAME := $(BTBLE_LIB_NAME)_sbc_enc
endif

$(BTBLE_LIB_NAME)-y := $(obj_c:.c=.o)
obj-y := $(BTBLE_LIB_NAME).a

subdir-ccflags-y += \
	-Iservices/bridge/ \
	-Iservices/bt_app/ \
	$(BT_IF_INCLUDES) \
	$(BT_PROFILES_INCLUDES) \
	-Iservices/ble_stack/common/api/ \
	-Iservices/ble_stack/hl/inc/ \
	-Iservices/ble_stack/hl/api/ \
	-Iservices/ble_stack/hl/src/gap/ \
	-Iservices/ble_stack/hl/src/gap/gapc/ \
	-Iservices/ble_stack/hl/src/gap/gapm/ \
	-Iservices/ble_stack/hl/src/gap/smpc/ \
	-Iservices/ble_stack/ke/api/ \
	-Iservices/ble_stack/ke/src/ \
	-Iservices/ble_stack/hl/src/gatt/attc/ \
	-Iservices/ble_stack/hl/src/gatt/attm/ \
	-Iservices/ble_stack/hl/src/gatt/atts/ \
	-Iservices/ble_stack/hl/src/gatt/gattc/ \
	-Iservices/ble_stack/hl/src/gatt/gattm/ \
	-Iservices/ble_stack/hl/src/gatt/ \
	-Iservices/ble_stack/hl/src/l2c/l2cc/ \
	-Iservices/ble_stack/hl/src/l2c/l2cm/ \
	-Iservices/ble_stack/hci/api/ \
	-Iservices/ble_stack/hci/src/ \
	-Iservices/ble_app/app_main/	\
	-Iservices/ble_stack/ble_ip/  \
	-Iservices/ble_app/app_datapath/	\
	-Iservices/ble_stack/app/api/  \
	-Iservices/ble_stack/app/src/

