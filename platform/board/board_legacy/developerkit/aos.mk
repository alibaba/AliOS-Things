NAME := board_developerkit

$(NAME)_MBINS_TYPE   := kernel
$(NAME)_VERSION      := 1.0.2
$(NAME)_SUMMARY      := Developer Kit is hardware development board base on AliOS-Things
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := mcu_stm32l4xx_cube
HOST_MCU_NAME        := STM32L496VGTx

ifeq ($(AOS_2NDBOOT_SUPPORT), yes)
$(NAME)_LIBSUFFIX := _2ndboot

HOST_OPENOCD := stm32l4xx
GLOBAL_DEFINES += USE_HAL_DRIVER
GLOBAL_DEFINES += STM32L496xx
GLOBAL_DEFINES += AOS_OTA_BANK_DUAL
GLOBAL_DEFINES += VECT_TAB_OFFSET=0x1800
GLOBAL_DEFINES += USING_FLAT_FLASH
GLOBAL_CFLAGS += -DAOS_OTA_RECOVERY_TYPE=2

GLOBAL_INCLUDES := bootloader           \
                   bootloader/Src       \
                   bootloader/Drivers/STM32L4xx_HAL_Driver/Inc \
                   bootloader/Drivers/CMSIS

$(NAME)_SOURCES := bootloader/startup_stm32l496xx_boot.s  \
                   bootloader/main.c                      \
                   bootloader/Src/system_stm32l4xx.c      \
                   bootloader/Src/stm32l4xx_it.c          \
                   bootloader/Src/hal_boot_flash.c        \
                   bootloader/Src/hal_boot_process.c      \
                   bootloader/Src/hal_boot_uart.c         \
                   bootloader/Src/hal_boot_wdg.c          \
                   bootloader/Src/hal_boot_gpio.c         \
                   bootloader/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c        \
                   bootloader/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c \
                   bootloader/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c    \
                   bootloader/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c \
                   aos/board.c                                                        \
                   aos/flash_partitions.c

GLOBAL_LDFLAGS += -T ./platform/board/board_legacy/developerkit/bootloader/STM32L496VGTx_FLASH_bootloader.ld

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_developerkit_2ndboot
CONFIG_SYSINFO_DEVICE_NAME := developerkit_2ndboot

EXTRA_TARGET_MAKEFILES +=  ./platform/board/board_legacy/developerkit/gen_image_bin.mk

else

SUPPORT_MBINS        := yes
MODULE               := 1062
ENABLE_VFP           := 1
ENABLE_USPACE        := 0

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

$(NAME)_SOURCES += aos/board.c                         \
                   aos/flash_partitions.c              \
                   aos/board_cli.c                     \
                   aos/soc_init.c                      \
                   aos/st7789.c                        \
                   mbmaster_hal/port_serial.c

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

ifeq ($(ENABLE_IRDA_HAL),1)
include ./platform/board/board_legacy/developerkit/irda_hal/irda_hal.mk
endif

ifeq ($(ENABLE_CAMERA_HAL),1)
include ./platform/board/board_legacy/developerkit/camera_hal/camera_hal.mk
endif

ifeq ($(ENABLE_AUDIO_HAL),1)
include ./platform/board/board_legacy/developerkit/audio_hal/audio_hal.mk
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
GLOBAL_LDFLAGS += -L --scatter=platform/board/board_legacy/developerkit/STM32L496.sct
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
GLOBAL_LDFLAGS += -T platform/board/board_legacy/developerkit/STM32L496VGTx_FLASH.ld
else ifeq ($(MBINS),app)
ifneq ($(MBINS_APP),)
GLOBAL_LDFLAGS += -T platform/board/board_legacy/developerkit/$(MBINS_APP).ld
else
GLOBAL_LDFLAGS += -T platform/board/board_legacy/developerkit/STM32L496VGTx_FLASH_app.ld
endif
else ifeq ($(MBINS),kernel)
GLOBAL_LDFLAGS += -T platform/board/board_legacy/developerkit/STM32L496VGTx_FLASH_kernel.ld
endif
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

