src = []
component = aos_component('ywss', src)
component.add_global_includes('./')
component.add_global_macros('CONFIG_YWSS')

libpath = '/'.join(['lib', component.get_arch(), component.get_compiler, 'ywss.a'])
component.add_prebuilt_libs(libpath)

