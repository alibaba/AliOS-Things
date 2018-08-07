DEFAULT_BLD             ?= $(CONFIG_DIR)/config.ubuntu.x86
POST_FINAL_OUT_HOOK     := Post_Distro
SUBDIRS                 := directory-not-exist-actually
UTEST_PROG              := sdk-testsuites
ifeq (Darwin,$(shell uname))
POST_FINAL_OUT_HOOK     :=
endif

FEATURE_MQTT_SHADOW             ?= n
FEATURE_COAP_DTLS_SUPPORT       ?= $(FEATURE_COAP_COMM_ENABLED)
FEATURE_OTA_ENABLED             ?= y
FEATURE_OTA_FETCH_CHANNEL       ?= HTTP
FEATURE_OTA_SIGNAL_CHANNEL      ?= MQTT
FEATURE_MQTT_COMM_ENABLED       ?= n

CONFIG_LIB_EXPORT               ?= static

# gateway & subdevice
FEATURE_SUBDEVICE_STATUS        ?= gateway   
# MQTT & CLOUD_CONN
FEATURE_SUBDEVICE_CHANNEL       ?= MQTT 

FEATURE_CMP_VIA_MQTT_DIRECT     ?= y
# MQTT & COAP & HTTP
FEATURE_CMP_VIA_CLOUD_CONN      ?= MQTT
FEATURE_SUPPORT_PRODUCT_SECRET  ?= n

CFLAGS  += -D_IS_LINKKIT_
CFLAGS  += -DFORCE_SSL_VERIFY
CFLAGS  += \
    -DCOAP_OBSERVE_SERVER_SUPPORTED \
    -DCOAP_OBSERVE_CLIENT_SUPPORTED \
	-DCOAP_SERV_MULTITHREAD \
    -DCOAP_USE_PLATFORM_MEMORY -DCOAP_USE_PLATFORM_LOG \

#
# Hacks for cmake sub-system
#
EXTRA_INSTALL_HDRS  := \
    $(EXPORT_DIR)/iot_import.h \
    $(EXPORT_DIR)/iot_export.h \
    $(EXPORT_DIR)/imports \
    $(EXPORT_DIR)/exports \

EXTRA_INCLUDE_DIRS	:= \
	$(EXPORT_DIR) \
	src/ref-impl/tls \
	src/security/pro \
	src/tools/linkkit_tsl_convert/include \
	build-rules \
	src/ref-impl/hal \

WIN32_CMAKE_SKIP    := \
    linkkit_tsl_convert \
    coap-example \

NOEXEC_CMAKE_DIRS   := tests src/services/linkkit/dm

