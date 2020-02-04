NAME := mcu_cy8c4147

HOST_OPENOCD := cy8c4147

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu cy8c4147

$(NAME)_COMPONENTS += arch_armv6m
$(NAME)_COMPONENTS += rhino

GLOBAL_INCLUDES += ../../arch/arm/armv6m/gcc/m0

GLOBAL_INCLUDES += PSoC4 \
				   hal

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS += --c99 --cpu=Cortex-M0PLUS -D__MICROLIB -g --apcs=interwork --split_sections
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS += --cpu=Cortex-M0PLUS  \
                 --cpu_mode=thumb \
                 --endian=little
else
GLOBAL_CFLAGS += -mcpu=cortex-m0plus          \
                 -mlittle-endian           \
                 -mthumb -mthumb-interwork \
                 -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=Cortex-M0PLUS -g --apcs=interwork --library_type=microlib --pd "__MICROLIB SETA 1"
else ifeq ($(COMPILER),iar)
GLOBAL_ASMFLAGS += --cpu Cortex-M0PLUS  \
                   --cpu_mode thumb \
                   --endian little
else
GLOBAL_ASMFLAGS += -mcpu=cortex-m0plus           \
                   -mlittle-endian           \
                   -mthumb -mthumb-interwork \
                   -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=Cortex-M0PLUS                         \
                  -L --strict                           \
                  -L --xref -L --callgraph -L --symbols \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --silent --cpu=Cortex-M0.vfp

else
GLOBAL_LDFLAGS += -mcpu=cortex-m0plus           \
                  -mlittle-endian           \
                  -mthumb -mthumb-interwork \
				  -lm \
                  --specs=nosys.specs       \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L 
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config 
else
GLOBAL_LDFLAGS += -T platform/mcu/cy8c4147/PSoC4/cm0plusgcc.ld
endif

GLOBAL_LDFLAGS += platform/mcu/cy8c4147/PSoC4/AsrLib.a

$(NAME)_SOURCES := aos/soc_impl.c                                                  \
                   hal/hw.c                                                        \
                   hal/flash_port.c                                                \
                   hal/uart_port.c                                                 \
                   runapp/aos_smallsize.c      \
				   PSoC4/CyBootAsmGnu.s        \
				   PSoC4/antpow.c              \
				   PSoC4/antpow_PM.c           \
				   PSoC4/Cm0plusStart.c        \
				   PSoC4/CyDMA.c               \
				   PSoC4/cy_em_eeprom.c        \
				   PSoC4/cyfitter_cfg.c        \
				   PSoC4/CyFlash.c             \
				   PSoC4/CyLFClk.c             \
                   PSoC4/CyLib.c               \
				   PSoC4/cymetadata.c          \
				   PSoC4/cyPm.c                \
				   PSoC4/cyutils.c             \
				   PSoC4/dio1.c                \
				   PSoC4/dio1_PM.c             \
				   PSoC4/global_irq.c          \
				   PSoC4/nss.c                 \
				   PSoC4/nss_PM.c              \
				   PSoC4/pin_wakeup.c          \
				   PSoC4/pin_wakeup_PM.c       \
				   PSoC4/RTC.c                 \
				   PSoC4/SPI_1_BOOT.c          \
				   PSoC4/SPI_1.c               \
				   PSoC4/SPI_1_miso_m.c        \
				   PSoC4/SPI_1_miso_m_PM.c     \
				   PSoC4/SPI_1_mosi_m.c        \
				   PSoC4/SPI_1_mosi_m_PM.c     \
				   PSoC4/SPI_1_PM.c            \
				   PSoC4/SPI_1_SCBCLK.c        \
				   PSoC4/SPI_1_sclk_m.c        \
				   PSoC4/SPI_1_sclk_m_PM.c     \
				   PSoC4/SPI_1_SPI_BOOT.c      \
				   PSoC4/SPI_1_SPI.c           \
				   PSoC4/SPI_1_SPI_UART.c      \
				   PSoC4/SPI_BUSY.c            \
				   PSoC4/SPI_BUSY_PM.c         \
				   PSoC4/SPI_NRESET.c          \
				   PSoC4/SPI_NRESET_PM.c       \
				   PSoC4/UART_1_BOOT.c         \
				   PSoC4/UART_1.c              \
				   PSoC4/UART_1_PM.c           \
				   PSoC4/UART_1_rx_wake.c      \
				   PSoC4/UART_1_rx_wake_PM.c   \
				   PSoC4/UART_1_RX_WAKEUP_IRQ.c \
				   PSoC4/UART_1_SCBCLK.c        \
				   PSoC4/UART_1_SCB_IRQ.c       \
				   PSoC4/UART_1_SPI_UART.c      \
				   PSoC4/UART_1_SPI_UART_INT.c  \
				   PSoC4/UART_1_tx.c            \
				   PSoC4/UART_1_tx_PM.c         \
				   PSoC4/UART_1_UART_BOOT.c     \
				   PSoC4/UART_1_UART.c          \
				   PSoC4/wakeup_irq.c
