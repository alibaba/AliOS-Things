src = Split('''
        drivers/fsl_adc.c
        drivers/fsl_clock.c
        drivers/fsl_common.c
        drivers/fsl_crc.c
        drivers/fsl_ctimer.c
        drivers/fsl_dma.c
        drivers/fsl_flashiap.c
        drivers/fsl_fmeas.c
        drivers/fsl_gint.c
        drivers/fsl_gpio.c
        drivers/fsl_i2c.c
        drivers/fsl_i2c_dma.c
        drivers/fsl_inputmux.c
        drivers/fsl_mrt.c
        drivers/fsl_pint.c
        drivers/fsl_power.c
        drivers/fsl_reset.c
        drivers/fsl_rit.c
        drivers/fsl_rtc.c
        drivers/fsl_sctimer.c
        drivers/fsl_spi.c
        drivers/fsl_spi_dma.c
        drivers/fsl_usart.c
        drivers/fsl_usart_dma.c
        drivers/fsl_utick.c
        drivers/fsl_wwdt.c
        system_LPC54102_cm4.c
        utilities/log/fsl_log.c
        utilities/str/fsl_str.c
        utilities/io/fsl_io.c
        utilities/fsl_debug_console.c
        mcuxpresso/startup_lpc5410x.c
        hal/hal_uart.c
        hal/csp_log.c
        hal/hal_flash.c
        hal/hal_wifi_wmi.c
        hal/hal_ota.c
        aos/aos.c
        aos/soc_impl.c
        hal/hook_impl.c
        wifi_qca/common_src/api_interface/api_init.c
        wifi_qca/common_src/api_interface/api_ioctl.c
        wifi_qca/common_src/api_interface/api_txrx.c
        wifi_qca/common_src/api_interface/api_wmi_rx.c
        wifi_qca/common_src/bmi/bmi.c
        wifi_qca/common_src/driver/driver_diag.c
        wifi_qca/common_src/driver/driver_init.c
        wifi_qca/common_src/driver/driver_main.c
        wifi_qca/common_src/driver/driver_netbuf.c
        wifi_qca/common_src/driver/driver_txrx.c
        wifi_qca/common_src/htc/htc.c
        wifi_qca/common_src/hcd/spi_hcd.c
        wifi_qca/common_src/hw_interface/hw_api.c
        wifi_qca/common_src/stack_common/api_stack_offload.c
        wifi_qca/common_src/qapi/qcom_api.c
        wifi_qca/common_src/qapi/qcom_legacy.c
        wifi_qca/common_src/reorder/rcv_aggr.c
        wifi_qca/common_src/storerecall/dset_api.c
        wifi_qca/common_src/storerecall/dset.c
        wifi_qca/common_src/storerecall/storerecall.c
        wifi_qca/common_src/util/util.c
        wifi_qca/common_src/wmi/wmi.c
        wifi_qca/custom_src/api_interface/cust_api_init.c
        wifi_qca/custom_src/api_interface/cust_api_ioctl.c
        wifi_qca/custom_src/api_interface/cust_api_txrx.c
        wifi_qca/custom_src/api_interface/cust_api_wmi_rx.c
        wifi_qca/custom_src/driver/cust_driver_main.c
        wifi_qca/custom_src/driver/cust_driver_netbuf.c
        wifi_qca/custom_src/hw_interface/cust_spi_hcd.c
        wifi_qca/custom_src/qapi/custom_qcom_api.c
        wifi_qca/custom_src/stack_custom/cust_api_stack_offload.c
        wifi_qca/custom_src/stack_custom/cust_api_stack_txrx.c
        wifi_qca/port/boards/lpcxpresso54102/alios/wifi_shield.c
        wifi_qca/port/boards/lpcxpresso54102/alios/wlan_qca400x.c
        wifi_qca/port/env/alios/wifi_env.c
        wifi_qca/port/drivers/spi_alios/wifi_spi.c
        wifi_qca/port/drivers/spi_alios/wifi_driver_main.c
        wifi_qca/port/pin_mux.c
''')

global_includes = Split('''
        CMSIS/Include
        drivers
        mcuxpresso
        utilities
        utilities/str
        utilities/log
        utilities/io
        #arch/arm/armv7m/gcc/m4/
        wifi_qca/common_src/include
        wifi_qca/common_src/hcd
        wifi_qca/common_src/stack_common
        wifi_qca/common_src/wmi
        wifi_qca/custom_src/include
        wifi_qca/custom_src/stack_custom
        wifi_qca/port/boards/lpcxpresso54102/alios/gt202
        wifi_qca/port/boards/lpcxpresso54102/alios
        wifi_qca/port/drivers/spi_alios
        wifi_qca/port/shields
        wifi_qca/port/env/alios
        wifi_qca/port
        wifi_qca/include
        wifi_qca/include/AR6002/hw2.0/hw
        wifi_qca/include/AR6002
        wifi_qca
''')

dependencis = Split('''
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
        frameworkd/fota
        network/sal
        osal
        kernel/init
''')

global_cflags = Split('''
        -fmessage-length=0
        -fno-builtin 
        -ffunction-sections 
        -fdata-sections 
        -fno-common 
        -std=gnu99 
        -nostdlib
        -mcpu=cortex-m4 
        -mlittle-endian 
        -mthumb 
        -mthumb-interwork 
        -march=armv7e-m
        -Wno-format 
        -Wno-incompatible-pointer-types
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

global_ldflags = Split('''
        -mcpu=cortex-m4       
        -mthumb -mthumb-interwork
        -mlittle-endian
        -nostartfiles
''')

global_macro = Split('''
        CORE_M4
        SDK_DEBUGCONSOLE=1
        RHINO_CONFIG_TASK_STACK_CUR_CHECK=1
        A_LITTLE_ENDIAN
        __MULTICORE_MASTER 
        __MULTICORE_M0SLAVE
        MULTICORE_MASTER_SLAVE_M0SLAVE
        SYSINFO_PRODUCT_MODEL=\\"ALI_AOS_LPC54102\\"
        SYSINFO_DEVICE_NAME=\\"LPC54102\\"
''')

global_asflags = Split('''
        -mthumb 
        -mcpu=cortex-m4
''')

sal = aos_global_config.get('sal', 1)
if sal == 1:
    aos_global_config.set('sal', 1)

aos_global_config.add_ld_files('lpc54102_flash_96K.ld')

module = aos_global_config.get('module', 'wifi.gt202')
if module == 1:
    aos_global_config.set('module', 'wifi.gt202')

if aos_global_config.arch == 'Cortex-M4F':
    global_ldflags.append('-L./platform/mcu/lpc54102/mcuxpresso')
    global_ldflags.append('-lfsl_power_cm4_hard')
elif aos_global_config.arch == 'Cortex-M4':
    global_ldflags.append('-L./platform/mcu/lpc54102/mcuxpresso')
    global_ldflags.append('-lfsl_power_lib_cm4')

component = aos_mcu_component('lpc54102impl', 'arm-none-eabi-', src)
component.add_comp_deps(*dependencis)
component.add_global_includes(*global_includes)
component.add_global_cflags(*global_cflags)
component.add_cflags(*local_cflags)
component.add_global_ldflags(*global_ldflags)
component.add_global_macros(*global_macro)
component.add_global_asflags(*global_asflags)
