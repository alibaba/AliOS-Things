aos_global_config.set_aos_global_config('no_with_lwip','1')

src = Split('''
        soc/soc_impl.c
        soc/hook_impl.c
        soc/trace_impl.c
        soc/uart.c
        main/arg_options.c
        main/main.c
        main/hw.c
        main/wifi_port.c
        main/ota_port.c
        main/nand.c
        main/vfs_trap.c
''')
component = aos_arch_component('linuximpl', src)

### can't work start ###
comp_names = [comp.name for comp in aos_global_config.components]
if 'fatfs' in comp_names:
    component.add_sources('main/sdmmc.c')

if 'net' in comp_names:
    aos_global_config.set_aos_global_config('LWIP', '1')

LWIP = aos_global_config.get_aos_global_config('LWIP')
if LWIP == '1':
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


component.add_component_dependencis('utility/log', 'platform/arch/linux')

component.add_global_includes('include', 'csp/lwip/include')


component.add_global_macro('CONFIG_AOS_RHINO_MMREGION')
component.add_global_macro('CONFIG_YSH_CMD_DUMPSYS')
component.add_global_macro('CSP_LINUXHOST')
component.add_global_macro('CONFIG_LOGMACRO_DETAILS')
component.add_global_macro('CONFIG_AOS_FATFS_SUPPORT')
component.add_global_macro('CONFIG_AOS_FATFS_SUPPORT_MMC')
component.add_global_macro('CONFIG_AOS_FOTA_BREAKPOINT')

#move from yts component
component.add_global_ldflags('-lreadline')
component.add_global_ldflags('-lncurses')
component.set_global_arch('linux')

tool_chain = aos_global_config.create_tool_chain()

tool_chain.set_cppflags('-DSYSINFO_PRODUCT_MODEL=\\"ALI_AOS_LINUXHOST\\" -DSYSINFO_DEVICE_NAME=\\"LINUXHOST\\" -m32 -std=gnu99 -Wall -Wno-missing-field-initializers -Wno-strict-aliasing -Wno-address -Wno-unused-result -lpthread -lm -lrt -DDEBUG -ggdb')

linkcom = '$LINK -o $TARGET -Wl,-Map,$MAPFILE -Wl,--start-group $SOURCES  $LIBS  -Wl,--end-group  -Wl,--gc-sections -Wl,--cref -m32 -lpthread -lm -lrt $LINKFLAGS $LDFLAGS'
tool_chain.set_linkcom(linkcom)

tool_chain.set_cflags('')
aos_global_config.tool_chain_config(tool_chain)
    
