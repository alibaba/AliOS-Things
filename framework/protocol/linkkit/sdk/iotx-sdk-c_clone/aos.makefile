include build-rules/funcs.mk
include aos_board_conf.mk

CROSS_PREFIX := $(strip $(CROSS_PREFIX))

SHELL   := bash
Q       ?= @
VPATH   := .O/src/infra/utils .O/src/infra/log .O/src/infra/system .O/src/sdk-impl .O/src/protocol/mqtt .O/src/services/linkkit/cm .O/src/services/linkkit/dm .O/src/services/linkkit/dev_reset .O/src/protocol/coap/local .O/src/services/awss .O/src/services/dev_bind .O/src/protocol/alcs 

EXT_IFLAGS  := \
    -Ibuild-rules/misc \
    -Iinclude \
    -Iinclude/exports \
    -Iinclude/imports \
    -Isrc/infra \
    -Isrc/infra/utils \
    -Isrc/infra/utils/digest \
    -Isrc/infra/utils/misc \
    -Isrc/infra/log \
    -Isrc/infra/system \
    -Isrc/infra/system/facility \

IFLAGS  := \
    -Isrc/infra/utils \
    -Isrc/infra/utils/digest \
    -Isrc/infra/utils/misc \
    -Isrc/infra/log \
    -Isrc/infra/system \
    -Isrc/infra/system/facility \
    -Isrc/sdk-impl \
    -Isrc/sdk-impl/deprecated_linkkit \
    -Isrc/protocol/mqtt \
    -Isrc/protocol/mqtt/client \
    -Isrc/protocol/mqtt/MQTTPacket \
    -Isrc/services/linkkit/cm \
    -Isrc/services/linkkit/cm/include \
    -Isrc/services/linkkit/dm \
    -Isrc/services/linkkit/dm/client \
    -Isrc/services/linkkit/dm/server \
    -Isrc/services/linkkit/dev_reset \
    -Isrc/protocol/coap/local \
    -Isrc/services/awss \
    -Isrc/services/dev_bind \
    -Isrc/services/dev_bind/os \
    -Isrc/services/dev_bind/os/product \
    -Isrc/protocol/alcs \
    -Iinclude \
    -Iinclude/exports \
    -Iinclude/imports \
    -Isrc/ref-impl/tls \
    -Isrc/ref-impl/tls/include \
    -Isrc/ref-impl/tls/include/mbedtls \
    -Isrc/ref-impl/tls/library \
    -Isrc/security/pro \
    -Isrc/security/pro/crypto \
    -Isrc/security/pro/itls \
    -Isrc/security/pro/id2 \
    -Isrc/security/pro/km \
    -Isrc/tools/linkkit_tsl_convert/include \
    -Ibuild-rules \
    -Ibuild-rules/hooks \
    -Ibuild-rules/docs \
    -Ibuild-rules/misc \
    -Ibuild-rules/scripts \
    -Isrc/ref-impl/hal \
    -Isrc/ref-impl/hal/os \
    -Isrc/ref-impl/hal/os/macos \
    -Isrc/ref-impl/hal/os/win7 \
    -Isrc/ref-impl/hal/os/ubuntu \
    -Isrc/ref-impl/hal/ssl \
    -Isrc/ref-impl/hal/ssl/openssl \
    -Isrc/ref-impl/hal/ssl/itls \
    -Isrc/ref-impl/hal/ssl/mbedtls \

