src     = Split('''
        ulog.c
		uring_fifo.c
''')
component = aos_component('ulog', src)

component.add_global_includes('include')
