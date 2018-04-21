src     = Split('''
        startup.s
        main.c 
''')

component = aos_component('mx1101_sdk', src)
component.add_includes('CMSIS/Include', 'O18B_SDK_GCC/MVs_new/MVs18_SDK/Libraries/driver/inc')
component.add_prebuilt_libs('O18B_SDK_GCC/lib/libdriver.a')
aos_global_config.add_ld_files('link.ld')

