#ifndef _IOTX_DM_COMMON_H_
#define _IOTX_DM_COMMON_H_

#ifndef _IN_
#define _IN_
#endif

#ifndef _OU_
#define _OU_
#endif
#include "lite-log.h"

#define DM_SUPPORT_MEMORY_MAGIC
#ifdef DM_SUPPORT_MEMORY_MAGIC
#define DM_malloc(size) LITE_malloc(size, MEM_MAGIC, "DM")
#else
#define DM_malloc(size) LITE_malloc(size)
#endif
#define DM_free(ptr)   {LITE_free(ptr);ptr = NULL;}

#define DM_READ_ONLY

#define dm_log_emerg(...)     log_emerg("DM", __VA_ARGS__)
#define dm_log_crit(...)      log_crit("DM", __VA_ARGS__)
#define dm_log_err(...)       log_err("DM", __VA_ARGS__)
#define dm_log_warning(...)   log_warning("DM", __VA_ARGS__)
#define dm_log_info(...)      log_info("DM", __VA_ARGS__)
#define dm_log_debug(...)     log_debug("DM", __VA_ARGS__)

//Constant Log
extern const char IOTX_DM_LOG_INVALID_PARAMETER[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_MEMORY_NOT_ENOUGH[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_SIGN_METHOD_NOT_FOUND[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_DATA_TYPE_NOT_EXIST[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_DATA_TYPE_INVALID[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_GET_INVALID_DEV_ID[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_OPERATOR_RES[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_DATA_TYPE_HAS_NO_PARSE_FUNC[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_DATA_TYPE_HAS_NO_SET_FUNC[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_JSON_PARSE_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_JSON_PARSE_INDEX_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_TSL_KEY_ID_INVALID[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_TSL_PROPERTY_NOT_EXIST[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_TSL_EVENT_NOT_EXIST[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_TSL_EVENT_DATA_NOT_EXIST[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_TSL_SERIVCE_NOT_EXIST[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_TSL_EVENT_NOT_EXIST[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_TSL_IDENTIFIER_NOT_EXIST[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_TSL_PROPERTY_SET_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_TSL_PROPERTY_GET_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_TSL_PROPERTY_ASSEMBLE_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_TSL_EVENT_SET_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_TSL_EVENT_GET_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_TSL_EVENT_OUTPUT_ASSEMBLE_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_CM_EVENT_UNKNOWN[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_CM_INIT_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_CM_CLOUD_CONNECTIVITY_CREATE_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_CM_CLOUD_CONNECTIVITY_CONNECT_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_CM_LOCAL_CONNECTIVITY_CREATE_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_CM_LOCAL_CONNECTIVITY_CONNECT_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_CM_SEND_MESSAGE_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_CM_SEND_RESULT[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_DMGR_INIT_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_DMGR_SERVICE_CLOUD_REGISTER_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_DMGR_SERVICE_LOCAL_REGISTER_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_DMGR_UNSUPPORT_DEVICE_TYPE[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_DMGR_DEVICE_NOT_FOUND[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_DMGR_DELETE_DEVICE_ITSELF[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_IPC_MESSAGE_LIST_FULL[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_MESSAGE_INIT_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_MESSAGE_CACHE_INIT_FAILED[] DM_READ_ONLY;
extern const char IOTX_DM_LOG_IPC_INIT_FAILED[] DM_READ_ONLY;

#define IOTX_DCM_UINT32_STRLEN (10)
#define IOTX_DCM_UINT64_STRLEN (20)

int iotx_dcm_copy(_IN_ void *input, _IN_ int input_len, _OU_ void **output, _IN_ int output_len);

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
int iotx_dcm_strarr_index(_IN_ char *input, _IN_ int input_len,
	_OU_ int *partial_input_len, _OU_ int *array_input_len, _OU_ int *array_index);

int iotx_dcm_itoa(_IN_ int input,_OU_ char **output);
int iotx_dcm_ftoa(_IN_ double input,_OU_ char **output);
int iotx_dcm_memtok(_IN_ char *input, _IN_ int input_len, _IN_ char delimiter, _IN_ int index, _OU_ int *offset);
int iotx_dcm_replace_char(_IN_ char *input, _IN_ int input_len, _IN_ char src, _IN_ char dest);
int iotx_dcm_service_name(_IN_ const char *prefix, _IN_ const char *name, _IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ char **service_name);
#endif
