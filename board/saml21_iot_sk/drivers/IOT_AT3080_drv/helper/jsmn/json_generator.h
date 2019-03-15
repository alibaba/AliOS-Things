/*! \file json_generator.h
 *
 * \brief JSON generator module
 *
 * This module is used to generate JSON strings
 *
 * \section json_gen_usage Usage
 *
 * The JSON Generator can create various JSON strings. Here is an example:
 *
 * \code
 * json_str_init(&jstr, buff, sizeof(buff));
 * // This initializes the json generator to generate string in the buffer buff
 *
 * json_start_object(&jstr);
 * json_set_val_str(&jstr, "name", "John Travolta");
 * json_set_val_int(&jstr, "roll", 123);
 * json_push_object(&jstr, "address");
 * json_set_val_str(&jstr, "house", "Atlas");
 * json_set_val_str(&jstr, "street", "New Avenue");
 * json_set_val_int(&jstr, "pin", 39065);
 * json_pop_object(&jstr);
 * json_close_object(&jstr);
 *
 * \endcode
 * will create the above mentioned JSON string.
 *
 * Similarly arrays can be created as:
 * \code
 * json_start_object(&jstr);
 * json_push_array_object(&jstr, "roll");
 * json_set_array_int(&jstr, 10);
 * json_set_array_int(&jstr, 20);
 * json_set_array_int(&jstr, 30);
 * json_pop_array_object(&jstr);
 * json_close_object(&jstr);
 * \endcode
 * will create the JSON string
 *
 * \code
 * { "roll": [ [ 10, 20, 30] ] }
 * \endcode
 *
 * Similarly complex use-case like objects within arrays can be created as:
 * \code
 * json_str_init(&jstr, buff, sizeof(buff));
 * json_start_object(&jstr);
 * json_push_array_object(&jstr, "arr");
 * json_start_object(&jstr);
 * json_set_val_str(&jstr, "name", "John Travolta");
 * json_set_val_int(&jstr, "roll", 123);
 * json_close_object(&jstr);
 * json_start_object(&jstr);
 * json_set_val_str(&jstr, "name", "Foo Bar");
 * json_set_val_int(&jstr, "roll", 112);
 * json_close_object(&jstr);
 * json_pop_array_object(&jstr);
 * json_close_object(&jstr);
 * \endcode
 * will create the JSON string
 * \code
 * { "arr": [
 *          { "name": "John Travolta", "roll": 123 },
 *          { "name": "Foo Bar", "roll": 112 }
 *      ]
 * }
 * \endcode
 */

/*
 *  Copyright (C) 2014, Marvell International Ltd.
 *  All Rights Reserved.
 */
#ifndef _JSON_GENERATOR_H_
#define _JSON_GENERATOR_H_

#include "mx_common.h"
#include "json_escape_str.h"

#define MAX_JSON_STR_LEN  64	/* Maximum object or member name length */
#define MAX_JSON_VAL_LEN  128	/* Maximum value name length */
typedef enum {
	JSON_VAL_STR,
	JSON_VAL_INT,
	JSON_VAL_UINT,
	JSON_VAL_UINT_64,
	JSON_VAL_FLOAT,
	JSON_VAL_BOOL,
	JSON_VAL_NULL,
} json_data_types;

struct json_str {
	char *buff;
	int len;
	int free_ptr;	/* Where the \0 currently is, start writing from this
			 * offset */
};
/** Initialize the JSON generator
 *
 * This function initializes the JSON generator.
 *
 * \param[out] jptr A pointer to the structure json_str which will be used as a
 * handle for the rest of the json_str operations.
 * \param[out] buff A pointer to a buffer that will be used to store the
 * generated JSON data.
 * \param[in] len The length of the buffer pointed to by buff
 */
void json_str_init(struct json_str *jptr, char *buff, int len);

/** Start a new JSON object
 *
 * This function starts a new JSON object
 *
 * \param[in] jptr Pointer to json_str object.
 *
 * \return kOverrunErr if buffer available is insufficient or a buffer
 * overflow has already occurred.
 * \return kNoErr if operation successful.
 */
int json_start_object(struct json_str *jptr);

