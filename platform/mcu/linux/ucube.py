aos_global_config.set('no_with_lwip', 1)

src = Split('''
    soc/uart.c
    main/arg_options.c
    main/main.c
    main/hw.c
    main/wifi_port.c
    main/nand.c
    main/vfs_trap.c
''')

global_cflags = Split('''
    -m32
    -std=gnu99
    -Wall
    -Wno-missing-field-initializers
    -Wno-strict-aliasing -Wno-address
    -Wno-unused-result
    -lpthread
    -lm
    -lrt
    -DDEBUG
    -ggdb
''')

global_macros = Split('''
    SYSINFO_PRODUCT_MODEL=\\"ALI_AOS_LINUXHOST\\"
    SYSINFO_DEVICE_NAME=\\"LINUXHOST\\"
    CONFIG_AOS_RHINO_MMREGION
    CONFIG_YSH_CMD_DUMPSYS
    CSP_LINUXHOST
    CONFIG_LOGMACRO_DETAILS
    CONFIG_AOS_FATFS_SUPPORT
    CONFIG_AOS_FATFS_SUPPORT_MMC
    CONFIG_AOS_FOTA_BREAKPOINT
''')

component = aos_mcu_component('linuximpl', '', src)

component.set_global_arch('linux')

component.add_global_cflags(*global_cflags)
component.add_global_asflags('-m32')
component.add_global_ldflags('-m32', '-lpthread', '-lm', '-lrt', '-lreadline', '-lncurses')
component.add_global_macros(*global_macros)

@post_config
def linuximpl_post_config(component):
    comp_names = [comp.name for comp in aos_global_config.components]
    if 'fatfs' in comp_names:
        component.add_sources('main/sdmmc.c')

    if 'net' in comp_names:
        aos_global_config.set('LWIP', 1)


linuximpl_post_config(component)

LWIP = aos_global_config.get('LWIP')
if LWIP == 1:
    lwip_src = Split('''
        csp/lwip/netif/delif.c 
        csp/lwip/netif/fifo.c 
        csp/lwip/netif/list.c 
        csp/lwip/netif/tapif.c 
        csp/lwip/netif/tcpdump.c 
        csp/lwip/netif/tunif.c
        csp/lwip/lwip_linuxhost.c
    ''')
    for s in lwip_src:
        component.add_sources(s)
### can't work end ###

if aos_global_config.app == 'yts':
    src_tmp = Split('''
        main/sdmmc.c
        csp/lwip/netif/delif.c
        csp/lwip/netif/fifo.c
        csp/lwip/netif/list.c
        csp/lwip/netif/tapif.c
        csp/lwip/netif/tcpdump.c 
        csp/lwip/netif/tunif.c
        csp/lwip/lwip_linuxhost.c
    ''')
    for s in src_tmp:
        component.add_sources(s)
		
if aos_global_config.get('vcall') == 'posix':
	component.add_macros("CONFIG_VCALL_POSIX") 
else:
	src_tmp = Split('''
		soc/soc_impl.c
		soc/hook_impl.c
		soc/trace_impl.c
	''')
	for s in src_tmp:
		component.add_sources(s)

component.add_comp_deps('utility/log', 'platform/arch/linux', 'kernel/vcall', 'kernel/init')

component.add_global_includes('include', 'csp/lwip/include')




    
