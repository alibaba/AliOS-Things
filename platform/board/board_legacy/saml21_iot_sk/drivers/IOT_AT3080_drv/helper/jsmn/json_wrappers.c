/*
 *  Copyright (C) 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */
#include "mx_common.h"
#include "json_parser.h"
#include "jsmn.h"
#include "json_utils.h"

#define CONFIG_JSON_FLOAT

/* Parse states are used to get a value based on the key */
typedef enum {KEY, FOUND, SKIP} parse_state;

/* Returns true if an exact string match is found, else false */
static bool json_token_streq(char *js, jsontok_t *t, char *s)
{
	return (strncmp(js + t->start, s, t->end - t->start) == 0
			&& strlen(s) == (size_t) (t->end - t->start));
}

/* Skips to the last element of an array or object.
 * If there is an array/object inside the given array/object,
 * the function is called recursively to skip all elements
 */
static jsontok_t *skip_to_last(jsontok_t *element)
{
	jsontok_t *t = element;
	if (t->size == 0)
		return t;
	int cnt = t->size;
	while (cnt--) {
		t++;
		if (t->size)
			t = skip_to_last(t);
	}
	return t;
}
/* Converts the value held by the token into a boolean */
static int json_str_to_bool(jobj_t *jobj, jsontok_t *t, bool *value)
{
	if (!t || t->type != JSMN_PRIMITIVE)
		return kFormatErr;
	if (json_token_streq(jobj->js, t, "true") ||
			json_token_streq(jobj->js, t, "1"))
		*value = true;
	else if (json_token_streq(jobj->js, t, "false") ||
			json_token_streq(jobj->js, t, "0"))
		*value = false;
	else
		return kFormatErr;
	return kNoErr;

}

/* Converts the value held by the token into an integer */
static int json_str_to_int(jobj_t *jobj, jsontok_t *t, int *value)
{
	if (!t || t->type != JSMN_PRIMITIVE)
		return kFormatErr;
	char *endptr;
	int int_val = strtoul(&jobj->js[t->start], &endptr, 10);
	if (endptr == &(jobj->js[t->end])) {
		*value = int_val;
		return kNoErr;
	} else {
		return kFormatErr;
	}
}
/* Converts the value held by the token into an int64 */
static int json_str_to_int64(jobj_t *jobj, jsontok_t *t, int64_t *value)
{
	if (!t || t->type != JSMN_PRIMITIVE)
		return kFormatErr;
	char *endptr;
	int64_t int_val = strtoull(&jobj->js[t->start], &endptr, 10);
	if (endptr == &(jobj->js[t->end])) {
		*value = int_val;
		return kNoErr;
	} else {
		return kFormatErr;
	}
}
#ifdef CONFIG_JSON_FLOAT
/* Converts the value held by the token into a float */
static int json_str_to_float(jobj_t *jobj, jsontok_t *t, float *value)
{
	if (!t || t->type != JSMN_PRIMITIVE)
		return kFormatErr;
	char *start_ptr = &jobj->js[t->start];
	char *endptr;

	*value = mico_strtof(start_ptr, &endptr);
	if (endptr != &(jobj->js[t->end]))
		return kFormatErr;
	return kNoErr;
}
#endif /* CONFIG_JSON_FLOAT */

/* Converts the value held by the token into a null terminated string */
static int json_str_to_str(jobj_t *jobj, jsontok_t *t, char *value, int maxlen)
{
	if (!t || t->type != JSMN_STRING)
		return kFormatErr;
	if ((t->end - t->start) >= maxlen)
		return kNoMemoryErr;
	strncpy(value, jobj->js + t->start, t->end - t->start);
	value[t->end - t->start] = 0;
	return kNoErr;
}

/* Searches for json element inside an object based on the key and populates
 * the val_t token if element is found and returns kNoErr.
 * If not found, returns error.
 */
