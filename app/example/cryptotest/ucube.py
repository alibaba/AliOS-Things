src     = Split('''
        cryptotest.c
''')
component = aos_component('cryptotest', src)

path = "../../security/alicrypto"
component.add_sources(path+"/libalicrypto/test/ali_crypto_test.c")
component.add_sources(path+"/libalicrypto/test/ali_crypto_test_comm.c")
component.add_sources(path+"/libalicrypto/test/ali_crypto_test_aes.c")
component.add_sources(path+"/libalicrypto/test/ali_crypto_test_hash.c")
component.add_sources(path+"/libalicrypto/test/ali_crypto_test_rand.c")
component.add_sources(path+"/libalicrypto/test/ali_crypto_test_rsa.c")
component.add_sources(path+"/libalicrypto/test/ali_crypto_test_hmac.c")
component.add_includes(path+"/libalicrypto/test/inc")

component.add_comp_deps('kernel/yloop', 'tools/cli', 'security/alicrypto')
component.add_global_macros('AOS_NO_WIFI')
