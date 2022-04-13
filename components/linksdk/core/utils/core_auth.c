#include "core_auth.h"

int32_t core_auth_tls_psk(aiot_sysdep_portfile_t *sysdep, char **psk_id, char psk[65], char *product_key, char *device_name, char *device_secret, char *module_name)
{
    int32_t res = STATE_SUCCESS;
    char *tmp_psk_id = NULL, *auth_type = "devicename", *sign_method = "hmacsha256";
    char *psk_id_src[] = { auth_type, sign_method, product_key, device_name, CORE_AUTH_TIMESTAMP};
    char *psk_plain_text = NULL, *psk_plain_text_src[] = { product_key, device_name, CORE_AUTH_TIMESTAMP};
    uint8_t psk_hex[32] = {0};

    if (NULL == device_secret) {
        return STATE_USER_INPUT_MISSING_DEVICE_SECRET;
    }

    res = core_sprintf(sysdep, &tmp_psk_id, "%s|%s|%s&%s|%s", psk_id_src, sizeof(psk_id_src)/sizeof(char *), module_name);
    if (res < STATE_SUCCESS) {
        return res;
    }

    res = core_sprintf(sysdep, &psk_plain_text, "id%s&%stimestamp%s", psk_plain_text_src, sizeof(psk_plain_text_src)/sizeof(char *), module_name);
    if (res < STATE_SUCCESS) {
        sysdep->core_sysdep_free(tmp_psk_id);
        return res;
    }

    core_hmac_sha256((const uint8_t *)psk_plain_text, (uint32_t)strlen(psk_plain_text), (const uint8_t *)device_secret, (uint32_t)strlen(device_secret), psk_hex);
    core_hex2str(psk_hex, 32, psk, 0);

    *psk_id = tmp_psk_id;
    sysdep->core_sysdep_free(psk_plain_text);

    return res;
}

int32_t core_auth_mqtt_username(aiot_sysdep_portfile_t *sysdep, char **dest, char *product_key, char *device_name, char *module_name)
{
    char *src[] = { device_name, product_key };

    return core_sprintf(sysdep, dest, "%s&%s", src, sizeof(src)/sizeof(char *), module_name);
}

int32_t core_auth_mqtt_password(aiot_sysdep_portfile_t *sysdep, char **dest, char *product_key, char *device_name, char *device_secret, char *module_name)
{
    int32_t res = 0;
    char *src[] = { product_key, device_name, device_name, product_key, CORE_AUTH_TIMESTAMP };
    char *plain_text = NULL;
    uint8_t sign[32] = {0};

    res = core_sprintf(sysdep, &plain_text, "clientId%s.%sdeviceName%sproductKey%stimestamp%s", src, sizeof(src)/sizeof(char *), module_name);
    if (res < STATE_SUCCESS) {
        return res;
    }

    *dest = sysdep->core_sysdep_malloc(65, module_name);
    if (*dest == NULL) {
        sysdep->core_sysdep_free(plain_text);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(*dest, 0, 65);

    core_hmac_sha256((const uint8_t *)plain_text, (uint32_t)strlen(plain_text), (const uint8_t *)device_secret, (uint32_t)strlen(device_secret),sign);
    core_hex2str(sign, 32, *dest, 0);

    sysdep->core_sysdep_free(plain_text);

    return 0;
}

int32_t core_auth_mqtt_clientid(aiot_sysdep_portfile_t *sysdep, char **dest, char *product_key, char *device_name, char *secure_mode, char *extend_clientid, char *module_name)
{
    char *src[] = { product_key, device_name, CORE_AUTH_TIMESTAMP, CORE_AUTH_SDK_VERSION, secure_mode, extend_clientid};

    return core_sprintf(sysdep, dest, "%s.%s|timestamp=%s,_ss=1,_v=%s,securemode=%s,signmethod=hmacsha256,ext=3,%s|", src, sizeof(src)/sizeof(char *), module_name);    /* ext bitmap: bit0-rrpc, bit1-ext_notify */
}

int32_t core_auth_http_body(aiot_sysdep_portfile_t *sysdep, char **dest, char *product_key, char *device_name, char *device_secret, char *module_name)
{
    int32_t res = 0;
    char *sign_ele[] = { product_key, device_name, device_name, product_key, NULL };
    char *plain_text = NULL;
    uint8_t sign_hex[32] = {0};
    char sign_str[65] = {0};

    res = core_sprintf(sysdep, &plain_text, "clientId%s.%sdeviceName%sproductKey%s", sign_ele, 4, module_name);
    if (res < STATE_SUCCESS) {
        return res;
    }

    core_hmac_sha256((const uint8_t *)plain_text, (uint32_t)strlen(plain_text), (const uint8_t *)device_secret, (uint32_t)strlen(device_secret), sign_hex);
    core_hex2str(sign_hex, 32, sign_str, 0);

    sysdep->core_sysdep_free(plain_text);
    sign_ele[4] = sign_str;
    res = core_sprintf(sysdep,
                       dest,
                       "{\"clientId\":\"%s.%s\",\"signmethod\":\"hmacsha256\",\"deviceName\":\"%s\",\"productKey\":\"%s\",\"sign\":\"%s\"}",
                       sign_ele,
                       sizeof(sign_ele)/sizeof(char *),
                       module_name);

    return res;
}

