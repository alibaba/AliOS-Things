NAME := board_developerkit_syscall


$(NAME)_MBINS_TYPE   := kernel
$(NAME)_VERSION      := 0.0.1
$(NAME)_SUMMARY      := configuration for board developerkit_syscall
SUPPORT_MBINS        := yes
MODULE               := 1062
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := stm32l4xx_syscall
ENABLE_VFP           := 1
HOST_MCU_NAME        := STM32L496VGTx
ENABLE_USPACE        := 1

$(NAME)_SOURCES += src/soc_init.c            \
                   src/startup_stm32l496xx.s \
                   src/main.c                \
                   src/usart.c               \
                   src/uspace_init.c

GLOBAL_INCLUDES += include

GLOBAL_CFLAGS += -DSTM32L496xx

ifeq ($(MBINS),)
GLOBAL_LDFLAGS += -T board/developerkit_syscall/STM32L496VGTx_FLASH.ld
else ifeq ($(MBINS),app)
ifneq ($(MBINS_APP),)
GLOBAL_LDFLAGS += -T board/developerkit_syscall/$(MBINS_APP).ld
else
GLOBAL_LDFLAGS += -T board/developerkit_syscall/STM32L496VGTx_FLASH_app.ld
endif
else ifeq ($(MBINS),kernel)
GLOBAL_LDFLAGS += -T board/developerkit_syscall/STM32L496VGTx_FLASH_kernel.ld
endif

GLOBAL_DEFINES += CONFIG_NO_TCPIP

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_developerkit
CONFIG_SYSINFO_DEVICE_NAME   := developerkit_syscall

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
