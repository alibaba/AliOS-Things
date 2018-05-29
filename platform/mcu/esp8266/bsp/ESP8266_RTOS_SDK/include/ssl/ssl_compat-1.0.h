/*
 * ssl_compat-1.0.h
 *
 *  Created on: Sep 7, 2015
 *      Author: liuhan
 */

#ifndef SSL_COMPAT_1_0_H_
#define SSL_COMPAT_1_0_H_

#include "ssl/ssl_platform.h"

/*encapsulation the function based on the espressif platform*/

#define SSL_library_init(a)						esp_ssl_library_init(a)
#define SSL_new(a) 								esp_ssl_new(a)
#define SSL_set_fd(a,b)							esp_ssl_set_fd(a,b)
#define SSL_free(a)								esp_ssl_free(a)
#define SSL_connect(a)							esp_ssl_connect(a)
#define SSL_accept(a)							esp_ssl_accept(a)
#define SSL_read(a,b,c)							esp_ssl_read(a,b,c)
#define SSL_write(a,b,c)						esp_ssl_write(a,b,c)
#define SSL_get_peer_certificate(a)				esp_ssl_get_peer_certificate(a)
#define SSL_get_verify_result(a)				esp_ssl_get_verify_result(a)
#define SSL_get_error(a,b)						esp_ssl_get_error(a,b)
#define SSL_pending(a)							esp_ssl_pending(a)
#define SSL_fragment_length_negotiation(a,b)	esp_ssl_fragment_length_negotiation(a,b)

#define SSL_CTX_new(a) 							esp_ssl_CTX_new(a)
#define SSL_CTX_set_option(a,b)					esp_ssl_CTX_set_option(a,b)
#define SSL_CTX_free(a)							esp_ssl_CTX_free(a)
#define SSL_CTX_load_verify_locations(a,b,c)	esp_ssl_CTX_load_verify_locations(a,b,c)
#define SSL_CTX_set_default_verify_paths(a)		esp_ssl_CTX_set_default_verify_paths(a)
#define SSL_CTX_use_certificate_chain_file(a,b)	esp_ssl_CTX_use_certificate_chain_file(a,b)
#define SSL_CTX_use_PrivateKey_file(a,b,c)		esp_ssl_CTX_use_PrivateKey_file(a,b,c)
#define SSL_CTX_check_private_key(a)			esp_ssl_CTX_check_private_key(a)
#define SSL_CTX_set_verify(a,b,c)				esp_ssl_CTX_set_verify(a)
#define SSL_CTX_set_verify_depth(a,b)			esp_ssl_CTX_set_verify_depth(a)
#define SSL_CTX_set_client_cert_cb 				esp_ssl_CTX_set_client_cert_cb
#define SSL_CTX_set_mode(a)						esp_ssl_CTX_set_mode(a)

#define X509_free(a)							esp_X509_free(a)
#define X509_STORE_CTX_get_current_cert(a)		esp_X509_store_ctx_get_current_cert(a)
#define X509_NAME_oneline(a,b,c)				esp_X509_NAME_oneline(a,b,c)
#define X509_get_issuer_name(a)					esp_X509_get_issuer_name(a)
#define X509_get_subject_name(a)				esp_X509_get_subject_name(a)
#define X509_STORE_CTX_get_error_depth(a)		esp_X509_STORE_CTX_get_error_depth(a)
#define X509_STORE_CTX_get_error(a)				esp_X509_STORE_CTX_get_error(a)
#define X509_verify_cert_error_string(a)		esp_X509_verify_cert_error_string(a)

#define EVP_sha1(a)								esp_EVP_sha1(a)
#define EVP_DigestInit(a,b)						esp_EVP_DigestInit(a,b)
#define EVP_DigestUpdate(a,b,c)					esp_EVP_DigestUpdate(a,b,c)
#define EVP_DigestFinal(a,b,c)					esp_EVP_DigestFinal(a,b,c)
#define EVP_cleanup(a)							esp_EVP_cleanup(a)

#define ERR_get_error(a)						esp_ERR_get_error(a)
#define ERR_error_string_n(a,b,c)				esp_ERR_error_string_n(a,b,c)
#define ERR_error_string(a,b)					esp_ERR_error_string(a,b)
#define ERR_free_strings(a)						esp_ERR_free_strings(a)
#define strerror(a)								esp_ERR_strerror(a)

#define CRYPTO_cleanup_all_ex_data(a)			esp_CRYPTO_cleanup_all_ex_data(a)

#define base64_encode(a,b,c,d,e)				esp_base64_encode(a,b,c,d,e)

#define TLSv1_client_method(a)					esp_TLSv1_client_method(a)
//#if	TLSv1_1__method
#define TLSv1_1_client_method(a)				esp_TLSv1_1_client_method(a)
//#endif
#define SSLv3_client_method(a)					esp_SSLv3_client_method(a)
#define SSLv23_client_method(a)					esp_SSLv23_client_method(a)

#define TLSv1_server_method(a)					esp_TLSv1_server_method(a)
//#if	TLSv1_1__method
#define TLSv1_1_server_method(a)				esp_TLSv1_1_server_method(a)
//#endif
#define SSLv3_server_method(a)					esp_SSLv3_server_method(a)
#define SSLv23_server_method(a)					esp_SSLv23_server_method(a)

/*encapsulation the protocol based on the espressif platform*/
#define SSL_ERROR_NONE							ESP_SSL_ERROR_NONE
#define SSL_ERROR_WANT_WRITE					ESP_SSL_ERROR_WANT_WRITE
#define SSL_ERROR_WANT_READ						ESP_SSL_ERROR_WANT_READ
#define SSL_ERROR_WANT_X509_LOOKUP				ESP_SSL_ERROR_WANT_X509_LOOKUP
#define SSL_ERROR_SYSCALL						ESP_SSL_ERROR_SYSCALL
#define SSL_ERROR_ZERO_RETURN					ESP_SSL_ERROR_ZERO_RETURN
#define SSL_ERROR_SSL							ESP_SSL_ERROR_SSL
#define SSL_FILETYPE_PEM						ESP_SSL_FILETYPE_PEM
#define SSL_VERIFY_PEER							ESP_SSL_VERIFY_PEER
#define EVP_MAX_MD_SIZE							ESP_EVP_MAX_MD_SIZE
#define SSL_VERIFY_FAIL_IF_NO_PEER_CERT			ESP_SSL_VERIFY_FAIL_IF_NO_PEER_CERT
#define SSL_MODE_ENABLE_PARTIAL_WRITE			ESP_SSL_MODE_ENABLE_PARTIAL_WRITE
#define SSL_VERIFY_NONE							ESP_SSL_VERIFY_NONE
#define SSL_ERROR_WANT_CONNECT					ESP_SSL_ERROR_WANT_CONNECT
#define SSL_ERROR_WANT_ACCEPT					ESP_SSL_ERROR_WANT_ACCEPT

/*encapsulation the protocol based on the different platform*/

#endif /* SSL_COMPAT_1_0_H_ */
