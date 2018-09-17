#ifndef QAPI_ALI_IOT_H
#define QAPI_ALI_IOT_H

#include "qapi_ali_iot_txm_base.h"
/*============================================================================
                USER_Mode_DEFS
============================================================================*/

/* Driver ID defines */

#define  TXM_QAPI_IOT_COAP_INIT                              TXM_QAPI_IOT_BASE
#define  TXM_QAPI_IOT_COAP_DEINIT                            TXM_QAPI_IOT_BASE + 1
#define  TXM_QAPI_IOT_COAP_DEVICE_NAME_AUTH                  TXM_QAPI_IOT_BASE + 2
#define  TXM_QAPI_IOT_COAP_YIELD                             TXM_QAPI_IOT_BASE + 3
#define  TXM_QAPI_IOT_COAP_SEND_MESSAGE                      TXM_QAPI_IOT_BASE + 4
#define  TXM_QAPI_IOT_COAP_GET_MESSAGE_PAYLOAD               TXM_QAPI_IOT_BASE + 5
#define  TXM_QAPI_IOT_COAP_GET_MESSAGE_CODE                  TXM_QAPI_IOT_BASE + 6

#define  TXM_QAPI_IOT_MQTT_CONSTRUCT                         TXM_QAPI_IOT_BASE + 7
#define  TXM_QAPI_IOT_MQTT_DESTROY                           TXM_QAPI_IOT_BASE + 8
#define  TXM_QAPI_IOT_MQTT_YIELD                             TXM_QAPI_IOT_BASE + 9
#define  TXM_QAPI_IOT_MQTT_CHECK_STATE_NORMAL                TXM_QAPI_IOT_BASE + 10
#define  TXM_QAPI_IOT_MQTT_SUBSCRIBE                         TXM_QAPI_IOT_BASE + 11
#define  TXM_QAPI_IOT_MQTT_UNSUBSCRIBE                       TXM_QAPI_IOT_BASE + 12
#define  TXM_QAPI_IOT_MQTT_PUBLISH                           TXM_QAPI_IOT_BASE + 13
#define  TXM_QAPI_IOT_SETUP_CONNINFO                         TXM_QAPI_IOT_BASE + 14
#define  TXM_QAPI_IOT_OPEN_LOG                               TXM_QAPI_IOT_BASE + 15
#define  TXM_QAPI_IOT_CLOSE_LOG                              TXM_QAPI_IOT_BASE + 16
#define  TXM_QAPI_IOT_SET_LOG_LEVEL                          TXM_QAPI_IOT_BASE + 17
#define  TXM_QAPI_IOT_DUMP_MEMORY_STATS                      TXM_QAPI_IOT_BASE + 18
#define  TXM_QAPI_IOT_VISUAL_AT_OUTPUT                       TXM_QAPI_IOT_BASE + 19
#define  TXM_QAPI_IOT_VISUAL_AT_INPUT                        TXM_QAPI_IOT_BASE + 20

/*SIMCOM zhangwei 2017-10-26 add ota qapi and HAL base index  begin*/
#define   TXM_QAPI_IOT_OTA_INIT                             TXM_QAPI_IOT_BASE + 21
#define   TXM_QAPI_IOT_OTA_DEINIT                           TXM_QAPI_IOT_BASE + 22
#define   TXM_QAPI_IOT_OTA_REPORTVERSION                    TXM_QAPI_IOT_BASE + 23
#define   TXM_QAPI_IOT_OTA_REPORTPROGRESS                   TXM_QAPI_IOT_BASE + 24
#define   TXM_QAPI_IOT_OTA_ISFETCHING                       TXM_QAPI_IOT_BASE + 25
#define   TXM_QAPI_IOT_OTA_ISFETCHFINISH                    TXM_QAPI_IOT_BASE + 26
#define   TXM_QAPI_IOT_OTA_FETCHYIELD                       TXM_QAPI_IOT_BASE + 27
#define   TXM_QAPI_IOT_OTA_IOCTL                            TXM_QAPI_IOT_BASE + 28
#define   TXM_QAPI_IOT_OTA_GETLASTERROR                     TXM_QAPI_IOT_BASE + 29

