#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <math.h>
#include <float.h>
#include <ctype.h>

#include "lite-cjson.h"

typedef struct
{
    const unsigned char *content;
    int length;
    int offset;
    int depth; /* How deeply nested (in arrays/objects) is the input at the current offset. */
} parse_buffer;

/* check if the given size is left to read in a given parse buffer (starting with 1) */
#define can_read(buffer, size) ((buffer != NULL) && (((buffer)->offset + size) <= (buffer)->length))
/* check if the buffer can be accessed at the given index (starting with 0) */
#define can_access_at_index(buffer, index) ((buffer != NULL) && (((buffer)->offset + index) < (buffer)->length))
#define cannot_access_at_index(buffer, index) (!can_access_at_index(buffer, index))
/* get a pointer to the buffer at the position */
#define buffer_at_offset(buffer) ((buffer)->content + (buffer)->offset)

/* Predeclare these prototypes. */
static int parse_value(lite_cjson_t* const item, parse_buffer * const input_buffer);
static int parse_string(lite_cjson_t * const item, parse_buffer * const input_buffer);
static int parse_array(_IN_ lite_cjson_t* const item, _IN_ parse_buffer * const input_buffer);
static int parse_object(lite_cjson_t * const item, parse_buffer * const input_buffer);

/* Utility to jump whitespace and cr/lf */
static parse_buffer *buffer_skip_whitespace(parse_buffer * const buffer)
{
    if ((buffer == NULL) || (buffer->content == NULL))
    {
        return NULL;
    }

    while (can_access_at_index(buffer, 0) && (buffer_at_offset(buffer)[0] <= 32))
    {
       buffer->offset++;
    }

    if (buffer->offset == buffer->length)
    {
        buffer->offset--;
    }

    return buffer;
}

/* skip the UTF-8 BOM (byte order mark) if it is at the beginning of a buffer */
static parse_buffer *skip_utf8_bom(parse_buffer * const buffer)
{
    if ((buffer == NULL) || (buffer->content == NULL) || (buffer->offset != 0))
    {
        return NULL;
    }

    if (can_access_at_index(buffer, 4) && (strncmp((const char*)buffer_at_offset(buffer), "\xEF\xBB\xBF", 3) == 0))
    {
        buffer->offset += 3;
    }

    return buffer;
}

/* Parse the input text to generate a number, and populate the result into item. */
static int parse_number(_IN_ lite_cjson_t* const item, _IN_ parse_buffer * const input_buffer)
{
    double number = 0;
	unsigned char *after_end = NULL;
    unsigned char number_c_string[64];
    unsigned char decimal_point = '.';
    int i = 0;

    if ((input_buffer == NULL) || (input_buffer->content == NULL))
    {
        return -1;
    }

    /* copy the number into a temporary buffer and replace '.' with the decimal point
     * of the current locale (for strtod)
     * This also takes care of '\0' not necessarily being available for marking the end of the input */
    for (i = 0; can_access_at_index(input_buffer, i); i++)
    {
        switch (buffer_at_offset(input_buffer)[i])
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '+':
            case '-':
            case 'e':
            case 'E':
				number_c_string[i] = buffer_at_offset(input_buffer)[i];
                break;
            case '.':
				number_c_string[i] = decimal_point;
                break;

            default:
                goto loop_end;
        }
    }
loop_end:
	number_c_string[i] = '\0';
	number = strtod((const char*)number_c_string, (char**)&after_end);
	if (number_c_string == after_end) {return -1;}
	
	item->type = cJSON_Number;
	item->value = (char *)buffer_at_offset(input_buffer);
	item->value_length = (int)(after_end - number_c_string);
	item->value_double = number;
	
	/* use saturation in case of overflow */
    if (number >= INT_MAX)
    {
        item->value_int = INT_MAX;
    }
    else if (number <= INT_MIN)
    {
        item->value_int = INT_MIN;
    }
    else
    {
        item->value_int = (int)number;
    }

    input_buffer->offset += (int)(after_end - number_c_string);
    return 0;
}

/* Build an array from input text. */
static int parse_array(_IN_ lite_cjson_t* const item, _IN_ parse_buffer * const input_buffer)
{
    lite_cjson_t current_item;
	int start_pos = input_buffer->offset;
	item->size = 0;
	
    if (input_buffer->depth >= LITE_CJSON_NESTING_LIMIT)
    {
        return -1; /* to deeply nested */
    }
    input_buffer->depth++;

    if (buffer_at_offset(input_buffer)[0] != '[')
    {
        /* not an array */
        goto fail;
    }

    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ']'))
    {
        /* empty array */
        goto success;
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(input_buffer, 0))
    {
        input_buffer->offset--;
        goto fail;
    }

    /* step back to character in front of the first element */
    input_buffer->offset--;
    /* loop through the comma separated array elements */
    do
    {
    	memset(&current_item,0,sizeof(lite_cjson_t));

        /* parse next value */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (parse_value(&current_item, input_buffer) != 0)
        {
            goto fail; /* failed to parse value */
        }
        buffer_skip_whitespace(input_buffer);
		
		//printf("current item index: %d, type: %d, length: %d, value: %.*s\n",
		//	item->size+1, current_item.type, current_item.value_length, current_item.value_length, current_item.value);
		
		item->size++;
    }
    while (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ','));

    if (cannot_access_at_index(input_buffer, 0) || buffer_at_offset(input_buffer)[0] != ']')
    {
        goto fail; /* expected end of array */
    }

success:
    input_buffer->depth--;

    item->type = cJSON_Array;
	item->value = (char *)(input_buffer->content + start_pos);
	item->value_length = input_buffer->offset - start_pos + 1;

    input_buffer->offset++;

    return 0;

fail:
	
    return -1;
}

/* Build an object from the text. */
static int parse_object(_IN_ lite_cjson_t * const item, _IN_ parse_buffer * const input_buffer)
{
    lite_cjson_t current_item_key;
	lite_cjson_t current_item_value;
	int start_pos = input_buffer->offset;
	item->size = 0;
	
    if (input_buffer->depth >= LITE_CJSON_NESTING_LIMIT)
    {
        return -1; /* to deeply nested */
    }
    input_buffer->depth++;

    if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != '{'))
    {
        goto fail; /* not an object */
    }

    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '}'))
    {
        goto success; /* empty object */
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(input_buffer, 0))
    {
        input_buffer->offset--;
        goto fail;
    }

    /* step back to character in front of the first element */
    input_buffer->offset--;
    /* loop through the comma separated array elements */
    do
    {
    	memset(&current_item_key,0,sizeof(lite_cjson_t));
		memset(&current_item_value,0,sizeof(lite_cjson_t));
		
        /* parse the name of the child */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (parse_string(&current_item_key, input_buffer) != 0)
        {
            goto fail; /* faile to parse name */
        }
        buffer_skip_whitespace(input_buffer);

        if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != ':'))
        {
            goto fail; /* invalid object */
        }

        /* parse the value */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (parse_value(&current_item_value, input_buffer) != 0)
        {
            goto fail; /* failed to parse value */
        }
        buffer_skip_whitespace(input_buffer);

		//printf("Current Object [Index: %d], [Key Length: %d, Key Value: %.*s], [Value Length: %d, Value: %.*s]\n",
		//	item->size+1,current_item_key.value_length,current_item_key.value_length,current_item_key.value,
		//	current_item_value.value_length,current_item_value.value_length,current_item_value.value);

		item->size++;
    }
    while (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ','));

    if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != '}'))
    {
        goto fail; /* expected end of object */
    }

success:
    input_buffer->depth--;

    item->type = cJSON_Object;
    item->value = (char *)(input_buffer->content + start_pos);
	item->value_length = input_buffer->offset - start_pos + 1;

    input_buffer->offset++;
	
    return 0;

fail:

    return -1;
}

/* Parse the input text into an unescaped cinput, and populate item. */
static int parse_string(_IN_ lite_cjson_t * const item, _IN_ parse_buffer * const input_buffer)
{
    const unsigned char *input_pointer = buffer_at_offset(input_buffer) + 1;
    const unsigned char *input_end = buffer_at_offset(input_buffer) + 1;
    //unsigned char *output_pointer = NULL;
    //unsigned char *output = NULL;

    /* not a string */
    if (buffer_at_offset(input_buffer)[0] != '\"')
    {
    	//printf("not a string");
        goto fail;
    }

    {
        /* calculate approximate size of the output (overestimate) */
        //int allocation_length = 0;
        int skipped_bytes = 0;
        while (((int)(input_end - input_buffer->content) < input_buffer->length) && (*input_end != '\"'))
        {
            /* is escape sequence */
            if (input_end[0] == '\\')
            {
                if ((int)(input_end + 1 - input_buffer->content) >= input_buffer->length)
                {
                    /* prevent buffer overflow when last input character is a backslash */
                    goto fail;
                }
                skipped_bytes++;
                input_end++;
            }
            input_end++;
        }
        if (((int)(input_end - input_buffer->content) >= input_buffer->length) || (*input_end != '\"'))
        {
        	//printf("end error\n");
            goto fail; /* string ended unexpectedly */
        }
		#if 0
        /* This is at most how much we need for the output */
        allocation_length = (int) (input_end - buffer_at_offset(input_buffer)) - skipped_bytes;
        output = (unsigned char*)input_buffer->hooks.allocate(allocation_length + sizeof(""));
        if (output == NULL)
        {
            goto fail; /* allocation failure */
        }
		#endif
    }

	#if 0
    output_pointer = output;
    /* loop through the string literal */
    while (input_pointer < input_end)
    {
        if (*input_pointer != '\\')
        {
            *output_pointer++ = *input_pointer++;
        }
        /* escape sequence */
        else
        {
            unsigned char sequence_length = 2;
            if ((input_end - input_pointer) < 1)
            {
                goto fail;
            }

            switch (input_pointer[1])
            {
                case 'b':
                    *output_pointer++ = '\b';
                    break;
                case 'f':
                    *output_pointer++ = '\f';
                    break;
                case 'n':
                    *output_pointer++ = '\n';
                    break;
                case 'r':
                    *output_pointer++ = '\r';
                    break;
                case 't':
                    *output_pointer++ = '\t';
                    break;
                case '\"':
                case '\\':
                case '/':
                    *output_pointer++ = input_pointer[1];
                    break;

                /* UTF-16 literal */
                case 'u':
                    sequence_length = utf16_literal_to_utf8(input_pointer, input_end, &output_pointer);
                    if (sequence_length == 0)
                    {
                        /* failed to convert UTF16-literal to UTF-8 */
                        goto fail;
                    }
                    break;

                default:
                    goto fail;
            }
            input_pointer += sequence_length;
        }
    }

    /* zero terminate the output */
    *output_pointer = '\0';
	#endif
	
    item->type = cJSON_String;
    item->value = (char*)input_pointer;
	item->value_length = input_end - input_pointer;
	
    input_buffer->offset = (int) (input_end - input_buffer->content);
    input_buffer->offset++;

    return 0;

fail:
	#if 0
    if (output != NULL)
    {
        input_buffer->hooks.deallocate(output);
    }
	#endif
    if (input_pointer != NULL)
    {
        input_buffer->offset = (int)(input_pointer - input_buffer->content);
    }

    return -1;
}

