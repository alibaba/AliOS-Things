/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _DM_UTILS_H_
#define _DM_UTILS_H_

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

int dm_utils_itoa_direct(_IN_ int input, _OU_ char **output);
int dm_utils_itoa(_IN_ int input, _OU_ char **output);
int dm_utils_ftoa_direct(_IN_ double input, _OU_ char **output);
int dm_utils_ftoa(_IN_ double input, _OU_ char **output);
int dm_utils_hex_to_str(_IN_ unsigned char *input, _IN_ int input_len, _OU_ char **output);
int dm_utils_str_to_hex(_IN_ char *input, _IN_ int input_len, _OU_ unsigned char **output, _OU_ int *output_len);
int dm_utils_memtok(_IN_ char *input, _IN_ int input_len, _IN_ char delimiter, _IN_ int index, _OU_ int *offset);
int dm_utils_replace_char(_IN_ char *input, _IN_ int input_len, _IN_ char src, _IN_ char dest);
int dm_utils_service_name(_IN_ const char *prefix, _IN_ const char *name, _IN_ char product_key[PRODUCT_KEY_MAXLEN],
                          _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ char **service_name);
int dm_utils_uri_add_prefix(_IN_ const char *prefix, _IN_ char *uri, _OU_ char **new_uri);
int dm_utils_json_parse(_IN_ const char *payload, _IN_ int payload_len, _IN_ int type, _OU_ lite_cjson_t *lite);
int dm_utils_json_object_item(_IN_ lite_cjson_t *lite, _IN_ const char *key, _IN_ int key_len, _IN_ int type,
                              _OU_ lite_cjson_t *lite_item);
void *dm_utils_malloc(unsigned int size);
void dm_utils_free(void *ptr);
#endif
