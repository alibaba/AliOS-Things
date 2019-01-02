NAME := developerkit


$(NAME)_MBINS_TYPE   := kernel
SUPPORT_MBINS 	     := yes
MODULE               := 1062
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := stm32l4xx_cube
ENABLE_VFP           := 1
HOST_MCU_NAME        := STM32L496VGTx
ENABLE_USPACE        := 0

$(NAME)_SOURCES += aos/board.c \
                   aos/board_cli.c \
                   aos/soc_init.c \
                   aos/st7789.c \
                   pwrmgmt_hal/board_cpu_pwr.c \
                   pwrmgmt_hal/board_cpu_pwr_rtc.c \
                   pwrmgmt_hal/board_cpu_pwr_systick.c \
                   pwrmgmt_hal/board_cpu_pwr_timer.c \
                   mbmaster_hal/port_serial.c
                   
$(NAME)_SOURCES += Src/adc.c \
$(NAME)_SOURCES += Src/crc.c \
$(NAME)_SOURCES += Src/dcmi.c \
$(NAME)_SOURCES += Src/dma.c \
$(NAME)_SOURCES += Src/i2c.c \
$(NAME)_SOURCES += Src/irtim.c \
$(NAME)_SOURCES += Src/main.c \
$(NAME)_SOURCES += Src/sai.c \
$(NAME)_SOURCES += Src/sdmmc.c \
$(NAME)_SOURCES += Src/spi.c \
$(NAME)_SOURCES += Src/stm32l4xx_hal_msp.c \
$(NAME)_SOURCES += Src/tim.c \
$(NAME)_SOURCES += Src/usart.c \
$(NAME)_SOURCES += Src/usb_otg.c \
$(NAME)_SOURCES += Src/gpio.c \

                   
ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += startup_stm32l496xx_keil.s    
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += startup_stm32l496xx_iar.s  
else
$(NAME)_SOURCES += startup_stm32l496xx.s
endif

GLOBAL_INCLUDES += . \
                   hal/ \
                   aos/ \
                   Inc/
				   
GLOBAL_CFLAGS += -DSTM32L496xx 

GLOBAL_DEFINES += STDIO_UART=0
GLOBAL_DEFINES += CONFIG_AOS_CLI_BOARD

$(NAME)_COMPONENTS += sensor
GLOBAL_DEFINES += AOS_SENSOR_BARO_BOSCH_BMP280
GLOBAL_DEFINES += AOS_SENSOR_ALS_LITEON_LTR553
GLOBAL_DEFINES += AOS_SENSOR_PS_LITEON_LTR553
GLOBAL_DEFINES += AOS_SENSOR_HUMI_SENSIRION_SHTC1
GLOBAL_DEFINES += AOS_SENSOR_TEMP_SENSIRION_SHTC1
GLOBAL_DEFINES += AOS_SENSOR_ACC_ST_LSM6DSL
GLOBAL_DEFINES += AOS_SENSOR_MAG_MEMSIC_MMC3680KJ
GLOBAL_DEFINES += AOS_SENSOR_GYRO_ST_LSM6DSL

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=board/developerkit/STM32L496.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config board/developerkit/STM32L496.icf
else
ifeq ($(post_run),1)
GLOBAL_LDFLAGS += -T board/developerkit/STM32L496VGTx_FLASH.ld
GLOBAL_DEFINES += VECT_TAB_OFFSET=0x4000
GLOBAL_DEFINES += USING_FLAT_FLASH
else
ifeq ($(MBINS),)
GLOBAL_LDFLAGS += -T board/developerkit/STM32L496VGTx_FLASH.ld
else ifeq ($(MBINS),app)
GLOBAL_LDFLAGS += -T board/developerkit/STM32L496VGTx_FLASH_app.ld
else ifeq ($(MBINS),kernel)
GLOBAL_LDFLAGS += -T board/developerkit/STM32L496VGTx_FLASH_kernel.ld
endif
endif
endif

sal ?= 1
ifeq (1,$(sal))
$(NAME)_COMPONENTS += sal
module ?= wifi.bk7231
else
GLOBAL_DEFINES += CONFIG_NO_TCPIP
endif

ifeq ($(COMPILER),armcc)
$(NAME)_LINK_FILES := startup_stm32l496xx_keil.o
$(NAME)_LINK_FILES += Src/stm32l4xx_hal_msp.o
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_developerkit
CONFIG_SYSINFO_DEVICE_NAME := developerkit

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
