NAME         := mcu_xmc
HOST_OPENOCD := xmc

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu xmc

$(NAME)_COMPONENTS += arch_armv7m

$(NAME)_COMPONENTS += newlib_stub rhino

GLOBAL_DEFINES += USE_HAL_DRIVER

GLOBAL_INCLUDES += Drivers/XMCLib/inc           \
                    Drivers/CMSIS/Include       \
                    Drivers/CMSIS/Infineon/XMC4800_series/Include   \

$(NAME)_SOURCES := Drivers/CMSIS/Infineon/XMC4800_series/Source/system_XMC4800.c   \
                    Drivers/XMCLib/src/xmc_acmp.c   \
                    Drivers/XMCLib/src/xmc_bccu.c   \
                    Drivers/XMCLib/src/xmc_can.c   \
                    Drivers/XMCLib/src/xmc_ccu4.c   \
                    Drivers/XMCLib/src/xmc_ccu8.c   \
                    Drivers/XMCLib/src/xmc_common.c   \
                    Drivers/XMCLib/src/xmc_dac.c   \
                    Drivers/XMCLib/src/xmc_dma.c   \
                    Drivers/XMCLib/src/xmc_dsd.c   \
                    Drivers/XMCLib/src/xmc_ebu.c   \
                    Drivers/XMCLib/src/xmc_ecat.c   \
                    Drivers/XMCLib/src/xmc_eru.c   \
                    Drivers/XMCLib/src/xmc_eth_mac.c   \
                    Drivers/XMCLib/src/xmc_eth_phy_dp83848.c   \
                    Drivers/XMCLib/src/xmc_eth_phy_ksz8031rnl.c   \
                    Drivers/XMCLib/src/xmc_eth_phy_ksz8081rnb.c   \
                    Drivers/XMCLib/src/xmc_fce.c   \
                    Drivers/XMCLib/src/xmc_gpio.c   \
                    Drivers/XMCLib/src/xmc_hrpwm.c   \
                    Drivers/XMCLib/src/xmc_i2c.c   \
                    Drivers/XMCLib/src/xmc_i2s.c   \
                    Drivers/XMCLib/src/xmc_ledts.c   \
                    Drivers/XMCLib/src/xmc_math.c   \
                    Drivers/XMCLib/src/xmc_pau.c   \
                    Drivers/XMCLib/src/xmc_posif.c   \
                    Drivers/XMCLib/src/xmc_prng.c   \
                    Drivers/XMCLib/src/xmc_rtc.c   \
                    Drivers/XMCLib/src/xmc_sdmmc.c   \
                    Drivers/XMCLib/src/xmc_spi.c   \
                    Drivers/XMCLib/src/xmc_uart.c   \
                    Drivers/XMCLib/src/xmc_usbd.c   \
                    Drivers/XMCLib/src/xmc_usbh.c   \
                    Drivers/XMCLib/src/xmc_usic.c   \
                    Drivers/XMCLib/src/xmc_vadc.c   \
                    Drivers/XMCLib/src/xmc_wdt.c   \
                    Drivers/XMCLib/src/xmc1_eru.c   \
                    Drivers/XMCLib/src/xmc1_flash.c   \
                    Drivers/XMCLib/src/xmc1_gpio.c   \
                    Drivers/XMCLib/src/xmc1_rtc.c   \
                    Drivers/XMCLib/src/xmc1_scu.c   \
                    Drivers/XMCLib/src/xmc4_eru.c   \
                    Drivers/XMCLib/src/xmc4_flash.c   \
                    Drivers/XMCLib/src/xmc4_gpio.c   \
                    Drivers/XMCLib/src/xmc4_rtc.c   \
                    Drivers/XMCLib/src/xmc4_scu.c   \

$(NAME)_SOURCES += hal/hw.c                \
                   hal/hal_flash_xmc4000.c                \
                   hal/hal_uart_xmc4000.c                \


ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS += --c99 --cpu=Cortex-M4 --apcs=/hardfp --fpu=vfpv4_sp_d16 -D__MICROLIB -g --split_sections
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS += --cpu=Cortex-M4  \
                 --endian=little
else
GLOBAL_CFLAGS += -mcpu=cortex-m4           \
                 -mlittle-endian           \
                 -mthumb -mthumb-interwork \
                 -mfloat-abi=hard          \
                 -mfpu=fpv4-sp-d16         \
                 -w
GLOBAL_CFLAGS += -D__VFP_FP__
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=Cortex-M4 -g --apcs=/hardfp --fpu=vfpv4_sp_d16 --library_type=microlib --pd "__MICROLIB SETA 1"
else ifeq ($(COMPILER),iar)
GLOBAL_ASMFLAGS += --cpu Cortex-M4  \
                   --endian little
else
GLOBAL_ASMFLAGS += -mcpu=cortex-m4   \
                   -mlittle-endian   \
                   -mthumb           \
                   -mfloat-abi=hard  \
                   -mfpu=fpv4-sp-d16 \
                   -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=Cortex-M4                    \
                  -L --fpu=vfpv4_sp_d16                 \
                  -L --apcs=/hardfp                     \
                  -L --strict                           \
                  -L --xref -L --callgraph -L --symbols \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --silent --cpu=Cortex-M4.vfp

else
GLOBAL_LDFLAGS += -mcpu=cortex-m4           \
                  -mlittle-endian           \
                  -mthumb -mthumb-interwork \
                  --specs=nosys.specs       \
                  -mfloat-abi=hard          \
                  -mfpu=fpv4-sp-d16         \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif
