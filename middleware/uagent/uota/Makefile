########## ota #################
NAME := OTA

GLOBAL_DEFINES += _PLATFORM_IS_LINUX_
GLOBAL_DEFINES += IOTX_WITHOUT_ITLS    #TLS
#GLOBAL_DEFINES += IOTX_WITHOUT_TLS    #ITLS
GLOBAL_DEFINES += USE_LPTHREAD 
GLOBAL_DEFINES += MQTT_DIRECT 
GLOBAL_DEFINES += MQTT_COMM_ENABLED
GLOBAL_DEFINES += CSP_LINUXHOST

GLOBAL_DEFINES += OTA_SIGNAL_CHANNEL=1
GLOBAL_DEFINES += OTA_HAL_MODULE
GLOBAL_DEFINES += HTTPS_DOWNLOAD
#GLOBAL_DEFINES += OTA_BREAKPOINT_SUPPORT
#GLOBAL_DEFINES += OTA_MULTI_BINS

OTA_SRC_DIR := ./src
OTA_INC_DIR := ./inc
OTA_HAL_DIR := ./hal
OTA_EXT_DIR := ./ext

$(NAME)_SOURCES += \
    $(OTA_SRC_DIR)/service/ota_service.c \
    $(OTA_SRC_DIR)/service/ota_version.c \
    $(OTA_SRC_DIR)/transport/ota_transport_mqtt.c \
    $(OTA_SRC_DIR)/transport/ota_transport_coap.c \
    $(OTA_SRC_DIR)/download/ota_manifest.c \
    $(OTA_SRC_DIR)/download/ota_download_http.c \
    $(OTA_SRC_DIR)/download/ota_download_coap.c \
    $(OTA_SRC_DIR)/verify/ota_util.c \
    $(OTA_SRC_DIR)/verify/ota_verify.c \
    $(OTA_SRC_DIR)/utility/cjson/cJSON.c \
    $(OTA_SRC_DIR)/utility/crc/crc.c \
    $(OTA_SRC_DIR)/verify/ota_rsa_verify.c \
    $(OTA_SRC_DIR)/verify/hash/ota_hash.c \
    $(OTA_SRC_DIR)/verify/hash/ota_md5.c \
    $(OTA_SRC_DIR)/verify/hash/ota_sha256.c \
    $(OTA_SRC_DIR)/verify/rsa/ota_bignum.c \
    $(OTA_SRC_DIR)/verify/rsa/ota_rsa.c \
    $(OTA_SRC_DIR)/verify/rsa/ota_hash_rsa.c \
    $(OTA_SRC_DIR)/utility/base64/ota_base64.c \
    $(OTA_SRC_DIR)/utility/hexstr2hexbuf/ota_hexstr2hexbuf.c \
    $(OTA_SRC_DIR)/otalib/ota.c \
    $(OTA_HAL_DIR)/ota_hal_os.c \
    $(OTA_HAL_DIR)/ota_hal_plat.c \
    $(OTA_HAL_DIR)/ota_hal_module.c

GLOBAL_INCLUDES += $(OTA_INC_DIR)
$(NAME)_INCLUDES += $(OTA_HAL_DIR) \
    $(OTA_SRC_DIR)/service \
    $(OTA_SRC_DIR)/transport \
    $(OTA_SRC_DIR)/download  \
    $(OTA_SRC_DIR)/verify  \
    $(OTA_SRC_DIR)/verify/rsa  \
    $(OTA_SRC_DIR)/verify/hash  \
    $(OTA_SRC_DIR)/utility/crc \
    $(OTA_SRC_DIR)/utility/cjson \
    $(OTA_SRC_DIR)/utility/base64 \
    $(OTA_SRC_DIR)/utility/hexstr2hexbuf \

########## coap #################
COAP_SRC_DIR := $(OTA_EXT_DIR)/linkkit/sdk/iotx-sdk-c_clone/src/protocol/coap
$(NAME)_SOURCES += \
    $(COAP_SRC_DIR)/CoAPDeserialize.c \
    $(COAP_SRC_DIR)/CoAPExport.c \
    $(COAP_SRC_DIR)/CoAPMessage.c \
    $(COAP_SRC_DIR)/CoAPNetwork.c \
    $(COAP_SRC_DIR)/CoAPSerialize.c \
    $(COAP_SRC_DIR)/iotx_coap_api.c \
    $(COAP_SRC_DIR)/iotx_ca_cert.c

$(NAME)_INCLUDES +=  $(COAP_SRC_DIR)/src
$(NAME)_CFLAGS += $(filter-out -Werror,$(CFLAGS)) -g -Wall

########## mqtt #################
MQTT_SRC_DIR := $(OTA_EXT_DIR)/linkkit/sdk/iotx-sdk-c_clone/src/protocol/mqtt
GLOBAL_INCLUDES += $(MQTT_SRC_DIR)  $(MQTT_SRC_DIR)/client
$(NAME)_SOURCES += $(MQTT_SRC_DIR)/client/mqtt_client.c\
	$(MQTT_SRC_DIR)/client/mqtt_instance.c \
	$(MQTT_SRC_DIR)/MQTTPacket/MQTTConnectClient.c \
	$(MQTT_SRC_DIR)/MQTTPacket/MQTTDeserializePublish.c \
	$(MQTT_SRC_DIR)/MQTTPacket/MQTTPacket.c \
	$(MQTT_SRC_DIR)/MQTTPacket/MQTTSerializePublish.c \
	$(MQTT_SRC_DIR)/MQTTPacket/MQTTSubscribeClient.c  \
	$(MQTT_SRC_DIR)/MQTTPacket/MQTTUnsubscribeClient.c

