src =Split(''' 
    tls_client.c
''')
component =aos_component('tls_client', src)

aos_global_config.set("DTLS", "no")

if aos_global_config.get("DTLS")=="yes":
    component.add_macros("DTLS_ENABLED")
    component.add_sources("dtls_client.c")

dependencis =Split(''' 
    security/mbedtls
    security/alicrypto
    network/netmgr
''')
for i in dependencis:
    component.add_comp_deps(i)





