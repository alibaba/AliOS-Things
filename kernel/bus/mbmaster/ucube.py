src     = Split('''
        api/api_readholdingregisters.c
        pdu/readholdingregisters.c
        adu/rtu/rtu.c
        adu/rtu/mbcrc.c
        physical/serial.c
        auxiliary/log.c
        auxiliary/other.c
        api/mbm.c
''')
component = aos_component('mbmaster', src)

component.add_global_includes('include')
component.add_global_macros('AOS_COMP_MBMASTER=1')
