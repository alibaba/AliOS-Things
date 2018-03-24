src     = Split('''
        yloop.c
        local_event.c
''')
component = aos_component('yloop', src)
component.add_component_dependencis('utility/log', 'kernel/vfs')
component.add_global_macro('AOS_LOOP')
