src = Glob('*.c')
component = aos_component('libid2', src)
component.add_global_includes('include')
component.add_component_dependencis('security/plat_gen', 'security/libkm')
component.add_global_macro('CONFIG_AOS_SUPPORT=1')
component.add_prebuilt_lib('lib/' + component.get_global_arch() + '/libid2.a')

