/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "iotx_utils_internal.h"
#include "ut_debug.h"

int unittest_string_utils(void)
{
    unsigned char       hexbuf[] = { 0x12, 0x34, 0x56, 0x78, 0xcd, 0xef, 0xab, 0x90 };
    char                hexstr[20] = { 0 };
    unsigned char       outbuf[10] = { 0x0 };

    LITE_hexbuf_convert(hexbuf, hexstr, sizeof(hexbuf), 0);
    ut_info("hexstr = '%s'", hexstr);
    LITE_hexbuf_convert(hexbuf, hexstr, sizeof(hexbuf), 1);
    ut_info("hexstr = '%s'", hexstr);

    LITE_hexstr_convert(hexstr, strlen(hexstr), outbuf, 10);
    HEXDUMP_INFO(outbuf, sizeof(outbuf));

#define UNITTEST_STRING_FORMAT      "Integer: %d, String: '%s', Char: %c, Hex: %04x"

    char               *str = NULL;

    str = LITE_format_string(UNITTEST_STRING_FORMAT, 100, "Hello", 'A', 640);
    ut_info("(Length: %02d) '%s'", (int)strlen(str), str);
    LITE_free(str);

    str = LITE_format_nstring(10, UNITTEST_STRING_FORMAT, 100, "Hello", 'A', 640);
    ut_info("(Length: %02d) '%s'", (int)strlen(str), str);
    LITE_free(str);

    str = LITE_format_nstring(20, UNITTEST_STRING_FORMAT, 100, "Hello", 'A', 640);
    ut_info("(Length: %02d) '%s'", (int)strlen(str), str);
    LITE_free(str);

    str = LITE_format_nstring(40, UNITTEST_STRING_FORMAT, 100, "Hello", 'A', 640);
    ut_info("(Length: %02d) '%s'", (int)strlen(str), str);
    LITE_free(str);

#define UNITTEST_JSON_FORMAT    \
    "{" \
    "\"KeyFlt\": \"Float\", " \
    "\"KeyInt\": \"Integer\", " \
    "\"KeyStr\": \"String\", " \
    "\"KeyBool\": \"Boolean\"" \
    "}"

    char                source[128];

    LITE_snprintf(source, sizeof(source), "%s", "Hello, World!");
    ut_info("ORIG: '%s'", source);
    LITE_replace_substr(source, "Worl", "Clou");
    ut_info("REPL: '%s'", source);

    LITE_snprintf(source, sizeof(source), "%s", UNITTEST_JSON_FORMAT);
    ut_info("ORIG: '%s'", source);
    LITE_replace_substr(source, "Float", "%f");
    LITE_replace_substr(source, "Integer", "%d");
    LITE_replace_substr(source, "String", "%s");
    LITE_replace_substr(source, "Boolean", "%s");
    ut_info("REPL: '%s'", source);

    return 0;
}

#define UNITTEST_JSON_SAMPLE    \
\
" " \
"{" \
    "\"code\":200," \
    "\"ArrayList\":[ 1, 4, 16 ]," \
    "\"KeyTrue\": true," \
    "\"KeyFalse\": false," \
    "\"data\":{ " \
        "\"iotId\":\"42Ze0mk3556498a1AlTP\"," \
        "\"iotToken\":\"0d7fdeb9dc1f4344a2cc0d45edcb0bcb\"," \
        "\"resources\":{" \
            "\"mqtt\":{" \
                "\"host\":\"10.10.10.10\"," \
                "\"port\":9999" \
            "}," \
            "\"codec\":{" \
                "\"name\":\"AES_CBC_NOPADDING\"," \
                "\"key\":\"12321321\"" \
            "}" \
        "}" \
    "}," \
    "\"message\":\"success\"" \
"}"
#define UNITTEST_JSON_SUBKEY    "data.resources"

int unittest_json_token(void)
{
    list_head_t    *key_list = NULL;
    json_key_t     *pos;

    key_list = LITE_json_keys_of(UNITTEST_JSON_SAMPLE, "");

    list_for_each_entry(pos, key_list, list, json_key_t) {
        if (pos->key) {
            char           *val = NULL;

            val = LITE_json_value_of(pos->key, UNITTEST_JSON_SAMPLE);
            ut_info("%-28s: %.48s", pos->key, val);
            LITE_free(val);
        }
    }
    LITE_json_keys_release(key_list);

#if defined(__GLIBC__)
    /* Demo usage without awareness of json_key_t{} and returned list */

    char           *sub_objc;
    void           *ret_list;
    void           *temp;
    char           *token;

    sub_objc = LITE_json_value_of(UNITTEST_JSON_SUBKEY, UNITTEST_JSON_SAMPLE);

    foreach_json_keys_in(sub_objc, token, ret_list, temp) {
        char       *val;

        val = LITE_json_value_of(token, sub_objc);
        ut_info("%s|%-18s: %.48s", UNITTEST_JSON_SUBKEY, token, val);
        LITE_free(val);
    }
    LITE_json_keys_release(ret_list);
    LITE_free(sub_objc);
#endif  /* #if defined(__GLIBC__) */

    return 0;
}

int unittest_json_parser(void)
{
    char           *val;
    char          **pkey;
    char           *keys[] = {
        "data.iotToken",
        "data.iotId",
        "data.resources.mqtt",
        "data.resources.mqtt.host",
        "data.resources.mqtt.port",
        "data.resources.codec",
        "data.resources.codec.name",
        "data.resources.codec.key",
        "message",
        0
    };

    for (pkey = keys; *pkey; ++ pkey) {
        val = LITE_json_value_of(*pkey, UNITTEST_JSON_SAMPLE);
        if (val == NULL) {
            ut_err("failed to get value of key: '%s'", *pkey);
            return -1;
        }

        ut_info("%-28s: %s", *pkey, val);
        LITE_free(val);
    }

    return 0;
}

