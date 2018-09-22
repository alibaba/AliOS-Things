/*! \file json_parser.h
 * \brief JSON Parser module
 *
 * This module is used to parse a JSON string
 * For JSON generation, please refer \ref json_generator.h
 *
 * \section json_parser_usage Usage
 *
 * The JSON data can be viewed as a tree of key-value pairs. Composite objects
 * indicate special nodes within the tree that anchor other subtrees. Arrays
 * are special nodes that hold a collection of JSON elements.
 *
 * The JSON parser module provides two sets of APIs, one to search inside a
 * JSON object and the other to search inside a JSON array.
 *
 * \subsection obj_search Search inside an Object
 * The APIs in this category are:
 * \li json_get_val_bool()
 * \li json_get_val_int()
 * \li json_get_val_int64()
 * \li json_get_val_float()
 * \li json_get_val_str()
 * \li json_get_composite_object()
 * \li json_release_composite_object()
 * \li json_get_array_object()
 * \li json_release_array_object()
 *
 * \subsection array_search Search inside an array
 * The APIs in this category are:
 * \li json_array_get_bool()
 * \li json_array_get_int()
 * \li json_array_get_in64t()
 * \li json_array_get_float()
 * \li json_array_get_str()
 * \li json_array_get_composite_object()
 * \li json_array_release_composite_object()
 * \li json_array_get_array_object()
 * \li json_array_release_array_object()
 *
 * A valid JSON string is an object and so the object search APIs should be
 * used first.
 *
 * The JSON data that looks like the following:
 * \code
 * { "name" : "JSON Parser",
 *   "version" : 2.0,
 *   "copyright" : 2014,
 *   "supported_el" : ["bool","int","float","str","object","array"],
 *   "features" : { "objects":true,
 *		    "arrays":"yes"
 *		    }
 * }
 * \endcode
 *
 * can be thought of as a tree with a root node that has 5 children "name",
 * "version", "copyright", "supported_el" and "features". The node at
 * "supported_el" is an array with 6 elements. The node at "features" further
 * has 2 child nodes "objects", "arrays".
 * The JSON can be much more complex with multiple levels of objects/arrays and
 * with different types of elements inside an array or object.
 *
 * This JSON parser is based on JSMN which treats the JSON string as a
 * collection of tokens. A token can be an object, array, string or primitive.
 * In the above, "name", "JSON Parser", "version", etc. are all strings.
 * 2.0, 2014, true are primitives.
 * The entire thing is a JSON object, as is the portion beginning from "{"
 * after "features" :.
 * The part between [] is an array.
 * So, in all, the above JSON string consists of 21 tokens. This detail is
 * given here so that the application developers have an idea about how large
 * the JSON token array should be, for successful JSON parsing.
 *
 * Using this JSON parser module you can parse the above elements with the
 * following C code:
 *
 * \code
 * #define NUM_TOKENS	30
 * // Define an array of JSON tokens
 * jsontok_t json_tokens[NUM_TOKENS];
 * jobj_t jobj;
 * // Initialise the JSON parser and parse the given string
 * int err = json_init(&jobj, json_tokens, NUM_TOKENS, json_str_ptr,
 *		strlen(json_str_ptr));
 * if (err != kNoErr)
 *    return err;
 * // Get the value of "name" which is a string
 * json_get_val_str(&jobj, "name", sval, sizeof(sval));
 * // Get the value of "version" which is a float
 * json_get_val_float(&jobj, "version", &fval);
 * // Get the value of "copyright" which is an integer
 * json_get_val_int(&jobj, "copyright", &ival);
 * // Get the JSON array object "supported_el". After this function returns
 * // successfully, only the array search APIs can be used unless the array
 * // is released.
 * json_get_array_object(&jobj, "supported_el", &num_elements);
 * // Get the value at index 2 of the array which is a string value
 * json_array_get_str(&jobj, 2, sval, sizeof(sval));
 *  // Release the array object so that the scope of searches returns back
 *  // to the parent object.
 *  json_release_array_object(&jobj);
 * // Get the JSON composite object "features". After this function returns
 * // successfully, the scope of the subsequent searches is constrained to
 * // within this object, unless the object is released.
 * json_get_composite_object(&jobj, "features");
 * // Get the value of "objects" which is a boolean
 * json_get_val_bool(&jobj, "objects", &bval);
 * // Release the composite object so that the scope of searches returns back
 * // to the parent object.
 * json_release_composite_object(&jobj);
 *
 * \endcode
 */

/*
 *  Copyright (C) 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */
#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

#include "mx_common.h"

