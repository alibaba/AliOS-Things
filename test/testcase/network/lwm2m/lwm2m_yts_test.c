/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "yunit/yunit.h"
#include "yunit/yts.h"

#include <aos/kernel.h>
#include <ulog/ulog.h>
#include <lwm2m.h>

#define TAG "lwm2m_yts"

void * lwm2m_connect_server(uint16_t secObjInstID,
                            void * userData)
{
    return NULL;
}

void lwm2m_close_connection(void * sessionH,
                            void * userData)
{

}

size_t
coap_serialize_message(void *packet, uint8_t *buffer, uint16_t buffer_len)
{
   return 0;
}

uint8_t lwm2m_buffer_send(void * sessionH,
                          uint8_t * buffer,
                          size_t length,
                          void * userdata)
{
    return 0;
}

void * lwm2m_malloc(size_t s)
{
    return aos_malloc(s);
}

void lwm2m_free(void * p)
{
    aos_free(p);
}

char * lwm2m_strdup(const char * str)
{
    int len = strlen(str) + 1;
    void * new = lwm2m_malloc(len);

    if(new == NULL)
        return NULL;
    return (char*) memcpy(new, str, len);
}


time_t lwm2m_gettime(void)
{
    return aos_now_ms()/1000;
}

static void test_lwm2m_data(void)
{
    int ret_i = -1;
    int size_i = 1;
    lwm2m_data_t * lwm2m_data = NULL;
    int test_integer = 10;
    int decode_integer = -1;
    char *test_uri_str = "/3/0/9";
    int test_uri_str_len = strlen(test_uri_str);
    lwm2m_uri_t uri;

    lwm2m_data = lwm2m_data_new(size_i);
    YUNIT_ASSERT( NULL != lwm2m_data );
    if (!lwm2m_data)
        return;

    lwm2m_data_encode(LWM2M_CODEC_TYPE_INTEGER, &test_integer, lwm2m_data);
    ret_i = lwm2m_data_decode(LWM2M_CODEC_TYPE_INTEGER, lwm2m_data, &decode_integer);
    YUNIT_ASSERT( 1 == ret_i && test_integer == decode_integer);

    ret_i = lwm2m_string_to_uri(test_uri_str, test_uri_str_len, &uri);
    YUNIT_ASSERT( test_uri_str_len == ret_i &&
                  uri.objectId == 3 && 
                  uri.instanceId == 0 && 
                  uri.resourceId == 9 );

    lwm2m_data_free(size_i, lwm2m_data);
    lwm2m_data = NULL;
}

#define OBJECT_NUM 3
#define TEST_OBJECT_ID 14310
static void test_lwm2m_proc(void)
{
    void *work = NULL;
    int ret_i;
    lwm2m_context_t *ctx = NULL;
    char * endpointName = "testlwm2mclient";
    lwm2m_object_t *objectArray[OBJECT_NUM] = { NULL };
    time_t timeout_sec = 5;
    lwm2m_object_t *testObject = NULL;

    ctx = lwm2m_init(NULL);
    YUNIT_ASSERT( NULL != ctx );
    if (NULL == ctx) {
        return;
    }

    objectArray[0] = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
    YUNIT_ASSERT( NULL != objectArray[0] );
    if (NULL == objectArray[0]) {
        goto exit;
    }
    memset(objectArray[0], 0, sizeof(lwm2m_object_t));
    objectArray[0]->objID = LWM2M_SECURITY_OBJECT_ID;

    objectArray[1] = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
    YUNIT_ASSERT( NULL != objectArray[1] );
    if (NULL == objectArray[1]) {
        goto exit;
    }
    memset(objectArray[1], 0, sizeof(lwm2m_object_t));
    objectArray[1]->objID = LWM2M_SERVER_OBJECT_ID;

    objectArray[2] = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
    YUNIT_ASSERT( NULL != objectArray[2] );
    if (NULL == objectArray[1]) {
        goto exit;
    }
    memset(objectArray[2], 0, sizeof(lwm2m_object_t));
    objectArray[2]->objID = LWM2M_DEVICE_OBJECT_ID;

    ret_i = lwm2m_configure(ctx, endpointName, NULL, NULL, OBJECT_NUM, objectArray);
    YUNIT_ASSERT( ret_i == 0);
    if (0 != ret_i) {
        goto exit;
    }

    testObject = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
    YUNIT_ASSERT( NULL != testObject );
    if (NULL == testObject) {
        goto exit;
    }

    memset(testObject, 0, sizeof(lwm2m_object_t));
    testObject->objID = TEST_OBJECT_ID;

    ret_i = lwm2m_add_object(ctx, testObject);
    YUNIT_ASSERT( ret_i == 0 );
    if (0 != ret_i) {
        goto exit;
    }

    ret_i = lwm2m_remove_object(ctx, TEST_OBJECT_ID);
    YUNIT_ASSERT( ret_i == 0 );
    if (0 != ret_i) {
        goto exit;
    }

    ret_i = lwm2m_step(ctx, timeout_sec);
    YUNIT_ASSERT(COAP_503_SERVICE_UNAVAILABLE == ret_i);

exit:
    lwm2m_free(objectArray[0]);
    lwm2m_free(objectArray[1]);
    lwm2m_free(objectArray[2]);

    lwm2m_deinit(ctx);
 }

static int init(void)
{

}

static int cleanup(void)
{

}

static void setup(void)
{

}

static void teardown(void)
{

}

static yunit_test_case_t aos_lwm2m_testcases[] = {
    { "lwm2m_data",        test_lwm2m_data },
    { "lwm2m_proc",        test_lwm2m_proc },
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "lwm2m", init, cleanup, setup, teardown, aos_lwm2m_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_lwm2m(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_lwm2m);
