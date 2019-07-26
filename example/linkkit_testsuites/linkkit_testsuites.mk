NAME := linkkit_testsuites

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := linkkit test suites

$(NAME)_SOURCES := app_entry.c cut.c sdk-testsuites.c ut_devcert.c
$(NAME)_COMPONENTS :=  netmgr cjson cli \
						framework/protocol/linkkit/sdk \
                      framework/protocol/linkkit/hal  \
					  framework/common

$(NAME)_SOURCES +=  utils/*.c
$(NAME)_INCLUDES += ./  

include example/linkkit_testsuites/make.settings

ifeq ($(FEATURE_MQTT_COMM_ENABLED),y)
$(NAME)_SOURCES += mqtt/utest_devinfo.c \
                   mqtt/utest_mqtt_check.c \
				   mqtt/utest_mqtt_cons.c \
				   mqtt/utest_mqtt_normal.c \
				   mqtt/utest_mqtt_preauth.c \
				   mqtt/utest_mqtt_pub.c \
				   mqtt/utest_mqtt_sub.c \
				   mqtt/utest_mqtt_unsub.c \
				   mqtt/utest_mqtt_yield.c \
				   mqtt/utest_mqtt.c \
$(NAME)_INCLUDES += mqtt
endif

ifeq ($(FEATURE_HTTP2_COMM_ENABLED),y)
$(NAME)_SOURCES += http2/utest_http2_basic.c \
                   http2/utest_http2_stream.c
$(NAME)_INCLUDES += http2
endif

ifeq ($(LINKKIT_TEST_CONFIG_COAP),y)
$(NAME)_SOURCES += external/coap/
$(NAME)_INCLUDES += external/coap/
endif

ifeq ($(FEATURE_DEVICE_MODEL_ENABLED),y)
$(NAME)_SOURCES += linkkit/utest_dynamic_registration.c  \
				   linkkit/utest_oversea.c  \
                   linkkit/single_new/single_config_new.c \
				   linkkit/single_new/test_linkkit_single_api_close.c \
                   linkkit/single_new/test_linkkit_single_api_connect.c \
				   linkkit/single_new/test_linkkit_single_api_open.c \
				   linkkit/single_new/test_linkkit_single_api_report.c \
				   linkkit/single_new/test_linkkit_single_api_yield.c \
				   linkkit/single_new/test_linkkit_single_multithread.c \

$(NAME)_INCLUDES += linkkit linkkit/single_new
endif

ifeq ($(FEATURE_DEVICE_MODEL_GATEWAY),y)
$(NAME)_SOURCES += linkkit/utest_gw.c
endif


ifeq ($(LINKKIT_TEST_CONFIG_LOGE),y)
$(NAME)_DEFINES += TEST_SUITES_LOG_LEVEL=2
else 
ifeq ($(LINKKIT_TEST_CONFIG_LOGW),y)
$(NAME)_DEFINES += TEST_SUITES_LOG_LEVEL=3
else 
ifeq ($(LINKKIT_TEST_CONFIG_LOGI),y)
$(NAME)_DEFINES += TEST_SUITES_LOG_LEVEL=4
else 
ifeq ($(LINKKIT_TEST_CONFIG_LOGD),y)
$(NAME)_DEFINES += TEST_SUITES_LOG_LEVEL=5
else 
$(NAME)_DEFINES += TEST_SUITES_LOG_LEVEL=5
endif
endif
endif
endif



SWITCH_VARS := FEATURE_MQTT_COMM_ENABLED   FEATURE_ALCS_ENABLED FEATURE_MQTT_DIRECT FEATURE_DEVICE_MODEL_ENABLED  \
    FEATURE_DEVICE_MODEL_GATEWAY  FEATURE_DEPRECATED_LINKKIT   FEATURE_DEV_BIND_ENABLED \
    FEATURE_DEVICE_MODEL_RAWDATA_SOLO  FEATURE_COAP_COMM_ENABLED FEATURE_HTTP2_COMM_ENABLED \
    FEATURE_HTTP_COMM_ENABLED FEATURE_SAL_ENABLED  FEATURE_WIFI_PROVISION_ENABLED FEATURE_AWSS_SUPPORT_SMARTCONFIG\
    FEATURE_AWSS_SUPPORT_ZEROCONFIG FEATURE_AWSS_SUPPORT_SMARTCONFIG FEATURE_AWSS_SUPPORT_ZEROCONFIG FEATURE_AWSS_SUPPORT_PHONEASAP \
    FEATURE_AWSS_SUPPORT_ROUTER FEATURE_AWSS_SUPPORT_DEV_AP FEATURE_OTA_ENABLED FEATURE_SUPPORT_TLS

$(foreach v, \
    $(SWITCH_VARS), \
    $(if $(filter y,$($(v))), \
        $(eval GLOBAL_DEFINES += $(subst FEATURE_,,$(v)))) \
)

