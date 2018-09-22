NAME := ATSAML21J18B
HOST_OPENOCD := at91samdXX
$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv6m
$(NAME)_COMPONENTS += libc rhino hal rhino.fs.kv rhino.vfs cjson cli digest_algorithm

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192

$(NAME)_SOURCES := aos/aos.c \
                   aos/soc_impl.c \
                   aos/trace_impl.c

GLOBAL_INCLUDES += . $(HOST_MCU_VARIANT)/include CMSIS/include

                   
$(NAME)_SOURCES += Drivers/Device_Startup/startup_saml21.c \
                   Drivers/Device_Startup/system_saml21.c \
                   atmel_start.c

$(NAME)_SOURCES += Drivers/hal/src/hal_atomic.c \
                   Drivers/hal/src/hal_delay.c \
                   Drivers/hal/src/hal_ext_irq.c \
                   Drivers/hal/src/hal_gpio.c \
                   Drivers/hal/src/hal_i2c_m_sync.c \
                   Drivers/hal/src/hal_init.c \
                   Drivers/hal/src/hal_io.c \
                   Drivers/hal/src/hal_pwm.c \
                   Drivers/hal/src/hal_sleep.c \
                   Drivers/hal/src/hal_usart_async.c \
                   Drivers/hal/src/hal_usart_sync.c \
                   Drivers/hal/src/hal_flash.c

GLOBAL_INCLUDES += Drivers/hal/include


$(NAME)_SOURCES += Drivers/hal/utils/src/utils_assert.c \
                   Drivers/hal/utils/src/utils_event.c \
                   Drivers/hal/utils/src/utils_list.c \
                   Drivers/hal/utils/src/utils_ringbuffer.c
                   
$(NAME)_INCLUDES += Drivers/hal/utils/internal_include

GLOBAL_INCLUDES += Drivers/hal/utils/include

$(NAME)_SOURCES += Drivers/hpl/core/hpl_core_m0plus_base.c \
                   Drivers/hpl/core/hpl_init.c \
                   Drivers/hpl/dmac/hpl_dmac.c \
                   Drivers/hpl/eic/hpl_eic.c \
                   Drivers/hpl/gclk/hpl_gclk.c \
                   Drivers/hpl/mclk/hpl_mclk.c \
                   Drivers/hpl/osc32kctrl/hpl_osc32kctrl.c \
                   Drivers/hpl/oscctrl/hpl_oscctrl.c \
                   Drivers/hpl/pm/hpl_pm.c \
                   Drivers/hpl/sercom/hpl_sercom.c \
                   Drivers/hpl/tc/hpl_tc.c \
                   Drivers/hpl/tcc/hpl_tcc.c \
                   Drivers/hpl/nvmctrl/hpl_nvmctrl.c

GLOBAL_INCLUDES += Drivers/hpl/core \
                   Drivers/hpl/gclk \
                   Drivers/hpl/pm \
                   Drivers/hpl/port \
                   Drivers/hpl/tc \
                   Drivers/hpl/tcc \
                   Drivers/hri


$(NAME)_SOURCES += stdio_redirect/stdio_io.c \
                   stdio_redirect/gcc/read.c \
                   stdio_redirect/gcc/write.c

$(NAME)_SOURCES += hal/uart.c \
                   hal/flash.c


GLOBAL_INCLUDES += stdio_redirect
                

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


