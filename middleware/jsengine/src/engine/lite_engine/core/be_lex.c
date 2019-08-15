/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/*
 *  BoneEngine Javascript 词法分析模块
 *  分析Javascript文件或者文本字符串，去掉空格，注释，
 *  分析出关键字，语句，操作符，操作数等JS关键语法成分的token
 *  把这些token放到本地symbol表中缓存，供其他模块使用。
 *
 */

#include "be_lex.h"

static void append_char(be_jse_lexer_ctx_t *lex, char ch)
{
    if (lex->token_len < MAX_TOKEN_LENGTH - 1) {
        lex->token[lex->token_len++] = ch;
    } else {
        be_jse_warn_at("Token name is too long! skipping character", lex,
                       lex->token_start);
    }
}
static bool is_token(be_jse_lexer_ctx_t *lex, const char *token)
{
    int i;
    for (i = 0; i < lex->token_len; i++) {
        if (lex->token[i] != token[i]) return false;
    }
    return token[lex->token_len] == 0;
}

/*直接跳到指定的字符 */
void lexer_seekto_char(be_jse_lexer_ctx_t *lex, int seek_to_char)
{
    if (lex->src) {
        lex->curr_pos = seek_to_char;
        return;
    }
}

/*取下一个字符 */
void lexer_get_next_char(be_jse_lexer_ctx_t *lex)
{
    if (lex->src) {
        lex->curr_char = lex->next_char;
        if (lex->curr_pos < lex->end_pos) {
            lex->next_char = lex->src[lex->curr_pos];
        } else {
            lex->next_char = 0;
        }
        lex->curr_pos++;
        return;
    }
}

static inline bool be_jse_lexer_is_token(be_jse_lexer_ctx_t *lex,
                                         const char *token, int startOffset)
{
    int i;
    for (i = startOffset; i < lex->token_len; i++) {
        if (lex->token[i] != token[i]) return false;
    }
    return token[lex->token_len] == 0;
}

