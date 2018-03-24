src = []
component = aos_component('tfs', src)

component.add_global_includes('include')
component.add_cflags( '-Wall')
component.add_cflags( '-Werror')
component.add_cflags( '-Os')

host_arch = component.get_global_arch()
component.add_prebuilt_lib('lib/'+host_arch+'/libtfs.a')
component.add_prebuilt_lib('lib/'+host_arch+'/libtfspal.a')