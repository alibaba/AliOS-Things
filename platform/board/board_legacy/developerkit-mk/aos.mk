NAME := board_developerkit-mk


$(NAME)_MBINS_TYPE   := kernel
$(NAME)_VERSION      := 1.0.1
$(NAME)_SUMMARY      := Developer Kit is hardware development board base on AliOS-Things
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := mcu_stm32l4xx_cube
HOST_MCU_NAME        := STM32L496VGTx

SUPPORT_MBINS        := yes
MODULE               := 1062
ENABLE_VFP           := 1
ENABLE_USPACE        := 1
APP_FORMAT           := bin

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init cli

$(NAME)_SOURCES += aos/board.c
$(NAME)_SOURCES += aos/flash_partitions.c
$(NAME)_SOURCES += aos/board_cli.c
$(NAME)_SOURCES += aos/soc_init.c

$(NAME)_SOURCES += Src/adc.c
$(NAME)_SOURCES += Src/crc.c
$(NAME)_SOURCES += Src/dcmi.c
$(NAME)_SOURCES += Src/dma.c
$(NAME)_SOURCES += Src/i2c.c
$(NAME)_SOURCES += Src/irtim.c
$(NAME)_SOURCES += Src/main.c
$(NAME)_SOURCES += Src/sai.c
$(NAME)_SOURCES += Src/sdmmc.c
$(NAME)_SOURCES += Src/spi.c
$(NAME)_SOURCES += Src/stm32l4xx_hal_msp.c
$(NAME)_SOURCES += Src/tim.c
$(NAME)_SOURCES += Src/usart.c
$(NAME)_SOURCES += Src/usb_otg.c
$(NAME)_SOURCES += Src/gpio.c


ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += startup_stm32l496xx_keil.s
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += startup_stm32l496xx_iar.s
else
$(NAME)_SOURCES += startup_stm32l496xx.s
endif

GLOBAL_INCLUDES += .    \
                   hal/ \
                   aos/ \
                   Inc/

GLOBAL_CFLAGS += -DSTM32L496xx

GLOBAL_DEFINES += STDIO_UART=0
GLOBAL_DEFINES += CLI_CONFIG_SUPPORT_BOARD_CMD=1

$(NAME)_COMPONENTS += sensor
AOS_SENSOR_BARO_BOSCH_BMP280 = y
AOS_SENSOR_ALS_LITEON_LTR553 = y
AOS_SENSOR_PS_LITEON_LTR553 = y
AOS_SENSOR_HUMI_SENSIRION_SHTC1 = y
AOS_SENSOR_TEMP_SENSIRION_SHTC1 = y
AOS_SENSOR_ACC_ST_LSM6DSL = y
AOS_SENSOR_MAG_MEMSIC_MMC3680KJ = y
AOS_SENSOR_GYRO_ST_LSM6DSL = y

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter= platform/board/board_legacy/developerkit/STM32L496.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/board/board_legacy/developerkit/STM32L496.icf
else
#AOS_DEVELOPERKIT_ENABLE_OTA is used for ctl the  developerkit OTA function
#if AOS_DEVELOPERKIT_ENABLE_OTA := 1, it will enable OTA function
#if AOS_DEVELOPERKIT_ENABLE_OTA := 0, it will disable OTA function
#AOS_DEVELOPERKIT_ENABLE_OTA :=1

ifneq ($(otaapp_SUMMARY),)
AOS_DEVELOPERKIT_ENABLE_OTA := 1
endif

ifneq ($(linkkitapp_SUMMARY),)
AOS_DEVELOPERKIT_ENABLE_OTA := 1
endif

ifeq ($(AOS_DEVELOPERKIT_ENABLE_OTA),1)
GLOBAL_LDFLAGS += -T platform/board/board_legacy/developerkit/STM32L496VGTx_FLASH_OTA.ld
GLOBAL_DEFINES += VECT_TAB_OFFSET=0x9000
GLOBAL_DEFINES += USING_FLAT_FLASH
GLOBAL_DEFINES += OTA_DUBANK
AOS_SDK_2NDBOOT_SUPPORT := yes
else
ifeq ($(MBINS),)
GLOBAL_LDFLAGS += -T platform/board/board_legacy/developerkit-mk/STM32L496VGTx_FLASH.ld
else ifeq ($(MBINS),app)
GLOBAL_LDFLAGS += -T platform/board/board_legacy/developerkit-mk/$(MBINS_APP).ld
else
GLOBAL_LDFLAGS += -T platform/board/board_legacy/developerkit-mk/STM32L496VGTx_FLASH_kernel.ld
endif
endif

ifneq (y,$(strip $(BSP_SUPPORT_EXTERNAL_MODULE)))
GLOBAL_DEFINES += CONFIG_NO_TCPIP
endif

ifeq ($(COMPILER),armcc)
$(NAME)_LINK_FILES := startup_stm32l496xx_keil.o
$(NAME)_LINK_FILES += Src/stm32l4xx_hal_msp.o
GLOBAL_CFLAGS += -D__ORDER_LITTLE_ENDIAN__=1
GLOBAL_CFLAGS += -D__BYTE_ORDER__=__ORDER_LITTLE_ENDIAN__
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_developerkit
CONFIG_SYSINFO_DEVICE_NAME   := developerkit

# Enable/Disable Arduino SPI/I2C interface support, Disable as default
#arduino_io ?= 1
ifeq (1,$(arduino_io))
GLOBAL_DEFINES += ARDUINO_SPI_I2C_ENABLED
endif

GLOBAL_DEFINES += WORKAROUND_DEVELOPERBOARD_DMA_UART
endif

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -DSYSINFO_ARCH=\"$(HOST_ARCH)\"
GLOBAL_CFLAGS += -DSYSINFO_MCU=\"$(HOST_MCU_FAMILY)\"

# Keil project support
$(NAME)_KEIL_VENDOR = STMicroelectronics
$(NAME)_KEIL_DEVICE = STM32L496VGTx

# Iar project support: OGChipSelectEditMenu
$(NAME)_IAR_OGCMENU = STM32L475VG ST STM32L475VG

