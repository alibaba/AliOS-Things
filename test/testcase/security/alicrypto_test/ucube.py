src = Split('''
    alicrypto_test.c
''')
path = "../../../../security/alicrypto"
component = aos_component('alicrypto_test', src)
component.add_comp_deps('security/alicrypto')
component.add_sources(path+"/libalicrypto/test/ali_crypto_test.c")
component.add_sources(path+"/libalicrypto/test/ali_crypto_test_comm.c")
component.add_sources(path+"/libalicrypto/test/ali_crypto_test_aes.c")
component.add_sources(path+"/libalicrypto/test/ali_crypto_test_hash.c")
component.add_sources(path+"/libalicrypto/test/ali_crypto_test_rand.c")
component.add_sources(path+"/libalicrypto/test/ali_crypto_test_rsa.c")
component.add_sources(path+"/libalicrypto/test/ali_crypto_test_hmac.c")
component.add_includes(path+"/libalicrypto/test/inc")
