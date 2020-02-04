/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"

#include <lwm2m.h>
#include "mbedconnection.h"

#include "linkkit/infra/infra_md5.h"

#define TAG "lwm2m_app"

#define LWM2M_CLIENT_PORT_STR "56830"
#define LWM2M_SERVER_ADDR_STR "47.111.5.71"

#define MAX_PACKET_SIZE 1024

#define LWM2M_CLIENT_NAME_STR "a1k6Tex0gDo.lwm2m_device_1"
#define DEVICE_SECRET_STR     "PhOiCvauesPrLw4KUB4d5FMVQJZnnhXm"
#define DEVICE_NAME_STR       "lwm2m_device_1"
#define PRODUCT_KEY_STR       "a1k6Tex0gDo"

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

typedef struct
{
    lwm2m_object_t * securityObjP;
    lwm2m_object_t * serverObject;
    int sock;
    dtls_connection_t * connList;
    lwm2m_context_t * lwm2mH;
    int addressFamily;
} client_data_t;

static int g_quit = 0;

#define OBJ_COUNT 5
lwm2m_object_t * objArray[OBJ_COUNT];

/*
* Generate PSK key using device secret, device name, product key, random
*/
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
                LOGE(TAG, "Internal allocation failure !\n");
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
                LOGE(TAG, "result(%d), failed to change value!\n", result);
            }
            else
            {
                LOGI(TAG, "Battery value changed!\n");
                lwm2m_resource_value_changed(lwm2mH, uri);
            }
            lwm2m_data_free(1, dataP);
            return;
        }
        else
        {
            LOGE(TAG, "write not supported for specified resource!\n");
        }
        return;
    }
    else
    {
        LOGE(TAG, "Object not found !\n");
    }
}

/*
* Update resource /3/0/9 (battery level) value
*/
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
            LOGI(TAG, "New Battery Level: %d\n", level);
            handle_value_changed(context, &uri, value, valueLength);
        }
        level = rand() % 20;
        if (0 > level) level = -level;
        next_change_time = tv_sec + level + 10;
    }
}

/*
* Main loop
*/
int lwm2m_client(void)
{
    client_data_t data;
    int result;
    lwm2m_context_t * lwm2mH = NULL;
    int i;
    const char * localPort = LWM2M_CLIENT_PORT_STR;
    const char * server = LWM2M_SERVER_ADDR_STR;
    const char * serverPort = LWM2M_DTLS_PORT_STR;
    char * name = LWM2M_CLIENT_NAME_STR;
    char * deviceName = DEVICE_NAME_STR;
    char * deviceSecret = DEVICE_SECRET_STR;
    char * productKey = PRODUCT_KEY_STR;

    int lifetime = 300;
    int batterylevelchanging = 1;

    char  psk_key[IOTX_SIGN_LENGTH] = {0};
    char  psk_id[IOTX_SIGN_LENGTH] = {0};

    char * pskId = NULL;
    char * psk = NULL;
    uint16_t pskLen = -1;
    char * pskBuffer = NULL;

    memset(&data, 0, sizeof(client_data_t));
    data.addressFamily = AF_INET;

    /*
    * generate PSK ID, PSK key using triple information
    */
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
    if (psk != NULL)
    {
        pskLen = strlen(psk) / 2;
        pskBuffer = lwm2m_malloc(pskLen);

        if (NULL == pskBuffer)
        {
            LOGE(TAG, "Failed to create PSK binary buffer\n");
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
                LOGE(TAG, "Failed to parse Pre-Shared-Key HEXSTRING\n");
                return -1;
            }

            *b = ((l - xlate) << 4) + (r - xlate);
        }
    }

    char serverUri[50];
    int serverId = 123;
    sprintf (serverUri, "coaps://%s:%s", server, serverPort);

    objArray[0] = get_security_object(serverId, serverUri, pskId, pskBuffer, pskLen, false);
    if (NULL == objArray[0])
    {
        LOGE(TAG, "Failed to create security object\n");
        return -1;
    }
    data.securityObjP = objArray[0];

    objArray[1] = get_server_object(serverId, "U", lifetime, false);
    if (NULL == objArray[1])
    {
        LOGE(TAG, "Failed to create server object\n");
        return -1;
    }

    objArray[2] = get_object_device();
    if (NULL == objArray[2])
    {
        LOGE(TAG, "Failed to create Device object\n");
        return -1;
    }

    objArray[3] = get_object_firmware();
    if (NULL == objArray[3])
    {
        LOGE(TAG, "Failed to create Firmware object\n");
        return -1;
    }

    objArray[4] = get_object_location();
    if (NULL == objArray[4])
    {
        LOGE(TAG, "Failed to create location object\n");
        return -1;
    }

    /*
     * The liblwm2m library is now initialized with the functions that will be in
     * charge of communication
     */
    lwm2mH = lwm2m_init(&data);
    if (NULL == lwm2mH)
    {
        LOGE(TAG, "lwm2m_init() failed\n");
        return -1;
    }

    data.lwm2mH = lwm2mH;

    /*
     * We configure the liblwm2m library with the name of the client - which shall be unique for each client -
     * the number of objects we will be passing through and the objects array
     */
    result = lwm2m_configure(lwm2mH, name, NULL, NULL, OBJ_COUNT, objArray);
    if (result != 0)
    {
        LOGE(TAG, "lwm2m_configure() failed: 0x%X\n", result);
        return -1;
    }

