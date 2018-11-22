#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#


$(NAME)_INCLUDES += ./
$(NAME)_INCLUDES += ../include
$(NAME)_INCLUDES += sdk/component/os/os_dep/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/app/monitor/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/cmsis
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/cmsis/device
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/fwlib
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/fwlib/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/fwlib/ram_lib/crypto
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/fwlib/rom_lib
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/os_dep/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/std_lib/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/std_lib/libc/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/std_lib/libc/rom/string
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/std_lib/libgcc/rtl8195a/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/rtl_lib
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/misc
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/misc/os
$(NAME)_INCLUDES += sdk/component/common/mbed/api
$(NAME)_INCLUDES += sdk/component/common/mbed/hal
$(NAME)_INCLUDES += sdk/component/common/mbed/hal_ext
$(NAME)_INCLUDES += sdk/component/common/mbed/targets/cmsis/rtl8711b
$(NAME)_INCLUDES += sdk/component/common/mbed/targets/hal/rtl8711b

$(NAME)_INCLUDES += sdk/project/realtek_amebaz_va0_example/inc
#GLOBAL_INCLUDES := .
#GLOBAL_INCLUDES += ..\..\mcu/include
#$(NAME)_INCLUDES += .
#$(NAME)_INCLUDES += .
#$(NAME)_INCLUDES += ../../include
#$(NAME)_INCLUDES += ../../../../kernel/hal/include/soc
#$(NAME)_INCLUDES += ../../../../include/aos
#$(NAME)_INCLUDES += ../../../../osal/mico/include
#$(NAME)_INCLUDES += ../../../../middleware/alink/protocol/os/platform
#$(NAME)_INCLUDES += platform\mcu\rtl8710bn\peripherals\include

$(NAME)_SOURCES += peripherals/platform_8711.c          \
                   peripherals/platform_adc.c           \
                   peripherals/platform_rtc.c           \
                   peripherals/platform_gpio.c          \
                   peripherals/platform_i2c.c           \
                   peripherals/platform_rng.c           \
                   peripherals/platform_mcu_powersave.c \
                   peripherals/platform_pwm.c           \
                   peripherals/platform_spi.c           \
                   peripherals/platform_uart.c          \
                   peripherals/platform_watchdog.c      \
                   peripherals/RingBufferUtils.c
