#include "lite-utils.h"
#include "iotx_dm_common.h"

//Constant Log
const char IOTX_DM_LOG_INVALID_PARAMETER[] DM_READ_ONLY = "Invalid Parameter";
const char IOTX_DM_LOG_MEMORY_NOT_ENOUGH[] DM_READ_ONLY = "Memory Not Enough";
const char IOTX_DM_LOG_SIGN_METHOD_NOT_FOUND[] DM_READ_ONLY = "Sign Method Not Found";
const char IOTX_DM_LOG_DATA_TYPE_NOT_EXIST[] DM_READ_ONLY = "Data Type Not Exist";
const char IOTX_DM_LOG_GET_INVALID_DEV_ID[] DM_READ_ONLY = "get device id fail";
const char IOTX_DM_LOG_OPERATOR_RES[] DM_READ_ONLY =      "OP %d: Res:%d";
const char IOTX_DM_LOG_DATA_TYPE_INVALID[] DM_READ_ONLY = "Data Type Invalid: %.*s";
const char IOTX_DM_LOG_DATA_TYPE_HAS_NO_PARSE_FUNC[] DM_READ_ONLY = "Data Type Has No Parse Func: %.*s";
const char IOTX_DM_LOG_DATA_TYPE_HAS_NO_SET_FUNC[] DM_READ_ONLY = "Data Type Has No Set Func: %.*s";
const char IOTX_DM_LOG_JSON_PARSE_FAILED[] DM_READ_ONLY = "JSON Parse Failed: %.*s";
const char IOTX_DM_LOG_JSON_PARSE_INDEX_FAILED[] DM_READ_ONLY = "JSON Parse Failed, Index: %d";
const char IOTX_DM_LOG_TSL_KEY_ID_INVALID[] DM_READ_ONLY = "TSL Key ID Invalid: %.*s";
const char IOTX_DM_LOG_TSL_PROPERTY_NOT_EXIST[] DM_READ_ONLY = "TSL Property Not Exist: %.*s";
const char IOTX_DM_LOG_TSL_EVENT_NOT_EXIST[] DM_READ_ONLY = "TSL Event Not Exist: %.*s";
const char IOTX_DM_LOG_TSL_EVENT_DATA_NOT_EXIST[] DM_READ_ONLY = "TSL Event Data Not Exist";
const char IOTX_DM_LOG_TSL_SERIVCE_NOT_EXIST[] DM_READ_ONLY = "TSL Service Not Exist";
const char IOTX_DM_LOG_TSL_IDENTIFIER_NOT_EXIST[] DM_READ_ONLY = "TSL Identifier Not Exist: %.*s";
const char IOTX_DM_LOG_TSL_PROPERTY_SET_FAILED[] DM_READ_ONLY = "TSL Property Set Failed: %.*s";
const char IOTX_DM_LOG_TSL_PROPERTY_GET_FAILED[] DM_READ_ONLY = "TSL Property Get Failed: %.*s";
const char IOTX_DM_LOG_TSL_PROPERTY_ASSEMBLE_FAILED[] DM_READ_ONLY = "TSL Property Assemble Failed: %.*s";
const char IOTX_DM_LOG_TSL_EVENT_SET_FAILED[] DM_READ_ONLY = "TSL Event Set Failed: %.*s";
const char IOTX_DM_LOG_TSL_EVENT_GET_FAILED[] DM_READ_ONLY = "TSL Event Get Failed: %.*s";
const char IOTX_DM_LOG_TSL_EVENT_OUTPUT_ASSEMBLE_FAILED[] DM_READ_ONLY = "TSL Event Output Assemble Failed: %.*s";
const char IOTX_DM_LOG_CM_EVENT_UNKNOWN[] DM_READ_ONLY = "DM CM Event Unknown: %d";
const char IOTX_DM_LOG_CM_INIT_FAILED[] DM_READ_ONLY = "DM CM Init Failed";
const char IOTX_DM_LOG_CM_CLOUD_CONNECTIVITY_CREATE_FAILED[] DM_READ_ONLY = "DM CM Cloud Connectivity Create Failed";
const char IOTX_DM_LOG_CM_CLOUD_CONNECTIVITY_CONNECT_FAILED[] DM_READ_ONLY = "DM CM Cloud Connectivity Connect Failed";
const char IOTX_DM_LOG_CM_LOCAL_CONNECTIVITY_CREATE_FAILED[] DM_READ_ONLY = "DM CM Local Connectivity Create Failed";
const char IOTX_DM_LOG_CM_LOCAL_CONNECTIVITY_CONNECT_FAILED[] DM_READ_ONLY = "DM CM Local Connectivity Connect Failed";
const char IOTX_DM_LOG_CM_SEND_MESSAGE_FAILED[] DM_READ_ONLY = "DM CM Send Message Failed";
const char IOTX_DM_LOG_CM_SEND_RESULT[] DM_READ_ONLY = "DM CM Send Result: %d";
const char IOTX_DM_LOG_DMGR_INIT_FAILED[] DM_READ_ONLY = "DM Manager Module Init Failed";
const char IOTX_DM_LOG_DMGR_SERVICE_CLOUD_REGISTER_FAILED[] DM_READ_ONLY = "Dmgr Service Cloud Register Failed: %.*s";
const char IOTX_DM_LOG_DMGR_SERVICE_LOCAL_REGISTER_FAILED[] DM_READ_ONLY = "Dmgr Service Local Register Failed: %.*s";
const char IOTX_DM_LOG_DMGR_UNSUPPORT_DEVICE_TYPE[] DM_READ_ONLY = "Dmgr Device Type Unknown: %d";
const char IOTX_DM_LOG_DMGR_DEVICE_NOT_FOUND[] DM_READ_ONLY = "DM Device Not Found: %d";
const char IOTX_DM_LOG_DMGR_DELETE_DEVICE_ITSELF[] DM_READ_ONLY = "DM Delete Device Itself";
const char IOTX_DM_LOG_IPC_MESSAGE_LIST_FULL[] DM_READ_ONLY = "DM IPC Message List is Full";
const char IOTX_DM_LOG_MESSAGE_INIT_FAILED[] DM_READ_ONLY = "DM Message Module Init Failed";
const char IOTX_DM_LOG_MESSAGE_CACHE_INIT_FAILED[] DM_READ_ONLY = "DM Message Cache Module Init Failed";
const char IOTX_DM_LOG_IPC_INIT_FAILED[] DM_READ_ONLY = "DM IPC Module Init Failed";

