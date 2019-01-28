NAME := mqttapp
$(NAME)_SOURCES := app_entry.c 

$(NAME)_COMPONENTS += framework/protocol/linkkit/sdk \
    				framework/protocol/linkkit/hal \
					framework/netmgr \
					framework/common \
					utility/cjson \
					tools/cli  

GLOBAL_CFLAGS +=  -DMQTT_DIRECT   

GLOBAL_DEFINES += CONFIG_AOS_CLI

ifeq ($(case),rrpc)
$(NAME)_SOURCES += mqtt_example_rrpc.c
else ifeq ($(case),multithread)
$(NAME)_SOURCES += mqtt_example_multithread.c
else ifeq ($(case),presstest)
$(NAME)_SOURCES += mqtt_presstest.c
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