/* 根据词法分析，提取JS   语法基本单元(token) */
static void next_token(be_jse_lexer_ctx_t *lex)
{
    lex->tk        = BE_TOKEN_END;
    lex->token_len = 0;

    /* Javascript 中的空白和注释需要先剔除掉 */

    while (lex->curr_char &&
           is_whitespace(lex->curr_char)) { /* 去除掉空格或者空行 */
        lexer_get_next_char(lex);
    }

    if (lex->curr_char == '/' && lex->next_char == '/') { /* 去除掉注释 "//" */

        while (lex->curr_char && lex->curr_char != '\n')
            lexer_get_next_char(lex);
        lexer_get_next_char(lex);
        next_token(lex);
        return;
    }

    if (lex->curr_char == '/' && lex->next_char == '*') { /* 去除掉注释 "/*" */
        while (lex->curr_char &&
               (lex->curr_char != '*' || lex->next_char != '/')) {
            lexer_get_next_char(lex);
        }
        lexer_get_next_char(lex);
        lexer_get_next_char(lex);
        next_token(lex);
        return;
    }

    lex->token_start = lex->curr_pos - 2;

    /* 是否为一个字符,基本语句单元 */
    if (is_alpha(lex->curr_char)) {
        while (is_alpha(lex->curr_char) || is_numeric(lex->curr_char)) {
            /* IDs */
            append_char(lex, lex->curr_char);
            lexer_get_next_char(lex);
        }
        lex->tk = BE_TOKEN_ID;
#if 0
        if (is_token(lex,"if"))              lex->tk = BE_TOKEN_KW_IF;
        else if (is_token(lex,"else"))       lex->tk = BE_TOKEN_KW_ELSE;
        else if (is_token(lex,"do"))         lex->tk = BE_TOKEN_KW_DO;
        else if (is_token(lex,"while"))      lex->tk = BE_TOKEN_KW_WHILE;
        else if (is_token(lex,"for"))        lex->tk = BE_TOKEN_KW_FOR;
        else if (is_token(lex,"break"))      lex->tk = BE_TOKEN_KW_BREAK;
        else if (is_token(lex,"continue"))   lex->tk = BE_TOKEN_KW_CONTINUE;
        else if (is_token(lex,"function"))   lex->tk = BE_TOKEN_KW_FUNCTION;
        else if (is_token(lex,"return"))     lex->tk = BE_TOKEN_KW_RETURN;
        else if (is_token(lex,"var"))        lex->tk = BE_TOKEN_KW_VAR;
        else if (is_token(lex,"true"))       lex->tk = BE_TOKEN_KW_TRUE;
        else if (is_token(lex,"false"))      lex->tk = BE_TOKEN_KW_FALSE;
        else if (is_token(lex,"null"))       lex->tk = BE_TOKEN_KW_NULL;
        else if (is_token(lex,"undefined"))  lex->tk = BE_TOKEN_KW_UNDEFINED;
        else if (is_token(lex,"new"))        lex->tk = BE_TOKEN_KW_NEW;
        else if (is_token(lex,"in"))         lex->tk = BE_TOKEN_KW_IN;
        else if (is_token(lex,"switch"))     lex->tk = BE_TOKEN_KW_SWITCH;
        else if (is_token(lex,"case"))       lex->tk = BE_TOKEN_KW_CASE;
        else if (is_token(lex,"default"))    lex->tk = BE_TOKEN_KW_DEFAULT;
#else
        /* We do fancy stuff here to reduce number of compares (hopefully GCC
           creates a jump table) */
        switch (lex->token[0]) {
            case 'b':
                if (be_jse_lexer_is_token(lex, "break", 1))
                    lex->tk = BE_TOKEN_KW_BREAK;
                break;
            case 'c':
                if (be_jse_lexer_is_token(lex, "case", 1))
                    lex->tk = BE_TOKEN_KW_CASE;
                else if (be_jse_lexer_is_token(lex, "continue", 1))
                    lex->tk = BE_TOKEN_KW_CONTINUE;
                break;
            case 'd':
                if (be_jse_lexer_is_token(lex, "default", 1))
                    lex->tk = BE_TOKEN_KW_DEFAULT;
                else if (be_jse_lexer_is_token(lex, "do", 1))
                    lex->tk = BE_TOKEN_KW_DO;
                break;
            case 'e':
                if (be_jse_lexer_is_token(lex, "else", 1))
                    lex->tk = BE_TOKEN_KW_ELSE;
                break;
            case 'f':
                if (be_jse_lexer_is_token(lex, "false", 1))
                    lex->tk = BE_TOKEN_KW_FALSE;
                else if (be_jse_lexer_is_token(lex, "for", 1))
                    lex->tk = BE_TOKEN_KW_FOR;
                else if (be_jse_lexer_is_token(lex, "function", 1))
                    lex->tk = BE_TOKEN_KW_FUNCTION;
                break;
            case 'i':
                if (be_jse_lexer_is_token(lex, "if", 1))
                    lex->tk = BE_TOKEN_KW_IF;
                else if (be_jse_lexer_is_token(lex, "in", 1))
                    lex->tk = BE_TOKEN_KW_IN;
                break;
            case 'n':
                if (be_jse_lexer_is_token(lex, "new", 1))
                    lex->tk = BE_TOKEN_KW_NEW;
                else if (be_jse_lexer_is_token(lex, "null", 1))
                    lex->tk = BE_TOKEN_KW_NULL;
                break;
            case 'r':
                if (be_jse_lexer_is_token(lex, "return", 1))
                    lex->tk = BE_TOKEN_KW_RETURN;
                break;
            case 's':
                if (be_jse_lexer_is_token(lex, "switch", 1))
                    lex->tk = BE_TOKEN_KW_SWITCH;
                break;
            case 't':
                if (be_jse_lexer_is_token(lex, "true", 1))
                    lex->tk = BE_TOKEN_KW_TRUE;
                break;
            case 'u':
                if (be_jse_lexer_is_token(lex, "undefined", 1))
                    lex->tk = BE_TOKEN_KW_UNDEFINED;
                break;
            case 'w':
                if (be_jse_lexer_is_token(lex, "while", 1))
                    lex->tk = BE_TOKEN_KW_WHILE;
                break;
            case 'v':
                if (be_jse_lexer_is_token(lex, "var", 1))
                    lex->tk = BE_TOKEN_KW_VAR;
                break;
            default:
                break;
        }

#endif
    }

    else if (is_numeric(lex->curr_char)) { /* 操作数处理 */
        bool canBeFloating = true;

        if (lex->curr_char == '0') {
            append_char(lex, lex->curr_char);
            lexer_get_next_char(lex);
        }
        if (lex->curr_char == 'x' ||
            lex->curr_char == 'b') { /* 0xXX or 0bxx 十六进制或者二进制 */
            canBeFloating = false; /* 整数 */
            append_char(lex, lex->curr_char);
            lexer_get_next_char(lex);
        }
        lex->tk = BE_TOKEN_INT;
        while (is_numeric(lex->curr_char) ||
               (!canBeFloating && is_hex_decimal(lex->curr_char))) { /* 整数 */
            append_char(lex, lex->curr_char);
            lexer_get_next_char(lex);
        }
        if (canBeFloating && lex->curr_char == '.') { /* 浮点小数 */
            lex->tk = BE_TOKEN_FLOAT;
            append_char(lex, '.');
            lexer_get_next_char(lex);
            while (is_numeric(lex->curr_char)) {
                append_char(lex, lex->curr_char);
                lexer_get_next_char(lex);
            }
        }
        /* 类似 3.45e2 */
        if (canBeFloating && (lex->curr_char == 'e' || lex->curr_char == 'E')) {
            lex->tk = BE_TOKEN_FLOAT;
            append_char(lex, lex->curr_char);
            lexer_get_next_char(lex);
            if (lex->curr_char == '-') {
                append_char(lex, lex->curr_char);
                lexer_get_next_char(lex);
            }
            while (is_numeric(lex->curr_char)) {
                append_char(lex, lex->curr_char);
                lexer_get_next_char(lex);
            }
        }
    } else if (lex->curr_char == '"') { /* 字符串 */
        lexer_get_next_char(lex);
        while (lex->curr_char && lex->curr_char != '"') {
            if (lex->curr_char == '\\') {
                lexer_get_next_char(lex);
                switch (lex->curr_char) {
                    case 'n':
                        append_char(lex, '\n');
                        break;
                    case '"':
                        append_char(lex, '"');
                        break;
                    case '\\':
                        append_char(lex, '\\');
                        break;
                    default:
                        append_char(lex, lex->curr_char);
                        break;
                }
            } else {
                append_char(lex, lex->curr_char);
            }
            lexer_get_next_char(lex);
        }
        lexer_get_next_char(lex);
        lex->tk = BE_TOKEN_STR;
    } else if (lex->curr_char == '\'') { /* 特殊的转义字符 */
        lexer_get_next_char(lex);
        while (lex->curr_char && lex->curr_char != '\'') {
            if (lex->curr_char == '\\') {
                lexer_get_next_char(lex);
                switch (lex->curr_char) {
                    case 'n':
                        append_char(lex, '\n');
                        break;
                    case 'a':
                        append_char(lex, '\a');
                        break;
                    case 'r':
                        append_char(lex, '\r');
                        break;
                    case 't':
                        append_char(lex, '\t');
                        break;
                    case '\'':
                        append_char(lex, '\'');
                        break;
                    case '\\':
                        append_char(lex, '\\');
                        break;
                    case 'x': {
                        /* 16进制数字 */
                        char buf[5] = "0x??";
                        lexer_get_next_char(lex);
                        buf[2] = lex->curr_char;
                        lexer_get_next_char(lex);
                        buf[3] = lex->curr_char;
                        append_char(lex, (char)string_to_int(buf));
                    } break;
                    default:
                        if (lex->curr_char >= '0' && lex->curr_char <= '7') {
                            /* 八进制数字 */
                            char buf[5] = "0???";
                            buf[1]      = lex->curr_char;
                            lexer_get_next_char(lex);
                            buf[2] = lex->curr_char;
                            lexer_get_next_char(lex);
                            buf[3] = lex->curr_char;
                            append_char(lex, (char)string_to_int(buf));
                        } else {
                            append_char(lex, lex->curr_char);
                        }
                        break;
                }
            } else {
                append_char(lex, lex->curr_char);
            }
            lexer_get_next_char(lex);
        }
        lexer_get_next_char(lex);
        lex->tk = BE_TOKEN_STR;
    } else { /* 运算符处理 */
        /* single chars */
        lex->tk = lex->curr_char;
        if (lex->curr_char) lexer_get_next_char(lex);

        if (lex->tk == '=' && lex->curr_char == '=') {
            lex->tk = BE_TOKEN_OP_EQUAL;
            lexer_get_next_char(lex);
            if (lex->curr_char == '=') {
                lex->tk = BE_TOKEN_OP_STRICT_EQUAL;
                lexer_get_next_char(lex);
            }
        } else if (lex->tk == '!' && lex->curr_char == '=') {
            lex->tk = BE_TOKEN_OP_NEQUAL;
            lexer_get_next_char(lex);
            if (lex->curr_char == '=') {
                lex->tk = BE_TOKEN_OP_STRICT_NEQUAL;
                lexer_get_next_char(lex);
            }
        } else if (lex->tk == '<' && lex->curr_char == '=') {
            lex->tk = BE_TOKEN_OP_LESS_EQUAL;
            lexer_get_next_char(lex);
        } else if (lex->tk == '<' && lex->curr_char == '<') {
            lex->tk = BE_TOKEN_OP_LEFT_SHIFT;
            lexer_get_next_char(lex);
            if (lex->curr_char == '=') {
                lex->tk = BE_TOKEN_OP_LEFT_SHIFT_EQL;
                lexer_get_next_char(lex);
            }
        } else if (lex->tk == '>' && lex->curr_char == '=') {
            lex->tk = BE_TOKEN_OP_MORE_EQUAL;
            lexer_get_next_char(lex);
        } else if (lex->tk == '>' && lex->curr_char == '>') {
            lex->tk = BE_TOKEN_OP_RIGHT_SHIFT;
            lexer_get_next_char(lex);
            if (lex->curr_char == '=') {
                lex->tk = BE_TOKEN_OP_RIGHT_SHIFT_EQUAL;
                lexer_get_next_char(lex);
            } else if (lex->curr_char == '>') {
                lex->tk = BE_TOKEN_OP_RIGHT_SHIFT_UNSIGN;
                lexer_get_next_char(lex);
            }
        } else if (lex->tk == '+' && lex->curr_char == '=') {
            lex->tk = BE_TOKEN_OP_PLUS_EQUAL;
            lexer_get_next_char(lex);
        } else if (lex->tk == '-' && lex->curr_char == '=') {
            lex->tk = BE_TOKEN_OP_MINUS_EQUAL;
            lexer_get_next_char(lex);
        } else if (lex->tk == '*' && lex->curr_char == '=') {
            lex->tk = BE_TOKEN_OP_MUL_EQUAL;
            lexer_get_next_char(lex);
        } else if (lex->tk == '/' && lex->curr_char == '=') {
            lex->tk = BE_TOKEN_OP_DIV_EQUAL;
            lexer_get_next_char(lex);
        } else if (lex->tk == '+' && lex->curr_char == '+') {
            lex->tk = BE_TOKEN_OP_PLUS_PLUS;
            lexer_get_next_char(lex);
        } else if (lex->tk == '-' && lex->curr_char == '-') {
            lex->tk = BE_TOKEN_OP_MINUS_MINUS;
            lexer_get_next_char(lex);
        } else if (lex->tk == '&' && lex->curr_char == '=') {
            lex->tk = BE_TOKEN_OP_AND_EQUAL;
            lexer_get_next_char(lex);
        } else if (lex->tk == '&' && lex->curr_char == '&') {
            lex->tk = BE_TOKEN_OP_LOGIC_AND;
            lexer_get_next_char(lex);
        } else if (lex->tk == '|' && lex->curr_char == '=') {
            lex->tk = BE_TOKEN_OP_OR_EQUAL;
            lexer_get_next_char(lex);
        } else if (lex->tk == '|' && lex->curr_char == '|') {
            lex->tk = BE_TOKEN_OP_LOGIC_OR;
            lexer_get_next_char(lex);
        } else if (lex->tk == '^' && lex->curr_char == '=') {
            lex->tk = BE_TOKEN_OP_XOR_EQUAL;
            lexer_get_next_char(lex);
        }
    }

    lex->token_last_end = lex->token_end;
    lex->token_end      = lex->curr_pos - 3;
}

