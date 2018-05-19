#include "lite-utils_internal.h"
#include "json_parser.h"
#include "cut.h"

extern int json_get_value_by_name_cb(char *p_cName, int iNameLen, char *p_cValue, int iValueLen, int iValueType,
                                     void *p_CBData);

CASE(json_parser, json_get_object)
{
    char *json_str, *pos;

    pos = json_get_object(JOBJECT, NULL, NULL);
    ASSERT_NULL(pos);

    json_str = " {\"code\":200}";
    pos = json_get_object(JOBJECT, json_str, json_str + 1);
    ASSERT_NULL(pos);

    pos = json_get_object(JOBJECT, json_str, json_str + strlen(json_str));
    ASSERT_EQ(*pos, '{');

    json_str = "[\"name\",\"address\",\"phont\"]";
    pos = json_get_object(JARRAY,  json_str, json_str + strlen(json_str));
    ASSERT_EQ(*pos, '[');
}

CASE(json_parser, json_get_next_object)
{
    char *json_str;
    char *key, *val;
    int key_len, val_len, val_type;

    json_str = "{code:200}";
    json_get_next_object(JOBJECT, json_str, json_str + strlen(json_str), &key, &key_len, \
                         &val, &val_len, &val_type);
    ASSERT_FALSE(key && key_len && val && val_len);

    json_str = "{\"code:200}";
    json_get_next_object(JOBJECT, json_str, json_str + strlen(json_str), &key, &key_len, \
                         &val, &val_len, &val_type);
    ASSERT_FALSE(key && key_len && val && val_len);

    json_str = "{\"code\":200}";
    json_get_next_object(JOBJECT, json_str, json_str + strlen(json_str), &key, &key_len, \
                         &val, &val_len, &val_type);

    ASSERT_NSTR_EQ(key, "code", key_len);
    ASSERT_NSTR_EQ(val, "200", val_len);
    ASSERT_EQ(val_type, JNUMBER);

    json_str = "{\"code\":\"OK\"}";
    json_get_next_object(JOBJECT, json_str, json_str + strlen(json_str), &key, &key_len, \
                         &val, &val_len, &val_type);
    ASSERT_NSTR_EQ(key, "code", key_len);
    ASSERT_NSTR_EQ(val, "OK", val_len);
    ASSERT_EQ(val_type, JSTRING);

    json_str = "{\"codec\":{\"method\":\"sha1\"}}";
    json_get_next_object(JOBJECT, json_str, json_str + strlen(json_str),  &key, &key_len, \
                         &val, &val_len, &val_type);
    ASSERT_NSTR_EQ(key, "codec", key_len);
    ASSERT_NSTR_EQ(val, "{\"method\":\"sha1\"}", val_len);
    ASSERT_EQ(val_type, JOBJECT);

    json_str = "{\"codec\":{\"method\":\"sha1\"}}";
    json_get_next_object(JOBJECT, json_str, json_str + strlen("{\"codec\":{\"method\":\"sha1\""),  &key, &key_len, \
                         &val, &val_len, &val_type);
    ASSERT_FALSE(key && key_len && val && val_len);

    json_str = "{\"code\":[200,404]}";
    json_get_next_object(JOBJECT, json_str, json_str + strlen(json_str), &key, &key_len, \
                         &val, &val_len, &val_type);
    ASSERT_NSTR_EQ(key, "code", key_len);
    ASSERT_NSTR_EQ(val, "[200,404]", val_len);
    ASSERT_EQ(val_type, JARRAY);

    json_str = "{\"code\":[[200,404]]}";
    json_get_next_object(JOBJECT, json_str, json_str + strlen(json_str), &key, &key_len, \
                         &val, &val_len, &val_type);
    ASSERT_NSTR_EQ(key, "code", key_len);
    ASSERT_NSTR_EQ(val, "[[200,404]]", val_len);
    ASSERT_EQ(val_type, JARRAY);

    json_str = "{\"ret\":TRUE}";
    json_get_next_object(JOBJECT, json_str, json_str + strlen(json_str) - 2, &key, &key_len, \
                         &val, &val_len, &val_type);
    ASSERT_FALSE(key && key_len && val && val_len);

    json_str = "{\"ret\":TRUE}";
    json_get_next_object(JOBJECT, json_str, json_str + strlen(json_str), &key, &key_len, \
                         &val, &val_len, &val_type);
    ASSERT_NSTR_EQ(key, "ret", key_len);
    ASSERT_NSTR_EQ(val, "TRUE", val_len);
    ASSERT_EQ(val_type, JBOOLEAN);

    json_str = "{\"ret\":FALSE}";
    json_get_next_object(JOBJECT, json_str, json_str + strlen(json_str), &key, &key_len, \
                         &val, &val_len, &val_type);
    ASSERT_NSTR_EQ(key, "ret", key_len);
    ASSERT_NSTR_EQ(val, "FALSE", val_len);
    ASSERT_EQ(val_type, JBOOLEAN);

    json_str = "[202, 404]";
    json_get_next_object(JARRAY, json_str, json_str + strlen(json_str), 0, 0, \
                         &val, &val_len, &val_type);
    ASSERT_NSTR_EQ(val, "[202, 404]", val_len);
    ASSERT_EQ(val_type, JARRAY);

}

