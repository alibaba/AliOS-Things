src = Split('''
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c 
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_can.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_comp.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc.c 
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc_ex.c 
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cryp.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cryp_ex.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dac.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dac_ex.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dcmi.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dfsdm.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dfsdm_ex.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma2d.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dsi.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_firewall.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c 
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gfxmmu.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_hash.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_hash_ex.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_hcd.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_irda.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_iwdg.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_lcd.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_lptim.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_ltdc.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_ltdc_ex.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_nand.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_nor.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_opamp.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_opamp_ex.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_ospi.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd_ex.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_qspi.c  
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc_ex.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sai.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sd.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sd_ex.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_smartcard.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_smartcard_ex.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_smbus.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sram.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_swpmi.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tsc.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_wwdg.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_adc.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_comp.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_crc.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_crs.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dac.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma2d.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_exti.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_fmc.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_gpio.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_i2c.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_lptim.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_lpuart.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_opamp.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_pwr.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rcc.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rng.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rtc.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_sdmmc.c   
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_spi.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usart.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usb.c
        Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_utils.c
        Drivers/CMSIS/Device/ST/STM32L4xx/Source/Templates/system_stm32l4xx.c
        aos/soc_impl.c
        aos/trace_impl.c
        aos/aos.c
        hal/hal_uart_stm32l4.c
        hal/flash_l4.c
        hal/flash_port.c
        hal/hw.c
        hal/hal_sd_stm32l4.c
        hal/hal_adc_stm32l4.c
        hal/hal_i2c_stm32l4.c
        hal/hal_gpio_stm32l4.c
        hal/hal_rtc_stm32l4.c
        hal/hal_spi_stm32l4.c
        hal/hal_qspi_stm32l4.c
        hal/hal_nand_stm32l4.c
        hal/hal_nor_stm32l4.c
''')

deps = Split('''
        platform/arch/arm/armv7m
        utility/libc
        kernel/rhino
        kernel/hal
        kernel/vfs
        utility/digest_algorithm
        kernel/vcall
        kernel/init
''')

global_macros = Split('''
        CONFIG_AOS_KV_MULTIPTN_MODE
        CONFIG_AOS_KV_PTN=6
        CONFIG_AOS_KV_SECOND_PTN=7
        CONFIG_AOS_KV_PTN_SIZE=4096
        CONFIG_AOS_KV_BUFFER_SIZE=8192
        STDIO_UART=2
        
''')


global_includes = Split('''
        Drivers/STM32L4xx_HAL_Driver/Inc
        Drivers/STM32L4xx_HAL_Driver/Inc/Legacy
        Drivers/CMSIS/Include
        Drivers/CMSIS/Device/ST/STM32L4xx/Include
''')


if aos_global_config.compiler == "armcc":
    global_cflags = Split('''
        --c99
        --cpu=Cortex-M4 
        --apcs=/hardfp 
        --fpu=vfpv4_sp_d16 
        -D__MICROLIB 
        -g 
        --split_sections
    ''')
    
    asflags = Split('''
    --cpu=Cortex-M4 
    -g --apcs=/hardfp
    --fpu=vfpv4_sp_d16 
    --library_type=microlib 
    --pd "__MICROLIB SETA 1"
    ''')

    global_ldflags = Split('''
        -L --cpu=Cortex-M4  
        -L --fpu=vfpv4_sp_d16
        -L --apcs=/hardfp
        -L --strict
        -L --xref -L --callgraph -L --symbols
        -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
    ''')
    
elif aos_global_config.compiler == "iar":
    global_cflags = Split('''
        --cpu=Cortex-M4
        --cpu_mode=thumb
        --endian=little
    ''')

    asflags = Split('''
        --cpu Cortex-M4
        --cpu_mode thumb
        --endian little
    ''')
    
    global_ldflags = Split('''
        -L --cpu=Cortex-M4  
        --silent --cpu=Cortex-M4.vfp
    ''')
else:
    global_cflags = Split('''
        -mcpu=cortex-m4
        -march=armv7-m 
        -mlittle-endian
        -mthumb -mthumb-interwork
        -mfloat-abi=hard
        -mfpu=fpv4-sp-d16
        -w
        -D__VFP_FP__
    ''')

    asflags = Split('''
        -mcpu=cortex-m4
        -mlittle-endian
        -mthumb
        -mfloat-abi=hard
        -mfpu=fpv4-sp-d16
        -w
    ''')

    global_ldflags = Split('''
        -mcpu=cortex-m4 
        -mlittle-endian 
        -mthumb -mthumb-interwork
        --specs=nosys.specs
        -mfloat-abi=hard
        -mfpu=fpv4-sp-d16
    ''')

prefix = ''
if aos_global_config.compiler == "gcc":
    prefix = 'arm-none-eabi-'

component = aos_mcu_component('stm32l4xx_cube', prefix, src)
component.set_global_arch('Cortex-M4')

component.add_comp_deps(*deps)
component.add_global_macros(*global_macros)
component.add_global_includes(*global_includes)
component.add_global_cflags(*global_cflags)
component.add_global_asflags(*asflags)
component.add_global_ldflags(*global_ldflags)
