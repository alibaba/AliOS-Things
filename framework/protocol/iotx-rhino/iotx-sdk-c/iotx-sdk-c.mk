NAME := iotx-sdk-c
$(NAME)_SELF_BUIlD_COMP_targets=output/release/lib/libiot_sdk.a
$(NAME)_SELF_BUIlD_COMP_scripts=iotx_sdk_make.sh

GLOBAL_INCLUDES += src/sdk-impl \
		src/utils/digest src/utils/misc \
		src/sdk-impl/exports src/utils/log src/sdk-impl/imports