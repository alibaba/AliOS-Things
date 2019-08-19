NAME := athost

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := athost for communicaiton module

$(NAME)_SOURCES := ./src/athost.c             \
                   ./src/at_util.c 

$(NAME)_INCLUDES += ./                         \
                    ./src

ifeq (y,$(ATHOST_CONFIG_WIFI_MGMT))
$(NAME)_SOURCES += ./src/atcmd/at_wifi_mgmt.c
GLOBAL_DEFINES += ATHOST_WIFI_MGMT
endif

ifeq (y,$(ATHOST_CONFIG_CIP))
$(NAME)_SOURCES += ./src/atcmd/at_tcp_udp.c
GLOBAL_DEFINES += ATHOST_CIP
endif

ifeq (y,$(ATHOST_CONFIG_MQTT))
$(NAME)_SOURCES += ./src/atcmd/at_mqtt.c
$(NAME)_COMPONENTS += linkkit_sdk_c
GLOBAL_DEFINES +=  MQTT_TEST USE_LPTHREAD
GLOBAL_DEFINES += ATHOST_MQTT
else
ifeq (y,$(ATHOST_CONFIG_COAP))
CONFIG_COAP_DTLS_SUPPORT := y
CONFIG_COAP_ONLINE := y

ifeq ($(CONFIG_COAP_DTLS_SUPPORT), y)
$(NAME)_DEFINES += COAP_DTLS_SUPPORT
endif
ifeq ($(CONFIG_COAP_ONLINE), y)
$(NAME)_DEFINES += COAP_ONLINE
endif

$(NAME)_SOURCES += ./src/atcmd/at_coap.c
$(NAME)_COMPONENTS += linkkit_sdk_c

GLOBAL_DEFINES += ATHOST_COAP
endif
endif
