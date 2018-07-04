#ifndef _IOTX_DM_SHADOW_H_
#define _IOTX_DM_SHADOW_H_

#include "iot_import.h"
#include "lite-cjson.h"
#include "iot_export_dm.h"

#define IOTX_DSW_KEY_SCHEMA                     "schema"
#define IOTX_DSW_KEY_LINK                       "link"
#define IOTX_DSW_KEY_PROFILE                    "profile"
#define IOTX_DSW_KEY_PROPERTIES                 "properties"
#define IOTX_DSW_KEY_EVENTS                     "events"
#define IOTX_DSW_KEY_SERVICES                   "services"

#define IOTX_DSW_KEY_PROFILE_PK                 "productKey"
#define IOTX_DSW_KEY_PROFILE_DN                 "deviceName"
#define IOTX_DSW_KEY_IDENTIFIER                 "identifier"
#define IOTX_DSW_KEY_NAME                       "name"
#define IOTX_DSW_KEY_DESC                       "desc"
#define IOTX_DSW_KEY_ACCESS_MODE                "accessMode"
#define IOTX_DSW_KEY_REQUIRED                   "required"
#define IOTX_DSW_KEY_METHOD                     "method"
#define IOTX_DSW_KEY_CALLTYPE                   "callType"
#define IOTX_DSW_KEY_OUTPUTDATA                 "outputData"
#define IOTX_DSW_KEY_INPUTDATA                  "inputData"
#define IOTX_DSW_KEY_DATATYPE                   "dataType"
#define IOTX_DSW_KEY_TYPE                       "type"
#define IOTX_DSW_KEY_SPECS                      "specs"
#define IOTX_DSW_KEY_UNIT                       "unit"
#define IOTX_DSW_KEY_UNITNAME                   "unitName"
#define IOTX_DSW_KEY_MIN                        "min"
#define IOTX_DSW_KEY_MAX                        "max"
#define IOTX_DSW_KEY_LENGTH                     "length"
#define IOTX_DSW_KEY_SIZE                       "size"
#define IOTX_DSW_KEY_ITEM                       "item"

//Special Service And Event
#define IOTX_DSW_SPECIAL_SERVICE_SET_IDENTIFIER "set"
#define IOTX_DSW_SPECIAL_SERVICE_SET_METHOD     "thing.service.property.set"
#define IOTX_DSW_SPECIAL_SERVICE_GET_IDENTIFIER "get"
#define IOTX_DSW_SPECIAL_SERVICE_GET_METHOD     "thing.service.property.get"
#define IOTX_DSW_SPECIAL_EVENT_POST_IDENTIFIER  "post"
#define IOTX_DSW_SPECIAL_EVENT_POST_METHOD      "thing.event.property.post"

#define IOTX_DSW_KEY_DELIMITER                  '.'

typedef struct {
	char product_key[PRODUCT_KEY_MAXLEN];
	char device_name[DEVICE_NAME_MAXLEN];
}iotx_dsw_profile_t;

typedef enum {
	IOTX_DSW_DATA_TYPE_NONE,                    //none
	IOTX_DSW_DATA_TYPE_INT,                     //int
	IOTX_DSW_DATA_TYPE_FLOAT,                   //float
	IOTX_DSW_DATA_TYPE_DOUBLE,                  //double
	IOTX_DSW_DATA_TYPE_TEXT,                    //string
	IOTX_DSW_DATA_TYPE_ENUM,                    //int
	IOTX_DSW_DATA_TYPE_DATE,                    //string
	IOTX_DSW_DATA_TYPE_BOOL,                    //bool,0 or 1
	IOTX_DSW_DATA_TYPE_ARRAY,                   //support int, float, double, text
	IOTX_DSW_DATA_TYPE_STRUCT,                  //support above 8 data types
}iotx_dsw_data_type_e;

typedef struct {
	iotx_dsw_data_type_e type;
	int size_int;
	char *size;
}iotx_dsw_dtspecs_array_t;

typedef struct {
	iotx_dsw_data_type_e type;
	int size;
	void *value;
}iotx_dsw_data_value_complex_t;

