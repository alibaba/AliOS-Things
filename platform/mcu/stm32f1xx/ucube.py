src =Split('''
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c  \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_can.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cec.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_crc.c  \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dac.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dac_ex.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_eth.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c  \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_hcd.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2c.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2s.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_irda.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_iwdg.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_mmc.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_nand.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_nor.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pccard.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd_ex.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc_ex.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_sd.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_smartcard.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi_ex.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_sram.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_usart.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_wwdg.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_adc.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_crc.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_dac.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_dma.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_exti.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_fsmc.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_gpio.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_i2c.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_pwr.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rcc.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rtc.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_sdmmc.c    \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_spi.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_tim.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usart.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usb.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_utils.c \
    Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c\
    hal/hal_uart_stm32f1.c \
    hal/hw.c \
    hal/hal_flash_stm32f1.c \
    hal/hal_gpio_stm32f1.c #\
    hal/hal_spi_stm32f1.c \
    hal/hal_i2c_stm32f1.c \
    hal/hal_sd_stm32f1.c \
    hal/hal_adc_stm32f1.c \
    hal/hal_rtc_stm32f1.c \
    hal/hal_spi_stm32f1.c \
    hal/hal_qspi_stm32f1.c \
    hal/hal_nand_stm32f1.c \
    hal/hal_nor_stm32f1.c
''')

prefix = ''
if aos_global_config.compiler == "gcc":
    prefix = 'arm-none-eabi-'

component =aos_mcu_component('stm32f1xx', prefix, src)
component.set_global_arch("Cortex-M3")

HOST_MCU_NAME = aos_global_config.get('HOST_MCU_NAME')
dependencis =Split('''
    platform/arch/arm/armv7m
    utility/libc
    kernel/rhino
    kernel/hal
    kernel/vfs
''')
for i in dependencis:
    component.add_comp_deps(i)

global_includes =Split('''
    hal\
    hal/include \
    Drivers/STM32F1xx_HAL_Driver/Inc \
    Drivers/STM32F1xx_HAL_Driver/Inc/Legacy \
    Drivers/CMSIS/Include \
    Drivers/CMSIS/Device/ST/STM32F1xx/Include
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split('''
    USE_HAL_DRIVER
''')

global_cflags = []

if aos_global_config.compiler == 'armcc':
    global_cflags = Split('''
        --c99
        --cpu=Cortex-M3
        --apcs=/softfp
        -D__MICROLIB
        -g
        --split_sections
    ''')
elif  aos_global_config.compiler == 'iar':
    global_cflags = Split('''
        --cpu=Cortex-M3
        --cpu_mode=thumb
        --endian=little
    ''')
else:
    global_cflags = Split('''
        -mcpu=cortex-m3
        -mlittle-endian
        -mthumb -mthumb-interwork
        -mfloat-abi=soft
        -w
        -D__VFP_FP__
    ''')

for i in global_cflags:
    component.add_global_cflags(i)

global_asflags = []
if aos_global_config.compiler == 'armcc':
    global_asflags = Split('''
        --cpu=Cortex-M3
        -g
        --apcs=/soft
        --library_type=microlib
        --pd
        "__MICROLIB SETA 1"
    ''')
elif  aos_global_config.compiler == 'iar':
    global_asflags = Split('''
        --cpu
        Cortex-M3
        --cpu_mode
        thumb
        --endian
        little
    ''')
else:
    global_asflags = Split('''
        -mcpu=cortex-m3
        -mlittle-endian
        -mthumb
        -mfloat-abi=soft
        -w
    ''')

for i in global_asflags:
    component.add_global_asflags(i)

global_ldflags = []
if aos_global_config.compiler == 'armcc':
    global_ldflags = Split('''
        -L
        --cpu=Cortex-M3
        -L
        --apcs=/soft
        -L
        --strict
        -L
        --xref
        -L
        --callgraph
        -L
        --symbols
        -L
        --info=sizes
        -L
        --info=totals
        -L
        --info=unused
        -L
        --info=veneers
        -L
        --info=summarysizes
    ''')
elif  aos_global_config.compiler == 'iar':
    global_ldflags = Split('''
        --silent
        --cpu=Cortex-M3
         --config
    ''')
else:
    global_ldflags = Split('''
        -mcpu=cortex-m3
        -mlittle-endian
        -mthumb
        -mthumb-interwork
        --specs=nosys.specs
        -mfloat-abi=soft
    ''')

for i in global_ldflags:
    component.add_global_ldflags(i)

component.add_comp_deps('osal')
component.add_comp_deps('kernel/init')

