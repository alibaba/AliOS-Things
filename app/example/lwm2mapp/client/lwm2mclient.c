/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v20.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    Benjamin Cabé - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
 *    Simon Bernard - Please refer to git log
 *    Julien Vermillard - Please refer to git log
 *    Axel Lorente - Please refer to git log
 *    Toby Jaffey - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    Christian Renz - Please refer to git log
 *    Ricky Liu - Please refer to git log
 *
 *******************************************************************************/

/*
 Copyright (c) 2013, 2014 Intel Corporation

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
     * Neither the name of Intel Corporation nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.

 David Navarro <david.navarro@intel.com>
 Bosch Software Innovations GmbH - Please refer to git log

*/
#include <lwm2m.h>
#include "lwm2mclient.h"
#include "lwm2mdebug.h"
#include "commandline.h"
#ifdef WITH_TINYDTLS
#include "dtlsconnection.h"
#elif defined(WITH_MBEDTLS)
#include "mbedconnection.h"
#else
#include "connection.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>
#ifdef WITH_LWIP
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include <sys/stat.h>
#include <errno.h>
#ifdef CSP_LINUXHOST
#include <signal.h>
#endif
#include "aos/cli.h"
#include "ulog/ulog.h"
#include "infra_md5.h"

#define MAX_PACKET_SIZE 1024
#define DEFAULT_SERVER_IPV6 "[::1]"
#define DEFAULT_SERVER_IPV4 "127.0.0.1"

int g_reboot = 0;
static int g_quit = 0;

#define OBJ_COUNT 9
lwm2m_object_t * objArray[OBJ_COUNT];

// only backup security and server objects
# define BACKUP_OBJECT_COUNT 2
lwm2m_object_t * backupObjectArray[BACKUP_OBJECT_COUNT];

void lwm2m_client_command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );

struct cli_command lwm2m_client_message_cmd[] = {
    { "lwm2m", "lwm2m client test", lwm2m_client_command },
};

typedef struct
{
    lwm2m_object_t * securityObjP;
    lwm2m_object_t * serverObject;
    int sock;
#if defined(WITH_TINYDTLS) || defined(WITH_MBEDTLS)
    dtls_connection_t * connList;
    lwm2m_context_t * lwm2mH;
#else
    connection_t * connList;
#endif
    int addressFamily;
} client_data_t;

static void prv_quit(char * buffer,
                     void * user_data)
{
    g_quit = 1;
}

void handle_sigint(int signum)
{
    g_quit = 2;
}

void handle_value_changed(lwm2m_context_t * lwm2mH,
                          lwm2m_uri_t * uri,
                          const char * value,
                          size_t valueLength)
{
    lwm2m_object_t * object = (lwm2m_object_t *)LWM2M_LIST_FIND(lwm2mH->objectList, uri->objectId);

    if (NULL != object)
    {
        if (object->writeFunc != NULL)
        {
            lwm2m_data_t * dataP;
            int result;

            dataP = lwm2m_data_new(1);
            if (dataP == NULL)
            {
                lwm2m_log(LOG_ERR, "Internal allocation failure !\n");
                return;
            }
            dataP->id = uri->resourceId;
            lwm2m_data_encode_nstring(value, valueLength, dataP);

            result = object->writeFunc(uri->instanceId, 1, dataP, object);
            if (COAP_405_METHOD_NOT_ALLOWED == result)
            {
                switch (uri->objectId)
                {
                case LWM2M_DEVICE_OBJECT_ID:
                    result = device_change(dataP, object);
                    break;
                default:
                    break;
                }
            }

            if (COAP_204_CHANGED != result)
            {
                lwm2m_log(LOG_ERR, "result(%d), failed to change value!\n", result);
            }
            else
            {
                lwm2m_log(LOG_INFO, "Battery value changed!\n");
                lwm2m_resource_value_changed(lwm2mH, uri);
            }
            lwm2m_data_free(1, dataP);
            return;
        }
        else
        {
            lwm2m_log(LOG_ERR, "write not supported for specified resource!\n");
        }
        return;
    }
    else
    {
        lwm2m_log(LOG_ERR, "Object not found !\n");
    }
}

#if defined(WITH_TINYDTLS)||defined(WITH_MBEDTLS)
void * lwm2m_connect_server(uint16_t secObjInstID,
                            void * userData)
{
  client_data_t * dataP;
  lwm2m_list_t * instance;
  dtls_connection_t * newConnP = NULL;
  dataP = (client_data_t *)userData;
  lwm2m_object_t  * securityObj = dataP->securityObjP;

  instance = LWM2M_LIST_FIND(dataP->securityObjP->instanceList, secObjInstID);
  if (instance == NULL) return NULL;

  newConnP = connection_create(dataP->connList, dataP->sock, securityObj, instance->id, dataP->lwm2mH, dataP->addressFamily);
  if (newConnP == NULL)
  {
      lwm2m_log(LOG_ERR, "Connection creation failed.\n");
      return NULL;
  }

  dataP->connList = newConnP;
  return (void *)newConnP;
}
#else
void * lwm2m_connect_server(uint16_t secObjInstID,
                            void * userData)
{
    client_data_t * dataP;
    char * uri;
    char * host;
    char * port;
    connection_t * newConnP = NULL;

    dataP = (client_data_t *)userData;

    uri = get_server_uri(dataP->securityObjP, secObjInstID);

    if (uri == NULL) return NULL;

    // parse uri in the form "coaps://[host]:[port]"
    if (0==strncmp(uri, "coaps://", strlen("coaps://"))) {
        host = uri+strlen("coaps://");
    }
    else if (0==strncmp(uri, "coap://",  strlen("coap://"))) {
        host = uri+strlen("coap://");
    }
    else {
        goto exit;
    }
    port = strrchr(host, ':');
    if (port == NULL) goto exit;
    // remove brackets
    if (host[0] == '[')
    {
        host++;
        if (*(port - 1) == ']')
        {
            *(port - 1) = 0;
        }
        else goto exit;
    }
    // split strings
    *port = 0;
    port++;

    lwm2m_log(LOG_INFO, "Opening connection to server at %s:%s\n", host, port);
    newConnP = connection_create(dataP->connList, dataP->sock, host, port, dataP->addressFamily);
    if (newConnP == NULL) {
        lwm2m_log(LOG_ERR, "Connection creation failed.\n");
    }
    else {
        dataP->connList = newConnP;
    }

exit:
    lwm2m_free(uri);
    return (void *)newConnP;
}
#endif

