src =Split('''
    board.c
    src/debug.c
    src/hw_gpio.c
    src/hw_spi.c
    src/hw_rtc.c
    src/eml3047_hw.c
    src/eml3047_it.c
    src/vcom.c
    src/lorawan_port.c
    src/lora.c
''')
component =aos_component('board_eml3047_new', src)


global_includes =Split('''
    .
    inc
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split('''
    STDIO_UART=0
    CONFIG_NO_TCPIP
    RHINO_CONFIG_TICK_TASK=0
    RHINO_CONFIG_WORKQUEUE=0
    $(if
    $(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)
''')
for i in global_macros:
    component.add_global_macros(i)

includes =Split('''
    ../../platform/mcu/stm32l0xx/Drivers/STM32L0xx_HAL_Driver/Inc
    ../../platform/mcu/stm32l0xx/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy
    ../../platform/mcu/stm32l0xx/Drivers/CMSIS/Device/ST/STM32L0xx/Include
    ../../platform/mcu/stm32l0xx/Drivers/CMSIS/Include
''')
for i in includes:
    component.add_includes(i)

macros =Split('''
    USE_HAL_DRIVER
    STM32L071xx
''')
for i in macros:
    component.add_macros(i)

linux_only_targets="helloworld lorawan.linkwan udata_demo.sensor_local_demo"