#define  TXM_QAPI_IOT_HAL_BASE                               TXM_QAPI_IOT_BASE+ 30
/*SIMCOM zhangwei 2017-10-19 fixed bug begin*/
#define TXM_QAPI_IOT_HAL_MUTEXCREATEP                        TXM_QAPI_IOT_HAL_BASE+ 0
#define TXM_QAPI_IOT_HAL_MUTEXDESTROY                        TXM_QAPI_IOT_HAL_BASE+ 1
#define TXM_QAPI_IOT_HAL_MUTEXLOCK                           TXM_QAPI_IOT_HAL_BASE+ 2
#define TXM_QAPI_IOT_HAL_MUTEXUNLOCK                         TXM_QAPI_IOT_HAL_BASE+ 3
#define TXM_QAPI_IOT_HAL_MALLOC                              TXM_QAPI_IOT_HAL_BASE+ 4
#define TXM_QAPI_IOT_HAL_FREE                                TXM_QAPI_IOT_HAL_BASE+ 5
#define TXM_QAPI_IOT_HAL_UPTIMEMS                            TXM_QAPI_IOT_HAL_BASE+ 6
#define TXM_QAPI_IOT_HAL_SLEEPMS                             TXM_QAPI_IOT_HAL_BASE+ 7
#define TXM_QAPI_IOT_HAL_GETPARTNERID                        TXM_QAPI_IOT_HAL_BASE+ 8
#define TXM_QAPI_IOT_HAL_TCP_ESTABLISH                       TXM_QAPI_IOT_HAL_BASE+ 9
#define TXM_QAPI_IOT_HAL_TCP_DESTROY                         TXM_QAPI_IOT_HAL_BASE+ 10
#define TXM_QAPI_IOT_HAL_TCP_WRITE                           TXM_QAPI_IOT_HAL_BASE+ 11
#define TXM_QAPI_IOT_HAL_TCP_READ                            TXM_QAPI_IOT_HAL_BASE+ 12
#define TXM_QAPI_IOT_HAL_SSL_ESTABLISH                       TXM_QAPI_IOT_HAL_BASE+ 13
#define TXM_QAPI_IOT_HAL_SSL_DESTROY                         TXM_QAPI_IOT_HAL_BASE+ 14
#define TXM_QAPI_IOT_HAL_SSL_WRITE                           TXM_QAPI_IOT_HAL_BASE+ 15
#define TXM_QAPI_IOT_HAL_SSL_READ                            TXM_QAPI_IOT_HAL_BASE+ 16
#define TXM_QAPI_IOT_HAL_UDP_CREATE                          TXM_QAPI_IOT_HAL_BASE+ 17
#define TXM_QAPI_IOT_HAL_UDP_CLOSE                           TXM_QAPI_IOT_HAL_BASE+ 18
#define TXM_QAPI_IOT_HAL_UDP_WRITE                           TXM_QAPI_IOT_HAL_BASE+ 19
#define TXM_QAPI_IOT_HAL_UDP_READ                            TXM_QAPI_IOT_HAL_BASE+ 20
#define TXM_QAPI_IOT_HAL_UDP_READTIMEOUT                     TXM_QAPI_IOT_HAL_BASE+ 21
#define TXM_QAPI_IOT_HAL_UDP_RESOLVEADDRESS                  TXM_QAPI_IOT_HAL_BASE+ 22
#define TXM_QAPI_IOT_HAL_DTLSSESSION_FREE                    TXM_QAPI_IOT_HAL_BASE+ 23
#define TXM_QAPI_IOT_HAL_DTLSSESSION_INIT                    TXM_QAPI_IOT_HAL_BASE+ 24
#define TXM_QAPI_IOT_HAL_DTLSSESSION_CREATE                  TXM_QAPI_IOT_HAL_BASE+ 25
#define TXM_QAPI_IOT_HAL_DTLSSESSION_WRITE                   TXM_QAPI_IOT_HAL_BASE+ 26
#define TXM_QAPI_IOT_HAL_DTLSSESSION_READ                    TXM_QAPI_IOT_HAL_BASE+ 27
/*SIMCOM zhangwei 2017-10-19 fixed bug end*/
/*SIMCOM zhangwei 2017-10-26 add ota qapi and HAL base index  end*/
#if 0

