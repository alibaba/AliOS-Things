########## ota #################
NAME := OTA

GLOBAL_DEFINES += OTA_LINUX
GLOBAL_DEFINES += OTA_WITH_LINKKIT
GLOBAL_DEFINES += OTA_SIGNAL_CHANNEL=1
GLOBAL_DEFINES += AOS_OTA_HTTPS

OTA_SRC_DIR := ./src
OTA_INC_DIR := ./inc
OTA_HAL_DIR := ./hal
OTA_EXT_DIR := ./ext

$(NAME)_SOURCES += \
    $(OTA_SRC_DIR)/ota_service.c \
    $(OTA_SRC_DIR)/transport/ota_transport_mqtt.c \
    $(OTA_SRC_DIR)/transport/ota_transport_coap.c \
    $(OTA_SRC_DIR)/download/ota_download_http.c \
    $(OTA_SRC_DIR)/download/ota_download_coap.c \
    $(OTA_SRC_DIR)/verify/ota_verify.c \
    $(OTA_SRC_DIR)/verify/crc/ota_crc.c \
    $(OTA_SRC_DIR)/verify/ota_rsa_verify.c \
    $(OTA_SRC_DIR)/verify/hash/ota_hash.c \
    $(OTA_SRC_DIR)/verify/hash/ota_md5.c \
    $(OTA_SRC_DIR)/verify/hash/ota_sha256.c \
    $(OTA_SRC_DIR)/verify/rsa/ota_bignum.c \
    $(OTA_SRC_DIR)/verify/rsa/ota_rsa.c \
    $(OTA_SRC_DIR)/verify/rsa/ota_hash_rsa.c \
    $(OTA_SRC_DIR)/verify/base64/ota_base64.c \
    $(OTA_HAL_DIR)/ota_hal_os.c \
    $(OTA_HAL_DIR)/ota_hal_host.c \
    $(OTA_HAL_DIR)/ota_hal_module.c

GLOBAL_INCLUDES += $(OTA_INC_DIR)
$(NAME)_INCLUDES += $(OTA_HAL_DIR) \
    $(OTA_SRC_DIR)/service \
    $(OTA_SRC_DIR)/transport \
    $(OTA_SRC_DIR)/download  \
    $(OTA_SRC_DIR)/verify  \
    $(OTA_SRC_DIR)/verify/rsa  \
    $(OTA_SRC_DIR)/verify/hash  \
    $(OTA_SRC_DIR)/verify/crc \
    $(OTA_SRC_DIR)/verify/base64 \

########## coap #################
COAP_SRC_DIR := $(OTA_EXT_DIR)/linkkit/sdk-c/src/protocol/coap/cloud
LK_SOURCES += \
    $(COAP_SRC_DIR)/CoAPDeserialize.c \
    $(COAP_SRC_DIR)/CoAPExport.c \
    $(COAP_SRC_DIR)/CoAPMessage.c \
    $(COAP_SRC_DIR)/CoAPNetwork.c \
    $(COAP_SRC_DIR)/CoAPSerialize.c \
    $(COAP_SRC_DIR)/iotx_coap_api.c \
    $(COAP_SRC_DIR)/iotx_ca_cert.c

$(NAME)_INCLUDES +=  $(COAP_SRC_DIR)/src
$(NAME)_CFLAGS += -g -Wall -Werror

########## mqtt #################
GLOBAL_DEFINES += IOTX_WITHOUT_ITLS    #TLS
#GLOBAL_DEFINES += IOTX_WITHOUT_TLS    #ITLS
GLOBAL_DEFINES += USE_LPTHREAD
GLOBAL_DEFINES += MQTT_DIRECT
GLOBAL_DEFINES += MQTT_COMM_ENABLED
MQTT_SRC_DIR := $(OTA_EXT_DIR)/linkkit/sdk-c/src/protocol/mqtt
GLOBAL_INCLUDES += $(MQTT_SRC_DIR)  $(MQTT_SRC_DIR)/client
LK_SOURCES += $(MQTT_SRC_DIR)/client/mqtt_client.c\
	$(MQTT_SRC_DIR)/MQTTPacket/MQTTConnectClient.c \
	$(MQTT_SRC_DIR)/MQTTPacket/MQTTDeserializePublish.c \
	$(MQTT_SRC_DIR)/MQTTPacket/MQTTPacket.c \
	$(MQTT_SRC_DIR)/MQTTPacket/MQTTSerializePublish.c \
	$(MQTT_SRC_DIR)/MQTTPacket/MQTTSubscribeClient.c  \
	$(MQTT_SRC_DIR)/MQTTPacket/MQTTUnsubscribeClient.c

########## iotkit #################
IOTKIT_DIR := $(OTA_EXT_DIR)/linkkit/sdk-c/src/infra
IOTKIT_INC := $(OTA_EXT_DIR)/linkkit/sdk-c/include
GLOBAL_INCLUDES  += $(IOTKIT_INC) \
        $(IOTKIT_INC)/imports \
	$(IOTKIT_INC)/exports \
        $(IOTKIT_DIR)/system \
        $(IOTKIT_DIR)/log/  \
	$(IOTKIT_DIR)/utils \
        $(IOTKIT_DIR)/utils/digest  \
        $(IOTKIT_DIR)/utils/misc  \

