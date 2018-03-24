src     = Split('''
       src/B-L475E-IOT01/runapp/stm32l4xx_hal_msp.c      
       src/B-L475E-IOT01/runapp/stm32l4xx_it.c           
       src/B-L475E-IOT01/runapp/soc_init.c          
       src/B-L475E-IOT01/runapp/system_stm32l4xx.c      
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c        
       src/B-L475E-IOT01/hal/flash_l4.c  
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c  
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c 
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c 
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c    
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c 
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c    
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_qspi.c   
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c 
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.c    
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc.c    
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc_ex.c 
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c    
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.c 
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c    
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c   
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c  
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c   
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c    
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c 
       Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c 
       Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_accelero.c 
       Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_gyro.c 
       Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_hsensor.c 
       Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.c 
       Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_psensor.c 
       Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_tsensor.c 
       Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01.c 
       Drivers/BSP/Components/es_wifi/es_wifi.c 
       Drivers/BSP/Components/hts221/hts221.c 
       Drivers/BSP/Components/lis3mdl/lis3mdl.c 
       Drivers/BSP/Components/lps22hb/lps22hb.c 
       Drivers/BSP/Components/lsm6dsl/lsm6dsl.c 
       Drivers/BSP/Components/vl53l0x/vl53l0x_api.c 
       Drivers/BSP/Components/vl53l0x/vl53l0x_api_calibration.c 
       Drivers/BSP/Components/vl53l0x/vl53l0x_api_core.c 
       Drivers/BSP/Components/vl53l0x/vl53l0x_api_ranging.c 
       Drivers/BSP/Components/vl53l0x/vl53l0x_api_strings.c 
       Drivers/BSP/Components/vl53l0x/vl53l0x_platform_log.c 
       aos/soc_impl.c                
       aos/trace_impl.c             
       src/B-L475E-IOT01/runapp/aos.c                    
       src/common/csp/wifi/src/es_wifi_io.c        
       src/common/csp/wifi/src/wifi.c              
       src/B-L475E-IOT01/hal/hw.c                     
       src/B-L475E-IOT01/hal/flash_port.c              
       src/B-L475E-IOT01/hal/ota_port.c              
       src/B-L475E-IOT01/hal/hal_i2c_stm32l4.c       
       src/B-L475E-IOT01/sensor/vl53l0x_platform.c 
       src/B-L475E-IOT01/sensor/vl53l0x_proximity.c 
       src/B-L475E-IOT01/sensor/sensors_data.c 
       src/B-L475E-IOT01/sensor/sensors.c 
       src/B-L475E-IOT01/hal/wifi_port.c 
''')

if aos_global_config.compiler == 'armcc':
    src.append( 'src/B-L475E-IOT01/runapp/startup_stm32l475xx_armcc.s' )
elif aos_global_config.compiler == 'iar':
    src.append( 'src/B-L475E-IOT01/runapp/startup_stm32l475xx_icc.s' )
else:
    src.append( 'src/B-L475E-IOT01/runapp/startup_stm32l475xx_gcc.s' )

if aos_global_config.ide != 'keil':
    src.append( 'src/B-L475E-IOT01/sensor/qspi.c')
    src.append( 'Middlewares/USB_Device/Core/Src/usbd_core.c' )
    src.append( 'Middlewares/USB_Device/Core/Src/usbd_ctlreq.c' )
    src.append( 'Middlewares/USB_Device/Core/Src/usbd_ioreq.c' )

component = aos_arch_component('stm32l475', src)
if aos_global_config.compiler == 'armcc':
    component.add_external_obj('src/B-L475E-IOT01/runapp/startup_stm32l475xx_armcc.o')
    component.add_external_obj('src/B-L475E-IOT01/runapp/stm32l4xx_it.o')

component.add_component_dependencis('platform/arch/arm/armv7m')
component.add_component_dependencis('utility/libc')
component.add_component_dependencis('kernel/rhino')
component.add_component_dependencis('kernel/hal')
component.add_component_dependencis('kernel/modules/fs/kv')
component.add_component_dependencis('kernel/vfs')
component.add_component_dependencis('utility/digest_algorithm')

component.set_global_arch('Cortex-M4')

