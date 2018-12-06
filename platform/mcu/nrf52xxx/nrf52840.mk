HOST_OPENOCD := pca10056

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192
GLOBAL_DEFINES += CONFIG_ARM
$(NAME)_LIBSUFFIX := -52840

GLOBAL_INCLUDES += \
                   src/pca10056/base_pro \
                   Drivers/softdevice/s140/headers/nrf52 \
                   Drivers/softdevice/s140/headers

$(NAME)_SOURCES += src/pca10056/base_pro/soc_init.c \
                   Drivers/toolchain/system_nrf52840.c

ifeq ($(ble),1)
$(NAME)_SOURCES  += hal/ais_ota_port.c
endif

$(NAME)_SOURCES  += hal/misc.c \
                    hal/gpio.c

GLOBAL_CFLAGS += -DNRF52840_XXAA -DBOARD_PCA10056
                   
ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += Drivers/toolchain/arm/arm_startup_nrf52840.s
     
else ifeq ($(COMPILER), iar)

else
$(NAME)_SOURCES += Drivers/toolchain/gcc/gcc_startup_nrf52840.S

endif
     
ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/mcu/nrf52xxx/Drivers/toolchain/arm/
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/mcu/stm32l4xx/src/STM32L433RC-Nucleo/STM32L433.icf
else

ifeq ($(MBINS),)
GLOBAL_LDFLAGS += -T platform/mcu/nrf52xxx/nrf52_common.ld
else ifeq ($(MBINS),app)
GLOBAL_LDFLAGS += -T platform/mcu/nrf52xxx/nrf52_common_app.ld
else ifeq ($(MBINS),kernel)
GLOBAL_LDFLAGS += -T platform/mcu/nrf52xxx/nrf52_common_kernel.ld
endif

endif

GLOBAL_CFLAGS += -DNRF_DFU_SETTINGS_VERSION=0

GLOBAL_DEFINES += NRF52840_XXAA