static int json_get_value(jobj_t *jobj, char *key, jsontok_t **val_t)
{
	jsontok_t *t = jobj->cur;
	int num_children = t->size;
	*val_t = NULL;
	/* If there are no children or not even number of children,
	 * it is an error.
	 * Number of children should be even because the object consists
	 * of key:value pairs.
	 */
	if ((num_children == 0) || ((num_children % 2) != 0))
		return kGeneralErr;

	/* If the current token type is not an object, it is an error */
	if (t->type != JSMN_OBJECT)
		return kFormatErr;
	parse_state state = KEY;
	while (num_children--) {
		/* Increment the token pointer first so that we begin from the
		 * first token inside the object.
		 */
		t++;
		/* For safety, check if the current token's end does not go
		 * beyond the parent object's end. This case is unlikely, yet,
		 * better to have a check.
		 */
		if (t->end > jobj->cur->end)
			return kGeneralErr;
		switch (state) {
		case KEY:
			/* First token inside an object should be a key.
			 * If not, it is an error.
			 */
			if (t->type != JSMN_STRING)
				return kGeneralErr;
			/* If the key matches with the given key, the member
			 * has been found. The next token will be its value.
			 * Else, just skip the value.
			 */
			if (json_token_streq(jobj->js, t, key))
					state = FOUND;
			else
				state = SKIP;
			break;
		case SKIP:
			/* If the value to be skipped is an array or object,
			 * skip to its last element.
			 */
			if (t->type == JSMN_ARRAY || t->type == JSMN_OBJECT)
				t = skip_to_last(t);
			state = KEY;
			break;

		case FOUND:
			/* Value found. Populate the token pointer and return
			 * success.
			 */
			*val_t = t;
			return kNoErr;
		}
	}
	return kNotFoundErr;
}

/* Search boolean value based on given key */
int json_get_val_bool(jobj_t *jobj, char *key, bool *value)
{
	jsontok_t *t;
	int ret = json_get_value(jobj, key, &t);
	if (ret != kNoErr)
		return ret;
	return json_str_to_bool(jobj, t, value);
}
/* Search integer value based on given key */
int json_get_val_int(jobj_t *jobj, char *key, int *value)
{
	jsontok_t *t;
	int ret = json_get_value(jobj, key, &t);
	if (ret != kNoErr)
		return ret;
	return json_str_to_int(jobj, t, value);
}
/* Search int64 value based on given key */
int json_get_val_int64(jobj_t *jobj, char *key, int64_t *value)
{
	jsontok_t *t;
	int ret = json_get_value(jobj, key, &t);
	if (ret != kNoErr)
		return ret;
	return json_str_to_int64(jobj, t, value);
}
/* Search float value based on given key */
#ifdef CONFIG_JSON_FLOAT
int json_get_val_float(jobj_t *jobj, char *key, float *value)
{
	jsontok_t *t;
	int ret = json_get_value(jobj, key, &t);
	if (ret != kNoErr)
		return ret;
	return json_str_to_float(jobj, t, value);
}
#else
int json_get_val_float(jobj_t *jobj, char *key, float *value)
{
	return kGeneralErr;
}
#endif /* CONFIG_JSON_FLOAT */

/* Search string value based on given key */
int json_get_val_str(jobj_t *jobj, char *key, char *value, int maxlen)
{
	jsontok_t *t;
	int ret = json_get_value(jobj, key, &t);
	if (ret != kNoErr)
		return ret;
	return json_str_to_str(jobj, t, value, maxlen);
}

int json_get_val_str_len(jobj_t *jobj, char *key, int *len)
{
	jsontok_t *t;
	int ret = json_get_value(jobj, key, &t);
	if (ret != kNoErr)
		return ret;
	if (!t || t->type != JSMN_STRING)
		return kFormatErr;
	*len = t->end - t->start;
	return kNoErr;
}
/* Search composite object based on given key */
int json_get_composite_object(jobj_t *jobj, char *key)
{
	jsontok_t *t;
	int ret = json_get_value(jobj, key, &t);
	if (ret != kNoErr)
		return ret;
	if (!t || t->type != JSMN_OBJECT)
		return kFormatErr;
	if (t->size % 2 != 0)
		return kFormatErr;
	/* Reduce the scope of subsequent searches to this object */
	jobj->cur = t;
	return kNoErr;
}