/* Parser core - when encountering text, process appropriately. */
static int parse_value(_IN_ lite_cjson_t * const lite, _IN_ parse_buffer * const input_buffer)
{
    if ((input_buffer == NULL) || (input_buffer->content == NULL))
    {
        return -1; /* no input */
    }

    /* parse the different types of values */
    /* null */
    if (can_read(input_buffer, 4) && (strncmp((const char*)buffer_at_offset(input_buffer), "null", 4) == 0))
    {
        lite->type = cJSON_NULL;
		lite->value = (char *)buffer_at_offset(input_buffer);
		lite->value_length = 4;
		
        input_buffer->offset += 4;
        return 0;
    }
    /* false */
    if (can_read(input_buffer, 5) && (strncmp((const char*)buffer_at_offset(input_buffer), "false", 5) == 0))
    {
        lite->type = cJSON_False;
		lite->value = (char *)buffer_at_offset(input_buffer);
		lite->value_length = 5;
		
        input_buffer->offset += 5;
        return 0;
    }
    /* true */
    if (can_read(input_buffer, 4) && (strncmp((const char*)buffer_at_offset(input_buffer), "true", 4) == 0))
    {
        lite->type = cJSON_True;
        lite->value = (char *)buffer_at_offset(input_buffer);
		lite->value_length = 4;
		
        input_buffer->offset += 4;
        return 0;
    }
    /* string */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '\"'))
    {
    	//printf("parse string\n");
        return parse_string(lite, input_buffer);
    }
	
    /* number */
    if (can_access_at_index(input_buffer, 0) && ((buffer_at_offset(input_buffer)[0] == '-') || ((buffer_at_offset(input_buffer)[0] >= '0') && (buffer_at_offset(input_buffer)[0] <= '9'))))
    {
    	//printf("parse number\n");
        return parse_number(lite, input_buffer);
    }

    /* array */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '['))
    {
    	//printf("parse array\n");
        return parse_array(lite, input_buffer);
    }
	
    /* object */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '{'))
    {
        return parse_object(lite, input_buffer);
    }
	
    return -1;
}

int lite_cjson_parse(_IN_ const char *src, _IN_ int src_len, _OU_ lite_cjson_t *lite)
{
	if (!lite || !src || !lite) {return -1;}

	parse_buffer buffer;

	memset(&buffer,0,sizeof(parse_buffer));
	buffer.content = (const unsigned char*)src;
	buffer.length = src_len;
	buffer.offset = 0;

	if (parse_value(lite, buffer_skip_whitespace(skip_utf8_bom(&buffer))) != 0)
	{
		lite->type = cJSON_Invalid;
		lite->value = NULL;
		lite->value_length = 0;
		return -1;
	}
	
	return 0;
}

int lite_cjson_is_false(_IN_ lite_cjson_t *lite)
{
	if (!lite) {return -1;}

	return (lite->type & 0xFF) == cJSON_False;
}

int lite_cjson_is_true(_IN_ lite_cjson_t *lite)
{
	if (!lite) {return -1;}

	return (lite->type & 0xFF) == cJSON_True;
}

int lite_cjson_is_null(_IN_ lite_cjson_t *lite)
{
	if (!lite) {return -1;}

	return (lite->type & 0xFF) == cJSON_NULL;
}

int lite_cjson_is_number(_IN_ lite_cjson_t *lite)
{
	if (!lite) {return -1;}

	return (lite->type & 0xFF) == cJSON_Number;
}

int lite_cjson_is_string(_IN_ lite_cjson_t *lite)
{
	if (!lite) {return -1;}

	return (lite->type & 0xFF) == cJSON_String;
}

int lite_cjson_is_array(_IN_ lite_cjson_t *lite)
{
	if (!lite) {return -1;}

	return (lite->type & 0xFF) == cJSON_Array;
}

int lite_cjson_is_object(_IN_ lite_cjson_t *lite)
{
	if (!lite) {return -1;}

	return (lite->type & 0xFF) == cJSON_Object;
}

int lite_cjson_array_item(_IN_ lite_cjson_t *lite, _IN_ int index, _OU_ lite_cjson_t *lite_item)
{
	if(!lite || lite->type != cJSON_Array || !lite->value || 
		index <0 || index >= lite->size || !lite_item) 
		return -1;

	parse_buffer buffer;
	parse_buffer *p_buffer = &buffer;

	memset(&buffer,0,sizeof(parse_buffer));
	buffer.content = (const unsigned char*)lite->value;
	buffer.length = lite->value_length;
	buffer.offset = 0;
	
	lite_cjson_t current_item;
	//int start_pos = p_buffer->offset;
	int iter_index = 0;
	
    if (buffer_at_offset(p_buffer)[0] != '[')
    {
        /* not an array */
        return -1;
    }

    p_buffer->offset++;
    buffer_skip_whitespace(p_buffer);
    if (can_access_at_index(p_buffer, 0) && (buffer_at_offset(p_buffer)[0] == ']'))
    {
        /* empty array */
        return -1;
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(p_buffer, 0))
    {
        p_buffer->offset--;
        return -1;
    }

    /* step back to character in front of the first element */
    p_buffer->offset--;
    /* loop through the comma separated array elements */
    do
    {
    	memset(&current_item,0,sizeof(lite_cjson_t));

        /* parse next value */
        p_buffer->offset++;
        buffer_skip_whitespace(p_buffer);
        if (parse_value(&current_item, p_buffer) != 0)
        {
            return -1; /* failed to parse value */
        }
        buffer_skip_whitespace(p_buffer);
		
		//printf("current item index: %d, type: %d, length: %d, value: %.*s\n",
		//	iter_index+1, current_item.type, current_item.value_length, current_item.value_length, current_item.value);

		if (iter_index == index) {
			memcpy(lite_item, &current_item, sizeof(lite_cjson_t));
			return 0;
		}

		iter_index++;
    }
    while (can_access_at_index(p_buffer, 0) && (buffer_at_offset(p_buffer)[0] == ','));
	
    return -1;
}