void lwm2m_close_connection(void * sessionH,
                            void * userData)
{
    client_data_t * app_data;
#if defined(WITH_TINYDTLS) || defined(WITH_MBEDTLS)
    dtls_connection_t * targetP;
#else
    connection_t * targetP;
#endif

    app_data = (client_data_t *)userData;
#if defined(WITH_TINYDTLS) || (WITH_MBEDTLS)
    targetP = (dtls_connection_t *)sessionH;
#else
    targetP = (connection_t *)sessionH;
#endif

    if (targetP == app_data->connList)
    {
        app_data->connList = targetP->next;
        lwm2m_free(targetP);
    }
    else
    {
#if defined(WITH_TINYDTLS) || defined(WITH_MBEDTLS)
        dtls_connection_t * parentP;
#else
        connection_t * parentP;
#endif

        parentP = app_data->connList;
        while (parentP != NULL && parentP->next != targetP)
        {
            parentP = parentP->next;
        }
        if (parentP != NULL)
        {
            parentP->next = targetP->next;
            lwm2m_free(targetP);
        }
    }
}

static void prv_output_servers(char * buffer,
                               void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *) user_data;
    lwm2m_server_t * targetP;

    targetP = lwm2mH->bootstrapServerList;

    if (lwm2mH->bootstrapServerList == NULL)
    {
        lwm2m_log(LOG_DEBUG, "No Bootstrap Server.\n");
    }
    else
    {
        lwm2m_log(LOG_DEBUG, "Bootstrap Servers:\n");
        for (targetP = lwm2mH->bootstrapServerList ; targetP != NULL ; targetP = targetP->next)
        {
            lwm2m_log(LOG_DEBUG, " - Security Object ID %d", targetP->secObjInstID);
            lwm2m_log(LOG_DEBUG, "\tHold Off Time: %lu s", (unsigned long)targetP->lifetime);
            lwm2m_log(LOG_DEBUG, "\tstatus: ");
            switch(targetP->status)
            {
            case STATE_DEREGISTERED:
                lwm2m_log(LOG_DEBUG, "DEREGISTERED\n");
                break;
            case STATE_BS_HOLD_OFF:
                lwm2m_log(LOG_DEBUG, "CLIENT HOLD OFF\n");
                break;
            case STATE_BS_INITIATED:
                lwm2m_log(LOG_DEBUG, "BOOTSTRAP INITIATED\n");
                break;
            case STATE_BS_PENDING:
                lwm2m_log(LOG_DEBUG, "BOOTSTRAP PENDING\n");
                break;
            case STATE_BS_FINISHED:
                lwm2m_log(LOG_DEBUG, "BOOTSTRAP FINISHED\n");
                break;
            case STATE_BS_FAILED:
                lwm2m_log(LOG_DEBUG, "BOOTSTRAP FAILED\n");
                break;
            default:
                lwm2m_log(LOG_ERR, "INVALID (%d)\n", (int)targetP->status);
            }
        }
    }

    if (lwm2mH->serverList == NULL)
    {
        lwm2m_log(LOG_ERR, "No LWM2M Server.\n");
    }
    else
    {
        lwm2m_log(LOG_DEBUG, "LWM2M Servers:\n");
        for (targetP = lwm2mH->serverList ; targetP != NULL ; targetP = targetP->next)
        {
            lwm2m_log(LOG_DEBUG, " - Server ID %d", targetP->shortID);
            lwm2m_log(LOG_DEBUG, "\tstatus: ");
            switch(targetP->status)
            {
            case STATE_DEREGISTERED:
                lwm2m_log(LOG_DEBUG, "DEREGISTERED\n");
                break;
            case STATE_REG_PENDING:
                lwm2m_log(LOG_DEBUG, "REGISTRATION PENDING\n");
                break;
            case STATE_REGISTERED:
                lwm2m_log(LOG_DEBUG, "REGISTERED\tlocation: \"%s\"\tLifetime: %lus\n", targetP->location, (unsigned long)targetP->lifetime);
                break;
            case STATE_REG_UPDATE_PENDING:
                lwm2m_log(LOG_DEBUG, "REGISTRATION UPDATE PENDING\n");
                break;
            case STATE_DEREG_PENDING:
                lwm2m_log(LOG_DEBUG, "DEREGISTRATION PENDING\n");
                break;
            case STATE_REG_FAILED:
                lwm2m_log(LOG_DEBUG, "REGISTRATION FAILED\n");
                break;
            default:
                lwm2m_log(LOG_ERR, "INVALID (%d)\n", (int)targetP->status);
            }
        }
    }
}

static void prv_change(char * buffer,
                       void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *) user_data;
    lwm2m_uri_t uri;
    char * end = NULL;
    int result;

    end = get_end_of_arg(buffer);
    if (end[0] == 0) goto syntax_error;

    result = lwm2m_string_to_uri(buffer, end - buffer, &uri);
    if (result == 0) goto syntax_error;

    buffer = get_next_arg(end, &end);

    if (buffer[0] == 0)
    {
        lwm2m_log(LOG_DEBUG, "report change!\n");
        lwm2m_resource_value_changed(lwm2mH, &uri);
    }
    else
    {
        handle_value_changed(lwm2mH, &uri, buffer, end - buffer);
    }
    return;