/* 初始化词法分析器 */
void be_jse_lexer_init(be_jse_lexer_ctx_t *lex, const char *src, int start,
                       int end)
{
    if (end < 0) {
        end = (int)strlen(src);
    }

    lex->src       = src;
    lex->start_pos = start;
    lex->end_pos   = end;
    lex->curr_pos  = 0;

    lex->tk             = 0;
    lex->token_start    = 0;
    lex->token_end      = 0;
    lex->token_last_end = 0;
    lex->token_len      = 0;

    be_jse_lexer_reset(lex);
}

/* 从如入的JS文本字符串的指定位置开始，初始化词法分析器 */

void be_jse_lexer_init2(be_jse_lexer_ctx_t *lex, be_jse_lexer_ctx_t *from,
                        int start)
{
    int idx = from->token_last_end + 1;

    if (idx >= from->end_pos) idx = from->end_pos;

    if (from->src) {
        be_jse_lexer_init(lex, from->src, start, idx);
        return;
    }
}

/* 复位词法分析器，复位状态机，buffer */
void be_jse_lexer_reset(be_jse_lexer_ctx_t *lex)
{
    lexer_seekto_char(lex, lex->start_pos);

    lexer_get_next_char(lex);
    lexer_get_next_char(lex);
    next_token(lex);
}