/** Start a new composite object
 *
 * This function starts a new composite object which may have its own set of
 * key-value pairs. Any json_set() calls performed after this will create
 * key-value pairs within this composite object.
 *
 * \param[in] jptr A pointer to the json_str handle returned by the
 * json_str_init() call.
 * \param[in] name The name of the composite object that should be created.
 *
 * \return kOverrunErr if buffer available is insufficient or a buffer
 * overflow has already occurred.
 * \return kNoErr if operation successful.
 */
int json_push_object(struct json_str *jptr, const char *name);

/** Close a composite object
 *
 * This closes a composite object. Any json_set() calls after this will create
 * key-value pairs at the same depth as the composite object that was closed.
 */
#define json_pop_object(jptr)  json_close_object((jptr))

/** Start a JSON array object.
 *
 * This function creates an element that has an array as its value.
 *
 * \param[in] jptr A pointer to the json_str handle returned by the
 * json_str_init() call.
 * \param[in] name The name of the array object
 *
 * \return kOverrunErr if buffer available is insufficient or a buffer
 * overflow has already occurred.
 * \return kNoErr if operation successful.
 */
int json_push_array_object(struct json_str *jptr, const char *name);

/** Close a JSON array object.
 *
 * This closes a previously pushed JSON object.
 *
 * \param[in] jptr Pointer to json_str object.
 *
 * \return kOverrunErr if buffer available is insufficient or a buffer
 * overflow has already occurred.
 * \return kNoErr if operation successful.
 */
int json_pop_array_object(struct json_str *jptr);

/** Close JSON object
 *
 * This function closes a JSON object that was started earlier.
 *
 * \param[in] jptr Pointer to json_str object.
 *
 * \return kOverrunErr if buffer available is insufficient or a buffer
 * overflow has already occurred.
 * \return kNoErr if operation successful.
 */
int json_close_object(struct json_str *jptr);

int json_set_object_value(struct json_str *jptr, const char *name,
			  const char *str, int64_t value, float val,
			  short precision, json_data_types data);

/** Create a key with a string value.
 *
 * This function adds a key-value pair to the JSON text with string as the
 * value.
 *
 * \param[in] jptr A pointer to the json_str handle returned by the
 * json_str_init() call.
 * \param[in] name The name of the key
 * \param[in] val The string value of the key
 */
#define json_set_val_str(jptr, name, val) \
		json_set_object_value(jptr, name, val, 0, 0.0, 0, \
				JSON_VAL_STR)

/** Create a key with an integer value.
 *
 * This function adds a key-value pair to the JSON text with an integer as the
 * value.
 *
 * \param[in] jptr A pointer to the json_str handle returned by the
 * json_str_init() call.
 * \param[in] name The name of the key
 * \param[in] val The integer value of the key
 */
#define json_set_val_int(jptr, name, val) \
		json_set_object_value(jptr, name, NULL, val, 0.0, 0, \
				JSON_VAL_INT)

/** Create a key with an unsigned integer value.
 *
 * This function adds a key-value pair to the JSON text with an unsigned integer
 * as the value.
 *
 * \param[in] jptr A pointer to the json_str handle returned by the
 * json_str_init() call.
 * \param[in] name The name of the key
 * \param[in] val The unsigned integer value of the key
 */
#define json_set_val_uint(jptr, name, val) \
	json_set_object_value(jptr, name, NULL, val, 0.0, 0, JSON_VAL_UINT)

/** Create a key with an unsigned 64 bit integer value.
 *
 * This function adds a key-value pair to the JSON text with an unsigned 64 bit
 * integer as the value.
 *
 * \param[in] jptr A pointer to the json_str handle returned by the
 * json_str_init() call.
 * \param[in] name The name of the key
 * \param[in] val The unsigned 64 bit integer value of the key
 */
#define json_set_val_uint_64(jptr, name, val) \
	json_set_object_value(jptr, name, NULL, val, 0.0, 0, \
			JSON_VAL_UINT_64)

/** Create a key with a float value.
 *
 * This function adds a key-value pair to the JSON text with a float as the
 * value.
 *
 * \param[in] jptr A pointer to the json_str handle returned by the
 * json_str_init() call.
 * \param[in] name The name of the key
 * \param[in] val The float value of the key
 */
#define json_set_val_float(jptr, name, val) \
		json_set_object_value(jptr, name, NULL, 0, val, 2, \
				JSON_VAL_FLOAT)