syntax_error:
    lwm2m_log(LOG_ERR, "Syntax error !\n");
}

static void prv_object_list(char * buffer,
                            void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    lwm2m_object_t * objectP;

    for (objectP = lwm2mH->objectList; objectP != NULL; objectP = objectP->next)
    {
        if (objectP->instanceList == NULL)
        {
            lwm2m_log(LOG_DEBUG, "/%d ", objectP->objID);
        }
        else
        {
            lwm2m_list_t * instanceP;

            for (instanceP = objectP->instanceList; instanceP != NULL ; instanceP = instanceP->next)
            {
                lwm2m_log(LOG_DEBUG, "/%d/%d  ", objectP->objID, instanceP->id);
            }
        }
        lwm2m_log(LOG_DEBUG, "\n");
    }
}

static void prv_instance_dump(lwm2m_object_t * objectP,
                              uint16_t id)
{
    int numData;
    lwm2m_data_t * dataArray;
    uint16_t res;

    numData = 0;
    res = objectP->readFunc(id, &numData, &dataArray, objectP);
    if (res != COAP_205_CONTENT)
    {
        lwm2m_log(LOG_ERR, "Error ");
        print_status(stdout, res);
        lwm2m_log(LOG_ERR, "\n");
        return;
    }

    dump_tlv(stdout, numData, dataArray, 0);
}


static void prv_object_dump(char * buffer,
                            void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *) user_data;
    lwm2m_uri_t uri;
    char * end = NULL;
    int result;
    lwm2m_object_t * objectP;

    end = get_end_of_arg(buffer);
    if (end[0] == 0) goto syntax_error;

    result = lwm2m_string_to_uri(buffer, end - buffer, &uri);
    if (result == 0) goto syntax_error;
    if (uri.flag & LWM2M_URI_FLAG_RESOURCE_ID) goto syntax_error;

    objectP = (lwm2m_object_t *)LWM2M_LIST_FIND(lwm2mH->objectList, uri.objectId);
    if (objectP == NULL)
    {
        lwm2m_log(LOG_ERR, "Object not found.\n");
        return;
    }

    if (uri.flag & LWM2M_URI_FLAG_INSTANCE_ID)
    {
        prv_instance_dump(objectP, uri.instanceId);
    }
    else
    {
        lwm2m_list_t * instanceP;

        for (instanceP = objectP->instanceList; instanceP != NULL ; instanceP = instanceP->next)
        {
            lwm2m_log(LOG_DEBUG, "Instance %d:\n", instanceP->id);
            prv_instance_dump(objectP, instanceP->id);
            lwm2m_log(LOG_DEBUG, "\n");
        }
    }

    return;

syntax_error:
    lwm2m_log(LOG_ERR, "Object dump syntax error !\n");
}

static void prv_update(char * buffer,
                       void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    if (buffer[0] == 0) goto syntax_error;

    uint16_t serverId = (uint16_t) atoi(buffer);
    int res = lwm2m_update_registration(lwm2mH, serverId, false);
    if (res != 0)
    {
        lwm2m_log(LOG_DEBUG, "Registration update error: ");
        print_status(stdout, res);
        lwm2m_log(LOG_DEBUG, "\r\n");
    }
    return;

syntax_error:
    lwm2m_log(LOG_ERR, "Object update syntax error !\n");
}

static void update_battery_level(lwm2m_context_t * context)
{
    static time_t next_change_time = 0;
    time_t tv_sec;

    tv_sec = lwm2m_gettime();
    if (tv_sec < 0) return;

    if (next_change_time < tv_sec)
    {
        char value[15];
        int valueLength;
        lwm2m_uri_t uri;
        int level = rand() % 100;

        if (0 > level) level = -level;
        if (lwm2m_string_to_uri("/3/0/9", 6, &uri))
        {
            valueLength = sprintf(value, "%d", level);
            lwm2m_log(LOG_INFO, "New Battery Level: %d\n", level);
            handle_value_changed(context, &uri, value, valueLength);
        }
        level = rand() % 20;
        if (0 > level) level = -level;
        next_change_time = tv_sec + level + 10;
    }
}

static void prv_add(char * buffer,
                    void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    lwm2m_object_t * objectP;
    int res;

    objectP = get_test_object();
    if (objectP == NULL)
    {
        lwm2m_log(LOG_ERR, "Creating object 31024 failed.\n");
        return;
    }
    res = lwm2m_add_object(lwm2mH, objectP);
    if (res != 0)
    {
        lwm2m_log(LOG_ERR, "Adding object 31024 failed: ");
        print_status(stdout, res);
        lwm2m_log(LOG_ERR, "\n");
    }
    else
    {
        lwm2m_log(LOG_DEBUG, "Object 31024 added.\n");
    }
    return;
}

static void prv_remove(char * buffer,
                       void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    int res;

    res = lwm2m_remove_object(lwm2mH, 31024);
    if (res != 0)
    {
        lwm2m_log(LOG_ERR, "Removing object 31024 failed: ");
        print_status(stdout, res);
        lwm2m_log(LOG_ERR, "\n");
    }
    else
    {
        lwm2m_log(LOG_DEBUG, "Object 31024 removed.\n");
    }
    return;
}

#ifdef LWM2M_BOOTSTRAP
static void prv_initiate_bootstrap(char * buffer,
                                   void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    lwm2m_server_t * targetP;

    // HACK !!!
    lwm2mH->state = STATE_BOOTSTRAP_REQUIRED;
    targetP = lwm2mH->bootstrapServerList;
    while (targetP != NULL)
    {
        targetP->lifetime = 0;
        targetP = targetP->next;
    }
}
#endif /* LWM2M_BOOTSTRAP */