typedef struct {
	iotx_dsw_data_type_e type;
	union {
		int value_int;
		float value_float;
		double value_double;
		void *value;                             //string or complex type accroding to data type
	};
}iotx_dsw_data_value_t;

typedef struct {
	iotx_dsw_data_type_e type;
	int specs_number;                            //used when type is enum and struct
	void *specs;                                 //nerver be used by struct
}iotx_dsw_data_type_t;

typedef struct {
	char *identifier;
	iotx_dsw_data_value_t data_value;
}iotx_dsw_data_t;

typedef struct {
	char *identifier;
	int input_data_number;                       //input_data Number
	iotx_dsw_data_t *input_datas;               //input_data array, type is iotx_dsw_data_t
	int output_data_number;                      //ouput_data Number
	iotx_dsw_data_t *output_datas;              //output_data array, type is iotx_dsw_data_t
}iotx_dsw_event_t;

typedef struct {
	char *identifier;                            //synchronized or asynchronized
	int input_data_number;                       //input_data_number
	iotx_dsw_data_t *input_datas;               //input_data array, type is iotx_dsw_data_t
	int output_data_number;                      //ouput_data Number
	iotx_dsw_data_t *output_datas;              //output_data array, type is iotx_dsw_data_t
}iotx_dsw_service_t;

typedef struct {
	iotx_dsw_profile_t profile;
	int property_number;
	iotx_dsw_data_t *properties;                //property array, type is iotx_dsw_data_t
	int event_number;
	iotx_dsw_event_t *events;                   //event array, type is iotx_dsw_event_t
	int service_number;
	iotx_dsw_service_t *services;               //service array, type is iotx_dsw_service_t
}iotx_dsw_t;