#define IOT_SetupConnInfo(product_key, device_name, device_secret, info_ptr)\
    ((int) (_txm_module_system_call12)(TXM_QAPI_IOT_SETUP_CONNINFO,\
                                       (ULONG) product_key,(ULONG) device_name, (ULONG) device_secret, (ULONG) info_ptr,(ULONG) 0,\
                                       (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                       (ULONG) 0))

#define IOT_OpenLog(ident)\
    ( (void)(_txm_module_system_call12)(TXM_QAPI_IOT_OPEN_LOG,\
                                        ((const char *)) ident,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                        (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                        (ULONG) 0))

#define IOT_CloseLog()\
    ( (void)(_txm_module_system_call12)(TXM_QAPI_IOT_CLOSE_LOG,\
                                        (ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                        (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                        (ULONG) 0))

#define IOT_SetLogLevel(level)\
    ( (void)(_txm_module_system_call12)(TXM_QAPI_IOT_SET_LOG_LEVEL,\
                                        (ULONG) level,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                        (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                        (ULONG) 0))

#define IOT_DumpMemoryStats(level)\
    ( (void)(_txm_module_system_call12)(TXM_QAPI_IOT_DUMP_MEMORY_STATS,\
                                        (ULONG) level,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                        (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                        (ULONG) 0))

#define IOT_Visual_AT_Output(data, length)\
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_VISUAL_AT_OUTPUT,\
                                  (ULONG) data,(ULONG) length, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define IOT_Visual_AT_Input(data, length)\
    ((unsigned short) (_txm_module_system_call12)(TXM_QAPI_IOT_VISUAL_AT_INPUT,\
            (ULONG) data,(ULONG) length, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
            (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
            (ULONG) 0))
#endif


#define   HAL_MutexCreate()  \
    ( (void*)(_txm_module_system_call12)(TXM_QAPI_IOT_HAL_MUTEXCREATEP,\
                                         (ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                         (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                         (ULONG) 0))

#define     HAL_MutexDestroy(mutex)  \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_MUTEXDESTROY,\
                                  (ULONG) mutex,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define      HAL_MutexLock(mutex) \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_MUTEXLOCK,\
                                  (ULONG) mutex,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define    HAL_MutexUnlock(mutex) \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_MUTEXUNLOCK,\
                                  (ULONG) mutex,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define     HAL_Malloc(size) \
    ((CHAR*) (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_MALLOC,\
                                         (ULONG) size,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                         (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                         (ULONG) 0))

#define     HAL_Free(ptr)  \
    ( (_txm_module_system_call12)( TXM_QAPI_IOT_HAL_FREE,\
                                   (ULONG) ptr,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                   (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                   (ULONG) 0))

#define      HAL_UptimeMs()  \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_UPTIMEMS,\
                                  (ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define     HAL_SleepMs(ms) \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_SLEEPMS ,\
                                  (ULONG) ms,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define     HAL_GetPartnerID(pid_str)  \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_GETPARTNERID ,\
                                  (ULONG) pid_str,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define     HAL_TCP_Establish(host, port)  \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_TCP_ESTABLISH ,\
                                  (ULONG) host,(ULONG) port, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define   HAL_TCP_Destroy( fd )     \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_TCP_DESTROY,\
                                  (ULONG) fd,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define   HAL_TCP_Write(fd,buf,len,timeout_ms) \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_TCP_WRITE,\
                                  (ULONG) fd,(ULONG) buf, (ULONG) len, (ULONG) timeout_ms,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define   HAL_TCP_Read( fd,buf,len,timeout_ms)    \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_TCP_READ ,\
                                  (ULONG) fd,(ULONG) buf, (ULONG) len, (ULONG) timeout_ms,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define     HAL_SSL_Establish(host,port,ca_crt,ca_crt_len)  \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_SSL_ESTABLISH ,\
                                  (ULONG) host,(ULONG) port, (ULONG) ca_crt, (ULONG) ca_crt_len,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define   HAL_SSL_Destroy( handle)   \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_SSL_DESTROY,\
                                  (ULONG) handle,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define    HAL_SSL_Write(handle, buf,len,timeout_ms) \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_SSL_WRITE,\
                                  (ULONG) handle,(ULONG) buf, (ULONG) len, (ULONG) timeout_ms,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define     HAL_SSL_Read(handle, buf, len,timeout_ms)       \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_SSL_READ,\
                                  (ULONG) handle,(ULONG)buf, (ULONG) len, (ULONG) timeout_ms,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

/*
*HAL_UDP of SDK 2.0 not compatible with latest version
*/
#define HAL_UDP_create(host, port) \
    ( (void*)(_txm_module_system_call12)(TXM_QAPI_IOT_HAL_UDP_CLOSE,\
                                         (ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                         (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                         (ULONG) 0))

#if 0
#define    HAL_UDP_create(p_socket)       \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_UDP_CREATE,\
                                  (ULONG) p_socket,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))
#endif

#define     HAL_UDP_close(p_socket)  \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_UDP_CLOSE,\
                                  (ULONG) p_socket,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define    HAL_UDP_write(p_socket,p_data,datalen)    \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_UDP_WRITE,\
                                  (ULONG) p_socket,(ULONG) 0, (ULONG) p_data, (ULONG) datalen,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#if 0
#define    HAL_UDP_write(p_socket,p_remote,p_data,datalen)    \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_UDP_WRITE,\
                                  (ULONG) p_socket,(ULONG) p_remote, (ULONG) p_data, (ULONG) datalen,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))
#endif

#define  HAL_UDP_read(p_socket,p_remote,p_data,datalen)            \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_UDP_READ,\
                                  (ULONG) p_socket,(ULONG) p_remote, (ULONG)p_remote, (ULONG) datalen,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define  HAL_UDP_readTimeout(p_socket,p_data,datalen,timeout )   \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_UDP_READTIMEOUT,\
                                  (ULONG) p_socket,(ULONG) 0, (ULONG) p_data, (ULONG) datalen,(ULONG) timeout,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))
#if 0
#define  HAL_UDP_readTimeout(p_socket,p_remote,p_data,datalen,timeout )   \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_UDP_READTIMEOUT,\
                                  (ULONG) p_socket,(ULONG) p_remote, (ULONG) p_data, (ULONG) datalen,(ULONG) timeout,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))
#endif

#define      HAL_UDP_resolveAddress(p_host,addr)       \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_UDP_RESOLVEADDRESS,\
                                  (ULONG) p_host,(ULONG) addr, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define       HAL_DTLSSession_free(context)    \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_DTLSSESSION_FREE,\
                                  (ULONG) context,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define    HAL_DTLSSession_init() \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_DTLSSESSION_INIT,\
                                  (ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define       HAL_DTLSSession_create(context, p_options)  \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_DTLSSESSION_CREATE,\
                                  (ULONG) context,(ULONG) p_options, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define       HAL_DTLSSession_write(context,p_data,p_datalen)  \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_DTLSSESSION_WRITE,\
                                  (ULONG) context,(ULONG) p_data, (ULONG) p_datalen, (ULONG) 0,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

#define   HAL_DTLSSession_read(context,p_data,p_datalen,timeout)  \
    ( (_txm_module_system_call12)(TXM_QAPI_IOT_HAL_DTLSSESSION_READ,\
                                  (ULONG) context,(ULONG) p_data, (ULONG) p_datalen, (ULONG) timeout,(ULONG) 0,\
                                  (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
                                  (ULONG) 0))

int HAL_Snprintf(_IN_ char *str, const int len, const char *fmt, ...);

int HAL_Vsnprintf(_IN_ char *str, _IN_ const int len, _IN_ const char *format, va_list ap);

void HAL_Printf(_IN_ const char *fmt, ...);

char *HAL_GetModuleID(char mid_str[]);

uint32_t HAL_Random(uint32_t region);

void HAL_Srandom(uint32_t seed);

int HAL_Atoi(const char *str);

#define atoi(str)  HAL_Atoi(str)

#endif

