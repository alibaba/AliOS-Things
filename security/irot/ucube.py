src = Glob('*.c')
component = aos_component('irot', src)
component.add_global_includes('include')
component.add_comp_deps('security/irot/km')