include_tmp = Split('''
       ../../arch/arm/armv7m/gcc/m4
       src/common/csp/lwip/include 
       src/common/csp/wifi/inc     
       src/B-L475E-IOT01/include   
       src/B-L475E-IOT01/runapp    
       Drivers/STM32L4xx_HAL_Driver/Inc 
       Drivers/STM32L4xx_HAL_Driver/Inc/Legacy 
       Drivers/BSP/B-L475E-IOT01 
       Drivers/BSP/Components/es_wifi 
       Drivers/BSP/Components/hts221 
       Drivers/BSP/Components/lis3mdl 
       Drivers/BSP/Components/lps22hb 
       Drivers/BSP/Components/lsm6dsl 
       Drivers/BSP/Components/vl53l0x 
       Drivers/CMSIS/Include 
       ../../../include/hal 
       Middlewares/USB_Device/Core/Inc
''')

for i in include_tmp:
    component.add_global_includes(i)
    
macro_tmp = Split('''
   CONFIG_AOS_KV_MULTIPTN_MODE
   CONFIG_AOS_KV_PTN=6
   CONFIG_AOS_KV_SECOND_PTN=7
   CONFIG_AOS_KV_PTN_SIZE=4096
   CONFIG_AOS_KV_BUFFER_SIZE=8192
   STM32L475xx
   RHINO_CONFIG_WORKQUEUE=1
''') 

for i in macro_tmp:
    component.add_global_macro(i)

if aos_global_config.compiler == 'armcc':
    cflags_tmp = Split('''
        --c99 
        --cpu=7E-M 
        -D__MICROLIB 
        -g 
        --apcs=interwork 
        --split_sections
    ''')
elif aos_global_config.compiler == 'iar':
    cflags_tmp = Split('''
        --cpu=Cortex-M4
        --cpu_mode=thumb
        --endian=little
    ''')
else:    
    cflags_tmp = Split('''
        -mcpu=cortex-m4
        -march=armv7-m  
        -mlittle-endian 
        -mthumb
        -mthumb-interwork
        -w
    ''')
for i in cflags_tmp: 
    component.add_global_cflags(i)

if aos_global_config.compiler == 'armcc':
    asflags_tmp = Split('''
        --cpu=7E-M
        -g
        --apcs=interwork
        --library_type=microlib
        --pd "__MICROLIB SETA 1"
    ''')
elif aos_global_config.compiler == 'iar':
    asflags_tmp = Split('''
        --cpu Cortex-M4
        --cpu_mode thumb
        --endian little
    ''')
else:
    asflags_tmp = Split('''
        -mcpu=cortex-m4 
        -march=armv7-m   
        -mlittle-endian 
        -mthumb 
        -mthumb-interwork 
        -w
    ''') 
for i in asflags_tmp:
    component.add_global_asflags(i)

if aos_global_config.compiler == 'armcc':
    ldflags_tmp = Split('''
        -L --cpu=7E-M
        -L --strict
        -L --xref -L --callgraph -L --symbols
        -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
    ''')
    ldflags_tmp.append('-L')
    ldflags_tmp.append('--scatter=platform/mcu/stm32l475/B-L475E-IOT01.sct')
elif aos_global_config.compiler == 'iar':
    ldflags_tmp = Split('''
        --silent 
        --cpu=Cortex-M4.vfp
    ''')     
    ldflags_tmp.append('--config')
    ldflags_tmp.append('platform/mcu/stm32l475/STM32L475.icf')
else:
    ldflags_tmp  = Split('''
        -mcpu=cortex-m4  
        -mlittle-endian  
        -mthumb 
        -mthumb-interwork 
        -nostartfiles    
        --specs=nosys.specs 
    ''') 
    CLIB_LDFLAGS_NANO_FLOAT = aos_global_config.get_aos_global_config('CLIB_LDFLAGS_NANO_FLOAT')
    ldflags_tmp.append( CLIB_LDFLAGS_NANO_FLOAT )
    ldflags_tmp.append('-T')
    ldflags_tmp.append('platform/mcu/stm32l475/STM32L475VGTx_FLASH.ld')

for i in ldflags_tmp:
    component.add_global_ldflags(i)

tool_chain = aos_global_config.create_tool_chain()
if aos_global_config.compiler == 'armcc':
    pass
elif aos_global_config.compiler == 'iar':
    pass
else:
    tool_chain.set_prefix('arm-none-eabi-')

aos_global_config.tool_chain_config(tool_chain)
