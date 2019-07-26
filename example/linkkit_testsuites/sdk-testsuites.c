/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "sdk-testsuites_internal.h"
#include "cut.h"
#include "app_entry.h"

static void team_test()
{
    /* Add cases from dev-team here */
#if (LINKKIT_TEST_CONFIG_HAL)
    ADD_SUITE(HAL_OS);
    ADD_SUITE(UTILS_STRING);
#endif

#if defined(MQTT_COMM_ENABLED)
    ADD_SUITE(INFO_REPORT);
    ADD_SUITE(MQTT_CHECK);
    ADD_SUITE(MQTT_CONS);
    ADD_SUITE(MQTT_NORMAL);
    ADD_SUITE(MQTT_PUB);
    ADD_SUITE(MQTT_SUB);
     ADD_SUITE(MQTT_UNSUB);
     ADD_SUITE(MQTT_YIELD);
 #ifndef MQTT_DIRECT
     ADD_SUITE(MQTT_PREAUTH);
 #endif
#endif
#ifdef HTTP2_COMM_ENABLED
    ADD_SUITE(HTTP2_BASIC);
    ADD_SUITE(HTTP2_STREAM);
#endif
#if defined(DEVICE_MODEL_GATEWAY) 
    ADD_SUITE(GW_TEST);
#endif
#if defined(DEVICE_MODEL_ENABLED)
   /*  ADD_SUITE(test_linkkit_single_normal);*/
    ADD_SUITE(test_linkkit_single_multithread);
    ADD_SUITE(test_linkkit_single_api_open);
    ADD_SUITE(test_linkkit_single_api_connect);
    ADD_SUITE(test_linkkit_single_api_close);
    ADD_SUITE(test_linkkit_single_api_yield);
    /* ADD_SUITE(test_linkkit_single_api_report);*/
    ADD_SUITE(OVERSEA_TEST);
/*    ADD_SUITE(DYNAMIC_REG);*/ 
#endif
}

int linkkit_main(void *paras)
{
    int    argc = 0;
    char   **argv = NULL;
    if(paras != NULL) {
        argc = ((app_main_paras_t *)paras)->argc;
        argv = ((app_main_paras_t *)paras)->argv; 
    }
#if (TEST_SUITES_LOG_LEVEL>0) && (TEST_SUITES_LOG_LEVEL<6)
    IOT_SetLogLevel(TEST_SUITES_LOG_LEVEL);
#else 
    IOT_SetLogLevel(IOT_LOG_NONE + 1);
#endif

#if !(FAST_VERIFY)
    team_test();
#endif  /* #if !(FAST_VERIFY) */

    cut_main(argc, argv);

    return 0;
}
