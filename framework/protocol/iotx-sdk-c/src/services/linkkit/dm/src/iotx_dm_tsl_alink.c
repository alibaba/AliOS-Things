#include "iot_import.h"
#include "lite-utils.h"
#include "lite-cjson.h"
#include "iotx_dm_common.h"
#include "iotx_dm_shadow.h"
#include "iotx_dm_tsl_alink.h"

typedef int (*iotx_dsw_data_parse)(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
typedef int (*iotx_dsw_array_parse)(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);

typedef struct {
	iotx_dsw_data_type_e type;
	const char *name;
	iotx_dsw_data_parse  func_parse;
	iotx_dsw_array_parse func_array_parse;
}iotx_dtsl_alink_mapping_t;

//Data Parse
static int _iotx_dsw_int_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _iotx_dsw_float_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _iotx_dsw_double_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _iotx_dsw_text_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _iotx_dsw_enum_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _iotx_dsw_date_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _iotx_dsw_bool_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _iotx_dsw_array_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _iotx_dsw_struct_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _iotx_dsw_property_parse(_IN_ iotx_dsw_data_t *property, _IN_ lite_cjson_t *root);

//Array Data Parse
static int _iotx_dsw_array_int_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _iotx_dsw_array_float_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _iotx_dsw_array_double_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _iotx_dsw_array_text_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _iotx_dsw_array_enum_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _iotx_dsw_array_date_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _iotx_dsw_array_bool_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _iotx_dsw_array_array_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _iotx_dsw_array_struct_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root);

static iotx_dtsl_alink_mapping_t g_iotx_dtsl_alink_mapping[] = {
	{IOTX_DSW_DATA_TYPE_NONE,     "none",     NULL,                      NULL                          },
	{IOTX_DSW_DATA_TYPE_INT,      "int",      _iotx_dsw_int_parse,      _iotx_dsw_array_int_parse      },
	{IOTX_DSW_DATA_TYPE_FLOAT,    "float",    _iotx_dsw_float_parse,    _iotx_dsw_array_float_parse    },
	{IOTX_DSW_DATA_TYPE_DOUBLE,   "double",   _iotx_dsw_double_parse,   _iotx_dsw_array_double_parse   },
	{IOTX_DSW_DATA_TYPE_TEXT,     "text",     _iotx_dsw_text_parse,     _iotx_dsw_array_text_parse     },
	{IOTX_DSW_DATA_TYPE_ENUM,     "enum",     _iotx_dsw_enum_parse,     _iotx_dsw_array_enum_parse     },
	{IOTX_DSW_DATA_TYPE_DATE,     "date",     _iotx_dsw_date_parse,     _iotx_dsw_array_date_parse     },
	{IOTX_DSW_DATA_TYPE_BOOL,     "bool",     _iotx_dsw_bool_parse,     _iotx_dsw_array_bool_parse     },
	{IOTX_DSW_DATA_TYPE_ARRAY,    "array",    _iotx_dsw_array_parse,    _iotx_dsw_array_array_parse    },
	{IOTX_DSW_DATA_TYPE_STRUCT,   "struct",   _iotx_dsw_struct_parse,   _iotx_dsw_array_struct_parse   }
};

static int _iotx_dsw_get_type(_IN_ const char *name, _IN_ int name_len, _OU_ iotx_dsw_data_type_e *type)
{
	if (name == NULL || name_len <=0 || type == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	int index = 0;

	for (index = 0;index <sizeof(g_iotx_dtsl_alink_mapping)/sizeof(iotx_dtsl_alink_mapping_t);index++) {
		if (strlen(g_iotx_dtsl_alink_mapping[index].name) == name_len &&
			memcmp(g_iotx_dtsl_alink_mapping[index].name,name,name_len) == 0) {
			dm_log_debug("Data Type Found: %d:%s",g_iotx_dtsl_alink_mapping[index].type,g_iotx_dtsl_alink_mapping[index].name);
			*type = g_iotx_dtsl_alink_mapping[index].type;
			return SUCCESS_RETURN;
		}
	}

	return FAIL_RETURN;
}

static int _iotx_dsw_schema_parse(_IN_ iotx_dsw_t *shadow, _IN_ lite_cjson_t *root)
{
	return SUCCESS_RETURN;
}

static int _iotx_dsw_link_parse(_IN_ iotx_dsw_t *shadow, _IN_ lite_cjson_t *root)
{
	return SUCCESS_RETURN;
}

static int _iotx_dsw_profile_parse(_IN_ iotx_dsw_t *shadow, _IN_ lite_cjson_t *root)
{
	int res = 0;
	lite_cjson_t lite_profile;
	lite_cjson_t lite_profile_pk;
	lite_cjson_t lite_profile_dn;

	memset(&lite_profile,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_PROFILE,strlen(IOTX_DSW_KEY_PROFILE),&lite_profile);
	if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_profile)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_PROFILE),IOTX_DSW_KEY_PROFILE);
		return FAIL_RETURN;
	}
	dm_log_debug("TSL Profile: %.*s",lite_profile.value_length,lite_profile.value);

	//Parse Product Key (Mandatory)
	memset(&lite_profile_pk,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(&lite_profile, IOTX_DSW_KEY_PROFILE_PK,strlen(IOTX_DSW_KEY_PROFILE_PK),&lite_profile_pk);
	if (res == SUCCESS_RETURN && lite_cjson_is_string(&lite_profile_pk) && lite_profile_pk.value_length < PRODUCT_KEY_MAXLEN) {
		memcpy(shadow->profile.product_key,lite_profile_pk.value,lite_profile_pk.value_length);
		dm_log_debug("TSL Product Key: %.*s",lite_profile_pk.value_length,lite_profile_pk.value);
	}else{
		dm_log_warning(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_PROFILE_PK),IOTX_DSW_KEY_PROFILE_PK);
		return FAIL_RETURN;
	}

	//Parse Device Name (Optional)
	memset(&lite_profile_dn,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(&lite_profile, IOTX_DSW_KEY_PROFILE_DN,strlen(IOTX_DSW_KEY_PROFILE_DN),&lite_profile_dn);
	if (res == SUCCESS_RETURN && lite_cjson_is_string(&lite_profile_dn) && lite_profile_dn.value_length < DEVICE_NAME_MAXLEN) {
		memcpy(shadow->profile.device_name,lite_profile_dn.value,lite_profile_dn.value_length);
		dm_log_debug("TSL Device name: %.*s",lite_profile_dn.value_length,lite_profile_dn.value);
	}else{
		dm_log_warning(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_PROFILE_DN),IOTX_DSW_KEY_PROFILE_DN);
	}

	return SUCCESS_RETURN;
}

