#include <string.h>
#include "tng_atca.h"
#include "tnglora_cert_def_2_device.h"
#include "tnglora_cert_def_4_device.h"
#include "tngtls_cert_def_2_device.h"
#include "tngtls_cert_def_3_device.h"


ATCA_STATUS tng_get_type(tng_type_t* type)
{
    ATCA_STATUS ret;
    uint8_t sn[ATCA_SERIAL_NUM_SIZE];

    if (type == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    ret = atcab_read_serial_number(sn);
    if (ret != ATCA_SUCCESS)
    {
        return ret;
    }

    if (sn[8] == 0x10 || sn[8] == 0x27)
    {
        *type = TNGTYPE_LORA;
    }
    else
    {
        *type = TNGTYPE_TLS;
    }

    return ATCA_SUCCESS;
}

ATCA_STATUS tng_get_device_cert_def(const atcacert_def_t **cert_def)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint8_t sn[ATCA_SERIAL_NUM_SIZE];
    uint8_t otpcode[32];

    if (cert_def == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    //Set default certificate definition
    *cert_def = &g_tngtls_cert_def_2_device;

    status = atcab_read_serial_number(sn);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    if (sn[8] == 0x01 || sn[8] == 0x10 || sn[8] == 0x27)
    {
        status = atcab_read_zone(ATCA_ZONE_OTP, 0, 0, 0, otpcode, 32);
        if (status != ATCA_SUCCESS)
        {
            return status;
        }

        if ((strncmp("wdNxAjae", otpcode, 8) == 0) || (strncmp("Rsuy5YJh", otpcode, 8) == 0))
        {
            *cert_def = &g_tngtls_cert_def_2_device;
        }
        else if ((strncmp("KQp2ZkD8", otpcode, 8) == 0) || (strncmp("x6tjuZMy", otpcode, 8) == 0))
        {
            *cert_def = &g_tngtls_cert_def_3_device;
        }
        else if (strncmp("BxZvm6q2", otpcode, 8) == 0)
        {
            *cert_def = &g_tnglora_cert_def_2_device;
        }
        else if ((strncmp("jsMu7iYO", otpcode, 8) == 0) || (strncmp("09qJNxI3", otpcode, 8) == 0))
        {
            *cert_def = &g_tnglora_cert_def_4_device;
        }
        else
        {
            //Nothing to do...Already initialized to default
        }
    }
    return status;
}

ATCA_STATUS tng_get_device_pubkey(uint8_t *public_key)
{
    ATCA_STATUS ret;
    tng_type_t type;

    ret = tng_get_type(&type);
    if (ret != ATCA_SUCCESS)
    {
        return ret;
    }

    if (type == TNGTYPE_LORA)
    {
        return atcab_get_pubkey(TNGLORA_PRIMARY_KEY_SLOT, public_key);
    }
    else
    {
        return atcab_get_pubkey(TNGTLS_PRIMARY_KEY_SLOT, public_key);
    }
}