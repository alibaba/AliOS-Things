/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <math.h>
#include <sys/time.h>
#include <stdarg.h>
#include "be_jse.h"
#include "be_jse_utils.h"

#ifndef BE_JSE_SILENT
static char tmp_token[MAX_TOKEN_LENGTH];
void be_jse_save_tmp_token(const char* token)
{
    strcpy(tmp_token, token);
}

const char* be_jse_get_tmp_token()
{
    return tmp_token;
}
#endif

ALWAYS_INLINE bool is_whitespace(char ch)
{
    return (ch==' ') || (ch=='\t') || (ch=='\n') || (ch=='\r');
}

ALWAYS_INLINE bool is_numeric(char ch)
{
    return (ch>='0') && (ch<='9');
}

ALWAYS_INLINE bool is_hex_decimal(char ch)
{
    return ((ch>='0') && (ch<='9')) ||
           ((ch>='a') && (ch<='f')) ||
           ((ch>='A') && (ch<='F'));
}
ALWAYS_INLINE bool is_alpha(char ch)
{
    return ((ch>='a') && (ch<='z')) || ((ch>='A') && (ch<='Z')) || ch=='_';
}


NO_INLINE void be_jse_error(const char *message)
{
    printf("ERROR: %s\n", message);
}

NO_INLINE void be_jse_error_at(const char *message, be_jse_lex_ctx_t *lex, int tokenPos)
{
    int line,col;
    lexer_dump_line_and_col(lex, tokenPos, &line, &col);
    be_jse_execute_error();
    printf("ERROR: %s at %d:%d current tolken=%s \n", message, line, col, lex->token);
}

NO_INLINE void be_jse_warn(const char *message)
{
    printf("WARNING: %s\n", message);
}

NO_INLINE void be_jse_warn_at(const char *message, be_jse_lex_ctx_t *lex, int tokenPos)
{
    int line,col;
    lexer_dump_line_and_col(lex, tokenPos, &line, &col);
    be_jse_execute_error();
    printf("WARNING: %s at %d:%d current tolken=%s \n", message, line, col, lex->token);
}

NO_INLINE void be_jse_assert_fail(const char *file, int line)
{
    be_jse_execute_error();
    printf("ASSERT FAIL AT %s:%d\n", file, line);
#if defined(ESP8266)

#else
    //exit(1);
    abort();
#endif

}

static char itoch(int val)
{
    if (val<10) return (char)('0'+val);
    return (char)('a'+val-10);
}

static void itostr_extra(be_jse_int_t vals,char *str,bool signedVal, unsigned int base)
{
    be_jse_uint_t val;

    if (signedVal && vals<0) {
        *(str++)='-';
        val = (be_jse_uint_t)(-vals);
    } else {
        val = (be_jse_uint_t)vals;
    }

    be_jse_uint_t tmp = val;
    int digits = 1;
    while (tmp>=base) {
        digits++;
        tmp /= base;
    }

    int i;
    for (i=digits-1; i>=0; i--) {
        str[i] = itoch((int)(val % base));
        val /= base;
    }
    str[digits] = 0;
}