static int _iotx_dsw_int_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	return SUCCESS_RETURN;
}

static int _iotx_dsw_float_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	return SUCCESS_RETURN;
}

static int _iotx_dsw_double_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	return SUCCESS_RETURN;
}

static int _iotx_dsw_text_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	return SUCCESS_RETURN;
}

static int _iotx_dsw_enum_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	return SUCCESS_RETURN;
}

static int _iotx_dsw_date_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	return SUCCESS_RETURN;
}

static int _iotx_dsw_bool_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	return SUCCESS_RETURN;
}

static int _iotx_dsw_array_int_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	iotx_dsw_data_value_complex_t *complex_array = (iotx_dsw_data_value_complex_t *)data_value->value;

	complex_array->value = DM_malloc((complex_array->size)*(sizeof(int)));
	if (complex_array->value == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(complex_array->value,0,(complex_array->size)*(sizeof(int)));

	//Just For Test
	#ifdef IOTX_DM_TSL_DEVELOP_TEST
	int index = 0;
	for (index = 0;index < complex_array->size;index++) {
		*((int*)(complex_array->value)+index) = index+1;
	}
	#endif

	return SUCCESS_RETURN;
}

static int _iotx_dsw_array_float_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	iotx_dsw_data_value_complex_t *complex_array = (iotx_dsw_data_value_complex_t *)data_value->value;

	complex_array->value = DM_malloc((complex_array->size)*(sizeof(float)));
	if (complex_array->value == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(complex_array->value,0,(complex_array->size)*(sizeof(float)));

	//Just For Test
	#ifdef IOTX_DM_TSL_DEVELOP_TEST
	int index = 0;
	for (index = 0;index < complex_array->size;index++) {
		*((float*)(complex_array->value)+index) = (float)index+0.2;
	}
	#endif

	return SUCCESS_RETURN;
}

static int _iotx_dsw_array_double_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	iotx_dsw_data_value_complex_t *complex_array = (iotx_dsw_data_value_complex_t *)data_value->value;

	complex_array->value = DM_malloc((complex_array->size)*(sizeof(double)));
	if (complex_array->value == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(complex_array->value,0,(complex_array->size)*(sizeof(double)));

	//Just For Test
	#ifdef IOTX_DM_TSL_DEVELOP_TEST
	int index = 0;
	for (index = 0;index < complex_array->size;index++) {
		*((double*)(complex_array->value)+index) = (double)index+0.2;
	}
	#endif

	return SUCCESS_RETURN;
}

static int _iotx_dsw_array_text_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	iotx_dsw_data_value_complex_t *complex_array = (iotx_dsw_data_value_complex_t *)data_value->value;

	complex_array->value = DM_malloc((complex_array->size)*(sizeof(char *)));
	if (complex_array->value == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(complex_array->value,0,(complex_array->size)*(sizeof(char *)));

	#ifdef IOTX_DM_TSL_DEVELOP_TEST
	int index = 0;
	char temp[10] = {0};
	for (index = 0;index < complex_array->size;index++) {
		memset(temp,0,sizeof(temp));
		HAL_Snprintf(temp,sizeof(temp),"%d",index+1);
		*((char **)(complex_array->value) + index) = DM_malloc(strlen(temp) + 1);
		if (*((char **)(complex_array->value) + index) != NULL) {
			memset(*((char **)(complex_array->value) + index),0,strlen(temp) + 1);
			memcpy(*((char **)(complex_array->value) + index),temp,strlen(temp));
		}
	}
	#endif
	return SUCCESS_RETURN;
}

static int _iotx_dsw_array_enum_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	iotx_dsw_data_value_complex_t *complex_array = (iotx_dsw_data_value_complex_t *)data_value->value;

	complex_array->value = DM_malloc((complex_array->size)*(sizeof(int)));
	if (complex_array->value == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(complex_array->value,0,(complex_array->size)*(sizeof(int)));

	return SUCCESS_RETURN;
}

static int _iotx_dsw_array_date_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	iotx_dsw_data_value_complex_t *complex_array = (iotx_dsw_data_value_complex_t *)data_value->value;

	complex_array->value = DM_malloc((complex_array->size)*(sizeof(char *)));
	if (complex_array->value == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(complex_array->value,0,(complex_array->size)*(sizeof(char *)));

	return SUCCESS_RETURN;
}

static int _iotx_dsw_array_bool_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	iotx_dsw_data_value_complex_t *complex_array = (iotx_dsw_data_value_complex_t *)data_value->value;

	complex_array->value = DM_malloc((complex_array->size)*(sizeof(int)));
	if (complex_array->value == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(complex_array->value,0,(complex_array->size)*(sizeof(int)));

	return SUCCESS_RETURN;
}

