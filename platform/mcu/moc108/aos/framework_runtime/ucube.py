src     = Split('''
        framework_runtime.c
''')

component = aos_component('framework_runtime', src)

#TODO:GLOBAL_LDFLAGS += -uframework_info