########## iotkit #################
IOTKIT_DIR := $(OTA_EXT_DIR)/linkkit/sdk/iotx-sdk-c_clone/src/infra
IOTKIT_INC := $(OTA_EXT_DIR)/linkkit/sdk/iotx-sdk-c_clone/include
GLOBAL_INCLUDES  += $(IOTKIT_INC) \
        $(IOTKIT_INC)/imports \
	$(IOTKIT_INC)/exports \
        $(IOTKIT_DIR)/system \
        $(IOTKIT_DIR)/log/  \
	$(IOTKIT_DIR)/utils \
        $(IOTKIT_DIR)/utils/digest  \
        $(IOTKIT_DIR)/utils/misc  \

$(NAME)_SOURCES  +=  \
	$(IOTKIT_DIR)/log/iotx_log.c \
	$(IOTKIT_DIR)/system/ca.c \
	$(IOTKIT_DIR)/system/device.c \
	$(IOTKIT_DIR)/system/guider.c \
	$(IOTKIT_DIR)/system/report.c \
	$(IOTKIT_DIR)/../sdk-impl/sdk-impl.c

$(NAME)_SOURCES  += $(IOTKIT_DIR)/utils/digest/utils_base64.c \
	$(IOTKIT_DIR)/utils/digest/utils_hmac.c \
	$(IOTKIT_DIR)/utils/digest/utils_md5.c\
	$(IOTKIT_DIR)/utils/digest/utils_sha1.c \
	$(IOTKIT_DIR)/utils/digest/utils_sha256.c

$(NAME)_SOURCES  += $(IOTKIT_DIR)/utils/misc/json_parser.c \
	$(IOTKIT_DIR)/utils/misc/json_token.c \
	$(IOTKIT_DIR)/utils/misc/mem_stats.c \
	$(IOTKIT_DIR)/utils/misc/string_utils.c \
	$(IOTKIT_DIR)/utils/misc/utils_epoch_time.c \
	$(IOTKIT_DIR)/utils/misc/utils_httpc.c \
	$(IOTKIT_DIR)/utils/misc/utils_list.c \
	$(IOTKIT_DIR)/utils/misc/utils_net.c \
	$(IOTKIT_DIR)/utils/misc/utils_timer.c \
	$(IOTKIT_DIR)/utils/misc/utils_event.c \
	$(IOTKIT_DIR)/utils/misc/lite_queue.c \
	$(IOTKIT_DIR)/utils/misc/lite-cjson.c

HAL_DIR = $(OTA_EXT_DIR)/linkkit/hal
$(NAME)_SOURCES  += $(HAL_DIR)/HAL_UDP_linux.c \
	$(HAL_DIR)/HAL_OS_linux.c \
	$(HAL_DIR)/HAL_TCP_linux.c \
	$(HAL_DIR)/HAL_Crypt_linux.c \
        $(HAL_DIR)/kv.c \
        $(HAL_DIR)/base64.c

#tls 
MBEDTLS_DIR = $(OTA_EXT_DIR)/linkkit/sdk/iotx-sdk-c_clone/src/ref-impl
$(NAME)_INCLUDES += $(MBEDTLS_DIR)/tls/include/mbedtls \
	$(MBEDTLS_DIR)/tls/include \
	$(MBEDTLS_DIR)/hal \

$(NAME)_SOURCES  += $(MBEDTLS_DIR)/hal/ssl/mbedtls/HAL_TLS_mbedtls.c
$(NAME)_SOURCES  += $(wildcard $(MBEDTLS_DIR)/tls/library/*.c)

#$(info $($(NAME)_SOURCES))
#$(info $($(NAME)_CFLAGS))
GLOBAL_INCLUDES += $($(NAME)_INCLUDES)
GLOBAL_DEFINES += $($(NAME)_DEFINES)

$(foreach inc, $(GLOBAL_INCLUDES), $(eval BASE_INCLUDES += -I$(inc)))
$(info $(BASE_INCLUDES))

$(foreach inc, $(GLOBAL_DEFINES), $(eval BASE_DEFINES += -D$(inc)))
$(info $(BASE_DEFINES))

OBJS := $(patsubst %.c,output/%.o,$($(NAME)_SOURCES))

CC ?= gcc
AR ?= ar
APP := $(NAME)_APP
APP_SRC := app/ota_app.c
APP_CFLAGS := -g

.PHONY: all

all: $(OBJS)
	$(AR) -cr lib$(NAME).a $(OBJS)
	$(CC) $(APP_SRC) $(APP_CFLAGS) -o $(APP) -l$(NAME) -lpthread -lrt  -L./ -I$(OTA_SRC_DIR)/service -I$(OTA_INC_DIR) -I$(OTA_HAL_DIR)

$(OBJS): output/%.o : %.c
	@mkdir -p $$(dirname $@)
	$(CC) -c $< -o $@  $(BASE_INCLUDES) $(BASE_DEFINES) $($(NAME)_CFLAGS)

clean:
	rm -frv output/ $(APP) lib$(NAME).a *.kv *.elf
