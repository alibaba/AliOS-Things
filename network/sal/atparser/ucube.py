src     = Split('''
        atparser.c
''')

component = aos_component('atparser', src)

component.add_global_includes('./')

component.add_global_macros('AOS_ATCMD')

with_atparser = aos_global_config.get('with_atparser', 1)
if with_atparser == 1:
    aos_global_config.set('with_atparser', 1)