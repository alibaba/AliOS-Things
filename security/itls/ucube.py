src = Split('''
    src/mbedtls_net.c
    src/mbedtls_alt.c
    src/mbedtls_auth.c
''')

aos_global_config.set("DEBUG", "yes")

component = aos_component('itls', src)

component.add_global_includes('include')

component.add_macros('CONFIG_PLAT_AOS')
component.add_macros('CONFIG_KEY_OTP_ENABLED')

if aos_global_config.get("DEBUG")!="no":
    component.add_global_macros("CONFIG_SSL_DEBUG")

if aos_global_config.board != 'linuxhost':
    component.add_macros('LWIP_ENABLED')

component.add_prebuilt_libs('lib/' + component.get_arch() + '/libitls.a')

component.add_comp_deps('security/alicrypto')
component.add_comp_deps('security/id2')

