src     = Split('''
        cameraapp.c
''')

component = aos_component('cameraapp', src)
component.add_comp_deps('kernel/yloop', 'kernel/cli', '3rdparty.experimental.fs.fatfs', 'board.developerkit.camera_hal')
component.add_global_macros('AOS_NO_WIFI')