restart:
    lwm2mH->state = STATE_INITIAL;

    /*
     *This call an internal function that create an socket
     */
    LOGI(TAG, "Trying to bind LWM2M Client to port %s\n", localPort);
    data.sock = create_socket(localPort, data.addressFamily);
    if (data.sock < 0)
    {
        LOGE(TAG, "Failed to open socket: %d %s\n", errno, strerror(errno));
        aos_msleep(5000);
        goto restart;
    }

    /*
     * We now enter in a while loop that will handle the communications from the server
     */
    while (0 == g_quit)
    {
        struct timeval tv;
        fd_set readfds;

        if (batterylevelchanging && (lwm2mH->state == STATE_READY))
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
        LOGI(TAG, " -> State: ");
        switch (lwm2mH->state)
        {
        case STATE_INITIAL:
            LOGI(TAG, "STATE_INITIAL\n");
            break;
        case STATE_BOOTSTRAP_REQUIRED:
            LOGI(TAG, "STATE_BOOTSTRAP_REQUIRED\n");
            break;
        case STATE_BOOTSTRAPPING:
            LOGI(TAG, "STATE_BOOTSTRAPPING\n");
            break;
        case STATE_REGISTER_REQUIRED:
            LOGI(TAG, "STATE_REGISTER_REQUIRED\n");
            break;
        case STATE_REGISTERING:
            LOGI(TAG, "STATE_REGISTERING\n");
            break;
        case STATE_READY:
            LOGI(TAG, "STATE_READY\n");
            break;
        default:
            LOGI(TAG, "Unknown...\n");
            break;
        }
        if (result != 0)
        {
            LOGI(TAG, "lwm2m_step() failed: 0x%X\n", result);
            close(data.sock);
            return -1;
        }

        /*
         * This part will set up an interruption until an event happen on SDTIN or the socket until "tv" timed out (set
         * with the precedent function)
         */
        result = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);

        if (result < 0)
        {
            if (errno != EINTR)
            {
                LOGE(TAG, "Error in select(): %d %s\n", errno, strerror(errno));
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
                numBytes = connection_handle_packet(data.connList, buffer, sizeof(buffer));
                if (numBytes < 0)
                {
                    LOGE(TAG, "error handling message %d\n", numBytes);
                }
            }
        }
    }
    /*
     * Finally when the loop is left smoothly - asked by user in the command line interface - we unregister our client from it
     */
    if (g_quit == 1)
    {
        free(pskBuffer);
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

    return 0;
}
