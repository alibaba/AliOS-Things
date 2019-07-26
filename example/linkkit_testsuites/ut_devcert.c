#ifdef SDK_TEAM_TEST
#include "iot_import.h"
#include "iot_export.h"
#include "cut.h"
#include "ut_devcert.h"

typedef struct {
    const char *sname;
    const char *cname;
    const char *pk;
    const char *ps;
    const char *dn;
    const char *ds;
} ut_devcert_map_t;

typedef struct {
    const char *sname;
    const char *cname;
    int index;
    const char *pk;
    const char *ps;
    const char *dn;
    const char *ds;
} ut_subdevcert_map_t;

static ut_devcert_map_t g_ut_devcert_map[] = {
#if defined(COAP_COMM_ENABLED)
    {"test_coap_normal",                                     "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_coap_normal_setup",                 "Qg1t1U4OS4jK6JfiumLO6ZUreUz0twO2"},
    {"ut_coap_deinit",                                       "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_coap_deinit_setup",                 "wfoHLJFInMtvZ4sj1SIiCwg63v5P0bEm"},
    {"ut_coap_auth",                                         "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_coap_auth_setup",                   "PlYAosNA2pwf97UXpvZPA2WNRKcaYs71"},
    {"ut_coap_getmsg",                                       "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_coap_getmsg_setup",                 "UInoRO89SN2Tmgr2HO7vhKjGIRlc4s1f"},
    {"ut_coap_init",                                         "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_coap_init_setup",                   "vM2XkW7ldvGMjIT71IYsPnx9i5DM8Zg7"},
    {"ut_coap_sendmsg",                                      "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_coap_sendmsg_setup",                "6jIXSvf6ZGSoe1uqIymPj5ey1QIx03xp"},
    {"ut_coap_yield",                                        "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_coap_yield_setup",                  "WFhpv90WJP7Jreqi8eTEEeEufOddLHcA"},
#endif
#if defined(HTTP_COMM_ENABLED)
    {"test_http_api_sendmsg",                                "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_http_sendmsg_setup",                "cuNYbomcgvBhJfh4b6JV7R6lUCtKYcwZ"},
    {"test_http_normal",                                     "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_http_normal_setup",                 "WQy7yrfNIKhO9Re090SPX6c1Gffbhxbt"},
#endif
#if defined(MQTT_COMM_ENABLED)
    {"test_mqtt_api_construct",                              "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_mqtt_construct_setup",              "o8YIWhPKukQYCeewcWsUJ8zhhRJ3DGiK"},
    {"test_mqtt_api_publish",                                "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_mqtt_publish_setup",                "fcWP0Wu34f0qarmi8U0o1cFmCiNbZ1G5"},
    {"test_mqtt_api_subscribe",                              "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_mqtt_subscribe_setup",              "BdRI4lTbeZBct2e5R1Uhnvsp8IKjoBb7"},
    {"test_mqtt_api_unsubscribe",                            "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_mqtt_unsubscribe_setup",            "WdspU5nbgC25t9NdVMiWNIaU1hY3BJ7E"},
    {"test_mqtt_api_yield",                                  "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_mqtt_yield_setup",                  "tPEwUUehSr4iWzFaM2rXTo0q052SUltK"},
    {"test_mqtt_api_checkstate",                             "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_mqtt_chkstate_setup",               "J4OXmxL18RZTdQuEOZnpy2dKdJUevPc2"},
    {"test_mqtt_multithread",                                "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_mqtt_multithread_setup",            "A1524QoWnHCVVcmPegKlniexUD7QXx3T"},
    {"test_mqtt_normal",                                     "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_mqtt_normal_setup",                 "FetcERtgkaEuqSN54crUdUy8otZgOE2L"},
    {"test_mqtt_stress",                                     "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_mqtt_stress_setup",                 "rKfExLOSUgB6urBs6atcG4aZTQzE2AVW"},
    {"MQTT_CHECK",                                           "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "utest_mqtt_check_setup",                 "0NTdm9xpUepmGPMFXRvmPRPhQFfRcXmh"},
    {"MQTT_CONS",                                            "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "utest_mqtt_cons_setup",                  "BhUSRflNwJCXeq5K02ozcbgb8rnXwsj5"},
    {"MQTT_NORMAL",                                          "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "utest_mqtt_normal_setup",                "5BnULD4kJW4tgGJTXnGz4916jY6b9ouI"},
    {"MQTT_PUB",                                             "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "utest_mqtt_pub_setup",                   "wjCMgwx9pMP1V2pktBVVizwImCFpZUqw"},
    {"MQTT_SUB",                                             "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "utest_mqtt_sub_setup",                   "0AnpP3TYTyT39JW7k9NkScXpaILH3jbx"},
    {"MQTT_UNSUB",                                           "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "utest_mqtt_unsub_setup",                 "OJX7oNg7gdqJ4iepLhDXFfnSaN0M6TpT"},
    {"MQTT_YIELD",                                           "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "utest_mqtt_yield_setup",                 "7DkZqP7ROFRLOVs2p5gY5e8F8GJTFSop"},
#endif
#if defined(OTA_ENABLED)
    {"test_ota_api_fetch",                                   "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_ota_fetch_setup",                   "kpMpfPeVBVegJLNNck9Y2s6dgNo5Sh8P"},
    {"test_ota_api_init",                                    "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_ota_init_setup",                    "UGunMnU85BPnJE5qePOq18gZA1bpBhvF"},
    {"test_ota_api_deinit",                                  "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_ota_deinit_setup",                  "kxNZA8sqQNyEDAaGAYNdboGqWloN8zRk"},
    {"test_ota_api_ioctl",                                   "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_ota_ioctl_setup",                   "AjZiWO8H24EYTq1E4pMP8nkOWYjklYj8"},
    {"test_ota_api_report_version",                          "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_ota_reportver_setup",               "mhwMG84jnve4xB84tjuEgJHWIhhBJ0zg"},
    {"test_ota_api_report_progress",                         "setup",       "a1qev6AHaVv",       "TlNTxxJ1DZsWddUG",        "test_ota_reportprogress_setup",          "f1Drnha5tgcfvRFQAuhw0CP9cZDnqutM"},
#endif
#if !defined(DEPRECATED_LINKKIT) 
    {"test_linkkit_single_api_close",                        "setup",       "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_close_setup",                       "sRivM5HCSf4fDNjquWNmUSfjdqiEgHpf"},
    {"test_linkkit_single_api_connect",                      "setup",       "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_connect_setup",                     "y5YNKxKhaZJoPMLGkdbW2kQ0ynGE1ZwD"},
    {"test_linkkit_single_api_open",                         "setup",       "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_open_setup",                        "eKwdYgE5OZ0ptoWQEWH3ff5MYOMgkQQk"},
    {"test_linkkit_single_api_report",                       "setup",       "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_report_setup",                      "u9Xzvj93ufxiBIVsYqMIZlinZdFMeWmV"},
    {"test_linkkit_single_api_yield",                        "setup",       "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_yield_setup",                       "CRgSQhY2jgzy67v4AM9DwbSQ6UmlrdlC"},
    {"test_linkkit_single_multithread",                      "setup",       "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_multithread_setup",                 "C2H1RbWzM22RZ2rivuZpjvfJxlHdUToX"},
    {"test_linkkit_single_normal",                           "setup",       "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_normal_setup",                      "nutG5O5bDnPkuYfX7ITWA1iZ8Q7i90Ke"},
    {"test_linkkit_gateway_api_close",                       "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_gw_close_setup",                    "8ZYAF3UigeoQwLeLLIy276szYao8YPfW"},
    {"test_linkkit_gateway_api_connect",                     "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_gw_connect_setup",                  "gQUpaR7YvM9t1hteK51itWD8F8TFRXKG"},
    {"test_linkkit_gateway_api_open",                        "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_gw_open_setup",                     "ijNvwKP8NIQB2NlCIwHvJN4aYlONGToH"},
    {"test_linkkit_gateway_api_report",                      "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_gw_report_setup",                   "a12W1DfeyK9Xx8XRVFjZYeJdlwq5fyxe"},
    {"test_linkkit_gateway_api_TriggerEvent",                "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_gw_evt_setup",                      "Yz1Ftoi7xd38aLy2x6zepHD2CkvC6vMv"},
    {"test_linkkit_gateway_api_yield",                       "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_gw_yield_setup",                    "YP8UmFJTFlkQUGsumPsAX5yloGFHzQkg"},
    {"test_linkkit_gateway_multithread",                     "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_gw_multithread_setup",              "GTHbeHBw3AjeAMhTYbdckSLzYV9rCWn2"},
    {"test_linkkit_gateway_normal",                          "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_gw_normal_setup",                   "mIXUZSOC3csNqtSMQDCSeWxlOh3NrzwS"},
    {"test_linkkit_gateway_subdev_api_report",               "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_gw_subreport_setup",                "lhN1VbVqEWMBCpDzKaK6srOE66WKtxLE"},
    {"test_linkkit_gateway_subdev_api_TriggerEvent",         "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_gw_subevt_setup",                   "drlfX2tVkOOkRtcj9TvHc7hKCa7Z6wXq"},
#endif                                  
#if defined(DEPRECATED_LINKKIT) && !defined(DEVICE_MODEL_GATEWAY)                     
    {"test_linkkit_single_api_end",                          "setup",       "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapi_end_setup",                  "XxhXofLbYsZE69MtAvLgSCIFj5CL6BUA"},
    {"test_linkkit_single_api_get_value",                    "setup",       "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapi_get_value_setup",            "nktyc7MEGkNwOqMr14cZqhM3IXbuAL1O"},
    {"test_linkkit_single_api_post_property",                "setup",       "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapi_post_property_setup",        "aQQUv5D6AXRmNTtQ8LSZGD1p3IdTWJNH"},
    {"test_linkkit_single_api_set_value",                    "setup",       "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapi_set_value_setup",            "gnj1Xfg5Kaut2Yi4mYmBym2UDC1QkEx1"},
    {"test_linkkit_single_api_start",                        "setup",       "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapi_start_setup",                "MR6aQSaB72O2mkwvweWvlRqLEMvI9coA"},
    {"test_linkkit_single_api_start",                        "case_08",     "a2qsmiNsgm5",       "2gN8kTYfNbZyia5v",        "test_oldapi_start_08",                   "bHXzBIbv6nboBlX1tBc6pUzarSrmh48R"},
    {"test_linkkit_single_api_start",                        "case_09",     "a6wNacbZ1sH",       "li917WJ22HQ2q6gZ",        "test_oldapi_start_09",                   "LbKVrbIJ3bcsPx10iPN1nuM8JouyGl6R"},
    {"test_linkkit_single_api_start",                        "case_10",     "a3tOpbVXK8C",       "cTylLiWfKwjtUUEu",        "test_oldapi_start_10",                   "8O4ZRs8mqW0ucSJwAvxUDCv2VOfd03w5"},
    {"test_linkkit_single_api_start",                        "case_11",     "a3tOpbVXK8C",       "cTylLiWfKwjtUUEu",        "test_oldapi_start_10",                   "8O4ZRs8mqW0ucSJwAvxUDCv2VOfd03w5"},
    {"test_linkkit_single_api_start",                        "case_12",     "a5n8LDnCiHI",       "67njorvnHTz6fT75",        "test_oldapi_start_12",                   "KFyiQs7gIOeNPXOKtNyWqNg2xkjX7G3y"},
    {"test_linkkit_single_api_trigger_event",                "setup",       "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapi_trigger_event_setup",        "YjnRultseYujME8hPMkkzhbksD5i7uXh"},
    {"test_linkkit_single_multithread",                      "setup",       "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapi_multithread_setup",          "ag2Lj8VqmqNAJ22EKjLmDXPW5RR63x3N"},
    {"test_linkkit_single_normal",                           "setup",       "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapi_normal_setup",               "FbMsw4mGuVpNgxq2IfgXdhzN0URb9azw"},
#endif                    
#if defined(DEPRECATED_LINKKIT) && defined(DEVICE_MODEL_GATEWAY)                    
    {"test_linkkit_gateway_api_delete_extinfos",             "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_delextinfo_setup",        "yuTrV852sMHnzfw6H5SxqYyA0OLNmjHf"},
    {"test_linkkit_gateway_api_devinfoget",                  "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_devinfoget_setup",        "WrELC92mCNFFMpspg8jD7KYoEBaj9iZT"},
    {"test_linkkit_gateway_api_event",                       "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_event_setup",             "6N94xNiSKTuejfvRrLZjX64yurUP68jo"},
    {"test_linkkit_gateway_api_eventsync",                   "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_evtsync_setup",           "TwxssU0b94OP7kwvjUhuZZgZUSQc5IqM"},
    {"test_linkkit_gateway_api_eventsync",                   "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_evtsync_setup",           "TwxssU0b94OP7kwvjUhuZZgZUSQc5IqM"},
    {"test_linkkit_gateway_api_exit",                        "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_exit_setup",              "JCazqVD44T1dCBcG5YPSnjjJR0seUBsr"},
    {"test_linkkit_gateway_api_fota_init",                   "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_fotainit_setup",          "NvNB5LcKbvqbuvDJVyPHbWaxqLh9m1uA"},
    {"test_linkkit_gateway_api_fota_init",                   "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_fotainit_setup",          "NvNB5LcKbvqbuvDJVyPHbWaxqLh9m1uA"},
    {"test_linkkit_gateway_api_get_num_devices",             "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_numdev_setup",            "iQ37Y35tX7UIUiPEt6ZmxHQPQ2uoMRMH"},
    {"test_linkkit_gateway_api_getparams",                   "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_getparam_setup",          "8YMbPoncMN5qeOVtPjOYTqfB9HOCdXte"},
    {"test_linkkit_gateway_api_init",                        "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_init_setup",              "275G2hMo6eIg6atLTChTKhVtvy76Dsp2"},
    {"test_linkkit_gateway_api_invoke_fota_service",         "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_ivkfota_setup",           "rJQdxZDgaKLEbRDn8BpZ9rMA2SaDRVU6"},
    {"test_linkkit_gateway_api_post_extinfos",               "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_pextinfo_setup",          "9VRnhAEw0iW2cgJ7zV3vwpmmYX8zRjPV"},
    {"test_linkkit_gateway_api_post_rawdata",                "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_prawdata_setup",          "DhysBOH10GdrFVWBEynmczE8q2xKdvdy"},
    {"test_linkkit_gateway_api_property",                    "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_prop_setup",              "mZfXdLfL2nM1Jo2WENVXvFOZeUnIic0B"},
    {"test_linkkit_gateway_api_propertysync",                "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_propsync_setup",          "bjK6WTmdiLb29VhynvX4KRqehplme6M4"},
    {"test_linkkit_gateway_api_subdev_creat",                "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_subcrt_setup",            "8ga7uw3IPaUCrXOKrrXanqurtMKRIq2W"},
    {"test_linkkit_gateway_api_subdev_destroy",              "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_destroy_setup",           "eVLkqdhiJKwvFW3HlgtqDTW1ReMfLRcG"},
    {"test_linkkit_gateway_api_subdev_login",                "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_sublogin_setup",          "cVvKsHrx2IJEqCpzOBP3ZpAEo0tOz6sA"},
    {"test_linkkit_gateway_api_subdev_logout",               "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_sublogout_setup",         "ohehG2iwqSPnwMmGKzieW3Y8CRk3I10s"},
    {"test_linkkit_gateway_api_subdev_register",             "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_subreg_setup",            "iNO9uYeZt4Pm38qh3JFts0KopEJIog8W"},
    {"test_linkkit_gateway_api_subdev_unregister",           "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_subunreg_setup",          "8RyhRGsj35si8s4OKgrh4zAqYCxljOy7"},
    {"test_linkkit_gateway_multhread",                       "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_multithread_setup",       "5apqZk6CZvGjLsxpopCp04TOnhrcniXU"},
    {"test_linkkit_gateway_normal",                          "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_normal_setup",            "ZoFdBc7248S8ssBFpQkCeteNLi2tZ04h"},
    {"test_linkkit_gateway_subdev_multhread",                "setup",       "a1xnvMLMpr3",       "jBkS12XffoIRxsiw",        "test_oldapi_gw_submulti_setup",          "DxDoNfWHY1y3uIBHwe4H3BEw92Ap9HX3"},
#endif        
};        
 
static ut_subdevcert_map_t g_ut_subdevcert_map[] = {
#if !defined(DEPRECATED_LINKKIT)
    {"test_linkkit_gateway_api_close",                      "setup", 0,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_sub_close_setup_01",               "00Ff24oUgzr4CwZpSidh3YUjYmHJE3Pj"},
    {"test_linkkit_gateway_api_connect",                    "setup", 0,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_sub_connect_setup_01",             "iFzuFwJ7xAqA3jRGjBNDlgmslZL0DZn2"},
    {"test_linkkit_gateway_api_open",                       "setup", 0,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_sub_open_setup_01",                "pmZNR4J09U4jkv6WT2R2z6MJcR2DKMG3"},
    {"test_linkkit_gateway_api_report",                     "setup", 0,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_sub_report_setup_01",              "pnooNdTIYGD9cRIlnVqsqAqvM2E50yfG"},
    {"test_linkkit_gateway_api_TriggerEvent",               "setup", 0,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_sub_evt_setup_01",                 "YFLtc1PRMPO8bF0Satgan8fd6Ei2AaEW"},
    {"test_linkkit_gateway_multithread",                    "setup", 0,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_sub_multithread_setup_01",         "Z5LxEacql7u5aRiQmLsCQh6yDcBolVMQ"},
    {"test_linkkit_gateway_normal",                         "setup", 0,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_sub_normal_setup_01",              "weuzluuegp2IYb8t016dKVvIwSXOV9Rn"},
    {"test_linkkit_gateway_subdev_api_report",              "setup", 0,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_sub_subreport_setup_01",           "nUSMTT8Qtyi5IIqxxizhNqarF678r5bJ"},
    {"test_linkkit_gateway_subdev_api_report",              "setup", 1,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_sub_subreport_setup_02",           "9IkXSOUQcTQVq3YyGlvPMLYL1UZqZiHV"},
    {"test_linkkit_gateway_subdev_api_report",              "setup", 2,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_sub_subreport_setup_03",           "O1Jo29llyPx7CymWDeeUkxToUh7fTbK0"},
    {"test_linkkit_gateway_subdev_api_TriggerEvent",        "setup", 0,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_sub_subevt_setup_01",              "iscgoFARM1ynDnxqCGqsqFNAsipLKJEh"},
#endif
#if defined(DEPRECATED_LINKKIT) && defined(DEVICE_MODEL_GATEWAY) 
    {"test_linkkit_gateway_api_get_num_devices",            "setup", 0,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapisub_numdev_setup_01",        "7v0GP1dz65QEEFIXOMsEy7nfhvGBtSQ7"},
    {"test_linkkit_gateway_api_get_num_devices",            "setup", 1,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapisub_numdev_setup_02",        "3ReN9L0ywuWipJ28d5aLhYITtyFMmRAs"},
    {"test_linkkit_gateway_api_subdev_creat",               "setup", 0,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapisub_create_setup_01",        "OkLSlVMY0bQxEBY8tAeeN1cIYfETGiyk"},
    {"test_linkkit_gateway_api_subdev_destroy",             "setup", 0,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapisub_destroy_setup_01",       "JgWLz66DWRf5K8IX9rwEr9KkBM8T2DzD"},
    {"test_linkkit_gateway_api_subdev_login",               "setup", 0,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapisub_login_setup_01",         "ez7hyjiCb2X4C4ieEu6dYKKHQBBgyacE"},
    {"test_linkkit_gateway_api_subdev_login",               "setup", 1,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapisub_login_setup_02",         "zttCjUrZiSEaNDthkCROxr1JEPpm2Kr8"},
    {"test_linkkit_gateway_api_subdev_logout",              "setup", 0,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapisub_logout_setup_01",        "8MAignCWyqPiaarftKM8y9YqEHqE3eiQ"},
    {"test_linkkit_gateway_api_subdev_register",            "setup", 0,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapisub_reg_setup_01",           "pVHKTNHYUe9ie6E6FIlPx98gXMaYpk16"},
    {"test_linkkit_gateway_api_subdev_register",            "setup", 1,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapisub_reg_setup_02",           "NrVg5pFwJSvaQSpw1nwLPC7ZxwbFeKBd"},
    {"test_linkkit_gateway_api_subdev_unregister",          "setup", 0,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapisub_unreg_setup_01",         "MQ61apetciaYifeTnsFWdneWqrxhmNxL"},
    {"test_linkkit_gateway_subdev_multhread",               "setup", 0,     "a1eu1SVUzB8",       "JeDTfLiOdwl2gKWe",        "test_oldapisub_multi_setup_01",         "nYGJgy3FkbxCotVaFSqI4w3SLZq1fPKk"},
#endif
};

static int casecmp(const char *dest, const char *src)
{
    if ((strlen(dest) == strlen(src)) &&
        (memcmp(dest, src, strlen(src)) == 0)) {
        return 0;
    }

    return -1;
}

static void _ut_replace_cert(char **dest_pk, char **dest_ps, char **dest_dn, char **dest_ds, char *src_pk, char *src_ps,
                             char *src_dn, char *src_ds)
{
#if defined(DEPRECATED_LINKKIT)
    HAL_SetProductKey(src_pk);
    HAL_SetProductSecret(src_ps);
    HAL_SetDeviceName(src_dn);
    HAL_SetDeviceSecret(src_ds);
#endif
    *dest_pk = src_pk;
    *dest_ps = src_ps;
    *dest_dn = src_dn;
    *dest_ds = src_ds;
}

void ut_replace_cert(char **pk, char **ps, char **dn, char **ds, void *case_data)
{
    int index = 0;
    int map_size = sizeof(g_ut_devcert_map) / sizeof(ut_devcert_map_t);

    struct cut_case *curr_case = (struct cut_case *)case_data;

    if (pk == NULL || ps == NULL || dn == NULL || ds == NULL || curr_case == NULL) {
        return;
    }

    for (index = 0; index < map_size; index++) {
        if ((casecmp(g_ut_devcert_map[index].sname, curr_case->sname) == 0) &&
            (g_ut_devcert_map[index].cname != NULL && (casecmp(g_ut_devcert_map[index].cname, curr_case->cname) == 0))) {

            _ut_replace_cert(pk, ps, dn, ds, (char *)g_ut_devcert_map[index].pk, (char *)g_ut_devcert_map[index].ps,
                             (char *)g_ut_devcert_map[index].dn, (char *)g_ut_devcert_map[index].ds);
            return;
        }
    }

    for (index = 0; index < map_size; index++) {
        if ((casecmp(g_ut_devcert_map[index].sname, curr_case->sname) == 0) &&
            (g_ut_devcert_map[index].cname != NULL && (casecmp(g_ut_devcert_map[index].cname, "setup") == 0))) {
            _ut_replace_cert(pk, ps, dn, ds, (char *)g_ut_devcert_map[index].pk, (char *)g_ut_devcert_map[index].ps,
                             (char *)g_ut_devcert_map[index].dn, (char *)g_ut_devcert_map[index].ds);
            return;
        }
    }
}

static void _ut_replace_subcert(char *dest_pk, char *dest_ps, char *dest_dn, char *dest_ds, char *src_pk, char *src_ps,
                             char *src_dn, char *src_ds)
{
    memset(dest_pk,0,PRODUCT_KEY_MAXLEN);
    memcpy(dest_pk,src_pk,strlen(src_pk));

    if (dest_ps != NULL) {
        memset(dest_ps,0,PRODUCT_SECRET_MAXLEN);
        memcpy(dest_ps,src_ps,strlen(src_ps));
    }

    memset(dest_dn,0,DEVICE_NAME_MAXLEN);
    memcpy(dest_dn,src_dn,strlen(src_dn));

    if (dest_ds != NULL) {
        memset(dest_ds,0,DEVICE_SECRET_MAXLEN);
        memcpy(dest_ds,src_ds,strlen(src_ds));
    }
}

void ut_replace_subcert(char *pk, char *ps, char *dn, char *ds, int dev_index, void *case_data)
{
    int index = 0;
    int map_size = sizeof(g_ut_subdevcert_map) / sizeof(ut_subdevcert_map_t);

    struct cut_case *curr_case = (struct cut_case *)case_data;

    if (pk == NULL || dn == NULL || curr_case == NULL) {
        return;
    }

    for (index = 0; index < map_size; index++) {
        if ((casecmp(g_ut_subdevcert_map[index].sname, curr_case->sname) == 0) &&
            (g_ut_subdevcert_map[index].cname != NULL && (casecmp(g_ut_subdevcert_map[index].cname, curr_case->cname) == 0)) &&
            (g_ut_subdevcert_map[index].index == dev_index)) {

            _ut_replace_subcert(pk, ps, dn, ds, (char *)g_ut_subdevcert_map[index].pk, (char *)g_ut_subdevcert_map[index].ps,
                             (char *)g_ut_subdevcert_map[index].dn, (char *)g_ut_subdevcert_map[index].ds);
            return;
        }
    }

    for (index = 0; index < map_size; index++) {
        if ((casecmp(g_ut_subdevcert_map[index].sname, curr_case->sname) == 0) &&
            (g_ut_subdevcert_map[index].cname != NULL && (casecmp(g_ut_subdevcert_map[index].cname, "setup") == 0)) &&
            (g_ut_subdevcert_map[index].index == dev_index)) {

            _ut_replace_subcert(pk, ps, dn, ds, (char *)g_ut_subdevcert_map[index].pk, (char *)g_ut_subdevcert_map[index].ps,
                             (char *)g_ut_subdevcert_map[index].dn, (char *)g_ut_subdevcert_map[index].ds);
            return;
        }
    }
}

#endif