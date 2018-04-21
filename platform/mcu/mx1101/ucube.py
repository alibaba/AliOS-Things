src = Split('''
        aos/soc_impl.c
        hal/i2c.c
        hal/flash.c 
''')

deps = Split('''
        kernel/rhino
        platform/arch/arm/armv7m
        platform/mcu/mx1101/sdk
        kernel/vcall
        kernel/init
''')

global_macro = Split('''
        STDIO_UART=0 
        CONFIG_NO_TCPIP
        RHINO_CONFIG_TICK_TASK=0 
        RHINO_CONFIG_WORKQUEUE=0
        CONFIG_AOS_KV_MULTIPTN_MODE
        CONFIG_AOS_KV_PTN=5
        CONFIG_AOS_KV_SECOND_PTN=6
        CONFIG_AOS_KV_PTN_SIZE=4096
        CONFIG_AOS_KV_BUFFER_SIZE=8192
        SYSINFO_PRODUCT_MODEL=\\"ALI_AOS_MX1101\\" 
        SYSINFO_DEVICE_NAME=\\"MX1101\\"
''')


global_cflags = Split('''
        -mcpu=cortex-m3
        -mthumb
        -mfloat-abi=soft
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
''')

global_includes = Split('''
        ../../arch/arm/armv7m/gcc/m4
''')

global_ldflags = Split('''
        -mcpu=cortex-m3        
        -mthumb -mthumb-interwork
        -mlittle-endian
        -nostartfiles
        --specs=nosys.specs
''')


prefix = ''
if aos_global_config.compiler == "gcc":
    prefix = 'arm-none-eabi-'

component = aos_mcu_component('MX1101', prefix, src)
component.set_global_arch('Cortex-M3')

component.add_comp_deps(*deps)
component.add_global_macros(*global_macro)
component.add_global_cflags(*global_cflags)
component.add_cflags(*local_cflags)
component.add_global_includes(*global_includes)
component.add_global_ldflags(*global_ldflags)