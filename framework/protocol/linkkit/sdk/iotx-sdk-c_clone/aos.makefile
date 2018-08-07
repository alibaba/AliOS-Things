include build-rules/funcs.mk
include aos_board_conf.mk

CROSS_PREFIX := $(strip $(CROSS_PREFIX))

SHELL := bash
Q     ?= @
VPATH := .O/src/infra/utils .O/src/infra/log .O/src/infra/system .O/src/sdk-impl .O/src/protocol/mqtt .O/src/protocol/http .O/src/protocol/alcs .O/src/services/linkkit/cm .O/src/services/linkkit/dm 

.PHONY: all
all: .O/usr/lib/libiot_sdk.a
	$(Q)cp -rf include/iot_import.h include/iot_export.h include/imports include/exports  .O/usr/include 2>/dev/null || true
	@rm -f *.gcda *.gcno $$(find build-rules -name "*.o")

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
    .O/src/infra/utils/misc/lite_queue.o \
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
    .O/src/protocol/mqtt/client/mqtt_client.o \
    .O/src/protocol/mqtt/client/mqtt_instance.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTPacket.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTSerializePublish.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTSubscribeClient.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTDeserializePublish.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTUnsubscribeClient.o \
    .O/src/protocol/mqtt/MQTTPacket/MQTTConnectClient.o \
    .O/src/protocol/http/iotx_http_api.o \
    .O/src/protocol/alcs/CoAPSerialize.o \
    .O/src/protocol/alcs/alcs_mqtt.o \
    .O/src/protocol/alcs/CoAPObserve.o \
    .O/src/protocol/alcs/alcs_api.o \
    .O/src/protocol/alcs/alcs_adapter.o \
    .O/src/protocol/alcs/CoAPNetwork.o \
    .O/src/protocol/alcs/CoAPExport.o \
    .O/src/protocol/alcs/alcs_client.o \
    .O/src/protocol/alcs/CoAPResource.o \
    .O/src/protocol/alcs/CoAPServer.o \
    .O/src/protocol/alcs/CoAPPlatform.o \
    .O/src/protocol/alcs/CoAPDeserialize.o \
    .O/src/protocol/alcs/CoAPMessage.o \
    .O/src/protocol/alcs/alcs_server.o \
    .O/src/protocol/alcs/alcs_coap.o \
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
    .O/src/services/linkkit/dm/linkkit_api.o \
    ./.O/src/services/linkkit/dm/linkkit_gateway_legacy.o \

	$(Q)mkdir -p $$(dirname $@)
	$(Q)$(call Brief_Log,"AR",$$(basename $@),"...")
	$(Q)$(CROSS_PREFIX)ar -rcs $@ $^

%.o:
	$(Q)$(call Brief_Log,"CC",$$(basename $@),"...")
	$(Q)mkdir -p $$(dirname $@)
	$(Q)S=$$(echo $@|sed 's,.O,.,1'); \
    $(CROSS_PREFIX)gcc -c \
        -o $@ \
        -D_IS_LINKKIT_ -DFORCE_SSL_VERIFY -DCOAP_OBSERVE_SERVER_SUPPORTED -DCOAP_OBSERVE_CLIENT_SUPPORTED -DCOAP_SERV_MULTITHREAD -DCOAP_USE_PLATFORM_MEMORY -DCOAP_USE_PLATFORM_LOG -DALCS_ENABLED -DHTTP_COMM_ENABLED -DMQTT_COMM_ENABLED -DMQTT_DIRECT -DSDK_ENHANCE -DSUPPORT_TLS -DCM_ENABLED -DDM_ENABLED -DCONFIG_DM_DEVTYPE_SINGLE -DCMP_VIA_MQTT_DIRECT $(CONFIG_ENV_CFLAGS) -g3 -DBUILD_AOS -DIOTX_NET_INIT_WITH_PK_EXT -DCM_VIA_CLOUD_CONN -DCM_VIA_CLOUD_CONN_MQTT -DCM_SUPPORT_MULTI_THREAD_VIA_HAL -I../../../../../include -I../../../../activation -DCM_SUPPORT_LOCAL_CONN -DCONFIG_DM_SUPPORT_LOCAL_CONN -DCONFIG_HTTP_AUTH_TIMEOUT=500 -DCONFIG_MID_HTTP_TIMEOUT=500 -DCONFIG_GUIDER_AUTH_TIMEOUT=500 -DCONFIG_MQTT_TX_MAXLEN=640 -DCONFIG_MQTT_RX_MAXLEN=1200 \
            -Isrc/infra/utils \
    -Isrc/infra/utils/misc \
    -Isrc/infra/utils/digest \
    -Isrc/infra/log \
    -Isrc/infra/system \
    -Isrc/sdk-impl \
    -Isrc/protocol/mqtt \
    -Isrc/protocol/mqtt/MQTTPacket \
    -Isrc/protocol/mqtt/client \
    -Isrc/protocol/http \
    -Isrc/protocol/alcs \
    -Isrc/services/linkkit/cm \
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

