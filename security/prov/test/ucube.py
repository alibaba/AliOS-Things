src = Glob('*.c')
component = aos_component('libprov_test', src)
component.add_global_includes('inc')
component.add_prebuilt_libs('lib/' + component.get_arch() + '/libprov_test.a')

