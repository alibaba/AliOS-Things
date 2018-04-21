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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "lite-utils_internal.h"
#include "json_parser.h"

#define json_debug log_debug

char *json_get_object(int type, char *str, char *str_end)
{
    char *pos = NULL;
    char ch = (type == JOBJECT) ? '{' : '[';

    if (!str || !str_end) {
        return NULL;
    }

    while (str != NULL && *str != 0 && str < str_end) {
        if (*str == ' ') {
            str++;
            continue;
        }
        pos = (*str == ch) ? str : NULL;
        break;
    }
    return pos;
}

char *json_get_next_object(int type, char *str, char *str_end, char **key, int *key_len,
                           char **val, int *val_len, int *val_type)
{
    char    JsonMark[JTYPEMAX][2] = { { '\"', '\"' }, { '{', '}' }, { '[', ']' }, { '0', ' ' } };
    int     iMarkDepth = 0, iValueType = JNONE, iNameLen = 0, iValueLen = 0, iStringDepth = 0;
    char   *p_cName = 0, *p_cValue = 0, *p_cPos = str;

    if (type == JOBJECT) {
        /* Get Key */
        p_cPos = strchr(p_cPos, '\"');
        if (!p_cPos) {
            goto do_exit;
        }
        p_cName = ++p_cPos;
        p_cPos = strchr(p_cPos, '\"');
        if (!p_cPos) {
            goto do_exit;
        }
        iNameLen = p_cPos - p_cName;

        /* Get Value */
        p_cPos = strchr(p_cPos, ':');
    }
    while (p_cPos && *p_cPos && p_cPos < str_end) {
        if (*p_cPos == '\"') {
            iValueType = JSTRING;
            p_cValue = ++p_cPos;
            break;
        } else if (*p_cPos == '{') {
            iValueType = JOBJECT;
            p_cValue = p_cPos++;
            break;
        } else if (*p_cPos == '[') {
            iValueType = JARRAY;
            p_cValue = p_cPos++;
            break;
        } else if ((*p_cPos == '-') || (*p_cPos >= '0' && *p_cPos <= '9')) {
            iValueType = JNUMBER;
            p_cValue = p_cPos++;
            break;
        } else if (*p_cPos == 't' || *p_cPos == 'T' || *p_cPos == 'f' || *p_cPos == 'F') {
            iValueType = JBOOLEAN;
            p_cValue = p_cPos;
            break;
        }
        p_cPos++;
    }

    while (p_cPos && *p_cPos && p_cPos < str_end && iValueType > JNONE) {
        if (iValueType == JBOOLEAN) {
            int     len = strlen(p_cValue);

            if ((*p_cValue == 't' || *p_cValue == 'T') && len >= 4
                && (!strncmp(p_cValue, "true", 4)
                    || !strncmp(p_cValue, "TRUE", 4))) {
                iValueLen = 4;
                p_cPos = p_cValue + iValueLen;
                break;
            } else if ((*p_cValue == 'f' || *p_cValue == 'F') && len >= 5
                       && (!strncmp(p_cValue, "false", 5)
                           || !strncmp(p_cValue, "FALSE", 5))) {
                iValueLen = 5;
                p_cPos = p_cValue + iValueLen;
                break;
            }
        } else if (iValueType == JNUMBER) {
            if ((*p_cPos < '0' || *p_cPos > '9') && (*p_cPos != '.') && (*p_cPos != '+') \
                && (*p_cPos != '-') && ((*p_cPos != 'e')) && (*p_cPos != 'E')) {
                iValueLen = p_cPos - p_cValue;
                break;
            }
        } else if (iValueType == JSTRING) {
            if (*p_cPos == '\"') {
                iValueLen = p_cPos - p_cValue;
                break;
            }
        } else if (*p_cPos == JsonMark[iValueType][1]) {
            if (iStringDepth  == 0) {
                if (iMarkDepth == 0) {
                    iValueLen = p_cPos - p_cValue + 1;
                    p_cPos++;
                    break;
                } else {
                    iMarkDepth--;
                }
            }
        } else if (*p_cPos == JsonMark[iValueType][0]) {
            if (iStringDepth == 0) {
                iMarkDepth++;
            }
        } else if (*p_cPos == '\"') {
            if (iStringDepth) {
                iStringDepth = 0;
            } else {
                iStringDepth = 1;
            }
        }
        p_cPos++;
    }

    if (type == JOBJECT) {
        if ((p_cName + iNameLen) > str_end) {
            goto do_exit;
        }
        *key = p_cName;
        *key_len = iNameLen;
    }
    if ((p_cValue + iValueLen) > str_end) {
        goto do_exit;
    }

    *val = p_cValue;
    *val_len = iValueLen;
    *val_type = iValueType;
    if (iValueType == JSTRING) {
        return p_cValue + iValueLen + 1;
    } else {
        return p_cValue + iValueLen;
    }

do_exit:
    *val = NULL;
    *val_len = 0;
    *key = NULL;
    *key_len = 0;
    return NULL;
}

