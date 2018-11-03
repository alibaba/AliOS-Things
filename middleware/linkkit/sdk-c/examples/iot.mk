DEPENDS             := src/ref-impl/hal
DEPENDS             += src/ref-impl/tls

HDR_REFS            += src/infra
HDR_REFS            += src/services

LDFLAGS             := -Bstatic
LDFLAGS             += -liot_sdk

LDFLAGS             += -liot_hal
CFLAGS              := $(filter-out -ansi,$(CFLAGS))
LDFLAGS             += -liot_tls
ifneq (,$(filter -D_PLATFORM_IS_WINDOWS_,$(CFLAGS)))
LDFLAGS             += -lws2_32
CFLAGS              := $(filter-out -DCOAP_COMM_ENABLED,$(CFLAGS))
endif

ifneq (,$(filter -DMQTT_COMM_ENABLED,$(CFLAGS)))
TARGET                          += mqtt-example mqtt-example-rrpc mqtt-example-multithread
SRCS_mqtt-example               := app_entry.c mqtt/mqtt_example.c
SRCS_mqtt-example-rrpc          := app_entry.c mqtt/mqtt_example_rrpc.c
SRCS_mqtt-example-multithread   := app_entry.c mqtt/mqtt_example_multithread.c

    ifneq (,$(filter -DSUPPORT_ITLS,$(CFLAGS)))
    LDFLAGS += \
        -litls \
        -lid2client \
        -lkm \
        -lplat_gen \
        -lalicrypto \
        -lmbedcrypto
    endif

endif

ifneq (,$(filter -DCOAP_COMM_ENABLED,$(CFLAGS)))
TARGET              += coap-example
SRCS_coap-example   := coap/coap_example.c app_entry.c
endif


ifneq (,$(filter -DHTTP_COMM_ENABLED,$(CFLAGS)))
TARGET              += http-example
SRCS_http-example   := http/http_example.c app_entry.c
endif

ifneq (,$(filter -DHTTP2_COMM_ENABLED,$(CFLAGS)))
TARGET              += http2-example 
SRCS_http2-example   := http2/http2_example_stream.c app_entry.c
    ifneq (,$(filter -DFS_ENABLED,$(CFLAGS)))
    TARGET              += http2_uploadfile
    SRCS_http2_uploadfile   := http2/http2_example_uploadfile.c app_entry.c

    endif
endif

ifneq (,$(filter -DOTA_ENABLED,$(CFLAGS)))

    ifneq (,$(filter -DMQTT_COMM_ENABLED,$(CFLAGS)))
    TARGET                  += ota_mqtt-example
    SRCS_ota_mqtt-example   := ota/ota_mqtt-example.c
    endif

endif

ifneq (,$(filter -DDEVICE_MODEL_ENABLED,$(CFLAGS)))
    ifneq (,$(filter -DOTA_ENABLED,$(CFLAGS)))
    ifeq (,$(filter -DDEPRECATED_LINKKIT,$(CFLAGS)))
        TARGET      += active-cota
        SRCS_active-cota:= app_entry.c ota/active_cota.c
    endif
    endif

    ifeq (,$(filter -DDEVICE_MODEL_GATEWAY,$(CFLAGS)))
        TARGET      += linkkit-example-solo linkkit-example-countdown

        ifneq (Darwin,$(shell uname))
        TARGET      += linkkit-example-sched
        endif

        SRCS_linkkit-example-solo       := app_entry.c cJSON.c linkkit/linkkit_example_solo.c
        SRCS_linkkit-example-countdown  := app_entry.c cJSON.c linkkit/linkkit_example_cntdown.c
        SRCS_linkkit-example-sched      := app_entry.c cJSON.c linkkit/linkkit_example_sched.c

    else
    
        TARGET      += linkkit-example-gw
        SRCS_linkkit-example-gw         := app_entry.c linkkit/linkkit_example_gateway.c cJSON.c
    endif 

endif

ifneq (,$(filter -D_PLATFORM_IS_WINDOWS_,$(CFLAGS)))
    TARGET          := mqtt-example
    SRCS            := mqtt/mqtt_example.c
endif
