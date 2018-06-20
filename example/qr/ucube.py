src     = Split('''
        qr.c
        decode.c
        identify.c
        quirc.c
        version_db.c
''')

component = aos_component('qr', src)
component.add_comp_deps('kernel/yloop', 'tools/cli', 'board.developerkit.camera_hal')
component.add_global_macros('AOS_NO_WIFI')