/* Globally unique success code */

typedef int  js_status;

/**
 * JSON token. Application just needs to define an array of these tokens and
 * pass it to json_init().
 */
typedef struct {
	/** Type of the token, like object, string, array, etc. */
	uint8_t type;
	/** Number of children of this token. Valid for objects and arrays. */
	uint8_t size;
	/** Start position in JSON data string. */
	int16_t start;
	/** End position in JSON data string. */
	int16_t end;
	/**  Parent token's index in the token's arrays. */
	int16_t parent;
} jsontok_t;

/*
 * JSON parser Object.
 *
 * This is for internal use only.
 * This should not be used by application code
 */
typedef struct {
	/* Offset in the JSON string */
	int16_t pos;
	/* Next token to allocate */
	int16_t toknext; /* next token to allocate */
	/* Superior token node, e.g parent object or array */
	int16_t toksuper;
} json_parser_t;

/** Object used by the JSON parser internally. This object needs to be
 * defined by the application but all the elements will be set internally
 * by the JSON parser. Please refer \ref json_parser_usage for more information.
 *
 * \note Please do not modify any elements of this object
 */
typedef struct {
	/** Internal JSON parser object */
	json_parser_t parser;
	/** Pointer to JSON string */
	char *js;
	/** Pointer to JSON tokens array */
	jsontok_t *tokens;
	/** Pointer to current active JSON token */
	jsontok_t *cur;
	/** Number of tokens available for parser*/
	int num_tokens;
} jobj_t;

/** Initialize the JSON Parser and parse the given JSON string.
 *
 * This function initializes the JSON object that will be used for
 * all the subsequent JSON parser APIs. It also parses the given JSON
 * string and sets up the internal jobj for the subsequent APIs.
 *
 * \param[in,out] jobj Pointer to a JSON object \ref jobj_t assigned by the
 * application. This is set up for further use internally by this API.
 * \param[in] tokens Pointer to an array of JSON tokens \ref jsontok_t
 * assigned by the application. To get an idea about how large the array should
 * be, please refer \ref json_parser_usage section.
 * \param[in] num_tokens Number of tokens in the tokens array.
 * \param[in] js Pointer to the JSON string.
 * \param[in] js_len Length of the JSON string.
 *
 * \return kNoErr on successful parsing. json_is_object() and/or
 * json_is_array() can then be used to find out if the parsed string is a
 * JSON object or array so that the appropriate parsing APIs can be used.
 * \return kNoMemoryErr if insufficient number of tokens are provided in
 * json_init().
 * \return kFormatErr if there are invalid characters in the given JSON
 * string.
 * \return kMalformedErr if the given JSON string is incomplete.
 * \return kFormatErr if the given json string is not a valid
 * JSON object.
 * \return kGeneralErr if any other error was encountered.
 */
int json_init(jobj_t *jobj, jsontok_t *tokens, int num_tokens,
		char *js, int js_len);

/** Find out if the current object is a JSON Object
 *
 * \param[in] jobj The current JSON object pointer of type \ref jobj_t
 *
 * \return true if it is an object
 * \return false if it is not an object
 */
bool json_is_object(jobj_t *jobj);

/** Find out if the current object is a JSON Array
 *
 * \param[in] jobj The current JSON object pointer of type \ref jobj_t
 *
 * \return true if it is an array
 * \return false if it is not an array
 */
bool json_is_array(jobj_t *jobj);
/** Get JSON bool value
 *
 * Gets the value of a JSON boolean element from an object based on
 * the given key.
 *
 * \param[in] jobj Current JSON search object \ref jobj_t.
 * \param[in] key Name of the JSON element.
 * \param[out] value Pointer to a boolean variable assigned by the application.
 * This will hold the value on success.
 *
 * \return kNoErr on success
 * \return kNotFoundErr if an element with the given key was not found.
 * \return kFormatErr if an element with the given key was found
 * but it was not a boolean.
 * \return kFormatErr if the search object was not a valid JSON
 * object.
 * \return kGeneralErr if any other error was encountered.
 */
int json_get_val_bool(jobj_t *jobj, char *key, bool *value);

/** Get JSON integer value
 *
 * Gets the value of a JSON integer element from an object based on
 * the given key.
 *
 * \param[in] jobj Current JSON search object \ref jobj_t.
 * \param[in] key Name of the JSON element.
 * \param[out] value Pointer to an integer variable assigned by the
 * application. This will hold the value on success.
 *
 * \return kNoErr on success
 * \return kNotFoundErr if an element with the given key was not found.
 * \return kFormatErr if an element with the given key was found
 * but it was not an integer.
 * \return kFormatErr if the search object was not a valid JSON
 * object.
 * \return kGeneralErr if any other error was encountered.
 */
