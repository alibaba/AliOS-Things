src     = Split('''
        yloop.c
        local_event.c
''')
component = aos_component('yloop', src)
component.add_comp_deps('utility/log', 'kernel/vfs')
component.add_global_macros('AOS_LOOP')

if aos_global_config.compiler == 'armcc':
    component.add_prebuilt_objs('local_event.o')
