NAME := athost

$(NAME)_SOURCES := \
                ./athost.c \
                ./util/athost_io.c \
                ./util/at_util.c  \
                ./atcmd/at_wifi_mgmt.c \
                ./atcmd/at_tcp_udp.c
                #./atcmd/at_uart.c \
                #./atcmd/at_wifi_ywss.c

ifeq (1,$(coap))
CONFIG_COAP_DTLS_SUPPORT := y
CONFIG_COAP_ONLINE := y

ifeq ($(CONFIG_COAP_DTLS_SUPPORT), y)
$(NAME)_DEFINES += COAP_DTLS_SUPPORT
endif
ifeq ($(CONFIG_COAP_ONLINE), y)
$(NAME)_DEFINES += COAP_ONLINE
endif

$(NAME)_SOURCES += ./atcmd/at_coap.c
$(NAME)_COMPONENTS += feature.linkkit-coap
GLOBAL_DEFINES +=  COAP_AT
else
$(NAME)_SOURCES += ./atcmd/at_mqtt.c
$(NAME)_COMPONENTS += feature.linkkit-mqtt
GLOBAL_DEFINES +=  MQTT_TEST USE_LPTHREAD
endif

GLOBAL_INCLUDES += ./include \
                   ./include/internal