static void prv_display_objects(char * buffer,
                                void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    lwm2m_object_t * object;

    for (object = lwm2mH->objectList; object != NULL; object = object->next){
        if (NULL != object) {
            switch (object->objID)
            {
            case LWM2M_SECURITY_OBJECT_ID:
                display_security_object(object);
                break;
            case LWM2M_SERVER_OBJECT_ID:
                display_server_object(object);
                break;
            case LWM2M_ACL_OBJECT_ID:
                break;
            case LWM2M_DEVICE_OBJECT_ID:
                display_device_object(object);
                break;
            case LWM2M_CONN_MONITOR_OBJECT_ID:
                break;
            case LWM2M_FIRMWARE_UPDATE_OBJECT_ID:
                display_firmware_object(object);
                break;
            case LWM2M_LOCATION_OBJECT_ID:
                display_location_object(object);
                break;
            case LWM2M_CONN_STATS_OBJECT_ID:
                break;
            case TEST_OBJECT_ID:
                display_test_object(object);
                break;
            }
        }
    }
}

#ifdef LWM2M_BOOTSTRAP
static void prv_display_backup(char * buffer,
        void * user_data)
{
   int i;
   for (i = 0 ; i < BACKUP_OBJECT_COUNT ; i++) {
       lwm2m_object_t * object = backupObjectArray[i];
       if (NULL != object) {
           switch (object->objID)
           {
           case LWM2M_SECURITY_OBJECT_ID:
               display_security_object(object);
               break;
           case LWM2M_SERVER_OBJECT_ID:
               display_server_object(object);
               break;
           default:
               break;
           }
       }
   }
}

static void prv_backup_objects(lwm2m_context_t * context)
{
    uint16_t i;

    for (i = 0; i < BACKUP_OBJECT_COUNT; i++) {
        if (NULL != backupObjectArray[i]) {
            switch (backupObjectArray[i]->objID)
            {
            case LWM2M_SECURITY_OBJECT_ID:
                clean_security_object(backupObjectArray[i]);
                lwm2m_free(backupObjectArray[i]);
                break;
            case LWM2M_SERVER_OBJECT_ID:
                clean_server_object(backupObjectArray[i]);
                lwm2m_free(backupObjectArray[i]);
                break;
            default:
                break;
            }
        }
        backupObjectArray[i] = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
        memset(backupObjectArray[i], 0, sizeof(lwm2m_object_t));
    }

    /*
     * Backup content of objects 0 (security) and 1 (server)
     */
    copy_security_object(backupObjectArray[0], (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SECURITY_OBJECT_ID));
    copy_server_object(backupObjectArray[1], (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SERVER_OBJECT_ID));
}

static void prv_restore_objects(lwm2m_context_t * context)
{
    lwm2m_object_t * targetP;

    /*
     * Restore content  of objects 0 (security) and 1 (server)
     */
    targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SECURITY_OBJECT_ID);
    // first delete internal content
    clean_security_object(targetP);
    // then restore previous object
    copy_security_object(targetP, backupObjectArray[0]);

    targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SERVER_OBJECT_ID);
    // first delete internal content
    clean_server_object(targetP);
    // then restore previous object
    copy_server_object(targetP, backupObjectArray[1]);

    // restart the old servers
    lwm2m_log(LOG_DEBUG, "[BOOTSTRAP] ObjectList restored\n");
}

static void update_bootstrap_info(lwm2m_client_state_t * previousBootstrapState,
        lwm2m_context_t * context)
{
    if (*previousBootstrapState != context->state)
    {
        *previousBootstrapState = context->state;
        switch(context->state)
        {
            case STATE_BOOTSTRAPPING:
#ifdef WITH_LOGS
                lwm2m_log(LOG_DEBUG, "[BOOTSTRAP] backup security and server objects\n");
#endif
                prv_backup_objects(context);
                break;
            default:
                break;
        }
    }
}

static void close_backup_object()
{
    int i;
    for (i = 0; i < BACKUP_OBJECT_COUNT; i++) {
        if (NULL != backupObjectArray[i]) {
            switch (backupObjectArray[i]->objID)
            {
            case LWM2M_SECURITY_OBJECT_ID:
                clean_security_object(backupObjectArray[i]);
                lwm2m_free(backupObjectArray[i]);
                break;
            case LWM2M_SERVER_OBJECT_ID:
                clean_server_object(backupObjectArray[i]);
                lwm2m_free(backupObjectArray[i]);
                break;
            default:
                break;
            }
        }
    }
}
#endif

void print_usage(void)
{
    lwm2m_log(LOG_DEBUG, "Usage: lwm2mclient [OPTION]\n");
    lwm2m_log(LOG_DEBUG, "Launch a LWM2M client.\n");
    lwm2m_log(LOG_DEBUG, "Options:\n");
    lwm2m_log(LOG_DEBUG, "  -n NAME\tSet the endpoint name of the Client. Default: testlwm2mclient\n");
    lwm2m_log(LOG_DEBUG, "  -l PORT\tSet the local UDP port of the Client. Default: 56830\n");
    lwm2m_log(LOG_DEBUG, "  -h HOST\tSet the hostname of the LWM2M Server to connect to. Default: localhost\n");
    lwm2m_log(LOG_DEBUG, "  -p PORT\tSet the port of the LWM2M Server to connect to. Default: "LWM2M_STANDARD_PORT_STR"\n");
    lwm2m_log(LOG_DEBUG, "  -4\t\tUse IPv4 connection. Default: IPv6 connection\n");
    lwm2m_log(LOG_DEBUG, "  -t TIME\tSet the lifetime of the Client. Default: 300\n");
    lwm2m_log(LOG_DEBUG, "  -b\t\tBootstrap requested.\n");
    lwm2m_log(LOG_DEBUG, "  -c\t\tChange battery level over time.\n");
#if defined (WITH_TINYDTLS) || defined (WITH_MBEDTLS)
    lwm2m_log(LOG_DEBUG, "  -i STRING\tSet the device management or bootstrap server PSK identity. If not set use none secure mode\n");
    lwm2m_log(LOG_DEBUG, "  -s HEXSTRING\tSet the device management or bootstrap server Pre-Shared-Key. If not set use none secure mode\n");
#endif
    lwm2m_log(LOG_DEBUG, "\n");
}

