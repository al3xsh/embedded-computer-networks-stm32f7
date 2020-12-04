/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
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

#if !defined(__MQTT_PLATFORM_)
#define __MQTT_PLATFORM_

#include "cmsis_os2.h"
#include "iot_socket.h"
#include "RTE_Components.h"
#ifdef    RTE_Security_mbedTLS
#include "mbedtls/debug.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/pk.h"
#include "mbedtls/x509.h"
#endif

#ifndef   MQTT_MBEDTLS
#if       defined(MBEDTLS_ENTROPY_C)        && \
          defined(MBEDTLS_CTR_DRBG_C)       && \
          defined(MBEDTLS_SSL_TLS_C)        && \
          defined(MBEDTLS_NET_C)            && \
          defined(MBEDTLS_PK_C)             && \
          defined(MBEDTLS_X509_USE_C)       && \
          defined(MBEDTLS_X509_CRT_PARSE_C)
#define   MQTT_MBEDTLS  1
#else
#define   MQTT_MBEDTLS  0
#endif
#endif

typedef struct Timer
{
  uint32_t end_time;
} Timer;

void TimerInit(Timer*);
char TimerIsExpired(Timer*);
void TimerCountdownMS(Timer*, unsigned int);
void TimerCountdown(Timer*, unsigned int);
int  TimerLeftMS(Timer*);

#if defined(MQTT_TASK)

typedef struct Thread
{
  osThreadId_t thread_id;
} Thread;

int ThreadStart(Thread*, void (*fn)(void*), void* arg);

typedef struct Mutex
{
  osMutexId_t mutex_id;
} Mutex;

void MutexInit(Mutex*);
int  MutexLock(Mutex*);
int  MutexUnlock(Mutex*);

#endif

#if (MQTT_MBEDTLS != 0)

typedef struct TLSdata
{
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_ssl_context ssl;
  mbedtls_ssl_config conf;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
  mbedtls_pk_context pkey;
  mbedtls_net_context server_fd;
} TLSdata;

typedef struct TLScert
{
  char *ca_cert;
  char *client_cert;
  char *client_key;
} TLScert;
#endif

typedef struct Network
{
  int socket;
  int (*mqttread) (struct Network*, unsigned char*, int, int);
  int (*mqttwrite) (struct Network*, unsigned char*, int, int);
#if (MQTT_MBEDTLS != 0)
  TLSdata tlsdata;
#endif
} Network;

int  NetworkInit(Network*);
int  NetworkConnect(Network*, char*, int);
void NetworkDisconnect(Network*);

#if (MQTT_MBEDTLS != 0)
int  NetworkConnectTLS(Network*, char*, int, TLScert*);
#endif

#endif
