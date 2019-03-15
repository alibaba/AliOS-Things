import os

src = Split('''
        hal/uart.c
        aos/aos.c 
        ../../arch/csky/cskyv2-l/cpu_impl.c 
        ../../arch/csky/cskyv2-l/port_s_novic.S 
        ../../arch/csky/cskyv2-l/port_c.c 
        ../../arch/csky/cskyv2-l/entry.S 
        modules/libc/minilibc_port.c 
        csi/csi_kernel/rhino/adapter/csi_rhino.c 
        csi/csi_kernel/rhino/driver/systick.c 
        csi/csi_kernel/rhino/driver/hook_impl.c 
        hal/ringbuffer.c 
        hal/i2c.c 
        hal/flash.c 
        libs/posix/time/clock_gettime.c 
        hal/eth_port.c
''')

global_includes = Split('''
        platforj/arch/csky/cskyv2-l
        csi/csi_core/include
        include
        csi/csi_driver/include
        csi/csi_kernel/include
        csi/csi_kernel/rhino/driver
        csi/libs/include
        hal
''')

global_asflags = Split('''
        -mcpu=ck802
        -ffunction-sections
        -fdata-sections
''')

global_cflags = Split('''
        -mcpu=ck802
        -c 
        -Wa,-melrw
        -Wall
        -ffunction-sections
        -fdata-sections
        SYSINFO_PRODUCT_MODEL=\\"ALI_AOS_CKHOBBIT\\"
        SYSINFO_DEVICE_NAME=\\"CKHOBBIT\\"
''')

global_ldflags = Split('''
        -mcpu=ck802
        -Wl,-ckmap='cb2201.map'
        -lm
''')


global_macro = Split('''
        CONFIG_AOS_UOTA_BREAKPOINT
        WITH_LWIP
''')

dependencis = Split('''
        platform/arch/csky/cskyv2-l
        kernel/rhino
        kernel/hal
        kernel/init
        osal
        network/lwip
        utility/cjson
        kernel/cli
        platform/mcu/csky/hal_init
''')


if aos_global_config.get('CONFIG_HARD_FLOAT') == 1:
    global_cflags.append('-mhard-float')

if aos_global_config.get('CONFIG_HAVE_VIC') == 'y':
    global_cflags.append('-mistack')

if aos_global_config.get('HOST_CHIP') == 'zx297100':
    global_macro.append('CONFIG_HAVE_ICU')
    global_includes.append('csi/csi_driver/sanechips/zx297100/include')
    global_includes.append('csi/csi_driver/sanechips/common/include')
    chip_src = Split('''
            csi/csi_driver/sanechips/zx297100/novic_irq_tbl.c
            csi/csi_driver/sanechips/zx297100/entry.S
            csi/csi_driver/sanechips/zx297100/startup.S
            csi/csi_driver/sanechips/zx297100/system.c
            csi/csi_driver/sanechips/zx297100/isr.c
            csi/csi_driver/sanechips/zx297100/lib.c
            csi/csi_driver/sanechips/zx297100/devices.c
            csi/csi_driver/sanechips/zx297100/pinmux.c
            csi/csi_driver/sanechips/zx297100/trap_c.c
    ''')

    driver_src = Split('''
            csi/csi_driver/sanechips/common/zx29_usart.c
            csi/csi_driver/sanechips/common/zx29_gpio.c
            csi/csi_driver/sanechips/common/zx29_intc.c
            csi/csi_driver/sanechips/common/zx29_efc.c
            csi/csi_driver/sanechips/common/zx29_clk.c
            csi/csi_driver/sanechips/common/zx29_iic.c
            csi/csi_driver/sanechips/common/zx29_spi.c
            csi/csi_driver/sanechips/common/zx29_timer.c
            csi/csi_driver/sanechips/common/zx29_wdt.c
            csi/csi_driver/sanechips/common/zx29_icp.c
            csi/csi_driver/sanechips/common/zx29_rtc.c
            csi/csi_driver/sanechips/common/zx29_sleep_pmu.S
            csi/csi_driver/sanechips/common/zx29_pmu.c
            csi/csi_driver/sanechips/common/zx29_bmu.c
    ''')

    src.extend(chip_src)
    src.extend(driver_src)
else:
    global_includes.append('csi/csi_driver/csky/ch2201/include')
    global_includes.append('csi/csi_driver/csky/common/include')
    tmp_src = Split('''
            csi/csi_driver/csky/common/spi/dw_spi.c
            csi/csi_driver/csky/common/usart/dw_usart.c
            csi/csi_driver/csky/common/eflash/ck_eflash.c
            csi/csi_driver/csky/common/timer/dw_timer.c
            csi/csi_driver/csky/common/gpio/dw_gpio.c
            csi/csi_driver/csky/common/iic/dw_iic.c
            csi/csi_driver/csky/common/rtc/ck_rtc.c
            csi/csi_driver/csky/common/wdt/dw_wdt.c
            csi/csi_driver/csky/common/pwm/ck_pwm.c
            csi/csi_driver/csky/common/dmac/dw_dmac.c
            csi/csi_driver/csky/common/adc/ck_adc.c
            csi/csi_driver/csky/common/trng/ck_trng.c
            csi/csi_driver/csky/common/crc/ck_crc.c
            csi/csi_driver/csky/common/aes/ck_aes.c
            csi/csi_driver/csky/common/rsa/ck_rsa.c
            csi/csi_driver/csky/common/sha/ck_sha_v1.c
            csi/csi_driver/csky/ch2201/startup.S
            csi/csi_driver/csky/ch2201/vectors.S
            csi/csi_driver/csky/ch2201/system.c
            csi/csi_driver/csky/ch2201/isr.c
            csi/csi_driver/csky/ch2201/lib.c
            csi/csi_driver/csky/ch2201/devices.c
            csi/csi_driver/csky/ch2201/pinmux.c
            csi/csi_driver/csky/ch2201/trap_c.c
            csi/csi_driver/csky/ch2201/ck_sys_freq.c
            csi/csi_driver/csky/ch2201/novic_irq_tbl.c
            csi/drivers/eth/csi_eth_enc28j60.c
            csi/libs/libc/malloc.c
        ''')
    src.extend(tmp_src)


component = aos_mcu_component('csky', 'csky-abiv2-elf-', src)
component.add_global_includes(*global_includes)
component.add_comp_deps(*dependencis)

for cflag in global_cflags:
    component.add_global_cflags(cflag)

for asflag in global_asflags:
    component.add_global_asflags(asflag)

for ldflag in global_ldflags:
    component.add_global_ldflags(ldflag)

for macro in global_macro:
    component.add_global_macros(macro)
