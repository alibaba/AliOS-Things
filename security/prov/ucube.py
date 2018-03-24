src = Glob('*.c')
component = aos_component('prov', src)
component.add_component_dependencis('security/plat_gen', 'security/alicrypto', 'security/libid2', 'security/libkm')
component.add_prebuilt_lib('lib/' + component.get_global_arch() + '/libprov.a')
component.add_global_includes('include')