CFLAGS  := \
    -DALCS_ENABLED \
    -DAWSS_SUPPORT_AHA \
    -DAWSS_SUPPORT_APLIST \
    -DAWSS_SUPPORT_DEV_AP \
    -DAWSS_SUPPORT_PHONEASAP \
    -DAWSS_SUPPORT_SMARTCONFIG \
    -DAWSS_SUPPORT_SMARTCONFIG_WPS \
    -DAWSS_SUPPORT_STATIS \
    -DAWSS_SUPPORT_ZEROCONFIG \
    -DBUILD_AOS \
    -DCOAP_SERV_MULTITHREAD \
    -DCONFIG_DM_DEVTYPE_SINGLE \
    -DCONFIG_PLATFORM_8711B \
    -DCONFIG_YWSS \
    -DDEVICE_MODEL_ENABLED \
    -DDEV_BIND_ENABLED \
    -DFORCE_SSL_VERIFY \
    -DHardFault_Handler=Ali_HardFault_Handler \
    -DLOG_REPORT_TO_CLOUD \
    -DLOG_SIMPLE \
    -DM3 \
    -DMQTT_COMM_ENABLED \
    -DMQTT_DIRECT \
    -DOTA_ENABLED \
    -DSUPPORT_TLS \
    -DSYSINFO_APP_VERSION=app-1.0.0-20190411.0443 \
    -DSYSINFO_DEVICE_NAME=MK3080 \
    -DSYSINFO_KERNEL_VERSION=AOS-R-1.3.4 \
    -DSYSINFO_PRODUCT_MODEL=ALI_AOS_MK3080 \
    -DWIFI_PROVISION_ENABLED \
    -DWITH_MQTT_DYN_BUF=1 \
    -DWITH_MQTT_SUB_SHORTCUT=1 \
    -DWITH_MQTT_ZIP_TOPIC=1 \
    -Iexamples \
    -Iexamples/linkkit \
    -Itests \
    -Os \
    -Werror \
    -fdata-sections \
    -ffunction-sections \
    -fno-short-enums \
    -march=armv7-m \
    -mcpu=cortex-m4 \
    -mlittle-endian \
    -mthumb \
    -mthumb-interwork \
    -w \

STAMP_LCOV  := .O/.coverage.done

.PHONY: all
all: .O/usr/lib/libiot_sdk.a  
	$(Q)cp -rf include/iot_import.h include/iot_export.h include/imports include/exports  .O/usr/include 2>/dev/null || true
	@rm -f *.gcda *.gcno $$(find build-rules -name "*.o")

	@if [ "$(WITH_LCOV)" = "1" ]; then \
	    mkdir -p $$(dirname $(STAMP_LCOV)); \
	    touch $(STAMP_LCOV); \
	else \
	    rm -f $(STAMP_LCOV); \
	fi


