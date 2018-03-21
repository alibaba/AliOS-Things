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
 *******************************************************************************/

#if !defined(MQTTFreeRTOS_H)
#define MQTTFreeRTOS_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "lwip/sockets.h"
#include "osdep_service.h"

#define MQTT_OVER_SSL (1)
#if (MQTT_OVER_SSL)
#if CONFIG_USE_POLARSSL
#include "polarssl/config.h"
#include "polarssl/net.h"
#include "polarssl/ssl.h"
#include "polarssl/error.h"
#include "polarssl/memory.h"
#elif CONFIG_USE_MBEDTLS
#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"
#endif
#endif

enum {
	MQTT_EXCESSIVE, MQTT_MSGDUMP, MQTT_DEBUG, MQTT_INFO, MQTT_ALWAYS, MQTT_WARNING, MQTT_ERROR
};

#define FreeRTOS_Select select

#define mqtt_printf(level, fmt, arg...)     \
	do {\
		if (level >= MQTT_DEBUG) {\
			{\
				printf("\r\n[%d]mqtt:", rtw_get_current_time());\
				printf(fmt, ##arg);\
				printf("\n\r");\
			} \
		}\
	}while(0)

typedef struct Timer 
{
	TickType_t xTicksToWait;
	TimeOut_t xTimeOut;
} Timer;

typedef struct Network Network;

struct Network
{
	int my_socket;
	int (*mqttread) (Network*, unsigned char*, int, int);
	int (*mqttwrite) (Network*, unsigned char*, int, int);
	void (*disconnect) (Network*);
	int m2m_rxevent;

#if (MQTT_OVER_SSL)
    unsigned char use_ssl;
#if CONFIG_USE_POLARSSL   
    ssl_context *ssl;
#elif CONFIG_USE_MBEDTLS
    mbedtls_ssl_context *ssl;
    mbedtls_ssl_config *conf;
#endif    
    char *rootCA;
    char *clientCA;
    char *private_key;
#endif
};

void TimerInit(Timer*);
char TimerIsExpired(Timer*);
void TimerCountdownMS(Timer*, unsigned int);
void TimerCountdown(Timer*, unsigned int);
int TimerLeftMS(Timer*);

typedef struct Mutex
{
	SemaphoreHandle_t sem;
} Mutex;

void MutexInit(Mutex*);
int MutexLock(Mutex*);
int MutexUnlock(Mutex*);

typedef struct Thread
{
	TaskHandle_t task;
} Thread;

int ThreadStart(Thread*, void (*fn)(void*), void* arg);

int FreeRTOS_read(Network*, unsigned char*, int, int);
int FreeRTOS_write(Network*, unsigned char*, int, int);
void FreeRTOS_disconnect(Network*);

void NetworkInit(Network*);
int NetworkConnect(Network*, char*, int);
/*int NetworkConnectTLS(Network*, char*, int, SlSockSecureFiles_t*, unsigned char, unsigned int, char);*/

#endif