/** Create a key with a float value and decimal precision.
 *
 * This function adds a key-value pair to the JSON text with a float as the
 * value.
 *
 * \param jptr A pointer to the json_str handle returned by the json_str_init()
 * call.
 * \param[in] name The name of the key
 * \param[in] val The float value of the key
 * \param[in] precision The number of precision digits for float
 */
#define json_set_val_float_precision(jptr, name, val, precision) \
		json_set_object_value(jptr, name, NULL, 0, \
				val, precision, JSON_VAL_FLOAT)

/** Create a key with an boolean value.
 *
 * This function adds a key-value pair to the JSON text with an integer as the
 * value.
 *
 * \param[in] jptr A pointer to the json_str handle returned by the
 * json_str_init() call.
 * \param[in] name The name of the key
 * \param[in] val The boolean value of the key.
 */
#define json_set_val_bool(jptr, name, val) ((val == true) ? \
		(json_set_object_value(jptr, name, NULL, 1, 0.0, 0, \
				JSON_VAL_BOOL)) : \
		(json_set_object_value(jptr, name, NULL, 0, 0.0, 0, \
				JSON_VAL_BOOL)))

/** Create a key with a null value.
 *
 * This function adds a key-value pair to the JSON text with null as the
 * value.
 *
 * \param[in] jptr A pointer to the json_str handle returned by the
 * json_str_init() call.
 * \param[in] name The name of the key
 */
#define json_set_val_null(jptr, name) \
		(json_set_object_value(jptr, name, NULL, 0, 0.0, 0, \
				JSON_VAL_NULL))

/** Start a JSON array
 *
 * This function starts a JSON array. This is different than the
 * json_push_arraj_object(), in that json_push_array_object() creates an
 * key-value pair with an array as the value, while this simply starts an array
 * object.
 *
 * \param[in] jptr Pointer to json_str object.
 *
 * \return kOverrunErr if buffer available is insufficient or a buffer
 * overflow has already occurred.
 * \return kNoErr if operation successful.
 */
int json_start_array(struct json_str *jptr);

/** Close a JSON array
 *
 * This function closes a JSON array started by json_start_array().
 *
 * \param[in] jptr Pointer to json_str object.
 *
 * \return kOverrunErr if buffer available is insufficient or a buffer
 * overflow has already occurred.
 * \return kNoErr if operation successful.
 */
int json_close_array(struct json_str *jptr);

int json_cli_init(void);

int json_set_array_value(struct json_str *jptr, char *str, int value,
		float val, json_data_types data);

/** Create a string array element
 *
 * This function creates a string array element in a previously started JSON
 * array.
 *
 * \param[in] jptr  A pointer to the json_str handle returned by the
 * json_str_init() call.
 * \param[in] val A pointer to the string value of the array element.
 */
#define json_set_array_str(jptr, val)  \
		json_set_array_value(jptr, val, 0, 0.0, JSON_VAL_STR)

/** Create an integer array element
 *
 * This function creates an integer array element in a previously started JSON
 * array.
 *
 * \param[in] jptr  A pointer to the json_str handle returned by the
 * json_str_init() call.
 * \param[in] val The integer value of the array element.
 */
#define json_set_array_int(jptr, val) \
		json_set_array_value(jptr, NULL, val, 0.0, JSON_VAL_INT)

/** Create a float array element
 *
 * This function creates a float array element in a previously started JSON
 * array.
 *
 * \param[in] jptr  A pointer to the json_str handle returned by the
 * json_str_init() call.
 * \param[in] val A pointer to the float value of the array element.
 */
#define json_set_array_float(jptr, val) \
		json_set_array_value(jptr, NULL, 0, val, JSON_VAL_FLOAT)

/** Create an boolean array element
 *
 * This function creates an boolean array element in a previously started JSON
 * array.
 *
 * \param[in] jptr  A pointer to the json_str handle returned by the
 * json_str_init() call.
 * \param[in] val The boolean value of the array element.
 */
#define json_set_array_bool(jptr, val) ((val == true) ? \
		(json_set_array_value(jptr, NULL, 1, 0.0, JSON_VAL_BOOL)) : \
		(json_set_array_value(jptr, NULL, 0, 0.0, JSON_VAL_BOOL)))
#endif /* _JSON_GENERATOR_H_ */
