NAME := waveshareopen103c


$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := Cortex-M3
HOST_MCU_FAMILY      := stm32f1xx
SUPPORT_BINS         := no
HOST_MCU_NAME        := STM32F103CBTx
ENABLE_VFP           := 0

$(NAME)_SOURCES += aos/board_partition.c \
                   aos/soc_init.c
                   
$(NAME)_SOURCES += Src/stm32f1xx_hal_msp.c \
                   Src/main.c 

sal ?= 0
ifeq (1,$(sal))
$(NAME)_COMPONENTS += sal
module ?= gprs.sim800
else
GLOBAL_DEFINES += CONFIG_NO_TCPIP
endif



                   
ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += startup_stm32f103xb_keil.s    
$(NAME)_LINK_FILES := startup_stm32f103xb_keil.o
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += startup_stm32f103xb_iar.s  
else
$(NAME)_SOURCES += startup_stm32f103xb.s
endif

GLOBAL_INCLUDES += . \
                   aos/ \
                   Inc/
                   
GLOBAL_CFLAGS += -DSTM32F103xB -DCENTRALIZE_MAPPING



ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=board/waveshareopen103c/stm32f103xb_flash.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config board/waveshareopen103c/stm32f103xb_flash.icf
else
GLOBAL_LDFLAGS += -T board/waveshareopen103c/STM32F103CBTx_FLASH.ld
endif



CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_lierda
CONFIG_SYSINFO_DEVICE_NAME := waveshareopen103c

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
