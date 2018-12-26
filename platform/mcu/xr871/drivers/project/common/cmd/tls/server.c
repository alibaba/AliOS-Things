/*
 *  SSL server demonstration program
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
#include "tls.h"

#if !defined(MBEDTLS_BIGNUM_C) || !defined(MBEDTLS_CERTS_C) ||    \
            !defined(MBEDTLS_ENTROPY_C) || !defined(MBEDTLS_SSL_TLS_C) || \
            !defined(MBEDTLS_SSL_SRV_C) || !defined(MBEDTLS_NET_C) ||     \
            !defined(MBEDTLS_RSA_C) || !defined(MBEDTLS_CTR_DRBG_C) ||    \
            !defined(MBEDTLS_X509_CRT_PARSE_C)|| \
            !defined(MBEDTLS_PEM_PARSE_C)

void mbedtls_server(void *arg)
{
	mbedtls_printf("MBEDTLS_BIGNUM_C and/or MBEDTLS_CERTS_C and/or MBEDTLS_ENTROPY_C "
	                "and/or MBEDTLS_SSL_TLS_C and/or MBEDTLS_SSL_SRV_C and/or "
	                "MBEDTLS_NET_C and/or MBEDTLS_RSA_C and/or "
	                "MBEDTLS_CTR_DRBG_C and/or MBEDTLS_X509_CRT_PARSE_C "
	                "and/or MBEDTLS_PEM_PARSE_C not defined.\n");
}
#else

#include <stdlib.h>
#include <string.h>
#include "net/mbedtls/entropy.h"
#include "net/mbedtls/ctr_drbg.h"
#include "net/mbedtls/certs.h"
#include "net/mbedtls/x509.h"
#include "net/mbedtls/ssl.h"
#include "net/mbedtls/net.h"
#include "net/mbedtls/error.h"
#include "net/mbedtls/debug.h"

#if defined(MBEDTLS_SSL_CACHE_C)
#include "mbedtls/ssl_cache.h"
#endif

extern volatile int mbedtls_string_mismatch;

static char tls_cal_checksum(void *buffer, int length)
{
	unsigned char *cal = (unsigned char *)buffer;
	unsigned char result = 0;
	while (length != 0) {
		result += cal[--length];
	}
	return result;
}

static void server_debug( void *ctx, int level,
                               const char *file, int line,
                               const char *str )
{
	((void) level);
	mbedtls_printf("%s:%04d: %s\n", file, line, str );
}

void mbedtls_server(void *arg)
{
	mbedtls_test_param *param = (mbedtls_test_param *)arg;
	unsigned int flags = param->flags;
	int ret = 0, len = 0;

	const char *pers = "ssl_server";
	mbedtls_net_context listen_fd, client_fd;
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;
	mbedtls_x509_crt srvcert;
	mbedtls_pk_context pkey;
#if defined(MBEDTLS_SSL_CACHE_C)
	mbedtls_ssl_cache_context cache;
#endif
	unsigned char *buf = mbedtls_calloc(1,TLS_TEST_BUF_SIZE);
	if (!buf) {
		mbedtls_printf("[TLS SRV]Malloc failed...\n");
		goto exit;
	}
	mbedtls_net_init(&listen_fd);
	mbedtls_net_init(&client_fd);
	mbedtls_ssl_init(&ssl);
	mbedtls_ssl_config_init(&conf);
#if defined(MBEDTLS_SSL_CACHE_C)
	mbedtls_ssl_cache_init(&cache);
#endif
	mbedtls_x509_crt_init(&srvcert);
	mbedtls_pk_init(&pkey);
	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctr_drbg);

#if defined(MBEDTLS_DEBUG_C)
	mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif
	/* load the certificates and private RSA key */
	mbedtls_printf("Loading the server cert. and key...\n");
	/*
	 * This demonstration program uses embedded test certificates.
	 * Instead, you may want to use mbedtls_x509_crt_parse_file() to read the
	 * server and CA certificates, as well as mbedtls_pk_parse_keyfile().
	 */
	ret = mbedtls_x509_crt_parse(&srvcert, (const unsigned char *) mbedtls_test_srv_crt,
	                              mbedtls_test_srv_crt_len);
	if (ret != 0) {
		mbedtls_printf( " failed\n  !  mbedtls_x509_crt_parse returned %d\n\n", ret );
		goto exit;
	}

	ret = mbedtls_x509_crt_parse(&srvcert, (const unsigned char *) mbedtls_test_cas_pem,
	                               mbedtls_test_cas_pem_len);
	if (ret != 0) {
		mbedtls_printf( " failed\n  !  mbedtls_x509_crt_parse returned %d\n\n", ret );
		goto exit;
	}

	ret = mbedtls_pk_parse_key(&pkey, (const unsigned char *) mbedtls_test_srv_key,
	                            mbedtls_test_srv_key_len, NULL, 0);
	if (ret != 0) {
		mbedtls_printf(" failed\n  !  mbedtls_pk_parse_key returned %d\n\n", ret);
		goto exit;
	}
	mbedtls_printf(" ok\n");
	/* Setup the listening TCP socket */
	char *tcp_port = NULL;
	if (flags & MBEDTLS_SSL_FLAG_SERVER_PORT)
		tcp_port = param->server_port;
	else
		tcp_port = SERVER_PORT;

	if ((ret = mbedtls_net_bind(&listen_fd, NULL, tcp_port, MBEDTLS_NET_PROTO_TCP)) != 0) {
		mbedtls_printf( " failed\n  ! mbedtls_net_bind returned %d\n\n", ret );
		goto exit;
	}
	mbedtls_printf(" ok\n");
	/* Seed the RNG */
	mbedtls_printf( "Seeding the random number generator...\n" );
	if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
	                                    (const unsigned char *) pers,
	                                    strlen(pers))) != 0) {
		mbedtls_printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
		goto exit;
	}
	mbedtls_printf(" ok\n");
	/* Setup stuff */
	mbedtls_printf("Setting up the SSL data....\n");
	if ((ret = mbedtls_ssl_config_defaults(&conf,
	                                MBEDTLS_SSL_IS_SERVER,
	                                MBEDTLS_SSL_TRANSPORT_STREAM,
	                                MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
		mbedtls_printf(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
		goto exit;
	}

	mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
	mbedtls_ssl_conf_dbg(&conf, server_debug, stdout);

#if defined(MBEDTLS_SSL_CACHE_C)
	mbedtls_ssl_conf_session_cache(&conf, &cache,
	                                mbedtls_ssl_cache_get,
	                                mbedtls_ssl_cache_set);
#endif

	mbedtls_ssl_conf_ca_chain(&conf, srvcert.next, NULL);
	if ((ret = mbedtls_ssl_conf_own_cert(&conf, &srvcert, &pkey)) != 0) {
		mbedtls_printf(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret);
		goto exit;
	}

	if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
		mbedtls_printf(" failed\n  ! mbedtls_ssl_setup returned %d\n", ret);
		goto exit;
	}
	mbedtls_printf(" ok\n");
reset:
	if (ret != 0) {
		mbedtls_printf("Last error was: %d\n", ret);
	}
	mbedtls_net_free(&client_fd);
	mbedtls_ssl_session_reset(&ssl);
	/* Wait until a client connects */
	mbedtls_printf("Waiting for a remote connection.\n");
	if ((ret = mbedtls_net_accept(&listen_fd, &client_fd,
					NULL, 0, NULL)) != 0) {
		mbedtls_printf(" failed\n  ! mbedtls_net_accept returned %d\n", ret);
		goto exit;
	}
	mbedtls_ssl_set_bio(&ssl, &client_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
	mbedtls_printf(" ok\n");

	/* Handshake */
	mbedtls_printf("Performing the SSL/TLS handshake.\n");
	while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
		if(ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
			mbedtls_printf(" failed\n  ! mbedtls_ssl_handshake returned %d\n", ret);
			goto reset;
		}
	}
	mbedtls_printf(" ok(%s)\n",mbedtls_ssl_get_ciphersuite(&ssl));
	mbedtls_printf("Read from client.\n");

	unsigned char checksum = 0;
	mbedtls_string_mismatch = -1;

	do {
		len = TLS_TEST_BUF_SIZE;
		memset( buf, 0, TLS_TEST_BUF_SIZE );
		ret = mbedtls_ssl_read(&ssl, buf, len);
		if(ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
			continue;
		if (ret <= 0) {
			switch (ret) {
				case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
					mbedtls_printf(" connection was closed gracefully\n");
					break;
				case MBEDTLS_ERR_NET_CONN_RESET:
					mbedtls_printf(" connection was reset by peer\n");
					break;
				default:
					mbedtls_printf(" mbedtls_ssl_read returned -0x%x\n", -ret);
                    			break;
			}
			break;
		}

		len = ret;
		checksum += tls_cal_checksum(buf, len);
		mbedtls_printf(" %d bytes read\n%s\n", len, (char *) buf);
	} while (1);

	if (checksum == 0xFF)
		mbedtls_string_mismatch = 0;
	else {
		mbedtls_string_mismatch = -1;
		mbedtls_printf("checksum: 0x%x\n", checksum);
	}
#if 0
	/* Write Response */
	mbedtls_printf("Write to client:\n");
	while ((ret = mbedtls_ssl_write(&ssl, buf, len)) <= 0) {
		if (ret == MBEDTLS_ERR_NET_CONN_RESET) {
			mbedtls_printf(" failed\n  ! peer closed the connection\n");
			goto reset;
		}
		if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
			mbedtls_printf( " failed\n  ! mbedtls_ssl_write returned %d\n\n", ret );
			goto exit;
		}
	}
#endif
	mbedtls_printf("Closing the connection...\n");
	while ((ret = mbedtls_ssl_close_notify(&ssl)) < 0) {
		if (ret != MBEDTLS_ERR_SSL_WANT_READ &&
		           ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
			mbedtls_printf("Warn : mbedtls_ssl_close_notify returned %d\n\n", ret);
			//goto reset;
			break;
		}
	}
	mbedtls_printf(" ok\n");
	ret = 0;

exit:
	if (ret != 0)
		mbedtls_printf("Last error was: %d\n\n", ret);

	mbedtls_net_free(&client_fd);
	mbedtls_net_free(&listen_fd);

	mbedtls_x509_crt_free(&srvcert);
	mbedtls_pk_free(&pkey);
	mbedtls_ssl_free(&ssl);
	mbedtls_ssl_config_free(&conf);
#if defined(MBEDTLS_SSL_CACHE_C)
	mbedtls_ssl_cache_free(&cache);
#endif
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
	FREE_BUF(buf);
}
#endif /* MBEDTLS_BIGNUM_C && MBEDTLS_CERTS_C && MBEDTLS_ENTROPY_C &&
	  MBEDTLS_SSL_TLS_C && MBEDTLS_SSL_SRV_C && MBEDTLS_NET_C &&
	  MBEDTLS_RSA_C && MBEDTLS_CTR_DRBG_C && MBEDTLS_X509_CRT_PARSE_C
	  && MBEDTLS_FS_IO && MBEDTLS_PEM_PARSE_C */