/*
 * The function start by setting up the command line interface (which may or not be useful depending on your project)
 *
 * This is an array of commands describes as { name, description, long description, callback, userdata }.
 * The firsts tree are easy to understand, the callback is the function that will be called when this command is typed
 * and in the last one will be stored the lwm2m context (allowing access to the server settings and the objects).
 */
command_desc_t commands[] =
{
    {"list", "List known servers.", NULL, prv_output_servers, NULL},
    {"change", "Change the value of resource.", " change URI [DATA]\r\n"
                                                "   URI: uri of the resource such as /3/0, /3/0/2\r\n"
                                                "   DATA: (optional) new value\r\n", prv_change, NULL},
    {"update", "Trigger a registration update", " update SERVER\r\n"
                                                "   SERVER: short server id such as 123\r\n", prv_update, NULL},
#ifdef LWM2M_BOOTSTRAP
    {"bootstrap", "Initiate a DI bootstrap process", NULL, prv_initiate_bootstrap, NULL},
    {"dispb", "Display current backup of objects/instances/resources\r\n"
              "\t(only security and server objects are backupped)", NULL, prv_display_backup, NULL},
#endif /* LWM2M_BOOTSTRAP */
    {"ls", "List Objects and Instances", NULL, prv_object_list, NULL},
    {"disp", "Display current objects/instances/resources", NULL, prv_display_objects, NULL},
    {"dump", "Dump an Object", "dump URI"
                               "URI: uri of the Object or Instance such as /3/0, /1\r\n", prv_object_dump, NULL},
    {"add", "Add support of object 31024", NULL, prv_add, NULL},
    {"rm", "Remove support of object 31024", NULL, prv_remove, NULL},
    {"quit", "Quit the client gracefully.", NULL, prv_quit, NULL},
    {"^C", "Quit the client abruptly (without sending a de-register message).", NULL, NULL, NULL},

    COMMAND_END_LIST
};

#ifndef PRODUCT_KEY_MAXLEN
#define PRODUCT_KEY_MAXLEN          (20 + 1)
#endif
#ifndef DEVICE_NAME_MAXLEN
#define DEVICE_NAME_MAXLEN          (32 + 1)
#endif
#ifndef DEVICE_ID_MAXLEN
#define DEVICE_ID_MAXLEN            (64 + 1)
#endif
#ifndef DEVICE_SECRET_MAXLEN
#define DEVICE_SECRET_MAXLEN        (64 + 1)
#endif
#define IOTX_SIGN_LENGTH  (PRODUCT_KEY_MAXLEN + DEVICE_NAME_MAXLEN + DEVICE_SECRET_MAXLEN + DEVICE_ID_MAXLEN)
#define IOTX_SIGN_SOURCE_LEN     (256)
#define IOTX_SIGN_SRC_STR_WITH_RANDOM  "clientId%sdeviceName%sproductKey%srandom%s"

int calc_sign_with_seq(const char *p_device_secret, const char *p_client_id,
                            const char *p_device_name, const char *p_product_key, const char* seq, char sign[IOTX_SIGN_LENGTH])
{
    char *p_msg = NULL;

    p_msg = (char *)lwm2m_malloc(IOTX_SIGN_SOURCE_LEN);
    if (NULL == p_msg) {
        return -1;
    }
    memset(sign,  0x00, IOTX_SIGN_LENGTH);
    memset(p_msg, 0x00, IOTX_SIGN_SOURCE_LEN);

    snprintf(p_msg, IOTX_SIGN_SOURCE_LEN,
                 IOTX_SIGN_SRC_STR_WITH_RANDOM,
                 p_client_id,
                 p_device_name,
                 p_product_key, seq);
    utils_hmac_md5(p_msg, strlen(p_msg), sign, p_device_secret, strlen(p_device_secret));

    lwm2m_free(p_msg);
    return 0;
}

