NAME = imx6dq

HOST_OPENOCD := imx6dq

# Host architecture is ARM Cortex A9
HOST_ARCH := Cortex-A9

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_COMPONENTS += platform/arch/arm/armv7a
$(NAME)_COMPONENTS += libc rhino hal middleware.common

GLOBAL_INCLUDES += ../../../arch/arm/armv7a/gcc/a9
GLOBAL_INCLUDES += ../../../../kernel/hal/include/soc/
GLOBAL_INCLUDES += ../aos   \
					../imx6_platform_sdk/sdk    \
					../imx6_platform_sdk/sdk/core    \
					../imx6_platform_sdk/sdk/core/src    \
					../imx6_platform_sdk/sdk/drivers/timer/test  \
					../imx6_platform_sdk/sdk/include  \
					../imx6_platform_sdk/board/mx6dq/smart_device  \
					../imx6_platform_sdk/sdk/include/mx6dq  \
					../imx6_platform_sdk/sdk/drivers  \
					../imx6_platform_sdk/sdk/drivers/board_id  \
					../imx6_platform_sdk/sdk/common  \
					../imx6_platform_sdk/board/common  \
					../imx6_platform_sdk/sdk/utility  \
					../imx6_platform_sdk/sdk/drivers/uart  \
					
#ifeq ($(OS_MUTICORE_NUM), 4)
ifdef OS_MUTICORE_NUM
GLOBAL_INCLUDES += ../imx6dq/smp
$(NAME)_SOURCES  += ../imx6dq/smp/smp_int.c
$(NAME)_SOURCES  += ../imx6dq/smp/smp_load.c
$(NAME)_SOURCES  += ../imx6dq/smp/smp_sync.c
endif					
					
					
#-mthumb -mthumb-interwork
GLOBAL_CFLAGS += -mcpu=cortex-a9 \
                 -march=armv7-a \
				 -mfpu=vfpv3 \
				 -mfloat-abi=softfp \
                 -mthumb-interwork \
                 -mlittle-endian

GLOBAL_CFLAGS += -w  

GLOBAL_ASMFLAGS += -mcpu=cortex-a9 \
                   -march=armv7-a  \
				   -mfpu=vfpv3 \
				   -mfloat-abi=softfp \
                   -mlittle-endian \
                   -mthumb-interwork \
                   -w

#--specs=nosys.specs
GLOBAL_LDFLAGS += -mcpu=cortex-a9        \
                  -mthumb-interwork \
                  -mlittle-endian \
				  -Wl,--gc-sections \
				  -nostartfiles \
				  --specs=nosys.specs \
				  -usystem_vectors \
                  $(CLIB_LDFLAGS_NANO_FLOAT)

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing


#$(NAME)_SOURCES := ../imx6_platform_sdk/sdk/core/src/startup.S   \


$(NAME)_SOURCES += 	../imx6_platform_sdk/sdk/core/src/mmu.c   \
					../imx6_platform_sdk/sdk/core/src/gic.c   \
					../imx6_platform_sdk/sdk/core/src/cortexA9.s   \
					../imx6_platform_sdk/sdk/core/src/ccm_pll.c   \
					../imx6_platform_sdk/sdk/core/src/armv7_cache.c   \
					../imx6_platform_sdk/sdk/core/src/arm_abort.S   \
					../imx6_platform_sdk/sdk/core/src/abort_handler.c   \
					../imx6_platform_sdk/sdk/core/src/interrupt.c   \
					../imx6_platform_sdk/sdk/drivers/cpu_utility/src/cpu_get_cores.c   \
					../imx6_platform_sdk/sdk/drivers/cpu_utility/src/cpu_multicore.c   \
					../imx6_platform_sdk/sdk/drivers/cpu_utility/src/cpu_workpoint.c   \
					../imx6_platform_sdk/sdk/drivers/timer/src/timer.c   \
					../imx6_platform_sdk/sdk/drivers/timer/src/epit.c   \
					../imx6_platform_sdk/sdk/drivers/timer/src/gpt.c   \
					../imx6_platform_sdk/sdk/utility/src/system_util.c   \
					../imx6_platform_sdk/sdk/drivers/uart/src/imx_uart.c   \
					../imx6_platform_sdk/sdk/drivers/i2c/src/imx_i2c.c   \
					../imx6_platform_sdk/sdk/common/ioexpander/src/max7310.c   \
					../imx6_platform_sdk/board/common/hardware_modules.c   \
					../imx6_platform_sdk/board/common/board_io_expanders.c   \
					../imx6_platform_sdk/board/mx6dq/smart_device/rev_a_iomux/uart_iomux_config.c   \
					../imx6_platform_sdk/board/mx6dq/smart_device/rev_a_iomux/uart1_iomux_config.c   \
					../imx6_platform_sdk/board/mx6dq/smart_device/rev_a_iomux/iomux_config.c   \
					../imx6_platform_sdk/board/mx6dq/smart_device/rev_a_iomux/i2c_iomux_config.c   \
					../imx6_platform_sdk/board/mx6dq/smart_device/rev_a_iomux/i2c1_iomux_config.c   \
					../imx6_platform_sdk/board/mx6dq/smart_device/rev_a_iomux/i2c2_iomux_config.c   \
					../imx6_platform_sdk/board/mx6dq/smart_device/rev_a_iomux/i2c3_iomux_config.c   \
					../imx6_platform_sdk/apps/common/platform_init.c   \
					../aos/int_handle.c   \
					../aos/aos_main.c   \
					../aos/hook_impl.c   \
					../aos/soc_impl.c   \
					../aos/soc_init.c \
					../hal/uart.c \


#	../aos/soc_init.c   \	
#	../imx6_platform_sdk/sdk/core/src/vectors.S   \		

#					cpu/vector_table.S \	  
# cpu/startup.S \ 

GLOBAL_LDS_FILES += platform/mcu/imx6/imx6_platform_sdk/apps/common/basic_alios_app_ocram.ld.S

GLOBAL_DEFINES += CONFIG_ARM

