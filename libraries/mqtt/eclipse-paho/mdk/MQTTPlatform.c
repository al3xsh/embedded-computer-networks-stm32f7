/*******************************************************************************
 * Copyright (c) 2014, 2017 IBM Corp.
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
 *    Ian Craggs - return codes from linux_read
 *******************************************************************************/

#include "MQTTPlatform.h"


static uint32_t TickFreq;

void TimerInit(Timer* timer)
{
  TickFreq = osKernelGetTickFreq();
  timer->end_time = 0U;
}

char TimerIsExpired(Timer* timer)
{
  uint32_t tick;
  uint32_t left;

  tick = osKernelGetTickCount();
  left = timer->end_time - tick;

  return (left >= 0x80000000U);
}

void TimerCountdownMS(Timer* timer, unsigned int timeout)
{
  uint32_t tick;

  tick = osKernelGetTickCount();
  timer->end_time = tick + ((timeout * TickFreq) / 1000U);
}

void TimerCountdown(Timer* timer, unsigned int timeout)
{
  uint32_t tick;

  tick = osKernelGetTickCount();
  timer->end_time = tick + (timeout * TickFreq);
}

int TimerLeftMS(Timer* timer)
{
  uint32_t tick;
  uint32_t left;

  tick = osKernelGetTickCount();
  left = timer->end_time - tick;
  if (left >= 0x80000000U)
  {
    left = 0U;
  }

  return (int)left;
}


#if defined(MQTT_TASK)

#ifndef MQTTCLIENT_THREAD_STACK_SIZE
#define MQTTCLIENT_THREAD_STACK_SIZE 1024U
#endif

osThreadAttr_t MQTTClient_ThreadAttr =
{
  .stack_size = MQTTCLIENT_THREAD_STACK_SIZE
};

int ThreadStart(Thread* thread, void (*fn)(void*), void* arg)
{
  return (osThreadNew(fn, arg, &MQTTClient_ThreadAttr) != NULL);
}

void MutexInit(Mutex* mutex)
{
  mutex->mutex_id = osMutexNew(NULL);
}

int MutexLock(Mutex* mutex)
{
  return (osMutexAcquire(mutex->mutex_id, osWaitForever) == osOK);
}

int MutexUnlock(Mutex* mutex)
{
  return (osMutexRelease(mutex->mutex_id) == osOK);
}

#endif


#if (MQTT_MBEDTLS != 0)

#include <string.h>
#include <stdio.h>

#ifndef MBEDTLS_MSG_INFO
#define MBEDTLS_MSG_INFO         0
#endif

#ifndef MBEDTLS_DEBUG_LEVEL
#define MBEDTLS_DEBUG_LEVEL      0
#endif

#ifndef MBEDTLS_SSL_READ_TIMEOUT
#define MBEDTLS_SSL_READ_TIMEOUT 500
#endif

#if (MBEDTLS_MSG_INFO == 1)
#define mbedtls_printf printf
#else
#define mbedtls_printf(...)
#endif

#if (MBEDTLS_DEBUG_LEVEL > 0)
static void out_debug (void *ctx, int level, const char *file,
                                             int line, const char *str ) {
  fprintf ((FILE *)ctx, "%s:%04d: %s", file, line, str);
  fflush ((FILE *)ctx);
}
#endif

#endif


static int network_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
  int bytes = 0;
  int rc;

  if (timeout_ms < 10)
  {
    timeout_ms = 10;
  }

  iotSocketSetOpt(n->socket, IOT_SOCKET_SO_RCVTIMEO, &timeout_ms, sizeof(int));

  while (bytes < len)
  {
    rc = iotSocketRecv(n->socket, &buffer[bytes], (uint32_t)(len - bytes));
    if (rc < 0)
    {
      if (rc != IOT_SOCKET_EAGAIN)
        bytes = -1;
      break;
    }
    else
      bytes += rc;
  }
  return bytes;
}