static int _lite_cjson_object_item(_IN_ lite_cjson_t *lite, _IN_ const char *key, _IN_ int key_len, _OU_ lite_cjson_t *lite_item)
{
	if(!lite || lite->type != cJSON_Object|| !lite->value || lite->size == 0 || !key || key_len <= 0 || !lite_item) {return -1;};

	parse_buffer buffer;
	parse_buffer *p_buffer = &buffer;

	memset(&buffer,0,sizeof(parse_buffer));
	buffer.content = (const unsigned char*)lite->value;
	buffer.length = lite->value_length;
	buffer.offset = 0;
	
	lite_cjson_t current_item_key;
	lite_cjson_t current_item_value;
	//int start_pos = p_buffer->offset;
	int index = 0;

    if (cannot_access_at_index(p_buffer, 0) || (buffer_at_offset(p_buffer)[0] != '{'))
    {
        return -1; /* not an object */
    }

    p_buffer->offset++;
    buffer_skip_whitespace(p_buffer);
    if (can_access_at_index(p_buffer, 0) && (buffer_at_offset(p_buffer)[0] == '}'))
    {
        return -1; /* empty object */
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(p_buffer, 0))
    {
        p_buffer->offset--;
        return -1;
    }

    /* step back to character in front of the first element */
    p_buffer->offset--;
    /* loop through the comma separated array elements */
    do
    {
    	memset(&current_item_key,0,sizeof(lite_cjson_t));
		memset(&current_item_value,0,sizeof(lite_cjson_t));
		
        /* parse the name of the child */
        p_buffer->offset++;
        buffer_skip_whitespace(p_buffer);
        if (parse_string(&current_item_key, p_buffer) != 0)
        {
            return -1; /* faile to parse name */
        }
        buffer_skip_whitespace(p_buffer);

        if (cannot_access_at_index(p_buffer, 0) || (buffer_at_offset(p_buffer)[0] != ':'))
        {
            return -1; /* invalid object */
        }

        /* parse the value */
        p_buffer->offset++;
        buffer_skip_whitespace(p_buffer);
        if (parse_value(&current_item_value, p_buffer) != 0)
        {
            return -1; /* failed to parse value */
        }
        buffer_skip_whitespace(p_buffer);

		//printf("Current Object [Index: %d], [Key Length: %d, Key Value: %.*s], [Value Length: %d, Value: %.*s]\n",
		//	index + 1, current_item_key.value_length,current_item_key.value_length,current_item_key.value,
		//	current_item_value.value_length,current_item_value.value_length,current_item_value.value);
		index++;

		//printf("key: %s, ken_len: %d\n",key,key_len);
		if ((current_item_key.value_length == key_len) &&
			memcmp(current_item_key.value, key, key_len) == 0)
		{
			memcpy(lite_item,&current_item_value,sizeof(lite_cjson_t));
			return 0;
		}
    }
    while (can_access_at_index(p_buffer, 0) && (buffer_at_offset(p_buffer)[0] == ','));

	return -1;
}

static int _lite_cjson_key_array_index(_IN_ const char *key, _IN_ int key_len, 
			_OU_ int *partial_key_len, _OU_ int *array_key_len, _OU_ int *array_index)
{
	if (!key || !partial_key_len || !array_key_len || !array_index) {return -1;}

	char *bracket_pre = NULL;
	char *bracket_suf = NULL;
	int index = 0;
	int deep = 0;
	char array_index_str[10] = {0};
	
	for (index = 0; index < key_len; index++) {
		switch(key[index]) {
			case '[':
				{
					if (deep != 0) {return -1;}
					deep++;
					if (!bracket_pre) {bracket_pre = (char *)&key[index];}
				}
				break;
			case ']':
				{
					if (deep != 1) {return -1;}
					deep--;
					if (key[index  -1] == '[') {return -1;}
					if (!bracket_suf) {bracket_suf = (char *)&key[index];}
				}
				break;
			default:
				break;
			
		}
	}

	if (bracket_pre && bracket_suf && ((bracket_suf - key + 1) == key_len)) {
		*partial_key_len = bracket_pre - key;
		*array_key_len = bracket_suf - key + 1;

		//Get Index
		memcpy(array_index_str,bracket_pre + 1,bracket_suf - bracket_pre - 1);
		*array_index = atoi(array_index_str);
		return 0;
	}

	return -1;
}

int lite_cjson_object_item(_IN_ lite_cjson_t *lite, _IN_ const char *key, _IN_ int key_len, _OU_ lite_cjson_t *lite_item)
{
	if(!lite || lite->type != cJSON_Object || !lite->value || lite->size == 0 || !key || key_len <= 0 || !lite_item) {return -1;};

	int res = 0;
	char *delim = NULL;
	lite_cjson_t lite_prev;
	lite_cjson_t lite_next;
	lite_cjson_t lite_iter;
	
	char *key_iter = (char *)key;
	int key_iter_len = 0;
	int partial_key_len = 0;
	int array_key_len = 0;
	int array_index = 0;

	memcpy(&lite_iter,lite,sizeof(lite_cjson_t));
	memset(&lite_prev,0,sizeof(lite_cjson_t));
	
	do {
		if ((delim = strchr(key_iter,'.')) != NULL) {
			//printf("delim exist,delim : %s\n",delim);
			memset(&lite_next,0,sizeof(lite_cjson_t));
			partial_key_len = array_key_len = array_index = 0;
			key_iter_len = (int)(delim - key_iter);
		} else {			
			key_iter_len = key_len - (key_iter - key);
			//printf("key: %s, last key: %s, key len: %d, last key len: %d\n",key, key_iter, key_len, key_iter_len);
		}
		
		if (_lite_cjson_key_array_index(key_iter, key_iter_len, 
				&partial_key_len, &array_key_len, &array_index) == 0) {

			//printf("partial_key_len: %d, array_key_len: %d, array_index: %d\n", partial_key_len, array_key_len, array_index);
			
			res = _lite_cjson_object_item(&lite_iter,key_iter,partial_key_len,&lite_prev);
			if (res || lite_prev.type != cJSON_Array) {return -1;}
			//printf("current array: %.*s\n",lite_prev.value_length,lite_prev.value);
			
			res = lite_cjson_array_item(&lite_prev,array_index,&lite_next);
			if (res) {return -1;}
			//printf("current array item: %.*s\n",lite_next.value_length,lite_next.value);

			memcpy(&lite_iter,&lite_next,sizeof(lite_cjson_t));
			key_iter += array_key_len + 1;
			//printf("key_iter: %s\n",key_iter);
		} else {
			res = _lite_cjson_object_item(&lite_iter,key_iter,key_iter_len,&lite_prev);
			if (res) {return -1;}
			//printf("current object: %.*s\n",lite_prev.value_length,lite_prev.value);

			memcpy(&lite_iter,&lite_prev,sizeof(lite_cjson_t));
			key_iter = delim + 1;
		}
	} while(delim);

	//printf("final lite cjson value: %.*s\n",lite_iter.value_length,lite_iter.value);
	memcpy(lite_item,&lite_iter,sizeof(lite_cjson_t));
	
	return 0;
}

