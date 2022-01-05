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

#include "ctype.h"

int media_os_isalnum(int c)
{
    return isalnum(c);
}

int media_os_isalpha(int c)
{
    return isalpha(c);
}

int media_os_isblank(int c)
{
    return isblank(c);
}

int media_os_iscntrl(int c)
{
    return iscntrl(c);
}

int media_os_isdigit(int c)
{
    return isdigit(c);
}

int media_os_isgraph(int c)
{
    return isgraph(c);
}

int media_os_islower(int c)
{
    return islower(c);
}

int media_os_isprint(int c)
{
    return isprint(c);
}

int media_os_ispunct(int c)
{
    return ispunct(c);
}

int media_os_isspace(int c)
{
    return isspace(c);
}

int media_os_isupper(int c)
{
    return isupper(c);
}

int media_os_isxdigit(int c)
{
    return isxdigit(c);
}

int media_os_tolower(int c)
{
    return tolower(c);
}

int media_os_toupper(int c)
{
    return toupper(c);
}