int json_get_val_int(jobj_t *jobj, char *key, int *value);

/** Get 64bit JSON integer value
 *
 * Gets the value of a JSON integer element from an object based on
 * the given key.
 *
 * \param[in] jobj Current JSON search object \ref jobj_t.
 * \param[in] key Name of the JSON element.
 * \param[out] value Pointer to a 64bit integer variable assigned by the
 * application. This will hold the value on success.
 *
 * \return kNoErr on success
 * \return kNotFoundErr if an element with the given key was not found.
 * \return kFormatErr if an element with the given key was found
 * but it was not an integer.
 * \return kFormatErr if the search object was not a valid JSON
 * object.
 * \return kGeneralErr if any other error was encountered.
 */
int json_get_val_int64(jobj_t *jobj, char *key, int64_t *value);

/** Get JSON float value
 *
 * Gets the value of a JSON float element from an object based on
 * the given key.
 *
 * \param[in] jobj Current JSON search object \ref jobj_t.
 * \param[in] key Name of the JSON element.
 * \param[out] value Pointer to a float variable assigned by the application.
 * This will hold the value on success.
 *
 * \return kNoErr on success
 * \return kNotFoundErr if an element with the given key was not found.
 * \return kFormatErr if an element with the given key was found
 * but it was not a float.
 * \return kFormatErr if the search object was not a valid JSON
 * object.
 * \return kGeneralErr if any other error was encountered.
 */
int json_get_val_float(jobj_t *jobj, char *key, float *value);

/** Get JSON string value
 *
 * Gets the value of a JSON string element from an object based on
 * the given key.
 *
 * \param[in] jobj Current JSON search object \ref jobj_t.
 * \param[in] key Name of the JSON element.
 * \param[out] value Pointer to a buffer assigned by the application to hold
 * the string. This will hold the null terminated string on success.
 * \param[in] max_len Length of the buffer passed.
 *
 * \return kNoErr on success
 * \return kNotFoundErr if an element with the given key was not found.
 * \return kFormatErr if an element with the given key was found
 * but it was not a string.
 * \return kFormatErr if the search object was not a valid JSON
 * object.
 * \return kNoMemoryErr if the buffer provided was insufficient.
 * \return kGeneralErr if any other error was encountered.
 */
int json_get_val_str(jobj_t *jobj, char *key, char *value, int max_len);

/** Get JSON string length
 *
 * Gets the length of a JSON string element from an object based on
 * the given key. This is useful when the application does not know
 * what size of buffer to use for json_get_val_str()
 *
 * \param[in] jobj Current JSON search object \ref jobj_t.
 * \param[in] key Name of the JSON element.
 * \param[out] len Pointer to an integer assigned by the application to hold
 * the length of string. Applications will have to use a buffer of size
 * atleast 1 byte more than this length
 *
 * \return kNoErr on success
 * \return kNotFoundErr if an element with the given key was not found.
 * \return kFormatErr if an element with the given key was found
 * but it was not a string.
 * \return kFormatErr if the search object was not a valid JSON
 * object.
 * \return kGeneralErr if any other error was encountered.
 */
int json_get_val_str_len(jobj_t *jobj, char *key, int *len);

/** Get JSON composite object
 *
 * Gets a composite JSON object from another object based on the given key.
 *
 * \param[in,out] jobj Current JSON search object \ref jobj_t.
 * On success, the jobj will be modified such that the scope of subsequent
 * searches will be limited to this composite object. Use
 * json_release_composite_object() to expand the scope back to the parent
 * object.
 * \param[in] key Name of the JSON element.
 *
 * \return kNoErr on success
 * \return kNotFoundErr if an element with the given key was not found.
 * \return kFormatErr if an element with the given key was found
 * but it was not an object.
 * \return kFormatErr if the search object was not a valid JSON
 * object or the object found was itself not a valid JSON object.
 * \return kGeneralErr if any other error was encountered.
 */
int json_get_composite_object(jobj_t *jobj, char *key);

/** Release a JSON composite object
 *
 * This function expands the scope of searches back to the parent object, if it
 * was previously constrained using json_get_composite_object().
 *
 * \param[in,out] jobj Current JSON search object \ref jobj_t. On success, this
 * will be modified such that the scope of future searches will be expanded
 * back to the parent object.
 *
 * \return kNoErr on success.
 * \return kGeneralErr if any error was encountered.
 */
