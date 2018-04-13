src     = Split('''
        yloop.c
        local_event.c
''')
component = aos_component('yloop', src)
component.add_comp_deps('utility/log', 'kernel/vfs')
component.add_global_macros('AOS_LOOP')
