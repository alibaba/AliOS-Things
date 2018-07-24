src     = Split('''
        prov_app.c
''')

component = aos_component('prov_app', src)
component.add_comp_deps('kernel/yloop', 'tools/cli', 'security/prov', 'security/prov/test')
component.add_global_macros('AOS_NO_WIFI')
