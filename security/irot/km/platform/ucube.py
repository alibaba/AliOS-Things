src = Glob('*.c')
component = aos_component('libplat_gen', src)
component.add_global_includes('include')
component.add_comp_deps('security/alicrypto')
component.add_prebuilt_libs('lib/' + component.get_arch() + '/libplat_gen.a')

