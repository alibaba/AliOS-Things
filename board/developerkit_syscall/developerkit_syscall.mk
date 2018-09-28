NAME := developerkit_syscall


$(NAME)_MBINS_TYPE   := kernel
SUPPORT_MBINS 	     := yes
MODULE               := 1062
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := stm32l4xx_syscall
ENABLE_VFP           := 1
HOST_MCU_NAME        := STM32L496VGTx
ENABLE_USPACE        := 1

$(NAME)_SOURCES += src/soc_init.c \
                   src/startup_stm32l496xx.s \
                   src/main.c \
                   src/usart.c \
                   src/uspace_init.c

GLOBAL_INCLUDES += include

GLOBAL_CFLAGS += -DSTM32L496xx


ifeq ($(MBINS),)
GLOBAL_LDFLAGS += -T board/developerkit/STM32L496VGTx_FLASH.ld
else ifeq ($(MBINS),app)
GLOBAL_LDFLAGS += -T board/developerkit_syscall/STM32L496VGTx_FLASH_app.ld
else ifeq ($(MBINS),kernel)
GLOBAL_LDFLAGS += -T board/developerkit_syscall/STM32L496VGTx_FLASH_kernel.ld
endif

$(NAME)_COMPONENTS += sal

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_developerkit
CONFIG_SYSINFO_DEVICE_NAME := developerkit_syscall

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
