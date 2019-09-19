NAME := mcu_imx6

HOST_OPENOCD := imx6

# Host architecture is ARM Cortex A9
HOST_ARCH := Cortex-A9

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu imx6

$(NAME)_COMPONENTS += arch_armv7a
$(NAME)_COMPONENTS += newlib_stub rhino

GLOBAL_INCLUDES += ./aos                                      \
                   ./imx6_platform_sdk/sdk                    \
                   ./imx6_platform_sdk/sdk/core               \
                   ./imx6_platform_sdk/sdk/core/src           \
                   ./imx6_platform_sdk/sdk/drivers/timer/test \
                   ./imx6_platform_sdk/sdk/include            \
                   ./imx6_platform_sdk/sdk/drivers            \
                   ./imx6_platform_sdk/sdk/drivers/board_id   \
                   ./imx6_platform_sdk/sdk/common             \
                   ./imx6_platform_sdk/board/common           \
                   ./imx6_platform_sdk/sdk/utility            \
                   ./imx6_platform_sdk/sdk/drivers/uart

#-mthumb -mthumb-interwork
GLOBAL_CFLAGS += -mcpu=cortex-a9    \
                 -mfpu=vfpv3        \
                 -mfloat-abi=softfp \
                 -mthumb-interwork  \
                 -mlittle-endian

GLOBAL_CFLAGS += -w

GLOBAL_ASMFLAGS += -mcpu=cortex-a9    \
                   -mfpu=vfpv3        \
                   -mfloat-abi=softfp \
                   -mlittle-endian    \
                   -mthumb-interwork  \
                   -w

#--specs=nosys.specs
GLOBAL_LDFLAGS += -mcpu=cortex-a9     \
                  -mthumb-interwork   \
                  -mlittle-endian     \
                  -Wl,--gc-sections   \
                  -nostartfiles       \
                  --specs=nosys.specs \
                  -usystem_vectors    \
                  $(CLIB_LDFLAGS_NANO_FLOAT)

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

#$(NAME)_SOURCES := ./imx6_platform_sdk/sdk/core/src/startup.S   \

$(NAME)_SOURCES +=  ./imx6_platform_sdk/sdk/core/src/mmu.c                          \
                    ./imx6_platform_sdk/sdk/core/src/gic.c                          \
                    ./imx6_platform_sdk/sdk/core/src/cortexA9.s                     \
                    ./imx6_platform_sdk/sdk/core/src/ccm_pll.c                      \
                    ./imx6_platform_sdk/sdk/core/src/armv7_cache.c                  \
                    ./imx6_platform_sdk/sdk/core/src/arm_abort.S                    \
                    ./imx6_platform_sdk/sdk/core/src/abort_handler.c                \
                    ./imx6_platform_sdk/sdk/core/src/interrupt.c                    \
                    ./imx6_platform_sdk/sdk/drivers/cpu_utility/src/cpu_get_cores.c \
                    ./imx6_platform_sdk/sdk/drivers/cpu_utility/src/cpu_multicore.c \
                    ./imx6_platform_sdk/sdk/drivers/cpu_utility/src/cpu_workpoint.c \
                    ./imx6_platform_sdk/sdk/drivers/timer/src/timer.c               \
                    ./imx6_platform_sdk/sdk/drivers/timer/src/epit.c                \
                    ./imx6_platform_sdk/sdk/drivers/timer/src/gpt.c                 \
                    ./imx6_platform_sdk/sdk/utility/src/system_util.c               \
                    ./imx6_platform_sdk/sdk/drivers/uart/src/imx_uart.c             \
                    ./imx6_platform_sdk/apps/common/platform_init.c                 \
                    ./aos/int_handle.c                                              \
                    ./aos/aos_main.c                                                \
                    ./aos/hook_impl.c                                               \
                    ./aos/soc_impl.c                                                \
                    ./aos/soc_init.c                                                \
                    ./hal/uart.c

GLOBAL_LDS_FILES += platform/mcu/imx6/imx6_platform_sdk/apps/common/basic_alios_app_ocram.ld.S

GLOBAL_DEFINES += CONFIG_ARM

ifeq ($(AOS_CONFIG_SMP),y)
include $($(NAME)_LOCATION)/imx6dq.mk
else
include $($(NAME)_LOCATION)/imx6sl.mk
endif