/**
 * @brief Create TSL struct from TSL string.
 *        This function used to parse TSL string into TSL struct.
 *
 * @param tsl. The TSL string in JSON format.
 * @param tsl_len. The length of tsl
 * @param shadow. The pointer of TSL Struct pointer, will be malloc memory.
 *                This memory should be free by iotx_dsw_destroy.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_create(_IN_ iotx_dm_tsl_type_t type, _IN_ const char *tsl, _IN_ int tsl_len, _OU_ iotx_dsw_t **shadow);

/**
 * @brief Get product key from TSL struct.
 *        This function used to get product key from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param product_key. The product key in TSL Struct.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_product_key(_IN_ iotx_dsw_t *shadow, _OU_ char product_key[PRODUCT_KEY_MAXLEN]);

/**
 * @brief Get device name from TSL struct.
 *        This function used to get device name from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param device name. The device name in TSL Struct.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_device_name(_IN_ iotx_dsw_t *shadow, _OU_ char device_name[DEVICE_NAME_MAXLEN]);

/**
 * @brief Get property from TSL struct.
 *        This function used to get property from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: property_id
 *		  array type: property_id(array)[index]
 *        struct type: property_id(struct).property_id or property_id(struct).property_id[index]
 *
 * @param key_len. The length of key.
 * @param property. The property in TSL Struct.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_property_data(_IN_ iotx_dsw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ void **data);

/**
 * @brief Get service data from TSL struct.
 *        This function used to get property from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: property_id
 *		  array type: property_id(array)[index]
 *        struct type: property_id(struct).property_id or property_id(struct).property_id[index]
 *
 * @param key_len. The length of key.
 * @param property. The property in TSL Struct.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_service_input_data(_IN_ iotx_dsw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ void **data);

/**
 * @brief Get service output data from TSL struct.
 *        This function used to get service output data from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: property_id
 *		  array type: property_id(array)[index]
 *        struct type: property_id(struct).property_id or property_id(struct).property_id[index]
 *
 * @param key_len. The length of key.
 * @param property. The property in TSL Struct.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_service_output_data(_IN_ iotx_dsw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ void **data);

/**
 * @brief Get event output data from TSL struct.
 *        This function used to get event output data from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: property_id
 *		  array type: property_id(array)[index]
 *        struct type: property_id(struct).property_id or property_id(struct).property_id[index]
 *
 * @param key_len. The length of key.
 * @param property. The property in TSL Struct.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_event_output_data(_IN_ iotx_dsw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ void **data);

/**
 * @brief Get property type from TSL struct.
 *        This function used to get property type from TSL struct.
 *
 * @param property. The handle of property.
 * @param type. The data type of property
 *
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_data_type(_IN_ void *data, _OU_ iotx_dsw_data_type_e *type);

/**
 * @brief Get event from TSL struct.
 *        This function used to get property from TSL struct.
 *
 * @param service. The handle of event.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: event_id
 *
 * @param key_len. The length of key.
 * @param property. The event in TSL Struct.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_event(_IN_ iotx_dsw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ void **event);

/**
 * @brief Get service from TSL struct.
 *        This function used to get property from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: service_id
 *
 * @param key_len. The length of key.
 * @param property. The service in TSL Struct.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_service(_IN_ iotx_dsw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ void **service);

/**
 * @brief Get property number from TSL struct.
 *        This function used to get property number from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param number. The property number in TSL Struct.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_property_number(_IN_ iotx_dsw_t * shadow,_OU_ int *number);

/**
 * @brief Get service number from TSL struct.
 *        This function used to get property number from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param number. The service number in TSL Struct.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_service_number(_IN_ iotx_dsw_t * shadow,_OU_ int *number);

/**
 * @brief Get event number from TSL struct.
 *        This function used to get property number from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param number. The event number in TSL Struct.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_event_number(_IN_ iotx_dsw_t * shadow,_OU_ int *number);

/**
 * @brief Get property reference from TSL struct by index.
 *        This function used to get property reference from TSL struct by index.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param index. The index of property
 * @param property. The property reference in TSL Struct.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_property_by_index(_IN_ iotx_dsw_t *shadow,_IN_ int index,_OU_ void **property);

/**
 * @brief Get service reference from TSL struct by index.
 *        This function used to get service reference from TSL struct by index.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param index. The index of service
 * @param service. The service reference in TSL Struct.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_service_by_index(_IN_ iotx_dsw_t *shadow,_IN_ int index,_OU_ void **service);

/**
 * @brief Get event reference from TSL struct by index.
 *        This function used to get event reference from TSL struct by index.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param index. The index of event
 * @param event. The event reference in TSL Struct.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_event_by_index(_IN_ iotx_dsw_t *shadow,_IN_ int index,_OU_ void **event);

/**
 * @brief Get service reference from TSL struct by identifier.
 *        This function used to get service reference from TSL struct by identifier.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param identifier. The identifier of event
 * @param service. The service reference in TSL Struct.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_service_by_identifier(_IN_ iotx_dsw_t *shadow, _IN_ char *identifier, _OU_ void **service);

/**
 * @brief Get event reference from TSL struct by identifier.
 *        This function used to get event reference from TSL struct by identifier.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param identifier. The identifier of event
 * @param event. The event reference in TSL Struct.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_event_by_identifier(_IN_ iotx_dsw_t *shadow, _IN_ char *identifier, _OU_ void **event);

/**
 * @brief Get property identifier from TSL struct by service handle.
 *        This function used to get property identifier from TSL struct by service handle.
 *
 * @param service. The handle of property.
 * @param method. The reference to property identifier in TSL Struct
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_property_identifier(_IN_ void *property, _OU_ char **identifier);

/**
 * @brief Get service method from TSL struct by service handle.
 *        This function used to get service method from TSL struct by service handle.
 *
 * @param service. The handle of service.
 * @param method. Generate method from service identifier
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_service_method(_IN_ void *service, _OU_ char **method);

/**
 * @brief Get event method from TSL struct by event handle.
 *        This function used to get event method from TSL struct by event handle.
 *
 * @param service. The handle of event.
 * @param method. Generate method from event identifier
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_event_method(_IN_ void *event, _OU_ char **method);

/**
 * @brief Set Property Value Into TSL Struct.
 *        This function used to set property value into TSL Struct.
 *
 * @param tsl. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: property_id
 *		  array type: property_id(array)[index]
 *        struct type: property_id(struct).property_id or property_id(struct).property_id[index]
 *
 * @param key_len. The length of key
 * @param value. The value to be set, data type decided by data type of property as follows:
 *        int: int*, float: float*, double: double*,
 *        text: char*, enum: int*, date: char*, bool: int*
 *        attention! value can be NULL to clear property value
 * @param value_len. The length of value, only be used when type is text or data
 *
 * @return success or fail.
 *
 */
