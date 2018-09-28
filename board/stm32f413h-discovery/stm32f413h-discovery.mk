NAME := stm32f413h-discovery


$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := stm32f4xx_cube
SUPPORT_BINS         := no
HOST_MCU_NAME        := STM32F413ZHT6
ENABLE_VFP           := 1

$(NAME)_SOURCES += aos/board_partition.c \
                   aos/soc_init.c
                   
$(NAME)_SOURCES += Src/stm32f4xx_hal_msp.c \
                   Src/pulse_skipper.c \
                   Src/main.c

$(NAME)_SOURCES += drv/stm32f413h_discovery.c

sound_enable ?= 0
ifeq ($(sound_enable),1)
$(NAME)_PREBUILT_LIBRARY := drv/audio/lib/libAcoustic_SL_CM4F_GCC_ot.a
$(NAME)_SOURCES += drv/audio/wm8994/wm8994.c \
                   drv/audio/st_acoustic_sl.c
GLOBAL_DEFINES += SOUND_LOCATION_ENABLE
endif

display_enable ?= 1
ifeq ($(display_enable),1)
$(NAME)_SOURCES += drv/display/st7789h2/st7789h2.c \
                   drv/display/stm32f413h_discovery_lcd.c \
                   drv/display/ft6x06/ft6x06.c \
                   drv/display/stm32f413h_discovery_ts.c
endif

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += startup_stm32f413xx_keil.s    
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += startup_stm32f413xx_iar.s  
else
$(NAME)_SOURCES += startup_stm32f413xx.s
endif

GLOBAL_INCLUDES += . \
                   hal/ \
                   aos/ \
                   Inc/ \
                   drv/display/
				   
GLOBAL_CFLAGS += -DSTM32F413xx

GLOBAL_DEFINES += STDIO_UART=0

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=board/stm32f413h-discovery/STM32F413ZHTx_FLASH.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config STM32L433.icf
else
GLOBAL_LDFLAGS += -T board/stm32f413h-discovery/STM32F413ZHTx_FLASH.ld
endif


sal ?= 0
ifeq (1,$(sal))
$(NAME)_COMPONENTS += sal
module ?= wifi.mk3060
else
GLOBAL_DEFINES += CONFIG_NO_TCPIP
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_f413-discovery
CONFIG_SYSINFO_DEVICE_NAME := f413-discovery

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
