include build-rules/funcs.mk
include win_board_conf.mk

CROSS_PREFIX := $(strip $(CROSS_PREFIX))

SHELL := bash
Q     ?= @
VPATH := .O/src/infra/utils .O/src/infra/log .O/src/infra/system .O/src/sdk-impl .O/src/protocol/mqtt .O/src/protocol/http .O/src/protocol/alcs .O/src/protocol/coap/local .O/src/services/linkkit/cm .O/src/services/linkkit/dm

.PHONY: all
all: .O/usr/lib/libiot_sdk.a
	$(Q)cp -rf include/iot_import.h include/iot_export.h include/imports include/exports  .O/usr/include 2>/dev/null || true
	@rm -f *.gcda *.gcno $$(find build-rules -name "*.o")

OTA_OBJECTS=.O/src/services/ota/src/ota.o \

AWSS_OBJECTS=.O/src/services/awss/awss_wifimgr.o \
    .O/src/services/awss/awss_notify.o \
    .O/src/services/awss/awss_packet.o \
    .O/src/services/awss/awss_timer.o \
    .O/src/services/awss/registrar.o \
    .O/src/services/awss/zconfig_ieee80211.o \
    .O/src/services/awss/awss_cmp_coap.o \
    .O/src/services/awss/zconfig_vendor_common.o \
    .O/src/services/awss/zconfig_utils.o \
    .O/src/services/awss/awss_cmp_mqtt.o \
    .O/src/services/awss/enrollee.o \
    .O/src/services/awss/zconfig_protocol.o \
    .O/src/services/awss/sha256.o \
    .O/src/services/awss/awss.o \
    .O/src/services/awss/awss_report.o \
    .O/src/services/awss/awss_main.o \
    .O/src/services/awss/passwd.o \
    .O/src/services/awss/os/os_misc.o \

DEV_BIND_OBJECTS=.O/src/services/dev_bind/awss_notify.o .O/src/services/dev_bind/awss_packet.o \
    .O/src/services/dev_bind/awss_timer.o .O/src/services/dev_bind/awss_cmp_coap.o \
	.O/src/services/dev_bind/awss_cmp_mqtt.o .O/src/services/dev_bind/awss_report.o \
	.O/src/services/dev_bind/os/os_misc.o

DEV_RESET_OBJECTS=.O/src/services/linkkit/dev_reset/awss_reset.o
NTP_OBJECTS=.O/src/services/ntp/ntp.o

ifeq ($(COMPILER),armcc)
CC:=armcc
TOOL_CHAIN_CFLAGS := -c --c99 -D__MICROLIB -g --gnu --library_type=microlib -W -Ospace -O3 -Isrc/infra/stdc/armlibc
AR=armar
AR_FALGS:= -rcs
TOOL_CHAIN_SPEC_OBJS:=
else ifeq ($(COMPILER),iar)
CC:=iccarm
TOOL_CHAIN_CFLAGS := --vla -e --dlib_config=full -D_TIMESPEC_DEFINED --silent --only_stdout --no_warnings --diag_warning=Pe167,Pe144,Pe513 -Ohz -Isrc/infra/stdc/iarlibc
AR=iarchive
AR_FALGS:= --create
TOOL_CHAIN_SPEC_OBJS:=
else
CC:=gcc
TOOL_CHAIN_CFLAGS := -c -Os
AR=ar
AR_FALGS:= -rcs
TOOL_CHAIN_SPEC_OBJS:=
endif
		
