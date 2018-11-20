src     = Split('''
        qr.c
        decode.c
        identify.c
        quirc.c
        version_db.c
        qrcodegen.c
''')

component = aos_component('qr', src)
component.add_comp_deps('kernel/yloop', 'kernel/cli', 'board.developerkit.camera_hal')
component.add_global_macros('AOS_NO_WIFI','DK_CAMERA_SNAPSHOP','CAMERA_GRAY_MIRROR')