static int _iotx_dsw_array_array_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	#if 0
	int res = 0;
	char size_str[IOTX_DCM_UINT32_STRLEN] = {0};
	lite_cjson_t lite_item,lite_type,lite_specs;
	iotx_dsw_data_value_complex_t *complex_array = (iotx_dsw_data_value_complex_t *)data_value->value;
	iotx_dsw_data_value_t *data_value_next_level;
	iotx_dsw_data_value_complex_t *complex_array_next_level = NULL;

	if (!lite_cjson_is_object(root)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	//Allocate Memory For Next Level Data Value And Next Level Complex Array
	data_value_next_level = DM_malloc(sizeof(iotx_dsw_data_value_t));
	if (data_value_next_level == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(data_value_next_level,0,sizeof(iotx_dsw_data_value_t));
	data_value_next_level->type = IOTX_DSW_DATA_TYPE_ARRAY;

	complex_array_next_level = DM_malloc(sizeof(iotx_dsw_data_value_complex_t));
	if (complex_array_next_level == NULL) {
		DM_free(data_value_next_level);
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(complex_array_next_level,0,sizeof(iotx_dsw_data_value_complex_t));
	complex_array->value = (void *)data_value_next_level;
	data_value_next_level->value = complex_array_next_level;

	//Parse Size (Mandatory)
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_SIZE,strlen(IOTX_DSW_KEY_SIZE),&lite_item);
	if (res != SUCCESS_RETURN && !lite_cjson_is_string(&lite_item)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_SIZE),IOTX_DSW_KEY_SIZE);
		return FAIL_RETURN;
	}
	if (lite_item.value_length > IOTX_DCM_UINT32_STRLEN) {return FAIL_RETURN;}
	memcpy(size_str,lite_item.value,lite_item.value_length);
	complex_array_next_level->size = atoi(size_str);

	dm_log_debug("TSL Property Array Array Size: %d",complex_array_next_level->size);

	//Parse Item And Type (Mandatory)
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_ITEM,strlen(IOTX_DSW_KEY_ITEM),&lite_item);
	if (res != SUCCESS_RETURN && !lite_cjson_is_object(&lite_item)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_ITEM),IOTX_DSW_KEY_ITEM);
		return FAIL_RETURN;
	}
	memset(&lite_type,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(&lite_item,IOTX_DSW_KEY_TYPE,strlen(IOTX_DSW_KEY_TYPE),&lite_type);
	if (res != SUCCESS_RETURN && !lite_cjson_is_string(&lite_type)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_TYPE),IOTX_DSW_KEY_TYPE);
		return FAIL_RETURN;
	}
	res = _iotx_dsw_get_type(lite_type.value,lite_type.value_length,&complex_array_next_level->type);
	if (res != SUCCESS_RETURN) {
		dm_log_err(IOTX_DM_LOG_DATA_TYPE_NOT_EXIST);
		return FAIL_RETURN;
	}

	//Parse Specs (Optional)
	memset(&lite_specs,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(&lite_item,IOTX_DSW_KEY_SPECS,strlen(IOTX_DSW_KEY_SPECS),&lite_specs);
	if ((complex_array_next_level->type == IOTX_DSW_DATA_TYPE_ARRAY || complex_array_next_level->type == IOTX_DSW_DATA_TYPE_STRUCT) &&
		(res != SUCCESS_RETURN)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_SPECS),IOTX_DSW_KEY_SPECS);
		return FAIL_RETURN;
	}

	if (g_iotx_dtsl_alink_mapping[complex_array_next_level->type].func_array_parse == NULL) {
		dm_log_err(IOTX_DM_LOG_DATA_TYPE_INVALID,lite_type.value_length,lite_type.value);
		return FAIL_RETURN;
	}
	dm_log_debug("TSL Property Specs Type: %s",g_iotx_dtsl_alink_mapping[complex_array_next_level->type].name);

	//Parse Array Type
	res = g_iotx_dtsl_alink_mapping[complex_array->type].func_array_parse(data_value_next_level,&lite_specs);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	#endif
	return SUCCESS_RETURN;
}

static int _iotx_dsw_array_struct_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	int res = 0, index = 0;
	lite_cjson_t lite_item;
	iotx_dsw_data_value_complex_t *complex_array = (iotx_dsw_data_value_complex_t *)data_value->value;
	iotx_dsw_data_t *data = NULL;

	if (!lite_cjson_is_array(root) || root->size <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	complex_array->size = root->size;
	complex_array->value = DM_malloc((root->size)*(sizeof(iotx_dsw_data_t)));
	if (complex_array->value == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(complex_array->value,0,(root->size)*(sizeof(iotx_dsw_data_t)));

	for(index = 0;index < root->size;index++) {
		data = (iotx_dsw_data_t *)complex_array->value + index;
		memset(&lite_item,0,sizeof(lite_cjson_t));

		res = lite_cjson_array_item(root,index,&lite_item);
		if (res != SUCCESS_RETURN) {continue;}

		res = _iotx_dsw_property_parse(data,&lite_item);
		if (res != SUCCESS_RETURN) {continue;}
	}

	return SUCCESS_RETURN;
}

static int _iotx_dsw_array_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	int res = 0;
	char size_str[IOTX_DCM_UINT32_STRLEN] = {0};
	lite_cjson_t lite_item,lite_type,lite_specs;
	iotx_dsw_data_value_complex_t *complex_array = NULL;

	dm_log_debug("IOTX_DSW_DATA_TYPE_ARRAY");

	if (!lite_cjson_is_object(root)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	//Allocate Memory For Data Type Specs
	complex_array = DM_malloc(sizeof(iotx_dsw_data_value_complex_t));
	if (complex_array == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(complex_array,0,sizeof(iotx_dsw_dtspecs_array_t));
	data_value->value = (void *)complex_array;

	//Parse Size (Mandatory)
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_SIZE,strlen(IOTX_DSW_KEY_SIZE),&lite_item);
	if (res != SUCCESS_RETURN && !lite_cjson_is_string(&lite_item)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_SIZE),IOTX_DSW_KEY_SIZE);
		return FAIL_RETURN;
	}
	if (lite_item.value_length > IOTX_DCM_UINT32_STRLEN) {return FAIL_RETURN;}
	memcpy(size_str,lite_item.value,lite_item.value_length);
	complex_array->size = atoi(size_str);

	dm_log_debug("TSL Property Array Size: %d",complex_array->size);

	//Parse Item And Type (Mandatory)
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_ITEM,strlen(IOTX_DSW_KEY_ITEM),&lite_item);
	if (res != SUCCESS_RETURN && !lite_cjson_is_object(&lite_item)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_ITEM),IOTX_DSW_KEY_ITEM);
		return FAIL_RETURN;
	}
	memset(&lite_type,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(&lite_item,IOTX_DSW_KEY_TYPE,strlen(IOTX_DSW_KEY_TYPE),&lite_type);
	if (res != SUCCESS_RETURN && !lite_cjson_is_string(&lite_type)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_TYPE),IOTX_DSW_KEY_TYPE);
		return FAIL_RETURN;
	}
	res = _iotx_dsw_get_type(lite_type.value,lite_type.value_length,&complex_array->type);
	if (res != SUCCESS_RETURN) {
		dm_log_err(IOTX_DM_LOG_DATA_TYPE_NOT_EXIST);
		return FAIL_RETURN;
	}

	//Parse Specs (Optional)
	memset(&lite_specs,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(&lite_item,IOTX_DSW_KEY_SPECS,strlen(IOTX_DSW_KEY_SPECS),&lite_specs);
	if ((complex_array->type == IOTX_DSW_DATA_TYPE_ARRAY || complex_array->type == IOTX_DSW_DATA_TYPE_STRUCT) &&
		(res != SUCCESS_RETURN)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_SPECS),IOTX_DSW_KEY_SPECS);
		return FAIL_RETURN;
	}

	if (g_iotx_dtsl_alink_mapping[complex_array->type].func_array_parse == NULL) {
		dm_log_err(IOTX_DM_LOG_DATA_TYPE_INVALID,lite_type.value_length,lite_type.value);
		return FAIL_RETURN;
	}
	dm_log_debug("TSL Property Specs Type: %s",g_iotx_dtsl_alink_mapping[complex_array->type].name);

	//Parse Array Type
	res = g_iotx_dtsl_alink_mapping[complex_array->type].func_array_parse(data_value,&lite_specs);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

