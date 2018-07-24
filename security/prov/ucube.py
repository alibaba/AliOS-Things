src = Glob('*.c')
component = aos_component('libprov', src)
component.add_comp_deps('security/alicrypto', 'security/irot')
component.add_prebuilt_libs('lib/' + component.get_arch() + '/libprov.a')
component.add_global_includes('include')