static int network_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
  int bytes = 0;
  int rc;

  iotSocketSetOpt(n->socket, IOT_SOCKET_SO_SNDTIMEO, &timeout_ms, sizeof(int));

  while (bytes < len)
  {
    rc = iotSocketSend(n->socket, &buffer[bytes], (uint32_t)(len - bytes));
    if (rc < 0)
    {
      if (rc != IOT_SOCKET_EAGAIN)
        bytes = -1;
      break;
    }
    else
      bytes += rc;
  }
  return bytes;
}


#if (MQTT_MBEDTLS != 0)

static int network_tls_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
  int bytes = 0;
  int rc;
  (void)timeout_ms;

  while (bytes < len)
  {
    rc = mbedtls_ssl_read(&n->tlsdata.ssl, &buffer[bytes], (size_t)(len - bytes));
    if (rc < 0)
    {
      if ((rc != MBEDTLS_ERR_SSL_WANT_READ) && (rc != MBEDTLS_ERR_SSL_WANT_WRITE) && (rc != MBEDTLS_ERR_SSL_TIMEOUT))
        bytes = -1;
      break;
    }
    else if (rc == 0)
    {
      bytes = 0;
      break;
    }
    else
      bytes += rc;
  }
  return bytes;
}

static int network_tls_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
  int bytes;
  int rc = -1;
  Timer timer;

  TimerInit(&timer);
  TimerCountdownMS(&timer, (unsigned int)timeout_ms);
  for (bytes = 0; bytes < len; bytes += rc)
  {
    while (!TimerIsExpired(&timer) &&
           ((rc = mbedtls_ssl_write(&n->tlsdata.ssl, &buffer[bytes], (size_t)(len - bytes))) <= 0))
    {
      if((rc != MBEDTLS_ERR_SSL_WANT_READ) && (rc != MBEDTLS_ERR_SSL_WANT_WRITE))
        break;
    }
    if (rc <= 0)
        break;
  }

  return rc;
}

static void network_tls_destory(Network* n)
{
  // Cleanup
  mbedtls_net_free(&n->tlsdata.server_fd);
  mbedtls_ssl_free(&n->tlsdata.ssl);
  mbedtls_ssl_config_free(&n->tlsdata.conf);
  mbedtls_x509_crt_free(&n->tlsdata.clicert);
  mbedtls_x509_crt_free(&n->tlsdata.cacert);
  mbedtls_pk_free(&n->tlsdata.pkey);
  mbedtls_ctr_drbg_free(&n->tlsdata.ctr_drbg);
  mbedtls_entropy_free(&n->tlsdata.entropy);
}

#endif


int NetworkInit(Network* n)
{
  n->socket = -1;
#if (MQTT_MBEDTLS != 0)
  n->tlsdata.server_fd.fd = -1;
#endif
  return 0;
}


int NetworkConnect(Network* n, char* addr, int port)
{
  unsigned char ip[4];
  unsigned int  ip_len = sizeof(ip);
  int rc;

  rc = iotSocketGetHostByName(addr, IOT_SOCKET_AF_INET, &ip[0], &ip_len);
  if (rc < 0)
    return (-1);

  n->socket = iotSocketCreate(IOT_SOCKET_AF_INET, IOT_SOCKET_SOCK_STREAM, IOT_SOCKET_IPPROTO_TCP);
  if (n->socket < 0)
    return (-1);

  rc = iotSocketConnect(n->socket, &ip[0], ip_len, (unsigned short)port);
  if (rc < 0)
    return (-1);

  n->mqttread = network_read;
  n->mqttwrite = network_write;

  return 0;
}


#if (MQTT_MBEDTLS != 0)