.O/usr/lib/libiot_sdk.a: \
    .O/src/infra/utils/misc/json_token.o \
    .O/src/infra/utils/misc/json_parser.o \
    .O/src/infra/utils/misc/string_utils.o \
    .O/src/infra/utils/misc/utils_epoch_time.o \
    .O/src/infra/utils/misc/utils_event.o \
    .O/src/infra/utils/misc/utils_httpc.o \
    .O/src/infra/utils/misc/mem_stats.o \
    .O/src/infra/utils/misc/utils_timer.o \
    .O/src/infra/utils/misc/log_report.o \
    .O/src/infra/utils/misc/lite-cjson.o \
    .O/src/infra/utils/misc/utils_net.o \
    .O/src/infra/utils/digest/utils_hmac.o \
    .O/src/infra/utils/digest/utils_md5.o \
    .O/src/infra/utils/digest/utils_sha256.o \
    .O/src/infra/utils/digest/utils_base64.o \
    .O/src/infra/utils/digest/utils_sha1.o \
    .O/src/infra/log/iotx_log.o \
    .O/src/infra/system/aos_support.o \
    .O/src/infra/system/guider.o \
    .O/src/infra/system/report.o \
    .O/src/infra/system/device.o \
    .O/src/infra/system/event.o \
    .O/src/infra/system/ca.o \
    .O/src/infra/system/facility/fac_string.o \
    .O/src/sdk-impl/sdk-impl.o \
    .O/src/sdk-impl/impl_linkkit.o \
    .O/src/sdk-impl/impl_logpost.o \
    .O/src/sdk-impl/impl_dynreg.o \
    .O/src/sdk-impl/deprecated_linkkit/impl_solo.o \
    .O/src/sdk-impl/deprecated_linkkit/impl_ntp.o \
    .O/src/sdk-impl/deprecated_linkkit/impl_gateway.o \
    .O/src/protocol/mqtt/client/mqtt_client.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTPacket.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTSerializePublish.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTSubscribeClient.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTDeserializePublish.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTUnsubscribeClient.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTConnectClient.o \
    .O/src/services/linkkit/cm/iotx_cm.o \
    .O/src/services/linkkit/cm/iotx_cm_coap.o \
    .O/src/services/linkkit/cm/iotx_cm_mqtt.o \
    .O/src/services/linkkit/dm/dm_api.o \
    .O/src/services/linkkit/dm/dm_cota.o \
    .O/src/services/linkkit/dm/dm_fota.o \
    .O/src/services/linkkit/dm/dm_ipc.o \
    .O/src/services/linkkit/dm/dm_manager.o \
    .O/src/services/linkkit/dm/dm_message.o \
    .O/src/services/linkkit/dm/dm_message_cache.o \
    .O/src/services/linkkit/dm/dm_msg_process.o \
    .O/src/services/linkkit/dm/dm_opt.o \
    .O/src/services/linkkit/dm/dm_ota.o \
    .O/src/services/linkkit/dm/dm_shadow.o \
    .O/src/services/linkkit/dm/dm_tsl_alink.o \
    .O/src/services/linkkit/dm/dm_utils.o \
    .O/src/services/linkkit/dm/server/dm_server_adapter.o \
    .O/src/services/linkkit/dm/server/dm_server.o \
    .O/src/services/linkkit/dm/client/dm_client_adapter.o \
    .O/src/services/linkkit/dm/client/dm_client.o \
    .O/src/services/linkkit/dev_reset/awss_reset.o \
    .O/src/services/linkkit/dev_reset/awss_reset_statis.o \
    .O/src/protocol/coap/local/CoAPSerialize.o \
    .O/src/protocol/coap/local/CoAPObserve.o \
    .O/src/protocol/coap/local/CoAPNetwork.o \
    .O/src/protocol/coap/local/CoAPPlatform.o \
    .O/src/protocol/coap/local/CoAPExport.o \
    .O/src/protocol/coap/local/CoAPResource.o \
    .O/src/protocol/coap/local/CoAPServer.o \
    .O/src/protocol/coap/local/CoAPDeserialize.o \
    .O/src/protocol/coap/local/CoAPMessage.o \
    .O/src/services/awss/awss_wifimgr.o \
    .O/src/services/awss/awss_main.o \
    .O/src/services/awss/zconfig_ieee80211.o \
    .O/src/services/awss/awss_dev_ap.o \
    .O/src/services/awss/awss_enrollee.o \
    .O/src/services/awss/awss_adha.o \
    .O/src/services/awss/awss_aplist.o \
    .O/src/services/awss/awss_smartconfig.o \
    .O/src/services/awss/zconfig_vendor_common.o \
    .O/src/services/awss/zconfig_utils.o \
    .O/src/services/awss/awss_aha.o \
    .O/src/services/awss/zconfig_protocol.o \
    .O/src/services/awss/awss_info.o \
    .O/src/services/awss/awss_crypt.o \
    .O/src/services/awss/awss_wps.o \
    .O/src/services/awss/awss_registrar.o \
    .O/src/services/awss/awss_ht40.o \
    .O/src/services/awss/awss_statis.o \
    .O/src/services/awss/awss.o \
    .O/src/services/dev_bind/awss_bind.o \
    .O/src/services/dev_bind/awss_bind_statis.o \
    .O/src/services/dev_bind/awss_event.o \
    .O/src/services/dev_bind/awss_notify.o \
    .O/src/services/dev_bind/awss_timer.o \
    .O/src/services/dev_bind/passwd.o \
    .O/src/services/dev_bind/awss_cmp_coap.o \
    .O/src/services/dev_bind/awss_cmp_mqtt.o \
    .O/src/services/dev_bind/sha256.o \
    .O/src/services/dev_bind/awss_report.o \
    .O/src/services/dev_bind/awss_packet.o \
    .O/src/services/dev_bind/os/os_misc.o \
    .O/src/protocol/alcs/alcs_mqtt.o \
    .O/src/protocol/alcs/alcs_api.o \
    .O/src/protocol/alcs/alcs_adapter.o \
    .O/src/protocol/alcs/alcs_client.o \
    .O/src/protocol/alcs/alcs_localsetup.o \
    .O/src/protocol/alcs/alcs_server.o \
    .O/src/protocol/alcs/alcs_coap.o \

	$(Q)mkdir -p $$(dirname $@)
	$(Q)$(call Brief_Log,"AR",$$(basename $@),"...")
	$(Q)$(CROSS_PREFIX)ar -rcs $@ $^ 2>/dev/null

%.o:
	$(Q)$(call Brief_Log,"CC",$$(basename $@),"...")
	$(Q)mkdir -p $$(dirname $@)
	S=$$(echo $@|sed 's,.O,.,1'); \
    $(CROSS_PREFIX)gcc -c \
        -o $@ \
        $(CONFIG_ENV_CFLAGS) \
        -I../../../../../include \
        -I../../../../activation \
        $(CFLAGS) \
        $(IFLAGS) \
        $${S//.o/.c} 



