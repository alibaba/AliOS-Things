/*******************************************************************************
 * Copyright (c) 2014, 2015 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *    Ian Craggs - convert to FreeRTOS
 *******************************************************************************/

#include "MQTTFreertos.h"
#include "netdb.h"

#define LWIP_IPV6 0
#if LWIP_IPV6
#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET6) ? ip6addr_ntoa_r((src),(dst),(size)) \
     : (((af) == AF_INET) ? ipaddr_ntoa_r((src),(dst),(size)) : NULL))
#define inet_pton(af,src,dst) \
    (((af) == AF_INET6) ? inet6_aton((src),(dst)) \
     : (((af) == AF_INET) ? inet_aton((src),(dst)) : 0))
#else /* LWIP_IPV6 */
#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET) ? ipaddr_ntoa_r((src),(dst),(size)) : NULL)
#define inet_pton(af,src,dst) \
    (((af) == AF_INET) ? inet_aton((src),(dst)) : 0)
#endif /* LWIP_IPV6 */

int ThreadStart(Thread* thread, void (*fn)(void*), void* arg)
{
	int rc = 0;
	uint16_t usTaskStackSize = (configMINIMAL_STACK_SIZE * 5);
	UBaseType_t uxTaskPriority = uxTaskPriorityGet(NULL); /* set the priority as the same as the calling task*/

	rc = xTaskCreate(fn,	/* The function that implements the task. */
		"MQTTTask",			/* Just a text name for the task to aid debugging. */
		usTaskStackSize,	/* The stack size is defined in FreeRTOSIPConfig.h. */
		arg,				/* The task parameter, not used in this case. */
		uxTaskPriority,		/* The priority assigned to the task is defined in FreeRTOSConfig.h. */
		&thread->task);		/* The task handle is not used. */

	return rc;
}


void MutexInit(Mutex* mutex)
{
	mutex->sem = xSemaphoreCreateMutex();
}

int MutexLock(Mutex* mutex)
{
	return xSemaphoreTake(mutex->sem, portMAX_DELAY);
}

int MutexUnlock(Mutex* mutex)
{
	return xSemaphoreGive(mutex->sem);
}


void TimerCountdownMS(Timer* timer, unsigned int timeout_ms)
{
	timer->xTicksToWait = timeout_ms / portTICK_PERIOD_MS; /* convert milliseconds to ticks */
	vTaskSetTimeOutState(&timer->xTimeOut); /* Record the time at which this function was entered. */
}


void TimerCountdown(Timer* timer, unsigned int timeout) 
{
	TimerCountdownMS(timer, timeout * 1000);
}


int TimerLeftMS(Timer* timer) 
{
	xTaskCheckForTimeOut(&timer->xTimeOut, &timer->xTicksToWait); /* updates xTicksToWait to the number left */
	return (timer->xTicksToWait * portTICK_PERIOD_MS);
}


char TimerIsExpired(Timer* timer)
{
	return xTaskCheckForTimeOut(&timer->xTimeOut, &timer->xTicksToWait) == pdTRUE;
}


void TimerInit(Timer* timer)
{
	timer->xTicksToWait = 0;
	memset(&timer->xTimeOut, '\0', sizeof(timer->xTimeOut));
}

#if CONFIG_USE_POLARSSL

int FreeRTOS_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	TickType_t xTicksToWait = timeout_ms / portTICK_PERIOD_MS; /* convert milliseconds to ticks */
	TimeOut_t xTimeOut;
	int recvLen = 0;

	int so_error;
	socklen_t errlen = sizeof(so_error);

	vTaskSetTimeOutState(&xTimeOut); /* Record the time at which this function was entered. */
	do
	{
		int rc = 0;
#if defined(LWIP_SO_SNDRCVTIMEO_NONSTANDARD) && (LWIP_SO_SNDRCVTIMEO_NONSTANDARD == 0)
		// timeout format is changed in lwip 1.5.0
        struct timeval timeout;
        timeout.tv_sec  = xTicksToWait / 1000;
        timeout.tv_usec = ( xTicksToWait % 1000 ) * 1000;
		setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval)); 
