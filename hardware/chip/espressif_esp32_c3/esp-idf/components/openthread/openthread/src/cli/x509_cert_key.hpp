/*
 *  Copyright (c) 2018, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * @brief
 *  This file contains the X.509 certificate and private key for Application
 *  CoAP Secure use with cipher suite ECDHE_ECDSA_WITH_AES_128_CCM8.
 */

#ifndef SRC_CLI_X509_CERT_KEY_HPP_
#define SRC_CLI_X509_CERT_KEY_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
#if defined(MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED)

/**SERVER
 * Generate your own private key and certificate using openssl.
 *
 * 1. Generate a EC (Elliptic Curve) Private Key.
 *      'openssl ecparam -genkey -out myECKey.pem -name prime256v1 -noout'
 * 2. Generate a .X509 Certificate (Contains Public Key).
 *      'openssl req -x509 -new -key myECKey.pem -out myX509Cert.pem'
 */

/**
 * place your X.509 certificate (PEM format) for ssl session
 * with ECDHE_ECDSA_WITH_AES_128_CCM_8 here.
 */
#define OT_CLI_COAPS_X509_CERT                                             \
    "-----BEGIN CERTIFICATE-----\r\n"                                      \
    "MIIBrTCCAVICBgDRArfDJTAKBggqhkjOPQQDAjBcMQswCQYDVQQGEwJaWTESMBAG\r\n" \
    "A1UECAwJWW91clN0YXRlMRAwDgYDVQQKDAdZb3VyT3JnMRQwEgYDVQQLDAtZb3Vy\r\n" \
    "T3JnVW5pdDERMA8GA1UEAwwIVmVuZG9yQ0EwIBcNMTgwNzEzMTIzNzA3WhgPMjI5\r\n" \
    "MjA0MjYxMjM3MDdaMGExCzAJBgNVBAYTAlpZMRIwEAYDVQQIDAlZb3VyU3RhdGUx\r\n" \
    "EDAOBgNVBAoMB1lvdXJPcmcxFDASBgNVBAsMC1lvdXJPcmdVbml0MRYwFAYDVQQD\r\n" \
    "DA1QWEMzLkU3NS0xMDBBMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEIUtlV99w\r\n" \
    "OggiASflg6CVsGMzXMXYrNgQ1piLIybCkrq+YoqJ3mwcbJHWlvxGPxNIQw6i8kzK\r\n" \
    "bkC642ZWgBT5MzAKBggqhkjOPQQDAgNJADBGAiEA/1yk69A+37kLBvdOWPDRXGwe\r\n" \
    "0AoICTGaLqzB3cF5mtACIQC28WwmzHb5gqe3nOPAM73py1v17EXZj07PU89BAEcb\r\n" \
    "yg==\r\n"                                                             \
    "-----END CERTIFICATE-----\r\n"

/**
 * place your private key (PEM format) for ssl session
 * with ECDHE_ECDSA_WITH_AES_128_CCM_8 here.
 */
#define OT_CLI_COAPS_PRIV_KEY                                              \
    "-----BEGIN EC PRIVATE KEY-----\r\n"                                   \
    "MHcCAQEEIFYQh2R7M48qOHePw+VE4b034UlZmWWC/iNAK34sQbucoAoGCCqGSM49\r\n" \
    "AwEHoUQDQgAEIUtlV99wOggiASflg6CVsGMzXMXYrNgQ1piLIybCkrq+YoqJ3mwc\r\n" \
    "bJHWlvxGPxNIQw6i8kzKbkC642ZWgBT5Mw==\r\n"                             \
    "-----END EC PRIVATE KEY-----\r\n"

/**
 * Place peers CA certificate (PEM format) here.
 * It's necessary to validate the peers certificate. If you haven't a
 * CA certificate, you must run the coaps without checking certificate.
 */
#define OT_CLI_COAPS_TRUSTED_ROOT_CERTIFICATE                              \
    "-----BEGIN CERTIFICATE-----\r\n"                                      \
    "MIICDzCCAbWgAwIBAgIESZYC0jAKBggqhkjOPQQDAjBcMQswCQYDVQQGEwJaWTES\r\n" \
    "MBAGA1UECAwJWW91clN0YXRlMRAwDgYDVQQKDAdZb3VyT3JnMRQwEgYDVQQLDAtZ\r\n" \
    "b3VyT3JnVW5pdDERMA8GA1UEAwwIVmVuZG9yQ0EwIBcNMTgwNzEzMTE1NjA5WhgP\r\n" \
    "MjI5MjA0MjYxMTU2MDlaMFwxCzAJBgNVBAYTAlpZMRIwEAYDVQQIDAlZb3VyU3Rh\r\n" \
    "dGUxEDAOBgNVBAoMB1lvdXJPcmcxFDASBgNVBAsMC1lvdXJPcmdVbml0MREwDwYD\r\n" \
    "VQQDDAhWZW5kb3JDQTBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABGAAuYcBIgP0\r\n" \
    "fMC1Bd+1nAH5S1goR0TaDAIadK4hULQr5LwziuDk9XTQaOTwmWB9iR1eiHC6RY8W\r\n" \
    "wyrGBbnEbzujYzBhMB0GA1UdDgQWBBQ+yCpIszhzbmXe2At1GofREjnBxjAfBgNV\r\n" \
    "HSMEGDAWgBQ+yCpIszhzbmXe2At1GofREjnBxjAPBgNVHRMBAf8EBTADAQH/MA4G\r\n" \
    "A1UdDwEB/wQEAwIBhjAKBggqhkjOPQQDAgNIADBFAiBW60XgdSRD24rbTgdneS+V\r\n" \
    "SHVix8LuXunPYW50LmxbrwIhAOw4gMroRIOS26y0TcND03FnyO3wBNF9MjM0hWKQ\r\n" \
    "JXk3\r\n"                                                             \
    "-----END CERTIFICATE-----\r\n"

#endif // MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#endif // OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* SRC_CLI_X509_CERT_KEY_HPP_ */
