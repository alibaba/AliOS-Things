/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2021 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __MEDIA_OS_CTYPE_H__
#define __MEDIA_OS_CTYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       Checks whether c is either a decimal digit or an uppercase or lowercase letter.
 *              The result is true if either isalpha or isdigit would also return true.
 *
 * @param[in]   Character to be checked, casted as an int, or EOF.
 *
 * @return      A value different from zero (i.e., true) if indeed c is either a digit or a letter.
 *              Zero (i.e., false) otherwise.
 *
 */
int media_os_isalnum(int c);

/**
 * @brief       Checks whether c is an alphabetic letter.
 *
 * @param[in]   Character to be checked, casted as an int, or EOF.
 *
 * @return      A value different from zero (i.e., true) if indeed c is an alphabetic letter.
 *              Zero (i.e., false) otherwise.
 *
 */
int media_os_isalpha(int c);

/**
 * @brief       Checks whether c is a blank character.
 *
 * @param[in]   Character to be checked, casted as an int, or EOF.
 *
 * @return      A value different from zero (i.e., true) if indeed c is a blank character.
 *              Zero (i.e., false) otherwise.
 *
 */
int media_os_isblank(int c);

/**
 * @brief       Checks whether c is a control character.
 *
 * @param[in]   Character to be checked, casted as an int, or EOF.
 *
 * @return      A value different from zero (i.e., true) if indeed c is a control character.
 *              Zero (i.e., false) otherwise.
 *
 */
int media_os_iscntrl(int c);

/**
 * @brief       Checks whether c is a decimal digit character.
 *
 * @param[in]   Character to be checked, casted as an int, or EOF.
 *
 * @return      A value different from zero (i.e., true) if indeed c is a decimal digit.
 *              Zero (i.e., false) otherwise.
 *
 */
int media_os_isdigit(int c);

/**
 * @brief       Checks whether c is a character with graphical representation.
 *
 * @param[in]   Character to be checked, casted as an int, or EOF.
 *
 * @return      A value different from zero (i.e., true) if indeed c has a graphical representation as character.
 *              Zero (i.e., false) otherwise.
 */
int media_os_isgraph(int c);

/**
 * @brief       Checks whether c is a lowercase letter.
 *
 * @param[in]   Character to be checked, casted as an int, or EOF.
 *
 * @return      A value different from zero (i.e., true) if indeed c is a lowercase alphabetic letter.
 *              Zero (i.e., false) otherwise.
 *
 */
int media_os_islower(int c);

/**
 * @brief       Checks whether c is a printable character.
 *
 * @param[in]   Character to be checked, casted as an int, or EOF.
 *
 * @return      A value different from zero (i.e., true) if indeed c is a printable character.
 *              Zero (i.e., false) otherwise.
 *
 */
int media_os_isprint(int c);

/**
 * @brief       Checks whether c is a punctuation character.
 *
 * @param[in]   Character to be checked, casted as an int, or EOF.
 *
 * @return      A value different from zero (i.e., true) if indeed c is a punctuation character.
 *              Zero (i.e., false) otherwise.
 *
 */
int media_os_ispunct(int c);

/**
 * @brief       Checks whether c is a white-space character.
 *
 * @param[in]   Character to be checked, casted as an int, or EOF.
 *
 * @return      A value different from zero (i.e., true) if indeed c is a white-space character.
 *              Zero (i.e., false) otherwise.
 *
 */
int media_os_isspace(int c);

/**
 * @brief       Checks if parameter c is an uppercase alphabetic letter.
 *
 * @param[in]   Character to be checked, casted as an int, or EOF.
 *
 * @return      A value different from zero (i.e., true) if indeed c is an uppercase alphabetic letter.
 *              Zero (i.e., false) otherwise.
 *
 */
int media_os_isupper(int c);

/**
 * @brief       Checks whether c is a hexdecimal digit character.
 *
 * @param[in]   Character to be checked, casted as an int, or EOF.
 *
 * @return      A value different from zero (i.e., true) if indeed c is a hexadecimal digit.
 *              Zero (i.e., false) otherwise.
 *
 */
int media_os_isxdigit(int c);

/**
 * @brief       Converts c to its lowercase equivalent if c is an uppercase letter and
 *              has a lowercase equivalent. If no such conversion is possible, the value returned is c unchanged.
 *
 * @param[in]   Character to be checked, casted as an int, or EOF.
 *
 * @return      The lowercase equivalent to c, if such value exists, or c (unchanged) otherwise.
 *              The value is returned as an int value that can be implicitly casted to char.
 *
 */
int media_os_tolower(int c);

/**
 * @brief       Converts c to its uppercase equivalent if c is a lowercase letter and
 *              has an uppercase equivalent. If no such conversion is possible, the value returned is c unchanged.
 *
 * @param[in]   Character to be checked, casted as an int, or EOF.
 *
 * @return      The uppercase equivalent to c, if such value exists, or c (unchanged) otherwise.
 *              The value is returned as an int value that can be implicitly casted to char.
 *
 */
int media_os_toupper(int c);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __MEDIA_OS_CTYPE_H__ */