static int _iotx_dsw_struct_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	int res = 0, index = 0;
	lite_cjson_t lite_item;
	iotx_dsw_data_t *property = NULL;
	iotx_dsw_data_value_complex_t *complex_struct = NULL;

	dm_log_debug("IOTX_DSW_DATA_TYPE_STRUCT");

	if (!lite_cjson_is_array(root) || root->size == 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	dm_log_debug("TSL Property Struct Size: %d",root->size);

	//Allocate Memory For Data Type Specs
	complex_struct = DM_malloc(sizeof(iotx_dsw_data_value_complex_t));
	if (complex_struct == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(complex_struct,0,sizeof(iotx_dsw_dtspecs_array_t));
	data_value->value = (void *)complex_struct;

	complex_struct->size = root->size;

	//Allocate Memory For Multi Identifier
	complex_struct->value = DM_malloc((complex_struct->size)*(sizeof(iotx_dsw_data_t)));
	if (complex_struct->value == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(complex_struct->value,0,(complex_struct->size)*(sizeof(iotx_dsw_data_t)));

	for (index = 0;index < complex_struct->size;index++) {
		memset(&lite_item,0,sizeof(lite_cjson_t));
		property = (iotx_dsw_data_t*)complex_struct->value + index;
		dm_log_debug("TSL Property Struct Index: %d",index);

		res = lite_cjson_array_item(root,index,&lite_item);
		if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_item)) {
			dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,root->value_length,root->value);
			return FAIL_RETURN;
		}
		dm_log_debug("TSL Property Struct Property: %.*s",lite_item.value_length,lite_item.value);
		res = _iotx_dsw_property_parse(property,&lite_item);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	}

	return SUCCESS_RETURN;
}

static int _iotx_dsw_data_parse(_IN_ iotx_dsw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
	int res = 0;
	lite_cjson_t lite_item;

	memset(data_value,0,sizeof(iotx_dsw_data_value_t));

	//Parse Type
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_TYPE,strlen(IOTX_DSW_KEY_TYPE),&lite_item);
	if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_TYPE),IOTX_DSW_KEY_TYPE);
		return FAIL_RETURN;
	}
	dm_log_debug("TSL Data Type: %.*s",lite_item.value_length,lite_item.value);
	res = _iotx_dsw_get_type(lite_item.value,lite_item.value_length,&data_value->type);
	if (res != SUCCESS_RETURN) {
		dm_log_err(IOTX_DM_LOG_DATA_TYPE_NOT_EXIST);
		return FAIL_RETURN;
	}

	//Parse Specs
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_SPECS,strlen(IOTX_DSW_KEY_SPECS),&lite_item);
	if (res != SUCCESS_RETURN) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_SPECS),IOTX_DSW_KEY_SPECS);
		return FAIL_RETURN;
	}
	dm_log_debug("TSL Data Specs: %.*s",lite_item.value_length,lite_item.value);

	//Parse Type And Value
	if (g_iotx_dtsl_alink_mapping[data_value->type].func_parse == NULL) {
		dm_log_err(IOTX_DM_LOG_DATA_TYPE_HAS_NO_PARSE_FUNC,
			strlen(g_iotx_dtsl_alink_mapping[data_value->type].name),g_iotx_dtsl_alink_mapping[data_value->type].name);
		return FAIL_RETURN;
	}
	res = g_iotx_dtsl_alink_mapping[data_value->type].func_parse(data_value,&lite_item);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

static int _iotx_dsw_property_parse(_IN_ iotx_dsw_data_t *property, _IN_ lite_cjson_t *root)
{
	int res = 0;
	lite_cjson_t lite_item;

	//Parse Identifier (Mandatory)
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_IDENTIFIER,strlen(IOTX_DSW_KEY_IDENTIFIER),&lite_item);
	if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_IDENTIFIER),IOTX_DSW_KEY_IDENTIFIER);
		return FAIL_RETURN;
	}
	res = iotx_dcm_copy(lite_item.value,lite_item.value_length,(void **)(&property->identifier),lite_item.value_length + 1);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	dm_log_debug("TSL Property Identifier: %s",property->identifier);

	//Parse DataType
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_DATATYPE,strlen(IOTX_DSW_KEY_DATATYPE),&lite_item);
	if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_item)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_DATATYPE),IOTX_DSW_KEY_DATATYPE);
		return FAIL_RETURN;
	}
	dm_log_debug("TSL Property Data Type: %.*s",lite_item.value_length,lite_item.value);
	res = _iotx_dsw_data_parse(&property->data_value,&lite_item);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

static int _iotx_dsw_properties_parse(_IN_ iotx_dsw_t *shadow, _IN_ lite_cjson_t *root)
{
	int res = 0,index = 0;
	lite_cjson_t lite_properties, lite_property;

	memset(&lite_properties,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_PROPERTIES,strlen(IOTX_DSW_KEY_PROPERTIES),&lite_properties);
	if (res != SUCCESS_RETURN || !lite_cjson_is_array(&lite_properties)) {
		dm_log_warning(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_PROPERTIES),IOTX_DSW_KEY_PROPERTIES);
		return FAIL_RETURN;
	}
	dm_log_debug("TSL Properties: %.*s, size: %d",
		lite_properties.value_length,lite_properties.value,lite_properties.size);

	if (lite_properties.size == 0) {return SUCCESS_RETURN;}

	//Allocate Memory For TSL Properties Struct
	shadow->property_number = lite_properties.size;
	shadow->properties = DM_malloc(sizeof(iotx_dsw_data_t)*(lite_properties.size));
	if (shadow->properties == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(shadow->properties,0,sizeof(iotx_dsw_data_t)*(lite_properties.size));

	for (index = 0;index < lite_properties.size;index++) {
		memset(&lite_property,0,sizeof(lite_cjson_t));
		res = lite_cjson_array_item(&lite_properties,index,&lite_property);
		if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_property)) {return FAIL_RETURN;}

		dm_log_debug("Index: %d, Current Property: %.*s",index,lite_property.value_length,lite_property.value);
		_iotx_dsw_property_parse(shadow->properties+index,&lite_property);
		dm_log_debug("~^_^~~^_^~~^_^~ I'm Delimiter ~^_^~^_^~^_^~\n");
	}

	return SUCCESS_RETURN;
}

