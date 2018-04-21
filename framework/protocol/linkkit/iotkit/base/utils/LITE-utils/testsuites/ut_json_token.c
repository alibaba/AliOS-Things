#include "lite-utils_internal.h"
#include "json_parser.h"
#include "cut.h"

const char *module_name = "ut_json_token";

#define UNITTEST_JSON_ARRAY_SAMPLE \
"{" \
    "\"empty_str\": [2]," \
    "\"empty\": []," \
    "\"double_str\": 1.8321278982," \
	"\"neg_double_str\": -1.8321278982," \
    "\"exp_double_str1\": 0.268E+4," \
    "\"exp_double_str2\": 0.268e-4," \
    "\"link\":\"/sys/q408EXte2fy/airCondition/thing/\"," \
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

extern int contain_arr(const char *src, int src_len, const char **arr_pre);

CASE(json_token, contain_arr)
{
    const char *arr_pre = NULL;
    char *str = NULL;
    int ret = contain_arr(NULL, 0, &arr_pre);
    ASSERT_EQ(-1, ret);

    ret = contain_arr("[]", 0, NULL);
    ASSERT_EQ(-1, ret);

    ret = contain_arr("[[]", strlen("[[]"), &arr_pre);
    ASSERT_EQ(-1, ret);

    ret = contain_arr("][", strlen("]["), &arr_pre);
    ASSERT_EQ(-1, ret);

    ret = contain_arr("[]", strlen("[]"), &arr_pre);
    ASSERT_EQ(-1, ret);

    ret = contain_arr("[1]name", strlen("[1]name"), &arr_pre);
    ASSERT_EQ(-1, ret);

    ret = contain_arr("[1]name[", strlen("[1]name["), &arr_pre);
    ASSERT_EQ(-1, ret);

    ret = contain_arr("[1]name[]", strlen("[1]name[]"), &arr_pre);
    ASSERT_EQ(-1, ret);

    ret = contain_arr("[1]name[1]", strlen("[1]name[1]"), &arr_pre);
    ASSERT_EQ(-1, ret);

    str = "[1]";
    ret = contain_arr(str, strlen(str), &arr_pre);
    log_info("%s contain arr:%s", str, arr_pre);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(str, arr_pre);

    str = "name[1]";
    ret = contain_arr(str, strlen(str), &arr_pre);
    log_info("%s contain arr:%s", str, arr_pre);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(str + 4, arr_pre);

    str = "name[1][1]";
    ret = contain_arr(str, strlen(str), &arr_pre);
    log_info("%s contain arr:%s", str, arr_pre);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(str + 4, arr_pre);
}


