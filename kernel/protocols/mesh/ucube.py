src = []
component = aos_component('mesh', src)
component.add_global_includes('include')
component.add_global_macros('CONFIG_AOS_MESH')

libpath = '/'.join(['lib', component.get_arch(), component.get_compiler, 'mesh.a'])
component.add_prebuilt_libs(libpath)

