NAME := board_m100c

JTAG := jlink_swd

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := configuration for board m100c

MODULE          := 1062
HOST_ARCH       := Cortex-M3
HOST_MCU_FAMILY := efm32gxx
SUPPORT_MBINS   := no

lorawanback      ?= 0
ifeq ($(lorawanback), 0)
$(NAME)_COMPONENTS += network.lorawan.lorawan_4_4_2.lorachip network.lorawan.lorawan_4_4_2
else ifeq ($(lorawanback), 1)
$(NAME)_COMPONENTS += network.lorawan.lorawan_4_4_0.lorachip network.lorawan.lorawan_4_4_0
endif

LORACHIP           := sx1276

GLOBAL_DEFINES += RHINO_CONFIG_TICK_TASK=0 RHINO_CONFIG_WORKQUEUE=0 RHINO_CONFIG_NORMAL_PRT=0
GLOBAL_DEFINES += EFM32G222F128

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_M100C
CONFIG_SYSINFO_DEVICE_NAME   := M100C

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

GLOBAL_LDFLAGS += -L $(SOURCE_ROOT)/board/m100c

GLOBAL_INCLUDES += .

ifeq ($(lorawanback), 0)
GLOBAL_INCLUDES += ../../network/lorawan/lorawan_4_4_2/lorachip/sx1276 \
                   ../../network/lorawan/lorawan_4_4_2/lora/system     \
                   ../../network/lorawan/lorawan_4_4_2/lora/mac        \
                   ../../network/lorawan/lorawan_4_4_2/lora/radio      \
                   ../../network/lorawan/lorawan_4_4_2/linkwan/include
else ifeq ($(lorawanback), 1)
GLOBAL_INCLUDES += ../../network/lorawan/lorawan_4_4_0/lorachip/sx1276 \
                   ../../network/lorawan/lorawan_4_4_0/lora/system     \
                   ../../network/lorawan/lorawan_4_4_0/lora/mac        \
                   ../../network/lorawan/lorawan_4_4_0/lora/radio      \
                   ../../network/lorawan/lorawan_4_4_0/linkwan/include
endif

$(NAME)_SOURCES := board.c
$(NAME)_SOURCES += hw.c
$(NAME)_SOURCES += debug.c
$(NAME)_SOURCES += hw_spi.c
$(NAME)_SOURCES += hw_rtc.c
$(NAME)_SOURCES += lorawan_port.c
$(NAME)_SOURCES += board_sx1278.c
