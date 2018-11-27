NAME := bosch_xdk


$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := Cortex-M3
HOST_MCU_FAMILY      := efm32gg390
SUPPORT_BINS         := no
ENABLE_VFP           := 1
HOST_MCU_NAME        := efm32gg390f1024

$(NAME)_SOURCES += aos/board.c \
                   aos/soc_init.c \
                   aos/board_cli.c

$(NAME)_SOURCES += BSP/source/BSP_WiFi_cc3100mod.c \
				   BSP/source/BSP_ExtensionPort.c \
				   BSP/source/BSP_Board.c \
				   BSP/source/BSP_DMA.c \
				   BSP/source/BSP_Adc.c \
				   BSP/source/BSP_BT_EM9301.c \
				   BSP/source/BSP_SensorNode.c \
				   BSP/source/BSP_SDCard.c \
				   BSP/source/BSP_IrqHandler.c \
				   BSP/source/BSP_USB.c
				   
$(NAME)_SOURCES += common/source/adc/Mcu_Adc.c \


ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES +=
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES +=
else
$(NAME)_SOURCES += startup_efm32_S.S
endif

GLOBAL_INCLUDES += . \
                   aos/ \
                   inc/

GLOBAL_INCLUDES += BSP/source \
				   BSP/include

GLOBAL_INCLUDES += common/source/adc \
				   common/include
				   
GLOBAL_CFLAGS += -DEFM32GG390F1024

GLOBAL_DEFINES += STDIO_UART=1
GLOBAL_DEFINES += CONFIG_AOS_CLI_BOARD
GLOBAL_DEFINES += XDK_USB_PRINT

GLOBAL_DEFINES      += AOS_SENSOR_HUMI_BOSCH_BME280
GLOBAL_DEFINES      += AOS_SENSOR_BARO_BOSCH_BME280
GLOBAL_DEFINES      += AOS_SENSOR_TEMP_BOSCH_BME280
GLOBAL_DEFINES      += AOS_SENSOR_ACC_BOSCH_BMA280
GLOBAL_DEFINES      += AOS_SENSOR_GYRO_BOSCH_BMG160
GLOBAL_DEFINES      += AOS_SENSOR_MAG_BOSCH_BMM150
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_BOSCH_BMI160
#GLOBAL_DEFINES      += AOS_SENSOR_GYRO_BOSCH_BMI160

use_bootloader = 1

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS +=
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS +=
else
ifeq ($(use_bootloader),1)
GLOBAL_LDFLAGS += -T board/bosch_xdk/efm32gg390_withbootloader.ld
else
GLOBAL_LDFLAGS += -T board/bosch_xdk/efm32gg390_nobootloader.ld
endif
endif

$(NAME)_COMPONENTS += sal
module ?= wifi.cc3100

ifeq ($(COMPILER),armcc)
$(NAME)_LINK_FILES := startup_stm32l496xx_keil.o
$(NAME)_LINK_FILES += Src/stm32l4xx_hal_msp.o
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_bosch_xdk
CONFIG_SYSINFO_DEVICE_NAME := bosch_xdk

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
