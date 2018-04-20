src =Split(''' 
    board.c
    board_cli.c  
''')
component =aos_board_component('starterkit', 'stm32l4xx', src)

aos_global_config.set('MODULE', '1062')
aos_global_config.set('HOST_ARCH', 'Cortex-M4')
aos_global_config.set('HOST_MCU_FAMILY', 'stm32l4xx')
aos_global_config.set('SUPPORT_BINS', 'no')
aos_global_config.set('HOST_MCU_NAME', 'STM32L433RC-Nucleo')

if aos_global_config.ide == 'keil':
    component.add_sources('st7789.c')
    
dependencis =Split(''' 
    device/sensor
''')
for i in dependencis:
    component.add_comp_deps(i)

global_includes =Split(''' 
    .
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split(''' 
    STDIO_UART=0
    CONFIG_AOS_CLI_BOARD
    AOS_SENSOR_ACC_MIR3_DA217
    AOS_SENSOR_ALS_LITEON_LTR553
    AOS_SENSOR_PS_LITEON_LTR553
''')
for i in global_macros:
    component.add_global_macros(i)

if aos_global_config.get('sal') == None:
    aos_global_config.set('sal',1)
    
if aos_global_config.get('no_tls') == None:
    aos_global_config.set('no_tls',1)
    
if aos_global_config.get('sal') == 1:
    component.add_comp_deps('device/sal')
    if aos_global_config.get('module') == None:
        aos_global_config.set('module','wifi.mk3060')
    else:
        aos_global_config.add_global_macros('CONFIG_NO_TCPIP')

aos_global_config.set('CONFIG_SYSINFO_PRODUCT_MODEL', 'ALI_AOS_starterkit')
aos_global_config.set('CONFIG_SYSINFO_DEVICE_NAME','starterkit')
 
CONFIG_SYSINFO_OS_VERSION = aos_global_config.get('CONFIG_SYSINFO_OS_VERSION')

component.add_global_macros('SYSINFO_OS_VERSION=\\"'+str(CONFIG_SYSINFO_OS_VERSION)+'\\"')
component.add_global_macros('SYSINFO_PRODUCT_MODEL=\\"'+'ALI_AOS_starterkit'+'\\"')
component.add_global_macros('SYSINFO_DEVICE_NAME=\\"'+'starterkit'+'\\"')

component.add_comp_deps('platform/mcu/stm32l4xx')

component.set_enable_vfp()
    
    