static int _iotx_dsw_data_array_search(_IN_ iotx_dsw_data_t *input, _IN_ int input_index, _IN_ char *key,
								_IN_ int key_len, _OU_ iotx_dsw_data_t **output, _OU_ int *output_index)
{
	int res = 0, deli_offset = 0;

	iotx_dsw_data_value_complex_t *complex_struct = (iotx_dsw_data_value_complex_t *)input->data_value.value;
	dm_log_debug("Current Key: %s, Len: %d",key,key_len);
	dm_log_debug("Current Item Identifier: %s",input->identifier);
	res = iotx_dcm_memtok(key,key_len,IOTX_DSW_KEY_DELIMITER,1,&deli_offset);
	if (res != SUCCESS_RETURN) {deli_offset = key_len;}

	switch(complex_struct->type)
	{
		case IOTX_DSW_DATA_TYPE_INT:
		case IOTX_DSW_DATA_TYPE_FLOAT:
		case IOTX_DSW_DATA_TYPE_DOUBLE:
		case IOTX_DSW_DATA_TYPE_TEXT:
		case IOTX_DSW_DATA_TYPE_ENUM:
		case IOTX_DSW_DATA_TYPE_DATE:
		case IOTX_DSW_DATA_TYPE_BOOL:
			{
				if (deli_offset != key_len) {return FAIL_RETURN;}
				if (output) {*output = input;}
				if (output_index) {*output_index = input_index;}
				return SUCCESS_RETURN;
			}
			break;
		default:
			dm_log_err("Unknown Data Type: %d",complex_struct->type);
			break;
	}

	return FAIL_RETURN;
}

static int _iotx_dsw_data_struct_search(_IN_ iotx_dsw_data_t *input,_IN_ char *key,
								_IN_ int key_len, _OU_ iotx_dsw_data_t **output, _OU_ int *index)
{
	int res = 0, item_index = 0, deli_offset = 0, partial_offset = 0;
	int partial_input_len = 0, array_input_len = 0, array_index = 0;
	iotx_dsw_data_t *data_item = NULL;

	dm_log_debug("Current Key: %.*s",key_len,key);

	iotx_dsw_data_value_complex_t *complex_struct = (iotx_dsw_data_value_complex_t *)input->data_value.value;

	res = iotx_dcm_memtok(key,key_len,IOTX_DSW_KEY_DELIMITER,1,&deli_offset);
	if (res != SUCCESS_RETURN) {deli_offset = key_len;}

	partial_offset = deli_offset;
	res = iotx_dcm_strarr_index(key,deli_offset,&partial_input_len,&array_input_len,&array_index);
	if (res == SUCCESS_RETURN) {
		dm_log_debug("Current Index: %d",array_index);
		partial_offset = partial_input_len;
	}

	for (item_index = 0;item_index < complex_struct->size;item_index++) {
		data_item = (iotx_dsw_data_t *)complex_struct->value + item_index;
		if (strlen(data_item->identifier) != partial_offset ||
			memcmp(data_item->identifier,key,partial_offset) != 0) {
			continue;
		}

		switch (data_item->data_value.type) {
			case IOTX_DSW_DATA_TYPE_INT:
			case IOTX_DSW_DATA_TYPE_FLOAT:
			case IOTX_DSW_DATA_TYPE_DOUBLE:
			case IOTX_DSW_DATA_TYPE_TEXT:
			case IOTX_DSW_DATA_TYPE_ENUM:
			case IOTX_DSW_DATA_TYPE_DATE:
			case IOTX_DSW_DATA_TYPE_BOOL:
				{
					if (partial_input_len != 0 || deli_offset != key_len) {return FAIL_RETURN;}
					if (output) {*output = data_item;}
					return SUCCESS_RETURN;
				}
				break;
			case IOTX_DSW_DATA_TYPE_ARRAY:
				{
					int key_offset = (deli_offset == key_len)?(deli_offset - 1):(deli_offset+1);
					int key_len_offset = (deli_offset == key_len)?(key_len):(deli_offset+1);
					if ((partial_input_len == 0) && (deli_offset == key_len)) {if (output) {*output = data_item;} return SUCCESS_RETURN;}
					if (partial_input_len == 0) {return FAIL_RETURN;}
					return _iotx_dsw_data_array_search(data_item,array_index,key+key_offset,key_len-key_len_offset,output,index);
				}
			case IOTX_DSW_DATA_TYPE_STRUCT:
				{
					if (deli_offset == key_len) {if (output) {*output = data_item;} return SUCCESS_RETURN;}
					if (partial_input_len != 0) {return FAIL_RETURN;}
					return _iotx_dsw_data_struct_search(data_item,key+deli_offset+1,key_len-deli_offset-1,output,index);
				}
		default:
			dm_log_err("Unknown Data Type");
			break;
		}
	}

	return FAIL_RETURN;
}