int lite_cjson_object_item_by_index(_IN_ lite_cjson_t *lite,_IN_ int index,_OU_ lite_cjson_t *lite_item_key,_OU_ lite_cjson_t *lite_item_value)
{
	if(!lite || lite->type != cJSON_Object|| !lite->value || lite->size == 0 || index < 0 || index >= lite->size) {return -1;};

	parse_buffer buffer;
	parse_buffer *p_buffer = &buffer;

	memset(&buffer,0,sizeof(parse_buffer));
	buffer.content = (const unsigned char*)lite->value;
	buffer.length = lite->value_length;
	buffer.offset = 0;
	
	lite_cjson_t current_item_key;
	lite_cjson_t current_item_value;
	//int start_pos = p_buffer->offset;
	int item_index = 0;

    if (cannot_access_at_index(p_buffer, 0) || (buffer_at_offset(p_buffer)[0] != '{'))
    {
        return -1; /* not an object */
    }

    p_buffer->offset++;
    buffer_skip_whitespace(p_buffer);
    if (can_access_at_index(p_buffer, 0) && (buffer_at_offset(p_buffer)[0] == '}'))
    {
        return -1; /* empty object */
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(p_buffer, 0))
    {
        p_buffer->offset--;
        return -1;
    }

    /* step back to character in front of the first element */
    p_buffer->offset--;
    /* loop through the comma separated array elements */
    do
    {
    	memset(&current_item_key,0,sizeof(lite_cjson_t));
		memset(&current_item_value,0,sizeof(lite_cjson_t));
		
        /* parse the name of the child */
        p_buffer->offset++;
        buffer_skip_whitespace(p_buffer);
        if (parse_string(&current_item_key, p_buffer) != 0)
        {
            return -1; /* faile to parse name */
        }
        buffer_skip_whitespace(p_buffer);

        if (cannot_access_at_index(p_buffer, 0) || (buffer_at_offset(p_buffer)[0] != ':'))
        {
            return -1; /* invalid object */
        }

        /* parse the value */
        p_buffer->offset++;
        buffer_skip_whitespace(p_buffer);
        if (parse_value(&current_item_value, p_buffer) != 0)
        {
            return -1; /* failed to parse value */
        }
        buffer_skip_whitespace(p_buffer);

		//printf("Current Object [Index: %d], [Key Length: %d, Key Value: %.*s], [Value Length: %d, Value: %.*s]\n",
		//	index + 1, current_item_key.value_length,current_item_key.value_length,current_item_key.value,
		//	current_item_value.value_length,current_item_value.value_length,current_item_value.value);

		//printf("index:%d, key: %.*s, value: %.*s\n",index, 
		//	current_item_key.value_length,current_item_key.value,
		//	current_item_value.value_length,current_item_value.value);
		
		if (item_index == index) {
			if (lite_item_key) {memcpy(lite_item_key,&current_item_key,sizeof(lite_cjson_t));}
			if (lite_item_value) {memcpy(lite_item_value,&current_item_value,sizeof(lite_cjson_t));}
			return 0;
		}

		item_index++;
    }
    while (can_access_at_index(p_buffer, 0) && (buffer_at_offset(p_buffer)[0] == ','));

	return -1;
}
/*** cjson create, add and print ***/

static char* lite_cjson_strdup(const char* str)
{
      size_t len;
      char* copy;

      len = strlen(str) + 1;
      if (!(copy = (char*)malloc(len))) return 0;
      memcpy(copy,str,len);
      return copy;
}

/* Internal constructor. */
static lite_cjson_item_t *lite_cjson_new_item(void)
{
	lite_cjson_item_t* node = (lite_cjson_item_t*)malloc(sizeof(lite_cjson_item_t));
	if (node) memset(node,0,sizeof(lite_cjson_item_t));
	return node;
}


/* Delete a lite_cjson_item_t structure. */
void lite_cjson_delete(lite_cjson_item_t *c)
{
	lite_cjson_item_t *next;
	while (c)
	{
		next=c->next;
		if (!(c->type&cJSON_IsReference) && c->child) lite_cjson_delete(c->child);
		if (!(c->type&cJSON_IsReference) && c->valuestring) free(c->valuestring);
		if (!(c->type&cJSON_StringIsConst) && c->string) free(c->string);
		free(c);
		c=next;
	}
}

static int pow2gt (int x)	{	--x;	x|=x>>1;	x|=x>>2;	x|=x>>4;	x|=x>>8;	x|=x>>16;	return x+1;	}

