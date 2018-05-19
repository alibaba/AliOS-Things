NAME := iotkit
$(NAME)_TYPE := framework
GLOBAL_INCLUDES += sdk-encap \
                  sdk-encap/imports \
                  iotkit-system \
                  base/log/LITE-log  \
                  base/utils  \
                  base/utils/include \
                  layers/config/include \
                  iotkit-system \
                          

include framework/protocol/linkkit/iotkit/base/log/iot-log.mk
include framework/protocol/linkkit/iotkit/base/utils/iot-utils.mk
include framework/protocol/linkkit/iotkit/hal-impl/iot-hal.mk
include framework/protocol/linkkit/iotkit/iotkit-system/iotkit-system.mk
include framework/protocol/linkkit/iotkit/layers/iot-layers.mk

ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing
endif


$(NAME)_COMPONENTS += digest_algorithm cjson base64 log modules.fs.kv  hal mbedtls

GLOBAL_DEFINES += CONFIG_IOT_KIT