int lwm2m_client_main(int argc, char *argv[])
{
    client_data_t data;
    int result;
    lwm2m_context_t * lwm2mH = NULL;
    int i;
    const char * localPort = "56830";
    const char * server = NULL;
    const char * serverPort = LWM2M_STANDARD_PORT_STR;
    char * name = "testlwm2mclient";
    int lifetime = 300;
    int batterylevelchanging = 0;
    time_t reboot_time = 0;
    int opt;
#ifdef LWM2M_BOOTSTRAP
    bool bootstrapRequested = false;
    bool serverPortChanged = false;
#endif

    char* deviceSecret = NULL;
    char* deviceName = NULL;
    char* productKey = NULL;

    char  psk_key[IOTX_SIGN_LENGTH] = {0};
    char  psk_id[IOTX_SIGN_LENGTH] = {0};

#ifdef LWM2M_BOOTSTRAP
    lwm2m_client_state_t previousState = STATE_INITIAL;
#endif

    char * pskId = NULL;
    char * psk = NULL;
    uint16_t pskLen = -1;
    char * pskBuffer = NULL;

    memset(&data, 0, sizeof(client_data_t));
    data.addressFamily = AF_INET6;

    opt = 1;
    while (opt < argc)
    {
        if (argv[opt] == NULL
            || argv[opt][0] != '-'
            || argv[opt][2] != 0)
        {
            print_usage();
            return 0;
        }
        switch (argv[opt][1])
        {
#ifdef LWM2M_BOOTSTRAP
        case 'b':
            bootstrapRequested = true;
            if (!serverPortChanged) serverPort = LWM2M_BSSERVER_PORT_STR;
            break;
#endif
        case 'c':
            batterylevelchanging = 1;
            break;
        case 't':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            if (1 != sscanf(argv[opt], "%d", &lifetime))
            {
                print_usage();
                return 0;
            }
            break;
#if defined(WITH_TINYDTLS) || defined(WITH_MBEDTLS)
        case 'i':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            pskId = argv[opt];
            break;
        case 's':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            psk = argv[opt];
            break;
#endif
        case 'n':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }

            if(strlen(name) > DEVICE_ID_MAXLEN)
            {
                lwm2m_log(LOG_ERR, "lwm2m name length is too large\n");
                return 0;
            }
            name = argv[opt];
            break;
        case 'l':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            localPort = argv[opt];
            break;
        case 'h':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            server = argv[opt];
            break;
        case 'p':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            serverPort = argv[opt];
#ifdef LWM2M_BOOTSTRAP
            serverPortChanged = true;
#endif
            break;
        case '4':
            data.addressFamily = AF_INET;
            break;
        case 'x':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            deviceSecret = argv[opt];
            break;
        case 'y':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            deviceName = argv[opt];
            break;
        case 'z':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            productKey = argv[opt];
            break;
        default:
            print_usage();
            return 0;
        }
        opt += 1;
    }

    if (!server)
    {
        server = (AF_INET == data.addressFamily ? DEFAULT_SERVER_IPV4 : DEFAULT_SERVER_IPV6);
    }

    /*
     *This call an internal function that create an IPV6 socket on the port 5683.
     */
    lwm2m_log(LOG_INFO, "Trying to bind LWM2M Client to port %s\n", localPort);
    data.sock = create_socket(localPort, data.addressFamily);
    if (data.sock < 0)
    {
        lwm2m_log(LOG_ERR, "Failed to open socket: %d %s\n", errno, strerror(errno));
        return -1;
    }

    if(deviceSecret != NULL && deviceName != NULL && productKey != NULL)
    {
        char * random4 = "random4";
        snprintf(psk_id, IOTX_SIGN_LENGTH,"%s:%s:%s:%s", productKey, deviceName, name, random4);
        calc_sign_with_seq(deviceSecret, name, deviceName, productKey, random4, psk_key);
        pskId = psk_id;
        psk = psk_key;
        name = pskId;
    }

    /*
     * Now the main function fill an array with each object, this list will be later passed to liblwm2m.
     * Those functions are located in their respective object file.
     */
#if defined(WITH_TINYDTLS) || defined(WITH_MBEDTLS)
    if (psk != NULL)
    {
        pskLen = strlen(psk) / 2;
        pskBuffer = lwm2m_malloc(pskLen);

        if (NULL == pskBuffer)
        {
            lwm2m_log(LOG_ERR, "Failed to create PSK binary buffer\n");
            return -1;
        }
        // Hex string to binary
        char *h = psk;
        char *b = pskBuffer;
        char xlate[] = "0123456789ABCDEF";

        for ( ; *h; h += 2, ++b)
        {
            char *l = strchr(xlate, toupper(*h));
            char *r = strchr(xlate, toupper(*(h+1)));

            if (!r || !l)
            {
                lwm2m_log(LOG_ERR, "Failed to parse Pre-Shared-Key HEXSTRING\n");
                return -1;
            }

            *b = ((l - xlate) << 4) + (r - xlate);
        }
    }
#endif

    char serverUri[50];
    int serverId = 123;
#if defined(WITH_TINYDTLS) || defined(WITH_MBEDTLS)
    sprintf (serverUri, "coaps://%s:%s", server, serverPort);
#else
    sprintf (serverUri, "coap://%s:%s", server, serverPort);
#endif
#ifdef LWM2M_BOOTSTRAP
    objArray[0] = get_security_object(serverId, serverUri, pskId, pskBuffer, pskLen, bootstrapRequested);
#else
    objArray[0] = get_security_object(serverId, serverUri, pskId, pskBuffer, pskLen, false);
#endif
    if (NULL == objArray[0])
    {
        lwm2m_log(LOG_ERR, "Failed to create security object\n");
        return -1;
    }
    data.securityObjP = objArray[0];

    objArray[1] = get_server_object(serverId, "U", lifetime, false);
    if (NULL == objArray[1])
    {
        lwm2m_log(LOG_ERR, "Failed to create server object\n");
        return -1;
    }

    objArray[2] = get_object_device();
    if (NULL == objArray[2])
    {
        lwm2m_log(LOG_ERR, "Failed to create Device object\n");
        return -1;
    }

    objArray[3] = get_object_firmware();
    if (NULL == objArray[3])
    {
        lwm2m_log(LOG_ERR, "Failed to create Firmware object\n");
        return -1;
    }

    objArray[4] = get_object_location();
    if (NULL == objArray[4])
    {
        lwm2m_log(LOG_ERR, "Failed to create location object\n");
        return -1;
    }

    objArray[5] = get_test_object();
    if (NULL == objArray[5])
    {
        lwm2m_log(LOG_ERR, "Failed to create test object\n");
        return -1;
    }

    objArray[6] = get_object_conn_m();
    if (NULL == objArray[6])
    {
        lwm2m_log(LOG_ERR, "Failed to create connectivity monitoring object\n");
        return -1;
    }

    objArray[7] = get_object_conn_s();
    if (NULL == objArray[7])
    {
        lwm2m_log(LOG_ERR, "Failed to create connectivity statistics object\n");
        return -1;
    }

    int instId = 0;
    objArray[8] = acc_ctrl_create_object();
    if (NULL == objArray[8])
    {
        lwm2m_log(LOG_ERR, "Failed to create Access Control object\n");
        return -1;
    }
    else if (acc_ctrl_obj_add_inst(objArray[8], instId, 3, 0, serverId)==false)
    {
        lwm2m_log(LOG_ERR, "Failed to create Access Control object instance\n");
        return -1;
    }
    else if (acc_ctrl_oi_add_ac_val(objArray[8], instId, 0, 0b000000000001111)==false)
    {
        lwm2m_log(LOG_ERR, "Failed to create Access Control ACL default resource\n");
        return -1;
    }
    else if (acc_ctrl_oi_add_ac_val(objArray[8], instId, 999, 0b000000000000001)==false)
    {
        lwm2m_log(LOG_ERR, "Failed to create Access Control ACL resource for serverId: 999\n");
        return -1;
    }
    /*
     * The liblwm2m library is now initialized with the functions that will be in
     * charge of communication
     */
    lwm2mH = lwm2m_init(&data);
    if (NULL == lwm2mH)
    {
        lwm2m_log(LOG_ERR, "lwm2m_init() failed\n");
        return -1;
    }

