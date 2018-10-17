/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iotx_system_internal.h"

#define JSON_INDENTATION            "    "

int iotx_facility_format_json(const char *str, char *outstr, int outlen)
{
    int length = 0;
    char newstr[2];
    char *INDENT = JSON_INDENTATION;
    int quoted  = 0;
    int escaped = 0;
    int indent  = 0;
    int i = 0, j = 0;

    newstr[0] = 0x00;
    newstr[1] = 0x00;

    if (str == NULL || strlen(str) == 0) {
        return -1;
    }

    length = strlen(str);

    memset(outstr, 0, outlen);

    for (i = 0 ; i < length ; i++) {
        char ch = str[i];
        switch (ch) {
            case '{':
            case '[':
                newstr[0] = ch;
                strncat(outstr, newstr, outlen - strlen(outstr));
                if (!quoted) {
                    strncat(outstr, "\n", outlen - strlen(outstr));

                    if (!(str[i + 1] == '}' || str[i + 1] == ']')) {
                        ++indent;

                        for (j = 0 ; j < indent ; j++) {
                            strncat(outstr, INDENT, outlen - strlen(outstr));
                        }
                    }
                }

                break;

            case '}':
            case ']':
                if (!quoted) {
                    if ((i > 0) && (!(str[i - 1] == '{' || str[i - 1] == '['))) {
                        strncat(outstr, "\n", outlen - strlen(outstr));
                        --indent;

                        for (j = 0 ; j < indent ; j++) {
                            strncat(outstr, INDENT, outlen - strlen(outstr));
                        }
                    } else if ((i > 0) && ((str[i - 1] == '[' && ch == ']') || (str[i - 1] == '{' && ch == '}'))) {
                        for (j = 0 ; j < indent ; j++) {
                            strncat(outstr, INDENT, outlen - strlen(outstr));
                        }
                    }
                }

                newstr[0] = ch;
                strncat(outstr, newstr, outlen - strlen(outstr));

                break;

            case '"':
                newstr[0] = ch;
                strncat(outstr, newstr, outlen - strlen(outstr));
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
                strncat(outstr, newstr, outlen - strlen(outstr));
                if (!quoted) {
                    strncat(outstr, "\n", outlen - strlen(outstr));

                    for (j = 0 ; j < indent ; j++) {
                        strncat(outstr, INDENT, outlen - strlen(outstr));
                    }
                }

                break;

            case ':':
                newstr[0] = ch;
                strncat(outstr, newstr, outlen - strlen(outstr));
                if (!quoted) {
                    strncat(outstr, "", outlen - strlen(outstr));
                }

                break;

            default:
                newstr[0] = ch;
                strncat(outstr, newstr, outlen - strlen(outstr));

                break;
        }
    }

    return 0;
}
