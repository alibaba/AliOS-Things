aos_global_config.set('no_with_lwip', 1)

src = Split('''
    soc/uart.c
    main/arg_options.c
    main/main.c
    main/hw.c
    main/wifi_port.c
    main/ota_port.c
    main/vfs_trap.c
''')

global_cflags = Split('''
    -DDEBUG
''')

global_macros = Split('''
    SYSINFO_PRODUCT_MODEL=\\"ALI_AOS_WIN32HOST\\"
    SYSINFO_DEVICE_NAME=\\"WIN32HOST\\"
    CONFIG_AOS_RHINO_MMREGION
    CONFIG_YSH_CMD_DUMPSYS
    CSP_LINUXHOST
    CONFIG_LOGMACRO_DETAILS
    CONFIG_AOS_FATFS_SUPPORT
    CONFIG_AOS_FATFS_SUPPORT_MMC
    CONFIG_AOS_UOTA_BREAKPOINT
    __LONG_LONG_MAX__=_I64_MAX
    poll=WSAPoll    
    O_NONBLOCK=1
    F_GETFL=3
    F_SETFL=4
    list_entry=CONTAINING_RECORD	
''')

component = aos_mcu_component('windows', '', src)

#component.set_global_arch('windows')

component.add_global_cflags(*global_cflags)
component.add_global_asflags('-m32')
component.add_global_ldflags('-lws2_32')
component.add_global_macros(*global_macros)


component.add_macros("CONFIG_OSAL_MSVS")

component.add_comp_deps('utility/log', 'osal', 'kernel/init')

component.add_global_includes('include')




    
