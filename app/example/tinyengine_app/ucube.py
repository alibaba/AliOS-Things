src     = Split('''
        tinyengine_app.c
''')

component = aos_component('tinyengine_app', src)
component.add_comp_deps('kernel/yloop', 'tools/cli')
