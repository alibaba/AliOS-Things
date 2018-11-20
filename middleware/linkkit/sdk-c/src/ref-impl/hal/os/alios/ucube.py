src =Split('''
    HAL_OS_rhino.c
    HAL_TCP_rhino.c
    HAL_PRODUCT_rhino.c
    HAL_UDP_rhino.c
    HAL_Crypt_rhino.c
    HAL_TLS_mbedtls.c
    HAL_DTLS_mbedtls.c
    HAL_AWSS_rhino.c

''')
    # HAL_TLS_mbedtls.c
    # HAL_DTLS_mbedtls.c
component =aos_component('iotx-hal', src)

dependencis =Split('''
    security/mbedtls
    utility/digest_algorithm
''')
for i in dependencis:
    component.add_comp_deps(i)

global_includes =Split('''

''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split('''
    COAP_DTLS_SUPPORT
''')
for i in global_macros:
    component.add_global_macros(i)

includes =Split('''

''')
for i in includes:
    component.add_includes(i)


cflags =Split('''

''')
for i in cflags:
    component.add_cflags(i)

component.add_global_macros('')


@post_config
def alink_ilop_post_config(component):
    comp_names = [comp.name for comp in aos_global_config.components]
    if 'ywss4linkkit' in comp_names:
        component.add_sources('HAL_AWSS_rhino.c')

alink_ilop_post_config(component)