#else
		setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, &xTicksToWait, sizeof(xTicksToWait)); 
#endif
#if (MQTT_OVER_SSL)
		if (n->use_ssl) {
			rc = ssl_read(n->ssl, buffer + recvLen, len - recvLen);

            getsockopt(n->my_socket, SOL_SOCKET, SO_ERROR, &so_error, &errlen);
            if (so_error && so_error != EAGAIN) {
                n->disconnect(n);
			}

		} else
#endif
		rc = recv(n->my_socket, buffer + recvLen, len - recvLen, 0);
		if (rc > 0)
			recvLen += rc;
		else if (rc < 0)
		{
			getsockopt(n->my_socket, SOL_SOCKET, SO_ERROR, &so_error, &errlen);
			if (so_error != EAGAIN) {
				n->disconnect(n);
            }
			recvLen = rc;
			break;
		}
	} while (recvLen < len && xTaskCheckForTimeOut(&xTimeOut, &xTicksToWait) == pdFALSE);

	return recvLen;
}


int FreeRTOS_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	TickType_t xTicksToWait = timeout_ms / portTICK_PERIOD_MS; /* convert milliseconds to ticks */
	TimeOut_t xTimeOut;
	int sentLen = 0;

	int so_error;
	socklen_t errlen = sizeof(so_error);

	vTaskSetTimeOutState(&xTimeOut); /* Record the time at which this function was entered. */
	do
	{
		int rc = 0;
#if defined(LWIP_SO_SNDRCVTIMEO_NONSTANDARD) && (LWIP_SO_SNDRCVTIMEO_NONSTANDARD == 0)
		// timeout format is changed in lwip 1.5.0
        struct timeval timeout;
        timeout.tv_sec  = xTicksToWait / 1000;
        timeout.tv_usec = ( xTicksToWait % 1000 ) * 1000;
		setsockopt(n->my_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval)); 
#else
		setsockopt(n->my_socket, SOL_SOCKET, SO_SNDTIMEO, &xTicksToWait, sizeof(xTicksToWait));
#endif
#if (MQTT_OVER_SSL)
        if (n->use_ssl) {
            rc = ssl_write(n->ssl, buffer + sentLen, len - sentLen);

            getsockopt(n->my_socket, SOL_SOCKET, SO_ERROR, &so_error, &errlen);
            if (so_error && so_error != EAGAIN) {
                n->disconnect(n);
            }
        } else
#endif
		rc = send(n->my_socket, buffer + sentLen, len - sentLen, 0);
		if (rc > 0)
			sentLen += rc;
		else if (rc < 0)
		{
			getsockopt(n->my_socket, SOL_SOCKET, SO_ERROR, &so_error, &len);
			if (so_error != EAGAIN) {
				n->disconnect(n);
			}
			sentLen = rc;
			break;
		}
	} while (sentLen < len && xTaskCheckForTimeOut(&xTimeOut, &xTicksToWait) == pdFALSE);

	return sentLen;
}


void FreeRTOS_disconnect(Network* n)
{
	shutdown(n->my_socket, SHUT_RDWR);
	close(n->my_socket);
	n->my_socket = -1;

#if (MQTT_OVER_SSL)
    if (n->use_ssl) {
        ssl_free(n->ssl);
        free(n->ssl);
        n->ssl = NULL;
    }
#endif
}

void NetworkInit(Network* n)
{
	n->my_socket = -1;
	n->mqttread = FreeRTOS_read;
	n->mqttwrite = FreeRTOS_write;
	n->disconnect = FreeRTOS_disconnect;

#if (MQTT_OVER_SSL)
	n->use_ssl = 0;
	n->ssl = NULL;
	n->rootCA = NULL;
	n->clientCA = NULL;
	n->private_key = NULL;
#endif
}


