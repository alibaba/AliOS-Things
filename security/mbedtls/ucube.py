src = Split('''
    src/mbedtls_net.c
    src/mbedtls_ssl.c
    src/aes.c
    src/aesni.c
    src/arc4.c
    src/asn1write.c
    src/base64.c
    src/bignum.c
    src/blowfish.c
    src/camellia.c
    src/ccm.c
    src/cipher.c
    src/cipher_wrap.c
    src/cmac.c
    src/ctr_drbg.c
    src/debug.c
    src/des.c
    src/dhm.c
    src/ecdh.c
    src/ecdsa.c
    src/ecjpake.c
    src/ecp.c
    src/ecp_curves.c
    src/entropy.c
    src/entropy_poll.c
    src/error.c
    src/gcm.c
    src/havege.c
    src/hmac_drbg.c
    src/md2.c
    src/md4.c
    src/md.c
    src/md_wrap.c
    src/memory_buffer_alloc.c
    src/net_sockets.c
    src/oid.c
    src/padlock.c
    src/pem.c
    src/pk.c
    src/pkcs11.c
    src/pkcs12.c
    src/pkcs5.c
    src/pkparse.c
    src/pk_wrap.c
    src/pkwrite.c
    src/platform.c
    src/ripemd160.c
    src/rsa.c
    src/sha512.c
    src/ssl_cache.c
    src/ssl_ciphersuites.c
    src/ssl_cli.c
    src/ssl_cookie.c
    src/ssl_srv.c
    src/ssl_ticket.c
    src/ssl_tls.c
    src/timing.c
    src/version.c
    src/version_features.c
    src/x509.c
    src/x509_create.c
    src/x509_crl.c
    src/x509_crt.c
    src/x509_csr.c
    src/x509write_crt.c
    src/x509write_csr.c
    src/xtea.c
    src/mbedtls_alt.c
''')
    
component = aos_component('mbedtls', src)

component.add_global_includes('include')

if aos_global_config.board != 'linuxhost':
    component.add_macro('LWIP_ENABLED')

component.add_component_dependencis('security/alicrypto')



