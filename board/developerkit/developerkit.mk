NAME := developerkit


$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := stm32l4xx_cube
SUPPORT_BINS         := no
ENABLE_VFP           := 1
HOST_MCU_NAME        := STM32L496VGTx

$(NAME)_SOURCES += aos/board.c \
                   aos/board_cli.c \
                   aos/soc_init.c
                   
$(NAME)_SOURCES += Src/stm32l4xx_hal_msp.c 
                   
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

GLOBAL_DEFINES += STDIO_UART=6
GLOBAL_DEFINES += CONFIG_AOS_CLI_BOARD

#$(NAME)_COMPONENTS += sensor
#GLOBAL_DEFINES += AOS_SENSOR_ACC_MIR3_DA217
#GLOBAL_DEFINES += AOS_SENSOR_ALS_LITEON_LTR553
#GLOBAL_DEFINES += AOS_SENSOR_PS_LITEON_LTR553

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=board/developerkit/STM32L496.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config STM32L496.icf
else
GLOBAL_LDFLAGS += -T board/developerkit/STM32L496VGTx_FLASH.ld
endif

sal ?= 1
ifeq (1,$(sal))
$(NAME)_COMPONENTS += sal
module ?= wifi.mk3060
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