#if (MQTT_OVER_SSL)
static int mqtt_tls_verify( void *data, x509_crt *crt, int depth, int *flags ) 
{
	char buf[1024]; 

	mqtt_printf(MQTT_DEBUG, "\nVerify requested for (Depth %d):\n", depth ); 
	x509_crt_info( buf, sizeof( buf ) - 1, "", crt ); 
	mqtt_printf(MQTT_DEBUG, "%s", buf ); 

	if( ( (*flags) & BADCERT_EXPIRED ) != 0 ) 
		mqtt_printf(MQTT_DEBUG, "  ! server certificate has expired\n" ); 

	if( ( (*flags) & BADCERT_REVOKED ) != 0 ) 
		mqtt_printf(MQTT_DEBUG, "  ! server certificate has been revoked\n" ); 

	if( ( (*flags) & BADCERT_CN_MISMATCH ) != 0 ) 
		mqtt_printf(MQTT_DEBUG, "  ! CN mismatch\n" ); 

	if( ( (*flags) & BADCERT_NOT_TRUSTED ) != 0 ) 
		mqtt_printf(MQTT_DEBUG, "  ! self-signed or not signed by a trusted CA\n" ); 

	if( ( (*flags) & BADCRL_NOT_TRUSTED ) != 0 ) 
		mqtt_printf(MQTT_DEBUG, "  ! CRL not trusted\n" ); 

	if( ( (*flags) & BADCRL_EXPIRED ) != 0 ) 
		mqtt_printf(MQTT_DEBUG, "  ! CRL expired\n" ); 

	if( ( (*flags) & BADCERT_OTHER ) != 0 ) 
		mqtt_printf(MQTT_DEBUG, "  ! other (unknown) flag\n" ); 

	if ( ( *flags ) == 0 ) 
		mqtt_printf(MQTT_DEBUG, "  This certificate has no flags\n" ); 

	return( 0 ); 
}

static int my_random(void *p_rng, unsigned char *output, size_t output_len)
{
	rtw_get_random_bytes(output, output_len);
	return 0;
}
#endif // #if (MQTT_OVER_SSL)