typedef struct {char *buffer; int length; int offset; } printbuffer;

static char* ensure(printbuffer *p,int needed)
{
	char *newbuffer;int newsize;
	if (!p || !p->buffer) return 0;
	needed+=p->offset;
	if (needed<=p->length) return p->buffer+p->offset;

	newsize=pow2gt(needed);
	newbuffer=(char*)malloc(newsize);
	if (!newbuffer) {free(p->buffer);p->length=0,p->buffer=0;return 0;}
	if (newbuffer) memcpy(newbuffer,p->buffer,p->length);
	free(p->buffer);
	p->length=newsize;
	p->buffer=newbuffer;
	return newbuffer+p->offset;
}

static int update(printbuffer *p)
{
	char *str;
	if (!p || !p->buffer) return 0;
	str=p->buffer+p->offset;
	return p->offset+strlen(str);
}

/* Render the number nicely from the given item into a string. */
static char *print_number(lite_cjson_item_t *item,printbuffer *p)
{
	char *str=0;
	double d=item->valuedouble;
	if (d==0)
	{
		if (p)	str=ensure(p,2);
		else	str=(char*)malloc(2);	/* special case for 0. */
		if (str) strcpy(str,"0");
	}
	else if (fabs(((double)item->valueint)-d)<=DBL_EPSILON && d<=INT_MAX && d>=INT_MIN)
	{
		if (p)	str=ensure(p,21);
		else	str=(char*)malloc(21);	/* 2^64+1 can be represented in 21 chars. */
		if (str)	sprintf(str,"%d",item->valueint);
	}
	else
	{
		if (p)	str=ensure(p,64);
		else	str=(char*)malloc(64);	/* This is a nice tradeoff. */
		if (str)
		{
            if (/*fabs(floor(d)-d)<=DBL_EPSILON &&*/ fabs(d)<1.0e60)sprintf(str,"%.0f",d);
			else if (fabs(d)<1.0e-6 || fabs(d)>1.0e9)			sprintf(str,"%e",d);
			else												sprintf(str,"%f",d);
		}
	}
	return str;
}


/* Render the cstring provided to an escaped version that can be printed. */
static char *print_string_ptr(const char *str,printbuffer *p)
{
	const char *ptr;char *ptr2,*out;int len=0,flag=0;unsigned char token;
	
	for (ptr=str;*ptr;ptr++) flag|=((*ptr>0 && *ptr<32)||(*ptr=='\"')||(*ptr=='\\'))?1:0;
	if (!flag)
	{
		len=ptr-str;
		if (p) out=ensure(p,len+3);
		else		out=(char*)malloc(len+3);
		if (!out) return 0;
		ptr2=out;*ptr2++='\"';
		strcpy(ptr2,str);
		ptr2[len]='\"';
		ptr2[len+1]=0;
		return out;
	}
	
	if (!str)
	{
		if (p)	out=ensure(p,3);
		else	out=(char*)malloc(3);
		if (!out) return 0;
		strcpy(out,"\"\"");
		return out;
	}
	ptr=str;while ((token=*ptr) && ++len) {if (strchr("\"\\\b\f\n\r\t",token)) len++; else if (token<32) len+=5;ptr++;}
	
	if (p)	out=ensure(p,len+3);
	else	out=(char*)malloc(len+3);
	if (!out) return 0;

	ptr2=out;ptr=str;
	*ptr2++='\"';
	while (*ptr)
	{
		if ((unsigned char)*ptr>31 && *ptr!='\"' && *ptr!='\\') *ptr2++=*ptr++;
		else
		{
			*ptr2++='\\';
			switch (token=*ptr++)
			{
				case '\\':	*ptr2++='\\';	break;
				case '\"':	*ptr2++='\"';	break;
				case '\b':	*ptr2++='b';	break;
				case '\f':	*ptr2++='f';	break;
				case '\n':	*ptr2++='n';	break;
				case '\r':	*ptr2++='r';	break;
				case '\t':	*ptr2++='t';	break;
				default: sprintf(ptr2,"u%04x",token);ptr2+=5;	break;	/* escape and print */
			}
		}
	}
	*ptr2++='\"';*ptr2++=0;
	return out;
}
/* Invote print_string_ptr (which is useful) on an item. */
static char *print_string(lite_cjson_item_t *item,printbuffer *p)	{return print_string_ptr(item->valuestring,p);}


static char *print_value(lite_cjson_item_t *item,int depth,int fmt,printbuffer *p);
static char *print_array(lite_cjson_item_t *item,int depth,int fmt,printbuffer *p);
static char *print_object(lite_cjson_item_t *item,int depth,int fmt,printbuffer *p);


/* Render a lite_cjson_item_t item/entity/structure to text. */
char *lite_cjson_print(lite_cjson_item_t *item)				{return print_value(item,0,1,0);}
char *lite_cjson_print_unformatted(lite_cjson_item_t *item)	{return print_value(item,0,0,0);}

char *lite_cjosnc_print_buffered(lite_cjson_item_t *item,int prebuffer,int fmt)
{
	printbuffer p;
	p.buffer=(char*)malloc(prebuffer);
	p.length=prebuffer;
	p.offset=0;
	return print_value(item,0,fmt,&p);
	return p.buffer;
}


