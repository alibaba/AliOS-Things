src = Glob('*.c')
component = aos_component('libkm', src)
component.add_comp_deps('security/irot/km/platform', 'security/alicrypto')
component.add_prebuilt_libs('lib/' + component.get_arch() + '/libkm.a')

