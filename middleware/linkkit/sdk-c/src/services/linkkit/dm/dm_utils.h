/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _DM_UTILS_H_
#define _DM_UTILS_H_

#include "iotx_dm_internal.h"

//Constant Log
extern const char DM_UTILS_LOG_INVALID_PARAMETER[]                              DM_READ_ONLY;
extern const char DM_UTILS_LOG_MEMORY_NOT_ENOUGH[]                              DM_READ_ONLY;
extern const char DM_UTILS_LOG_SIGN_METHOD_NOT_FOUND[]                          DM_READ_ONLY;
extern const char DM_UTILS_LOG_DATA_TYPE_NOT_EXIST[]                            DM_READ_ONLY;
extern const char DM_UTILS_LOG_DATA_TYPE_INVALID[]                              DM_READ_ONLY;
extern const char DM_UTILS_LOG_GET_INVALID_DEV_ID[]                             DM_READ_ONLY;
extern const char DM_UTILS_LOG_OPERATOR_RES[]                                   DM_READ_ONLY;
extern const char DM_UTILS_LOG_DATA_TYPE_HAS_NO_PARSE_FUNC[]                    DM_READ_ONLY;
extern const char DM_UTILS_LOG_DATA_TYPE_HAS_NO_SET_FUNC[]                      DM_READ_ONLY;
extern const char DM_UTILS_LOG_JSON_PARSE_FAILED[]                              DM_READ_ONLY;
extern const char DM_UTILS_LOG_JSON_PARSE_INDEX_FAILED[]                        DM_READ_ONLY;
extern const char DM_UTILS_LOG_TSL_KEY_ID_INVALID[]                             DM_READ_ONLY;
extern const char DM_UTILS_LOG_TSL_PROPERTY_NOT_EXIST[]                         DM_READ_ONLY;
extern const char DM_UTILS_LOG_TSL_EVENT_NOT_EXIST[]                            DM_READ_ONLY;
extern const char DM_UTILS_LOG_TSL_EVENT_DATA_NOT_EXIST[]                       DM_READ_ONLY;
extern const char DM_UTILS_LOG_TSL_SERIVCE_NOT_EXIST[]                          DM_READ_ONLY;
extern const char DM_UTILS_LOG_TSL_EVENT_NOT_EXIST[]                            DM_READ_ONLY;
extern const char DM_UTILS_LOG_TSL_IDENTIFIER_NOT_EXIST[]                       DM_READ_ONLY;
extern const char DM_UTILS_LOG_TSL_PROPERTY_SET_FAILED[]                        DM_READ_ONLY;
extern const char DM_UTILS_LOG_TSL_PROPERTY_GET_FAILED[]                        DM_READ_ONLY;
extern const char DM_UTILS_LOG_TSL_PROPERTY_ASSEMBLE_FAILED[]                   DM_READ_ONLY;
extern const char DM_UTILS_LOG_TSL_EVENT_SET_FAILED[]                           DM_READ_ONLY;
extern const char DM_UTILS_LOG_TSL_EVENT_GET_FAILED[]                           DM_READ_ONLY;
extern const char DM_UTILS_LOG_TSL_EVENT_OUTPUT_ASSEMBLE_FAILED[]               DM_READ_ONLY;
extern const char DM_UTILS_LOG_DM_INIT_FAILED[]                                 DM_READ_ONLY;
extern const char DM_UTILS_LOG_CM_EVENT_UNKNOWN[]                               DM_READ_ONLY;
extern const char DM_UTILS_LOG_CM_INIT_FAILED[]                                 DM_READ_ONLY;
extern const char DM_UTILS_LOG_CM_CLOUD_CONNECTIVITY_CREATE_FAILED[]            DM_READ_ONLY;
extern const char DM_UTILS_LOG_CM_CLOUD_CONNECTIVITY_CONNECT_FAILED[]           DM_READ_ONLY;
extern const char DM_UTILS_LOG_CM_CLOUD_CONNECTIVITY_NOT_EXIST[]                DM_READ_ONLY;
extern const char DM_UTILS_LOG_CM_LOCAL_CONNECTIVITY_CREATE_FAILED[]            DM_READ_ONLY;
extern const char DM_UTILS_LOG_CM_LOCAL_CONNECTIVITY_CONNECT_FAILED[]           DM_READ_ONLY;
extern const char DM_UTILS_LOG_CM_LOCAL_CONNECTIVITY_NOT_EXIST[]                DM_READ_ONLY;
extern const char DM_UTILS_LOG_CM_SEND_MESSAGE_FAILED[]                         DM_READ_ONLY;
extern const char DM_UTILS_LOG_CM_SEND_RESULT[]                                 DM_READ_ONLY;
extern const char DM_UTILS_LOG_DMGR_INIT_FAILED[]                               DM_READ_ONLY;
extern const char DM_UTILS_LOG_DMGR_SERVICE_CLOUD_REGISTER_FAILED[]             DM_READ_ONLY;
extern const char DM_UTILS_LOG_DMGR_SERVICE_LOCAL_REGISTER_FAILED[]             DM_READ_ONLY;
extern const char DM_UTILS_LOG_DMGR_UNSUPPORT_DEVICE_TYPE[]                     DM_READ_ONLY;
extern const char DM_UTILS_LOG_DMGR_DEVICE_NOT_FOUND[]                          DM_READ_ONLY;
extern const char DM_UTILS_LOG_DMGR_DELETE_DEVICE_ITSELF[]                      DM_READ_ONLY;
extern const char DM_UTILS_LOG_IPC_MESSAGE_LIST_FULL[]                          DM_READ_ONLY;
extern const char DM_UTILS_LOG_MESSAGE_INIT_FAILED[]                            DM_READ_ONLY;
extern const char DM_UTILS_LOG_MESSAGE_CACHE_INIT_FAILED[]                      DM_READ_ONLY;
extern const char DM_UTILS_LOG_IPC_INIT_FAILED[]                                DM_READ_ONLY;
extern const char DM_UTILS_LOG_DOPT_UNKNOWN_OPT[]                               DM_READ_ONLY;
extern const char DM_UTILS_LOG_OTA_INIT_FAILED[]                                DM_READ_ONLY;
extern const char DM_UTILS_LOG_COTA_INIT_FAILED[]                               DM_READ_ONLY;
extern const char DM_UTILS_LOG_FOTA_INIT_FAILED[]                               DM_READ_ONLY;
extern const char DM_UTILS_LOG_OTA_FETCH_FAILED[]                               DM_READ_ONLY;