int iotx_dsw_set_property_value(_IN_ iotx_dsw_t *shadow, _IN_ char *key, _IN_ int key_len, _IN_ void *value, _IN_ int value_len);

/**
 * @brief Get Property Value From TSL Struct.
 *        This function used to get property value from TSL Struct.
 *
 * @param tsl. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: property_id
 *		  array type: property_id(array)[index]
 *        struct type: property_id(struct).property_id or property_id(struct).property_id[index]
 *
 * @param key_len. The length of key
 * @param value. The variable to store value, data type decided by data type of property as follows:
 *        int: int*, float: float*, double: double*,
 *        text: char**, enum: int*, date: char**, bool: int*
 *        attention! value can not be NULL
 *
 * @warning if data type is text or date, *value well be end with '\0'.
 *          the memory allocated to *value must be free by user.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_property_value(_IN_ iotx_dsw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ void *value);

/**
 * @brief Set event output value into TSL struct.
 *        This function used to set event output value into TSL struct.
 *
 * @param tsl. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: event_id.event_data_id
 *		  array type: event_id.event_data_id(array)[index]
 *        struct type: event_id.event_data_id(struct).property_id
 *                     or event_id.event_data_id(struct).property_id[index]
 *
 * @param key_len. The length of key
 * @param value. The value to be set, data type decided by data type of property as follows:
 *        int: int*, float: float*, double: double*,
 *        text: char*, enum: int*, date: char*, bool: int*
 *        attention! value can be NULL to clear property value
 * @param value_len. The length of value, only be used when type is text or data
 *
 * @return success or fail.
 *
 */
int iotx_dsw_set_event_output_value(_IN_ iotx_dsw_t *shadow, _IN_ char *key, _IN_ int key_len, _IN_ void *value, _IN_ int value_len);

