NAME := uDataapp
$(NAME)_SOURCES :=   uData-example.c


ifeq ($(dtc),1)

ifeq ($(COMPILER),iar)
$(NAME)_COMPONENTS += feature.linkkit-noawss
else
$(NAME)_COMPONENTS += feature.linkkit
endif    


$(NAME)_COMPONENTS += network/netmgr \
                      middleware/common \
                      middleware/uagent/uota  \
                      utility/cjson   \
                      device/sensor \
                      middleware/udata					  
                                        
$(NAME)_INCLUDES += \
    ../../../middleware/uagent/uota/src/service

ifeq ($(case),sched)
$(NAME)_SOURCES += linkkit_example_sched.c 
GLOBAL_DEFINES += DEPRECATED_LINKKIT 
else ifeq ($(case),cntdown)
ifneq ($(newapi),)
$(NAME)_SOURCES += newapi/cntdown.c 
else
$(NAME)_SOURCES += linkkit_example_cntdown.c 
GLOBAL_DEFINES += DEPRECATED_LINKKIT 
endif
else
ifneq ($(newapi),)
$(NAME)_SOURCES += newapi/solo.c 
else
$(NAME)_SOURCES += linkkit_example_solo.c 
GLOBAL_DEFINES += DEPRECATED_LINKKIT 
endif
endif

GLOBAL_DEFINES += CONFIG_DM_DEVTYPE_SINGLE

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
GLOBAL_DEFINES += FOTA_RAM_LIMIT_MODE
GLOBAL_DEFINES += ESP8266_CHIPSET
endif

GLOBAL_INCLUDES += ./
GLOBAL_INCLUDES += ../../../device/sensor/include
GLOBAL_DEFINES  += DATA_TO_CLOUD

else

$(NAME)_COMPONENTS := cli network.netmgr middleware.common device.sensor middleware.udata
ifeq ($(CONFIG_SYSINFO_DEVICE_NAME),developerkit)
$(NAME)_COMPONENTS += 3rdparty.experimental.gui.littlevGL
$(NAME)_SOURCES    += house.c
$(NAME)_SOURCES    += weather.c
$(NAME)_SOURCES    += AliOS_Things_logo.c
$(NAME)_SOURCES    += sensor_display.c
#GLOBAL_DEFINES += LITTLEVGL_DISPLAY
GLOBAL_DEFINES += BONE_ENGINE_NOT_NEED
endif



GLOBAL_DEFINES += LITTLEVGL_DEVELOPERKIT

GLOBAL_INCLUDES += ./
GLOBAL_INCLUDES += ../../../device/sensor/include

endif