int json_parse_name_value(char *p_cJsonStr, int iStrLen, json_parse_cb pfnCB, void *p_CBData)
{
    char    *pos = 0, *key = 0, *val = 0;
    int     klen = 0, vlen = 0, vtype = 0;
    int     ret = JSON_RESULT_ERR;

    if (p_cJsonStr == NULL || iStrLen == 0 || pfnCB == NULL) {
        return ret;
    }

    json_object_for_each_kv(p_cJsonStr, iStrLen, pos, key, klen, val, vlen, vtype) {
        if (key && klen && val && vlen) {
            ret = JSON_RESULT_OK;
            if (JSON_PARSE_FINISH == pfnCB(key, klen, val, vlen, vtype, p_CBData)) {
                break;
            }
        }
    }

    return ret;
}

int json_get_value_by_name_cb(char *p_cName, int iNameLen, char *p_cValue, int iValueLen, int iValueType,
                              void *p_CBData)
{
    JSON_NV     *p_stNameValue = (JSON_NV *)p_CBData;

#ifdef JSON_DEBUG
    int         i;

    if (p_cName) {
        json_debug("Name:");
        for (i = 0; i < iNameLen; i++) {
            json_debug("%c", *(p_cName + i));
        }
    }

    if (p_cValue) {
        json_debug("Value:");
        for (i = 0; i < iValueLen; i++) {
            json_debug("%c", *(p_cValue + i));
        }
    }
#endif

    if ((iNameLen == p_stNameValue->nLen) && !strncmp(p_cName, p_stNameValue->pN, p_stNameValue->nLen)) {
        p_stNameValue->pV = p_cValue;
        p_stNameValue->vLen = iValueLen;
        p_stNameValue->vType = iValueType;
        return JSON_PARSE_FINISH;
    } else {
        return JSON_PARSE_OK;
    }
}

char *json_get_value_by_name(char *p_cJsonStr, int iStrLen, char *p_cName, int *p_iValueLen, int *p_iValueType)
{
    JSON_NV     stNV;

    memset(&stNV, 0, sizeof(stNV));
    stNV.pN = p_cName;
    stNV.nLen = strlen(p_cName);
    if (JSON_RESULT_OK == json_parse_name_value(p_cJsonStr, iStrLen, json_get_value_by_name_cb, (void *)&stNV)) {
        if (p_iValueLen) {
            *p_iValueLen = stNV.vLen;
        }
        if (p_iValueType) {
            *p_iValueType = stNV.vType;
        }
    }
    return stNV.pV;
}

char *json_get_value_by_name_len(char *p_cJsonStr, int iStrLen, char *p_cName, int p_cNameLen, int *p_iValueLen,
                                 int *p_iValueType)
{
    JSON_NV     stNV;

    memset(&stNV, 0, sizeof(stNV));
    stNV.pN = p_cName;
    stNV.nLen = p_cNameLen;
    if (JSON_RESULT_OK == json_parse_name_value(p_cJsonStr, iStrLen, json_get_value_by_name_cb, (void *)&stNV)) {
        if (p_iValueLen) {
            *p_iValueLen = stNV.vLen;
        }
        if (p_iValueType) {
            *p_iValueType = stNV.vType;
        }
    }
    return stNV.pV;
}