ifeq ($(APP),linkkit_gateway)
INPUT_FLAGS := -DFORCE_SSL_VERIFY -DCOAP_SERV_MULTITHREAD -DALCS_ENABLED -DDEPRECATED_LINKKIT -DDEVICE_MODEL_GATEWAY -DMQTT_COMM_ENABLED -DMQTT_DIRECT -DDEVICE_MODEL_ENABLED -DSUPPORT_TLS -DWIFI_PROVISION_ENABLED -DDEV_BIND_ENABLED -DCONFIG_SDK_THREAD_COST=1 -DOTA_SIGNAL_CHANNEL=1  -D__UBUNTU_SDK_DEMO__ -DWITH_MEM_STATS=0 -DWITH_MQTT_DYNBUF=1 -DCONFIG_HTTP_AUTH_TIMEOUT=500 -DCONFIG_MID_HTTP_TIMEOUT=500 -DCONFIG_GUIDER_AUTH_TIMEOUT=500 -DCONFIG_MQTT_RX_MAXLEN=5000 -DCONFIG_MQTT_SUBTOPIC_MAXNUM=65535 -DCONFIG_MBEDTLS_DEBUG_LEVEL=0 $(CONFIG_ENV_CFLAGS) $(TOOL_CHAIN_CFLAGS)
GATEEAY_OBJS := 
else
INPUT_FLAGS := -DFORCE_SSL_VERIFY -DCOAP_SERV_MULTITHREAD -DALCS_ENABLED -DDEPRECATED_LINKKIT -DMQTT_COMM_ENABLED -DMQTT_DIRECT -DDEVICE_MODEL_ENABLED -DSUPPORT_TLS -DWIFI_PROVISION_ENABLED -DDEV_BIND_ENABLED -DOTA_SIGNAL_CHANNEL=1 -D__UBUNTU_SDK_DEMO__ -DWITH_MEM_STATS=0 -DWITH_MQTT_DYNBUF=1 -DCONFIG_HTTP_AUTH_TIMEOUT=500 -DCONFIG_MID_HTTP_TIMEOUT=500 -DCONFIG_GUIDER_AUTH_TIMEOUT=500 -DCONFIG_MQTT_RX_MAXLEN=5000 -DCONFIG_MQTT_SUBTOPIC_MAXNUM=65535 -DCONFIG_MBEDTLS_DEBUG_LEVEL=0 $(CONFIG_ENV_CFLAGS) $(TOOL_CHAIN_CFLAGS)  
endif

