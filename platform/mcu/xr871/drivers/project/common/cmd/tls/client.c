/*
 *  SSL client demonstration program
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

#if !defined(MBEDTLS_BIGNUM_C) || !defined(MBEDTLS_ENTROPY_C) ||  \
           !defined(MBEDTLS_SSL_TLS_C) || !defined(MBEDTLS_SSL_CLI_C) || \
           !defined(MBEDTLS_NET_C) || !defined(MBEDTLS_RSA_C) ||	 \
           !defined(MBEDTLS_CERTS_C) || !defined(MBEDTLS_PEM_PARSE_C) || \
           !defined(MBEDTLS_CTR_DRBG_C) || !defined(MBEDTLS_X509_CRT_PARSE_C)

void mbedtls_client( void *arg)
{
	mbedtls_printf("MBEDTLS_BIGNUM_C and/or MBEDTLS_ENTROPY_C and/or "
	                      "MBEDTLS_SSL_TLS_C and/or MBEDTLS_SSL_CLI_C and/or "
	                      "MBEDTLS_NET_C and/or MBEDTLS_RSA_C and/or "
	                      "MBEDTLS_CTR_DRBG_C and/or MBEDTLS_X509_CRT_PARSE_C "
	                      "not defined.\n");
}

#else
#include <string.h>
#include "net/mbedtls/net.h"
#include "net/mbedtls/debug.h"
#include "net/mbedtls/ssl.h"
#include "net/mbedtls/entropy.h"
#include "net/mbedtls/ctr_drbg.h"
#include "net/mbedtls/error.h"
#include "net/mbedtls/certs.h"

extern volatile int mbedtls_string_mismatch;

static void cli_debug( void *ctx, int level,
                           const char *file, int line,
                           const char *str )
{
	((void) level);
	mbedtls_printf("%s:%04d: %s\n", file, line, str);
}

void mbedtls_client(void *arg)
{
	mbedtls_test_param *param = (mbedtls_test_param *)arg;
	unsigned int flags = param->flags;
	unsigned int time_start = 0,time_now = 0, time_elapse = 0;
	unsigned int milliseconds = 0;
	int ret = 0, len = 0;
	const char *pers = "ssl_client1";
	mbedtls_net_context server_fd;
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;
	mbedtls_x509_crt cacert;
	unsigned char *buf = mbedtls_calloc(1,TLS_TEST_BUF_SIZE);
	if (!buf) {
		mbedtls_printf( "\n[TLS CLI]Malloc failed.\n" );
		goto exit;
	}

#if defined(MBEDTLS_DEBUG_C)
	mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif
	/* Initialize the RNG and the session data */
	mbedtls_net_init(&server_fd);
	mbedtls_ssl_init(&ssl);
	mbedtls_ssl_config_init(&conf);
	mbedtls_x509_crt_init(&cacert);
	mbedtls_ctr_drbg_init(&ctr_drbg);

	mbedtls_printf("Seeding the random number generator.\n");
	mbedtls_entropy_init(&entropy);
	if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
	                                    (const unsigned char *) pers,
	                                     strlen(pers))) != 0) {
		mbedtls_printf(" failed\n ! mbedtls_ctr_drbg_seed returned %d\n", ret);
		goto exit;
	}
	mbedtls_printf("ok\n");
	/* Initialize certificates */
	mbedtls_printf("Loading the CA root certificate .\n");
#if defined(MBEDTLS_CUSTOM_CA)
	ret = mbedtls_x509_crt_parse(&cacert, (const unsigned char *) mbedtls_custom_cas_pem,
	                               mbedtls_custom_cas_pem_len);
#else
	ret = mbedtls_x509_crt_parse(&cacert, (const unsigned char *) mbedtls_test_cas_pem,
	                               mbedtls_test_cas_pem_len);