/* Release a composite object*/
int json_release_composite_object(jobj_t *jobj)
{
	if (jobj->cur->parent < 0)
		return kGeneralErr;
	/* Increase the scope of subsequent searches the parent element */
	jobj->cur = &jobj->tokens[jobj->cur->parent];
	return kNoErr;
}

/* Search array object based on given key */
int json_get_array_object(jobj_t *jobj, char *key, int *num_elements)
{
	jsontok_t *t;
	int ret = json_get_value(jobj, key, &t);
	if (ret != kNoErr)
		return ret;
	if (!t || t->type != JSMN_ARRAY)
		return kFormatErr;
	/* Reduce the scope of subsequent searches to this array */
	jobj->cur = t;
	/* Indicate the number of array elements found, if requested */
	if (num_elements)
		*num_elements = t->size;
	return kNoErr;
}
/* Release array object */
int json_release_array_object(jobj_t *jobj)
{
	return json_release_composite_object(jobj);
}

int json_array_get_num_elements(jobj_t *jobj)
{
	if (jobj->cur->type != JSMN_ARRAY)
		return kGeneralErr;
	return jobj->cur->size;

}
/* Fetch the JSON value from an array based on index.
 * val_t is appropriately populated if the element is found
 * and kNoErr is returned. Else error is returned.
 */
static int json_get_array_index(jobj_t *jobj, uint16_t index, jsontok_t **val_t)
{
	*val_t = NULL;
	if (jobj->cur->type != JSMN_ARRAY)
		return kFormatErr;
	/* Given index exceeds the size of array. */
	if (index >= jobj->cur->size)
		return kFormatErr;
	jsontok_t *t = jobj->cur;
	/* Incrementing once so that the token pointer points to index 0*/
	t++;
	while (index--) {
		/* For safety, check if the current token's end does not go
		 * beyond the parent object's end. This case is unlikely, yet,
		 * better to have a check.
		 */
		if (t->end > jobj->cur->end)
			return kGeneralErr;
		/* If the element is an array or object, skip to its last
		 * element.
		 */
		if (t->type == JSMN_ARRAY || t->type == JSMN_OBJECT)
			t = skip_to_last(t);
		t++;
	}
	*val_t = t;
	return kNoErr;
}

/* Search boolean value inside an array based on given index */
int json_array_get_bool(jobj_t *jobj, uint16_t index, bool *value)
{
	jsontok_t *t;
	int ret = json_get_array_index(jobj, index, &t);
	if (ret != kNoErr)
		return ret;
	return json_str_to_bool(jobj, t, value);
}
/* Search integer value inside an array based on given index */
int json_array_get_int(jobj_t *jobj, uint16_t index, int *value)
{
	jsontok_t *t;
	int ret = json_get_array_index(jobj, index, &t);
	if (ret != kNoErr)
		return ret;
	return json_str_to_int(jobj, t, value);
}
/* Search int64 value inside an array based on given index */
int json_array_get_int64(jobj_t *jobj, uint16_t index, int64_t *value)
{
	jsontok_t *t;
	int ret = json_get_array_index(jobj, index, &t);
	if (ret != kNoErr)
		return ret;
	return json_str_to_int64(jobj, t, value);
}
/* Search float value inside an array based on given index */
#ifdef CONFIG_JSON_FLOAT
int json_array_get_float(jobj_t *jobj, uint16_t index, float *value)
{
	jsontok_t *t;
	int ret = json_get_array_index(jobj, index, &t);
	if (ret != kNoErr)
		return ret;
	return json_str_to_float(jobj, t, value);
}
#else
int json_array_get_float(jobj_t *jobj, uint16_t index, float *value)
{
	return kGeneralErr;
}
#endif /* CONFIG_JSON_FLOAT */

