#include "lite-utils_internal.h"
#include "cut.h"

static char *module_name = "ut_string_utils";

CASE(string, strdup)
{
    char *p = LITE_strdup(NULL, MEM_MAGIC, module_name);
    ASSERT_NULL(p);

    p = LITE_strdup("strdup case", MEM_MAGIC, module_name);
    ASSERT_STR_EQ(p, "strdup case");
    LITE_free(p);
}

CASE(string, hexbuf_convert)
{
    unsigned char       hexbuf[] = { 0x12, 0x34, 0x56, 0x78, 0xcd, 0xef, 0xab, 0x90 };
    char                hexstr[20] = { 0 };

    LITE_hexbuf_convert(hexbuf, hexstr, sizeof(hexbuf), 0);
    log_info("hexstr = '%s'", hexstr);
    ASSERT_STR_EQ(hexstr, "12345678cdefab90");

    LITE_hexbuf_convert(hexbuf, hexstr, sizeof(hexbuf), 1);
    log_info("hexstr = '%s'", hexstr);
    ASSERT_STR_EQ(hexstr, "12345678CDEFAB90");
}

CASE(string, hexstr_convert)
{
    unsigned char       hexbuf[] = { 0x12, 0x34, 0x56, 0x78, 0xcd, 0xef, 0xab, 0x90 };
    char                hexstr[20] = { 0 };
    unsigned char       outbuf[10] = { 0x0 };
    char                str_uppercase[] = {"1234567890abcdef"};

    LITE_hexbuf_convert(hexbuf, hexstr, sizeof(hexbuf), 1);
    LITE_hexstr_convert(hexstr, outbuf, strlen(hexstr));
    HEXDUMP_INFO(outbuf, sizeof(outbuf));

    ASSERT_EQ(outbuf[3], 0x78);

    memset(outbuf, 0, sizeof(outbuf));
    LITE_hexstr_convert(str_uppercase, outbuf, strlen(str_uppercase));
    HEXDUMP_INFO(outbuf, sizeof(outbuf));
    ASSERT_EQ(outbuf[7], 0xef);

    memset(outbuf, 0, sizeof(outbuf));
    LITE_hexstr_convert("1234567", outbuf, strlen("1234567"));
    ASSERT_EQ(outbuf[0], 0x00);

    memset(outbuf, 0, sizeof(outbuf));
    LITE_hexstr_convert("[]", outbuf, strlen("[]"));
    ASSERT_EQ(outbuf[0], 0x00);
}

CASE(string, replace_substr)
{
    char originalString[] = {"hello world"};
    LITE_replace_substr(originalString, "world", "sea");
    ASSERT_STR_EQ(originalString, "hello sea");
}

DATA(string)
{
    char           *fmt;
    char           *str;
};

SETUP(string)
{
    data->fmt = LITE_malloc(100, MEM_MAGIC, module_name);
    strcpy(data->fmt, "Integer: %d, String: '%s', Char: %c, Hex: %04x");
}

TEARDOWN(string)
{
    LITE_free(data->fmt);
    LITE_free(data->str);
}

CASEs(string, format)
{
    unsigned int len = strlen(data->fmt) + 3 + strlen("Hello") + 1 + 4;
    data->str = LITE_format_string(len, data->fmt, MEM_MAGIC, module_name, 100, "Hello", 'A', 640);
    log_info("(Length: %02d) '%s'", (int)strlen(data->str), data->str);

    ASSERT_EQ(49, strlen(data->str));
    ASSERT_STR_EQ(data->str, "Integer: 100, String: 'Hello', Char: A, Hex: 0280");
}

CASEs(string, format_nstring)
{
    data->str = LITE_format_nstring(10, data->fmt, MEM_MAGIC, module_name, 100, "Hello", 'A', 640);
    log_info("(Length: %02d) '%s'", (int)strlen(data->str), data->str);
    ASSERT_EQ(10, strlen(data->str));
    ASSERT_STR_EQ(data->str, "Integer: 1");
}


CASEs(string, format_nstring_ext)
{
    data->str = LITE_format_nstring(20, data->fmt, MEM_MAGIC, module_name, 100, "Hello", 'A', 640);
    log_info("(Length: %02d) '%s'", (int)strlen(data->str), data->str);
    ASSERT_EQ(20, strlen(data->str));
    ASSERT_STR_EQ(data->str, "Integer: 100, String");
}

CASE(string, always_failure)
{
    ASSERT_TRUE(0);
}

SUITE(string) = {
    ADD_CASE(string, strdup),
    ADD_CASE(string, hexbuf_convert),
    ADD_CASE(string, hexstr_convert),
    ADD_CASE(string, format),
    ADD_CASE(string, format_nstring),
    ADD_CASE(string, format_nstring_ext),
    ADD_CASE(string, always_failure),
    ADD_CASE(string, replace_substr),
    ADD_CASE_NULL
};

