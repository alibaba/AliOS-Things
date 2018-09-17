src = Split('''
        platform_init.c         
        platform_vector_table.c
        hardfault_handler.c
        startup/startup_stm32f4xx.s
        wifi/wifi_port.c 
        GCC/platform_unhandled_isr.c
        aos/soc_impl.c               
        aos/trace_impl.c            
        aos/aos.c                   
        hal/i2c.c                   
        hal/hw.c 
        hal/uart.c 
        hal/flash.c 
''')

deps = Split('''
        platform/arch/arm/armv7m
        utility/libc
        kernel/rhino
        kernel/hal
        network/netmgr
        middleware/common
        security/mbedtls
        utility/cjson
        tools/cli
        utility/digest_algorithm
        osal
        kernel/init
        platform/mcu/stm32f4xx/spi_flash
        network/lwip
        platform/mcu/stm32f4xx/peripherals
''')

global_macros = Split('''
        CONFIG_AOS_KV_MULTIPTN_MODE
        CONFIG_AOS_KV_PTN=6
        CONFIG_AOS_KV_SECOND_PTN=7
        CONFIG_AOS_KV_PTN_SIZE=4096
        CONFIG_AOS_KV_BUFFER_SIZE=8192
        USE_STDPERIPH_DRIVER
        _STM3x_
        _STM32x_
        STM32F4xx
        CONFIG_ARM
        SYSINFO_PRODUCT_MODEL=\\"ALI_AOS_MK3165\\"
        SYSINFO_DEVICE_NAME=\\"MK3165\\"
''')

mcu_variant = aos_global_config.get('HOST_MCU_VARIANT')
if mcu_variant in ['STM32F405', 'STM32F415', 'STM32F407', 'STM32F417']:
    global_macros.append('STM32F40_41xxx')
elif mcu_variant in ['STM32F427', 'STM32F437']:
    global_macros.append('STM32F427_437xx')
elif mcu_variant in ['STM32F429', 'STM32F439']:
    global_macros.append('STM32F429_439xx')
elif mcu_variant == 'STM32F401':
    global_macros.append('STM32F401xx')
elif mcu_variant == 'STM32F411':
    global_macros.append('STM32F411xE')
elif mcu_variant == 'STM32F412':
    global_macros.append('STM32F412xG')
elif mcu_variant == 'STM32F446':
    global_macros.append('STM32F446xx')


global_cflags = Split('''
        -mcpu=cortex-m4
        -march=armv7-m
        -mthumb -mthumb-interwork
        -mlittle-endian
        -w
    ''')

local_cflags = Split('''
        -Wall 
        -Werror 
        -Wno-unused-variable 
        -Wno-unused-parameter 
        -Wno-implicit-function-declaration
        -Wno-type-limits 
        -Wno-sign-compare 
        -Wno-pointer-sign 
        -Wno-uninitialized
        -Wno-return-type 
        -Wno-unused-function 
        -Wno-unused-but-set-variable
        -Wno-unused-value 
        -Wno-strict-aliasing
        -Werror 
        -Wstrict-prototypes  
        -W 
        -Wshadow  
        -Wwrite-strings 
        -pedantic 
        -U__STRICT_ANSI__ 
        -Wconversion 
        -Wextra 
        -Wdeclaration-after-statement 
        -Wconversion 
        -Waddress 
        -Wlogical-op 
        -Wstrict-prototypes 
        -Wold-style-definition 
        -Wmissing-prototypes 
        -Wmissing-declarations 
        -Wmissing-field-initializers 
        -Wdouble-promotion 
        -Wswitch-enum 
        -Wswitch-default 
        -Wuninitialized 
        -Wunknown-pragmas 
        -Wfloat-equal  
        -Wundef  
        -Wshadow
''')

global_includes = Split('''
        ../../arch/arm/armv7m/gcc/m4
        include
        csp/lwip/include
        startup   
        driver 
        wifi/inc
''')

global_ldflags = Split('''
        -mcpu=cortex-m4       
        -mthumb -mthumb-interwork
        -mlittle-endian
        -nostartfiles
        --specs=nosys.specs
''')

if aos_global_config.compiler == "armcc":
    asflags = Split('''
            --cpu=7E-M 
            -g
            --apcs=interwork
            -D__MICROLIB -DSTM32F4xx
    ''')
elif aos_global_config.compiler == "iar":
    asflags = Split('''
            --cpu Cortex-M4
            --cpu_mode thumb
            --endian little
    ''')
else:
    asflags = Split('''
           -mcpu=cortex-m4
           -march=armv7-m 
           -mlittle-endian
           -mthumb -mthumb-interwork
           -w
    ''')

if not aos_global_config.get('NO_WIFI'):
    src.append('wlan_bus_driver/wlan_platform_common.c')
    src.append('wlan_bus_driver/wlan_platform.c')
    src.append('wlan_bus_driver/wlan_bus_' + aos_global_config.get('BUS') + '.c')
else:
    if aos_global_config.get('SHARED_WIFI_SPI_BUS'):
        src.append('wlan_bus_driver/wlan_bus_SPI.c')

if aos_global_config.get('APP') == 'bootloader':
    aos_global_config.add_ld_files('GCC/bootloader.ld')
else:
    if aos_global_config.get('APP') == 'spi_flash_write':
        aos_global_config.add_ld_files('GCC/app_ram.ld')
        global_macros.append('__JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__=16384')
    else:
        aos_global_config.add_ld_files('GCC/app_with_bootloader.ld')


prefix = ''
if aos_global_config.compiler == "gcc":
    prefix = 'arm-none-eabi-'

component = aos_mcu_component('stm32f4xx', prefix, src)
component.set_global_arch('Cortex-M4')

component.add_comp_deps(*deps)
component.add_global_macros(*global_macros)
component.add_global_cflags(*global_cflags)
component.add_cflags(*local_cflags)
component.add_global_asflags(*asflags)
component.add_global_includes(*global_includes)
component.add_global_ldflags(*global_ldflags)
