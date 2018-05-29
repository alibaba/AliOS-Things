/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#include "lite-utils.h"

static char *module_name = "utils-api_demo";

int demo_string_utils(void)
{
    unsigned char       hexbuf[] = { 0x12, 0x34, 0x56, 0x78, 0xcd, 0xef, 0xab, 0x90 };
    char                hexstr[20] = { 0 };
    unsigned char       outbuf[10] = { 0x0 };

    LITE_hexbuf_convert(hexbuf, hexstr, sizeof(hexbuf), 0);
    log_info("hexstr = '%s'", hexstr);
    LITE_hexbuf_convert(hexbuf, hexstr, sizeof(hexbuf), 1);
    log_info("hexstr = '%s'", hexstr);

    LITE_hexstr_convert(hexstr, outbuf, strlen(hexstr));
    HEXDUMP_INFO(outbuf, sizeof(outbuf));

#define UNITTEST_STRING_FORMAT      "Integer: %d, String: '%s', Char: %c, Hex: %04x"

    char               *str = NULL;
    unsigned int        len = 0;
    len = strlen(UNITTEST_STRING_FORMAT) + 3 + strlen("Hello") + 1 + 4;
    str = LITE_format_string(len, UNITTEST_STRING_FORMAT, MEM_MAGIC, module_name, 100, "Hello", 'A', 640);
    log_info("(Length: %02d) '%s'", (int)strlen(str), str);
    LITE_free(str);

    str = LITE_format_nstring(10, UNITTEST_STRING_FORMAT, MEM_MAGIC, module_name, 100, "Hello", 'A', 640);
    log_info("(Length: %02d) '%s'", (int)strlen(str), str);
    LITE_free(str);

    str = LITE_format_nstring(20, UNITTEST_STRING_FORMAT, MEM_MAGIC, module_name, 100, "Hello", 'A', 640);
    log_info("(Length: %02d) '%s'", (int)strlen(str), str);
    LITE_free(str);

    str = LITE_format_nstring(40, UNITTEST_STRING_FORMAT, MEM_MAGIC, module_name, 100, "Hello", 'A', 640);
    log_info("(Length: %02d) '%s'", (int)strlen(str), str);
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
    log_info("ORIG: '%s'", source);
    LITE_replace_substr(source, "Worl", "Clou");
    log_info("REPL: '%s'", source);

    LITE_snprintf(source, sizeof(source), "%s", UNITTEST_JSON_FORMAT);
    log_info("ORIG: '%s'", source);
    LITE_replace_substr(source, "Float", "%f");
    LITE_replace_substr(source, "Integer", "%d");
    LITE_replace_substr(source, "String", "%s");
    LITE_replace_substr(source, "Boolean", "%s");
    log_info("REPL: '%s'", source);

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


int demo_json_token(void)
{
    list_head_t    *key_list = NULL;
    json_key_t     *pos;

    key_list = LITE_json_keys_of(UNITTEST_JSON_SAMPLE, "", MEM_MAGIC, module_name);

    list_for_each_entry(pos, key_list, list, json_key_t) {
        if (pos->key) {
            char           *val = NULL;

            val = LITE_json_value_of(pos->key, UNITTEST_JSON_SAMPLE, MEM_MAGIC, module_name);
            log_info("%-28s: %.48s", pos->key, val);
            LITE_free(val);
        }
    }
    LITE_json_keys_release(key_list);

#if defined(__GLIBC__)
    /* Demo _usage without awareness of json_key_t{} and returned list */

    char           *sub_objc;
    void           *ret_list;
    void           *temp;
    char           *token;

    sub_objc = LITE_json_value_of(UNITTEST_JSON_SUBKEY, UNITTEST_JSON_SAMPLE, MEM_MAGIC, module_name);

    foreach_json_keys_in(sub_objc, token, ret_list, temp) {
        char       *val;

        val = LITE_json_value_of(token, sub_objc, MEM_MAGIC, module_name);
        log_info("%s|%-18s: %.48s", UNITTEST_JSON_SUBKEY, token, val);
        LITE_free(val);
    }
    LITE_json_keys_release(ret_list);
    LITE_free(sub_objc);
#endif  /* #if defined(__GLIBC__) */

    return 0;
}

int demo_json_parser(void)
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
        val = LITE_json_value_of(*pkey, UNITTEST_JSON_SAMPLE, MEM_MAGIC, module_name);
        if (val == NULL) {
            log_err("failed to get value of key: '%s'", *pkey);
            return -1;
        }

        log_info("%-28s: %s", *pkey, val);
        LITE_free(val);
    }

    return 0;
}