int json_release_composite_object(jobj_t *jobj);

/** Get JSON array object
 *
 * Gets a JSON array object from another object based on the given key.
 *
 * \param[in,out] jobj Current JSON search object \ref jobj_t.
 * On success, the jobj will be modified such that the scope of subsequent
 * searches will be limited to this array. Use json_release_array_object()
 * to expand the scope back to the parent object.
 * \param[in] key Name of the JSON element.
 * \param[out] num_elements Pointer to an integer assigned by the application.
 * On sucess, this will hold the number of elements in the array found.
 *
 * \return kNoErr on success
 * \return kNotFoundErr if an element with the given key was not found.
 * \return kFormatErr if an element with the given key was found
 * but it was not an array.
 * \return kFormatErr if the search object was not a valid JSON
 * object.
 * \return kGeneralErr if any other error was encountered.
 */
int json_get_array_object(jobj_t *jobj, char *key, int *num_elements);

/** Release a JSON array object
 *
 * This function expands the scope of searches back to the parent object, if it
 * was previously constrained using json_get_array_object().
 *
 * \param[in,out] jobj Current JSON search object \ref jobj_t. On success, this
 * will be modified such that the scope of future searches will be expanded
 * back to the parent object.
 *
 * \return kNoErr on success.
 * \return kGeneralErr if any error was encountered.
 */
int json_release_array_object(jobj_t *jobj);

/** Get number of elements in an array
 *
 * This API is valid only if the current scope is an array
 *
 * \param[in] jobj The current JSON object pointer.
 *
 * \return Number of elements in the JSON Array
 * \return kGeneralErr if the current scope is not an array
 */
int json_array_get_num_elements(jobj_t *jobj);

/** Get JSON bool value from array
 *
 * Gets the value of a JSON boolean element from an array based on the given
 * index.
 *
 * \param[in] jobj Current JSON search object \ref jobj_t.
 * \param[in] index Index in the array (beginning from 0).
 * \param[out] value Pointer to a boolean variable assigned by the application.
 * This will hold the value on success.
 *
 * \return kNoErr on success
 * \return kFormatErr if the index provided was invalid.
 * \return kFormatErr if an element was found at the given index,
 * but it was not a boolean.
 * \return kFormatErr if the search object was not a valid JSON
 * array.
 * \return kGeneralErr if any other error was encountered.
 */
int json_array_get_bool(jobj_t *jobj, uint16_t index, bool *value);

/** Get JSON integer value from array
 *
 * Gets the value of a JSON integer element from an array based on the given
 * index.
 *
 * \param[in] jobj Current JSON search object \ref jobj_t.
 * \param[in] index Index in the array (beginning from 0).
 * \param[out] value Pointer to an integer variable assigned by the
 * application. This will hold the value on success.
 *
 * \return kNoErr on success
 * \return kFormatErr if the index provided was invalid.
 * \return kFormatErr if an element was found at the given index,
 * but it was not an integer.
 * \return kFormatErr if the search object was not a valid JSON
 * array.
 * \return kGeneralErr if any other error was encountered.
 */
int json_array_get_int(jobj_t *jobj, uint16_t index, int *value);

/** Get 64bit JSON integer value from array
 *
 * Gets the value of a JSON integer element from an array based on the given
 * index.
 *
 * \param[in] jobj Current JSON search object \ref jobj_t.
 * \param[in] index Index in the array (beginning from 0).
 * \param[out] value Pointer to a 64bit integer variable assigned by the
 * application. This will hold the value on success.
 *
 * \return kNoErr on success
 * \return kFormatErr if the index provided was invalid.
 * \return kFormatErr if an element was found at the given index,
 * but it was not an integer.
 * \return kFormatErr if the search object was not a valid JSON
 * array.
 * \return kGeneralErr if any other error was encountered.
 */
int json_array_get_int64(jobj_t *jobj, uint16_t index, int64_t *value);

/** Get JSON float value from array
 *
 * Gets the value of a JSON float element from an array based on the given
 * index.
 *
 * \param[in] jobj Current JSON search object \ref jobj_t.
 * \param[in] index Index in the array (beginning from 0).
 * \param[out] value Pointer to a float variable assigned by the application.
 * This will hold the value on success.
 *
 * \return kNoErr on success
 * \return kFormatErr if the index provided was invalid.
 * \return kFormatErr if an element was found at the given index,
 * but it was not a float.
 * \return kFormatErr if the search object was not a valid JSON
 * array.
 * \return kGeneralErr if any other error was encountered.
 */