.O/usr/lib/libiot_sdk.a: \
    .O/src/infra/utils/misc/linked_list.o \
    .O/src/infra/utils/misc/json_token.o \
    .O/src/infra/utils/misc/utils_list.o \
    .O/src/infra/utils/misc/json_parser.o \
    .O/src/infra/utils/misc/string_utils.o \
    .O/src/infra/utils/misc/utils_epoch_time.o \
    .O/src/infra/utils/misc/utils_event.o \
    .O/src/infra/utils/misc/utils_httpc.o \
    .O/src/infra/utils/misc/mem_stats.o \
    .O/src/infra/utils/misc/utils_timer.o \
    .O/src/infra/utils/misc/lite-cjson.o \
    .O/src/infra/utils/misc/utils_net.o \
    .O/src/infra/utils/digest/utils_hmac.o \
    .O/src/infra/utils/digest/utils_md5.o \
    .O/src/infra/utils/digest/utils_sha256.o \
    .O/src/infra/utils/digest/utils_base64.o \
    .O/src/infra/utils/digest/utils_sha1.o \
    .O/src/infra/log/iotx_log.o \
    .O/src/infra/system/guider.o \
    .O/src/infra/system/report.o \
    .O/src/infra/system/device.o \
    .O/src/infra/system/ca.o \
    .O/src/sdk-impl/sdk-impl.o \
    .O/src/sdk-impl/impl_linkkit.o \
    .O/src/sdk-impl/impl_logpost.o \
    .O/src/sdk-impl/impl_dynreg.o \
    .O/src/protocol/mqtt/client/mqtt_client.o \
    .O/src/protocol/mqtt/client/mqtt_instance.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTPacket.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTSerializePublish.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTSubscribeClient.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTDeserializePublish.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTUnsubscribeClient.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTConnectClient.o \
    .O/src/services/uota/hal/ota_hal_module.o \
    .O/src/protocol/alcs/alcs_mqtt.o \
    .O/src/protocol/alcs/alcs_api.o \
    .O/src/protocol/alcs/alcs_adapter.o \
    .O/src/protocol/alcs/alcs_client.o \
    .O/src/protocol/alcs/alcs_server.o \
    .O/src/protocol/alcs/alcs_coap.o \
    .O/src/protocol/coap/local/CoAPSerialize.o \
    .O/src/protocol/coap/local/CoAPObserve.o \
    .O/src/protocol/coap/local/CoAPNetwork.o \
    .O/src/protocol/coap/local/CoAPExport.o \
    .O/src/protocol/coap/local/CoAPResource.o \
    .O/src/protocol/coap/local/CoAPServer.o \
    .O/src/protocol/coap/local/CoAPPlatform.o \
    .O/src/protocol/coap/local/CoAPDeserialize.o \
    .O/src/protocol/coap/local/CoAPMessage.o \
    .O/src/services/linkkit/cm/iotx_cm_api.o \
    .O/src/services/linkkit/cm/iotx_cm_cloud_conn.o \
    .O/src/services/linkkit/cm/iotx_cm_common.o \
    .O/src/services/linkkit/cm/iotx_cm_conn_coap.o \
    .O/src/services/linkkit/cm/iotx_cm_connectivity.o \
    .O/src/services/linkkit/cm/iotx_cm_conn_http.o \
    .O/src/services/linkkit/cm/iotx_cm_conn_mqtt.o \
    .O/src/services/linkkit/cm/iotx_cm_local_conn.o \
    .O/src/services/linkkit/cm/iotx_cm_log.o \
    .O/src/services/linkkit/cm/iotx_local_conn_alcs.o \
    .O/src/services/linkkit/dm/dm_api.o \
    .O/src/services/linkkit/dm/dm_cm_wrapper.o \
    .O/src/services/linkkit/dm/dm_conn.o \
    .O/src/services/linkkit/dm/dm_cota.o \
    .O/src/services/linkkit/dm/dm_dispatch.o \
    .O/src/services/linkkit/dm/dm_fota.o \
    .O/src/services/linkkit/dm/dm_ipc.o \
    .O/src/services/linkkit/dm/dm_manager.o \
    .O/src/services/linkkit/dm/dm_message.o \
    .O/src/services/linkkit/dm/dm_message_cache.o \
    .O/src/services/linkkit/dm/dm_opt.o \
    .O/src/services/linkkit/dm/dm_ota.o \
    .O/src/services/linkkit/dm/dm_shadow.o \
    .O/src/services/linkkit/dm/dm_subscribe.o \
    .O/src/services/linkkit/dm/dm_tsl_alink.o \
    .O/src/services/linkkit/dm/dm_utils.o \
    .O/src/services/linkkit/dm/linkkit_gateway_legacy.o \
    .O/src/services/linkkit/dm/linkkit_solo_legacy.o \
    $(GATEEAY_OBJS) \
    $(OTA_OBJECTS)  \
    $(DEV_BIND_OBJECTS) \
    $(DEV_RESET_OBJECTS) \
    $(AWSS_OBJECTS) \
    $(NTP_OBJECTS) \
    $(TOOL_CHAIN_SPEC_OBJS)
    

	$(Q)mkdir -p $$(dirname $@)
	$(Q)$(call Brief_Log,"AR",$$(basename $@),"...")
	$(Q)$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)$(AR) $(AR_FALGS) $@ $^