#if defined(WITH_TINYDTLS) || defined(WITH_MBEDTLS)
    data.lwm2mH = lwm2mH;
#endif

    /*
     * We configure the liblwm2m library with the name of the client - which shall be unique for each client -
     * the number of objects we will be passing through and the objects array
     */
    result = lwm2m_configure(lwm2mH, name, NULL, NULL, OBJ_COUNT, objArray);
    if (result != 0)
    {
        lwm2m_log(LOG_ERR, "lwm2m_configure() failed: 0x%X\n", result);
        return -1;
    }

#ifdef CSP_LINUXHOST
    signal(SIGINT, handle_sigint);
#endif

    /**
     * Initialize value changed callback.
     */
    init_value_change(lwm2mH);

    /*
     * As you now have your lwm2m context complete you can pass it as an argument to all the command line functions
     * precedently viewed (first point)
     */
    for (i = 0 ; commands[i].name != NULL ; i++)
    {
        commands[i].userData = (void *)lwm2mH;
    }
    lwm2m_log(LOG_DEBUG, "LWM2M Client \"%s\" started on port %s\n", name, localPort);
    lwm2m_log(LOG_DEBUG, "> ");
    /*
     * We now enter in a while loop that will handle the communications from the server
     */
    while (0 == g_quit)
    {
        struct timeval tv;
        fd_set readfds;

        if (g_reboot)
        {
            time_t tv_sec;

            tv_sec = lwm2m_gettime();

            if (0 == reboot_time)
            {
                reboot_time = tv_sec + 5;
            }
            if (reboot_time < tv_sec)
            {
                /*
                 * Message should normally be lost with reboot ...
                 */
                lwm2m_log(LOG_ERR, "reboot time expired, rebooting ...\n");
                system_reboot();
            }
            else
            {
                tv.tv_sec = reboot_time - tv_sec;
            }
        }
        else if (batterylevelchanging && (lwm2mH->state == STATE_READY))
        {
            update_battery_level(lwm2mH);
            tv.tv_sec = 5;
        }
        else
        {
            tv.tv_sec = 6;
        }
        tv.tv_usec = 0;

        FD_ZERO(&readfds);
        FD_SET(data.sock, &readfds);

        /*
         * This function does two things:
         *  - first it does the work needed by liblwm2m (eg. (re)sending some packets).
         *  - Secondly it adjusts the timeout value (default 60s) depending on the state of the transaction
         *    (eg. retransmission) and the time between the next operation
         */
        result = lwm2m_step(lwm2mH, &(tv.tv_sec));
        lwm2m_log(LOG_INFO, " -> State: ");
        switch (lwm2mH->state)
        {
        case STATE_INITIAL:
            lwm2m_log(LOG_INFO, "STATE_INITIAL\n");
            break;
        case STATE_BOOTSTRAP_REQUIRED:
            lwm2m_log(LOG_INFO, "STATE_BOOTSTRAP_REQUIRED\n");
            break;
        case STATE_BOOTSTRAPPING:
            lwm2m_log(LOG_INFO, "STATE_BOOTSTRAPPING\n");
            break;
        case STATE_REGISTER_REQUIRED:
            lwm2m_log(LOG_INFO, "STATE_REGISTER_REQUIRED\n");
            break;
        case STATE_REGISTERING:
            lwm2m_log(LOG_INFO, "STATE_REGISTERING\n");
            break;
        case STATE_READY:
            lwm2m_log(LOG_INFO, "STATE_READY\n");
            break;
        default:
            lwm2m_log(LOG_INFO, "Unknown...\n");
            break;
        }
        if (result != 0)
        {
            lwm2m_log(LOG_INFO, "lwm2m_step() failed: 0x%X\n", result);
#ifdef LWM2M_BOOTSTRAP
            if(previousState == STATE_BOOTSTRAPPING)
            {
#ifdef WITH_LOGS
                lwm2m_log(LOG_DEBUG, "[BOOTSTRAP] restore security and server objects\n");
#endif
                prv_restore_objects(lwm2mH);
                lwm2mH->state = STATE_INITIAL;
            }
            else
#endif
                return -1;
        }
#ifdef LWM2M_BOOTSTRAP
        update_bootstrap_info(&previousState, lwm2mH);
#endif
        /*
         * This part will set up an interruption until an event happen on SDTIN or the socket until "tv" timed out (set
         * with the precedent function)
         */
        result = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);

        if (result < 0)
        {
            if (errno != EINTR)
            {
              lwm2m_log(LOG_ERR, "Error in select(): %d %s\n", errno, strerror(errno));
            }
        }
        else if (result > 0)
        {
            uint8_t buffer[MAX_PACKET_SIZE];
            int numBytes;

            /*
             * If an event happens on the socket
             */
            if (FD_ISSET(data.sock, &readfds))
            {
#ifdef WITH_MBEDTLS
                numBytes = connection_handle_packet(data.connList, buffer, sizeof(buffer));
                if (numBytes < 0)
                {
                    lwm2m_log(LOG_ERR, "error handling message %d\n", numBytes);
                } else if( numBytes > 0)
                {
                    conn_s_updateRxStatistic(objArray[7], numBytes, false);
                }
#else
                struct sockaddr_storage addr;
                socklen_t addrLen;

                addrLen = sizeof(addr);
                /*
                 * We retrieve the data received
                 */
                numBytes = recvfrom(data.sock, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&addr, &addrLen);

                if (0 > numBytes)
                {
                    lwm2m_log(LOG_ERR, "Error in recvfrom(): %d %s\n", errno, strerror(errno));
                }
                else if (0 < numBytes)
                {
#if LWIP_IPV6
                    char s[INET6_ADDRSTRLEN];
#else
                    char s[INET_ADDRSTRLEN];
#endif
                    in_port_t port;

#if defined(WITH_TINYDTLS)
                    dtls_connection_t * connP;
#else
                    connection_t * connP;
#endif
                    if (AF_INET == addr.ss_family)
                    {
                        struct sockaddr_in *saddr = (struct sockaddr_in *)&addr;
                        inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET_ADDRSTRLEN);
                        port = saddr->sin_port;
                    }
#if LWIP_IPV6
                    else if (AF_INET6 == addr.ss_family)
                    {
                        struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)&addr;
                        inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
                        port = saddr->sin6_port;
                    }
#endif
                    else
                    {
                        lwm2m_log(LOG_ERR, "addr family %d err \n", addr.ss_family);
                        return ;
                    }
                    lwm2m_log(LOG_ERR, "%d bytes received from [%s]:%hu\n", numBytes, s, ntohs(port));

                    /*
                     * Display it in the STDERR
                     */
                    output_buffer(stderr, buffer, numBytes, 0);

                    connP = connection_find(data.connList, &addr, addrLen);
                    if (connP != NULL)
                    {
                        /*
                         * Let liblwm2m respond to the query depending on the context
                         */
#if defined(WITH_TINYDTLS)
                        int result = connection_handle_packet(connP, buffer, numBytes);
                        if (0 != result)
                        {
                             lwm2m_log(LOG_ERR, "error handling message %d\n",result);
                        }
#else
                        lwm2m_handle_packet(lwm2mH, buffer, numBytes, connP);
#endif
                        conn_s_updateRxStatistic(objArray[7], numBytes, false);
                    }
                    else
                    {
                        lwm2m_log(LOG_ERR, "received bytes ignored!\n");
                    }
                }
