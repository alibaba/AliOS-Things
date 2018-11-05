NAME := device_sal_cc3100
WIFILIB_PATH = 3rd-party/TI/simplelink
$(NAME)_SOURCES += \
	$(WIFILIB_PATH)/source/device.c \
	$(WIFILIB_PATH)/source/driver.c \
	$(WIFILIB_PATH)/source/flowcont.c \
	$(WIFILIB_PATH)/source/fs.c \
	$(WIFILIB_PATH)/source/netapp.c \
	$(WIFILIB_PATH)/source/netcfg.c \
	$(WIFILIB_PATH)/source/nonos.c \
	$(WIFILIB_PATH)/source/socket.c \
	$(WIFILIB_PATH)/source/spawn.c \
	$(WIFILIB_PATH)/source/wlan.c \
	WlanConnect/WlanConnect.c \
	WlanDriver/WlanDriver.c \
	NetworkConfig/NetworkConfig.c \
	cc3100.c \
	wifi_atcmd_cc3100.c

	
GLOBAL_DEFINES += DEV_SAL_CC3100

$(NAME)_COMPONENTS += yloop



GLOBAL_INCLUDES += ./
GLOBAL_INCLUDES += ./include
GLOBAL_INCLUDES += $(WIFILIB_PATH)/include
#GLOBAL_INCLUDES += ./3rd-party/mcu_vendor/ti/SimpleLink_CC32xx/v1_40_01_00/source/ti/drivers/net/wifi
#GLOBAL_INCLUDES += ./3rd-party/mcu_vendor/ti/SimpleLink_CC32xx/v1_40_01_00/source

