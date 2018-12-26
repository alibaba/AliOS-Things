NAME := stm32f429zi-nucleo


$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := stm32f4xx_cube
SUPPORT_BINS         := no
HOST_MCU_NAME        := STM32F429ZIT6
ENABLE_VFP           := 1

$(NAME)_SOURCES += aos/board_partition.c \
                   aos/soc_init.c
                   
$(NAME)_SOURCES += Src/stm32f4xx_hal_msp.c 


                   
ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += startup_stm32f429xx_keil.s    
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += startup_stm32f429xx_iar.s  
else
$(NAME)_SOURCES += startup_stm32f429xx.s
endif

GLOBAL_INCLUDES += . \
                   hal/ \
                   aos/ \
                   Inc/
				   
GLOBAL_CFLAGS += -DSTM32F429xx 



no_tls ?= 0
ifeq (1, $(no_tls))
GLOBAL_DEFINES += IOTX_WITHOUT_TLS MQTT_DIRECT
endif

sal ?= 1
ifeq (1,$(sal))
$(NAME)_COMPONENTS += sal
module ?= wifi.mk3060
else
GLOBAL_DEFINES += CONFIG_NO_TCPIP
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=board/stm32f429zi-nucleo/STM32F429.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config board/stm32f429zi-nucleo/STM32F429.icf
else
GLOBAL_LDFLAGS += -T board/stm32f429zi-nucleo/STM32F429ZITx_FLASH.ld
endif


CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_stm32f429-nucleo
CONFIG_SYSINFO_DEVICE_NAME := NUCLEO-F429ZI

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -DSYSINFO_ARCH=\"$(HOST_ARCH)\"
GLOBAL_CFLAGS += -DSYSINFO_MCU=\"STM32F429\"


ifeq ($(COMPILER),armcc)
$(NAME)_LINK_FILES := startup_stm32f429xx_keil.o
endif