int json_array_get_float(jobj_t *jobj, uint16_t index, float *value);

/** Get JSON string value from array
 *
 * Gets the value of a JSON string element from an array based on the given
 * index.
 *
 * \param[in] jobj Current JSON search object \ref jobj_t.
 * \param[in] index Index in the array (beginning from 0).
 * \param[out] value Pointer to a buffer assigned by the application to hold
 * the string. This will hold the null terminated string on success.
 * \param[in] maxlen Length of the buffer passed.
 *
 * \return kNoErr on success
 * \return kFormatErr if the index provided was invalid.
 * \return kFormatErr if an element was found at the given index,
 * but it was not a string.
 * \return kFormatErr if the search object was not a valid JSON
 * array.
 * \return kNoMemoryErr if the buffer provided was insufficient.
 * \return kGeneralErr if any other error was encountered.
 */
int json_array_get_str(jobj_t *jobj, uint16_t index, char *value, int maxlen);

/** Get JSON string length from array
 *
 * Gets the length of a JSON string element from an array based on
 * the given index. This is useful when the application does not know
 * what size of buffer to use for json_array_get_str()
 *
 * \param[in] jobj Current JSON search object \ref jobj_t.
 * \param[in] index Index in the array (beginning from 0).
 * \param[out] len Pointer to an integer assigned by the application to hold
 * the length of string. Applications will have to use a buffer of size
 * atleast 1 byte more than this length
 *
 * \return kNoErr on success
 * \return kFormatErr if the index provided was invalid.
 * \return kFormatErr if an element was found at the given index,
 * but it was not a string.
 * \return kFormatErr if the search object was not a valid JSON
 * array.
 * \return kGeneralErr if any other error was encountered.
 */
int json_array_get_str_len(jobj_t *jobj, uint16_t index, int *len);

/** Get JSON composite object from array
 *
 * Gets a composite JSON object from an array based on the given index.
 *
 * \param[in,out] jobj Current JSON search object \ref jobj_t.
 * On success, the jobj will be modified such that the scope of subsequent
 * searches will be limited to this composite object. Use
 * json_array_release_composite_object() to expand the scope back to the
 * parent array.
 * \param[in] index Index in the array (beginning from 0).
 *
 * \return kNoErr on success
 * \return kFormatErr if the index provided was invalid.
 * \return kFormatErr if an element was found at the given index,
 * but it was not an object.
 * \return kFormatErr if the search object was not a valid JSON
 * array.
 * \return kFormatErr if the object found was not a valid JSON
 * object.
 * \return kGeneralErr if any other error was encountered.
 */
int json_array_get_composite_object(jobj_t *jobj, uint16_t index);

/** Release a JSON composite object from an array
 *
 * This function expands the scope of searches back to the parent array, if it
 * was previously constrained using json_array_get_composite_object().
 *
 * \param[in,out] jobj Current JSON search object \ref jobj_t. On success, this
 * will be modified such that the scope of future searches will be expanded
 * back to the parent array.
 *
 * \return kNoErr on success.
 * \return kGeneralErr if any error was encountered.
 */
int json_array_release_composite_object(jobj_t *jobj);

/** Get JSON array object from array
 *
 * Gets a JSON array object from another array based on the given index.
 *
 * \param[in,out] jobj Current JSON search object \ref jobj_t.
 * On success, the jobj will be modified such that the scope of subsequent
 * searches will be limited to this composite object. Use
 * json_array_release_composite_object() to expand the scope back to the
 * parent array.
 * \param[in] index Index in the array (beginning from 0).
 * \param[out] num_elements Pointer to an integer assigned by the application.
 * On sucess, this will hold the number of elements in the array found.
 *
 * \return kNoErr on success
 * \return kFormatErr if the index provided was invalid.
 * \return kFormatErr if an element was found at the given index,
 * but it was not an array.
 * \return kFormatErr if the search object was not a valid JSON
 * array.
 * \return kGeneralErr if any other error was encountered.
 */
int json_array_get_array_object(jobj_t *jobj, uint16_t index,
		int *num_elements);

/** Release a JSON array object from an array
 *
 * This function expands the scope of searches back to the parent array, if it
 * was previously constrained using json_array_get_array_object().
 *
 * \param[in,out] jobj Current JSON search object \ref jobj_t. On success, this
 * will be modified such that the scope of future searches will be expanded
 * back to the parent array.
 *
 * \return kNoErr on success.
 * \return kGeneralErr if any error was encountered.
 */
int json_array_release_array_object(jobj_t *jobj);

#endif /* __JSON_PARSER_H__ */
