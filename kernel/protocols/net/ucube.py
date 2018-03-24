src = Split('''
        port/sys_arch.c
''')

core_src = Split('''
        core/init.c
        core/def.c
        core/dns.c
        core/inet_chksum.c
        core/ip.c
        core/mem.c
        core/memp.c
        core/netif.c
        core/pbuf.c
        core/raw.c
        core/stats.c
        core/sys.c
        core/tcp.c
        core/tcp_in.c
        core/tcp_out.c
        core/timeouts.c
        core/udp.c
''')

core_tcp4_src = Split('''
        core/ipv4/autoip.c
        core/ipv4/dhcp.c
        core/ipv4/etharp.c
        core/ipv4/icmp.c
        core/ipv4/igmp.c
        core/ipv4/ip4_frag.c
        core/ipv4/ip4.c
        core/ipv4/ip4_addr.c
''')

core_tcp6_src = Split('''
        core/ipv6/dhcp6.c
        core/ipv6/ethip6.c
        core/ipv6/icmp6.c
        core/ipv6/inet6.c
        core/ipv6/ip6.c
        core/ipv6/ip6_addr.c
        core/ipv6/ip6_frag.c
        core/ipv6/mld6.c
        core/ipv6/nd6.c
''')

api_src = Split('''
        api/api_lib.c
        api/api_msg.c
        api/err.c
        api/netbuf.c
        api/netdb.c
        api/netifapi.c
        api/sockets.c
        api/tcpip.c
''')

net_if_src = Split('''
        netif/ethernet.c
        netif/slipif.c
''')

tftp_src = Split('''
        apps/tftp/tftp_server.c
        apps/tftp/tftp_common.c
        apps/tftp/tftp_client.c
        apps/tftp/tftp_ota.c
''')

src.extend(core_src)
src.extend(core_tcp4_src)
src.extend(core_tcp6_src)
src.extend(api_src)
src.extend(net_if_src)
src.extend(tftp_src)

component = aos_component('net', src)
component.add_global_includes('include', 'port/include')

if aos_global_config.get_aos_global_config('no_with_lwip') != '1':
    component.add_global_macro('WITH_LWIP')
    aos_global_config.set_aos_global_config('with_lwip','1')

component.add_global_macro('CONFIG_NET_LWIP')
