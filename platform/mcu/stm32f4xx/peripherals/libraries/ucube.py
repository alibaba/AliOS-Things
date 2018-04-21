src = Split('''
        system_stm32f4xx.c
        STM32F4xx_StdPeriph_Driver/src/misc.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_adc.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_can.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_crc.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dac.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dbgmcu.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rng.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_iwdg.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_pwr.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rtc.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sdio.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c
        STM32F4xx_StdPeriph_Driver/src/stm32f4xx_wwdg.c
''')

component = aos_component('STM32F4xx_Peripheral_Libraries', src)

if aos_global_config.get('HOST_MCU_VARIANT') not in ['STM32F411', 'STM32F401']:
    component.add_sources('STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fsmc.c')

if aos_global_config.get('HOST_MCU_VARIANT') == 'STM32F412':
    component.add_sources('STM32F4xx_StdPeriph_Driver/src/stm32f4xx_qspi.c')

component.add_global_includes('STM32F4xx_StdPeriph_Driver/inc', '../../../' +aos_global_config.arch +'/CMSIS')
