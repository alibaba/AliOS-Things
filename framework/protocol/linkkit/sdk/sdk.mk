NAME := linkkit_sdk
$(NAME)_SELF_BUIlD_COMP_targets=output/release/lib/libiot_sdk.a


$(NAME)_SELF_BUIlD_COMP_scripts=iotx_sdk_make.sh

GLOBAL_INCLUDES += iotx-sdk-c_clone/src/sdk-impl \
        iotx-sdk-c_clone/src/infra/utils/digest \
        iotx-sdk-c_clone/src/infra/utils \
        iotx-sdk-c_clone/src/infra/utils/misc \
        iotx-sdk-c_clone/src/infra/log \
        iotx-sdk-c_clone/include/exports \
        iotx-sdk-c_clone/include/imports \
        iotx-sdk-c_clone/include \
        iotx-sdk-c_clone/src/protocol/alcs \
        iotx-sdk-c_clone/src/services/linkkit/dm \
        iotx-sdk-c_clone/src/services/linkkit/cm \
        iotx-sdk-c_clone/src/utils/misc  \
        iotx-sdk-c_clone/src/protocol/mqtt/Link-MQTT \
        iotx-sdk-c_clone/src/protocol/mqtt \
        iotx-sdk-c_clone/src/protocol/mqtt/client