/**
 * @brief Get event output value from TSL struct.
 *        This function used to get event output value from TSL struct.
 *
 * @param tsl. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: event_id.event_data_id
 *		  array type: event_id.event_data_id(array)[index]
 *        struct type: event_id.event_data_id(struct).property_id
 *                     or event_id.event_data_id(struct).property_id[index]
 *
 * @param key_len. The length of key
 * @param value. The variable to store value, data type decided by data type of property as follows:
 *        int: int*, float: float*, double: double*,
 *        text: char**, enum: int*, date: char**, bool: int*
 *        attention! value can not be NULL
 *
 * @warning if data type is text or date, *value well be end with '\0'.
 *          the memory allocated to *value must be free by user.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_event_output_value(_IN_ iotx_dsw_t *shadow, _IN_ char *key, _IN_ int key_len, _IN_ void *value);

/**
 * @brief Set service input value from TSL struct.
 *        This function used to set service input value from TSL struct.
 *
 * @param tsl. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: event_id.event_data_id
 *		  array type: event_id.event_data_id(array)[index]
 *        struct type: event_id.event_data_id(struct).property_id
 *                     or event_id.event_data_id(struct).property_id[index]
 *
 * @param key_len. The length of key
 * @param value. The variable to store value, data type decided by data type of property as follows:
 *        int: int*, float: float*, double: double*,
 *        text: char**, enum: int*, date: char**, bool: int*
 *        attention! value can not be NULL
 *
 * @warning if data type is text or date, *value well be end with '\0'.
 *          the memory allocated to *value must be free by user.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_set_service_input_value(_IN_ iotx_dsw_t *shadow, _IN_ char *key, _IN_ int key_len, _IN_ void *value, _IN_ int value_len);

/**
 * @brief Get service input value from TSL struct.
 *        This function used to get service input value from TSL struct.
 *
 * @param tsl. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: event_id.event_data_id
 *		  array type: event_id.event_data_id(array)[index]
 *        struct type: event_id.event_data_id(struct).property_id
 *                     or event_id.event_data_id(struct).property_id[index]
 *
 * @param key_len. The length of key
 * @param value. The variable to store value, data type decided by data type of property as follows:
 *        int: int*, float: float*, double: double*,
 *        text: char**, enum: int*, date: char**, bool: int*
 *        attention! value can not be NULL
 *
 * @warning if data type is text or date, *value well be end with '\0'.
 *          the memory allocated to *value must be free by user.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_service_input_value(_IN_ iotx_dsw_t *shadow, _IN_ char *key, _IN_ int key_len, _IN_ void *value);

/**
 * @brief Set service output value from TSL struct.
 *        This function used to set service output value from TSL struct.
 *
 * @param tsl. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: event_id.event_data_id
 *		  array type: event_id.event_data_id(array)[index]
 *        struct type: event_id.event_data_id(struct).property_id
 *                     or event_id.event_data_id(struct).property_id[index]
 *
 * @param key_len. The length of key
 * @param value. The variable to store value, data type decided by data type of property as follows:
 *        int: int*, float: float*, double: double*,
 *        text: char**, enum: int*, date: char**, bool: int*
 *        attention! value can not be NULL
 *
 * @warning if data type is text or date, *value well be end with '\0'.
 *          the memory allocated to *value must be free by user.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_set_service_output_value(_IN_ iotx_dsw_t *shadow, _IN_ char *key, _IN_ int key_len, _IN_ void *value, _IN_ int value_len);

/**
 * @brief Get service output value from TSL struct.
 *        This function used to get service output value from TSL struct.
 *
 * @param tsl. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: event_id.event_data_id
 *		  array type: event_id.event_data_id(array)[index]
 *        struct type: event_id.event_data_id(struct).property_id
 *                     or event_id.event_data_id(struct).property_id[index]
 *
 * @param key_len. The length of key
 * @param value. The variable to store value, data type decided by data type of property as follows:
 *        int: int*, float: float*, double: double*,
 *        text: char**, enum: int*, date: char**, bool: int*
 *        attention! value can not be NULL
 *
 * @warning if data type is text or date, *value well be end with '\0'.
 *          the memory allocated to *value must be free by user.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_get_service_output_value(_IN_ iotx_dsw_t *shadow, _IN_ char *key, _IN_ int key_len, _IN_ void *value);

/**
 * @brief Get property payload from TSL struct.
 *        This function used to get property payload from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct
 * @param identifier. The Property Identifier
 * @param identifier_len. The Property Identifier Length
 * @param lite. The pointer to json array where to store property value
 *
 * @warning The payload malloc by this function and need to be free manully.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_assemble_property(_IN_ iotx_dsw_t *shadow, _IN_ char *identifier, _IN_ int identifier_len, _IN_ lite_cjson_item_t *lite);

/**
 * @brief Get event output payload from TSL struct.
 *        This function used to get event output payload from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct
 * @param identifier. The Event Identifier
 * @param identifier_len. The Event Identifier Length
 * @param lite. The pointer to json array where to store event output value
 *
 * @warning The payload malloc by this function and need to be free manully.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_assemble_event_output(_IN_ iotx_dsw_t *shadow, _IN_ char *identifier, _IN_ int identifier_len, _IN_ lite_cjson_item_t *lite);

/**
 * @brief Get service output payload from TSL struct.
 *        This function used to get service output payload from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct
 * @param identifier. The Service Identifier
 * @param identifier_len. The Service Identifier Length
 * @param lite. The pointer to json array where to store service output value
 *
 * @warning The payload malloc by this function and need to be free manully.
 *
 * @return success or fail.
 *
 */
int iotx_dsw_assemble_service_output(_IN_ iotx_dsw_t *shadow, _IN_ char *identifier, _IN_ int identifier_len, _IN_ lite_cjson_item_t *lite);

/**
 * @brief Free TSL struct.
 *        This function used to free TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 *
 * @return success or fail.
 *
 */
void iotx_dsw_destroy(_IN_ iotx_dsw_t **shadow);

/**
 * @brief Print detailed information of TSL struct.
 *        This function used to print detailed information of TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 *
 * @return success or fail.
 *
 */
void iotx_dsw_print(_IN_ iotx_dsw_t *shadow);

#endif
