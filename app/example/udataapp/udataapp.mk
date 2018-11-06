NAME := udataapp
$(NAME)_SOURCES :=   udata_example.c

$(NAME)_COMPONENTS := cli yloop middleware.common device.sensor middleware.udata
#GLOBAL_DEFINES += LITTLEVGL_DEVELOPERKIT
GLOBAL_INCLUDES += ./
GLOBAL_INCLUDES += ../../../device/sensor/include

udata_cjson_support := 1
ifeq ($(udata_cjson_support),1)
GLOBAL_DEFINES += UDATA_CJSON_SUPPORTED
$(NAME)_COMPONENTS += utility.cjson
endif


ifeq ($(dtc),linkkit)

$(NAME)_SOURCES +=   linkkit/app_entry.c

$(NAME)_COMPONENTS += feature.linkkit

$(NAME)_COMPONENTS += network/netmgr \
                      middleware/common \
                      utility/cjson

##ifeq ($(COMPILER),iar)
##$(NAME)_COMPONENTS += feature.linkkit-nouota
##else
$(NAME)_COMPONENTS += middleware/uagent/uota
##endif

ifeq ($(case),sched)
ifneq ($(deprecated),)
$(NAME)_SOURCES += linkkit/deprecated/sched.c
GLOBAL_DEFINES += DEPRECATED_LINKKIT
else
$(NAME)_SOURCES += linkkit/linkkit_example_sched.c
endif
else ifeq ($(case),cntdown)
ifneq ($(deprecated),)
$(NAME)_SOURCES += linkkit/deprecated/cntdown.c
GLOBAL_DEFINES += DEPRECATED_LINKKIT
else
$(NAME)_SOURCES += linkkit/linkkit_example_cntdown.c
endif
else
ifneq ($(deprecated),)
$(NAME)_SOURCES += linkkit/deprecated/solo.c
GLOBAL_DEFINES += DEPRECATED_LINKKIT
else
$(NAME)_SOURCES += linkkit/linkkit_example_solo.c
endif
endif

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif

ifeq ($(print_heap),1)
$(NAME)_DEFINES += CONFIG_PRINT_HEAP
endif

ifneq ($(HOST_MCU_FAMILY),esp8266)
$(NAME)_COMPONENTS  += cli
GLOBAL_DEFINES += CONFIG_AOS_CLI
else
GLOBAL_DEFINES += ESP8266_CHIPSET
endif

GLOBAL_INCLUDES += ./
GLOBAL_DEFINES += DTC_LINKKIT

else ifeq ($(dtc),mqtt)

$(NAME)_SOURCES    += mqtt/mqtt_example.c

$(NAME)_COMPONENTS += sensor middleware.uagent.uota netmgr feature.linkkit-mqtt  utility.cjson
GLOBAL_DEFINES     += CONFIG_AOS_CLI USE_LPTHREAD

GLOBAL_DEFINES     += DTC_MQTT
endif
