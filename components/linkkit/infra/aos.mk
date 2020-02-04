NAME := libiot_infra

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.2
$(NAME)_SUMMARY := system tools

$(NAME)_SOURCES-y += infra_defs.c
$(NAME)_SOURCES-$(INFRA_AES) += infra_aes.c
$(NAME)_SOURCES-$(INFRA_CJSON) += infra_cjson.c
$(NAME)_SOURCES-$(INFRA_COMPAT) += infra_compat.c
$(NAME)_SOURCES-$(INFRA_HTTPC) += infra_httpc.c
$(NAME)_SOURCES-$(INFRA_JSON_PARSER) += infra_json_parser.c
$(NAME)_SOURCES-$(INFRA_LOG) += infra_log.c
$(NAME)_SOURCES-$(INFRA_MD5) += infra_md5.c
$(NAME)_SOURCES-$(INFRA_NET) += infra_net.c
$(NAME)_SOURCES-$(INFRA_PREAUTH) += infra_preauth.c
$(NAME)_SOURCES-$(INFRA_REPORT) += infra_report.c
$(NAME)_SOURCES-$(INFRA_LOG_NETWORK_PAYLOAD) += infra_prt_nwk_payload.c
$(NAME)_SOURCES-$(INFRA_SHA1) += infra_sha1.c
$(NAME)_SOURCES-$(INFRA_SHA256) += infra_sha256.c
$(NAME)_SOURCES-$(INFRA_STRING) += infra_string.c
$(NAME)_SOURCES-$(INFRA_TIMER) += infra_timer.c
$(NAME)_SOURCES-$(INFRA_STRING) += infra_string.c

ifeq (y,$(strip $(MQTT_COMM_ENABLED)))
$(NAME)_INCLUDES += ../mqtt
endif

ifeq (y,$(strip $(DEVICE_MODEL_ENABLED)))
$(NAME)_INCLUDES += ../dev_model
endif
