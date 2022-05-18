#
#    Copyright 2016 The OpenThread Authors. All Rights Reserved.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#

include $(abs_top_nlbuild_autotools_dir)/automake/pre.am

EXTRA_DIST                                    = \
    mbedtls-config.h                            \
    repo/include                                \
    $(NULL)

lib_LIBRARIES                                 = \
    libmbedcrypto.a                             \
    libmbedcrypto-radio.a                       \
    $(NULL)

# Do not enable -Wconversion for mbedtls
override CFLAGS                              := $(filter-out -Wconversion,$(CFLAGS))
override CXXFLAGS                            := $(filter-out -Wconversion,$(CXXFLAGS))

# Do not enable -pedantic-errors for mbedtls
override CFLAGS                              := $(filter-out -pedantic-errors,$(CFLAGS))
override CXXFLAGS                            := $(filter-out -pedantic-errors,$(CXXFLAGS))

# Do not enable -Wcast-align for mbedtls
override CFLAGS                              := $(filter-out -Wcast-align,$(CFLAGS))
override CXXFLAGS                            := $(filter-out -Wcast-align,$(CXXFLAGS))

MBEDTLS_SRCDIR                                = $(top_srcdir)/third_party/mbedtls/repo

libmbedcrypto_a_CPPFLAGS                      = \
    -I$(top_srcdir)/include                     \
    -I$(top_srcdir)/src/core                    \
    -I$(MBEDTLS_SRCDIR)/include                 \
    $(MBEDTLS_CPPFLAGS)                         \
    $(NULL)

libmbedcrypto_a_SOURCES                       = \
    repo/library/aes.c                          \
    repo/library/aesni.c                        \
    repo/library/arc4.c                         \
    repo/library/aria.c                         \
    repo/library/asn1parse.c                    \
    repo/library/asn1write.c                    \
    repo/library/base64.c                       \
    repo/library/bignum.c                       \
    repo/library/blowfish.c                     \
    repo/library/camellia.c                     \
    repo/library/ccm.c                          \
    repo/library/certs.c                        \
    repo/library/chacha20.c                     \
    repo/library/chachapoly.c                   \
    repo/library/check_crypto_config.h          \
    repo/library/cipher.c                       \
    repo/library/cipher_wrap.c                  \
    repo/library/cmac.c                         \
    repo/library/common.h                       \
    repo/library/ctr_drbg.c                     \
    repo/library/debug.c                        \
    repo/library/des.c                          \
    repo/library/dhm.c                          \
    repo/library/ecdh.c                         \
    repo/library/ecdsa.c                        \
    repo/library/ecjpake.c                      \
    repo/library/ecp.c                          \
    repo/library/ecp_curves.c                   \
    repo/library/entropy.c                      \
    repo/library/entropy_poll.c                 \
    repo/library/error.c                        \
    repo/library/gcm.c                          \
    repo/library/havege.c                       \
    repo/library/hkdf.c                         \
    repo/library/hmac_drbg.c                    \
    repo/library/md.c                           \
    repo/library/md2.c                          \
    repo/library/md4.c                          \
    repo/library/md5.c                          \
    repo/library/memory_buffer_alloc.c          \
    repo/library/net_sockets.c                  \
    repo/library/nist_kw.c                      \
    repo/library/oid.c                          \
    repo/library/padlock.c                      \
    repo/library/pem.c                          \
    repo/library/pk.c                           \
    repo/library/pk_wrap.c                      \
    repo/library/pkcs11.c                       \
    repo/library/pkcs12.c                       \
    repo/library/pkcs5.c                        \
    repo/library/pkparse.c                      \
    repo/library/pkwrite.c                      \
    repo/library/platform.c                     \
    repo/library/platform_util.c                \
    repo/library/poly1305.c                     \
    repo/library/psa_crypto.c                   \
    repo/library/psa_crypto_core.h              \
    repo/library/psa_crypto_driver_wrappers.c   \
    repo/library/psa_crypto_driver_wrappers.h   \
    repo/library/psa_crypto_invasive.h          \
    repo/library/psa_crypto_its.h               \
    repo/library/psa_crypto_se.c                \
    repo/library/psa_crypto_se.h                \
    repo/library/psa_crypto_slot_management.c   \
    repo/library/psa_crypto_slot_management.h   \
    repo/library/psa_crypto_storage.c           \
    repo/library/psa_crypto_storage.h           \
    repo/library/psa_its_file.c                 \
    repo/library/ripemd160.c                    \
    repo/library/rsa.c                          \
    repo/library/rsa_internal.c                 \
    repo/library/sha1.c                         \
    repo/library/sha256.c                       \
    repo/library/sha512.c                       \
    repo/library/ssl_cache.c                    \
    repo/library/ssl_ciphersuites.c             \
    repo/library/ssl_cli.c                      \
    repo/library/ssl_cookie.c                   \
    repo/library/ssl_invasive.h                 \
    repo/library/ssl_msg.c                      \
    repo/library/ssl_srv.c                      \
    repo/library/ssl_ticket.c                   \
    repo/library/ssl_tls.c                      \
    repo/library/ssl_tls13_keys.c               \
    repo/library/ssl_tls13_keys.h               \
    repo/library/threading.c                    \
    repo/library/timing.c                       \
    repo/library/version.c                      \
    repo/library/version_features.c             \
    repo/library/x509.c                         \
    repo/library/x509_create.c                  \
    repo/library/x509_crl.c                     \
    repo/library/x509_crt.c                     \
    repo/library/x509_csr.c                     \
    repo/library/x509write_crt.c                \
    repo/library/x509write_csr.c                \
    repo/library/xtea.c                         \
    $(NULL)

libmbedcrypto_radio_a_CPPFLAGS                = \
    -I$(top_srcdir)/include                     \
    -I$(top_srcdir)/src/core                    \
    -I$(MBEDTLS_SRCDIR)/include                 \
    $(MBEDTLS_CPPFLAGS)                         \
    $(NULL)

libmbedcrypto_radio_a_SOURCES                 = \
    repo/library/aes.c                          \
    repo/library/platform_util.c                \
    $(NULL)

if OPENTHREAD_BUILD_COVERAGE
Dash                                          = -
CLEANFILES                                    = $(shell find $(top_builddir)/third_party/mbedtls $(Dash)name "*.gcda" $(Dash)o $(Dash)name "*.gcno")
endif # OPENTHREAD_BUILD_COVERAGE

include $(abs_top_nlbuild_autotools_dir)/automake/post.am
