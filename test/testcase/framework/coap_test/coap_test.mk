NAME := coap_test

$(NAME)_COMPONENTS  += connectivity.coap

$(NAME)_SOURCES     += coap_test.c

CONFIG_COAP_DTLS_SUPPORT := y
#CONFIG_COAP_ONLINE := y

ifeq ($(CONFIG_COAP_DTLS_SUPPORT), y)
$(NAME)_DEFINES += COAP_DTLS_SUPPORT
endif
ifeq ($(CONFIG_COAP_ONLINE), y)
$(NAME)_DEFINES += COAP_ONLINE
endif

$(NAME)_INCLUDES += ../../../../utility/iotx-utils/sdk-impl/imports/
$(NAME)_INCLUDES += ../../../../utility/iotx-utils/LITE-log
$(NAME)_INCLUDES += ../../../../utility/iotx-utils//LITE-utils
$(NAME)_INCLUDES += ../../../../utility/iotx-utils//digest
$(NAME)_INCLUDES += ../../../../connectivity/coap/iot-coap-c/
$(NAME)_INCLUDES += ../../../../utility/iotx-utils/sdk-impl

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

