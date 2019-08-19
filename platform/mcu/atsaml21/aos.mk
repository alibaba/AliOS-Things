NAME := mcu_atsaml21
HOST_OPENOCD := at91samdXX

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu atsaml21

$(NAME)_COMPONENTS += arch_armv6m rhino

GLOBAL_INCLUDES += drivers/cmsis/include \
                   drivers/saml21b/include \
                   drivers/startup \
                   drivers/hal/include \
                   drivers/hal/utils/include \
                   drivers/hpl/core \
                   drivers/hpl/gclk \
                   drivers/hpl/pm \
                   drivers/hpl/port \
                   drivers/hpl/tc \
                   drivers/hpl/tcc \
                   drivers/hri \
                   drivers/stdio_redirect

$(NAME)_INCLUDES += drivers/hal/utils/internal_include

$(NAME)_SOURCES += drivers/startup/system_saml21.c \
                   drivers/startup/atmel_start.c \
                   drivers/startup/driver_init.c \
                   drivers/startup/stdio_start.c \
                   drivers/hal/src/hal_atomic.c \
                   drivers/hal/src/hal_delay.c \
                   drivers/hal/src/hal_ext_irq.c \
                   drivers/hal/src/hal_gpio.c \
                   drivers/hal/src/hal_i2c_m_sync.c \
                   drivers/hal/src/hal_init.c \
                   drivers/hal/src/hal_io.c \
                   drivers/hal/src/hal_pwm.c \
                   drivers/hal/src/hal_sleep.c \
                   drivers/hal/src/hal_usart_async.c \
                   drivers/hal/src/hal_usart_sync.c \
                   drivers/hal/src/hal_flash.c \
                   drivers/hal/utils/src/utils_assert.c \
                   drivers/hal/utils/src/utils_event.c \
                   drivers/hal/utils/src/utils_list.c \
                   drivers/hal/utils/src/utils_ringbuffer.c \
                   drivers/hpl/core/hpl_core_m0plus_base.c \
                   drivers/hpl/core/hpl_init.c \
                   drivers/hpl/dmac/hpl_dmac.c \
                   drivers/hpl/eic/hpl_eic.c \
                   drivers/hpl/gclk/hpl_gclk.c \
                   drivers/hpl/mclk/hpl_mclk.c \
                   drivers/hpl/osc32kctrl/hpl_osc32kctrl.c \
                   drivers/hpl/oscctrl/hpl_oscctrl.c \
                   drivers/hpl/pm/hpl_pm.c \
                   drivers/hpl/sercom/hpl_sercom.c \
                   drivers/hpl/tc/hpl_tc.c \
                   drivers/hpl/tcc/hpl_tcc.c \
                   drivers/hpl/nvmctrl/hpl_nvmctrl.c \
                   drivers/stdio_redirect/stdio_io.c \
                   drivers/stdio_redirect/gcc/read.c \
                   drivers/stdio_redirect/gcc/write.c \
                   hal/uart.c \
                   hal/flash.c \
                   hal/timer.c

GLOBAL_DEFINES += $$(if $$(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)

GLOBAL_CFLAGS += -mcpu=cortex-m0plus \
                 -march=armv6-m \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian \
                 -w

GLOBAL_LDFLAGS += -mcpu=cortex-m0plus        \
                  -mthumb -mthumb-interwork \
                  -mlittle-endian \
                  -lm \
                  -Wl,--wrap,main \
                  --specs=nano.specs -u _printf_float


EXTRA_POST_BUILD_TARGETS += copy_output_for_eclipse

