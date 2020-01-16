NAME := board_starterkit

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board starterkit
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_stm32l4xx_cube
SUPPORT_MBINS      := no
ENABLE_VFP         := 1
HOST_MCU_NAME      := STM32L433CCTx

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

$(NAME)_SOURCES += aos/board.c     \
                   aos/board_cli.c \
                   aos/st7789.c    \
                   aos/soc_init.c

$(NAME)_SOURCES += Src/stm32l4xx_hal_msp.c \
                   Src/main.c

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += startup_stm32l433xx_keil.s
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += startup_stm32l433xx_iar.s
else
$(NAME)_SOURCES += startup_stm32l433xx.s
endif

GLOBAL_INCLUDES += .    \
                   hal/ \
                   aos/ \
                   Inc/

GLOBAL_CFLAGS += -DSTM32L433xx

GLOBAL_DEFINES += STDIO_UART=0
GLOBAL_DEFINES += CLI_CONFIG_SUPPORT_BOARD_CMD=1

$(NAME)_COMPONENTS += sensor
AOS_SENSOR_ACC_MIR3_DA217 = y
AOS_SENSOR_ALS_LITEON_LTR553 = y
AOS_SENSOR_PS_LITEON_LTR553 = y
AOS_SENSOR_ACC_SUPPORT_STEP = y
GLOBAL_DEFINES     += IOTX_WITHOUT_TLS MQTT_DIRECT

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/board/starterkit/STM32L433.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/board/starterkit/STM32L433.icf
else
GLOBAL_LDFLAGS += -T platform/board/starterkit/STM32L433RCTxP_FLASH.ld
endif

no_tls             ?= 1
ifneq (y,$(strip $(BSP_SUPPORT_EXTERNAL_MODULE)))
GLOBAL_DEFINES += CONFIG_NO_TCPIP
endif

ifeq (y, $(strip $(AOS_COMP_SAL)))
$(NAME)_COMPONENTS += sal
endif

ifeq ($(COMPILER),armcc)
$(NAME)_LINK_FILES := startup_stm32l433xx_keil.o
$(NAME)_LINK_FILES += Src/stm32l4xx_hal_msp.o
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_starterkit
CONFIG_SYSINFO_DEVICE_NAME   := starterkit

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

# Keil project support
$(NAME)_KEIL_VENDOR = STMicroelectronics
$(NAME)_KEIL_DEVICE = STM32L433RCTx

# Iar project support: OGChipSelectEditMenu
$(NAME)_IAR_OGCMENU = STM32L475VG ST STM32L475VG