int NetworkConnect(Network* n, char* addr, int port)
{
	struct sockaddr_in sAddr;
	int retVal = -1;
	struct hostent *hptr;
	char   **pptr;
	char   str[32];
	int	keepalive_enable = 1;
	int keep_idle = 30;
	
	if(n->my_socket >= 0){
		n->disconnect(n);
	}
	if ((hptr = gethostbyname(addr)) == 0)
	{
		mqtt_printf(MQTT_DEBUG, "gethostbyname failed!");
		goto exit;
	}
	pptr = hptr->h_addr_list;

	for(; *pptr!=NULL; pptr++)
	{
		inet_ntop(hptr->h_addrtype, (const ip_addr_t *)*pptr, str, sizeof(str));
	}
	inet_ntop(hptr->h_addrtype, (const ip_addr_t *)hptr->h_addr, str, sizeof(str));
	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(port);
	sAddr.sin_addr.s_addr = inet_addr(str);
	mqtt_printf(MQTT_DEBUG, "addr = %s", str);
	if ((n->my_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		goto exit;
	}
	setsockopt( n->my_socket, SOL_SOCKET, SO_KEEPALIVE,
			(const char *) &keepalive_enable, sizeof( keepalive_enable ) );	
	setsockopt( n->my_socket, IPPROTO_TCP, TCP_KEEPIDLE,
			(const char *) &keep_idle, sizeof( keep_idle ) );	
	if ((retVal = connect(n->my_socket, (struct sockaddr*)&sAddr, sizeof(sAddr))) < 0)
	{
		close(n->my_socket);
		mqtt_printf(MQTT_DEBUG, "Connect failed!!");
		goto exit;
	}

#if (MQTT_OVER_SSL)
	x509_crt *root_crt;
	x509_crt *client_crt;
	pk_context *client_rsa;

	root_crt = NULL;
	client_crt = NULL;
	client_rsa = NULL;

	if ( n->use_ssl != 0 ) {
		memory_set_own(pvPortMalloc, vPortFree);

		n->ssl = (ssl_context *) malloc( sizeof(ssl_context) );
		if ( n->ssl == NULL ) {
			mqtt_printf(MQTT_DEBUG, "malloc ssl failed!");
			goto err;
		}

		memset(n->ssl, 0, sizeof(ssl_context));
		if ( ssl_init(n->ssl) != 0 ) {
			mqtt_printf(MQTT_DEBUG, "init ssl failed!");
			goto err;
		}

		ssl_set_endpoint(n->ssl, SSL_IS_CLIENT);

		if (n->rootCA != NULL) {
			root_crt = (x509_crt *) polarssl_malloc( sizeof(x509_crt) );
			if ( root_crt == NULL ) {
				mqtt_printf(MQTT_DEBUG, "malloc root_crt failed!");
				goto err;
			}
			memset(root_crt, 0, sizeof(x509_crt));
			ssl_set_authmode( n->ssl, SSL_VERIFY_REQUIRED );
			if (x509_crt_parse( root_crt, n->rootCA, strlen(n->rootCA) ) != 0) {
				mqtt_printf(MQTT_DEBUG, "parse root_crt failed!");
				goto err;
			}
			ssl_set_ca_chain( n->ssl, root_crt, NULL, NULL );
			ssl_set_verify( n->ssl, mqtt_tls_verify, NULL );
			mqtt_printf(MQTT_DEBUG, "root_crt parse done");
		} else {
			ssl_set_authmode(n->ssl, SSL_VERIFY_NONE);
		}

		if (n->clientCA != NULL && n->private_key != NULL) {
			client_crt = (x509_crt *) polarssl_malloc( sizeof(x509_crt) );
			if ( client_crt == NULL ) {
				mqtt_printf(MQTT_DEBUG, "malloc client_crt failed!");
				goto err;
			}
			memset(client_crt, 0, sizeof(x509_crt));
			x509_crt_init(client_crt);

			client_rsa = (pk_context *) polarssl_malloc( sizeof(pk_context) );
			if ( client_rsa == NULL ) {
				mqtt_printf(MQTT_DEBUG, "malloc client_rsa failed!");
				goto err;
			}
			memset(client_rsa, 0, sizeof(pk_context));
			pk_init(client_rsa);

			if ( x509_crt_parse(client_crt, n->clientCA, strlen(n->clientCA)) != 0 ) {
				mqtt_printf(MQTT_DEBUG, "parse client_crt failed!");
				goto err;
			}

			if ( pk_parse_key(client_rsa, n->private_key, strlen(n->private_key), NULL, 0) != 0 ) {
				mqtt_printf(MQTT_DEBUG, "parse client_rsa failed!");
				goto err;
			}

			ssl_set_own_cert(n->ssl, client_crt, client_rsa);
			mqtt_printf(MQTT_DEBUG, "client_crt parse done");
		}

		ssl_set_rng(n->ssl, my_random, NULL);
		ssl_set_bio(n->ssl, net_recv, &n->my_socket, net_send, &n->my_socket);

		retVal = ssl_handshake(n->ssl);
		if (retVal < 0) {
			mqtt_printf(MQTT_DEBUG, "ssl handshake failed err:-0x%04X", -retVal);
			goto err;
		} else {
			mqtt_printf(MQTT_DEBUG, "ssl handshake success");
		}
	}

	if (client_rsa) {
		pk_free(client_rsa);
		polarssl_free(client_rsa);
	}
	if (client_crt) {
		x509_crt_free(client_crt);
		polarssl_free(client_crt);
	}
	if (root_crt) {
		x509_crt_free(root_crt);
		polarssl_free(root_crt);
	}

	goto exit;

err:
	if (client_rsa) {
		pk_free(client_rsa);
		polarssl_free(client_rsa);
	}
	if (client_crt) {
		x509_crt_free(client_crt);
		polarssl_free(client_crt);
	}
	if (root_crt) {
		x509_crt_free(root_crt);
		polarssl_free(root_crt);
	}

	net_close(n->my_socket);
	ssl_free(n->ssl);
	free(n->ssl);

	retVal = -1;

#endif // #if (MQTT_OVER_SSL)

exit:
	return retVal;
}

#elif CONFIG_USE_MBEDTLS /* CONFIG_USE_POLARSSL */

int FreeRTOS_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	TickType_t xTicksToWait = timeout_ms / portTICK_PERIOD_MS; /* convert milliseconds to ticks */
	TimeOut_t xTimeOut;
	int recvLen = 0;

	int so_error;
	socklen_t errlen = sizeof(so_error);

	vTaskSetTimeOutState(&xTimeOut); /* Record the time at which this function was entered. */
	do
	{
		int rc = 0;
#if defined(LWIP_SO_SNDRCVTIMEO_NONSTANDARD) && (LWIP_SO_SNDRCVTIMEO_NONSTANDARD == 0)
		// timeout format is changed in lwip 1.5.0
		struct timeval timeout;
		timeout.tv_sec  = xTicksToWait / 1000;
		timeout.tv_usec = ( xTicksToWait % 1000 ) * 1000;
		setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval)); 
