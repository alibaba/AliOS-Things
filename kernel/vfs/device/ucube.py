src     = Split('''
        vfs_adc.c
        vfs_uart.c
        vfs_gpio.c
        vfs_spi.c
        vfs_pwm.c
        vfs_rtc.c
        vfs_wdg.c
        vfs_i2c.c
''')

component = aos_component('vfs_device', src)

component.add_global_includes('../include/device', '../include', '../../hal/soc/')

