MODULE_NAME := src/services/fota
ALL_SUB_DIRS := samples src/platform src/protocol/alcs src/protocol/coap src/protocol/http src/protocol/http2 src/protocol/mqtt src/security/base src/services/cota src/services/file_upload src/services/fota src/services/linkkit src/services/linkkit/cm src/services/linkkit/dm src/services/ota src/services/shadow src/services/subdev src/thirdparty/tfs src/thirdparty/tls src/tools/linkkit_tsl_convert src/utils tests
AR := arm-none-eabi-ar
CC := arm-none-eabi-gcc
CFLAGS := --coverage -DBUILD_AOS -DCMP_VIA_MQTT_DIRECT -DCM_ENABLED -DCM_SUPPORT_MULTI_THREAD -DCM_SUPPORT_MULTI_THREAD_VIA_HAL -DCM_VIA_CLOUD_CONN -DCM_VIA_CLOUD_CONN_MQTT -DCOAP_COMM_ENABLED -DCOAP_DTLS_SUPPORT -DCOAP_OBSERVE_CLIENT_SUPPORTED -DCOAP_OBSERVE_SERVER_SUPPORTED -DCOAP_SERV_MULTITHREAD -DCOAP_USE_PLATFORM_LOG -DCOAP_USE_PLATFORM_MEMORY -DCONFIG_DM_DEVTYPE_GATEWAY -DCONFIG_GUIDER_AUTH_TIMEOUT=500 -DCONFIG_HTTP_AUTH_TIMEOUT=500 -DCONFIG_MID_HTTP_TIMEOUT=500 -DDM_ENABLED -DGATEWAY_SUPPORT -DHTTP_COMM_ENABLED -DIOTX_NET_INIT_WITH_PK_EXT -DIOTX_WITHOUT_ITLS -DMQTT_COMM_ENABLED -DMQTT_DIRECT -DMQTT_SHADOW -DOTA_ENABLED -DOTA_SIGNAL_CHANNEL=1 -DSDK_ENHANCE -DSERVICE_OTA_ENABLED -DSUBDEVICE_ENABLED -DSUBDEVICE_SUPPORT -DSUBDEV_VIA_MQTT -DSUPPORT_TLS -DTEST_COAP_DAILY -DTEST_HTTP_DAILY -DTEST_ON_DAILY -I/home/zhangtian/work/aos/framework/protocol/iotx-rhino/iotx-sdk-c/../../../include -MD -g3 -march=armv5te -mcpu=arm968e-s -mlittle-endian -mthumb -mthumb-interwork -w
COMP_LIB := libiot_sdk.a
COMP_LIB_COMPONENTS := src/protocol/coap src/protocol/http src/protocol/mqtt src/security/base src/services/linkkit/cm src/services/linkkit/dm src/services/ota src/services/shadow src/services/subdev src/utils
CONFIG_VENDOR := rhino
CROSS_CANDIDATES := AR CC CXX LD OBJCOPY STRIP
CROSS_PREFIX := arm-none-eabi-
CXX := arm-none-eabi-g++
IMPORT_DIR := /home/zhangtian/work/aos/framework/protocol/iotx-rhino/iotx-sdk-c/src/thirdparty/lib
INSTALL_DIR := /home/zhangtian/work/aos/framework/protocol/iotx-rhino/iotx-sdk-c/.O/usr
INSTALL_LIB_DIR := /home/zhangtian/work/aos/framework/protocol/iotx-rhino/iotx-sdk-c/.O/usr/lib
KERNEL_DIR := 
LD := arm-none-eabi-ld
LDFLAGS := -lpthread
MAKE_ENV_VARS := 
OBJCOPY := arm-none-eabi-objcopy
PACKAGE_DIR := /home/zhangtian/work/aos/framework/protocol/iotx-rhino/iotx-sdk-c/src/packages
RULE_DIR := /home/zhangtian/work/aos/framework/protocol/iotx-rhino/iotx-sdk-c/build-rules
STRIP := arm-none-eabi-strip
SYSROOT_INC := /home/zhangtian/work/aos/framework/protocol/iotx-rhino/iotx-sdk-c/.O/usr/include
TOP_DIR := /home/zhangtian/work/aos/framework/protocol/iotx-rhino/iotx-sdk-c
WIN32_CMAKE_SKIP := coap-example linkkit-example linkkit_tsl_convert
COMP_LIB_FILES :=  libiot_utils.a  libiot_system.a  libiot_mqtt.a  libiot_ota.a  libiot_shadow.a  libiot_coap.a  libiot_http.a  libiot_subdev.a  libiot_cmp.a  libiot_dm.a

include $(RULE_DIR)/settings.mk
include $(CONFIG_TPL)

all:

LIBA_TARGET         := libiot_fota.a
HDR_REFS            := src
env:
	@echo ""
	@printf -- "-----------------------------------------------------------------\n"
	@$(foreach var,$(SHOW_ENV_VARS),$(call Dump_Var,$(var)))
	@printf -- "-----------------------------------------------------------------\n"
	@echo ""

include $(RULE_DIR)/rules.mk
