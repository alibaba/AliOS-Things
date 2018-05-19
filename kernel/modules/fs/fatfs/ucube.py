src = Split('''
    fatfs.c
    diskio.c
    ff/ff.c
    ff/ffunicode.c
''')

component = aos_component('fatfs', src)
component.add_cflags('-Wall')
component.add_cflags('-Werror')

component.add_global_macros('AOS_FATFS')
component.add_global_includes('include')
component.add_global_includes('ff/include')
