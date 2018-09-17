src = Split('''
        src/osa/aos/sst_osa.c
        src/wrapper/km/sst_wrapper.c
		src/sst.c
''')
component =aos_component('libsst', src)

aos_global_config.set("SST_DBG", "N")
aos_global_config.set("ID2_SUPPORT", "Y")

if aos_global_config.get("SST_DBG")=="Y":
    component.add_macros('CONFIG_SST_DBG')
if aos_global_config.get("ID2_SUPPORT")=="Y":
    component.add_macros('CONFIG_ID2_SUPPORT=1')

incs = Split('''
    src
    src/wrapper/inc
    src/osa/inc
''')

component.add_global_includes('include')
component.add_includes(*incs)

component.add_comp_deps('security/irot', 'security/alicrypto', 'kernel/rhino/fs/kv')