static int _iotx_dsw_data_search(_IN_ iotx_dsw_data_t *input,_IN_ char *key,
								_IN_ int key_len, _OU_ iotx_dsw_data_t **output, _OU_ int *index)
{
	int res = 0, deli_offset = 0, partial_offset = 0;
	int partial_input_len = 0, array_input_len = 0, array_index = 0;

	if (input == NULL || key == NULL || key_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = iotx_dcm_memtok(key,key_len,IOTX_DSW_KEY_DELIMITER,1,&deli_offset);
	if (res != SUCCESS_RETURN) {deli_offset = key_len;}

	partial_offset = deli_offset;
	res = iotx_dcm_strarr_index(key,deli_offset,&partial_input_len,&array_input_len,&array_index);
	if (res == SUCCESS_RETURN) {
		dm_log_debug("Current Index: %d",array_index);
		partial_offset = partial_input_len;
	}

	dm_log_debug("Current Input Identifier: %s",input->identifier);
	dm_log_debug("Current Compare Key: %.*s",partial_offset,key);

	if (strlen(input->identifier) != partial_offset ||
		memcmp(input->identifier,key,partial_offset) != 0) {
		return FAIL_RETURN;
	}

	switch (input->data_value.type) {
		case IOTX_DSW_DATA_TYPE_INT:
		case IOTX_DSW_DATA_TYPE_FLOAT:
		case IOTX_DSW_DATA_TYPE_DOUBLE:
		case IOTX_DSW_DATA_TYPE_TEXT:
		case IOTX_DSW_DATA_TYPE_ENUM:
		case IOTX_DSW_DATA_TYPE_DATE:
		case IOTX_DSW_DATA_TYPE_BOOL:
			{
				if (partial_input_len != 0 || deli_offset != key_len) {return FAIL_RETURN;}
				if (output) {*output = input;}
				return SUCCESS_RETURN;
			}
			break;
		case IOTX_DSW_DATA_TYPE_ARRAY:
			{
				int key_offset = (deli_offset == key_len)?(deli_offset - 1):(deli_offset+1);
				int key_len_offset = (deli_offset == key_len)?(key_len):(deli_offset+1);
				if ((partial_input_len == 0) && (deli_offset == key_len)) {if (output) {*output = input;} return SUCCESS_RETURN;}
				if (partial_input_len == 0) {return FAIL_RETURN;}
				return _iotx_dsw_data_array_search(input,array_index,key+key_offset,key_len-key_len_offset,output,index);
			}
			break;
		case IOTX_DSW_DATA_TYPE_STRUCT:
			{
				if (deli_offset == key_len) {if (output) {*output = input;} return SUCCESS_RETURN;}
				if (partial_input_len != 0) {return FAIL_RETURN;}
				return _iotx_dsw_data_struct_search(input,key+deli_offset+1,key_len-deli_offset-1,output,index);
			}
			break;
		default:
			dm_log_warning("Unknow Data Type");
			break;
	}

	return FAIL_RETURN;
}

static int _iotx_dsw_property_search(_IN_ iotx_dsw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ iotx_dsw_data_t **property, _OU_ int *index)
{
	int res = 0, item_index = 0;
	iotx_dsw_data_t *property_item = NULL;

	if (shadow == NULL || key == NULL || key_len <=0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	if (shadow->property_number == 0 || shadow->properties == NULL) {
		dm_log_err(IOTX_DM_LOG_TSL_PROPERTY_NOT_EXIST,key_len,key);
		return FAIL_RETURN;
	}

	for (item_index = 0;item_index < shadow->property_number;item_index++) {
		property_item = shadow->properties + item_index;
		res = _iotx_dsw_data_search(property_item,key,key_len,property,index);
		if (res == SUCCESS_RETURN) {return SUCCESS_RETURN;}
	}

	return FAIL_RETURN;
}

static int _iotx_dsw_event_outputdata_parse(_IN_ iotx_dsw_t *shadow, _IN_ iotx_dsw_data_t *event_data, _IN_ lite_cjson_t *root)
{
	int res = 0;
	lite_cjson_t lite_item;

	//Parse Identifier (Madantory)
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_IDENTIFIER,strlen(IOTX_DSW_KEY_IDENTIFIER),&lite_item);
	if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_IDENTIFIER),IOTX_DSW_KEY_IDENTIFIER);
		return FAIL_RETURN;
	}
	res = iotx_dcm_copy(lite_item.value,lite_item.value_length,(void **)&(event_data->identifier),lite_item.value_length + 1);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	dm_log_debug("TSL Ouput Event Identifier: %s",event_data->identifier);

	//Search If There Is Same Identifier In Properties
	res = _iotx_dsw_property_search(shadow,event_data->identifier,strlen(event_data->identifier),NULL,NULL);
	if (res == SUCCESS_RETURN) {return SUCCESS_RETURN;}

	//This Identifier Not Exist In Properties, Parse It
	//Parse DataType
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_DATATYPE,strlen(IOTX_DSW_KEY_DATATYPE),&lite_item);
	if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_item)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_DATATYPE),IOTX_DSW_KEY_DATATYPE);
		return FAIL_RETURN;
	}
	dm_log_debug("TSL Output Event Data Type: %.*s",lite_item.value_length,lite_item.value);
	res = _iotx_dsw_data_parse(&event_data->data_value,&lite_item);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

static int _iotx_dsw_event_outputdatas_parse(_IN_ iotx_dsw_t *shadow, _IN_ iotx_dsw_event_t *event, _IN_ lite_cjson_t *root)
{
	int res = 0, index = 0;
	lite_cjson_t lite_item;
	iotx_dsw_data_t *output_data = NULL;

	dm_log_debug("TSL Event Output Data Number: %d",event->output_data_number);
	if (event->output_data_number == 0) {return SUCCESS_RETURN;}

	//Allocate Memory For Output Datas
	event->output_datas = DM_malloc((event->output_data_number)*(sizeof(iotx_dsw_data_t)));
	if (event->output_datas == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(event->output_datas,0,(event->output_data_number)*(sizeof(iotx_dsw_data_t)));

	for (index = 0;index < event->output_data_number;index++) {
		memset(&lite_item,0,sizeof(lite_cjson_t));
		output_data = event->output_datas+index;

		res = lite_cjson_array_item(root,index,&lite_item);
		if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_item)) {return FAIL_RETURN;}

		dm_log_debug("Index: %d, Current Event: %.*s",index,lite_item.value_length,lite_item.value);
		_iotx_dsw_event_outputdata_parse(shadow,output_data,&lite_item);
		dm_log_debug("~^_^~~^_^~~^_^~ I'm Delimiter ~^_^~^_^~^_^~\n");
	}

	return SUCCESS_RETURN;
}

static int _iotx_dsw_event_parse(_IN_ iotx_dsw_t *shadow, _IN_ iotx_dsw_event_t *event, _IN_ lite_cjson_t *root)
{
	int res = 0;
	lite_cjson_t lite_item;

	//Parse Identifier (Mandatory)
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_IDENTIFIER,strlen(IOTX_DSW_KEY_IDENTIFIER),&lite_item);
	if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_IDENTIFIER),IOTX_DSW_KEY_IDENTIFIER);
		return FAIL_RETURN;
	}
	res = iotx_dcm_copy(lite_item.value,lite_item.value_length,(void **)(&event->identifier),lite_item.value_length + 1);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	dm_log_debug("TSL Event Identifier: %s",event->identifier);

	//Parse Output Data (Madantory)
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_OUTPUTDATA,strlen(IOTX_DSW_KEY_OUTPUTDATA),&lite_item);
	if (res != SUCCESS_RETURN || !lite_cjson_is_array(&lite_item)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_OUTPUTDATA),IOTX_DSW_KEY_OUTPUTDATA);
	}
	event->output_data_number = lite_item.size;
	res = _iotx_dsw_event_outputdatas_parse(shadow,event,&lite_item);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