int iotx_dcm_copy(_IN_ void *input, _IN_ int input_len, _OU_ void **output, _IN_ int output_len)
{
	if (input == NULL || output == NULL || *output != NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	*output = DM_malloc(output_len);
	if (*output == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(*output,0,output_len);
	memcpy(*output,input,input_len);

	return SUCCESS_RETURN;
}

int iotx_dcm_strarr_index(_IN_ char *input, _IN_ int input_len,
	_OU_ int *partial_input_len, _OU_ int *array_input_len, _OU_ int *array_index)
{
	if (input == NULL || input_len <= 1 || array_index == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	int index = 0;
	int deep = 0;
	char *bracket_pre = NULL;
	char *bracket_suf = NULL;
	char array_index_str[10] = {0};

	for (index = 0; index < input_len; index++){
		switch (input[index]) {
			case '[':
				{
					if (deep != 0) {return FAIL_RETURN;}
						deep++;
					if (!bracket_pre) {bracket_pre = (char *)&input[index];}
				}
				break;
			case ']':
				{
					if (deep != 1) {return FAIL_RETURN;}
					deep--;
					if (input[index  -1] == '[') {return FAIL_RETURN;}
					if (!bracket_suf) {bracket_suf = (char *)&input[index];}
				}
				break;
			default:
				break;
		}
	}

	if (bracket_pre && bracket_suf && ((bracket_suf - input + 1) == input_len)) {
		if (partial_input_len) {*partial_input_len = bracket_pre - input;}
		if (array_input_len) {*array_input_len = bracket_suf - input + 1;}

		//Get Index
		memcpy(array_index_str,bracket_pre + 1,bracket_suf - bracket_pre - 1);
		*array_index = atoi(array_index_str);
		return SUCCESS_RETURN;
	}

	return FAIL_RETURN;
}

int iotx_dcm_itoa(_IN_ int input,_OU_ char **output)
{
	int res = 0;
	char temp_output[10 + 1] = {0};

	if (output == NULL || *output != NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = HAL_Snprintf(temp_output,10,"%d",input);
	if (res < 0) {return FAIL_RETURN;}

	*output = DM_malloc(strlen(temp_output) + 1);
	if (*output == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(*output,0,strlen(temp_output) + 1);
	memcpy(*output,temp_output,strlen(temp_output));

	return SUCCESS_RETURN;
}

int iotx_dcm_ftoa(_IN_ double input,_OU_ char **output)
{
	int res = 0;
	char temp_output[30 + 1] = {0};

	if (output == NULL || *output != NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = HAL_Snprintf(temp_output,30,"%f",input);
	if (res < 0) {return FAIL_RETURN;}

	*output = DM_malloc(strlen(temp_output) + 1);
	if (*output == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(*output,0,strlen(temp_output) + 1);
	memcpy(*output,temp_output,strlen(temp_output));

	return SUCCESS_RETURN;
}

int iotx_dcm_memtok(_IN_ char *input, _IN_ int input_len, _IN_ char delimiter, _IN_ int index, _OU_ int *offset)
{
	int item_index = 0;
	int count = 0;

	if (input == NULL || input_len <= 0 || offset == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	for (item_index = 0;item_index < input_len;item_index++) {
		if (input[item_index] == delimiter && (item_index + 1) < input_len) {
			count++;
			if (count == index) {*offset = item_index;return SUCCESS_RETURN;}
		}
	}

	return FAIL_RETURN;
}

int iotx_dcm_replace_char(_IN_ char *input, _IN_ int input_len, _IN_ char src, _IN_ char dest)
{
	int index = 0;

	if (input == NULL || input_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	for (index = 0;index < input_len;index++) {
		if (input[index] == src) {input[index] = dest;}
	}

	return SUCCESS_RETURN;
}

int iotx_dcm_service_name(_IN_ const char *prefix, _IN_ const char *name, _IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ char **service_name)
{
	int service_name_len = 0;

	if (name == NULL || product_key == NULL || device_name == NULL ||
		service_name == NULL || *service_name != NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	service_name_len = (prefix == NULL)?(0):(strlen(prefix)) + strlen(name) + strlen(product_key) + strlen(device_name) + 1;
	*service_name = DM_malloc(service_name_len);
	if (*service_name == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(*service_name,0,service_name_len);

	if (prefix != NULL) {HAL_Snprintf(*service_name,service_name_len,prefix,product_key,device_name);}
	memcpy(*service_name + strlen(*service_name),name,strlen(name));

	return SUCCESS_RETURN;
}
