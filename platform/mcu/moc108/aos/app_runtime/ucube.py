src     = Split('''
        app_runtime.c
''')

component = aos_component('app_runtime', src)

#TODO:GLOBAL_LDFLAGS += -uapp_info
