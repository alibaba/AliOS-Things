src     = Split('''
        wifihalapp.c
''')

component = aos_component('wifihalapp', src)
component.add_comp_deps('kernel/yloop', 'tools/cli')