#define UNITTEST_JSON_ARRAY_SAMPLE \
"{" \
    "\"link\":\"/sys/q408EXte2fy/airCondition/thing/\""\
    "\"profile\": {" \
        "\"productKey\": \"q408EXte2fy\"," \
        "\"deviceName\": \"airCondition\""  \
    "}," \
    "\"events\": [" \
        "[\"country\",[\"provice\",\"city\"],[\"name\",\"address\",\"phone\"]]," \
        "{" \
            "\"name\": \"alarm\"," \
            "\"identifier\": \"alarm\"," \
            "\"desc\": \"警报\"," \
            "\"type\": \"info\"," \
            "\"required\": true," \
            "\"outputData\": [" \
                "{" \
                    "\"identifier\": \"errorCode\"," \
                    "\"name\": \"错误码\"," \
                    "\"dataType\": {" \
                        "\"type\": \"text\"," \
                        "\"specs\": {" \
                            "\"length\": \"255\"," \
                            "\"unit\": \"\"" \
                        "}" \
                    "}" \
                "}" \
            "]," \
            "\"method\": \"thing.event.alarm.post\"" \
        "}," \
        "[\"name\",\"address\",\"phone\"]," \
        "{" \
            "\"identifier\": \"post\"," \
            "\"name\": \"post\"," \
            "\"type\": \"info\"," \
            "\"required\": true," \
            "\"desc\": \"属性上报\"," \
            "\"method\": \"thing.event.property.post\"," \
            "\"outputData\": [" \
                "{" \
                    "\"identifier\": \"fan_struct_property\"," \
                    "\"name\": \"风扇结构型属性\"," \
                    "\"dataType\": {" \
                        "\"type\": \"struct\"," \
                        "\"specs\": [" \
                            "{" \
                                "\"identifier\": \"fan_struct_property_float_child\"," \
                                "\"name\": \"风扇结构型属性浮点子属性\"," \
                                "\"dataType\": {" \
                                    "\"type\": \"float\"," \
                                    "\"specs\": {" \
                                        "\"min\": \"0.0\"," \
                                        "\"max\": \"255.0\"," \
                                        "\"precise\": \"1\"," \
                                        "\"unit\": \"T\"," \
                                        "\"unitName\": \"度\"" \
                                    "}" \
                                "}" \
                            "}," \
                            "{" \
                                "\"identifier\": \"fan_struct_property_enum_child\"," \
                                "\"name\": \"风扇结构型属性枚举子属性\"," \
                                "\"dataType\": {" \
                                    "\"type\": \"enum\"," \
                                    "\"specs\": {" \
                                        "\"0\": \"one\"," \
                                        "\"1\": \"two\"," \
                                        "\"2\": \"three\"" \
                                    "}" \
                                "}" \
                            "}" \
                        "]" \
                    "}" \
                "}" \
            "]" \
        "}" \
    "]" \
"}"

int demo_json_array_token()
{
    char           *val;
    list_head_t    *key_list = NULL;
    json_key_t     *pos;

    key_list = LITE_json_keys_of_ext(UNITTEST_JSON_ARRAY_SAMPLE, "", MEM_MAGIC, module_name);

    list_for_each_entry(pos, key_list, list, json_key_t) {
        if (pos->key) {
            val = LITE_json_value_of_ext(pos->key, UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
            if (NULL != val) {
                printf(": %s(%d): %-28s: %s\n", __FUNCTION__, __LINE__, pos->key, val);
            } else {
                log_err("failed to get value of key: '%s'", pos->key);
            }
            LITE_free(val);
        }
    }

    LITE_json_keys_release(key_list);
    return 0;
}

int demo_json_array_parse()
{
    char *keys[] = {
        "link",
        "profile",
        "profile.productKey",
        "profile.deviceName",
        "events",
        "events[1]",
        "events[1][1]",
        "events[1][2]",
        "events[1][2][1]",
        "events[1][2][2]",
        "events[1][3]",
        "events[1][3][1]",
        "events[1][3][2]",
        "events[1][3][3]",
        "events[2]",
        "events[2].name",
        "events[2].identifier",
        "events[2].desc",
        "events[2].type",
        "events[2].required",
        "events[2].outputData",
        "events[2].outputData[1]",
        "events[2].outputData[1].identifier",
        "events[2].outputData[1].name",
        "events[2].outputData[1].dataType",
        "events[2].outputData[1].dataType.type",
        "events[2].outputData[1].dataType.specs",
        "events[2].outputData[1].dataType.specs.length",
        "events[2].method",
        "events[3]",
        "events[4]",
        "events[4].identifier",
        "events[4].name",
        "events[4].type",
        "events[4].required",
        "events[4].desc",
        "events[4].method",
        "events[4].outputData",
        "events[4].outputData[1]",
        "events[4].outputData[1].identifier",
        "events[4].outputData[1].name",
        "events[4].outputData[1].dataType",
        "events[4].outputData[1].dataType.type",
        "events[4].outputData[1].dataType.specs",
        "events[4].outputData[1].dataType.specs[1]",
        "events[4].outputData[1].dataType.specs[1].identifier",
        "events[4].outputData[1].dataType.specs[1].name",
        "events[4].outputData[1].dataType.specs[1].dataType",
        "events[4].outputData[1].dataType.specs[1].dataType.type",
        "events[4].outputData[1].dataType.specs[1].dataType.specs",
        "events[4].outputData[1].dataType.specs[1].dataType.specs.min",
        "events[4].outputData[1].dataType.specs[1].dataType.specs.max",
        "events[4].outputData[1].dataType.specs[1].dataType.specs.precise",
        "events[4].outputData[1].dataType.specs[1].dataType.specs.unit",
        "events[4].outputData[1].dataType.specs[1].dataType.specs.unitName",
        "events[4].outputData[1].dataType.specs[2]",
        "events[4].outputData[1].dataType.specs[2].identifier",
        "events[4].outputData[1].dataType.specs[2].name",
        "events[4].outputData[1].dataType.specs[2].dataType",
        "events[4].outputData[1].dataType.specs[2].dataType.type",
        "events[4].outputData[1].dataType.specs[2].dataType.specs",
        "events[4].outputData[1].dataType.specs[2].dataType.specs.0",
        "events[4].outputData[1].dataType.specs[2].dataType.specs.1",
        "events[4].outputData[1].dataType.specs[2].dataType.specs.2"
    };

    char           *val = NULL;
    int             i = 0;

    for (i = 0; i < sizeof(keys) / sizeof(keys[0]); ++ i) {
        val = LITE_json_value_of_ext(keys[i], UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
        if (val == NULL) {
            log_err("failed to get value of key: '%s'", keys[i]);
            return -1;
        }
        printf(": %s(%d): %-28s: %s\n", __FUNCTION__, __LINE__, keys[i], val);
        LITE_free(val);
    }
    return 0;
}