static int _iotx_dsw_events_parse(_IN_ iotx_dsw_t *shadow, _IN_ lite_cjson_t *root)
{
	int res = 0, index = 0;
	lite_cjson_t lite_events,lite_event;

	memset(&lite_events,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_EVENTS,strlen(IOTX_DSW_KEY_EVENTS),&lite_events);
	if (res != SUCCESS_RETURN || !lite_cjson_is_array(&lite_events)) {
		dm_log_warning(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_EVENTS),IOTX_DSW_KEY_EVENTS);
		return FAIL_RETURN;
	}
	dm_log_debug("TSL Events: %.*s, size: %d",
		lite_events.value_length,lite_events.value,lite_events.size);
	dm_log_debug("TSL Events: size: %d",lite_events.size);

	if (lite_events.size == 0) {return SUCCESS_RETURN;}

	//Allocate Memory For TSL Events Struct
	shadow->event_number = lite_events.size;
	shadow->events = DM_malloc(sizeof(iotx_dsw_event_t)*(lite_events.size));
	if (shadow->events == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(shadow->events,0,sizeof(iotx_dsw_event_t)*(lite_events.size));

	for (index = 0;index < lite_events.size;index++) {
		memset(&lite_event,0,sizeof(lite_cjson_t));
		res = lite_cjson_array_item(&lite_events,index,&lite_event);
		if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_event)) {return FAIL_RETURN;}

		dm_log_debug("Index: %d, Current Event: %.*s",index,lite_event.value_length,lite_event.value);
		_iotx_dsw_event_parse(shadow,shadow->events+index,&lite_event);
		dm_log_debug("~^_^~~^_^~~^_^~ I'm Delimiter ~^_^~^_^~^_^~\n");
	}
	return SUCCESS_RETURN;
}

static int _iotx_dsw_service_outputdata_parse(_IN_ iotx_dsw_t *shadow, _IN_ iotx_dsw_data_t *service_data, _IN_ lite_cjson_t *root)
{
	int res = 0;
	lite_cjson_t lite_item;

	//Parse Identifier (Madantory)
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_IDENTIFIER,strlen(IOTX_DSW_KEY_IDENTIFIER),&lite_item);
	if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_IDENTIFIER),IOTX_DSW_KEY_IDENTIFIER);
		return FAIL_RETURN;
	}
	res = iotx_dcm_copy(lite_item.value,lite_item.value_length,(void **)&(service_data->identifier),lite_item.value_length + 1);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	dm_log_debug("TSL Ouput Service Identifier: %s",service_data->identifier);

	//Search If There Is Same Identifier In Properties
	res = _iotx_dsw_property_search(shadow,service_data->identifier,strlen(service_data->identifier),NULL,NULL);
	if (res == SUCCESS_RETURN) {return SUCCESS_RETURN;}

	//This Identifier Not Exist In Properties, Parse It
	//Parse DataType
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_DATATYPE,strlen(IOTX_DSW_KEY_DATATYPE),&lite_item);
	if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_item)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_DATATYPE),IOTX_DSW_KEY_DATATYPE);
		return FAIL_RETURN;
	}
	dm_log_debug("TSL Output Service Data Type: %.*s",lite_item.value_length,lite_item.value);
	res = _iotx_dsw_data_parse(&service_data->data_value,&lite_item);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

static int _iotx_dsw_service_outputdatas_parse(_IN_ iotx_dsw_t *shadow, _IN_ iotx_dsw_service_t *service, _IN_ lite_cjson_t *root)
{
	int res = 0, index = 0;
	lite_cjson_t lite_item;
	iotx_dsw_data_t *output_data = NULL;

	dm_log_debug("TSL Service Output Data Number: %d",service->output_data_number);
	if (service->output_data_number == 0) {return SUCCESS_RETURN;}

	//Allocate Memory For Output Datas
	service->output_datas = DM_malloc((service->output_data_number)*(sizeof(iotx_dsw_data_t)));
	if (service->output_datas == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(service->output_datas,0,(service->output_data_number)*(sizeof(iotx_dsw_data_t)));

	for (index = 0;index < service->output_data_number;index++) {
		memset(&lite_item,0,sizeof(lite_cjson_t));
		output_data = service->output_datas+index;

		res = lite_cjson_array_item(root,index,&lite_item);
		if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_item)) {return FAIL_RETURN;}

		dm_log_debug("Index: %d, Current Service Output Data: %.*s",index,lite_item.value_length,lite_item.value);
		_iotx_dsw_service_outputdata_parse(shadow,output_data,&lite_item);
		dm_log_debug("~^_^~~^_^~~^_^~ I'm Delimiter ~^_^~^_^~^_^~\n");
	}

	return SUCCESS_RETURN;
}

static int _iotx_dsw_service_inputdata_get_parse(_IN_ iotx_dsw_t *shadow, _IN_ iotx_dsw_data_t *input_data, _IN_ lite_cjson_t *root)
{
	int res = 0;

	if (!lite_cjson_is_string(root)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = iotx_dcm_copy(root->value,root->value_length,(void **)&(input_data->identifier),root->value_length + 1);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	dm_log_debug("TSL Service InputData Identifier: %s",input_data->identifier);

	return SUCCESS_RETURN;
}

static int _iotx_dsw_service_inputdata_parse(_IN_ iotx_dsw_t *shadow, _IN_ iotx_dsw_data_t *input_data, _IN_ lite_cjson_t *root)
{
	int res = 0;
	lite_cjson_t lite_item;

	if (!lite_cjson_is_object(root)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	//Parse Identifier (Madantory)
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_IDENTIFIER,strlen(IOTX_DSW_KEY_IDENTIFIER),&lite_item);
	if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_IDENTIFIER),IOTX_DSW_KEY_IDENTIFIER);
		return FAIL_RETURN;
	}
	res = iotx_dcm_copy(lite_item.value,lite_item.value_length,(void **)&(input_data->identifier),lite_item.value_length + 1);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	dm_log_debug("TSL Input Service Identifier: %s",input_data->identifier);

	//Search If There Is Same Identifier In Properties
	res = _iotx_dsw_property_search(shadow,input_data->identifier,strlen(input_data->identifier),NULL,NULL);
	if (res == SUCCESS_RETURN) {return SUCCESS_RETURN;}

	//This Identifier Not Exist In Properties, Parse It
	//Parse DataType
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_DATATYPE,strlen(IOTX_DSW_KEY_DATATYPE),&lite_item);
	if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_item)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_DATATYPE),IOTX_DSW_KEY_DATATYPE);
		return FAIL_RETURN;
	}
	dm_log_debug("TSL Input Service Data Type: %.*s",lite_item.value_length,lite_item.value);
	res = _iotx_dsw_data_parse(&input_data->data_value,&lite_item);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