LK_SOURCES  +=  \
	$(IOTKIT_DIR)/log/iotx_log.c \
	$(IOTKIT_DIR)/system/ca.c \
	$(IOTKIT_DIR)/system/device.c \
	$(IOTKIT_DIR)/system/guider.c \
	$(IOTKIT_DIR)/system/report.c \
        $(IOTKIT_DIR)/system/aos_support.c \
	$(IOTKIT_DIR)/../sdk-impl/sdk-impl.c \
        $(IOTKIT_DIR)/../sdk-impl/impl_dynreg.c

LK_SOURCES  += $(IOTKIT_DIR)/utils/digest/utils_base64.c \
	$(IOTKIT_DIR)/utils/digest/utils_hmac.c \
	$(IOTKIT_DIR)/utils/digest/utils_md5.c\
	$(IOTKIT_DIR)/utils/digest/utils_sha1.c \
	$(IOTKIT_DIR)/utils/digest/utils_sha256.c

LK_SOURCES  += $(IOTKIT_DIR)/utils/misc/json_parser.c \
	$(IOTKIT_DIR)/utils/misc/json_token.c \
	$(IOTKIT_DIR)/utils/misc/mem_stats.c \
	$(IOTKIT_DIR)/utils/misc/string_utils.c \
	$(IOTKIT_DIR)/utils/misc/utils_epoch_time.c \
	$(IOTKIT_DIR)/utils/misc/utils_httpc.c \
	$(IOTKIT_DIR)/utils/misc/utils_list.c \
	$(IOTKIT_DIR)/utils/misc/utils_net.c \
	$(IOTKIT_DIR)/utils/misc/utils_timer.c \
	$(IOTKIT_DIR)/utils/misc/utils_event.c \
	$(IOTKIT_DIR)/utils/misc/lite-cjson.c

HAL_DIR = $(OTA_EXT_DIR)/linkkit/hal
LK_SOURCES  += $(HAL_DIR)/HAL_UDP_linux.c \
	$(HAL_DIR)/HAL_OS_linux.c \
	$(HAL_DIR)/HAL_TCP_linux.c \
	$(HAL_DIR)/HAL_Crypt_linux.c \
        $(HAL_DIR)/kv.c \
        $(HAL_DIR)/base64.c \
	$(HAL_DIR)/cJSON.c \
	
$(NAME)_INCLUDES += $(OTA_EXT_DIR)/linkkit/hal

#tls 
MBEDTLS_DIR = $(OTA_EXT_DIR)/linkkit/sdk-c/src/ref-impl
$(NAME)_INCLUDES += $(MBEDTLS_DIR)/tls/include/mbedtls \
	$(MBEDTLS_DIR)/tls/include \
	$(MBEDTLS_DIR)/hal \

LK_SOURCES  += $(MBEDTLS_DIR)/hal/ssl/mbedtls/HAL_TLS_mbedtls.c
LK_SOURCES  += $(wildcard $(MBEDTLS_DIR)/tls/library/*.c)

#$(info $($(NAME)_SOURCES))
#$(info $($(NAME)_CFLAGS))
GLOBAL_INCLUDES += $($(NAME)_INCLUDES)
GLOBAL_DEFINES += $($(NAME)_DEFINES)

$(foreach inc, $(GLOBAL_INCLUDES), $(eval BASE_INCLUDES += -I$(inc)))
$(info $(BASE_INCLUDES))

$(foreach inc, $(GLOBAL_DEFINES), $(eval BASE_DEFINES += -D$(inc)))
$(info $(BASE_DEFINES))

LK_OBJS  := $(patsubst %.c,output/%.o,$(LK_SOURCES))
OTA_OBJS := $(patsubst %.c,output/%.o,$($(NAME)_SOURCES))

CC ?= gcc
AR ?= ar
APP := $(NAME)_APP
APP_SRC := app/ota_app.c
APP_CFLAGS := -g -Wall -Werror -Os

.PHONY: all

all: $(OTA_OBJS) $(LK_OBJS)
	$(AR) -cr lib$(NAME).a $(OTA_OBJS)
	$(AR) -cr libLK.a $(LK_OBJS)
	$(CC) $(APP_SRC) $(APP_CFLAGS) -o $(APP) -lLK -l$(NAME) -lLK -lpthread -lrt  -L./ -I$(IOTKIT_INC) -I$(IOTKIT_INC)/imports -I$(IOTKIT_INC)/exports -I$(OTA_SRC_DIR)/service -I$(OTA_INC_DIR) -I$(OTA_HAL_DIR)

$(OTA_OBJS): output/%.o : %.c
	@mkdir -p $$(dirname $@)
	$(CC) -c $< -o $@  $(BASE_INCLUDES) $(BASE_DEFINES) $($(NAME)_CFLAGS)

$(LK_OBJS): output/%.o : %.c
	@mkdir -p $$(dirname $@)
	$(CC) -c $< -o $@  $(BASE_INCLUDES) $(BASE_DEFINES) $($(NAME)_CFLAGS)

clean:
	rm -frv output/ $(APP) *.a *.kv *.elf
