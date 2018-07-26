#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "json.h"

typedef struct {
    AJsonObjType type;

    int   t_len;
    char *t;
} Token;

static int isdigit(int c)
{
    switch (c) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return 1;
    }

    return 0;
}

inline static int isdot(int c)
{
    return c == '.';
}

int AJson_ReadNum(Token *token, char *buf, int len, int *parsed_len)
{
    int nb_dot = 0;

    int i;
    for (i = 0; i < len; i++) {
        char c = buf[i];
        if (!isdigit(c) && !isdot(c))
            break;

        if (isdot(c))
            nb_dot++;
    }

    if (i == len)
        return -1;

    if (nb_dot > 1)
        return -1;

    memset(token, 0, sizeof(Token));
    token->type = nb_dot ? DOUBLE : INT;

    token->t_len = i;
    token->t = buf;

    *parsed_len = i;

    return 0;
}

int AJson_ReadString(Token *token, char *buf, int len, int *parsed_len)
{
    if (!token || !buf || len < 2)
        return -1;

    if (buf[0] != '"')
        return -1;  /* string must start with '"' */

    int i;
    for (i = 1; i < len; i++) {
        char c = buf[i];
        if (c == '\\') {
            i++;    /* skip next char */
            continue;
        }

        if (c == '"')
            break;
    }

    if (i >= len)
        return -1;

    memset(token, 0, sizeof(Token));
    token->type = STRING;
    token->t = buf + 1;
    token->t_len = i - 1;

    if (parsed_len)
        *parsed_len = i + 1;

    return 0;
}

int AJson_ReadObject(Token *token, char *buf, int len, int *parsed_len)
{
    if (!token || !buf || len < 2)
        return -1;

    if (buf[0] != '{')
        return -1;  /* json object must start with '{' */

    int nesting = 0;

    int i;
    for (i = 0; i < len; i++) {
        char c = buf[i];
        switch (c) {
        case '\\':
            i++;    /* skip next char */
            printf("skip next char: %c\n", buf[i]);
            break;
        case '{':
            nesting++;
            break;
        case '}':
            nesting--;
            break;
        }

        if (nesting == 0)
            break;
    }

    if (i >= len)
        return -1;

    memset(token, 0, sizeof(Token));
    token->type = OBJECT;
    token->t = buf;
    token->t_len = i + 1;

    if (parsed_len)
        *parsed_len = i + 1;

    return 0;
}

int AJson_ReadArray(Token *token, char *buf, int len, int *parsed_len)
{
    if (!token || !buf || len < 2)
        return -1;

    if (buf[0] != '[')
        return -1;  /* json object must start with '{' */

    int nesting = 0;

    int i;
    for (i = 0; i < len; i++) {
        char c = buf[i];
        switch (c) {
        case '\\':
            i++;    /* skip next char */
            printf("skip next char: %c\n", buf[i]);
            break;
        case '[':
            nesting++;
            break;
        case ']':
            nesting--;
            break;
        }

        if (nesting == 0)
            break;
    }

    if (i >= len)
        return -1;

    memset(token, 0, sizeof(Token));
    token->type = ARRAY;
    token->t = buf;
    token->t_len = i + 1;

    if (parsed_len)
        *parsed_len = i + 1;

    return 0;
}

int AJson_ReadBoolean(Token *token, char *buf, int len, int *parsed_len)
{
    if (!token || !buf || len < 2)
        return -1;

    int is_true = 0;

    if (len >= 4 && strncmp(buf, "true", 4) == 0)
        is_true = 1;
    else if (len >= 5 && strncmp(buf, "false", 5) == 0)
        is_true = 0;
    else
        return -1;

    memset(token, 0, sizeof(Token));
    token->type = BOOLEAN;
    token->t = buf;
    token->t_len = is_true ? 4 : 5;

    if (parsed_len)
        *parsed_len = token->t_len;

    return 0;
}

static int AJson_ReadToken(Token *token, AJsonObjType type, char *js, int len, int *parsed_len)
{
    int ret = -1;

    switch (type) {
    case INT:
        ret = AJson_ReadNum(token, js, len, parsed_len);
        break;
    case BOOLEAN:
        ret = AJson_ReadBoolean(token, js, len, parsed_len);
        break;
    case ARRAY:
        ret = AJson_ReadArray(token, js, len, parsed_len);
        break;
    case OBJECT:
        ret = AJson_ReadObject(token, js, len, parsed_len);
        break;
    case STRING:
        ret = AJson_ReadString(token, js, len, parsed_len);
        break;
    default:
        break;
    }

    return ret;
}

static int AJson_ReadKey(char *js, int len, char **key, int *key_len, int *parsed_len)
{
    Token token;
    if (AJson_ReadString(&token, js, len, parsed_len) < 0)
        return -1;

    *key = token.t;
    *key_len = token.t_len;

    return 0;
}

static char *skip_spaces(char *buf, int len)
{
    char *end = buf + len;
    while (buf <= end && (*buf == ' ' || *buf == '\t' || *buf == '\r' || *buf == '\n'))
        buf++;
    return buf;
}

static AJsonObjType guess_object_type(char c)
{
    switch (c) {
    case '"':
        return STRING;
    case '{':
        return OBJECT;
    case '[':
        return ARRAY;
    case 't':
    case 'f':
        return BOOLEAN;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return INT;
    }

    return UNKNOWN;
}