#else
		setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, &xTicksToWait, sizeof(xTicksToWait)); 
#endif
#if (MQTT_OVER_SSL)
		if (n->use_ssl) {
			rc = mbedtls_ssl_read(n->ssl, buffer + recvLen, len - recvLen);

			getsockopt(n->my_socket, SOL_SOCKET, SO_ERROR, &so_error, &errlen);
			if (so_error && so_error != EAGAIN) {
				n->disconnect(n);
			}

		} else
#endif
		rc = recv(n->my_socket, buffer + recvLen, len - recvLen, 0);
		if (rc > 0)
			recvLen += rc;
		else if (rc < 0)
		{
			getsockopt(n->my_socket, SOL_SOCKET, SO_ERROR, &so_error, &errlen);
			if (so_error != EAGAIN) {
				n->disconnect(n);
			}
			recvLen = rc;
			break;
		}
	} while (recvLen < len && xTaskCheckForTimeOut(&xTimeOut, &xTicksToWait) == pdFALSE);

	return recvLen;
}


int FreeRTOS_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	TickType_t xTicksToWait = timeout_ms / portTICK_PERIOD_MS; /* convert milliseconds to ticks */
	TimeOut_t xTimeOut;
	int sentLen = 0;

	int so_error;
	socklen_t errlen = sizeof(so_error);

	vTaskSetTimeOutState(&xTimeOut); /* Record the time at which this function was entered. */
	do
	{
		int rc = 0;
#if defined(LWIP_SO_SNDRCVTIMEO_NONSTANDARD) && (LWIP_SO_SNDRCVTIMEO_NONSTANDARD == 0)
		// timeout format is changed in lwip 1.5.0
		struct timeval timeout;
		timeout.tv_sec  = xTicksToWait / 1000;
		timeout.tv_usec = ( xTicksToWait % 1000 ) * 1000;
		setsockopt(n->my_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval)); 
#else
		setsockopt(n->my_socket, SOL_SOCKET, SO_SNDTIMEO, &xTicksToWait, sizeof(xTicksToWait));
#endif
#if (MQTT_OVER_SSL)
		if (n->use_ssl) {
			rc = mbedtls_ssl_write(n->ssl, buffer + sentLen, len - sentLen);

			getsockopt(n->my_socket, SOL_SOCKET, SO_ERROR, &so_error, &errlen);
			if (so_error && so_error != EAGAIN) {
				n->disconnect(n);
			}
		} else
#endif
		rc = send(n->my_socket, buffer + sentLen, len - sentLen, 0);
		if (rc > 0)
			sentLen += rc;
		else if (rc < 0)
		{
			getsockopt(n->my_socket, SOL_SOCKET, SO_ERROR, &so_error, &len);
			if (so_error != EAGAIN) {
				n->disconnect(n);
			}
			sentLen = rc;
			break;
		}
	} while (sentLen < len && xTaskCheckForTimeOut(&xTimeOut, &xTicksToWait) == pdFALSE);

	return sentLen;
}


void FreeRTOS_disconnect(Network* n)
{
	shutdown(n->my_socket, SHUT_RDWR);
	close(n->my_socket);
	n->my_socket = -1;

#if (MQTT_OVER_SSL)
	if (n->use_ssl) {
		mbedtls_ssl_free(n->ssl);
	mbedtls_ssl_config_free(n->conf);
		free(n->ssl);
		n->ssl = NULL;
	}
#endif
}

