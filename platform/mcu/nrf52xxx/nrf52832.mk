HOST_OPENOCD := pca10040

GLOBAL_DEFINES += CONFIG_ARM
$(NAME)_LIBSUFFIX := -52832

GLOBAL_INCLUDES += \
                    src/pca10040/base_pro \
                    Drivers/softdevice/s132/headers \
                    Drivers/softdevice/s132/headers/nrf52

$(NAME)_SOURCES += src/pca10040/base_pro/soc_init.c \
                    Drivers/toolchain/system_nrf52.c \
                    Drivers/drivers_nrf/power/nrf_drv_power.c \
                    Drivers/drivers_nrf/rtc/nrf_drv_rtc.c

ifeq ($(ble),1)
$(NAME)_SOURCES  += hal/ais_ota_port.c
endif

$(NAME)_SOURCES  += hal/misc.c
$(NAME)_SOURCES  += hal/pwrmgmt_hal/board_cpu_pwr_rtc.c
$(NAME)_SOURCES  += hal/pwrmgmt_hal/board_cpu_pwr_systick.c
$(NAME)_SOURCES  += hal/pwrmgmt_hal/board_cpu_pwr.c
$(NAME)_SOURCES  += hal/flash.c
$(NAME)_SOURCES  += hal/gpio.c

#GLOBAL_CFLAGS += -O3 -g3
GLOBAL_CFLAGS += -DNRF52832_XXAA
GLOBAL_CFLAGS += -DBOARD_PCA10040
#GLOBAL_CFLAGS += -DBSP_DEFINES_ONLY
GLOBAL_CFLAGS += -DCONFIG_GPIO_AS_PINRESET
GLOBAL_CFLAGS += -DFLOAT_ABI_HARD
GLOBAL_CFLAGS += -DNRF52
GLOBAL_CFLAGS += -DNRF52_PAN_74
#GLOBAL_CFLAGS += -mcpu=cortex-m4
#GLOBAL_CFLAGS += -mthumb -mabi=aapcs
#GLOBAL_CFLAGS +=  -Wall -Werror
#GLOBAL_CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
#GLOBAL_CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
#GLOBAL_CFLAGS += -fno-builtin -fshort-enums

GLOBAL_ASMFLAGS += -DNRF52832_XXAA
GLOBAL_ASMFLAGS += -DBOARD_PCA10040
GLOBAL_ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
GLOBAL_ASMFLAGS += -DFLOAT_ABI_HARD
GLOBAL_ASMFLAGS += -DNRF52
GLOBAL_ASMFLAGS += -DNRF52_PAN_74
#GLOBAL_ASMFLAGS += -g3
#GLOBAL_ASMFLAGS += -mcpu=cortex-m4
#GLOBAL_ASMFLAGS += -mthumb -mabi=aapcs
#GLOBAL_ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16                   

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += Drivers/toolchain/arm/arm_startup_nrf52.s
     
else ifeq ($(COMPILER), iar)

else
$(NAME)_SOURCES += Drivers/toolchain/gcc/gcc_startup_nrf52.S

endif
     
ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/mcu/nrf52xxx/Drivers/toolchain/arm/
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/mcu/stm32l4xx/src/STM32L433RC-Nucleo/STM32L433.icf
else

ifeq ($(MBINS),)
GLOBAL_LDFLAGS += -T platform/mcu/nrf52xxx/nrf52832_xxaa.ld
else ifeq ($(MBINS),app)
GLOBAL_LDFLAGS += -T platform/mcu/nrf52xxx/nrf52832_xxaa_app.ld
else ifeq ($(MBINS),kernel)
GLOBAL_LDFLAGS += -T platform/mcu/nrf52xxx/nrf52832_xxaa_kernel.ld
endif

GLOBAL_CFLAGS += -DNRF_DFU_SETTINGS_VERSION=0

GLOBAL_DEFINES += NRF52832_XXAA

GLOBAL_DEFINES += nrf52

endif