int NetworkConnectTLS(Network* n, char* addr, int port, TLScert *tlscert)
{
  const char *pers = "MQTT_Platform_TLS_Wrapper";
  char portstr[6];
  int ret;

  // Initialize the RNG and the session data
  mbedtls_net_init(&n->tlsdata.server_fd);
  mbedtls_ssl_init(&n->tlsdata.ssl);
  mbedtls_ssl_config_init(&n->tlsdata.conf);
  mbedtls_ctr_drbg_init(&n->tlsdata.ctr_drbg);
  mbedtls_x509_crt_init(&n->tlsdata.cacert);
  mbedtls_x509_crt_init(&n->tlsdata.clicert);
  mbedtls_pk_init(&n->tlsdata.pkey);

#if (MBEDTLS_DEBUG_LEVEL > 0)
  mbedtls_debug_set_threshold(MBEDTLS_DEBUG_LEVEL);
  mbedtls_ssl_conf_dbg(&n->tlsdata.conf, out_debug, stdout);
#endif

  mbedtls_printf("  . Seeding the random number generator...");
  mbedtls_entropy_init(&n->tlsdata.entropy);
  if ((ret = mbedtls_ctr_drbg_seed(&n->tlsdata.ctr_drbg,
                                   mbedtls_entropy_func,
                                   &n->tlsdata.entropy,
                                   (const unsigned char *)pers,
                                   strlen(pers))) != 0)
  {
    mbedtls_printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
    network_tls_destory(n);
    return ret;
  }
  mbedtls_printf(" ok\n");

  // Load the trusted CA
  if ((tlscert != NULL) && (tlscert->ca_cert != NULL))
  {
    mbedtls_printf("  . Loading the CA root certificate ...");
    ret = mbedtls_x509_crt_parse(&n->tlsdata.cacert,
                                 (const unsigned char *)tlscert->ca_cert,
                                 strlen(tlscert->ca_cert) + 1);
    if (ret < 0)
    {
      mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
      network_tls_destory(n);
      return ret;
    }
    mbedtls_printf(" ok (%d skipped)\n", ret);
  }

  // Load own certificate and private key
  if ((tlscert != NULL) && (tlscert->client_cert != NULL) && (tlscert->client_key != NULL))
  {
    mbedtls_printf("  . Loading the client cert. and key...");
    ret = mbedtls_x509_crt_parse(&n->tlsdata.clicert,
                                 (const unsigned char *)tlscert->client_cert,
                                 strlen(tlscert->client_cert) + 1);
    if (ret != 0)
    {
      mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
      network_tls_destory(n);
      return ret;
    }
    ret = mbedtls_pk_parse_key(&n->tlsdata.pkey,
                               (const unsigned char *)tlscert->client_key,
                               strlen(tlscert->client_key) + 1,
                               NULL,
                               0);
    if (ret != 0)
    {
      mbedtls_printf(" failed\n  !  mbedtls_pk_parse_key returned -0x%x\n\n", -ret);
      network_tls_destory(n);
      return ret;
    }
    if ((ret = mbedtls_ssl_conf_own_cert(&n->tlsdata.conf, &n->tlsdata.clicert, &n->tlsdata.pkey)) != 0)
    {
      mbedtls_printf(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret);
      network_tls_destory(n);
      return ret;
    }
    mbedtls_printf(" ok\n");
  }

  // Start the connection
  snprintf(portstr, 6, "%d", port);
  mbedtls_printf("  . Connecting to %s/%s...", addr, portstr);
  if ((ret = mbedtls_net_connect(&n->tlsdata.server_fd, addr, portstr,
                                 MBEDTLS_NET_PROTO_TCP)) != 0)
  {
    mbedtls_printf(" failed\n  ! mbedtls_net_connect returned -0x%x\n\n", -ret);
    network_tls_destory(n);
    return ret;
  }
  ret = mbedtls_net_set_block(&n->tlsdata.server_fd);
  if (ret != 0)
  {
    mbedtls_printf(" failed\n  ! net_set_(non)block() returned -0x%x\n\n", -ret);
    network_tls_destory(n);
    return ret;
  }
  mbedtls_printf(" ok\n");

  // Setup stuff
  mbedtls_printf("  . Setting up the SSL/TLS structure...");

  if ((ret = mbedtls_ssl_config_defaults(&n->tlsdata.conf,
                                         MBEDTLS_SSL_IS_CLIENT,
                                         MBEDTLS_SSL_TRANSPORT_STREAM,
                                         MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
  {
    mbedtls_printf(" failed\n  ! mbedtls_ssl_config_defaults returned -0x%x\n\n", -ret);
    network_tls_destory(n);
    return ret;
  }

  mbedtls_ssl_conf_rng(&n->tlsdata.conf, mbedtls_ctr_drbg_random, &n->tlsdata.ctr_drbg);

  if ((tlscert != NULL) && (tlscert->ca_cert != NULL))
  {
    mbedtls_ssl_conf_authmode(&n->tlsdata.conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&n->tlsdata.conf, &n->tlsdata.cacert, NULL);
  }
  else
  {
    mbedtls_ssl_conf_authmode(&n->tlsdata.conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
  }

  mbedtls_ssl_conf_read_timeout(&n->tlsdata.conf, MBEDTLS_SSL_READ_TIMEOUT);

  if ((ret = mbedtls_ssl_setup(&n->tlsdata.ssl, &n->tlsdata.conf)) != 0)
  {
    mbedtls_printf(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
    network_tls_destory(n);
    return ret;
  }

  if ((ret = mbedtls_ssl_set_hostname(&n->tlsdata.ssl, addr)) != 0)
  {
    mbedtls_printf(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
    network_tls_destory(n);
    return ret;
  }

  mbedtls_ssl_set_bio(&n->tlsdata.ssl, &n->tlsdata.server_fd, mbedtls_net_send, NULL, mbedtls_net_recv_timeout);

  mbedtls_printf(" ok\n");

  // Handshake
  mbedtls_printf("  . Performing the SSL/TLS handshake...");
  while ((ret = mbedtls_ssl_handshake(&n->tlsdata.ssl)) != 0)
  {
    if(ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
    {
      mbedtls_printf(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n", -ret);
      if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED)
        mbedtls_printf("    Unable to verify the server's certificate.\n");
        network_tls_destory(n);
        return ret;
    }
  }
  mbedtls_printf(" ok\n    [ Protocol is %s ]\n    [ Ciphersuite is %s ]\n",
                 mbedtls_ssl_get_version(&n->tlsdata.ssl),
                 mbedtls_ssl_get_ciphersuite(&n->tlsdata.ssl));

  if ((ret = mbedtls_ssl_get_record_expansion(&n->tlsdata.ssl)) >= 0)
    mbedtls_printf("    [ Record expansion is %d ]\n", ret);
  else
    mbedtls_printf("    [ Record expansion is unknown (compression) ]\n");

  // Verify the server certificate
  if ((tlscert != NULL) && (tlscert->ca_cert != NULL))
  {
    mbedtls_printf("  . Verifying peer X.509 certificate..." );
    if (mbedtls_ssl_get_verify_result(&n->tlsdata.ssl) != 0)
    {
      mbedtls_printf( " failed\n" );
      network_tls_destory(n);
      return ret;
    }
    else
      mbedtls_printf( " ok\n" );
  }

  mbedtls_x509_crt_free(&n->tlsdata.clicert);
  mbedtls_x509_crt_free(&n->tlsdata.cacert);

  n->mqttread = network_tls_read;
  n->mqttwrite = network_tls_write;

  return 0;
}

#endif


void NetworkDisconnect(Network* n)
{
  if (n->socket >= 0)
    iotSocketClose(n->socket);
  n->socket = -1;

#if (MQTT_MBEDTLS != 0)
  if (n->tlsdata.server_fd.fd >= 0)
  {
    int ret;

    // Close the connection
    mbedtls_printf( "  . Closing the connection..." );
    /* No error checking, the connection might be closed already */
    do
      ret = mbedtls_ssl_close_notify(&n->tlsdata.ssl);
    while (ret == MBEDTLS_ERR_SSL_WANT_WRITE);
    mbedtls_printf( " done\n" );

    network_tls_destory(n);
    n->tlsdata.server_fd.fd = -1;
  }
#endif
}