#endif /* WITH_MBEDTLS */
            }
        }
    }
    /*
     * Finally when the loop is left smoothly - asked by user in the command line interface - we unregister our client from it
     */
    if (g_quit == 1)
    {
#if defined(WITH_TINYDTLS) || defined(WITH_MBEDTLS)
        free(pskBuffer);
#endif

#ifdef LWM2M_BOOTSTRAP
        close_backup_object();
#endif
        lwm2m_deinit(lwm2mH);
    }
    close(data.sock);
    connection_free(data.connList);

    clean_security_object(objArray[0]);
    lwm2m_free(objArray[0]);
    clean_server_object(objArray[1]);
    lwm2m_free(objArray[1]);
    free_object_device(objArray[2]);
    free_object_firmware(objArray[3]);
    free_object_location(objArray[4]);
    free_test_object(objArray[5]);
    free_object_conn_m(objArray[6]);
    free_object_conn_s(objArray[7]);
    acl_ctrl_free_object(objArray[8]);

#ifdef MEMORY_TRACE
    if (g_quit == 1)
    {
        trace_print(0, 1);
    }
#endif

    return 0;
}

static void lwm2m_client_help_command( int argc, char **argv )
{
    int i = 0;
    lwm2m_log(LOG_INFO, "Usage: lwm2m [-c] [options]\n");
    lwm2m_log(LOG_INFO, "       lwm2m [-h]\n");
    for(i = 0; commands[i].name != NULL; i++)
    {
        lwm2m_log(LOG_INFO, "%s    %s\n", commands[i].name, commands[i].shortDesc);
        if(commands[i].longDesc != NULL)
        {
            lwm2m_log(LOG_INFO, "                %s\n", commands[i].longDesc);
        }
    }
}

void lwm2m_client_command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    int i;
    char buffer[MAX_PACKET_SIZE] = {0};

    if ((argc < 2) || (argv == NULL))
    {
        lwm2m_log(LOG_ERR, "Invalid command\n" );
        return ;
    }

    if (strcmp( argv[1], "-h" ) == 0 )
    {
        lwm2m_client_help_command( argc - 2, &argv[2] );
        return;
    }

    if(strcmp( argv[1], "-c" ) != 0)
    {
        lwm2m_log(LOG_ERR, "Invalid command, please use command \"lwm2m -c [option]\"\n");
        return;
    }

    for(i = 2; i < argc; i ++)
    {
       sprintf(buffer, "%s %s", buffer, argv[i]);
    }

    /*
     * We call the corresponding callback of the typed command passing it the buffer for further arguments
     */
    handle_command(commands, (char*)buffer);
}

int lwm2m_client_cli_register( void )
{
    if( 0 == aos_cli_register_commands( lwm2m_client_message_cmd, 1 ) )
        return -1;
    else
        return 0;
}
