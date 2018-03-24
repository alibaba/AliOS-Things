src = Glob('*.c')
component = aos_component('libkm', src)
component.add_global_includes('include')
component.add_component_dependencis('security/plat_gen', 'security/alicrypto')
component.add_prebuilt_lib('lib/' + component.get_global_arch() + '/libkm.a')

