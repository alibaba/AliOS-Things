/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "be_jse_utils.h"
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "be_jse.h"
#include "be_jse_api.h"
#include "be_utils.h"
#ifdef SUPPORT_NODE_MODELES
#include "cJSON.h"
#include "hal/system.h"
#endif

#ifdef JSE_IDE_DEBUG
#include "websocket.h"
#endif

#ifndef BE_JSE_SILENT
static char tmp_token[MAX_TOKEN_LENGTH] = {0};
void be_jse_save_tmp_token(const char *token) {
    if (token)
        strncpy(tmp_token, token, MAX_TOKEN_LENGTH - 1);
    else
        tmp_token[0] = 0;
}

const char *be_jse_get_tmp_token() { return tmp_token; }
#endif

#if 0
ALWAYS_INLINE bool is_whitespace(char ch) {
    return (ch == ' ') || (ch == '\t') || (ch == '\n') || (ch == '\r');
}

ALWAYS_INLINE bool is_numeric(char ch) { return (ch >= '0') && (ch <= '9'); }

ALWAYS_INLINE bool is_hex_decimal(char ch) {
    return ((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) ||
           ((ch >= 'A') && (ch <= 'F'));
}
ALWAYS_INLINE bool is_alpha(char ch) {
    return ((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z')) ||
           ch == '_';
}

#else

/* attribute flags */
#define FL_PRINT (0x01)
#define FL_SPACE (0x02)
#define FL_DIGIT (0x04)
#define FL_ALPHA (0x08)
#define FL_UPPER (0x10)
#define FL_LOWER (0x20)
#define FL_XDIGIT (0x40)

/* shorthand character attributes */
#define AT_PR (FL_PRINT)
#define AT_SP (FL_SPACE | FL_PRINT)
#define AT_DI (FL_DIGIT | FL_PRINT | FL_XDIGIT)
#define AT_AL (FL_ALPHA | FL_PRINT)
#define AT_UP (FL_UPPER | FL_ALPHA | FL_PRINT)
#define AT_LO (FL_LOWER | FL_ALPHA | FL_PRINT)
#define AT_UX (FL_UPPER | FL_ALPHA | FL_PRINT | FL_XDIGIT)
#define AT_LX (FL_LOWER | FL_ALPHA | FL_PRINT | FL_XDIGIT)

/* table of attributes for ascii characters */
static const unsigned char attr[] = {
    0,     0,     0,     0,     0,     0,     0,     0,     0,     AT_SP, AT_SP,
    AT_SP, AT_SP, AT_SP, 0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     AT_SP,
    AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR,
    AT_PR, AT_PR, AT_PR, AT_PR, AT_DI, AT_DI, AT_DI, AT_DI, AT_DI, AT_DI, AT_DI,
    AT_DI, AT_DI, AT_DI, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_UX,
    AT_UX, AT_UX, AT_UX, AT_UX, AT_UX, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP,
    AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP,
    AT_UP, AT_UP, AT_UP, AT_PR, AT_PR, AT_PR, AT_PR, AT_AL, AT_PR, AT_LX, AT_LX,
    AT_LX, AT_LX, AT_LX, AT_LX, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO,
    AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO,
    AT_LO, AT_LO, AT_PR, AT_PR, AT_PR, AT_PR, 0};

ALWAYS_INLINE bool is_whitespace(char ch) { return (attr[ch] & FL_SPACE) != 0; }

ALWAYS_INLINE bool is_numeric(char ch) { return (attr[ch] & FL_DIGIT) != 0; }

ALWAYS_INLINE bool is_hex_decimal(char ch) {
    return (attr[ch] & FL_XDIGIT) != 0;
}
ALWAYS_INLINE bool is_alpha(char ch) { return (attr[ch] & FL_ALPHA) != 0; }

#endif

#define BonePrefix "BoneEngine > "

NO_INLINE void be_jse_error(const char *message) {
    /* printf("ERROR: %s\n", message); */
    char *buf = calloc(1, 256);
    snprintf(buf, 255, "%sERROR: %s", BonePrefix, message);
    printf("%s\n", buf);
    fflush(stdout);
#ifdef JSE_IDE_DEBUG
    /* 通过websocket上报BoneEngine错误信息输出 */
    bone_websocket_send_frame("/ide/console", BE_LOG_LEVEL_ERROR, buf);
#endif
    free(buf);
}

NO_INLINE void be_jse_error_at(const char *message, be_jse_lexer_ctx_t *lex,
                               int tokenPos) {
    int line, col;
    lexer_dump_line_and_col(lex, tokenPos, &line, &col);
    be_jse_execute_error();
    /* printf("ERROR: %s at %d:%d current tolken=%s \n", message, line, col,
              lex->token); */
    char *buf = calloc(1, 256);
    snprintf(buf, 255, "%sERROR: %s at %d:%d, %s is not defined", BonePrefix,
             message, line, col, lex->token);
    printf("%s\n", buf);
    fflush(stdout);
#ifdef JSE_IDE_DEBUG
    /* 通过websocket上报BoneEngine错误信息输出 */
    bone_websocket_send_frame("/ide/console", BE_LOG_LEVEL_ERROR, buf);
#endif
    free(buf);
}

NO_INLINE void be_jse_warn(const char *message) {
    /* printf("WARNING: %s\n", message); */
    char *buf = calloc(1, 256);
    snprintf(buf, 255, "%sWARNING: %s", BonePrefix, message);
    printf("%s\n", buf);
    fflush(stdout);
    /* 通过websocket上报BoneEngine错误信息输出 */
#ifdef JSE_IDE_DEBUG
    bone_websocket_send_frame("/ide/console", BE_LOG_LEVEL_ERROR, buf);
#endif
    free(buf);
}

NO_INLINE void be_jse_warn_at(const char *message, be_jse_lexer_ctx_t *lex,
                              int tokenPos) {
    int line, col;
    lexer_dump_line_and_col(lex, tokenPos, &line, &col);
    be_jse_execute_error();
    /* printf("WARNING: %s at %d:%d current tolken=%s \n", message, line, col,
              lex->token); */
    char *buf = calloc(1, 256);
    snprintf(buf, 255, "%sWARNING: %s at %d:%d current tolken=%s", BonePrefix,
             message, line, col, lex->token);
    printf("%s\n", buf);
    fflush(stdout);
#ifdef JSE_IDE_DEBUG
    /* 通过websocket上报BoneEngine错误信息输出 */
    bone_websocket_send_frame("/ide/console", BE_LOG_LEVEL_ERROR, buf);
#endif
    free(buf);
}

NO_INLINE void be_jse_assert_fail(const char *file, int line) {
    be_jse_execute_error();
    /* printf("ASSERT FAIL AT %s:%d\n", file, line); */
    char *buf = calloc(1, 256);
    snprintf(buf, 255, "%sASSERT FAIL AT %s:%d", BonePrefix, file, line);
    printf("%s\n", buf);
    fflush(stdout);
#ifdef JSE_IDE_DEBUG
    /* 通过websocket上报BoneEngine错误信息输出 */
    bone_websocket_send_frame("/ide/console", BE_LOG_LEVEL_ERROR, buf);
#endif
    free(buf);
#if defined(USE_FREERTOS)

#else
    /* exit(1); */
    abort();
#endif
}

#if 0
static void itostr_extra(be_jse_int_t vals, char *str, bool signedVal,
                         unsigned int base) {
    be_jse_uint_t val;

    if (signedVal && vals < 0) {
        *(str++) = '-';
        val = (be_jse_uint_t)(-vals);
    } else {
        val = (be_jse_uint_t)vals;
    }

    be_jse_uint_t tmp = val;
    int digits = 1;
    while (tmp >= base) {
        digits++;
        tmp /= base;
    }

    int i;
    for (i = digits - 1; i >= 0; i--) {
        str[i] = itoch((int)(val % base));
        val /= base;
    }
    str[digits] = 0;
}

static void ftoa_bounded_extra(be_jse_float_t val, char *str, size_t len,
                               int radix, int fractionalDigits) {
    const be_jse_float_t stopAtError = 0.0000001;
    if (isnan(val))
        strncpy(str, "NaN", len);
    else if (!isfinite(val)) {
        if (val < 0)
            strncpy(str, "-Infinity", len);
        else
            strncpy(str, "Infinity", len);
    } else {
        if (val < 0) {
            if (--len <= 0) {
                *str = 0;
                return;
            }
            *(str++) = '-';
            val = -val;
        }

        if (((be_jse_int_t)(val + stopAtError)) == (1 + (be_jse_int_t)val))
            val = (be_jse_float_t)(1 + (be_jse_int_t)val);

        be_jse_float_t d = 1;
        while (d * radix <= val) d *= radix;
        while (d >= 1) {
            int v = (int)(val / d);
            val -= v * d;
            if (--len <= 0) {
                *str = 0;
                return;
            }
            *(str++) = itoch(v);
            d /= radix;
        }
#ifndef USE_NO_FLOATS
        if (((fractionalDigits < 0) && val > 0) || fractionalDigits > 0) {
            if (--len <= 0) {
                *str = 0;
                return;
            }
            *(str++) = '.';
            val *= radix;
            while (((fractionalDigits < 0) && (fractionalDigits > -12) &&
                    (val > stopAtError)) ||
                   (fractionalDigits > 0)) {
                int v =
                    (int)(val + ((fractionalDigits == 1) ? 0.4 :
                    0.00000001));
                val = (val - v) * radix;
                if (--len <= 0) {
                    *str = 0;
                    return;
                }
                if (v == radix) v = radix - 1;
                *(str++) = itoch(v);
                fractionalDigits--;
            }
        }
#endif

        *(str++) = 0;
    }
}

ALWAYS_INLINE void itostr(be_jse_int_t val, char *str, unsigned int base) {
    itostr_extra(val, str, true, base);
}

void ftoa_bounded(be_jse_float_t val, char *str, size_t len) {
    ftoa_bounded_extra(val, str, len, 10, -1);
}
#endif

static NO_INLINE int get_radix(const char **s, int forceRadix, bool *hasError) {
    int radix = 10;

    if (forceRadix > 36) {
        if (hasError) *hasError = true;
        return 0;
    }

    if (**s == '0') {
        radix = 8;
        (*s)++;

        if (**s == 'o' || **s == 'O') {
            radix = 8;
            if (forceRadix && forceRadix != 8 && forceRadix < 25) return 0;
            (*s)++;

        } else if (**s == 'x' || **s == 'X') {
            radix = 16;
            if (forceRadix && forceRadix != 16 && forceRadix < 34) return 0;
            (*s)++;

        } else if (**s == 'b' || **s == 'B') {
            radix = 2;
            if (forceRadix && forceRadix != 2 && forceRadix < 12)
                return 0;
            else
                (*s)++;
        } else if (!forceRadix) {
            const char *p;
            for (p = *s; *p; p++)
                if (*p == '.' || *p == '8' || *p == '9')
                    radix = 10;
                else if (*p < '0' || *p > '9')
                    break;
        }
    }
    if (forceRadix > 0 && forceRadix <= 36) radix = forceRadix;

    return radix;
}

int chtod(char ch) {
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    else if (ch >= 'a' && ch <= 'z')
        return 10 + ch - 'a';
    else if (ch >= 'A' && ch <= 'Z')
        return 10 + ch - 'A';
    else
        return -1;
}

static be_jse_float_t string_to_floatr(const char *s, int forceRadix) {
    while (is_whitespace(*s)) s++;

    bool isNegated = false;
    if (*s == '-') {
        isNegated = true;
        s++;
    } else if (*s == '+') {
        s++;
    }

    const char *numberStart = s;

    int radix = get_radix(&s, forceRadix, 0);
    if (!radix) return NAN;

    be_jse_float_t v = 0;
    be_jse_float_t mul = 0.1;

    while (*s) {
        int digit = chtod(*s);
        if (digit < 0 || digit >= radix) break;
        v = (v * radix) + digit;
        s++;
    }

    if (radix == 10) {
        if (*s == '.') {
            s++;

            while (*s) {
                if (*s >= '0' && *s <= '9')
                    v += mul * (*s - '0');
                else
                    break;
                mul /= 10;
                s++;
            }
        }

        if (*s == 'e' || *s == 'E') {
            s++;
            bool isENegated = false;
            if (*s == '-' || *s == '+') {
                isENegated = *s == '-';
                s++;
            }
            int e = 0;
            while (*s) {
                if (*s >= '0' && *s <= '9')
                    e = (e * 10) + (*s - '0');
                else
                    break;
                s++;
            }
            if (isENegated) e = -e;

            while (e > 0) {
                v *= 10;
                e--;
            }
            while (e < 0) {
                v /= 10;
                e++;
            }
        }
    }

    if (numberStart == s || (numberStart[0] == '.' && numberStart[1] == 0))
        return NAN;

    if (isNegated) return -v;
    return v;
}
static long long string_to_intr(const char *s, int forceRadix, bool *hasError) {
    while (is_whitespace(*s)) s++;

    bool isNegated = false;
    long long v = 0;
    if (*s == '-') {
        isNegated = true;
        s++;
    } else if (*s == '+') {
        s++;
    }

    const char *numberStart = s;

    int radix = get_radix(&s, forceRadix, hasError);
    if (!radix) return 0;

    while (*s) {
        int digit = chtod(*s);
        if (digit < 0 || digit >= radix) break;
        v = v * radix + digit;
        s++;
    }

    if (hasError) *hasError = s == numberStart;

    if (isNegated) return -v;
    return v;
}

be_jse_float_t string_to_float(const char *s) { return string_to_floatr(s, 0); }

long long string_to_int(const char *s) { return string_to_intr(s, 0, 0); }

#ifdef SUPPORT_NODE_MODELES

static int pathBack(char *path, int pos) {
    int i;
    for (i = pos - 2; i > 0; i--) {
        if (path[i] == '/') {
            path[i + 1] = 0;
            return i + 1;
        }
    }
    return i;
}

char *getFilePath(char *name) {
    int i;
    char *str = strdup(name);
    int pos = strlen(name);

    for (i = pos - 2; i > 0; i--) {
        if (str[i] == '/') {
            str[i + 1] = 0;
            break;
        }
    }
    return str;
}

char *getClearPath(char *path) {
    int len = strlen(path);
    int i;
    char *str = calloc(1, len + 1);
    int pos = 0;

    /* 拷贝 ./ */
    for (i = 0; i < 2; i++) {
        str[pos] = path[i];
        pos++;
    }

    for (i = 2; i < len;) {
        if (path[i] == '.') {
            /* ../ */
            if (path[i + 1] == '.' && path[i + 2] == '/') {
                i += 3;
                pos = pathBack(str, pos);
            }

            /* .. */
            else if (path[i + 1] == '.') {
                i += 2;
                pos = pathBack(str, pos);
            }

            /* ./ */
            else if (path[i + 1] == '/') {
                i += 2;
            }

            /* . */
            else if (path[i + 1] == 0) {
                i++;
            }

            /* 其它 */
            else {
                str[pos] = path[i];
                pos++;
                i++;
            }

        } else {
            str[pos] = path[i];
            pos++;

            i++;
        }
    }

    return str;
}

/* node_modules/gpio */
char *getNodeModulePath(char *path) {
    char *fullPath = NULL;
    int path_len;
    int fd;
    int file_len;
    cJSON *root;
    cJSON *item;
    char *json_data;

    path_len = strlen(path) + 128;
    fullPath = calloc(1, path_len);
    sprintf(fullPath, "%s/%s", BE_FS_ROOT_DIR, path);

    /* 去掉中间的./ */
    json_data = getClearPath(fullPath);
    sprintf(fullPath, "%s", json_data);
    free(json_data);

    printf("%s %d try open %s  \n", __FUNCTION__, __LINE__, fullPath);

    fd = be_open(fullPath, O_RDONLY);
    if (fd >= 0) {
        be_close(fd);
        free(fullPath);
        return path;
    }

    strcat(fullPath, "/package.json");

    printf("%s %d try open %s  \n", __FUNCTION__, __LINE__, fullPath);

    fd = be_open(fullPath, O_RDONLY);
    if (fd >= 0) {
        /* 分析package.json */
        file_len = be_lseek(fd, 0, SEEK_END);
        json_data = calloc(1, sizeof(char) * (file_len + 1));
        if (NULL == json_data) {
            be_close(fd);

            free(fullPath);
            printf("%s %d out of memory \n", __FUNCTION__, __LINE__);
            return (path);
        }
        be_lseek(fd, 0, SEEK_SET);
        be_read(fd, json_data, file_len);
        be_close(fd);

        /*parser the package json data */
        root = cJSON_Parse(json_data);
        if (NULL == root) {
            free(json_data);
            free(fullPath);
            printf("%s %d cJSON_Parse failed \n", __FUNCTION__, __LINE__);
            return (path);
        }

        /* find the main */
        item = cJSON_GetObjectItem(root, "main");
        if (NULL != item && cJSON_String == item->type &&
            strstr(item->valuestring, ".js")) {
            sprintf(fullPath, "%s/%s", path, item->valuestring);

            free(json_data);
            cJSON_Delete(root);
            free(path);

            printf("%s %d package find main, %s \n", __FUNCTION__, __LINE__,
                   fullPath);
            return fullPath;
        }

        free(json_data);
        cJSON_Delete(root);
        printf("%s %d package incomplete , package.json = %s \n", __FUNCTION__,
               __LINE__, fullPath);
        /* 默认index.js */
        sprintf(fullPath, "%s/%s", path, "index.js");
        return fullPath;
    } else {
        sprintf(fullPath, "%s/%s", path, "index.js");
        return fullPath;
    }

    free(fullPath);
    return path;
}

#endif