int AJson_Parse(AJsonObject *root, char *js, int len)
{
    char *ptr = js;
    char *end = js + len;

    ptr = skip_spaces(ptr, len);
    if (ptr > end)
        return -1;

    root->type = guess_object_type(*ptr);
    if (root->type != ARRAY && root->type != OBJECT)
        return -1;

    root->val_len = end - ptr;
    root->val = ptr;

    return 0;
}

int AJson_GetInt(AJsonObject *obj, int *val)
{
    if (obj->type != INT)
        return -1;
    *val = atoi(obj->val);
    return 0;
}

int AJson_GetDouble(AJsonObject *obj, double *val)
{
    if (obj->type != DOUBLE && obj->type != INT)
        return -1;
    *val = atof(obj->val);
    return 0;
}

int AJson_GetBoolean(AJsonObject *obj, int *val)
{
    if (obj->type != BOOLEAN)
        return -1;

    if (strncmp(obj->val, "true", obj->val_len) == 0)
        *val = 1;
    else if (strncmp(obj->val, "false", obj->val_len) == 0)
        *val = 0;
    else
        return -1;

    return 0;
}

int AJson_GetString(AJsonObject *obj, char **str, int *str_len)
{
#if 0
    if (obj->type != STRING)
        return -1;
#endif
    *str = obj->val;
    *str_len = obj->val_len;
    return 0;
}

int AJson_CopyString(AJsonObject *obj, char *buf, int len)
{
    if (obj->type != STRING)
        return -1;

    if (len < obj->val_len + 1)
        return -1;

    strncpy(buf, obj->val, obj->val_len);
    buf[obj->val_len] = '\0';

    return 0;
}

static AJsonObject *AJson_ReadOneObject(AJsonObject *root, AJsonObject *tmp)
{
    char *ptr = root->val;
    char *end = root->val + root->val_len;

    char *key = NULL;
    int key_len = 0;
    int parsed_len = 0;

    ptr = skip_spaces(ptr, end - ptr);
    if (ptr > end)
        return NULL;

    if (AJson_ReadKey(ptr, end - ptr, &key, &key_len, &parsed_len) < 0)
        return NULL;

    ptr += parsed_len;
    if (ptr > end)
        return NULL;

    ptr = skip_spaces(ptr, end - ptr);
    if (ptr > end)
        return NULL;

    if (*ptr != ':')
        return NULL;
    ptr++;

    if (ptr > end)
        return NULL;

    ptr = skip_spaces(ptr, end - ptr);
    if (ptr > end)
        return NULL;

    AJsonObjType type = guess_object_type(*ptr);
    if (type == UNKNOWN)
        return NULL;

    Token token;
    if (AJson_ReadToken(&token, type, ptr, end - ptr, &parsed_len) < 0)
        return NULL;

    ptr += parsed_len;
    if (ptr > end)
        return NULL;

    memset(tmp, 0, sizeof(AJsonObject));
    tmp->type = token.type;
    tmp->key_len = key_len;
    tmp->key = key;
    tmp->val_len = token.t_len;
    tmp->val = token.t;

    root->val = ptr;
    root->val_len = end - ptr;

    return tmp;
}

static AJsonObject *AJson_ReadOneArrayItem(AJsonObject *arr, AJsonObject *tmp)
{
    char *ptr = arr->val;
    char *end = arr->val + arr->val_len;

    int parsed_len = 0;

    ptr = skip_spaces(ptr, end - ptr);
    if (ptr > end)
        return NULL;

    AJsonObjType type = guess_object_type(*ptr);
    if (type == UNKNOWN)
        return NULL;

    Token token;
    if (AJson_ReadToken(&token, type, ptr, end - ptr, &parsed_len) < 0)
        return NULL;
    ptr += parsed_len;

    memset(tmp, 0, sizeof(AJsonObject));
    tmp->type = token.type;
    tmp->val_len = token.t_len;
    tmp->val = token.t;

    arr->val = ptr;
    arr->val_len = end - ptr;

    return tmp;
}

AJsonObject *AJson_First(AJsonObject *root, AJsonObject *tmp)
{
    char *ptr = root->val;
    char *end = root->val + root->val_len;

    ptr = skip_spaces(ptr, end - ptr);
    if (ptr > end)
        return NULL;

    AJsonObjType type = guess_object_type(*ptr);
    if (type != ARRAY && type != OBJECT)
        return NULL;

    ptr++;  /* skip '[' or '{' */
    if (ptr > end)
        return NULL;

    root->val = ptr;
    root->val_len = end - ptr;

    if (type == ARRAY)
        return AJson_ReadOneArrayItem(root, tmp);

    return AJson_ReadOneObject(root, tmp);
}

AJsonObject *AJson_Next(AJsonObject *root, AJsonObject *tmp)
{
    char *ptr = root->val;
    char *end = root->val + root->val_len;

    ptr = skip_spaces(ptr, end - ptr);
    if (ptr > end)
        return NULL;

    if (((root->type == OBJECT) && *ptr == '}') ||
        ((root->type == ARRAY)  && *ptr == ']')) {
        root->val = ++ptr;
        root->val_len = end - ptr;
        return NULL;   /* end of array */
    }

    if (*ptr != ',')
        return NULL;
    ptr++;

    root->val = ptr;
    root->val_len = end - ptr;

    if (root->type == ARRAY)
        return AJson_ReadOneArrayItem(root, tmp);
    return AJson_ReadOneObject(root, tmp);
}
