HOST_OPENOCD := LPC54608
NAME := mcu_lpc54608impl

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := driver & sdk for platform/mcu lpc54608impl

$(NAME)_COMPONENTS += arch_armv7m
$(NAME)_COMPONENTS += newlib_stub rhino netmgr lwip

GLOBAL_CFLAGS += -DCORE_M4
GLOBAL_CFLAGS += -fmessage-length=0
GLOBAL_CFLAGS += -fno-builtin -ffunction-sections -fdata-sections -fno-common -std=c99 -DSDK_DEBUGCONSOLE=1
#GLOBAL_CFLAGS += $(CPU_CFLAGS) -mlittle-endian
GLOBAL_CFLAGS += -mcpu=cortex-m4 -mlittle-endian -mthumb -mthumb-interwork -w

GLOBAL_INCLUDES += ./ \
                   CMSIS/Include \
                   drivers \
                   gcc \
                   utilities \
                   utilities/str \
                   utilities/log \
                   utilities/io
GLOBAL_INCLUDES += ./lwip-2.0.2/port/
#GLOBAL_LDFLAGS += -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -mlittle-endian -mthumb-interwork -nostartfiles
GLOBAL_LDFLAGS  += -mcpu=cortex-m4           \
                   -mthumb -mthumb-interwork \
                   -mlittle-endian           \
                   $(CLIB_LDFLAGS_NANO_FLOAT)

GLOBAL_LDFLAGS += -L$(SOURCE_ROOT)platform/mcu/lpc54608/gcc
ifeq ($(HOST_ARCH),Cortex-M4F)
GLOBAL_LDFLAGS += -lpower_hardabi
else ifeq ($(HOST_ARCH),Cortex-M4)
GLOBAL_LDFLAGS += -lpower_softabi
endif

$(NAME)_SOURCES :=

#$(NAME)_SOURCES     += ../../arch/arm/armv7m/gcc/m4/port_c.c
#$(NAME)_SOURCES     += ../../arch/arm/armv7m/gcc/m4/port_s.S

$(NAME)_SOURCES += ./drivers/fsl_adc.c
$(NAME)_SOURCES += ./drivers/fsl_clock.c
$(NAME)_SOURCES += ./drivers/fsl_common.c
$(NAME)_SOURCES += ./drivers/fsl_crc.c
$(NAME)_SOURCES += ./drivers/fsl_ctimer.c
$(NAME)_SOURCES += ./drivers/fsl_dma.c
$(NAME)_SOURCES += ./drivers/fsl_flashiap.c
$(NAME)_SOURCES += ./drivers/fsl_fmeas.c
$(NAME)_SOURCES += ./drivers/fsl_gint.c
$(NAME)_SOURCES += ./drivers/fsl_gpio.c
$(NAME)_SOURCES += ./drivers/fsl_i2c.c
$(NAME)_SOURCES += ./drivers/fsl_i2c_dma.c
$(NAME)_SOURCES += ./drivers/fsl_inputmux.c
$(NAME)_SOURCES += ./drivers/fsl_mrt.c
$(NAME)_SOURCES += ./drivers/fsl_pint.c
$(NAME)_SOURCES += ./drivers/fsl_power.c
$(NAME)_SOURCES += ./drivers/fsl_reset.c
$(NAME)_SOURCES += ./drivers/fsl_rit.c
$(NAME)_SOURCES += ./drivers/fsl_rtc.c
$(NAME)_SOURCES += ./drivers/fsl_sctimer.c
$(NAME)_SOURCES += ./drivers/fsl_spi.c
$(NAME)_SOURCES += ./drivers/fsl_spi_dma.c
$(NAME)_SOURCES += ./drivers/fsl_flexcomm.c
$(NAME)_SOURCES += ./drivers/fsl_usart.c
$(NAME)_SOURCES += ./drivers/fsl_usart_dma.c
$(NAME)_SOURCES += ./drivers/fsl_enet.c
$(NAME)_SOURCES += ./drivers/fsl_utick.c
$(NAME)_SOURCES += ./drivers/fsl_wwdt.c
$(NAME)_SOURCES += ./system_LPC54608.c
$(NAME)_SOURCES += ./utilities/log/fsl_log.c
$(NAME)_SOURCES += ./utilities/str/fsl_str.c
$(NAME)_SOURCES += ./utilities/io/fsl_io.c
$(NAME)_SOURCES += ./utilities/fsl_debug_console.c
$(NAME)_SOURCES += ./gcc/startup_LPC54608.S
$(NAME)_SOURCES += ./hal/hal_uart.c
$(NAME)_SOURCES += ./hal/csp_log.c
$(NAME)_SOURCES += ./hal/hal_flash.c
$(NAME)_SOURCES += ./lwip-2.0.2/port/ethernetif.c
$(NAME)_SOURCES += ./aos/aos.c
$(NAME)_SOURCES += ./aos/soc_impl.c
$(NAME)_SOURCES += ./hal/hook_impl.c

GLOBAL_CFLAGS += -DA_LITTLE_ENDIAN

ifndef OVERRIDE_LD_FILE
GLOBAL_LDFLAGS += -T platform/mcu/lpc54608/gcc/LPC54608J512_flash.ld
#GLOBAL_LDFLAGS += -T platform/mcu/lpc54608/gcc/LPC54608J512_ram.ld
endif
