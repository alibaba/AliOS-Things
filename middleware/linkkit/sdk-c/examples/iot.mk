DEPENDS             := src/ref-impl/hal
DEPENDS             += src/ref-impl/tls

HDR_REFS            += src/infra
HDR_REFS            += src/services

LDFLAGS             := -Bstatic
LDFLAGS             += -liot_sdk

LDFLAGS             += -liot_hal
CFLAGS              := $(filter-out -ansi,$(CFLAGS))
ifneq (,$(filter -D_PLATFORM_IS_WINDOWS_,$(CFLAGS)))
LDFLAGS             += -lws2_32
CFLAGS              := $(filter-out -DCOAP_COMM_ENABLED,$(CFLAGS))
endif
ifneq (,$(filter -DSUPPORT_ITLS,$(CFLAGS)))
LDFLAGS             += -litls
else
LDFLAGS             += -liot_tls
endif

SRCS_mqtt-example-rrpc          := app_entry.c mqtt/mqtt_example_rrpc.c
SRCS_mqtt-example               := app_entry.c mqtt/mqtt_example.c
SRCS_mqtt-example-multithread   := app_entry.c mqtt/mqtt_example_multithread.c
SRCS_http2-example-uploadfile   := http2/http2_example_uploadfile.c app_entry.c
SRCS_http2-example              := http2/http2_example_stream.c app_entry.c
SRCS_coap-example               := coap/coap_example.c app_entry.c
SRCS_http-example               := http/http_example.c app_entry.c
SRCS_ota-example-mqtt           := ota/ota_example_mqtt.c
SRCS_linkkit-example-cota       := app_entry.c linkkit/linkkit_example_cota.c
SRCS_linkkit-example-sched      := app_entry.c cJSON.c linkkit/linkkit_example_sched.c
SRCS_linkkit-example-solo       := app_entry.c cJSON.c linkkit/linkkit_example_solo.c
SRCS_linkkit-example-countdown  := app_entry.c cJSON.c linkkit/linkkit_example_cntdown.c
SRCS_linkkit-example-gw         := app_entry.c cJSON.c linkkit/linkkit_example_gateway.c
SRCS_linkkit-example-dev-shadow := app_entry.c cJSON.c linkkit/linkkit_example_dev_shadow.c

# Syntax of Append_Conditional
# ---
#
# $(call Append_Conditional, TARGET, \  <-- Operated Variable
#   member1 member2 ...            , \  <-- Appended Members
#   switch1 switch2 ...            , \  <-- All These Switches are Defined
#   switch3 switch4 ...)                <-- All These Switches are Not Defined (Optional)


$(call Append_Conditional, TARGET, mqtt-example-rrpc,           MQTT_COMM_ENABLED)
$(call Append_Conditional, TARGET, mqtt-example,                MQTT_COMM_ENABLED)
$(call Append_Conditional, TARGET, mqtt-example-multithread,    MQTT_COMM_ENABLED)

$(call Append_Conditional, LDFLAGS, \
    -litls \
    -lid2client \
    -lkm \
    -lplat_gen \
    -lalicrypto \
    -lmbedcrypto \
, \
SUPPORT_ITLS, \
SUPPORT_TLS)

$(call Append_Conditional, TARGET, coap-example,                COAP_COMM_ENABLED)
$(call Append_Conditional, TARGET, http-example,                HTTP_COMM_ENABLED)

$(call Append_Conditional, TARGET, http2-example,               HTTP2_COMM_ENABLED)
$(call Append_Conditional, TARGET, http2-example-uploadfile,    HTTP2_COMM_ENABLED FS_ENABLED)

$(call Append_Conditional, TARGET, ota-example-mqtt,            OTA_ENABLED MQTT_COMM_ENABLED)
$(call Append_Conditional, TARGET, linkkit-example-cota, \
    OTA_ENABLED DEVICE_MODEL_ENABLED, \
    DEPRECATED_LINKKIT \
)

ifneq (,$(filter -DDEPRECATED_LINKKIT,$(CFLAGS)))
$(call Append_Conditional, TARGET, linkkit-example-solo,        DEVICE_MODEL_ENABLED, DEVICE_MODEL_GATEWAY)
$(call Append_Conditional, TARGET, linkkit-example-gw,          DEVICE_MODEL_ENABLED  DEVICE_MODEL_GATEWAY)
else
$(call Append_Conditional, TARGET, linkkit-example-solo,        DEVICE_MODEL_ENABLED)
$(call Append_Conditional, TARGET, linkkit-example-gw,          DEVICE_MODEL_ENABLED  DEVICE_MODEL_GATEWAY)
$(call Append_Conditional, TARGET, linkkit-example-dev-shadow,  DEVICE_MODEL_ENABLED  DEVICE_MODEL_SHADOW)
endif

$(call Append_Conditional, TARGET, linkkit-example-countdown,   DEVICE_MODEL_ENABLED, DEVICE_MODEL_GATEWAY)
ifneq (Darwin,$(shell uname))
$(call Append_Conditional, TARGET, linkkit-example-sched,       DEVICE_MODEL_ENABLED, DEVICE_MODEL_GATEWAY)
endif

# Clear All Above when Build for Windows
#
ifneq (,$(filter -D_PLATFORM_IS_WINDOWS_,$(CFLAGS)))
    TARGET          := mqtt-example linkkit-example-solo
endif