static void ftoa_bounded_extra(be_jse_float_t val,char *str, size_t len, int radix, int fractionalDigits)
{
    const be_jse_float_t stopAtError = 0.0000001;
    if (isnan(val)) strncpy(str,"NaN",len);
    else if (!isfinite(val)) {
        if (val<0) strncpy(str,"-Infinity",len);
        else strncpy(str,"Infinity",len);
    } else {
        if (val<0) {
            if (--len <= 0) {
                *str=0;
                return;
            }
            *(str++) = '-';
            val = -val;
        }

        if (((be_jse_int_t)(val+stopAtError)) == (1+(be_jse_int_t)val))
            val = (be_jse_float_t)(1+(be_jse_int_t)val);

        be_jse_float_t d = 1;
        while (d*radix <= val) d*=radix;
        while (d >= 1) {
            int v = (int)(val / d);
            val -= v*d;
            if (--len <= 0) {
                *str=0;
                return;
            }
            *(str++) = itoch(v);
            d /= radix;
        }
#ifndef USE_NO_FLOATS
        if (((fractionalDigits<0) && val>0) || fractionalDigits>0) {
            if (--len <= 0) {
                *str=0;
                return;
            }
            *(str++)='.';
            val*=radix;
            while (((fractionalDigits<0) && (fractionalDigits>-12) && (val > stopAtError)) || (fractionalDigits > 0)) {
                int v = (int)(val+((fractionalDigits==1) ? 0.4 : 0.00000001) );
                val = (val-v)*radix;
                if (--len <= 0) {
                    *str=0;
                    return;
                }
                if (v==radix) v=radix-1;
                *(str++)=itoch(v);
                fractionalDigits--;
            }
        }
#endif

        *(str++)=0;
    }
}

ALWAYS_INLINE void itostr(be_jse_int_t val,char *str,unsigned int base)
{
    itostr_extra(val, str, true, base);
}

void ftoa_bounded(be_jse_float_t val,char *str, size_t len)
{
    ftoa_bounded_extra(val, str, len, 10, -1);
}

static NO_INLINE int get_radix(const char **s, int forceRadix, bool *hasError)
{
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
            if (forceRadix && forceRadix!=8 && forceRadix<25) return 0;
            (*s)++;


        } else if (**s == 'x' || **s == 'X') {
            radix = 16;
            if (forceRadix && forceRadix!=16 && forceRadix<34) return 0;
            (*s)++;


        } else if (**s == 'b' || **s == 'B') {
            radix = 2;
            if (forceRadix && forceRadix!=2 && forceRadix<12)
                return 0;
            else
                (*s)++;
        } else if (!forceRadix) {

            const char *p;
            for (p=*s; *p; p++)
                if (*p=='.' || *p=='8' || *p=='9')
                    radix = 10;
                else if (*p<'0' || *p>'9') break;
        }
    }
    if (forceRadix>0 && forceRadix<=36)
        radix = forceRadix;

    return radix;
}

int chtod(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    else if (ch >= 'a' && ch <= 'z')
        return 10 + ch - 'a';
    else if (ch >= 'A' && ch <= 'Z')
        return 10 + ch - 'A';
    else return -1;
}

static be_jse_float_t string_to_floatr(const char *s,int forceRadix)
{
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
        if (digit<0 || digit>=radix)
            break;
        v = (v*radix) + digit;
        s++;
    }

    if (radix == 10) {

        if (*s == '.') {
            s++;

            while (*s) {
                if (*s >= '0' && *s <= '9')
                    v += mul*(*s - '0');
                else break;
                mul /= 10;
                s++;
            }
        }


        if (*s == 'e' || *s == 'E') {
            s++;
            bool isENegated = false;
            if (*s == '-' || *s == '+') {
                isENegated = *s=='-';
                s++;
            }
            int e = 0;
            while (*s) {
                if (*s >= '0' && *s <= '9')
                    e = (e*10) + (*s - '0');
                else break;
                s++;
            }
            if (isENegated) e=-e;

            while (e>0) {
                v*=10;
                e--;
            }
            while (e<0) {
                v/=10;
                e++;
            }
        }
    }

    if (numberStart==s || (numberStart[0]=='.' && numberStart[1]==0)) return NAN;

    if (isNegated) return -v;
    return v;
}
static long long string_to_intr(const char *s, int forceRadix, bool *hasError)
{
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
        if (digit<0 || digit>=radix)
            break;
        v = v*radix + digit;
        s++;
    }

    if (hasError)
        *hasError = s==numberStart;

    if (isNegated) return -v;
    return v;
}


be_jse_float_t string_to_float(const char *s)
{
    return string_to_floatr(s,0);
}

long long string_to_int(const char *s)
{
    return string_to_intr(s,0,0);
}