/* Render a value to text. */
static char *print_value(lite_cjson_item_t *item,int depth,int fmt,printbuffer *p)
{
	char *out=0;
	if (!item) return 0;
	if (p)
	{
		switch ((item->type)&255)
		{
			case cJSON_NULL:	{out=ensure(p,5);	if (out) strcpy(out,"null");	break;}
			case cJSON_False:	{out=ensure(p,6);	if (out) strcpy(out,"false");	break;}
			case cJSON_True:	{out=ensure(p,5);	if (out) strcpy(out,"true");	break;}
			case cJSON_Number:	out=print_number(item,p);break;
			case cJSON_String:	out=print_string(item,p);break;
			case cJSON_Array:	out=print_array(item,depth,fmt,p);break;
			case cJSON_Object:	out=print_object(item,depth,fmt,p);break;
		}
	}
	else
	{
		switch ((item->type)&255)
		{
			case cJSON_NULL:	out=lite_cjson_strdup("null");	break;
			case cJSON_False:	out=lite_cjson_strdup("false");break;
			case cJSON_True:	out=lite_cjson_strdup("true"); break;
			case cJSON_Number:	out=print_number(item,0);break;
			case cJSON_String:	out=print_string(item,0);break;
			case cJSON_Array:	out=print_array(item,depth,fmt,0);break;
			case cJSON_Object:	out=print_object(item,depth,fmt,0);break;
		}
	}
	return out;
}


/* Render an array to text */
static char *print_array(lite_cjson_item_t *item,int depth,int fmt,printbuffer *p)
{
	char **entries;
	char *out=0,*ptr,*ret;int len=5;
	lite_cjson_item_t *child=item->child;
	int numentries=0,i=0,fail=0;
	size_t tmplen=0;
	
	/* How many entries in the array? */
	while (child) numentries++,child=child->next;
	/* Explicitly handle numentries==0 */
	if (!numentries)
	{
		if (p)	out=ensure(p,3);
		else	out=(char*)malloc(3);
		if (out) strcpy(out,"[]");
		return out;
	}

	if (p)
	{
		/* Compose the output array. */
		i=p->offset;
		ptr=ensure(p,1);if (!ptr) return 0;	*ptr='[';	p->offset++;
		child=item->child;
		while (child && !fail)
		{
			print_value(child,depth+1,fmt,p);
			p->offset=update(p);
			if (child->next) {len=fmt?2:1;ptr=ensure(p,len+1);if (!ptr) return 0;*ptr++=',';if(fmt)*ptr++=' ';*ptr=0;p->offset+=len;}
			child=child->next;
		}
		ptr=ensure(p,2);if (!ptr) return 0;	*ptr++=']';*ptr=0;
		out=(p->buffer)+i;
	}
	else
	{
		/* Allocate an array to hold the values for each */
		entries=(char**)malloc(numentries*sizeof(char*));
		if (!entries) return 0;
		memset(entries,0,numentries*sizeof(char*));
		/* Retrieve all the results: */
		child=item->child;
		while (child && !fail)
		{
			ret=print_value(child,depth+1,fmt,0);
			entries[i++]=ret;
			if (ret) len+=strlen(ret)+2+(fmt?1:0); else fail=1;
			child=child->next;
		}
		
		/* If we didn't fail, try to malloc the output string */
		if (!fail)	out=(char*)malloc(len);
		/* If that fails, we fail. */
		if (!out) fail=1;

		/* Handle failure. */
		if (fail)
		{
			for (i=0;i<numentries;i++) if (entries[i]) free(entries[i]);
			free(entries);
			return 0;
		}
		
		/* Compose the output array. */
		*out='[';
		ptr=out+1;*ptr=0;
		for (i=0;i<numentries;i++)
		{
			tmplen=strlen(entries[i]);memcpy(ptr,entries[i],tmplen);ptr+=tmplen;
			if (i!=numentries-1) {*ptr++=',';if(fmt)*ptr++=' ';*ptr=0;}
			free(entries[i]);
		}
		free(entries);
		*ptr++=']';*ptr++=0;
	}
	return out;	
}


/* Render an object to text. */
static char *print_object(lite_cjson_item_t *item,int depth,int fmt,printbuffer *p)
{
	char **entries=0,**names=0;
	char *out=0,*ptr,*ret,*str;int len=7,i=0,j;
	lite_cjson_item_t *child=item->child;
	int numentries=0,fail=0;
	size_t tmplen=0;
	/* Count the number of entries. */
	while (child) numentries++,child=child->next;
	/* Explicitly handle empty object case */
	if (!numentries)
	{
		if (p) out=ensure(p,fmt?depth+4:3);
		else	out=(char*)malloc(fmt?depth+4:3);
		if (!out)	return 0;
		ptr=out;*ptr++='{';
		if (fmt) {*ptr++='\n';for (i=0;i<depth-1;i++) *ptr++='\t';}
		*ptr++='}';*ptr++=0;
		return out;
	}
	if (p)
	{
		/* Compose the output: */
		i=p->offset;
		len=fmt?2:1;	ptr=ensure(p,len+1);	if (!ptr) return 0;
		*ptr++='{';	if (fmt) *ptr++='\n';	*ptr=0;	p->offset+=len;
		child=item->child;depth++;
		while (child)
		{
			if (fmt)
			{
				ptr=ensure(p,depth);	if (!ptr) return 0;
				for (j=0;j<depth;j++) *ptr++='\t';
				p->offset+=depth;
			}
			print_string_ptr(child->string,p);
			p->offset=update(p);
			
			len=fmt?2:1;
			ptr=ensure(p,len);	if (!ptr) return 0;
			*ptr++=':';if (fmt) *ptr++='\t';
			p->offset+=len;
			
			print_value(child,depth,fmt,p);
			p->offset=update(p);

			len=(fmt?1:0)+(child->next?1:0);
			ptr=ensure(p,len+1); if (!ptr) return 0;
			if (child->next) *ptr++=',';
			if (fmt) *ptr++='\n';*ptr=0;
			p->offset+=len;
			child=child->next;
		}
		ptr=ensure(p,fmt?(depth+1):2);	 if (!ptr) return 0;
		if (fmt)	for (i=0;i<depth-1;i++) *ptr++='\t';
		*ptr++='}';*ptr=0;
		out=(p->buffer)+i;
	}
	else
	{
		/* Allocate space for the names and the objects */
		entries=(char**)malloc(numentries*sizeof(char*));
		if (!entries) return 0;
		names=(char**)malloc(numentries*sizeof(char*));
		if (!names) {free(entries);return 0;}
		memset(entries,0,sizeof(char*)*numentries);
		memset(names,0,sizeof(char*)*numentries);

		/* Collect all the results into our arrays: */
		child=item->child;depth++;if (fmt) len+=depth;
		while (child)
		{
			names[i]=str=print_string_ptr(child->string,0);
			entries[i++]=ret=print_value(child,depth,fmt,0);
			if (str && ret) len+=strlen(ret)+strlen(str)+2+(fmt?2+depth:0); else fail=1;
			child=child->next;
		}
		
		/* Try to allocate the output string */
		if (!fail)	out=(char*)malloc(len);
		if (!out) fail=1;

		/* Handle failure */
		if (fail)
		{
			for (i=0;i<numentries;i++) {if (names[i]) free(names[i]);if (entries[i]) free(entries[i]);}
			free(names);free(entries);
			return 0;
		}
		
		/* Compose the output: */
		*out='{';ptr=out+1;if (fmt)*ptr++='\n';*ptr=0;
		for (i=0;i<numentries;i++)
		{
			if (fmt) for (j=0;j<depth;j++) *ptr++='\t';
			tmplen=strlen(names[i]);memcpy(ptr,names[i],tmplen);ptr+=tmplen;
			*ptr++=':';if (fmt) *ptr++='\t';
			strcpy(ptr,entries[i]);ptr+=strlen(entries[i]);
			if (i!=numentries-1) *ptr++=',';
			if (fmt) *ptr++='\n';*ptr=0;
			free(names[i]);free(entries[i]);
		}
		
		free(names);free(entries);
		if (fmt) for (i=0;i<depth-1;i++) *ptr++='\t';
		*ptr++='}';*ptr++=0;
	}
	return out;	
}

