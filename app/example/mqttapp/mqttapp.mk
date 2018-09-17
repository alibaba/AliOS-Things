NAME := mqttapp
$(NAME)_SOURCES := app_entry.c \

$(NAME)_COMPONENTS += feature.linkkit-mqtt \
                      network/netmgr \
                      middleware/common \
                      utility/cjson \
                      tools/cli


GLOBAL_DEFINES += CONFIG_AOS_CLI USE_LPTHREAD

ifeq ($(case),rrpc)
$(NAME)_SOURCES += mqtt_example_rrpc.c 
else ifeq ($(case),multithread)
$(NAME)_SOURCES += mqtt_example_multithread.c 
else
$(NAME)_SOURCES += mqtt_example.c 
endif


ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif

ifneq ($(loop),0)
$(NAME)_DEFINES      += TEST_LOOP
endif

GLOBAL_INCLUDES += ./