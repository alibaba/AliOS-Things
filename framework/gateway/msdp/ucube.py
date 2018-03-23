src = Split('''
        msdp.c
        msdp_common.c
        msdp_zigbee.c
        msdp_gateway.c
''')

component = aos_component('msdp', src)