void NetworkInit(Network* n)
{
	n->my_socket = -1;
	n->mqttread = FreeRTOS_read;
	n->mqttwrite = FreeRTOS_write;
	n->disconnect = FreeRTOS_disconnect;

#if (MQTT_OVER_SSL)
	n->use_ssl = 0;
	n->ssl = NULL;
	n->conf = NULL;
	n->rootCA = NULL;
	n->clientCA = NULL;
	n->private_key = NULL;
#endif
}


#if (MQTT_OVER_SSL)
static int mqtt_tls_verify( void *data, mbedtls_x509_crt *crt, int depth, int *flags ) 
{ 
	char buf[1024]; 

	mqtt_printf(MQTT_DEBUG, "\nVerify requested for (Depth %d):\n", depth ); 
	mbedtls_x509_crt_info( buf, sizeof( buf ) - 1, "", crt ); 
	mqtt_printf(MQTT_DEBUG, "%s", buf ); 

	if( ( (*flags) & MBEDTLS_X509_BADCERT_EXPIRED ) != 0 ) 
		mqtt_printf(MQTT_DEBUG, "  ! server certificate has expired\n" ); 

	if( ( (*flags) & MBEDTLS_X509_BADCERT_REVOKED ) != 0 ) 
		mqtt_printf(MQTT_DEBUG, "  ! server certificate has been revoked\n" ); 

	if( ( (*flags) & MBEDTLS_X509_BADCERT_CN_MISMATCH ) != 0 ) 
		mqtt_printf(MQTT_DEBUG, "  ! CN mismatch\n" ); 

	if( ( (*flags) & MBEDTLS_X509_BADCERT_NOT_TRUSTED ) != 0 ) 
		mqtt_printf(MQTT_DEBUG, "  ! self-signed or not signed by a trusted CA\n" ); 

	if( ( (*flags) & MBEDTLS_X509_BADCRL_NOT_TRUSTED ) != 0 ) 
		mqtt_printf(MQTT_DEBUG, "  ! CRL not trusted\n" ); 

	if( ( (*flags) & MBEDTLS_X509_BADCRL_EXPIRED ) != 0 ) 
		mqtt_printf(MQTT_DEBUG, "  ! CRL expired\n" ); 

	if( ( (*flags) & MBEDTLS_X509_BADCERT_OTHER ) != 0 ) 
		mqtt_printf(MQTT_DEBUG, "  ! other (unknown) flag\n" ); 

	if ( ( *flags ) == 0 ) 
		mqtt_printf(MQTT_DEBUG, "  This certificate has no flags\n" ); 

	return( 0 ); 
}

static void* my_calloc(size_t nelements, size_t elementSize)
{
	size_t size;
	void *ptr = NULL;

	size = nelements * elementSize;
	ptr = pvPortMalloc(size);

	if(ptr)
		memset(ptr, 0, size);

	return ptr;
}

static int my_random(void *p_rng, unsigned char *output, size_t output_len)
{
	rtw_get_random_bytes(output, output_len);
	return 0;
}
#endif // #if (MQTT_OVER_SSL)


