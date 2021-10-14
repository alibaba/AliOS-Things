src     = Split('''
        crc16.c
	crc32.c
''')
component = aos_component('crc', src)

component.add_global_includes('include')