%.o:
	$(Q)$(call Brief_Log,"CC",$$(basename $@),"...")
	$(Q)mkdir -p $$(dirname $@)
	$(Q)S=$$(echo $@|sed 's,.O,.,1'); \
    $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)$(CC) \
        -o $@ \
         $(INPUT_FLAGS) \
            -Isrc/infra/utils \
    -Isrc/infra/utils/misc \
    -Isrc/infra/utils/digest \
    -Isrc/infra/log \
    -Isrc/infra/system \
    -Isrc/sdk-impl \
    -Isrc/protocol/mqtt \
    -Isrc/protocol/mqtt/MQTTPacket \
    -Isrc/protocol/mqtt/client \
    -Isrc/services/uota/inc \
    -Isrc/services/uota/src \
    -Isrc/services/uota/src/download \
    -Isrc/services/uota/src/transport \
    -Isrc/services/uota/src/service \
    -Isrc/services/uota/src/recovery \
    -Isrc/services/uota/src/recovery/nbpatch \
    -Isrc/services/uota/src/recovery/xz-embedded \
    -Isrc/services/uota/src/recovery/xz-embedded/linux \
    -Isrc/services/uota/src/recovery/xz-embedded/linux/lib \
    -Isrc/services/uota/src/recovery/xz-embedded/linux/lib/xz \
    -Isrc/services/uota/src/recovery/xz-embedded/linux/Documentation \
    -Isrc/services/uota/src/recovery/xz-embedded/linux/scripts \
    -Isrc/services/uota/src/recovery/xz-embedded/linux/include \
    -Isrc/services/uota/src/recovery/xz-embedded/linux/include/linux \
    -Isrc/services/uota/src/recovery/xz-embedded/linux/include/linux/decompress \
    -Isrc/services/uota/src/recovery/xz-embedded/userspace \
    -Isrc/services/uota/src/otalib \
    -Isrc/services/uota/src/utility \
    -Isrc/services/uota/src/utility/cjson \
    -Isrc/services/uota/src/utility/base64 \
    -Isrc/services/uota/src/utility/hexstr2hexbuf \
    -Isrc/services/uota/src/utility/crc \
    -Isrc/services/uota/src/verify \
    -Isrc/services/uota/src/verify/rsa  \
    -Isrc/services/uota/src/verify/hash  \
    -Isrc/services/uota/hal \
    -Isrc/services/uota/app \
    -Isrc/services/awss \
    -Isrc/protocol/http \
    -Isrc/protocol/alcs \
    -Isrc/services/linkkit/cm \
    -Isrc/services/linkkit/ntp \
    -Isrc/services/linkkit/dev_rest \
    -Isrc/services/dev_bind \
    -Isrc/services/dev_bind/os \
    -Isrc/services/dev_bind/os/product \
    -Isrc/services/dev_bind/utility \
    -Isrc/services/linkkit/cm/example \
    -Isrc/services/linkkit/dm \
    -Isrc/services/linkkit/dm/example \
    -Iinclude \
    -Iinclude/imports \
    -Iinclude/exports \
    -Isrc/ref-impl/tls \
    -Isrc/ref-impl/tls/include \
    -Isrc/ref-impl/tls/include/mbedtls \
    -Isrc/ref-impl/tls/include/mbedtls/uOTA_required \
    -Isrc/ref-impl/tls/library \
    -Isrc/ref-impl/tls/library/uOTA_required \
    -Isrc/security/pro \
    -Isrc/security/pro/crypto \
    -Isrc/security/pro/id2 \
    -Isrc/security/pro/itls \
    -Isrc/security/pro/km \
    -Isrc/tools/linkkit_tsl_convert/include \
    -Ibuild-rules \
    -Ibuild-rules/misc \
    -Ibuild-rules/hooks \
    -Ibuild-rules/docs \
    -Ibuild-rules/scripts \
    -Isrc/ref-impl/hal \
    -Isrc/ref-impl/hal/os \
    -Isrc/ref-impl/hal/os/macos \
    -Isrc/ref-impl/hal/os/ubuntu \
    -Isrc/ref-impl/hal/os/win7 \
    -Isrc/ref-impl/hal/ssl \
    -Isrc/ref-impl/hal/ssl/mbedtls \
    -Isrc/ref-impl/hal/ssl/openssl \
    -Isrc/ref-impl/hal/ssl/itls \
        $${S//.o/.c}

clean:
	rm -fr .O
