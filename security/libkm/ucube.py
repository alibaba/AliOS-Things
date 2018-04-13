src = Glob('*.c')
component = aos_component('libkm', src)
component.add_global_includes('include')
component.add_comp_deps('security/plat_gen', 'security/alicrypto')
component.add_prebuilt_libs('lib/' + component.get_arch() + '/libkm.a')

