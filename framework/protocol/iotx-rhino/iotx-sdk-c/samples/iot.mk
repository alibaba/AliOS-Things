DEPENDS             := src/platform
HDR_REFS            += src

LDFLAGS             := -Bstatic
LDFLAGS             += -liot_sdk
LDFLAGS             += -liot_platform
CFLAGS              := $(filter-out -ansi,$(CFLAGS))
ifeq (,$(filter -DIOTX_WITHOUT_TLS,$(CFLAGS)))
LDFLAGS     += -liot_tls
endif
ifneq (,$(filter -D_PLATFORM_IS_WINDOWS_,$(CFLAGS)))
LDFLAGS             += -lws2_32
CFLAGS              := $(filter-out -DCOAP_COMM_ENABLED,$(CFLAGS))
endif

ifneq (,$(filter -DMQTT_COMM_ENABLED,$(CFLAGS)))
TARGET              += mqtt-example mqtt_rrpc-example mqtt_multi_thread-example
SRCS_mqtt-example   := mqtt/mqtt-example.c
SRCS_mqtt_rrpc-example := mqtt/mqtt_rrpc-example.c
SRCS_mqtt_multi_thread-example := mqtt/mqtt_multi_thread-example.c

    ifneq (,$(filter -DOTA_ENABLED,$(CFLAGS)))
    ifneq (,$(filter -DOTA_SIGNAL_CHANNEL=1,$(CFLAGS)))
    TARGET                += ota_mqtt-example
    SRCS_ota_mqtt-example := ota/ota_mqtt-example.c
    endif
    endif

    ifneq (,$(filter -DMQTT_SHADOW,$(CFLAGS)))
    TARGET              += shadow-example
    SRCS_shadow-example := device-shadow/shadow-example.c
    endif
    
    ifneq (,$(filter -DMQTT_ID2_AUTH,$(CFLAGS)))
    ifneq (,$(filter -DON_DAILY,$(CFLAGS)))
    LDFLAGS     += -ltfs
    else
    LDFLAGS     += -ltfs_online
    endif
    ifeq (,$(filter -DIOTX_WITHOUT_ITLS,$(CFLAGS)))
    LDFLAGS     += -litls -liot_tfs -liot_sdk
    endif
    endif

endif

ifneq (,$(filter -DCOAP_COMM_ENABLED,$(CFLAGS)))
TARGET              += coap-example

SRCS_coap-example   := coap/coap-example.c
SRCS                += coap/coap-example.c

    ifneq (,$(filter -DOTA_ENABLED,$(CFLAGS)))
    ifneq (,$(filter -DOTA_SIGNAL_CHANNEL=2,$(CFLAGS)))
    TARGET                += ota_coap-example
    SRCS_ota_coap-example := ota/ota_coap-example.c
    endif
    endif
endif

ifneq (,$(filter -DHTTP_COMM_ENABLED,$(CFLAGS)))
TARGET              += http-example
SRCS_http-example   := http/http-example.c
endif

ifneq (,$(filter -DHTTP2_COMM_ENABLED,$(CFLAGS)))
TARGET              += http2-example
SRCS_http2-example  := http2/http2-example.c
endif

ifneq (,$(filter -DSUBDEVICE_ENABLED,$(CFLAGS)))
TARGET                += subdev-example
SRCS_subdev-example   += subdev/subdev-example.c \
                         subdev/subdev_example_api.c
endif

ifneq (,$(filter -DCM_ENABLED,$(CFLAGS)))   
TARGET                    += cm-example 
SRCS_cm-example          := cmp/cmp-example.c
endif


ifneq (,$(filter -DDM_ENABLED,$(CFLAGS)))   

#    ifeq (,$(filter -DSUBDEV_ENABLE,$(CFLAGS)))
	ifneq (,$(filter -DCONFIG_DM_DEVTYPE_SINGLE,$(CFLAGS)))
    TARGET                           += linkkit-example_single
    SRCS_linkkit-example_single      := linkkit/linkkit_sample_single.c
	endif
	ifneq (,$(filter -DCONFIG_DM_DEVTYPE_GATEWAY,$(CFLAGS)))
    TARGET                  += linkkit-example_gw
    SRCS_linkkit-example_gw += linkkit/linkkit_sample_gateway.c \
                              linkkit/light.c \
                              linkkit/cJSON.c
	endif
#    endif

endif
