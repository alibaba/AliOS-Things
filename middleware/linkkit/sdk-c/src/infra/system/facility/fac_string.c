/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iotx_system_internal.h"

#define JSON_NEWLINE            "\r\n"
#define JSON_INDENT             "    "

#define JSON_PRINT_NEWSTR       HAL_Printf("%s", newstr);
#define JSON_PRINT_NEWLINE      \
    do { \
        HAL_Printf("%s", JSON_NEWLINE); \
        if (mark == '>' || mark == '<' || mark == ':') { \
            HAL_Printf("%c ", mark); \
        } \
    } while(0)

int iotx_facility_json_print(const char *str, int level, ...)
{
    int             length = 0;
    char            newstr[2];
    int             quoted  = 0;
    int             escaped = 0;
    int             indent  = 0;
    int             i = 0, j = 0;
    int             curr_level = LITE_get_loglevel();
    int             res = -1;
    lite_cjson_t    lite;
    va_list         ap;
    int             mark = ' ';

    newstr[0] = 0x00;
    newstr[1] = 0x00;

    if (curr_level < level) {
        return 1;
    }

    if (str == NULL || strlen(str) == 0) {
        return -1;
    }

    res = lite_cjson_parse(str, strlen(str), &lite);
    if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {
        return -2;
    }

    length = strlen(str);
    HAL_Printf("%s%s", "\033", lvl_color[level]);
    va_start(ap, level);
    mark = va_arg(ap, int);
    JSON_PRINT_NEWLINE;
    va_end(ap);

    for (i = 0 ; i < length ; i++) {
        char ch = str[i];
        switch (ch) {
            case '{':
            case '[':
                newstr[0] = ch;
                JSON_PRINT_NEWSTR;

                if (!quoted) {
                    JSON_PRINT_NEWLINE;

                    if (!(str[i + 1] == '}' || str[i + 1] == ']')) {
                        ++indent;

                        for (j = 0 ; j < indent ; j++) {
                            HAL_Printf("%s", JSON_INDENT);
                        }
                    }
                }

                break;

            case '}':
            case ']':
                if (!quoted) {
                    if ((i > 0) && (!(str[i - 1] == '{' || str[i - 1] == '['))) {
                        JSON_PRINT_NEWLINE;
                        --indent;

                        for (j = 0 ; j < indent ; j++) {
                            HAL_Printf("%s", JSON_INDENT);
                        }
                    } else if ((i > 0) && ((str[i - 1] == '[' && ch == ']') || (str[i - 1] == '{' && ch == '}'))) {
                        for (j = 0 ; j < indent ; j++) {
                            HAL_Printf("%s", JSON_INDENT);
                        }
                    }
                }

                newstr[0] = ch;
                JSON_PRINT_NEWSTR;

                break;

            case '"':
                newstr[0] = ch;
                JSON_PRINT_NEWSTR;
                escaped = 1;

                if (i > 0 && str[i - 1] == '\\') {
                    escaped = !escaped;
                }

                if (!escaped) {
                    quoted = !quoted;
                }

                break;

            case ',':
                newstr[0] = ch;
                JSON_PRINT_NEWSTR;
                if (!quoted) {
                    JSON_PRINT_NEWLINE;

                    for (j = 0 ; j < indent ; j++) {
                        HAL_Printf("%s", JSON_INDENT);
                    }
                }

                break;

            case ':':
                newstr[0] = ch;
                JSON_PRINT_NEWSTR;
                if (!quoted) {
                    HAL_Printf("%s", " ");
                }

                break;

            default:
                newstr[0] = ch;
                JSON_PRINT_NEWSTR;

                break;
        }
    }

    HAL_Printf("%s", JSON_NEWLINE JSON_NEWLINE);
    HAL_Printf("%s", "\033[0m");
    return 0;
}