#endif
	if (ret < 0) {
		mbedtls_printf(" failed\n ! mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
		goto exit;
	}
	mbedtls_printf(" ok (%d skipped)\n", ret);
	/* Start the connection */
	char *port = NULL;
	if (flags & MBEDTLS_SSL_FLAG_SERVER_PORT)
		port = param->server_port;
	else
		port = SERVER_PORT;

	char *server = NULL;
	if (flags & MBEDTLS_SSL_FLAG_SERVER_NAME)
		server = param->server_name;
	else
		goto exit;
	mbedtls_printf("Connecting to %s (%s).\n", server, port);
	if ((ret = mbedtls_net_connect(&server_fd, server,
	                                   port, MBEDTLS_NET_PROTO_TCP)) != 0) {
		mbedtls_printf(" failed\n  ! mbedtls_net_connect returned %d\n\n", ret);
		goto exit;
	}
	mbedtls_printf(" ok\n");
	/* Setup stuff */
	mbedtls_printf("Setting up the SSL/TLS structure...\n");
	if ((ret = mbedtls_ssl_config_defaults(&conf,
					MBEDTLS_SSL_IS_CLIENT,
					MBEDTLS_SSL_TRANSPORT_STREAM,
					MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
		mbedtls_printf(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
		goto exit;
	}
	mbedtls_printf(" ok\n");
	/*
	 * OPTIONAL is not optimal for security,
	 * but makes interop easier in this simplified example.
	 */
#if defined(MBEDTLS_CUSTOM_CA)
	mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
#else
	mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
#endif
	mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
	mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
	mbedtls_ssl_conf_dbg(&conf, cli_debug, stdout);
	if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
		mbedtls_printf(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
		goto exit;
	}
	#if 0
	if ((ret = mbedtls_ssl_set_hostname(&ssl, server)) != 0)
	{
		mbedtls_printf( " failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret );
		goto exit;
	}
	#endif
	mbedtls_ssl_set_bio( &ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL );
	/* Handshake */
	mbedtls_printf( "Performing the SSL/TLS handshake...\n" );
	while ((ret = mbedtls_ssl_handshake( &ssl ) ) != 0) {
		if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
			mbedtls_printf( " failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret);
			goto exit;
		}
	}
	mbedtls_printf(" ok(%s)\n", mbedtls_ssl_get_ciphersuite(&ssl));
	/* Verify the server certificate */
	mbedtls_printf("Verifying peer X.509 certificate...\n");

	/* In real life, we probably want to bail out when ret != 0 */
	if ((ret = mbedtls_ssl_get_verify_result( &ssl ) ) != 0) {
		char *vrfy_buf = malloc(512);
		if (!vrfy_buf)
			mbedtls_printf("[TLS-CLI]Malloc vrfy buf failed\n");
		else {
			mbedtls_printf(" failed\n");
			mbedtls_x509_crt_verify_info(vrfy_buf, sizeof( vrfy_buf ), "  ! ", ret);
			mbedtls_printf("%s\n", vrfy_buf);
			free(vrfy_buf);
		}
	}
	else
		mbedtls_printf(" ok\n");
	/* Write */
	mbedtls_printf("Write to server.\n");

	if (flags & MBEDTLS_SSL_FLAG_CONTINUE)
		milliseconds = param->continue_ms;
	else
		milliseconds = DEFALT_TEST_TIME;
	time_start = TLS_GET_TIME();

client:
	mbedtls_string_mismatch = -1;

	len = sprintf((char *) buf, CLIENT_DATA);
	while (( ret = mbedtls_ssl_write(&ssl, buf, len)) <= 0) {
		if(ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
		{
			mbedtls_printf(" failed\n  ! mbedtls_ssl_write returned %d\n", ret);
			goto exit;
		}
	}
	mbedtls_printf(" %d bytes written\n", len);
//	mbedtls_printf("%s\n", (char *) buf);
	/* Read response */
	mbedtls_printf("Read from server:\n");
	unsigned int length = 0;
	do {
		len = TLS_TEST_BUF_SIZE;
		memset(buf, 0, len);
		unsigned char *read_buf = buf;

read_data:
		ret = mbedtls_ssl_read(&ssl, read_buf, len);
		if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
			continue;
		if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
			break;
		if (ret < 0) {
			mbedtls_printf("failed\n  ! mbedtls_ssl_read returned %d\n\n", ret);
			break;
		}
		if (ret == 0) {
			mbedtls_printf("\n\nEOF\n\n" );
			break;
		}
		mbedtls_printf(" %d bytes read\n", ret);
		//mbedtls_printf("%s\n", (char *)buf);
		length += ret;
		if (ret >= mbedtls_strlen(CLIENT_DATA)) {
			mbedtls_printf("Read data success\n");
			break;
		} else {
			read_buf += ret;
			len -= ret;
			goto read_data;
		}
	} while (1);
	if ((ret = mbedtls_strncmp(CLIENT_DATA, (char *)buf, mbedtls_strlen(CLIENT_DATA))) != 0) {
		mbedtls_string_mismatch = -1;
		goto exit;
	} else
		mbedtls_string_mismatch = 0;
	time_now = TLS_GET_TIME();
	if (time_now >= time_start) {
		time_elapse = time_now - time_start;
	} else {
		time_elapse = 0xFFFFFFFFUL - time_start + time_now;
	}
	if (time_elapse < milliseconds)
		goto client;
	mbedtls_ssl_close_notify(&ssl);
exit:
	if (ret != 0)
		mbedtls_printf("Last error was: %d\n\n", ret);
	mbedtls_net_free(&server_fd);
	mbedtls_x509_crt_free(&cacert);
	mbedtls_ssl_free(&ssl);
	mbedtls_ssl_config_free(&conf);
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
	FREE_BUF(buf);
}
#endif /* MBEDTLS_BIGNUM_C && MBEDTLS_ENTROPY_C && MBEDTLS_SSL_TLS_C &&
	  MBEDTLS_SSL_CLI_C && MBEDTLS_NET_C && MBEDTLS_RSA_C &&
	  MBEDTLS_CERTS_C && MBEDTLS_PEM_PARSE_C && MBEDTLS_CTR_DRBG_C &&
	  MBEDTLS_X509_CRT_PARSE_C */
