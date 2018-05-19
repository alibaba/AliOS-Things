src = Glob('*.c')
component = aos_component('prov', src)
component.add_comp_deps('security/plat_gen', 'security/alicrypto', 'security/libid2', 'security/libkm')
component.add_prebuilt_libs('lib/' + component.get_arch() + '/libprov.a')
component.add_global_includes('include')