#define DM_UTILS_UINT16_STRLEN (5)
#define DM_UTILS_UINT32_STRLEN (10)
#define DM_UTILS_UINT64_STRLEN (20)

int dm_utils_copy_direct(_IN_ void *input, _IN_ int input_len, _OU_ void **output, _IN_ int output_len);

int dm_utils_copy(_IN_ void *input, _IN_ int input_len, _OU_ void **output, _IN_ int output_len);

/**
 * @brief search array index in a string.
 *        This function used to search array index in a string.
 *
 * @param input. The string to be searched
 * @param input_len. The length of input
 * @param partial_input_len. The length of input except [xx]
 * @param array_input_len. The length of input include [xx]
 * @param array_index. The array index in [xx]
 *
 * @warning input must be type of "xxxxx[xx]"
 * @return success or fail.
 *
 */
int dm_utils_strarr_index(_IN_ char *input, _IN_ int input_len,
	_OU_ int *partial_input_len, _OU_ int *array_input_len, _OU_ int *array_index);

int dm_utils_itoa_direct(_IN_ int input,_OU_ char **output);
int dm_utils_itoa(_IN_ int input,_OU_ char **output);
int dm_utils_ftoa_direct(_IN_ double input,_OU_ char **output);
int dm_utils_ftoa(_IN_ double input,_OU_ char **output);
int dm_utils_hex_to_str(_IN_ unsigned char *input, _IN_ int input_len, _OU_ char **output);
int dm_utils_str_to_hex(_IN_ char *input, _IN_ int input_len, _OU_ unsigned char **output, _OU_ int *output_len);
int dm_utils_memtok(_IN_ char *input, _IN_ int input_len, _IN_ char delimiter, _IN_ int index, _OU_ int *offset);
int dm_utils_replace_char(_IN_ char *input, _IN_ int input_len, _IN_ char src, _IN_ char dest);
int dm_utils_service_name(_IN_ const char *prefix, _IN_ const char *name, _IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ char **service_name);
#endif
