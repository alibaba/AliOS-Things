/****************************************************************************
* This confidential and proprietary software may be used only as authorized *
* by a licensing agreement from ARM Israel.                                 *
* Copyright (C) 2015 ARM Limited or its affiliates. All rights reserved.    *
* The entire notice above must be reproduced on all authorized copies and   *
* copies may only be made to the extent permitted by a licensing agreement  *
* from ARM Israel.                                                          *
*****************************************************************************/

/*!
@file
@brief This file contains the definitions of the CryptoCell AES errors.
*/

#ifndef SSI_AES_ERROR_H
#define SSI_AES_ERROR_H

#include "crys_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

/************************ Defines ******************************/

/* generic errors */
#define SASI_FATAL_ERROR                 CRYS_FATAL_ERROR
#define SASI_OUT_OF_RESOURCE_ERROR       CRYS_OUT_OF_RESOURCE_ERROR
#define SASI_ILLEGAL_RESOURCE_VAL_ERROR  CRYS_ILLEGAL_RESOURCE_VAL_ERROR

/* CRYS_AES_MODULE_ERROR_BASE - 0x00F00000 */
#define SASI_AES_INVALID_USER_CONTEXT_POINTER_ERROR     (CRYS_AES_MODULE_ERROR_BASE + 0x00UL)
#define SASI_AES_INVALID_IV_OR_TWEAK_PTR_ERROR          (CRYS_AES_MODULE_ERROR_BASE + 0x01UL)
#define SASI_AES_ILLEGAL_OPERATION_MODE_ERROR           (CRYS_AES_MODULE_ERROR_BASE + 0x02UL)
#define SASI_AES_ILLEGAL_KEY_SIZE_ERROR                 (CRYS_AES_MODULE_ERROR_BASE + 0x03UL)
#define SASI_AES_INVALID_KEY_POINTER_ERROR              (CRYS_AES_MODULE_ERROR_BASE + 0x04UL)
#define SASI_AES_KEY_TYPE_NOT_SUPPORTED_ERROR           (CRYS_AES_MODULE_ERROR_BASE + 0x05UL)
#define SASI_AES_INVALID_ENCRYPT_MODE_ERROR             (CRYS_AES_MODULE_ERROR_BASE + 0x06UL)
#define SASI_AES_USER_CONTEXT_CORRUPTED_ERROR           (CRYS_AES_MODULE_ERROR_BASE + 0x07UL)
#define SASI_AES_DATA_IN_POINTER_INVALID_ERROR          (CRYS_AES_MODULE_ERROR_BASE + 0x08UL)
#define SASI_AES_DATA_OUT_POINTER_INVALID_ERROR         (CRYS_AES_MODULE_ERROR_BASE + 0x09UL)
#define SASI_AES_DATA_IN_SIZE_ILLEGAL                   (CRYS_AES_MODULE_ERROR_BASE + 0x0AUL)
#define SASI_AES_DATA_OUT_DATA_IN_OVERLAP_ERROR         (CRYS_AES_MODULE_ERROR_BASE + 0x0BUL)
#define SASI_AES_DATA_IN_BUFFER_SIZE_ERROR              (CRYS_AES_MODULE_ERROR_BASE + 0x0CUL)
#define SASI_AES_DATA_OUT_BUFFER_SIZE_ERROR             (CRYS_AES_MODULE_ERROR_BASE + 0x0DUL)
#define SASI_AES_ILLEGAL_PADDING_TYPE_ERROR             (CRYS_AES_MODULE_ERROR_BASE + 0x0EUL)
#define SASI_AES_INCORRECT_PADDING_ERROR                (CRYS_AES_MODULE_ERROR_BASE + 0x0FUL)
#define SASI_AES_CORRUPTED_OUTPUT_ERROR                 (CRYS_AES_MODULE_ERROR_BASE + 0x10UL)
#define SASI_AES_DATA_OUT_SIZE_POINTER_INVALID_ERROR    (CRYS_AES_MODULE_ERROR_BASE + 0x11UL)
#define SASI_AES_DECRYPTION_NOT_ALLOWED_ON_THIS_MODE    (CRYS_AES_MODULE_ERROR_BASE + 0x12UL)

#define SASI_AES_ADDITIONAL_BLOCK_NOT_PERMITTED_ERROR   (CRYS_AES_MODULE_ERROR_BASE + 0x15UL)
#define SASI_AES_CTX_SIZES_ERROR                       (CRYS_AES_MODULE_ERROR_BASE + 0x16UL)

#define SASI_AES_ILLEGAL_PARAMS_ERROR               (CRYS_AES_MODULE_ERROR_BASE + 0x60UL)
#define SASI_AES_CTR_ILLEGAL_BLOCK_OFFSET_ERROR     (CRYS_AES_MODULE_ERROR_BASE + 0x70UL)
#define SASI_AES_CTR_ILLEGAL_COUNTER_ERROR          (CRYS_AES_MODULE_ERROR_BASE + 0x71UL)
#define SASI_AES_IS_NOT_SUPPORTED                   (CRYS_AES_MODULE_ERROR_BASE + 0xFFUL)

/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  *****************************/

/************************ Public Variables *********************/

/************************ Public Functions *********************/

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SSI_AES_ERROR_H */
