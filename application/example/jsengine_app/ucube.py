src     = Split('''
        jsengine_app.c
''')

component = aos_component('jsengine_app', src)
component.add_comp_deps('kernel/yloop', 'tools/cli')
