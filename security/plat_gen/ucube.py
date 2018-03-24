src = Glob('*.c')
component = aos_component('plat_gen', src)
component.add_global_includes('include')
component.add_prebuilt_lib('lib/' + component.get_global_arch() + '/libplat_gen.a')