/* Search string value inside an array based on given index */
int json_array_get_str(jobj_t *jobj, uint16_t index, char *value, int maxlen)
{
	jsontok_t *t;
	int ret = json_get_array_index(jobj, index, &t);
	if (ret != kNoErr)
		return ret;
	return json_str_to_str(jobj, t, value, maxlen);
}

int json_array_get_str_len(jobj_t *jobj, uint16_t index, int *len)
{
	jsontok_t *t;
	int ret = json_get_array_index(jobj, index, &t);
	if (ret != kNoErr)
		return ret;
	if (!t || t->type != JSMN_STRING)
		return kFormatErr;
	*len = t->end - t->start;
	return kNoErr;
}

/* Search composite object inside an array based on given index */
int json_array_get_composite_object(jobj_t *jobj, uint16_t index)
{
	jsontok_t *t;
	int ret = json_get_array_index(jobj, index, &t);
	if (ret != kNoErr)
		return ret;
	if (!t || t->type != JSMN_OBJECT)
		return kFormatErr;
	/* If number of children is not even, return error.
	 * Number of children should be even because the object consists
	 * of key:value pairs.
	 */
	if (t->size % 2 != 0)
		return kFormatErr;
	jobj->cur = t;
	return kNoErr;
}
/* Release the composite object inside the array */
int json_array_release_composite_object(jobj_t *jobj)
{
	return json_release_composite_object(jobj);
}

/* Search an array inside an array based on given index */
int json_array_get_array_object(jobj_t *jobj, uint16_t index,
		int *num_elements)
{
	jsontok_t *t;
	int ret = json_get_array_index(jobj, index, &t);
	if (ret != kNoErr)
		return ret;
	if (!t || t->type != JSMN_ARRAY)
		return kFormatErr;
	jobj->cur = t;
	*num_elements = t->size;
	return kNoErr;
}
/* Release the array */
int json_array_release_array_object(jobj_t *jobj)
{
	return json_release_composite_object(jobj);
}

/* Initialize the JSON parser */
static void json_obj_init(jobj_t *jobj, jsontok_t *tokens, int num_tokens)
{
	jobj->js = NULL;
	jobj->tokens = tokens;
	jobj->num_tokens = num_tokens;
	jobj->cur = NULL;
	jsmn_init(&jobj->parser);
}

bool json_is_object(jobj_t *jobj)
{
	if (jobj->cur->type == JSMN_OBJECT)
		return true;
	else
		return false;
}
bool json_is_array(jobj_t *jobj)
{
	if (jobj->cur->type == JSMN_ARRAY)
		return true;
	else
		return false;
}
/* Parse the given JSON string */
int json_init(jobj_t *jobj, jsontok_t *tokens, int num_tokens,
		char *js, int js_len)
{
	json_obj_init(jobj, tokens, num_tokens);
	int parsed_tokens = jsmn_parse(&jobj->parser, js, js_len,
			jobj->tokens, jobj->num_tokens);
	if (parsed_tokens < 0) {
		switch (parsed_tokens) {
		case JSMN_ERROR_NOMEM:
			return kNoMemoryErr;
		case JSMN_ERROR_INVAL:
			return kFormatErr;
		case JSMN_ERROR_PART:
			return kMalformedErr;
		default:
			return kGeneralErr;
		}
	}
	jobj->js = js;
	jobj->num_tokens = parsed_tokens;
	jobj->cur = jobj->tokens;
	/* Number of children for an object should always be even as
	 * a JSON object consists of key:value pairs.
	 * If the type of first token is an object, and it has even
	 * number of children, it is valid.
	 * Else, if the type of the first token is array, it is valid.
	 * Else, the JSON string is invalid.
	 */
	if ((jobj->tokens->type == JSMN_OBJECT) &&
			((jobj->tokens->size % 2) == 0))
		return kNoErr;
	else if (jobj->tokens->type == JSMN_ARRAY)
		return kNoErr;
	else
		return kFormatErr;
}