/* Utility for array list handling. */
static void suffix_object(lite_cjson_item_t *prev,lite_cjson_item_t *item) {prev->next=item;item->prev=prev;}
/* Utility for handling references. */
//static lite_cjson_item_t *create_reference(lite_cjson_item_t *item) {lite_cjson_item_t *ref=lite_cjson_new_item();if (!ref) return 0;memcpy(ref,item,sizeof(lite_cjson_item_t));ref->string=0;ref->type|=cJSON_IsReference;ref->next=ref->prev=0;return ref;}

/* Add item to array/object. */
void   lite_cjson_add_item_to_array(lite_cjson_item_t *array, lite_cjson_item_t *item)						{lite_cjson_item_t *c=array->child;if (!item) return; if (!c) {array->child=item;} else {while (c && c->next) c=c->next; suffix_object(c,item);}}
void   lite_cjson_add_item_to_object(lite_cjson_item_t *object,const char *string,lite_cjson_item_t *item)	{if (!item) return; if (item->string) free(item->string);item->string=lite_cjson_strdup(string);lite_cjson_add_item_to_array(object,item);}

/* Create basic types: */
lite_cjson_item_t *lite_cjson_create_null(void)					{lite_cjson_item_t *item=lite_cjson_new_item();if(item)item->type=cJSON_NULL;return item;}
lite_cjson_item_t *lite_cjson_create_true(void)					{lite_cjson_item_t *item=lite_cjson_new_item();if(item)item->type=cJSON_True;return item;}
lite_cjson_item_t *lite_cjson_create_false(void)				{lite_cjson_item_t *item=lite_cjson_new_item();if(item)item->type=cJSON_False;return item;}
lite_cjson_item_t *lite_cjson_create_bool(int b)				{lite_cjson_item_t *item=lite_cjson_new_item();if(item)item->type=b?cJSON_True:cJSON_False;return item;}
lite_cjson_item_t *lite_cjson_create_number(double num)				{lite_cjson_item_t *item=lite_cjson_new_item();if(item){item->type=cJSON_Number;item->valuedouble=num;item->valueint=(int)num;}return item;}
lite_cjson_item_t *lite_cjson_create_string(const char *string)			{lite_cjson_item_t *item=lite_cjson_new_item();if(item){item->type=cJSON_String;item->valuestring=lite_cjson_strdup(string);}return item;}
lite_cjson_item_t *lite_cjson_create_array(void)				{lite_cjson_item_t *item=lite_cjson_new_item();if(item)item->type=cJSON_Array;return item;}
lite_cjson_item_t *lite_cjson_create_object(void)				{lite_cjson_item_t *item=lite_cjson_new_item();if(item)item->type=cJSON_Object;return item;}

/* Create Arrays: */
lite_cjson_item_t *lite_cjson_create_intArray(const int *numbers,int count)		{int i;lite_cjson_item_t *n=0,*p=0,*a=lite_cjson_create_array();for(i=0;a && i<count;i++){n=lite_cjson_create_number(numbers[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}
lite_cjson_item_t *lite_cjson_create_floatArray(const float *numbers,int count)	{int i;lite_cjson_item_t *n=0,*p=0,*a=lite_cjson_create_array();for(i=0;a && i<count;i++){n=lite_cjson_create_number(numbers[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}
lite_cjson_item_t *lite_cjson_create_doubleArray(const double *numbers,int count)	{int i;lite_cjson_item_t *n=0,*p=0,*a=lite_cjson_create_array();for(i=0;a && i<count;i++){n=lite_cjson_create_number(numbers[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}
lite_cjson_item_t *lite_cjson_create_stringArray(const char **strings,int count)	{int i;lite_cjson_item_t *n=0,*p=0,*a=lite_cjson_create_array();for(i=0;a && i<count;i++){n=lite_cjson_create_string(strings[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}