void be_jse_lexer_deinit(be_jse_lexer_ctx_t *lex)
{
    lex->tk = BE_TOKEN_END;

    if (lex->src) {
        lex->src = NULL;
        return;
    }
}

/* 把token 转化成字符串 */
#ifndef BE_JSE_SILENT
void lexer_token_to_str(int token, char *str, size_t len)
{
    if (token > 32 && token < 128) {
        be_assert(len >= 4);
        str[0] = '\'';
        str[1] = (char)token;
        str[2] = '\'';
        str[3] = 0;
        return;
    }
    switch (token) {
        case BE_TOKEN_END:
            strncpy(str, "EOF", len);
            return;
        case BE_TOKEN_ID:
            strncpy(str, "ID", len);
            return;
        case BE_TOKEN_INT:
            strncpy(str, "INT", len);
            return;
        case BE_TOKEN_FLOAT:
            strncpy(str, "FLOAT", len);
            return;
        case BE_TOKEN_STR:
            strncpy(str, "STRING", len);
            return;
        case BE_TOKEN_OP_EQUAL:
            strncpy(str, "==", len);
            return;
        case BE_TOKEN_OP_STRICT_EQUAL:
            strncpy(str, "===", len);
            return;
        case BE_TOKEN_OP_NEQUAL:
            strncpy(str, "!=", len);
            return;
        case BE_TOKEN_OP_STRICT_NEQUAL:
            strncpy(str, "!==", len);
            return;
        case BE_TOKEN_OP_LESS_EQUAL:
            strncpy(str, "<=", len);
            return;
        case BE_TOKEN_OP_LEFT_SHIFT:
            strncpy(str, "<<", len);
            return;
        case BE_TOKEN_OP_LEFT_SHIFT_EQL:
            strncpy(str, "<<=", len);
            return;
        case BE_TOKEN_OP_MORE_EQUAL:
            strncpy(str, ">=", len);
            return;
        case BE_TOKEN_OP_RIGHT_SHIFT:
            strncpy(str, ">>", len);
            return;
        case BE_TOKEN_OP_RIGHT_SHIFT_UNSIGN:
            strncpy(str, ">>", len);
            return;
        case BE_TOKEN_OP_RIGHT_SHIFT_EQUAL:
            strncpy(str, ">>=", len);
            return;
        case BE_TOKEN_OP_PLUS_EQUAL:
            strncpy(str, "+=", len);
            return;
        case BE_TOKEN_OP_MUL_EQUAL:
            strncpy(str, "*=", len);
            return;
        case BE_TOKEN_OP_DIV_EQUAL:
            strncpy(str, "/=", len);
            return;
        case BE_TOKEN_OP_MINUS_EQUAL:
            strncpy(str, "-=", len);
            return;
        case BE_TOKEN_OP_PLUS_PLUS:
            strncpy(str, "++", len);
            return;
        case BE_TOKEN_OP_MINUS_MINUS:
            strncpy(str, "--", len);
            return;
        case BE_TOKEN_OP_AND_EQUAL:
            strncpy(str, "&=", len);
            return;
        case BE_TOKEN_OP_LOGIC_AND:
            strncpy(str, "&&", len);
            return;
        case BE_TOKEN_OP_OR_EQUAL:
            strncpy(str, "|=", len);
            return;
        case BE_TOKEN_OP_LOGIC_OR:
            strncpy(str, "||", len);
            return;
        case BE_TOKEN_OP_XOR_EQUAL:
            strncpy(str, "^=", len);
            return;
        case BE_TOKEN_KW_IF:
            strncpy(str, "if", len);
            return;
        case BE_TOKEN_KW_ELSE:
            strncpy(str, "else", len);
            return;
        case BE_TOKEN_KW_DO:
            strncpy(str, "do", len);
            return;
        case BE_TOKEN_KW_WHILE:
            strncpy(str, "while", len);
            return;
        case BE_TOKEN_KW_FOR:
            strncpy(str, "for", len);
            return;
        case BE_TOKEN_KW_BREAK:
            strncpy(str, "return", len);
            return;
        case BE_TOKEN_KW_CONTINUE:
            strncpy(str, "continue", len);
            return;
        case BE_TOKEN_KW_FUNCTION:
            strncpy(str, "function", len);
            return;
        case BE_TOKEN_KW_RETURN:
            strncpy(str, "return", len);
            return;
        case BE_TOKEN_KW_VAR:
            strncpy(str, "var", len);
            return;
        case BE_TOKEN_KW_TRUE:
            strncpy(str, "true", len);
            return;
        case BE_TOKEN_KW_FALSE:
            strncpy(str, "false", len);
            return;
        case BE_TOKEN_KW_NULL:
            strncpy(str, "null", len);
            return;
        case BE_TOKEN_KW_UNDEFINED:
            strncpy(str, "undefined", len);
            return;
        case BE_TOKEN_KW_NEW:
            strncpy(str, "new", len);
            return;
        case BE_TOKEN_KW_IN:
            strncpy(str, "in", len);
            return;
    }
    be_assert(len >= 10);
    strncpy(str, "?[", len);
    /* itostr(token, &str[2], 10); */
    itoa(token, &str[2], 10);
    strncat(str, "]", len);
}

