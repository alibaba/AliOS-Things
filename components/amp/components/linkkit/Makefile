TARGET = liblinkkit.a
MODULE = linkkit

MOD_SOURCES := \
		certs/root_ca.c

MOD_SOURCES += \
		dev_model/dm_api.c \
		dev_model/dm_client_adapter.c \
		dev_model/dm_client.c \
		dev_model/dm_cota.c \
		dev_model/dm_fota.c \
		dev_model/dm_ipc.c \
		dev_model/dm_log_report.c \
		dev_model/dm_manager.c \
		dev_model/dm_message_cache.c \
		dev_model/dm_message.c \
		dev_model/dm_msg_process.c \
		dev_model/dm_opt.c \
		dev_model/dm_ota.c \
		dev_model/dm_server_adapter.c \
		dev_model/dm_server.c \
		dev_model/dm_shadow.c \
		dev_model/dm_tsl_alink.c \
		dev_model/dm_utils.c \
		dev_model/impl_gateway.c \
		dev_model/impl_linkkit.c \
		dev_model/impl_ntp.c \
		dev_model/impl_solo.c \
		dev_model/iotx_cm_coap.c \
		dev_model/iotx_cm_mqtt.c \
		dev_model/iotx_cm.c

MOD_SOURCES += \
		dev_sign/dev_sign_mqtt.c

MOD_SOURCES += \
		dynamic_register/dynreg.c

MOD_SOURCES += \
		infra/infra_activation.c \
		infra/infra_aes.c \
		infra/infra_cjson.c \
		infra/infra_compat.c \
		infra/infra_defs.c \
		infra/infra_httpc.c \
		infra/infra_json_parser.c \
		infra/infra_log.c \
		infra/infra_md5.c \
		infra/infra_net.c \
		infra/infra_sha256.c \
		infra/infra_string.c \
		infra/infra_timer.c \
		infra/infra_report.c \
		infra/infra_prt_nwk_payload.c

MOD_SOURCES += \
		mqtt/iotx_mqtt_client.c \
		mqtt/mqtt_api.c \
		mqtt/MQTTConnectClient.c \
		mqtt/MQTTDeserializePublish.c \
		mqtt/MQTTPacket.c \
		mqtt/MQTTSerializePublish.c \
		mqtt/MQTTSubscribeClient.c \
		mqtt/MQTTUnsubscribeClient.c

MOD_INCLUDES := \
		. \
		dev_model \
		dev_sign \
		dynamic_register \
		infra \
		mqtt \
		../ulog

# MOD_OBJS_DIR = $(COMP_DIR)/$(MODULE)/build

include $(TOOLS_DIR)/rules.mk
