DEFAULT_BLD             ?= $(CONFIG_DIR)/config.ubuntu.x86
POST_FINAL_OUT_HOOK     := Post_Distro
SUBDIRS                 := directory-not-exist-actually
UTEST_PROG              := sdk-testsuites
ifeq (Darwin,$(shell uname))
POST_FINAL_OUT_HOOK     :=
endif

FEATURE_MQTT_SHADOW             ?= n
FEATURE_COAP_DTLS_SUPPORT       ?= $(FEATURE_COAP_COMM_ENABLED)
FEATURE_ALCS_ENABLED            ?= y
FEATURE_OTA_ENABLED             ?= y
FEATURE_OTA_FETCH_CHANNEL       ?= HTTP
FEATURE_OTA_SIGNAL_CHANNEL      ?= MQTT
FEATURE_MQTT_COMM_ENABLED       ?= n
FEATURE_HTTP_COMM_ENABLED       ?= n
FEATURE_HTTP2_COMM_ENABLED      ?= n
FEATURE_SUBDEVICE_ENABLED       ?= n

## FEATURE_COAP_COMM_ENABLED
##
## Brief:   Establish non-persistent connection with AliCloud via CoAP-based protocol
## Usage:   Switching to "y" leads to building CoAP related implementation into SDK and COAP_COMM_ENABLED included into CFLAGS
##          Switching to "n" leads to keeping CoAP-based cloud connection implementations out of SDK
##
FEATURE_COAP_COMM_ENABLED       ?= n

## FEATURE_DEPRECATED_LINKKIT
##
## Brief:   Enable providing Thing-Model related functions via deprecated/old/obsoleted APIs
##          They're named as linkkit_xxx() or linkkit_gateway_xxx(), declared in linkkit_export.h and linkkit_gateway_export.h
##
##          NOTE: These APIs are known carrying non-fixable bugs so deprecated
##          NOTE: In long term we recommend migration to writing Thing-Model related applications with IOT_Linkkit_XXX() APIs
##
## Usage:   Switching to "y" leads to SDK providing APIs in linkkit_export.h + linkkit_gateway_export.h
##          Switching to "n" leads to SDK providing APIs in iot_export_linkkit.h
##
FEATURE_DEPRECATED_LINKKIT      ?= y

CONFIG_LIB_EXPORT               ?= static

CFLAGS  += -Iexamples

CFLAGS  += -DOTA_WITH_LINKKIT -DOTA_RSA_DISABLE
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

EXTRA_INCLUDE_DIRS  := \
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

