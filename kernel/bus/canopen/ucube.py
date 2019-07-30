src     = Split('''
        src/dcf.c
        src/emcy.c
        src/lifegrd.c
        src/lss.c
        src/nmtMaster.c
        src/nmtSlave.c
        src/objacces.c
        src/pdo.c
        src/sdo.c
        src/states.c
        src/sync.c
        src/timer.c
''')
component = aos_component('canopen', src)

component.add_global_includes('include')
component.add_global_macros('AOS_CANOPEN')