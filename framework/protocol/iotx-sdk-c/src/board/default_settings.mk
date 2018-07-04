DEFAULT_BLD             := $(CONFIG_DIR)/config.aos.make
POST_FINAL_OUT_HOOK     := Post_Distro
SUBDIRS                 := directory-not-exist-actually
ifeq (y,$(strip $(FEATURE_SDK_UNIT_TEST)))
UTEST_PROG              := sdk-testsuites
endif
ifeq (Darwin,$(shell uname))
POST_FINAL_OUT_HOOK     :=
endif

FEATURE_MQTT_SHADOW         ?= $(FEATURE_MQTT_COMM_ENABLED)
FEATURE_COAP_DTLS_SUPPORT   ?= $(FEATURE_COAP_COMM_ENABLED)
FEATURE_MQTT_ID2_AUTH       ?= n
FEATURE_MQTT_ID2_CRYPTO     ?= n
FEATURE_OTA_ENABLED         ?= y
FEATURE_OTA_FETCH_CHANNEL   ?= HTTP
FEATURE_OTA_SIGNAL_CHANNEL  ?= MQTT
FEATURE_MQTT_ID2_ENV        ?= online
FEATURE_MQTT_COMM_ENABLED   ?= n
FEATURE_SERVICE_COTA_ENABLED   ?= n

CONFIG_LIB_EXPORT           ?= static

# gateway & subdevice
FEATURE_SUBDEVICE_STATUS    ?= gateway   
# MQTT & CLOUD_CONN
FEATURE_SUBDEVICE_CHANNEL   ?= MQTT 

FEATURE_CMP_VIA_MQTT_DIRECT ?= y
# MQTT & COAP & HTTP
FEATURE_CMP_VIA_CLOUD_CONN  ?= MQTT

FEATURE_SUPPORT_PRODUCT_SECRET ?= n

CFLAGS  += -DFORCE_SSL_VERIFY
CFLAGS  += \
    -DCOAP_OBSERVE_SERVER_SUPPORTED \
    -DCOAP_OBSERVE_CLIENT_SUPPORTED \
    -DCOAP_SERV_MULTITHREAD \
    -DCOAP_USE_PLATFORM_MEMORY -DCOAP_USE_PLATFORM_LOG \

EXTRA_INSTALL_HDRS  := \
    src/sdk-impl/iot_import.h \
    src/sdk-impl/iot_export.h \
    src/sdk-impl/imports \
    src/sdk-impl/exports \

EXTRA_INCLUDE_DIRS	:= \
	src/sdk-impl \
	src/thirdparty/tls \
	src/thirdparty/lib \
	src/tools/linkkit_tsl_convert/include \
	build-rules \
	src/platform \

WIN32_CMAKE_SKIP    := \
    linkkit_tsl_convert \
    linkkit-example \
    coap-example \

