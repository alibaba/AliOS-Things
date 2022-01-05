// Copyright (2018) Baidu Inc. All rights reserved.
/**
 * File: lightduer_flash_strings.h
 * Auth: Sijun Li(lisijun@baidu.com)
 * Desc: APIs of store string list to flash.
 */
#ifndef BAIDU_DUER_LIGHTDUER_FLASH_STRINGS_H
#define BAIDU_DUER_LIGHTDUER_FLASH_STRINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lightduer_flash.h"

typedef enum {
    ALARM_FLASH             = 0,
    MAX_FLASH_MODULE        = 1,
}duer_flash_module;

typedef struct {
    duer_flash_context_t ctx;
    int max_ele_count;
    unsigned int *ele_list;
} duer_flash_string_context_t;

typedef char *(*duer_raw2string_func)(void *);
typedef void (*duer_free_string_func)(char *);

// Need users to implement to adapt different platform.
/**
 * DESC:
 * Developer needs to implement this interface to init flash context.
 * Must call duer_set_flash_config in this function.
 *
 * @PARAM[in] module: selected module, corresponding to context.
 *
 * @RETURN: pointer of created context.
 */
extern duer_flash_string_context_t *duer_flash_init(duer_flash_module module);

/**
 * DESC:
 * Should be called in duer_flash_init, 
 * This function will only take effect when first time called.
 * 
 * PARAM config: configuration of hardware flash.
 *
 * @RETURN: none
 */
extern void duer_set_flash_config(const duer_flash_config_t *config);

// High level APIs.
/**
 * DESC:
 * Append one flash string element to flash data. if flash data is empty, the element will be 
 * written to the begining of flash addr (address offset = 0).
 * 
 * @PARAM[in] ctx: pointer of context.
 * @PARAM[in] raw_data: raw data of element needs to store to flash.
 * @PARAM[in] raw2string_func: function to convert raw_data to string. if this param is NULL, 
 *              treat raw_data as a string.
 * @PARAM[in] free_string_func: function to free the string created by raw2string_func. Must
 *              not NULL when raw2string_func not NULL.
 *
 * @RETURN: 0 when success, else when fail.
 */
extern int duer_append_to_flash(
        duer_flash_string_context_t *ctx,
        void *raw_data,
        duer_raw2string_func raw2string_func,
        duer_free_string_func free_string_func);

/**
 * DESC:
 * Get the first and last flash element address offset, and reset the status in ctx.
 * MUST be called before duer_update_to_flash.
 * 
 * @PARAM[in] ctx: pointer of context.
 * @PARAM[out] p_first_ele_addr: the address offset of frist flash element.
 * @PARAM[out] p_last_ele_addr: the address offset of last flash element.
 *
 * @RETURN: none
 */
extern void duer_update_to_flash_prepare(
        duer_flash_string_context_t *ctx,
        unsigned int *p_first_ele_addr,
        unsigned int *p_last_ele_addr);

/**
 * DESC:
 * Update to add one flash string element.
 * MUST be called before duer_update_to_flash.
 * 
 * @PARAM[in] ctx: pointer of context.
 * @PARAM[in] raw_data: raw data of element needs to store to flash.
 * @PARAM[in] raw2string_func: function to convert raw_data to string. if this param is NULL, 
 *              treat raw_data as a string.
 * @PARAM[in] free_string_func: function to free the string created by raw2string_func. Must
 *              not NULL when raw2string_func not NULL.
 * @PARAM[out] p_last_ele_addr: new address offset of last flash element, after updating.
 *              this value will be used when updating next element.
 *
 * @RETURN: 0 when success, else when fail.
 */
extern int duer_update_to_flash(
        duer_flash_string_context_t *ctx,
        void *raw_data,
        duer_raw2string_func raw2string_func,
        duer_free_string_func free_string_func,
        unsigned int *p_last_ele_addr);

/**
 * DESC:
 * Update the flash header, to mark the data added via duer_update_to_flash as new data,
 * and mark as old data as deleted.
 * Must be called after all elements updated to flash.
 *
 * Note: we keep this function out of duer_update_to_flash to prevent data lossing. For example,
 *      if hardware reboot when updating flash, the old data will still be stored in flash 
 *      because the header havn't updated.
 * 
 * @PARAM[in] ctx: pointer of context.
 * @PARAM[in] raw_data: raw data of element needs to store to flash.
 * @PARAM[in] raw2string_func: function to convert raw_data to string. if this param is NULL, 
 *              treat raw_data as a string.
 * @PARAM[in] free_string_func: function to free the string created by raw2string_func. Must
 *              not NULL when raw2string_func not NULL.
 * @PARAM[out] p_last_ele_addr: new address offset of last flash element, after updating.
 *              this value will be used when updating next element.
 *
 * @RETURN: 0 when success, else when fail.
 */
extern void duer_update_flash_header(
        duer_flash_string_context_t *ctx,
        unsigned int first_ele_addr);

/**
 * DESC:
 * Scan the flash area to retrieve flash element's address, and store in ctx.
 *
 * @PARAM[in] ctx: pointer of context.
 *
 * @RETURN: none
 */
extern void duer_get_all_ele_from_flash(
        duer_flash_string_context_t *ctx);

/**
 * DESC:
 * Get flash header of element, to retrieve information of element, like string length.
 * SHOULD be called before duer_parse_flash_ele_to_string.
 * 
 * @PARAM[in] ctx: pointer of context.
 * @PARAM[in] flash_index: index of flash element to get.
 * @PARAM[out] p_header: pointer to store header output.
 *
 * @RETURN: 0 when success, else when fail.
 */
extern int duer_get_flash_header(
        duer_flash_string_context_t *ctx,
        unsigned int flash_index,
        duer_flash_data_header *p_header);

/**
 * DESC:
 * Get the string value of flash element.
 * 
 * @PARAM[in] ctx: pointer of context.
 * @PARAM[in] flash_index: index of flash element to get.
 * @PARAM[in] header: flash header value.
 * @PARAM[out] payload_string: buffer to store string output. Must allocate memory according to 
 *              length value stored in header, before call this function.
 *
 * @RETURN: 0 when success, else when fail.
 */
extern void duer_parse_flash_ele_to_string(
        duer_flash_string_context_t *ctx,
        unsigned int flash_index,
        duer_flash_data_header data,
        char *payload_string);

#ifdef __cplusplus
}
#endif

#endif//BAIDU_DUER_LIGHTDUER_FLASH_STRINGS_H