void lexer_get_token_str(be_jse_lexer_ctx_t *lex, char *str, size_t len)
{
    if (lex->tk == BE_TOKEN_ID) {
        strncpy(str, "ID:", len);
        strncat(str, lexer_get_token(lex), len);
    } else if (lex->tk == BE_TOKEN_STR) {
        strncpy(str, "String:'", len);
        strncat(str, lexer_get_token(lex), len);
        strncat(str, "'", len);
    } else {
        lexer_token_to_str(lex->tk, str, len);
    }
}
#endif

char *lexer_get_token(be_jse_lexer_ctx_t *lex)
{
    be_assert(lex->token_len < MAX_TOKEN_LENGTH);
    lex->token[lex->token_len] = 0; /* add final null */
    return lex->token;
}

bool lexer_token_cmp(be_jse_lexer_ctx_t *lex, int expected_tk)
{
    if (lex->tk != expected_tk) {
#ifndef BE_JSE_SILENT
        char buf[BE_JSE_ERROR_BUF_SIZE];
        size_t bufpos = 0;
        strncpy(&buf[bufpos], "Got ", BE_JSE_ERROR_BUF_SIZE - bufpos);
        bufpos = strlen(buf);
        lexer_get_token_str(lex, &buf[bufpos], BE_JSE_ERROR_BUF_SIZE - bufpos);
        bufpos = strlen(buf);
        strncpy(&buf[bufpos], " expected ", BE_JSE_ERROR_BUF_SIZE - bufpos);
        bufpos = strlen(buf);
        lexer_token_to_str(expected_tk, &buf[bufpos],
                           BE_JSE_ERROR_BUF_SIZE - bufpos);
        be_jse_error_at(buf, lex, lex->token_start);
#else
        be_jse_error_at("be_jse_lex_match error", lex, lex->token_start);
#endif
        next_token(lex);
        return false;
    }
    next_token(lex);
    return true;
}

/* 获取行与列信息便于查看出错位置 */
void lexer_dump_line_and_col(be_jse_lexer_ctx_t *lex, int charPos, int *line,
                             int *col)
{
#ifdef BE_JSE_SILENT
    NOT_USED(lex);
    NOT_USED(charPos);
    *line = 0;
    *col  = 0;
#else
    int currentPos = lex->curr_pos;
    /* reset us completely */
    *line = 1;
    *col = 1;
    lexer_seekto_char(lex, 0);
    lexer_get_next_char(lex);
    while (lex->curr_char && lex->curr_pos < charPos - 1) {
        if (lex->curr_char == '\n') {
            *col = 0;
            (*line)++;
        } else {
            (*col)++;
        }
        lexer_get_next_char(lex);
    }

    /* Go back to where we were */
    be_assert(currentPos >
              1); /* must be, as lex should already have been loaded */
    lexer_seekto_char(lex, currentPos - 2);
    lexer_get_next_char(lex);
    lexer_get_next_char(lex);

#endif
}