CASE(json_token, json_value_of)
{
    char *key;
    char *val = LITE_json_value_of(NULL, UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    ASSERT_NULL(val);

    val = LITE_json_value_of("events", NULL, MEM_MAGIC, module_name);
    ASSERT_NULL(val);

    val = LITE_json_value_of("unknown.profile", UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    ASSERT_NULL(val);

    val = LITE_json_value_of("events[1]", UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    ASSERT_NULL(val);

    key = "double_str";
    val = LITE_json_value_of(key, UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    log_info("Get `%s` from `UNITTEST_JSON_ARRAY_SAMPLE`: `%s`", key, val);
    ASSERT_NOT_NULL(val);
    ASSERT_STR_EQ(val, "1.8321278982");
    LITE_free(val);

    key = "neg_double_str";
    val = LITE_json_value_of(key, UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    log_info("Get `%s` from `UNITTEST_JSON_ARRAY_SAMPLE`: `%s`", key, val);
    ASSERT_NOT_NULL(val);
    ASSERT_STR_EQ(val, "-1.8321278982");
    LITE_free(val);

    key = "exp_double_str1";
    val = LITE_json_value_of(key, UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    log_info("Get `%s` from `UNITTEST_JSON_ARRAY_SAMPLE`: `%s`", key, val);
    ASSERT_NOT_NULL(val);
    ASSERT_STR_EQ(val, "0.268E+4");
    LITE_free(val);

    key = "exp_double_str2";
    val = LITE_json_value_of(key, UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    log_info("Get `%s` from `UNITTEST_JSON_ARRAY_SAMPLE`: `%s`", key, val);
    ASSERT_NOT_NULL(val);
    ASSERT_STR_EQ(val, "0.268e-4");
    LITE_free(val);

    key = "link";
    val = LITE_json_value_of(key, UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    log_info("Get `%s` from `UNITTEST_JSON_ARRAY_SAMPLE`: `%s`", key, val);
    ASSERT_NOT_NULL(val);
    ASSERT_STR_EQ(val, "/sys/q408EXte2fy/airCondition/thing/");
    LITE_free(val);

    key = "profile.productKey";
    val = LITE_json_value_of(key, UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    log_info("Get `%s` from `UNITTEST_JSON_ARRAY_SAMPLE`: `%s`", key, val);
    ASSERT_NOT_NULL(val);
    ASSERT_STR_EQ(val, "q408EXte2fy");
    LITE_free(val);

}

CASE(json_token, json_keys_release)
{
    list_head_t    *key_list = NULL;
    key_list = LITE_json_keys_of(UNITTEST_JSON_ARRAY_SAMPLE, "", MEM_MAGIC, module_name);
    LITE_json_keys_release(key_list);

    LITE_json_keys_release(NULL);
}

CASE(json_token, json_keys_of)
{
    list_head_t    *key_list = NULL;
    key_list = LITE_json_keys_of(UNITTEST_JSON_ARRAY_SAMPLE, NULL, MEM_MAGIC, module_name);
    ASSERT_NULL(key_list);

    key_list = LITE_json_keys_of(NULL, "", MEM_MAGIC, module_name);
    ASSERT_NULL(key_list);
    LITE_json_keys_release(key_list);

    key_list = LITE_json_keys_of(UNITTEST_JSON_ARRAY_SAMPLE, "", MEM_MAGIC, module_name);
    ASSERT_NOT_NULL(key_list);
    LITE_json_keys_release(key_list);

    key_list = LITE_json_keys_of(UNITTEST_JSON_ARRAY_SAMPLE, "");
    ASSERT_NOT_NULL(key_list);
    LITE_json_keys_release(key_list);
}

CASE(json_token, json_value_of_ext)
{
    char *json_str;
    char *key;
    char *val = LITE_json_value_of_ext(NULL, NULL, MEM_MAGIC, module_name);
    ASSERT_NULL(val);

    val = LITE_json_value_of_ext(NULL, UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    ASSERT_NULL(val);

    val = LITE_json_value_of_ext("events", NULL, MEM_MAGIC, module_name);
    ASSERT_NULL(val);

    val = LITE_json_value_of_ext("events.errkey", UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    ASSERT_NULL(val);

    val = LITE_json_value_of_ext("errkey.name", UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    ASSERT_NULL(val);

    val = LITE_json_value_of_ext("events[7]", UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    ASSERT_NULL(val);

    val = LITE_json_value_of_ext("events[7].name", UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    ASSERT_NULL(val);

    val = LITE_json_value_of_ext("events[a]", UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    ASSERT_NULL(val);

    val = LITE_json_value_of_ext("empty[1]", UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    ASSERT_NULL(val);

    key = "profile";
    val = LITE_json_value_of_ext(key, UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    log_info("Get `%s` from `UNITTEST_JSON_ARRAY_SAMPLE`: `%s`", key, val);
    ASSERT_STR_EQ(val, "{\"productKey\": \"q408EXte2fy\",\"deviceName\": \"airCondition\"}");
    LITE_free(val);

    key = "profile.productKey";
    val = LITE_json_value_of_ext(key, UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    log_info("Get `%s` from `UNITTEST_JSON_ARRAY_SAMPLE`: `%s`", key, val);
    ASSERT_STR_EQ(val, "q408EXte2fy");
    LITE_free(val);

    key = "events[1][1]";
    val = LITE_json_value_of_ext(key, UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    log_info("Get `%s` from `UNITTEST_JSON_ARRAY_SAMPLE`: `%s`", key, val);
    ASSERT_STR_EQ(val, "country");
    LITE_free(val);

    key = "events[1][2][1]";
    val = LITE_json_value_of_ext(key, UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    log_info("Get `%s` from `UNITTEST_JSON_ARRAY_SAMPLE`: `%s`", key, val);
    ASSERT_STR_EQ(val, "provice");
    LITE_free(val);

    key = "events[4].outputData[1].dataType.specs[2].dataType.specs.2";
    val = LITE_json_value_of_ext(key, UNITTEST_JSON_ARRAY_SAMPLE, MEM_MAGIC, module_name);
    log_info("Get `%s` from `UNITTEST_JSON_ARRAY_SAMPLE`: `%s`", key, val);
    ASSERT_STR_EQ(val, "three");
    LITE_free(val);

    json_str = "[\"country\",[{\"provice\":\"Zhejiang\"},\"city\"],[\"name\",\"address\",\"phone\"]]";
    key = "[1]";
    val = LITE_json_value_of_ext(key, json_str, MEM_MAGIC, module_name);
    log_info("Get `%s` from `%s`: `%s`", key, json_str, val);
    ASSERT_STR_EQ(val, "country");
    LITE_free(val);

    key = "[2]";
    val = LITE_json_value_of_ext(key, json_str, MEM_MAGIC, module_name);
    log_info("Get `%s` from `%s`: `%s`", key, json_str, val);
    ASSERT_STR_EQ(val, "[{\"provice\":\"Zhejiang\"},\"city\"]");
    LITE_free(val);

    key = "[2][1]";
    val = LITE_json_value_of_ext(key, json_str, MEM_MAGIC, module_name);
    log_info("Get `%s` from `%s`: `%s`", key, json_str, val);
    ASSERT_STR_EQ(val, "{\"provice\":\"Zhejiang\"}");
    LITE_free(val);

    key = "[2][1].provice";
    val = LITE_json_value_of_ext(key, json_str, MEM_MAGIC, module_name);
    log_info("Get `%s` from `%s`: `%s`", key, json_str, val);
    ASSERT_STR_EQ(val, "Zhejiang");
    LITE_free(val);

    key = "[2][2]";
    val = LITE_json_value_of_ext(key, json_str, MEM_MAGIC, module_name);
    log_info("Get `%s` from `%s`: `%s`", key, json_str, val);
    ASSERT_STR_EQ(val, "city");
    LITE_free(val);
}


CASE(json_token, json_value_of_ext2)
{
    int val_len = 0;
    char *json_str = NULL;
    char *key = NULL;
    char *val = NULL;

    val = LITE_json_value_of_ext2("events", NULL, 0, NULL);
    ASSERT_NULL(val);

    key = "profile";
    val = LITE_json_value_of_ext2(key, UNITTEST_JSON_ARRAY_SAMPLE, strlen(UNITTEST_JSON_ARRAY_SAMPLE), &val_len);
    log_info("Get `%s` from `UNITTEST_JSON_ARRAY_SAMPLE`: `%.*s`", key, val_len, val);
    ASSERT_NSTR_EQ(val, "{\"productKey\": \"q408EXte2fy\",\"deviceName\": \"airCondition\"}", val_len);

    json_str = "[\"country\",[{\"provice\":\"Zhejiang\"},\"city\"],[\"name\",\"address\",\"phone\"]]";
    key = "[1]";
    val = LITE_json_value_of_ext2(key, json_str, strlen(json_str), &val_len);
    log_info("Get `%s` from `%s`: `%.*s`", key, json_str, val_len, val);
    ASSERT_NSTR_EQ(val, "country", val_len);

    key = "[2]";
    val = LITE_json_value_of_ext2(key, json_str, strlen(json_str), &val_len);
    log_info("Get `%s` from `%s`: `%.*s`", key, json_str, val_len, val);
    ASSERT_NSTR_EQ(val, "[{\"provice\":\"Zhejiang\"},\"city\"]", val_len);

    key = "[2][1]";
    val = LITE_json_value_of_ext2(key, json_str, strlen(json_str), &val_len);
    log_info("Get `%s` from `%s`: `%.*s`", key, json_str, val_len, val);
    ASSERT_NSTR_EQ(val, "{\"provice\":\"Zhejiang\"}", val_len);

    key = "[2][1].provice";
    val = LITE_json_value_of_ext2(key, json_str, strlen(json_str), &val_len);
    log_info("Get `%s` from `%s`: `%.*s`", key, json_str, val_len, val);
    ASSERT_NSTR_EQ(val, "Zhejiang", val_len);

    key = "[2][2]";
    val = LITE_json_value_of_ext2(key, json_str, strlen(json_str), &val_len);
    log_info("Get `%s` from `%s`: `%.*s`", key, json_str, val_len, val);
    ASSERT_NSTR_EQ(val, "city", val_len);

    json_str = "{\"method\":\"thing.service.property.set\",\"id\":\"14450\",\"params\":{\"PropertyCharacter\":\"dwadw}sdad\"}}";
    key = "params";
    val = LITE_json_value_of_ext2(key, json_str, strlen(json_str), &val_len);
    log_info("Get `%s` from `%s`: `%.*s`", key, json_str, val_len, val);
    ASSERT_NSTR_EQ(val, "{\"PropertyCharacter\":\"dwadw}sdad\"}", val_len);

    json_str = "{\"method\":\"thing.service.property.set\",\"id\":\"14450\",\"params\":{\"PropertyCharacter\":\"dwadw{sdad\"}}";
    key = "params";
    val = LITE_json_value_of_ext2(key, json_str, strlen(json_str), &val_len);
    log_info("Get `%s` from `%s`: `%.*s`", key, json_str, val_len, val);
    ASSERT_NSTR_EQ(val, "{\"PropertyCharacter\":\"dwadw{sdad\"}", val_len);

    json_str = "{\"method\":\"thing.service.property.set\",\"id\":\"14450\",\"params\":{\"PropertyCharacter\":\"dwadw{sdad}sdad\"}}";
    key = "params";
    val = LITE_json_value_of_ext2(key, json_str, strlen(json_str), &val_len);
    log_info("Get `%s` from `%s`: `%.*s`", key, json_str, val_len, val);
    ASSERT_NSTR_EQ(val, "{\"PropertyCharacter\":\"dwadw{sdad}sdad\"}", val_len);
}


CASE(json_token, json_keys_of_ext)
{
    json_key_t     *pos = NULL;
    list_head_t *key_list = LITE_json_keys_of_ext(NULL, NULL, MEM_MAGIC, module_name);
    ASSERT_NULL(key_list);

    key_list = LITE_json_keys_of_ext(NULL, "", MEM_MAGIC, module_name);
    ASSERT_NULL(key_list);

    key_list = LITE_json_keys_of_ext(UNITTEST_JSON_ARRAY_SAMPLE, NULL, MEM_MAGIC, module_name);
    ASSERT_NULL(key_list);

    key_list = LITE_json_keys_of_ext(UNITTEST_JSON_ARRAY_SAMPLE, "", MEM_MAGIC, module_name);
    ASSERT_NOT_NULL(key_list);
    list_for_each_entry(pos, key_list, list, json_key_t) {
        if (pos->key) {
            log_info("`key`: `%s`", pos->key);
        }
    }
    LITE_json_keys_release(key_list);

    key_list = LITE_json_keys_of_ext(UNITTEST_JSON_ARRAY_SAMPLE, "");
    ASSERT_NOT_NULL(key_list);
    LITE_json_keys_release(key_list);

}

CASE(json_token, get_json_item_size)
{
    int arr_size = 0;
    char *json_str = "[{\"tpye\":\"alarm\"},\"country\",[\"provice\",\"city\"],[\"name\",\"address\",\"phone\"]]";

    arr_size = get_json_item_size(NULL, 0);
    ASSERT_EQ(arr_size, 0);

    arr_size = get_json_item_size("{}", strlen("{}"));
    ASSERT_EQ(arr_size, 0);

    arr_size = get_json_item_size("{\"tpye\":\"alarm\"}", strlen("{\"tpye\":\"alarm\"}"));
    ASSERT_EQ(arr_size, 1);

    arr_size = get_json_item_size(json_str, strlen(json_str));
    ASSERT_EQ(arr_size, 4);
}

SUITE(json_token) = {
    ADD_CASE(json_token, contain_arr),
    ADD_CASE(json_token, json_value_of),
    ADD_CASE(json_token, json_keys_release),
    ADD_CASE(json_token, json_keys_of),
    ADD_CASE(json_token, json_value_of_ext),
    ADD_CASE(json_token, json_value_of_ext2),
    ADD_CASE(json_token, json_keys_of_ext),
    ADD_CASE(json_token, get_json_item_size),
    ADD_CASE_NULL
};

