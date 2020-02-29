/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "id2_config.h"
#include "id2_plat.h"
#include "id2_priv.h"
#include "id2_client.h"
#include "tee_client_api.h"
#include "km_cmd.h"
#include "km.h"

static const TEEC_UUID tee_km_uuid = KM_SRV_UUID;
extern TEEC_Context *g_tee_context;

extern int is_id2_client_inited(void);
extern void id2_reset_cache(void);

enum
{
    KEY_TYPE_AES_128    =       1,
    KEY_TYPE_AES_192    =       2,
    KEY_TYPE_AES_256    =       3,
    KEY_TYPE_3DES_112   =       4,
    KEY_TYPE_3DES_168   =       5,
};

#define ID2_OTP_SS_KEY_LEN      0x10
#define ID2_OTP_AUTH_CODE_VER   0x00010000

#define ID2_SS_KEY_LEN          16

#define ID2_ROUNDUP(a, b)       (((a) + ((b) - 1)) & ~((b) - 1))

enum
{
    SHA256_HASH_SIZE    =       32,
};

static irot_result_t teec_set_otp_data(uint8_t* otp_data, uint32_t len)
{
    irot_result_t ret = IROT_SUCCESS;

    TEEC_Session ss;
    TEEC_Operation op;
    TEEC_Result result;
    TEEC_UUID tee_uuid;
    tee_uuid = tee_km_uuid;

    if (otp_data == NULL || len == 0) {
        id2_log_error("bad input\n");
        return IROT_ERROR_BAD_PARAMETERS;
    }

    id2_log_hex_dump("otp_data", (uint8_t*)otp_data, len);
    
    result = TEEC_OpenSession(g_tee_context, &ss, &tee_uuid,
        TEEC_LOGIN_USER, NULL, NULL, NULL);
    if (result != TEEC_SUCCESS)
    {
        id2_log_error("open ss err(%08x)\n", result);
        ret = -1;
        goto cleanup;
    }

    op.paramTypes = TEEC_PARAM_TYPES(
                        TEEC_MEMREF_TEMP_INPUT,
                        TEEC_NONE,
                        TEEC_NONE,
                        TEEC_NONE);
    op.params[0].tmpref.buffer = otp_data;
    op.params[0].tmpref.size = len;

    result = TEEC_InvokeCommand(&ss, TEE_ID2_SET_OTP_DATA, &op, NULL);
    if (result != TEEC_SUCCESS)
    {
        id2_log_error("invokecmd err(%08x)\n", result);
        ret = result;
    }

    TEEC_CloseSession(&ss);
cleanup:
    return ret;
}

////////////////////////////////////////////////////////////////////////////////

irot_result_t id2_client_get_prov_stat(bool* is_prov)
{
    irot_result_t ret = IROT_SUCCESS;
    uint8_t id2[ID2_ID_LEN];
    uint32_t len = ID2_ID_LEN;
    uint32_t kmret;

    if (!is_id2_client_inited())
    {
        id2_log_error("id2 client not inited\n");
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    if (is_prov == NULL)
    {
        id2_log_error("invalid input arg\n");
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto _out;
    }

    kmret = km_get_id2(id2, &len);
    if (kmret == KM_ERR_ITEM_NOT_FOUND)
    {
        *is_prov = false;
        ret = IROT_SUCCESS;
        id2_log_debug("OTP prov state: False\n");
        goto _out;
    }
    else if (kmret != KM_SUCCESS)
    {
        id2_log_error("KM get id2 fail, 0x%08X\n", kmret);
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    id2_log_debug("OTP prov state: True\n");
    *is_prov = true;

_out:
    id2_log_debug("exit.\n");
    return ret;
}

enum
{
    DEVICE_TEST_FP_LEN  = 16,
};

irot_result_t id2_client_get_otp_auth_code(const uint8_t* token, uint32_t token_len,
        uint8_t* auth_code, uint32_t* len)
{
    irot_result_t ret = IROT_SUCCESS;

    TEEC_Session ss;
    TEEC_Operation op;
    TEEC_Result result;
    TEEC_UUID tee_uuid;

    uint8_t* tmp_token = NULL;

    if (!is_id2_client_inited())
    {
        id2_log_error("id2 client not inited\n");
        ret = IROT_ERROR_GENERIC;
        goto cleanup;
    }

    tee_uuid = tee_km_uuid;

    if (NULL == auth_code || *len == 0) {
        id2_log_error("invalid input\n");
        return -1;
    }

    result = TEEC_OpenSession(g_tee_context, &ss, &tee_uuid,
        TEEC_LOGIN_USER, NULL, NULL, NULL);
    if (result != TEEC_SUCCESS)
    {
        id2_log_error("open ss err(%08x)\n", result);
        ret = -1;
        goto cleanup;
    }

	if (token == NULL || token_len != 32) {
		id2_log_error("%s():%d: invalid temp token(%s) len (%d)\n", __func__, __LINE__,token, token_len);
		ret = -1;
		goto cleanup1;
	}

	tmp_token = ls_osa_malloc(token_len);
	if (tmp_token == NULL)
	{
		id2_log_error("%s: malloc %d fail\n", __func__, token_len);
		ret = IROT_ERROR_OUT_OF_MEMORY;
		goto cleanup1;
	}

	memcpy(tmp_token, token, token_len);

	op.paramTypes = TEEC_PARAM_TYPES(
						TEEC_MEMREF_TEMP_INPUT,
						TEEC_NONE,
						TEEC_NONE,
						TEEC_NONE);
	op.params[0].tmpref.buffer = (uint8_t *)tmp_token;
	op.params[0].tmpref.size = token_len;
	result = TEEC_InvokeCommand(&ss, TEE_ID2_PROV_TOKEN, &op, NULL);
	if (result != TEEC_SUCCESS)
	{
		id2_log_error("%s():%d: invokecmd err(%08x)\n", __func__, __LINE__, result);
		ret = result;
		goto cleanup1;
	}


    op.paramTypes = TEEC_PARAM_TYPES(
                        TEEC_MEMREF_TEMP_OUTPUT,
                        TEEC_VALUE_INOUT,
                        TEEC_NONE,
                        TEEC_NONE);
    op.params[0].tmpref.buffer = auth_code;
    op.params[0].tmpref.size = *len;
    op.params[1].value.a = *len;

    result = TEEC_InvokeCommand(&ss, TEE_ID2_GET_OTP_AUTHCODE, &op, NULL);
    if (result != TEEC_SUCCESS)
    {
        id2_log_error("invokecmd err(%08x)\n", result);
        ret = result;
        goto cleanup1;
    }

    *len = op.params[1].value.a;
    id2_log_hex_dump("otp_auth_code", auth_code, *len);

cleanup1:
    TEEC_CloseSession(&ss);
cleanup:

    if (tmp_token != NULL) {
        ls_osa_free(tmp_token);
        tmp_token = NULL;
    }

    return ret;
}

irot_result_t id2_client_load_otp_data(const uint8_t* otp_data, uint32_t len)
{
    irot_result_t ret;

    id2_log_debug("enter\n");

    if (!is_id2_client_inited())
    {
        id2_log_error("id2 client not inited\n");
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    ret = teec_set_otp_data((uint8_t*)otp_data, len);
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("set otp data fail(0x%08x)\n", ret);
        goto _out;
    }

_out:
    id2_log_debug("exit\n");
    return ret;
}