int NetworkConnect(Network* n, char* addr, int port)
{
	struct sockaddr_in sAddr;
	int retVal = -1;
	struct hostent *hptr;
	char   **pptr;
	char   str[32];
	int	keepalive_enable = 1;
	int keep_idle = 30;
	
	if(n->my_socket >= 0){
		n->disconnect(n);
	}
	if ((hptr = gethostbyname(addr)) == 0)
	{
		mqtt_printf(MQTT_DEBUG, "gethostbyname failed!");
		return -1;
	}
	pptr = hptr->h_addr_list;

	for(; *pptr!=NULL; pptr++)
	{
		inet_ntop(hptr->h_addrtype, (const ip_addr_t *)*pptr, str, sizeof(str));
	}
	inet_ntop(hptr->h_addrtype, (const ip_addr_t *)hptr->h_addr, str, sizeof(str));
	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(port);
	sAddr.sin_addr.s_addr = inet_addr(str);
	mqtt_printf(MQTT_DEBUG, "addr = %s", str);
	if ((n->my_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		goto exit;
	}
	setsockopt( n->my_socket, SOL_SOCKET, SO_KEEPALIVE,
			(const char *) &keepalive_enable, sizeof( keepalive_enable ) );	
	setsockopt( n->my_socket, IPPROTO_TCP, TCP_KEEPIDLE,
			(const char *) &keep_idle, sizeof( keep_idle ) );	
	if ((retVal = connect(n->my_socket, (struct sockaddr*)&sAddr, sizeof(sAddr))) < 0)
	{
		close(n->my_socket);
		mqtt_printf(MQTT_DEBUG, "Connect failed!!");
		goto exit;
	}

#if (MQTT_OVER_SSL)
	mbedtls_x509_crt *root_crt;
	mbedtls_x509_crt *client_crt;
	mbedtls_pk_context *client_rsa;

	root_crt = NULL;
	client_crt = NULL;
	client_rsa = NULL;

	if ( n->use_ssl != 0 ) {
		mbedtls_platform_set_calloc_free(my_calloc, vPortFree);

		n->ssl = (mbedtls_ssl_context *) malloc( sizeof(mbedtls_ssl_context) );
		n->conf = (mbedtls_ssl_config *) malloc( sizeof(mbedtls_ssl_config) );      
		if (( n->ssl == NULL )||( n->conf == NULL )) {
			mqtt_printf(MQTT_DEBUG, "malloc ssl failed!");
			goto err;
		}

	mbedtls_ssl_init(n->ssl);
	mbedtls_ssl_config_init(n->conf);        

	if (n->rootCA != NULL) {
		root_crt = (mbedtls_x509_crt *) mbedtls_calloc( sizeof(mbedtls_x509_crt), 1);
		if ( root_crt == NULL ) {
			mqtt_printf(MQTT_DEBUG, "malloc root_crt failed!");
			goto err;
		}

		mbedtls_x509_crt_init(root_crt);

		if (mbedtls_x509_crt_parse( root_crt, n->rootCA, strlen(n->rootCA)+1 ) != 0) {
			mqtt_printf(MQTT_DEBUG, "parse root_crt failed!");
			goto err;
		}
 		mbedtls_ssl_conf_ca_chain( n->conf, root_crt, NULL);
		mbedtls_ssl_conf_authmode(n->conf, MBEDTLS_SSL_VERIFY_REQUIRED);              
		mbedtls_ssl_conf_verify( n->conf, mqtt_tls_verify, NULL );
		mqtt_printf(MQTT_DEBUG, "root_crt parse done");
	} else {
		mbedtls_ssl_conf_authmode(n->conf, MBEDTLS_SSL_VERIFY_NONE);
	}

	if (n->clientCA != NULL && n->private_key != NULL) {
		client_crt = (mbedtls_x509_crt *) mbedtls_calloc( sizeof(mbedtls_x509_crt), 1);
		if ( client_crt == NULL ) {
			mqtt_printf(MQTT_DEBUG, "malloc client_crt failed!");
			goto err;
		}
		mbedtls_x509_crt_init(client_crt);

		client_rsa = (mbedtls_pk_context *) mbedtls_calloc( sizeof(mbedtls_pk_context), 1);
		if ( client_rsa == NULL ) {
			mqtt_printf(MQTT_DEBUG, "malloc client_rsa failed!");
			goto err;
		}
		mbedtls_pk_init(client_rsa);

		if ( mbedtls_x509_crt_parse(client_crt, n->clientCA, strlen(n->clientCA)+1) != 0 ) {
			mqtt_printf(MQTT_DEBUG, "parse client_crt failed!");
			goto err;
		}

		if ( mbedtls_pk_parse_key(client_rsa, n->private_key, strlen(n->private_key)+1, NULL, 0) != 0 ) {
			mqtt_printf(MQTT_DEBUG, "parse client_rsa failed!");
			goto err;
		}
	} 

	mbedtls_ssl_conf_own_cert(n->conf, client_crt, client_rsa);
	mbedtls_ssl_set_bio(n->ssl, &n->my_socket, mbedtls_net_send, mbedtls_net_recv, NULL);
	mbedtls_ssl_conf_rng(n->conf, my_random, NULL);	

	if((mbedtls_ssl_config_defaults(n->conf,
					MBEDTLS_SSL_IS_CLIENT,
					MBEDTLS_SSL_TRANSPORT_STREAM,
					MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {

		mqtt_printf(MQTT_DEBUG, "ssl config defaults failed!");
		goto err;
	}

	if((mbedtls_ssl_setup(n->ssl, n->conf)) != 0) {
		mqtt_printf(MQTT_DEBUG,"mbedtls_ssl_setup failed!");
		goto err;
	}
	
	retVal = mbedtls_ssl_handshake(n->ssl);
	if (retVal < 0) {
		mqtt_printf(MQTT_DEBUG, "ssl handshake failed err:-0x%04X", -retVal);
		goto err;
	} else {
		mqtt_printf(MQTT_DEBUG, "ssl handshake success");
	}
  }

	if (client_rsa) {
		mbedtls_pk_free(client_rsa);
		mbedtls_free(client_rsa);
	}
	if (client_crt) {
		mbedtls_x509_crt_free(client_crt);
		mbedtls_free(client_crt);
	}
	if (root_crt) {
		mbedtls_x509_crt_free(root_crt);
		mbedtls_free(root_crt);
	}

	goto exit;

err:
	if (client_rsa) {
		mbedtls_pk_free(client_rsa);
		mbedtls_free(client_rsa);
	}
	if (client_crt) {
		mbedtls_x509_crt_free(client_crt);
		mbedtls_free(client_crt);
	}
	if (root_crt) {
		mbedtls_x509_crt_free(root_crt);
		mbedtls_free(root_crt);
	}

	mbedtls_net_free(&n->my_socket);
	mbedtls_ssl_free(n->ssl);
	free(n->ssl);

	retVal = -1;

#endif // #if (MQTT_OVER_SSL)

exit:
	return retVal;
}
#endif /* CONFIG_USE_POLARSSL */

#if 0
int NetworkConnectTLS(Network *n, char* addr, int port, SlSockSecureFiles_t* certificates, unsigned char sec_method, unsigned int cipher, char server_verify)
{
	SlSockAddrIn_t sAddr;
	int addrSize;
	int retVal;
	unsigned long ipAddress;

	retVal = sl_NetAppDnsGetHostByName(addr, strlen(addr), &ipAddress, AF_INET);
	if (retVal < 0) {
		return -1;
	}

	sAddr.sin_family = AF_INET;
	sAddr.sin_port = sl_Htons((unsigned short)port);
	sAddr.sin_addr.s_addr = sl_Htonl(ipAddress);

	addrSize = sizeof(SlSockAddrIn_t);

	n->my_socket = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, SL_SEC_SOCKET);
	if (n->my_socket < 0) {
		return -1;
	}

	SlSockSecureMethod method;
	method.secureMethod = sec_method;
	retVal = sl_SetSockOpt(n->my_socket, SL_SOL_SOCKET, SL_SO_SECMETHOD, &method, sizeof(method));
	if (retVal < 0) {
		return retVal;
	}

	SlSockSecureMask mask;
	mask.secureMask = cipher;
	retVal = sl_SetSockOpt(n->my_socket, SL_SOL_SOCKET, SL_SO_SECURE_MASK, &mask, sizeof(mask));
	if (retVal < 0) {
		return retVal;
	}

	if (certificates != NULL) {
		retVal = sl_SetSockOpt(n->my_socket, SL_SOL_SOCKET, SL_SO_SECURE_FILES, certificates->secureFiles, sizeof(SlSockSecureFiles_t));
		if (retVal < 0)
		{
			return retVal;
		}
	}

	retVal = sl_Connect(n->my_socket, (SlSockAddr_t *)&sAddr, addrSize);
	if (retVal < 0) {
		if (server_verify || retVal != -453) {
			sl_Close(n->my_socket);
			return retVal;
		}
	}

	SysTickIntRegister(SysTickIntHandler);
	SysTickPeriodSet(80000);
	SysTickEnable();

	return retVal;
}
#endif