CASE(json_parser, json_get_value_by_name)
{
    char *val;
    char *pjson = "{\"code\":\"OK\"}";
    int val_len, val_type;

    val = json_get_value_by_name(pjson, strlen(pjson), "code", &val_len, &val_type);
    ASSERT_NSTR_EQ(val, "OK", val_len);
    ASSERT_EQ(val_type, JSTRING);

    val = json_get_value_by_name(pjson, strlen(pjson), "unknown", &val_len, &val_type);
    ASSERT_NULL(val);
}

CASE(json_parser, json_get_value_by_name_len)
{
    char *val;
    char *pjson = "{\"code\":\"OK\",\"link\": \"/sys/q408EXte2fy/airCondition/thing/\"}";
    int val_len, val_type;

    val = json_get_value_by_name_len(pjson, strlen(pjson), "unknown", strlen("unknown"), &val_len, &val_type);
    ASSERT_NULL(val);

    val = json_get_value_by_name_len(pjson, strlen("{\"code\":\"OK\""), "link", strlen("link"), &val_len, &val_type);
    ASSERT_NULL(val);

    val = json_get_value_by_name_len(pjson, strlen("{\"code\":\"OK\""), "code", strlen("unknown"), &val_len, &val_type);
    ASSERT_NULL(val);

    val = json_get_value_by_name_len(pjson, strlen(pjson), "link", strlen("link"), &val_len, &val_type);
    ASSERT_NSTR_EQ(val, "/sys/q408EXte2fy/airCondition/thing/", val_len);
    ASSERT_EQ(val_type, JSTRING);

    val = json_get_value_by_name_len(pjson, strlen(pjson), "code", strlen("code"), &val_len, &val_type);
    ASSERT_NSTR_EQ(val, "OK", val_len);
    ASSERT_EQ(val_type, JSTRING);
}


CASE(json_parser, json_parse_name_value)
{
    char json_str[16] = {"code"};
    JSON_NV     stNV;

    memset(&stNV, 0, sizeof(stNV));
    stNV.pN = "code";
    stNV.nLen = strlen("code");
    char *pjson = "{\"code\":\"OK\"}";
    int ret = json_parse_name_value(NULL, strlen(pjson), json_get_value_by_name_cb, (void *)&stNV);
    ASSERT_EQ(ret, JSON_RESULT_ERR);

    ret = json_parse_name_value(json_str, strlen(json_str) + 2, json_get_value_by_name_cb, (void *)&stNV);
    ASSERT_EQ(ret, JSON_RESULT_ERR);
}

SUITE(json_parser) = {
    ADD_CASE(json_parser, json_get_object),
    ADD_CASE(json_parser, json_get_next_object),
    ADD_CASE(json_parser, json_parse_name_value),
    ADD_CASE(json_parser, json_get_value_by_name),
    ADD_CASE(json_parser, json_get_value_by_name_len),
    ADD_CASE_NULL
};

