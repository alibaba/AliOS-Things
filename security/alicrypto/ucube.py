src = Split('''
        ./libalicrypto/mbed/cipher/aes.c
        ./libalicrypto/mbed/hash/hash.c
        ./libalicrypto/mbed/asym/rsa.c
        ./libalicrypto/mbed/mac/hmac.c 		
        ./libalicrypto/sw/ali_crypto_rand.c
        ./libalicrypto/ali_crypto.c 			
        ./mbedtls/library/aes.c
        ./mbedtls/library/md5.c 						
        ./mbedtls/library/sha1.c 					
        ./mbedtls/library/sha256.c 					
        ./mbedtls/library/hash_wrap.c 				
        ./mbedtls/library/bignum.c 					
        ./mbedtls/library/oid.c 						
        ./mbedtls/library/rsa.c 						
        ./mbedtls/library/threading.c 				
        ./mbedtls/library/mbedtls_alt.c 				
        ./mbedtls/library/asn1parse.c 				
        ./mbedtls/library/hmac.c 					
''')


gincs = Split('''
        ./libalicrypto/inc
''')

comp = aos_component('alicrypto', src)
comp.add_global_includes(*gincs)
comp.add_macros('CONFIG_CRYPT_MBED=1')
comp.add_macros('CONFIG_DBG_CRYPT=1')
if aos_global_config.get('osal') == 'winmsvs':
    comp.add_macros('MBEDTLS_PLATFORM_C')
comp.add_global_macros('CONFIG_ALICRYPTO')