static int _iotx_dsw_service_inputdatas_parse(_IN_ iotx_dsw_t *shadow, _IN_ iotx_dsw_service_t *service, _IN_ lite_cjson_t *root)
{
	int res = 0, index = 0;
	lite_cjson_t lite_item;
	iotx_dsw_data_t *input_data = NULL;

	dm_log_debug("TSL Service Input Data Number: %d",service->input_data_number);
	if (service->input_data_number == 0) {return SUCCESS_RETURN;}

	//Allocate Memory For Output Datas
	service->input_datas = DM_malloc((service->input_data_number)*(sizeof(iotx_dsw_data_t)));
	if (service->input_datas == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(service->input_datas,0,(service->input_data_number)*(sizeof(iotx_dsw_data_t)));

	for (index = 0;index < service->input_data_number;index++) {
		memset(&lite_item,0,sizeof(lite_cjson_t));
		input_data = service->input_datas+index;

		res = lite_cjson_array_item(root,index,&lite_item);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

		dm_log_debug("Index: %d, Current Service Input Data: %.*s",index,lite_item.value_length,lite_item.value);

		//There Is A God-Damned Special Case For thing.service.property.get(method)/get(identifier)
		if (strcmp(service->identifier,IOTX_DSW_SPECIAL_SERVICE_GET_IDENTIFIER) == 0) {
			_iotx_dsw_service_inputdata_get_parse(shadow,input_data,&lite_item);
		}else{
			_iotx_dsw_service_inputdata_parse(shadow,input_data,&lite_item);
		}
		dm_log_debug("~^_^~~^_^~~^_^~ I'm Delimiter ~^_^~^_^~^_^~\n");
	}

	return SUCCESS_RETURN;
}

static int _iotx_dsw_service_parse(_IN_ iotx_dsw_t *shadow, _IN_ iotx_dsw_service_t *service, _IN_ lite_cjson_t *root)
{
	int res = 0;
	lite_cjson_t lite_item;

	//Parse Identifier (Mandatory)
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_IDENTIFIER,strlen(IOTX_DSW_KEY_IDENTIFIER),&lite_item);
	if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_IDENTIFIER),IOTX_DSW_KEY_IDENTIFIER);
		return FAIL_RETURN;
	}
	res = iotx_dcm_copy(lite_item.value,lite_item.value_length,(void **)(&service->identifier),lite_item.value_length + 1);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	dm_log_debug("TSL Service Identifier: %s",service->identifier);

	//Parse Output Data (Optional)
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_OUTPUTDATA,strlen(IOTX_DSW_KEY_OUTPUTDATA),&lite_item);
	if (res == SUCCESS_RETURN && lite_cjson_is_array(&lite_item)) {
		service->output_data_number = lite_item.size;
		res = _iotx_dsw_service_outputdatas_parse(shadow,service,&lite_item);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	}else{
		dm_log_debug("TSL Service Output Data Not Exist");
	}

	//Parse Input Data (Optional)
	memset(&lite_item,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_INPUTDATA,strlen(IOTX_DSW_KEY_INPUTDATA),&lite_item);
	if (res == SUCCESS_RETURN && lite_cjson_is_array(&lite_item)) {
		service->input_data_number = lite_item.size;
		res = _iotx_dsw_service_inputdatas_parse(shadow,service,&lite_item);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	}else{
		dm_log_debug("TSL Service Input Data Not Exist");
	}

	return SUCCESS_RETURN;
}

static int _iotx_dsw_services_parse(_IN_ iotx_dsw_t *shadow, _IN_ lite_cjson_t *root)
{
	int res = 0, index = 0;
	lite_cjson_t lite_services, lite_service;
	iotx_dsw_service_t *service = NULL;

	memset(&lite_services,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(root,IOTX_DSW_KEY_SERVICES,strlen(IOTX_DSW_KEY_SERVICES),&lite_services);
	if (res != SUCCESS_RETURN || !lite_cjson_is_array(&lite_services)) {
		dm_log_warning(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DSW_KEY_SERVICES),IOTX_DSW_KEY_SERVICES);
		return FAIL_RETURN;
	}
	dm_log_debug("TSL Services: %.*s",
		lite_services.value_length,lite_services.value);
	dm_log_debug("TSL Services: size: %d",lite_services.size);

	if (lite_services.size == 0) {return SUCCESS_RETURN;}

	//Allocate Memory For TSL Services Struct
	shadow->service_number = lite_services.size;
	shadow->services = DM_malloc(sizeof(iotx_dsw_service_t)*(lite_services.size));
	if (shadow->services == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(shadow->services,0,sizeof(iotx_dsw_service_t)*(lite_services.size));

	for (index = 0;index <lite_services.size;index++) {
		memset(&lite_service,0,sizeof(lite_cjson_t));
		service = shadow->services+index;

		res = lite_cjson_array_item(&lite_services,index,&lite_service);
		if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_service)) {return FAIL_RETURN;}

		dm_log_debug("Index: %d, Current Service: %.*s",index,lite_service.value_length,lite_service.value);
		_iotx_dsw_service_parse(shadow,service,&lite_service);
		dm_log_debug("Service Parse======I'm Delimiter======Service Parse\n");
	}

	return SUCCESS_RETURN;
}

int iotx_dtsl_alink_create(_IN_ const char *tsl, _IN_ int tsl_len, _OU_ iotx_dsw_t **shadow)
{
	int res = 0;
	lite_cjson_t lite_root;

	if (shadow == NULL || *shadow != NULL || tsl == NULL || tsl_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	*shadow = DM_malloc(sizeof(iotx_dsw_t));
	if (*shadow == NULL) {
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(*shadow,0,sizeof(iotx_dsw_t));

	//Parse Root
	memset(&lite_root,0,sizeof(lite_root));
	res = lite_cjson_parse(tsl,tsl_len,&lite_root);
	if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_root)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,tsl_len,tsl);
		DM_free(*shadow);
		return FAIL_RETURN;
	}

	//Parse Schema (Optional)
	_iotx_dsw_schema_parse(*shadow,&lite_root);

	//Parse Link (Optional)
	_iotx_dsw_link_parse(*shadow,&lite_root);

	//Parse Profile (Optional)
	_iotx_dsw_profile_parse(*shadow,&lite_root);

	//Parse Properties (Mandatory)
	res = _iotx_dsw_properties_parse(*shadow,&lite_root);
	if (res != SUCCESS_RETURN) {DM_free(*shadow);return FAIL_RETURN;}

	//Parse Events (Mandatory)
	res = _iotx_dsw_events_parse(*shadow,&lite_root);
	if (res != SUCCESS_RETURN) {DM_free(*shadow);return FAIL_RETURN;}

	//Parse Services (Mandatory)
	res = _iotx_dsw_services_parse(*shadow,&lite_root);
	if (res != SUCCESS_RETURN) {DM_free(*shadow);return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

