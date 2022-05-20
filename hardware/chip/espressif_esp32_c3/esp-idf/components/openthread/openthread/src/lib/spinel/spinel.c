/*
 *    Copyright (c) 2016, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 *    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  -------------------------------------------------------------------
 *
 *  ## Unit Test ##
 *
 *  This file includes its own unit test. To compile the unit test,
 *  simply compile this file with the macro SPINEL_SELF_TEST set to 1.
 *  For example:
 *
 *      cc spinel.c -Wall -DSPINEL_SELF_TEST=1 -o spinel
 *
 *  -------------------------------------------------------------------
 */

// ----------------------------------------------------------------------------
// MARK: -
// MARK: Headers

#include "spinel.h"

#include <errno.h>
#include <limits.h>

#ifndef SPINEL_PLATFORM_HEADER
/* These are all already included in the spinel platform header
 * if SPINEL_PLATFORM_HEADER was defined.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif // #ifndef SPINEL_PLATFORM_HEADER

// ----------------------------------------------------------------------------
// MARK: -

// IAR's errno.h apparently doesn't define EOVERFLOW.
#ifndef EOVERFLOW
// There is no real good choice for what to set
// errno to in this case, so we just pick the
// value '1' somewhat arbitrarily.
#define EOVERFLOW 1
#endif

// IAR's errno.h apparently doesn't define EINVAL.
#ifndef EINVAL
// There is no real good choice for what to set
// errno to in this case, so we just pick the
// value '1' somewhat arbitrarily.
#define EINVAL 1
#endif

// IAR's errno.h apparently doesn't define ENOMEM.
#ifndef ENOMEM
// There is no real good choice for what to set
// errno to in this case, so we just pick the
// value '1' somewhat arbitrarily.
#define ENOMEM 1
#endif

#ifndef SPINEL_PLATFORM_SHOULD_LOG_ASSERTS
#define SPINEL_PLATFORM_SHOULD_LOG_ASSERTS 0
#endif

#ifndef SPINEL_PLATFORM_DOESNT_IMPLEMENT_ERRNO_VAR
#define SPINEL_PLATFORM_DOESNT_IMPLEMENT_ERRNO_VAR 0
#endif

#ifndef SPINEL_PLATFORM_DOESNT_IMPLEMENT_FPRINTF
#define SPINEL_PLATFORM_DOESNT_IMPLEMENT_FPRINTF 0
#endif

#ifndef SPINEL_SELF_TEST
#define SPINEL_SELF_TEST 0
#endif

#if defined(errno) && SPINEL_PLATFORM_DOESNT_IMPLEMENT_ERRNO_VAR
#error "SPINEL_PLATFORM_DOESNT_IMPLEMENT_ERRNO_VAR is set but errno is already defined."
#endif

// Work-around for platforms that don't implement the `errno` variable.
#if !defined(errno) && SPINEL_PLATFORM_DOESNT_IMPLEMENT_ERRNO_VAR
static int spinel_errno_workaround_;
#define errno spinel_errno_workaround_
#endif // SPINEL_PLATFORM_DOESNT_IMPLEMENT_ERRNO_VAR

#ifndef assert_printf
#if SPINEL_PLATFORM_DOESNT_IMPLEMENT_FPRINTF
#define assert_printf(fmt, ...) printf(__FILE__ ":%d: " fmt "\n", __LINE__, __VA_ARGS__)
#else // if SPINEL_PLATFORM_DOESNT_IMPLEMENT_FPRINTF
#define assert_printf(fmt, ...) fprintf(stderr, __FILE__ ":%d: " fmt "\n", __LINE__, __VA_ARGS__)
#endif // else SPINEL_PLATFORM_DOESNT_IMPLEMENT_FPRINTF
#endif

#ifndef require_action
#if SPINEL_PLATFORM_SHOULD_LOG_ASSERTS
#define require_action(c, l, a)                           \
    do                                                    \
    {                                                     \
        if (!(c))                                         \
        {                                                 \
            assert_printf("Requirement Failed (%s)", #c); \
            a;                                            \
            goto l;                                       \
        }                                                 \
    } while (0)
#else // if DEBUG
#define require_action(c, l, a) \
    do                          \
    {                           \
        if (!(c))               \
        {                       \
            a;                  \
            goto l;             \
        }                       \
    } while (0)
#endif // else DEBUG
#endif // ifndef require_action

#ifndef require
#define require(c, l) require_action(c, l, {})
#endif

#ifndef strnlen
static size_t spinel_strnlen(const char *s, size_t maxlen)
{
    size_t ret;

    for (ret = 0; (ret < maxlen) && (s[ret] != 0); ret++)
    {
        // Empty loop.
    }

    return ret;
}
#else
#define spinel_strnlen strnlen
#endif

typedef struct
{
    va_list obj;
} va_list_obj;

#define SPINEL_MAX_PACK_LENGTH 32767

// ----------------------------------------------------------------------------

// This function validates whether a given byte sequence (string) follows UTF8 encoding.
static bool spinel_validate_utf8(const uint8_t *string)
{
    bool    ret = true;
    uint8_t byte;
    uint8_t continuation_bytes = 0;

    while ((byte = *string++) != 0)
    {
        if ((byte & 0x80) == 0)
        {
            continue;
        }

        // This is a leading byte 1xxx-xxxx.

        if ((byte & 0x40) == 0) // 10xx-xxxx
        {
            // We got a continuation byte pattern without seeing a leading byte earlier.
            ret = false;
            goto bail;
        }
        else if ((byte & 0x20) == 0) // 110x-xxxx
        {
            continuation_bytes = 1;
        }
        else if ((byte & 0x10) == 0) // 1110-xxxx
        {
            continuation_bytes = 2;
        }
        else if ((byte & 0x08) == 0) // 1111-0xxx
        {
            continuation_bytes = 3;
        }
        else // 1111-1xxx  (invalid pattern).
        {
            ret = false;
            goto bail;
        }

        while (continuation_bytes-- != 0)
        {
            byte = *string++;

            // Verify the continuation byte pattern 10xx-xxxx
            if ((byte & 0xc0) != 0x80)
            {
                ret = false;
                goto bail;
            }
        }
    }

bail:
    return ret;
}

// ----------------------------------------------------------------------------
// MARK: -

spinel_ssize_t spinel_packed_uint_decode(const uint8_t *bytes, spinel_size_t len, unsigned int *value_ptr)
{
    spinel_ssize_t ret   = 0;
    unsigned int   value = 0;

    unsigned int i = 0;

    do
    {
        if ((len < sizeof(uint8_t)) || (i >= sizeof(unsigned int) * CHAR_BIT))
        {
            ret = -1;
            break;
        }

        value |= (unsigned int)(bytes[0] & 0x7F) << i;
        i += 7;
        ret += sizeof(uint8_t);
        bytes += sizeof(uint8_t);
        len -= sizeof(uint8_t);
    } while ((bytes[-1] & 0x80) == 0x80);

    if ((ret > 0) && (value_ptr != NULL))
    {
        *value_ptr = value;
    }

    return ret;
}

spinel_ssize_t spinel_packed_uint_size(unsigned int value)
{
    spinel_ssize_t ret;

    if (value < (1 << 7))
    {
        ret = 1;
    }
    else if (value < (1 << 14))
    {
        ret = 2;
    }
    else if (value < (1 << 21))
    {
        ret = 3;
    }
    else if (value < (1 << 28))
    {
        ret = 4;
    }
    else
    {
        ret = 5;
    }

    return ret;
}

spinel_ssize_t spinel_packed_uint_encode(uint8_t *bytes, spinel_size_t len, unsigned int value)
{
    const spinel_ssize_t encoded_size = spinel_packed_uint_size(value);

    if ((spinel_ssize_t)len >= encoded_size)
    {
        spinel_ssize_t i;

        for (i = 0; i != encoded_size - 1; ++i)
        {
            *bytes++ = (value & 0x7F) | 0x80;
            value    = (value >> 7);
        }

        *bytes++ = (value & 0x7F);
    }

    return encoded_size;
}

const char *spinel_next_packed_datatype(const char *pack_format)
{
    int depth = 0;

    do
    {
        switch (*++pack_format)
        {
        case '(':
            depth++;
            break;

        case ')':
            depth--;

            if (depth == 0)
            {
                pack_format++;
            }

            break;
        }
    } while ((depth > 0) && *pack_format != 0);

    return pack_format;
}

static spinel_ssize_t spinel_datatype_vunpack_(bool           in_place,
                                               const uint8_t *data_in,
                                               spinel_size_t  data_len,
                                               const char *   pack_format,
                                               va_list_obj *  args)
{
    spinel_ssize_t ret = 0;

    // Buffer length sanity check
    require_action(data_len <= SPINEL_MAX_PACK_LENGTH, bail, (ret = -1, errno = EINVAL));

    for (; *pack_format != 0; pack_format = spinel_next_packed_datatype(pack_format))
    {
        if (*pack_format == ')')
        {
            // Don't go past the end of a struct.
            break;
        }

        switch ((spinel_datatype_t)pack_format[0])
        {
        case SPINEL_DATATYPE_BOOL_C:
        {
            bool *arg_ptr = va_arg(args->obj, bool *);
            require_action(data_len >= sizeof(uint8_t), bail, (ret = -1, errno = EOVERFLOW));

            if (arg_ptr)
            {
                *arg_ptr = data_in[0] != 0;
            }

            ret += sizeof(uint8_t);
            data_in += sizeof(uint8_t);
            data_len -= sizeof(uint8_t);
            break;
        }

        case SPINEL_DATATYPE_INT8_C:
        case SPINEL_DATATYPE_UINT8_C:
        {
            uint8_t *arg_ptr = va_arg(args->obj, uint8_t *);
            require_action(data_len >= sizeof(uint8_t), bail, (ret = -1, errno = EOVERFLOW));

            if (arg_ptr)
            {
                *arg_ptr = data_in[0];
            }

            ret += sizeof(uint8_t);
            data_in += sizeof(uint8_t);
            data_len -= sizeof(uint8_t);
            break;
        }

        case SPINEL_DATATYPE_INT16_C:
        case SPINEL_DATATYPE_UINT16_C:
        {
            uint16_t *arg_ptr = va_arg(args->obj, uint16_t *);
            require_action(data_len >= sizeof(uint16_t), bail, (ret = -1, errno = EOVERFLOW));

            if (arg_ptr)
            {
                *arg_ptr = (uint16_t)((data_in[1] << 8) | data_in[0]);
            }

            ret += sizeof(uint16_t);
            data_in += sizeof(uint16_t);
            data_len -= sizeof(uint16_t);
            break;
        }

        case SPINEL_DATATYPE_INT32_C:
        case SPINEL_DATATYPE_UINT32_C:
        {
            uint32_t *arg_ptr = va_arg(args->obj, uint32_t *);
            require_action(data_len >= sizeof(uint32_t), bail, (ret = -1, errno = EOVERFLOW));

            if (arg_ptr)
            {
                *arg_ptr = (uint32_t)((data_in[3] << 24) | (data_in[2] << 16) | (data_in[1] << 8) | data_in[0]);
            }

            ret += sizeof(uint32_t);
            data_in += sizeof(uint32_t);
            data_len -= sizeof(uint32_t);
            break;
        }

        case SPINEL_DATATYPE_INT64_C:
        case SPINEL_DATATYPE_UINT64_C:
        {
            uint64_t *arg_ptr = va_arg(args->obj, uint64_t *);
            require_action(data_len >= sizeof(uint64_t), bail, (ret = -1, errno = EOVERFLOW));

            if (arg_ptr)
            {
                uint32_t l32 = (uint32_t)((data_in[3] << 24) | (data_in[2] << 16) | (data_in[1] << 8) | data_in[0]);
                uint32_t h32 = (uint32_t)((data_in[7] << 24) | (data_in[6] << 16) | (data_in[5] << 8) | data_in[4]);

                *arg_ptr = ((uint64_t)l32) | (((uint64_t)h32) << 32);
            }

            ret += sizeof(uint64_t);
            data_in += sizeof(uint64_t);
            data_len -= sizeof(uint64_t);
            break;
        }

        case SPINEL_DATATYPE_IPv6ADDR_C:
        {
            require_action(data_len >= sizeof(spinel_ipv6addr_t), bail, (ret = -1, errno = EOVERFLOW));

            if (in_place)
            {
                spinel_ipv6addr_t *arg = va_arg(args->obj, spinel_ipv6addr_t *);
                if (arg)
                {
                    memcpy(arg, data_in, sizeof(spinel_ipv6addr_t));
                }
            }
            else
            {
                const spinel_ipv6addr_t **arg_ptr = va_arg(args->obj, const spinel_ipv6addr_t **);
                if (arg_ptr)
                {
                    *arg_ptr = (const spinel_ipv6addr_t *)data_in;
                }
            }

            ret += sizeof(spinel_ipv6addr_t);
            data_in += sizeof(spinel_ipv6addr_t);
            data_len -= sizeof(spinel_ipv6addr_t);
            break;
        }

        case SPINEL_DATATYPE_EUI64_C:
        {
            require_action(data_len >= sizeof(spinel_eui64_t), bail, (ret = -1, errno = EOVERFLOW));

            if (in_place)
            {
                spinel_eui64_t *arg = va_arg(args->obj, spinel_eui64_t *);
                if (arg)
                {
                    memcpy(arg, data_in, sizeof(spinel_eui64_t));
                }
            }
            else
            {
                const spinel_eui64_t **arg_ptr = va_arg(args->obj, const spinel_eui64_t **);
                if (arg_ptr)
                {
                    *arg_ptr = (const spinel_eui64_t *)data_in;
                }
            }

            ret += sizeof(spinel_eui64_t);
            data_in += sizeof(spinel_eui64_t);
            data_len -= sizeof(spinel_eui64_t);
            break;
        }

        case SPINEL_DATATYPE_EUI48_C:
        {
            require_action(data_len >= sizeof(spinel_eui48_t), bail, (ret = -1, errno = EOVERFLOW));

            if (in_place)
            {
                spinel_eui48_t *arg = va_arg(args->obj, spinel_eui48_t *);
                if (arg)
                {
                    memcpy(arg, data_in, sizeof(spinel_eui48_t));
                }
            }
            else
            {
                const spinel_eui48_t **arg_ptr = va_arg(args->obj, const spinel_eui48_t **);
                if (arg_ptr)
                {
                    *arg_ptr = (const spinel_eui48_t *)data_in;
                }
            }

            ret += sizeof(spinel_eui48_t);
            data_in += sizeof(spinel_eui48_t);
            data_len -= sizeof(spinel_eui48_t);
            break;
        }

        case SPINEL_DATATYPE_UINT_PACKED_C:
        {
            unsigned int * arg_ptr = va_arg(args->obj, unsigned int *);
            spinel_ssize_t pui_len = spinel_packed_uint_decode(data_in, data_len, arg_ptr);

            // Range check
            require_action(NULL == arg_ptr || (*arg_ptr < SPINEL_MAX_UINT_PACKED), bail, (ret = -1, errno = ERANGE));

            require(pui_len > 0, bail);

            require(pui_len <= (spinel_ssize_t)data_len, bail);

            ret += pui_len;
            data_in += pui_len;
            data_len -= (spinel_size_t)pui_len;
            break;
        }

        case SPINEL_DATATYPE_UTF8_C:
        {
            size_t len;

            // Make sure we have at least one byte.
            require_action(data_len > 0, bail, (ret = -1, errno = EOVERFLOW));

            // Add 1 for zero termination. If not zero terminated,
            // len will then be data_len+1, which we will detect
            // in the next check.
            len = spinel_strnlen((const char *)data_in, data_len) + 1;

            // Verify that the string is zero terminated.
            require_action(len <= data_len, bail, (ret = -1, errno = EOVERFLOW));

            // Verify the string follows valid UTF8 encoding.
            require_action(spinel_validate_utf8(data_in), bail, (ret = -1, errno = EINVAL));

            if (in_place)
            {
                char * arg     = va_arg(args->obj, char *);
                size_t len_arg = va_arg(args->obj, size_t);
                if (arg)
                {
                    require_action(len_arg >= len, bail, (ret = -1, errno = ENOMEM));
                    memcpy(arg, data_in, len);
                }
            }
            else
            {
                const char **arg_ptr = va_arg(args->obj, const char **);
                if (arg_ptr)
                {
                    *arg_ptr = (const char *)data_in;
                }
            }

            ret += (spinel_size_t)len;
            data_in += len;
            data_len -= (spinel_size_t)len;
            break;
        }

        case SPINEL_DATATYPE_DATA_C:
        case SPINEL_DATATYPE_DATA_WLEN_C:
        {
            spinel_ssize_t pui_len       = 0;
            uint16_t       block_len     = 0;
            const uint8_t *block_ptr     = data_in;
            void *         arg_ptr       = va_arg(args->obj, void *);
            unsigned int * block_len_ptr = va_arg(args->obj, unsigned int *);
            char           nextformat    = *spinel_next_packed_datatype(pack_format);

            if ((pack_format[0] == SPINEL_DATATYPE_DATA_WLEN_C) || ((nextformat != 0) && (nextformat != ')')))
            {
                pui_len = spinel_datatype_unpack(data_in, data_len, SPINEL_DATATYPE_UINT16_S, &block_len);
                block_ptr += pui_len;

                require(pui_len > 0, bail);
                require(block_len < SPINEL_FRAME_MAX_SIZE, bail);
            }
            else
            {
                block_len = (uint16_t)data_len;
                pui_len   = 0;
            }

            require_action((spinel_ssize_t)data_len >= (block_len + pui_len), bail, (ret = -1, errno = EOVERFLOW));

            if (in_place)
            {
                require_action(NULL != block_len_ptr && *block_len_ptr >= block_len, bail, (ret = -1, errno = EINVAL));
                memcpy(arg_ptr, block_ptr, block_len);
            }
            else
            {
                const uint8_t **block_ptr_ptr = (const uint8_t **)arg_ptr;
                if (NULL != block_ptr_ptr)
                {
                    *block_ptr_ptr = block_ptr;
                }
            }

            if (NULL != block_len_ptr)
            {
                *block_len_ptr = block_len;
            }

            block_len += (uint16_t)pui_len;
            ret += block_len;
            data_in += block_len;
            data_len -= block_len;
            break;
        }

        case 'T':
        case SPINEL_DATATYPE_STRUCT_C:
        {
            spinel_ssize_t pui_len    = 0;
            uint16_t       block_len  = 0;
            spinel_ssize_t actual_len = 0;
            const uint8_t *block_ptr  = data_in;
            char           nextformat = *spinel_next_packed_datatype(pack_format);

            if ((pack_format[0] == SPINEL_DATATYPE_STRUCT_C) || ((nextformat != 0) && (nextformat != ')')))
            {
                pui_len = spinel_datatype_unpack(data_in, data_len, SPINEL_DATATYPE_UINT16_S, &block_len);
                block_ptr += pui_len;

                require(pui_len > 0, bail);
                require(block_len < SPINEL_FRAME_MAX_SIZE, bail);
            }
            else
            {
                block_len = (uint16_t)data_len;
                pui_len   = 0;
            }

            require_action((spinel_ssize_t)data_len >= (block_len + pui_len), bail, (ret = -1, errno = EOVERFLOW));

            actual_len = spinel_datatype_vunpack_(false, block_ptr, block_len, pack_format + 2, args);

            require_action(actual_len > -1, bail, (ret = -1, errno = EOVERFLOW));

            if (pui_len)
            {
                block_len += (uint16_t)pui_len;
            }
            else
            {
                block_len = (uint16_t)actual_len;
            }

            ret += block_len;
            data_in += block_len;
            data_len -= block_len;
            break;
        }

        case '.':
            // Skip.
            break;

        case SPINEL_DATATYPE_ARRAY_C:
        default:
            // Unsupported Type!
            ret   = -1;
            errno = EINVAL;
            goto bail;
        }
    }

    return ret;

bail:
    return ret;
}

spinel_ssize_t spinel_datatype_unpack_in_place(const uint8_t *data_in,
                                               spinel_size_t  data_len,
                                               const char *   pack_format,
                                               ...)
{
    spinel_ssize_t ret;
    va_list_obj    args;
    va_start(args.obj, pack_format);

    ret = spinel_datatype_vunpack_(true, data_in, data_len, pack_format, &args);

    va_end(args.obj);
    return ret;
}

spinel_ssize_t spinel_datatype_unpack(const uint8_t *data_in, spinel_size_t data_len, const char *pack_format, ...)
{
    spinel_ssize_t ret;
    va_list_obj    args;
    va_start(args.obj, pack_format);

    ret = spinel_datatype_vunpack_(false, data_in, data_len, pack_format, &args);

    va_end(args.obj);
    return ret;
}

spinel_ssize_t spinel_datatype_vunpack_in_place(const uint8_t *data_in,
                                                spinel_size_t  data_len,
                                                const char *   pack_format,
                                                va_list        args)
{
    spinel_ssize_t ret;
    va_list_obj    args_obj;
    va_copy(args_obj.obj, args);

    ret = spinel_datatype_vunpack_(true, data_in, data_len, pack_format, &args_obj);

    va_end(args_obj.obj);
    return ret;
}

spinel_ssize_t spinel_datatype_vunpack(const uint8_t *data_in,
                                       spinel_size_t  data_len,
                                       const char *   pack_format,
                                       va_list        args)
{
    spinel_ssize_t ret;
    va_list_obj    args_obj;
    va_copy(args_obj.obj, args);

    ret = spinel_datatype_vunpack_(false, data_in, data_len, pack_format, &args_obj);

    va_end(args_obj.obj);
    return ret;
}

static spinel_ssize_t spinel_datatype_vpack_(uint8_t *     data_out,
                                             spinel_size_t data_len_max,
                                             const char *  pack_format,
                                             va_list_obj * args)
{
    spinel_ssize_t ret = 0;

    // Buffer length sanity check
    require_action(data_len_max <= SPINEL_MAX_PACK_LENGTH, bail, (ret = -1, errno = EINVAL));

    if (data_out == NULL)
    {
        data_len_max = 0;
    }

    for (; *pack_format != 0; pack_format = spinel_next_packed_datatype(pack_format))
    {
        if (*pack_format == ')')
        {
            // Don't go past the end of a struct.
            break;
        }

        switch ((spinel_datatype_t)*pack_format)
        {
        case SPINEL_DATATYPE_BOOL_C:
        {
            bool arg = (bool)va_arg(args->obj, int);
            ret += sizeof(uint8_t);

            if (data_len_max >= sizeof(uint8_t))
            {
                data_out[0] = (arg != false);
                data_out += sizeof(uint8_t);
                data_len_max -= sizeof(uint8_t);
            }
            else
            {
                data_len_max = 0;
            }

            break;
        }

        case SPINEL_DATATYPE_INT8_C:
        case SPINEL_DATATYPE_UINT8_C:
        {
            uint8_t arg = (uint8_t)va_arg(args->obj, int);
            ret += sizeof(uint8_t);

            if (data_len_max >= sizeof(uint8_t))
            {
                data_out[0] = arg;
                data_out += sizeof(uint8_t);
                data_len_max -= sizeof(uint8_t);
            }
            else
            {
                data_len_max = 0;
            }

            break;
        }

        case SPINEL_DATATYPE_INT16_C:
        case SPINEL_DATATYPE_UINT16_C:
        {
            uint16_t arg = (uint16_t)va_arg(args->obj, int);
            ret += sizeof(uint16_t);

            if (data_len_max >= sizeof(uint16_t))
            {
                data_out[1] = (arg >> 8) & 0xff;
                data_out[0] = (arg >> 0) & 0xff;
                data_out += sizeof(uint16_t);
                data_len_max -= sizeof(uint16_t);
            }
            else
            {
                data_len_max = 0;
            }

            break;
        }

        case SPINEL_DATATYPE_INT32_C:
        case SPINEL_DATATYPE_UINT32_C:
        {
            uint32_t arg = (uint32_t)va_arg(args->obj, int);
            ret += sizeof(uint32_t);

            if (data_len_max >= sizeof(uint32_t))
            {
                data_out[3] = (arg >> 24) & 0xff;
                data_out[2] = (arg >> 16) & 0xff;
                data_out[1] = (arg >> 8) & 0xff;
                data_out[0] = (arg >> 0) & 0xff;
                data_out += sizeof(uint32_t);
                data_len_max -= sizeof(uint32_t);
            }
            else
            {
                data_len_max = 0;
            }

            break;
        }

        case SPINEL_DATATYPE_INT64_C:
        case SPINEL_DATATYPE_UINT64_C:
        {
            uint64_t arg = va_arg(args->obj, uint64_t);

            ret += sizeof(uint64_t);

            if (data_len_max >= sizeof(uint64_t))
            {
                data_out[7] = (arg >> 56) & 0xff;
                data_out[6] = (arg >> 48) & 0xff;
                data_out[5] = (arg >> 40) & 0xff;
                data_out[4] = (arg >> 32) & 0xff;
                data_out[3] = (arg >> 24) & 0xff;
                data_out[2] = (arg >> 16) & 0xff;
                data_out[1] = (arg >> 8) & 0xff;
                data_out[0] = (arg >> 0) & 0xff;
                data_out += sizeof(uint64_t);
                data_len_max -= sizeof(uint64_t);
            }
            else
            {
                data_len_max = 0;
            }

            break;
        }

        case SPINEL_DATATYPE_IPv6ADDR_C:
        {
            spinel_ipv6addr_t *arg = va_arg(args->obj, spinel_ipv6addr_t *);
            ret += sizeof(spinel_ipv6addr_t);

            if (data_len_max >= sizeof(spinel_ipv6addr_t))
            {
                *(spinel_ipv6addr_t *)data_out = *arg;
                data_out += sizeof(spinel_ipv6addr_t);
                data_len_max -= sizeof(spinel_ipv6addr_t);
            }
            else
            {
                data_len_max = 0;
            }

            break;
        }

        case SPINEL_DATATYPE_EUI48_C:
        {
            spinel_eui48_t *arg = va_arg(args->obj, spinel_eui48_t *);
            ret += sizeof(spinel_eui48_t);

            if (data_len_max >= sizeof(spinel_eui48_t))
            {
                *(spinel_eui48_t *)data_out = *arg;
                data_out += sizeof(spinel_eui48_t);
                data_len_max -= sizeof(spinel_eui48_t);
            }
            else
            {
                data_len_max = 0;
            }

            break;
        }

        case SPINEL_DATATYPE_EUI64_C:
        {
            spinel_eui64_t *arg = va_arg(args->obj, spinel_eui64_t *);
            ret += sizeof(spinel_eui64_t);

            if (data_len_max >= sizeof(spinel_eui64_t))
            {
                *(spinel_eui64_t *)data_out = *arg;
                data_out += sizeof(spinel_eui64_t);
                data_len_max -= sizeof(spinel_eui64_t);
            }
            else
            {
                data_len_max = 0;
            }

            break;
        }

        case SPINEL_DATATYPE_UINT_PACKED_C:
        {
            uint32_t       arg = va_arg(args->obj, uint32_t);
            spinel_ssize_t encoded_size;

            // Range Check
            require_action(arg < SPINEL_MAX_UINT_PACKED, bail, {
                ret   = -1;
                errno = EINVAL;
            });

            encoded_size = spinel_packed_uint_encode(data_out, data_len_max, arg);
            ret += encoded_size;

            if ((spinel_ssize_t)data_len_max >= encoded_size)
            {
                data_out += encoded_size;
                data_len_max -= (spinel_size_t)encoded_size;
            }
            else
            {
                data_len_max = 0;
            }

            break;
        }

        case SPINEL_DATATYPE_UTF8_C:
        {
            const char *string_arg     = va_arg(args->obj, const char *);
            size_t      string_arg_len = 0;

            if (string_arg)
            {
                string_arg_len = strlen(string_arg) + 1;
            }
            else
            {
                string_arg     = "";
                string_arg_len = 1;
            }

            ret += (spinel_size_t)string_arg_len;

            if (data_len_max >= string_arg_len)
            {
                memcpy(data_out, string_arg, string_arg_len);

                data_out += string_arg_len;
                data_len_max -= (spinel_size_t)string_arg_len;
            }
            else
            {
                data_len_max = 0;
            }

            break;
        }

        case SPINEL_DATATYPE_DATA_WLEN_C:
        case SPINEL_DATATYPE_DATA_C:
        {
            const uint8_t *arg           = va_arg(args->obj, const uint8_t *);
            uint32_t       data_size_arg = va_arg(args->obj, uint32_t);
            spinel_ssize_t size_len      = 0;
            char           nextformat    = *spinel_next_packed_datatype(pack_format);

            if ((pack_format[0] == SPINEL_DATATYPE_DATA_WLEN_C) || ((nextformat != 0) && (nextformat != ')')))
            {
                size_len = spinel_datatype_pack(data_out, data_len_max, SPINEL_DATATYPE_UINT16_S, data_size_arg);
                require_action(size_len > 0, bail, {
                    ret   = -1;
                    errno = EINVAL;
                });
            }

            ret += (spinel_size_t)size_len + data_size_arg;

            if (data_len_max >= (spinel_size_t)size_len + data_size_arg)
            {
                data_out += size_len;
                data_len_max -= (spinel_size_t)size_len;

                if (data_out && arg)
                {
                    memcpy(data_out, arg, data_size_arg);
                }

                data_out += data_size_arg;
                data_len_max -= data_size_arg;
            }
            else
            {
                data_len_max = 0;
            }

            break;
        }

        case 'T':
        case SPINEL_DATATYPE_STRUCT_C:
        {
            spinel_ssize_t struct_len = 0;
            spinel_ssize_t size_len   = 0;
            char           nextformat = *spinel_next_packed_datatype(pack_format);

            require_action(pack_format[1] == '(', bail, {
                ret   = -1;
                errno = EINVAL;
            });

            // First we figure out the size of the struct
            {
                va_list_obj subargs;
                va_copy(subargs.obj, args->obj);
                struct_len = spinel_datatype_vpack_(NULL, 0, pack_format + 2, &subargs);
                va_end(subargs.obj);
            }

            if ((pack_format[0] == SPINEL_DATATYPE_STRUCT_C) || ((nextformat != 0) && (nextformat != ')')))
            {
                size_len = spinel_datatype_pack(data_out, data_len_max, SPINEL_DATATYPE_UINT16_S, struct_len);
                require_action(size_len > 0, bail, {
                    ret   = -1;
                    errno = EINVAL;
                });
            }

            ret += size_len + struct_len;

            if (struct_len + size_len <= (spinel_ssize_t)data_len_max)
            {
                data_out += size_len;
                data_len_max -= (spinel_size_t)size_len;

                struct_len = spinel_datatype_vpack_(data_out, data_len_max, pack_format + 2, args);

                data_out += struct_len;
                data_len_max -= (spinel_size_t)struct_len;
            }
            else
            {
                data_len_max = 0;
            }

            break;
        }

        case '.':
            // Skip.
            break;

        default:
            // Unsupported Type!
            ret   = -1;
            errno = EINVAL;
            goto bail;
        }
    }

bail:
    return ret;
}

spinel_ssize_t spinel_datatype_pack(uint8_t *data_out, spinel_size_t data_len_max, const char *pack_format, ...)
{
    int         ret;
    va_list_obj args;
    va_start(args.obj, pack_format);

    ret = spinel_datatype_vpack_(data_out, data_len_max, pack_format, &args);

    va_end(args.obj);
    return ret;
}

spinel_ssize_t spinel_datatype_vpack(uint8_t *     data_out,
                                     spinel_size_t data_len_max,
                                     const char *  pack_format,
                                     va_list       args)
{
    int         ret;
    va_list_obj args_obj;
    va_copy(args_obj.obj, args);

    ret = spinel_datatype_vpack_(data_out, data_len_max, pack_format, &args_obj);

    va_end(args_obj.obj);
    return ret;
}

// ----------------------------------------------------------------------------
// MARK: -

// LCOV_EXCL_START

struct spinel_cstr
{
    uint32_t    val;
    const char *str;
};

static const char *spinel_to_cstr(const struct spinel_cstr *table, uint32_t val)
{
    int i;

    for (i = 0; table[i].str; i++)
        if (val == table[i].val)
            return table[i].str;
    return "UNKNOWN";
}

const char *spinel_command_to_cstr(spinel_command_t command)
{
    static const struct spinel_cstr spinel_commands_cstr[] = {
        {SPINEL_CMD_NOOP, "NOOP"},
        {SPINEL_CMD_RESET, "RESET"},
        {SPINEL_CMD_PROP_VALUE_GET, "PROP_VALUE_GET"},
        {SPINEL_CMD_PROP_VALUE_SET, "PROP_VALUE_SET"},
        {SPINEL_CMD_PROP_VALUE_INSERT, "PROP_VALUE_INSERT"},
        {SPINEL_CMD_PROP_VALUE_REMOVE, "PROP_VALUE_REMOVE"},
        {SPINEL_CMD_PROP_VALUE_IS, "PROP_VALUE_IS"},
        {SPINEL_CMD_PROP_VALUE_INSERTED, "PROP_VALUE_INSERTED"},
        {SPINEL_CMD_PROP_VALUE_REMOVED, "PROP_VALUE_REMOVED"},
        {SPINEL_CMD_NET_SAVE, "NET_SAVE"},
        {SPINEL_CMD_NET_CLEAR, "NET_CLEAR"},
        {SPINEL_CMD_NET_RECALL, "NET_RECALL"},
        {SPINEL_CMD_HBO_OFFLOAD, "HBO_OFFLOAD"},
        {SPINEL_CMD_HBO_RECLAIM, "HBO_RECLAIM"},
        {SPINEL_CMD_HBO_DROP, "HBO_DROP"},
        {SPINEL_CMD_HBO_OFFLOADED, "HBO_OFFLOADED"},
        {SPINEL_CMD_HBO_RECLAIMED, "HBO_RECLAIMED"},
        {SPINEL_CMD_HBO_DROPPED, "HBO_DROPPED"},
        {SPINEL_CMD_PEEK, "PEEK"},
        {SPINEL_CMD_PEEK_RET, "PEEK_RET"},
        {SPINEL_CMD_POKE, "POKE"},
        {SPINEL_CMD_PROP_VALUE_MULTI_GET, "PROP_VALUE_MULTI_GET"},
        {SPINEL_CMD_PROP_VALUE_MULTI_SET, "PROP_VALUE_MULTI_SET"},
        {SPINEL_CMD_PROP_VALUES_ARE, "PROP_VALUES_ARE"},
        {0, NULL},
    };

    return spinel_to_cstr(spinel_commands_cstr, command);
}

const char *spinel_prop_key_to_cstr(spinel_prop_key_t prop_key)
{
    static const struct spinel_cstr spinel_prop_cstr[] = {
        {SPINEL_PROP_LAST_STATUS, "LAST_STATUS"},
        {SPINEL_PROP_PROTOCOL_VERSION, "PROTOCOL_VERSION"},
        {SPINEL_PROP_NCP_VERSION, "NCP_VERSION"},
        {SPINEL_PROP_INTERFACE_TYPE, "INTERFACE_TYPE"},
        {SPINEL_PROP_VENDOR_ID, "VENDOR_ID"},
        {SPINEL_PROP_CAPS, "CAPS"},
        {SPINEL_PROP_INTERFACE_COUNT, "INTERFACE_COUNT"},
        {SPINEL_PROP_POWER_STATE, "POWER_STATE"},
        {SPINEL_PROP_HWADDR, "HWADDR"},
        {SPINEL_PROP_LOCK, "LOCK"},
        {SPINEL_PROP_HBO_MEM_MAX, "HBO_MEM_MAX"},
        {SPINEL_PROP_HBO_BLOCK_MAX, "HBO_BLOCK_MAX"},
        {SPINEL_PROP_HOST_POWER_STATE, "HOST_POWER_STATE"},
        {SPINEL_PROP_MCU_POWER_STATE, "MCU_POWER_STATE"},
        {SPINEL_PROP_GPIO_CONFIG, "GPIO_CONFIG"},
        {SPINEL_PROP_GPIO_STATE, "GPIO_STATE"},
        {SPINEL_PROP_GPIO_STATE_SET, "GPIO_STATE_SET"},
        {SPINEL_PROP_GPIO_STATE_CLEAR, "GPIO_STATE_CLEAR"},
        {SPINEL_PROP_TRNG_32, "TRNG_32"},
        {SPINEL_PROP_TRNG_128, "TRNG_128"},
        {SPINEL_PROP_TRNG_RAW_32, "TRNG_RAW_32"},
        {SPINEL_PROP_UNSOL_UPDATE_FILTER, "UNSOL_UPDATE_FILTER"},
        {SPINEL_PROP_UNSOL_UPDATE_LIST, "UNSOL_UPDATE_LIST"},
        {SPINEL_PROP_PHY_ENABLED, "PHY_ENABLED"},
        {SPINEL_PROP_PHY_CHAN, "PHY_CHAN"},
        {SPINEL_PROP_PHY_CHAN_SUPPORTED, "PHY_CHAN_SUPPORTED"},
        {SPINEL_PROP_PHY_FREQ, "PHY_FREQ"},
        {SPINEL_PROP_PHY_CCA_THRESHOLD, "PHY_CCA_THRESHOLD"},
        {SPINEL_PROP_PHY_TX_POWER, "PHY_TX_POWER"},
        {SPINEL_PROP_PHY_FEM_LNA_GAIN, "PHY_FEM_LNA_GAIN"},
        {SPINEL_PROP_PHY_RSSI, "PHY_RSSI"},
        {SPINEL_PROP_PHY_RX_SENSITIVITY, "PHY_RX_SENSITIVITY"},
        {SPINEL_PROP_PHY_PCAP_ENABLED, "PHY_PCAP_ENABLED"},
        {SPINEL_PROP_PHY_CHAN_PREFERRED, "PHY_CHAN_PREFERRED"},
        {SPINEL_PROP_PHY_CHAN_MAX_POWER, "PHY_CHAN_MAX_POWER"},
        {SPINEL_PROP_JAM_DETECT_ENABLE, "JAM_DETECT_ENABLE"},
        {SPINEL_PROP_JAM_DETECTED, "JAM_DETECTED"},
        {SPINEL_PROP_JAM_DETECT_RSSI_THRESHOLD, "JAM_DETECT_RSSI_THRESHOLD"},
        {SPINEL_PROP_JAM_DETECT_WINDOW, "JAM_DETECT_WINDOW"},
        {SPINEL_PROP_JAM_DETECT_BUSY, "JAM_DETECT_BUSY"},
        {SPINEL_PROP_JAM_DETECT_HISTORY_BITMAP, "JAM_DETECT_HISTORY_BITMAP"},
        {SPINEL_PROP_CHANNEL_MONITOR_SAMPLE_INTERVAL, "CHANNEL_MONITOR_SAMPLE_INTERVAL"},
        {SPINEL_PROP_CHANNEL_MONITOR_RSSI_THRESHOLD, "CHANNEL_MONITOR_RSSI_THRESHOLD"},
        {SPINEL_PROP_CHANNEL_MONITOR_SAMPLE_WINDOW, "CHANNEL_MONITOR_SAMPLE_WINDOW"},
        {SPINEL_PROP_CHANNEL_MONITOR_SAMPLE_COUNT, "CHANNEL_MONITOR_SAMPLE_COUNT"},
        {SPINEL_PROP_CHANNEL_MONITOR_CHANNEL_OCCUPANCY, "CHANNEL_MONITOR_CHANNEL_OCCUPANCY"},
        {SPINEL_PROP_RADIO_CAPS, "RADIO_CAPS"},
        {SPINEL_PROP_RADIO_COEX_METRICS, "RADIO_COEX_METRICS"},
        {SPINEL_PROP_RADIO_COEX_ENABLE, "RADIO_COEX_ENABLE"},
        {SPINEL_PROP_MAC_SCAN_STATE, "MAC_SCAN_STATE"},
        {SPINEL_PROP_MAC_SCAN_MASK, "MAC_SCAN_MASK"},
        {SPINEL_PROP_MAC_SCAN_PERIOD, "MAC_SCAN_PERIOD"},
        {SPINEL_PROP_MAC_SCAN_BEACON, "MAC_SCAN_BEACON"},
        {SPINEL_PROP_MAC_15_4_LADDR, "MAC_15_4_LADDR"},
        {SPINEL_PROP_MAC_15_4_SADDR, "MAC_15_4_SADDR"},
        {SPINEL_PROP_MAC_15_4_PANID, "MAC_15_4_PANID"},
        {SPINEL_PROP_MAC_RAW_STREAM_ENABLED, "MAC_RAW_STREAM_ENABLED"},
        {SPINEL_PROP_MAC_PROMISCUOUS_MODE, "MAC_PROMISCUOUS_MODE"},
        {SPINEL_PROP_MAC_ENERGY_SCAN_RESULT, "MAC_ENERGY_SCAN_RESULT"},
        {SPINEL_PROP_MAC_DATA_POLL_PERIOD, "MAC_DATA_POLL_PERIOD"},
        {SPINEL_PROP_MAC_ALLOWLIST, "MAC_ALLOWLIST"},
        {SPINEL_PROP_MAC_ALLOWLIST_ENABLED, "MAC_ALLOWLIST_ENABLED"},
        {SPINEL_PROP_MAC_EXTENDED_ADDR, "MAC_EXTENDED_ADDR"},
        {SPINEL_PROP_MAC_SRC_MATCH_ENABLED, "MAC_SRC_MATCH_ENABLED"},
        {SPINEL_PROP_MAC_SRC_MATCH_SHORT_ADDRESSES, "MAC_SRC_MATCH_SHORT_ADDRESSES"},
        {SPINEL_PROP_MAC_SRC_MATCH_EXTENDED_ADDRESSES, "MAC_SRC_MATCH_EXTENDED_ADDRESSES"},
        {SPINEL_PROP_MAC_DENYLIST, "MAC_DENYLIST"},
        {SPINEL_PROP_MAC_DENYLIST_ENABLED, "MAC_DENYLIST_ENABLED"},
        {SPINEL_PROP_MAC_FIXED_RSS, "MAC_FIXED_RSS"},
        {SPINEL_PROP_MAC_CCA_FAILURE_RATE, "MAC_CCA_FAILURE_RATE"},
        {SPINEL_PROP_MAC_MAX_RETRY_NUMBER_DIRECT, "MAC_MAX_RETRY_NUMBER_DIRECT"},
        {SPINEL_PROP_MAC_MAX_RETRY_NUMBER_INDIRECT, "MAC_MAX_RETRY_NUMBER_INDIRECT"},
        {SPINEL_PROP_NET_SAVED, "NET_SAVED"},
        {SPINEL_PROP_NET_IF_UP, "NET_IF_UP"},
        {SPINEL_PROP_NET_STACK_UP, "NET_STACK_UP"},
        {SPINEL_PROP_NET_ROLE, "NET_ROLE"},
        {SPINEL_PROP_NET_NETWORK_NAME, "NET_NETWORK_NAME"},
        {SPINEL_PROP_NET_XPANID, "NET_XPANID"},
        {SPINEL_PROP_NET_NETWORK_KEY, "NET_NETWORK_KEY"},
        {SPINEL_PROP_NET_KEY_SEQUENCE_COUNTER, "NET_KEY_SEQUENCE_COUNTER"},
        {SPINEL_PROP_NET_PARTITION_ID, "NET_PARTITION_ID"},
        {SPINEL_PROP_NET_REQUIRE_JOIN_EXISTING, "NET_REQUIRE_JOIN_EXISTING"},
        {SPINEL_PROP_NET_KEY_SWITCH_GUARDTIME, "NET_KEY_SWITCH_GUARDTIME"},
        {SPINEL_PROP_NET_PSKC, "NET_PSKC"},
        {SPINEL_PROP_THREAD_LEADER_ADDR, "THREAD_LEADER_ADDR"},
        {SPINEL_PROP_THREAD_PARENT, "THREAD_PARENT"},
        {SPINEL_PROP_THREAD_CHILD_TABLE, "THREAD_CHILD_TABLE"},
        {SPINEL_PROP_THREAD_LEADER_RID, "THREAD_LEADER_RID"},
        {SPINEL_PROP_THREAD_LEADER_WEIGHT, "THREAD_LEADER_WEIGHT"},
        {SPINEL_PROP_THREAD_LOCAL_LEADER_WEIGHT, "THREAD_LOCAL_LEADER_WEIGHT"},
        {SPINEL_PROP_THREAD_NETWORK_DATA, "THREAD_NETWORK_DATA"},
        {SPINEL_PROP_THREAD_NETWORK_DATA_VERSION, "THREAD_NETWORK_DATA_VERSION"},
        {SPINEL_PROP_THREAD_STABLE_NETWORK_DATA, "THREAD_STABLE_NETWORK_DATA"},
        {SPINEL_PROP_THREAD_STABLE_NETWORK_DATA_VERSION, "THREAD_STABLE_NETWORK_DATA_VERSION"},
        {SPINEL_PROP_THREAD_ON_MESH_NETS, "THREAD_ON_MESH_NETS"},
        {SPINEL_PROP_THREAD_OFF_MESH_ROUTES, "THREAD_OFF_MESH_ROUTES"},
        {SPINEL_PROP_THREAD_ASSISTING_PORTS, "THREAD_ASSISTING_PORTS"},
        {SPINEL_PROP_THREAD_ALLOW_LOCAL_NET_DATA_CHANGE, "THREAD_ALLOW_LOCAL_NET_DATA_CHANGE"},
        {SPINEL_PROP_THREAD_MODE, "THREAD_MODE"},
        {SPINEL_PROP_THREAD_CHILD_TIMEOUT, "THREAD_CHILD_TIMEOUT"},
        {SPINEL_PROP_THREAD_RLOC16, "THREAD_RLOC16"},
        {SPINEL_PROP_THREAD_ROUTER_UPGRADE_THRESHOLD, "THREAD_ROUTER_UPGRADE_THRESHOLD"},
        {SPINEL_PROP_THREAD_CONTEXT_REUSE_DELAY, "THREAD_CONTEXT_REUSE_DELAY"},
        {SPINEL_PROP_THREAD_NETWORK_ID_TIMEOUT, "THREAD_NETWORK_ID_TIMEOUT"},
        {SPINEL_PROP_THREAD_ACTIVE_ROUTER_IDS, "THREAD_ACTIVE_ROUTER_IDS"},
        {SPINEL_PROP_THREAD_RLOC16_DEBUG_PASSTHRU, "THREAD_RLOC16_DEBUG_PASSTHRU"},
        {SPINEL_PROP_THREAD_ROUTER_ROLE_ENABLED, "THREAD_ROUTER_ROLE_ENABLED"},
        {SPINEL_PROP_THREAD_ROUTER_DOWNGRADE_THRESHOLD, "THREAD_ROUTER_DOWNGRADE_THRESHOLD"},
        {SPINEL_PROP_THREAD_ROUTER_SELECTION_JITTER, "THREAD_ROUTER_SELECTION_JITTER"},
        {SPINEL_PROP_THREAD_PREFERRED_ROUTER_ID, "THREAD_PREFERRED_ROUTER_ID"},
        {SPINEL_PROP_THREAD_NEIGHBOR_TABLE, "THREAD_NEIGHBOR_TABLE"},
        {SPINEL_PROP_THREAD_CHILD_COUNT_MAX, "THREAD_CHILD_COUNT_MAX"},
        {SPINEL_PROP_THREAD_LEADER_NETWORK_DATA, "THREAD_LEADER_NETWORK_DATA"},
        {SPINEL_PROP_THREAD_STABLE_LEADER_NETWORK_DATA, "THREAD_STABLE_LEADER_NETWORK_DATA"},
        {SPINEL_PROP_THREAD_JOINERS, "THREAD_JOINERS"},
        {SPINEL_PROP_THREAD_COMMISSIONER_ENABLED, "THREAD_COMMISSIONER_ENABLED"},
        {SPINEL_PROP_THREAD_TMF_PROXY_ENABLED, "THREAD_TMF_PROXY_ENABLED"},
        {SPINEL_PROP_THREAD_TMF_PROXY_STREAM, "THREAD_TMF_PROXY_STREAM"},
        {SPINEL_PROP_THREAD_UDP_FORWARD_STREAM, "THREAD_UDP_FORWARD_STREAM"},
        {SPINEL_PROP_THREAD_DISCOVERY_SCAN_JOINER_FLAG, "THREAD_DISCOVERY_SCAN_JOINER_FLAG"},
        {SPINEL_PROP_THREAD_DISCOVERY_SCAN_ENABLE_FILTERING, "THREAD_DISCOVERY_SCAN_ENABLE_FILTERING"},
        {SPINEL_PROP_THREAD_DISCOVERY_SCAN_PANID, "THREAD_DISCOVERY_SCAN_PANID"},
        {SPINEL_PROP_THREAD_STEERING_DATA, "THREAD_STEERING_DATA"},
        {SPINEL_PROP_THREAD_ROUTER_TABLE, "THREAD_ROUTER_TABLE"},
        {SPINEL_PROP_THREAD_ACTIVE_DATASET, "THREAD_ACTIVE_DATASET"},
        {SPINEL_PROP_THREAD_PENDING_DATASET, "THREAD_PENDING_DATASET"},
        {SPINEL_PROP_THREAD_MGMT_SET_ACTIVE_DATASET, "THREAD_MGMT_SET_ACTIVE_DATASET"},
        {SPINEL_PROP_THREAD_MGMT_SET_PENDING_DATASET, "THREAD_MGMT_SET_PENDING_DATASET"},
        {SPINEL_PROP_DATASET_ACTIVE_TIMESTAMP, "DATASET_ACTIVE_TIMESTAMP"},
        {SPINEL_PROP_DATASET_PENDING_TIMESTAMP, "DATASET_PENDING_TIMESTAMP"},
        {SPINEL_PROP_DATASET_DELAY_TIMER, "DATASET_DELAY_TIMER"},
        {SPINEL_PROP_DATASET_SECURITY_POLICY, "DATASET_SECURITY_POLICY"},
        {SPINEL_PROP_DATASET_RAW_TLVS, "DATASET_RAW_TLVS"},
        {SPINEL_PROP_THREAD_CHILD_TABLE_ADDRESSES, "THREAD_CHILD_TABLE_ADDRESSES"},
        {SPINEL_PROP_THREAD_NEIGHBOR_TABLE_ERROR_RATES, "THREAD_NEIGHBOR_TABLE_ERROR_RATES"},
        {SPINEL_PROP_THREAD_ADDRESS_CACHE_TABLE, "THREAD_ADDRESS_CACHE_TABLE"},
        {SPINEL_PROP_THREAD_MGMT_GET_ACTIVE_DATASET, "THREAD_MGMT_GET_ACTIVE_DATASET"},
        {SPINEL_PROP_THREAD_MGMT_GET_PENDING_DATASET, "THREAD_MGMT_GET_PENDING_DATASET"},
        {SPINEL_PROP_DATASET_DEST_ADDRESS, "DATASET_DEST_ADDRESS"},
        {SPINEL_PROP_THREAD_NEW_DATASET, "THREAD_NEW_DATASET"},
        {SPINEL_PROP_THREAD_CSL_PERIOD, "THREAD_CSL_PERIOD"},
        {SPINEL_PROP_THREAD_CSL_TIMEOUT, "THREAD_CSL_TIMEOUT"},
        {SPINEL_PROP_THREAD_CSL_CHANNEL, "THREAD_CSL_CHANNEL"},
        {SPINEL_PROP_THREAD_DOMAIN_NAME, "THREAD_DOMAIN_NAME"},
        {SPINEL_PROP_THREAD_LINK_METRICS_QUERY, "THREAD_LINK_METRICS_QUERY"},
        {SPINEL_PROP_THREAD_LINK_METRICS_QUERY_RESULT, "THREAD_LINK_METRICS_QUERY_RESULT"},
        {SPINEL_PROP_THREAD_LINK_METRICS_PROBE, "THREAD_LINK_METRICS_PROBE"},
        {SPINEL_PROP_THREAD_LINK_METRICS_MGMT_ENH_ACK, "THREAD_LINK_METRICS_MGMT_ENH_ACK"},
        {SPINEL_PROP_THREAD_LINK_METRICS_MGMT_ENH_ACK_IE, "THREAD_LINK_METRICS_MGMT_ENH_ACK_IE"},
        {SPINEL_PROP_THREAD_LINK_METRICS_MGMT_FORWARD, "THREAD_LINK_METRICS_MGMT_FORWARD"},
        {SPINEL_PROP_THREAD_LINK_METRICS_MGMT_RESPONSE, "THREAD_LINK_METRICS_MGMT_RESPONSE"},
        {SPINEL_PROP_THREAD_MLR_REQUEST, "THREAD_MLR_REQUEST"},
        {SPINEL_PROP_THREAD_MLR_RESPONSE, "THREAD_MLR_RESPONSE"},
        {SPINEL_PROP_THREAD_BACKBONE_ROUTER_PRIMARY, "THREAD_BACKBONE_ROUTER_PRIMARY"},
        {SPINEL_PROP_THREAD_BACKBONE_ROUTER_LOCAL_STATE, "THREAD_BACKBONE_ROUTER_LOCAL_STATE"},
        {SPINEL_PROP_THREAD_BACKBONE_ROUTER_LOCAL_CONFIG, "THREAD_BACKBONE_ROUTER_LOCAL_CONFIG"},
        {SPINEL_PROP_THREAD_BACKBONE_ROUTER_LOCAL_REGISTER, "THREAD_BACKBONE_ROUTER_LOCAL_REGISTER"},
        {SPINEL_PROP_THREAD_BACKBONE_ROUTER_LOCAL_REGISTRATION_JITTER,
         "THREAD_BACKBONE_ROUTER_LOCAL_REGISTRATION_JITTER"},
        {SPINEL_PROP_MESHCOP_JOINER_STATE, "MESHCOP_JOINER_STATE"},
        {SPINEL_PROP_MESHCOP_JOINER_COMMISSIONING, "MESHCOP_JOINER_COMMISSIONING"},
        {SPINEL_PROP_IPV6_LL_ADDR, "IPV6_LL_ADDR"},
        {SPINEL_PROP_IPV6_ML_ADDR, "IPV6_ML_ADDR"},
        {SPINEL_PROP_IPV6_ML_PREFIX, "IPV6_ML_PREFIX"},
        {SPINEL_PROP_IPV6_ADDRESS_TABLE, "IPV6_ADDRESS_TABLE"},
        {SPINEL_PROP_IPV6_ROUTE_TABLE, "IPV6_ROUTE_TABLE"},
        {SPINEL_PROP_IPV6_ICMP_PING_OFFLOAD, "IPV6_ICMP_PING_OFFLOAD"},
        {SPINEL_PROP_IPV6_MULTICAST_ADDRESS_TABLE, "IPV6_MULTICAST_ADDRESS_TABLE"},
        {SPINEL_PROP_IPV6_ICMP_PING_OFFLOAD_MODE, "IPV6_ICMP_PING_OFFLOAD_MODE"},
        {SPINEL_PROP_STREAM_DEBUG, "STREAM_DEBUG"},
        {SPINEL_PROP_STREAM_RAW, "STREAM_RAW"},
        {SPINEL_PROP_STREAM_NET, "STREAM_NET"},
        {SPINEL_PROP_STREAM_NET_INSECURE, "STREAM_NET_INSECURE"},
        {SPINEL_PROP_STREAM_LOG, "STREAM_LOG"},
        {SPINEL_PROP_MESHCOP_COMMISSIONER_STATE, "MESHCOP_COMMISSIONER_STATE"},
        {SPINEL_PROP_MESHCOP_COMMISSIONER_JOINERS, "MESHCOP_COMMISSIONER_JOINERS"},
        {SPINEL_PROP_MESHCOP_COMMISSIONER_PROVISIONING_URL, "MESHCOP_COMMISSIONER_PROVISIONING_URL"},
        {SPINEL_PROP_MESHCOP_COMMISSIONER_SESSION_ID, "MESHCOP_COMMISSIONER_SESSION_ID"},
        {SPINEL_PROP_MESHCOP_JOINER_DISCERNER, "MESHCOP_JOINER_DISCERNER"},
        {SPINEL_PROP_MESHCOP_COMMISSIONER_ANNOUNCE_BEGIN, "MESHCOP_COMMISSIONER_ANNOUNCE_BEGIN"},
        {SPINEL_PROP_MESHCOP_COMMISSIONER_ENERGY_SCAN, "MESHCOP_COMMISSIONER_ENERGY_SCAN"},
        {SPINEL_PROP_MESHCOP_COMMISSIONER_ENERGY_SCAN_RESULT, "MESHCOP_COMMISSIONER_ENERGY_SCAN_RESULT"},
        {SPINEL_PROP_MESHCOP_COMMISSIONER_PAN_ID_QUERY, "MESHCOP_COMMISSIONER_PAN_ID_QUERY"},
        {SPINEL_PROP_MESHCOP_COMMISSIONER_PAN_ID_CONFLICT_RESULT, "MESHCOP_COMMISSIONER_PAN_ID_CONFLICT_RESULT"},
        {SPINEL_PROP_MESHCOP_COMMISSIONER_MGMT_GET, "MESHCOP_COMMISSIONER_MGMT_GET"},
        {SPINEL_PROP_MESHCOP_COMMISSIONER_MGMT_SET, "MESHCOP_COMMISSIONER_MGMT_SET"},
        {SPINEL_PROP_MESHCOP_COMMISSIONER_GENERATE_PSKC, "MESHCOP_COMMISSIONER_GENERATE_PSKC"},
        {SPINEL_PROP_CHANNEL_MANAGER_NEW_CHANNEL, "CHANNEL_MANAGER_NEW_CHANNEL"},
        {SPINEL_PROP_CHANNEL_MANAGER_DELAY, "CHANNEL_MANAGER_DELAY"},
        {SPINEL_PROP_CHANNEL_MANAGER_SUPPORTED_CHANNELS, "CHANNEL_MANAGER_SUPPORTED_CHANNELS"},
        {SPINEL_PROP_CHANNEL_MANAGER_FAVORED_CHANNELS, "CHANNEL_MANAGER_FAVORED_CHANNELS"},
        {SPINEL_PROP_CHANNEL_MANAGER_CHANNEL_SELECT, "CHANNEL_MANAGER_CHANNEL_SELECT"},
        {SPINEL_PROP_CHANNEL_MANAGER_AUTO_SELECT_ENABLED, "CHANNEL_MANAGER_AUTO_SELECT_ENABLED"},
        {SPINEL_PROP_CHANNEL_MANAGER_AUTO_SELECT_INTERVAL, "CHANNEL_MANAGER_AUTO_SELECT_INTERVAL"},
        {SPINEL_PROP_THREAD_NETWORK_TIME, "THREAD_NETWORK_TIME"},
        {SPINEL_PROP_TIME_SYNC_PERIOD, "TIME_SYNC_PERIOD"},
        {SPINEL_PROP_TIME_SYNC_XTAL_THRESHOLD, "TIME_SYNC_XTAL_THRESHOLD"},
        {SPINEL_PROP_CHILD_SUPERVISION_INTERVAL, "CHILD_SUPERVISION_INTERVAL"},
        {SPINEL_PROP_CHILD_SUPERVISION_CHECK_TIMEOUT, "CHILD_SUPERVISION_CHECK_TIMEOUT"},
        {SPINEL_PROP_RCP_VERSION, "RCP_VERSION"},
        {SPINEL_PROP_PARENT_RESPONSE_INFO, "PARENT_RESPONSE_INFO"},
        {SPINEL_PROP_SLAAC_ENABLED, "SLAAC_ENABLED"},
        {SPINEL_PROP_SUPPORTED_RADIO_LINKS, "SUPPORTED_RADIO_LINKS"},
        {SPINEL_PROP_NEIGHBOR_TABLE_MULTI_RADIO_INFO, "NEIGHBOR_TABLE_MULTI_RADIO_INFO"},
        {SPINEL_PROP_SRP_CLIENT_START, "SRP_CLIENT_START"},
        {SPINEL_PROP_SRP_CLIENT_LEASE_INTERVAL, "SRP_CLIENT_LEASE_INTERVAL"},
        {SPINEL_PROP_SRP_CLIENT_KEY_LEASE_INTERVAL, "SRP_CLIENT_KEY_LEASE_INTERVAL"},
        {SPINEL_PROP_SRP_CLIENT_HOST_INFO, "SRP_CLIENT_HOST_INFO"},
        {SPINEL_PROP_SRP_CLIENT_HOST_NAME, "SRP_CLIENT_HOST_NAME"},
        {SPINEL_PROP_SRP_CLIENT_HOST_ADDRESSES, "SRP_CLIENT_HOST_ADDRESSES"},
        {SPINEL_PROP_SRP_CLIENT_SERVICES, "SRP_CLIENT_SERVICES"},
        {SPINEL_PROP_SRP_CLIENT_HOST_SERVICES_REMOVE, "SRP_CLIENT_HOST_SERVICES_REMOVE"},
        {SPINEL_PROP_SRP_CLIENT_HOST_SERVICES_CLEAR, "SRP_CLIENT_HOST_SERVICES_CLEAR"},
        {SPINEL_PROP_SRP_CLIENT_EVENT, "SRP_CLIENT_EVENT"},
        {SPINEL_PROP_SRP_CLIENT_SERVICE_KEY_ENABLED, "SRP_CLIENT_SERVICE_KEY_ENABLED"},
        {SPINEL_PROP_SERVER_ALLOW_LOCAL_DATA_CHANGE, "SERVER_ALLOW_LOCAL_DATA_CHANGE"},
        {SPINEL_PROP_SERVER_SERVICES, "SERVER_SERVICES"},
        {SPINEL_PROP_SERVER_LEADER_SERVICES, "SERVER_LEADER_SERVICES"},
        {SPINEL_PROP_RCP_API_VERSION, "RCP_API_VERSION"},
        {SPINEL_PROP_UART_BITRATE, "UART_BITRATE"},
        {SPINEL_PROP_UART_XON_XOFF, "UART_XON_XOFF"},
        {SPINEL_PROP_15_4_PIB_PHY_CHANNELS_SUPPORTED, "15_4_PIB_PHY_CHANNELS_SUPPORTED"},
        {SPINEL_PROP_15_4_PIB_MAC_PROMISCUOUS_MODE, "15_4_PIB_MAC_PROMISCUOUS_MODE"},
        {SPINEL_PROP_15_4_PIB_MAC_SECURITY_ENABLED, "15_4_PIB_MAC_SECURITY_ENABLED"},
        {SPINEL_PROP_CNTR_RESET, "CNTR_RESET"},
        {SPINEL_PROP_CNTR_TX_PKT_TOTAL, "CNTR_TX_PKT_TOTAL"},
        {SPINEL_PROP_CNTR_TX_PKT_ACK_REQ, "CNTR_TX_PKT_ACK_REQ"},
        {SPINEL_PROP_CNTR_TX_PKT_ACKED, "CNTR_TX_PKT_ACKED"},
        {SPINEL_PROP_CNTR_TX_PKT_NO_ACK_REQ, "CNTR_TX_PKT_NO_ACK_REQ"},
        {SPINEL_PROP_CNTR_TX_PKT_DATA, "CNTR_TX_PKT_DATA"},
        {SPINEL_PROP_CNTR_TX_PKT_DATA_POLL, "CNTR_TX_PKT_DATA_POLL"},
        {SPINEL_PROP_CNTR_TX_PKT_BEACON, "CNTR_TX_PKT_BEACON"},
        {SPINEL_PROP_CNTR_TX_PKT_BEACON_REQ, "CNTR_TX_PKT_BEACON_REQ"},
        {SPINEL_PROP_CNTR_TX_PKT_OTHER, "CNTR_TX_PKT_OTHER"},
        {SPINEL_PROP_CNTR_TX_PKT_RETRY, "CNTR_TX_PKT_RETRY"},
        {SPINEL_PROP_CNTR_TX_ERR_CCA, "CNTR_TX_ERR_CCA"},
        {SPINEL_PROP_CNTR_TX_PKT_UNICAST, "CNTR_TX_PKT_UNICAST"},
        {SPINEL_PROP_CNTR_TX_PKT_BROADCAST, "CNTR_TX_PKT_BROADCAST"},
        {SPINEL_PROP_CNTR_TX_ERR_ABORT, "CNTR_TX_ERR_ABORT"},
        {SPINEL_PROP_CNTR_RX_PKT_TOTAL, "CNTR_RX_PKT_TOTAL"},
        {SPINEL_PROP_CNTR_RX_PKT_DATA, "CNTR_RX_PKT_DATA"},
        {SPINEL_PROP_CNTR_RX_PKT_DATA_POLL, "CNTR_RX_PKT_DATA_POLL"},
        {SPINEL_PROP_CNTR_RX_PKT_BEACON, "CNTR_RX_PKT_BEACON"},
        {SPINEL_PROP_CNTR_RX_PKT_BEACON_REQ, "CNTR_RX_PKT_BEACON_REQ"},
        {SPINEL_PROP_CNTR_RX_PKT_OTHER, "CNTR_RX_PKT_OTHER"},
        {SPINEL_PROP_CNTR_RX_PKT_FILT_WL, "CNTR_RX_PKT_FILT_WL"},
        {SPINEL_PROP_CNTR_RX_PKT_FILT_DA, "CNTR_RX_PKT_FILT_DA"},
        {SPINEL_PROP_CNTR_RX_ERR_EMPTY, "CNTR_RX_ERR_EMPTY"},
        {SPINEL_PROP_CNTR_RX_ERR_UKWN_NBR, "CNTR_RX_ERR_UKWN_NBR"},
        {SPINEL_PROP_CNTR_RX_ERR_NVLD_SADDR, "CNTR_RX_ERR_NVLD_SADDR"},
        {SPINEL_PROP_CNTR_RX_ERR_SECURITY, "CNTR_RX_ERR_SECURITY"},
        {SPINEL_PROP_CNTR_RX_ERR_BAD_FCS, "CNTR_RX_ERR_BAD_FCS"},
        {SPINEL_PROP_CNTR_RX_ERR_OTHER, "CNTR_RX_ERR_OTHER"},
        {SPINEL_PROP_CNTR_RX_PKT_DUP, "CNTR_RX_PKT_DUP"},
        {SPINEL_PROP_CNTR_RX_PKT_UNICAST, "CNTR_RX_PKT_UNICAST"},
        {SPINEL_PROP_CNTR_RX_PKT_BROADCAST, "CNTR_RX_PKT_BROADCAST"},
        {SPINEL_PROP_CNTR_TX_IP_SEC_TOTAL, "CNTR_TX_IP_SEC_TOTAL"},
        {SPINEL_PROP_CNTR_TX_IP_INSEC_TOTAL, "CNTR_TX_IP_INSEC_TOTAL"},
        {SPINEL_PROP_CNTR_TX_IP_DROPPED, "CNTR_TX_IP_DROPPED"},
        {SPINEL_PROP_CNTR_RX_IP_SEC_TOTAL, "CNTR_RX_IP_SEC_TOTAL"},
        {SPINEL_PROP_CNTR_RX_IP_INSEC_TOTAL, "CNTR_RX_IP_INSEC_TOTAL"},
        {SPINEL_PROP_CNTR_RX_IP_DROPPED, "CNTR_RX_IP_DROPPED"},
        {SPINEL_PROP_CNTR_TX_SPINEL_TOTAL, "CNTR_TX_SPINEL_TOTAL"},
        {SPINEL_PROP_CNTR_RX_SPINEL_TOTAL, "CNTR_RX_SPINEL_TOTAL"},
        {SPINEL_PROP_CNTR_RX_SPINEL_ERR, "CNTR_RX_SPINEL_ERR"},
        {SPINEL_PROP_CNTR_RX_SPINEL_OUT_OF_ORDER_TID, "CNTR_RX_SPINEL_OUT_OF_ORDER_TID"},
        {SPINEL_PROP_CNTR_IP_TX_SUCCESS, "CNTR_IP_TX_SUCCESS"},
        {SPINEL_PROP_CNTR_IP_RX_SUCCESS, "CNTR_IP_RX_SUCCESS"},
        {SPINEL_PROP_CNTR_IP_TX_FAILURE, "CNTR_IP_TX_FAILURE"},
        {SPINEL_PROP_CNTR_IP_RX_FAILURE, "CNTR_IP_RX_FAILURE"},
        {SPINEL_PROP_MSG_BUFFER_COUNTERS, "MSG_BUFFER_COUNTERS"},
        {SPINEL_PROP_CNTR_ALL_MAC_COUNTERS, "CNTR_ALL_MAC_COUNTERS"},
        {SPINEL_PROP_CNTR_MLE_COUNTERS, "CNTR_MLE_COUNTERS"},
        {SPINEL_PROP_CNTR_ALL_IP_COUNTERS, "CNTR_ALL_IP_COUNTERS"},
        {SPINEL_PROP_CNTR_MAC_RETRY_HISTOGRAM, "CNTR_MAC_RETRY_HISTOGRAM"},
        {SPINEL_PROP_NEST_STREAM_MFG, "NEST_STREAM_MFG"},
        {SPINEL_PROP_NEST_LEGACY_ULA_PREFIX, "NEST_LEGACY_ULA_PREFIX"},
        {SPINEL_PROP_NEST_LEGACY_LAST_NODE_JOINED, "NEST_LEGACY_LAST_NODE_JOINED"},
        {SPINEL_PROP_DEBUG_TEST_ASSERT, "DEBUG_TEST_ASSERT"},
        {SPINEL_PROP_DEBUG_NCP_LOG_LEVEL, "DEBUG_NCP_LOG_LEVEL"},
        {SPINEL_PROP_DEBUG_TEST_WATCHDOG, "DEBUG_TEST_WATCHDOG"},
        {SPINEL_PROP_RCP_MAC_FRAME_COUNTER, "RCP_MAC_FRAME_COUNTER"},
        {SPINEL_PROP_RCP_MAC_KEY, "RCP_MAC_KEY"},
        {SPINEL_PROP_DEBUG_LOG_TIMESTAMP_BASE, "DEBUG_LOG_TIMESTAMP_BASE"},
        {SPINEL_PROP_DEBUG_TREL_TEST_MODE_ENABLE, "DEBUG_TREL_TEST_MODE_ENABLE"},
        {0, NULL},
    };

    return spinel_to_cstr(spinel_prop_cstr, prop_key);
}

const char *spinel_net_role_to_cstr(uint8_t net_role)
{
    static const struct spinel_cstr spinel_net_cstr[] = {
        {SPINEL_NET_ROLE_DETACHED, "NET_ROLE_DETACHED"},
        {SPINEL_NET_ROLE_CHILD, "NET_ROLE_CHILD"},
        {SPINEL_NET_ROLE_ROUTER, "NET_ROLE_ROUTER"},
        {SPINEL_NET_ROLE_LEADER, "NET_ROLE_LEADER"},
        {0, NULL},
    };

    return spinel_to_cstr(spinel_net_cstr, net_role);
}

const char *spinel_mcu_power_state_to_cstr(uint8_t mcu_power_state)
{
    static const struct spinel_cstr spinel_mcu_power_state_cstr[] = {
        {SPINEL_MCU_POWER_STATE_ON, "MCU_POWER_STATE_ON"},
        {SPINEL_MCU_POWER_STATE_LOW_POWER, "MCU_POWER_STATE_LOW_POWER"},
        {SPINEL_MCU_POWER_STATE_OFF, "MCU_POWER_STATE_OFF"},
        {0, NULL},
    };

    return spinel_to_cstr(spinel_mcu_power_state_cstr, mcu_power_state);
}

const char *spinel_status_to_cstr(spinel_status_t status)
{
    static const struct spinel_cstr spinel_status_cstr[] = {
        {SPINEL_STATUS_OK, "OK"},
        {SPINEL_STATUS_FAILURE, "FAILURE"},
        {SPINEL_STATUS_UNIMPLEMENTED, "UNIMPLEMENTED"},
        {SPINEL_STATUS_INVALID_ARGUMENT, "INVALID_ARGUMENT"},
        {SPINEL_STATUS_INVALID_STATE, "INVALID_STATE"},
        {SPINEL_STATUS_INVALID_COMMAND, "INVALID_COMMAND"},
        {SPINEL_STATUS_INVALID_INTERFACE, "INVALID_INTERFACE"},
        {SPINEL_STATUS_INTERNAL_ERROR, "INTERNAL_ERROR"},
        {SPINEL_STATUS_SECURITY_ERROR, "SECURITY_ERROR"},
        {SPINEL_STATUS_PARSE_ERROR, "PARSE_ERROR"},
        {SPINEL_STATUS_IN_PROGRESS, "IN_PROGRESS"},
        {SPINEL_STATUS_NOMEM, "NOMEM"},
        {SPINEL_STATUS_BUSY, "BUSY"},
        {SPINEL_STATUS_PROP_NOT_FOUND, "PROP_NOT_FOUND"},
        {SPINEL_STATUS_DROPPED, "DROPPED"},
        {SPINEL_STATUS_EMPTY, "EMPTY"},
        {SPINEL_STATUS_CMD_TOO_BIG, "CMD_TOO_BIG"},
        {SPINEL_STATUS_NO_ACK, "NO_ACK"},
        {SPINEL_STATUS_CCA_FAILURE, "CCA_FAILURE"},
        {SPINEL_STATUS_ALREADY, "ALREADY"},
        {SPINEL_STATUS_ITEM_NOT_FOUND, "ITEM_NOT_FOUND"},
        {SPINEL_STATUS_INVALID_COMMAND_FOR_PROP, "INVALID_COMMAND_FOR_PROP"},
        {SPINEL_STATUS_RESPONSE_TIMEOUT, "RESPONSE_TIMEOUT"},
        {SPINEL_STATUS_JOIN_FAILURE, "JOIN_FAILURE"},
        {SPINEL_STATUS_JOIN_SECURITY, "JOIN_SECURITY"},
        {SPINEL_STATUS_JOIN_NO_PEERS, "JOIN_NO_PEERS"},
        {SPINEL_STATUS_JOIN_INCOMPATIBLE, "JOIN_INCOMPATIBLE"},
        {SPINEL_STATUS_JOIN_RSP_TIMEOUT, "JOIN_RSP_TIMEOUT"},
        {SPINEL_STATUS_JOIN_SUCCESS, "JOIN_SUCCESS"},
        {SPINEL_STATUS_RESET_POWER_ON, "RESET_POWER_ON"},
        {SPINEL_STATUS_RESET_EXTERNAL, "RESET_EXTERNAL"},
        {SPINEL_STATUS_RESET_SOFTWARE, "RESET_SOFTWARE"},
        {SPINEL_STATUS_RESET_FAULT, "RESET_FAULT"},
        {SPINEL_STATUS_RESET_CRASH, "RESET_CRASH"},
        {SPINEL_STATUS_RESET_ASSERT, "RESET_ASSERT"},
        {SPINEL_STATUS_RESET_OTHER, "RESET_OTHER"},
        {SPINEL_STATUS_RESET_UNKNOWN, "RESET_UNKNOWN"},
        {SPINEL_STATUS_RESET_WATCHDOG, "RESET_WATCHDOG"},
        {0, NULL},
    };

    return spinel_to_cstr(spinel_status_cstr, status);
}

const char *spinel_capability_to_cstr(spinel_capability_t capability)
{
    static const struct spinel_cstr spinel_cap_cstr[] = {
        {SPINEL_CAP_LOCK, "LOCK"},
        {SPINEL_CAP_NET_SAVE, "NET_SAVE"},
        {SPINEL_CAP_HBO, "HBO"},
        {SPINEL_CAP_POWER_SAVE, "POWER_SAVE"},
        {SPINEL_CAP_COUNTERS, "COUNTERS"},
        {SPINEL_CAP_JAM_DETECT, "JAM_DETECT"},
        {SPINEL_CAP_PEEK_POKE, "PEEK_POKE"},
        {SPINEL_CAP_WRITABLE_RAW_STREAM, "WRITABLE_RAW_STREAM"},
        {SPINEL_CAP_GPIO, "GPIO"},
        {SPINEL_CAP_TRNG, "TRNG"},
        {SPINEL_CAP_CMD_MULTI, "CMD_MULTI"},
        {SPINEL_CAP_UNSOL_UPDATE_FILTER, "UNSOL_UPDATE_FILTER"},
        {SPINEL_CAP_MCU_POWER_STATE, "MCU_POWER_STATE"},
        {SPINEL_CAP_PCAP, "PCAP"},
        {SPINEL_CAP_802_15_4_2003, "802_15_4_2003"},
        {SPINEL_CAP_802_15_4_2006, "802_15_4_2006"},
        {SPINEL_CAP_802_15_4_2011, "802_15_4_2011"},
        {SPINEL_CAP_802_15_4_PIB, "802_15_4_PIB"},
        {SPINEL_CAP_802_15_4_2450MHZ_OQPSK, "802_15_4_2450MHZ_OQPSK"},
        {SPINEL_CAP_802_15_4_915MHZ_OQPSK, "802_15_4_915MHZ_OQPSK"},
        {SPINEL_CAP_802_15_4_868MHZ_OQPSK, "802_15_4_868MHZ_OQPSK"},
        {SPINEL_CAP_802_15_4_915MHZ_BPSK, "802_15_4_915MHZ_BPSK"},
        {SPINEL_CAP_802_15_4_868MHZ_BPSK, "802_15_4_868MHZ_BPSK"},
        {SPINEL_CAP_802_15_4_915MHZ_ASK, "802_15_4_915MHZ_ASK"},
        {SPINEL_CAP_802_15_4_868MHZ_ASK, "802_15_4_868MHZ_ASK"},
        {SPINEL_CAP_CONFIG_FTD, "CONFIG_FTD"},
        {SPINEL_CAP_CONFIG_MTD, "CONFIG_MTD"},
        {SPINEL_CAP_CONFIG_RADIO, "CONFIG_RADIO"},
        {SPINEL_CAP_ROLE_ROUTER, "ROLE_ROUTER"},
        {SPINEL_CAP_ROLE_SLEEPY, "ROLE_SLEEPY"},
        {SPINEL_CAP_NET_THREAD_1_0, "NET_THREAD_1_0"},
        {SPINEL_CAP_NET_THREAD_1_1, "NET_THREAD_1_1"},
        {SPINEL_CAP_NET_THREAD_1_2, "NET_THREAD_1_2"},
        {SPINEL_CAP_RCP_API_VERSION, "RCP_API_VERSION"},
        {SPINEL_CAP_MAC_ALLOWLIST, "MAC_ALLOWLIST"},
        {SPINEL_CAP_MAC_RAW, "MAC_RAW"},
        {SPINEL_CAP_OOB_STEERING_DATA, "OOB_STEERING_DATA"},
        {SPINEL_CAP_CHANNEL_MONITOR, "CHANNEL_MONITOR"},
        {SPINEL_CAP_CHANNEL_MANAGER, "CHANNEL_MANAGER"},
        {SPINEL_CAP_OPENTHREAD_LOG_METADATA, "OPENTHREAD_LOG_METADATA"},
        {SPINEL_CAP_TIME_SYNC, "TIME_SYNC"},
        {SPINEL_CAP_CHILD_SUPERVISION, "CHILD_SUPERVISION"},
        {SPINEL_CAP_POSIX, "POSIX"},
        {SPINEL_CAP_SLAAC, "SLAAC"},
        {SPINEL_CAP_RADIO_COEX, "RADIO_COEX"},
        {SPINEL_CAP_MAC_RETRY_HISTOGRAM, "MAC_RETRY_HISTOGRAM"},
        {SPINEL_CAP_MULTI_RADIO, "MULTI_RADIO"},
        {SPINEL_CAP_SRP_CLIENT, "SRP_CLIENT"},
        {SPINEL_CAP_DUA, "DUA"},
        {SPINEL_CAP_REFERENCE_DEVICE, "REFERENCE_DEVICE"},
        {SPINEL_CAP_ERROR_RATE_TRACKING, "ERROR_RATE_TRACKING"},
        {SPINEL_CAP_THREAD_COMMISSIONER, "THREAD_COMMISSIONER"},
        {SPINEL_CAP_THREAD_TMF_PROXY, "THREAD_TMF_PROXY"},
        {SPINEL_CAP_THREAD_UDP_FORWARD, "THREAD_UDP_FORWARD"},
        {SPINEL_CAP_THREAD_JOINER, "THREAD_JOINER"},
        {SPINEL_CAP_THREAD_BORDER_ROUTER, "THREAD_BORDER_ROUTER"},
        {SPINEL_CAP_THREAD_SERVICE, "THREAD_SERVICE"},
        {SPINEL_CAP_THREAD_CSL_RECEIVER, "THREAD_CSL_RECEIVER"},
        {SPINEL_CAP_THREAD_BACKBONE_ROUTER, "THREAD_BACKBONE_ROUTER"},
        {SPINEL_CAP_NEST_LEGACY_INTERFACE, "NEST_LEGACY_INTERFACE"},
        {SPINEL_CAP_NEST_LEGACY_NET_WAKE, "NEST_LEGACY_NET_WAKE"},
        {SPINEL_CAP_NEST_TRANSMIT_HOOK, "NEST_TRANSMIT_HOOK"},
        {0, NULL},
    };

    return spinel_to_cstr(spinel_cap_cstr, capability);
}

// LCOV_EXCL_STOP

/* -------------------------------------------------------------------------- */

#if SPINEL_SELF_TEST

int main(void)
{
    int                  ret             = -1;
    const spinel_eui64_t static_eui64    = {{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}};
    const char           static_string[] = "static_string";
    uint8_t              buffer[1024];
    ssize_t              len;

    len =
        spinel_datatype_pack(buffer, sizeof(buffer), "CiiLUE", 0x88, 9, 0xA3, 0xDEADBEEF, static_string, &static_eui64);

    if (len != 30)
    {
        printf("error:%d: len != 30; (%d)\n", __LINE__, (int)len);
        goto bail;
    }

    {
        const char *str = NULL;

        // Length ends right before the string.
        len = spinel_datatype_unpack(buffer, 8, "CiiLU", NULL, NULL, NULL, NULL, &str);

        if (len != -1)
        {
            printf("error:%d: len != -1; (%d)\n", __LINE__, (int)len);
            goto bail;
        }

        if (str != NULL)
        {
            printf("error:%d: str != NULL\n", __LINE__);
            goto bail;
        }
    }

    len = 30;

    {
        uint8_t               c     = 0;
        unsigned int          i1    = 0;
        unsigned int          i2    = 0;
        uint32_t              l     = 0;
        const char *          str   = NULL;
        const spinel_eui64_t *eui64 = NULL;

        len = spinel_datatype_unpack(buffer, (spinel_size_t)len, "CiiLUE", &c, &i1, &i2, &l, &str, &eui64);

        if (len != 30)
        {
            printf("error:%d: len != 30; (%d)\n", __LINE__, (int)len);
            goto bail;
        }

        if (c != 0x88)
        {
            printf("error: x != 0x88; (%d)\n", c);
            goto bail;
        }

        if (i1 != 9)
        {
            printf("error: i1 != 9; (%d)\n", i1);
            goto bail;
        }

        if (i2 != 0xA3)
        {
            printf("error: i2 != 0xA3; (0x%02X)\n", i2);
            goto bail;
        }

        if (l != 0xDEADBEEF)
        {
            printf("error: l != 0xDEADBEEF; (0x%08X)\n", (unsigned int)l);
            goto bail;
        }

        if (strcmp(str, static_string) != 0)
        {
            printf("error:%d: strcmp(str,static_string) != 0\n", __LINE__);
            goto bail;
        }

        if (memcmp(eui64, &static_eui64, sizeof(spinel_eui64_t)) != 0)
        {
            printf("error:%d: memcmp(eui64, &eui64, sizeof(spinel_eui64_t)) != 0\n", __LINE__);
            goto bail;
        }
    }

    {
        uint8_t        c  = 0;
        unsigned int   i1 = 0;
        unsigned int   i2 = 0;
        uint32_t       l  = 0;
        char           str[sizeof(static_string)];
        spinel_eui64_t eui64 = {{0}};

        len = spinel_datatype_unpack_in_place(buffer, (spinel_size_t)len, "CiiLUE", &c, &i1, &i2, &l, &str, sizeof(str),
                                              &eui64);

        if (len != 30)
        {
            printf("error:%d: len != 30; (%d)\n", __LINE__, (int)len);
            goto bail;
        }

        if (c != 0x88)
        {
            printf("error: x != 0x88; (%d)\n", c);
            goto bail;
        }

        if (i1 != 9)
        {
            printf("error: i1 != 9; (%d)\n", i1);
            goto bail;
        }

        if (i2 != 0xA3)
        {
            printf("error: i2 != 0xA3; (0x%02X)\n", i2);
            goto bail;
        }

        if (l != 0xDEADBEEF)
        {
            printf("error: l != 0xDEADBEEF; (0x%08X)\n", (unsigned int)l);
            goto bail;
        }

        if (strcmp(str, static_string) != 0)
        {
            printf("error:%d: strcmp(str,static_string) != 0\n", __LINE__);
            goto bail;
        }

        if (memcmp(&eui64, &static_eui64, sizeof(spinel_eui64_t)) != 0)
        {
            printf("error:%d: memcmp(&eui64, &static_eui64, sizeof(spinel_eui64_t)) != 0\n", __LINE__);
            goto bail;
        }
    }

    // -----------------------------------

    memset(buffer, 0xAA, sizeof(buffer));

    len = spinel_datatype_pack(buffer, sizeof(buffer), "Cit(iL)UE", 0x88, 9, 0xA3, 0xDEADBEEF, static_string,
                               &static_eui64);

    if (len != 32)
    {
        printf("error:%d: len != 32; (%d)\n", __LINE__, (int)len);
        goto bail;
    }

    {
        uint8_t         c     = 0;
        unsigned int    i1    = 0;
        unsigned int    i2    = 0;
        uint32_t        l     = 0;
        const char *    str   = NULL;
        spinel_eui64_t *eui64 = NULL;

        len = spinel_datatype_unpack(buffer, (spinel_size_t)len, "Cit(iL)UE", &c, &i1, &i2, &l, &str, &eui64);

        if (len != 32)
        {
            printf("error:%d: len != 24; (%d)\n", __LINE__, (int)len);
            goto bail;
        }

        if (c != 0x88)
        {
            printf("error: x != 0x88; (%d)\n", c);
            goto bail;
        }

        if (i1 != 9)
        {
            printf("error: i1 != 9; (%d)\n", i1);
            goto bail;
        }

        if (i2 != 0xA3)
        {
            printf("error: i2 != 0xA3; (0x%02X)\n", i2);
            goto bail;
        }

        if (l != 0xDEADBEEF)
        {
            printf("error: l != 0xDEADBEEF; (0x%08X)\n", (unsigned int)l);
            goto bail;
        }

        if (strcmp(str, static_string) != 0)
        {
            printf("error:%d: strcmp(str,static_string) != 0\n", __LINE__);
            goto bail;
        }

        if (memcmp(eui64, &static_eui64, sizeof(spinel_eui64_t)) != 0)
        {
            printf("error:%d: memcmp(eui64, &static_eui64, sizeof(spinel_eui64_t)) != 0\n", __LINE__);
            goto bail;
        }
    }

    {
        uint8_t        c  = 0;
        unsigned int   i1 = 0;
        unsigned int   i2 = 0;
        uint32_t       l  = 0;
        char           str[sizeof(static_string)];
        spinel_eui64_t eui64 = {{0}};

        len = spinel_datatype_unpack_in_place(buffer, (spinel_size_t)len, "Cit(iL)UE", &c, &i1, &i2, &l, &str,
                                              sizeof(str), &eui64);

        if (len != 32)
        {
            printf("error:%d: len != 24; (%d)\n", __LINE__, (int)len);
            goto bail;
        }

        if (c != 0x88)
        {
            printf("error: x != 0x88; (%d)\n", c);
            goto bail;
        }

        if (i1 != 9)
        {
            printf("error: i1 != 9; (%d)\n", i1);
            goto bail;
        }

        if (i2 != 0xA3)
        {
            printf("error: i2 != 0xA3; (0x%02X)\n", i2);
            goto bail;
        }

        if (l != 0xDEADBEEF)
        {
            printf("error: l != 0xDEADBEEF; (0x%08X)\n", (unsigned int)l);
            goto bail;
        }

        if (strcmp(str, static_string) != 0)
        {
            printf("error:%d: strcmp(str,static_string) != 0\n", __LINE__);
            goto bail;
        }

        if (memcmp(&eui64, &static_eui64, sizeof(spinel_eui64_t)) != 0)
        {
            printf("error:%d: memcmp(&eui64, &static_eui64, sizeof(spinel_eui64_t)) != 0\n", __LINE__);
            goto bail;
        }
    }

    {
        // Test UTF8 validation - Good/Valid strings

        // Single symbols
        const uint8_t single1[] = {0};                            // 0000
        const uint8_t single2[] = {0x7F, 0x00};                   // 007F
        const uint8_t single3[] = {0xC2, 0x80, 0x00};             // 080
        const uint8_t single4[] = {0xDF, 0xBF, 0x00};             // 07FF
        const uint8_t single5[] = {0xE0, 0xA0, 0x80, 0x00};       // 0800
        const uint8_t single6[] = {0xEF, 0xBF, 0xBF, 0x00};       // FFFF
        const uint8_t single7[] = {0xF0, 0x90, 0x80, 0x80, 0x00}; // 010000
        const uint8_t single8[] = {0xF4, 0x8F, 0xBF, 0xBF, 0x00}; // 10FFFF

        // Strings
        const uint8_t str1[] = "spinel";
        const uint8_t str2[] = "OpenThread";
        const uint8_t str3[] = {0x41, 0x7F, 0xEF, 0xBF, 0xBF, 0xC2, 0x80, 0x21, 0x33, 0x00};
        const uint8_t str4[] = {0xCE, 0xBA, 0xE1, 0xBD, 0xB9, 0xCF, 0x83, 0xCE, 0xBC, 0xCE, 0xB5, 0x00}; // κόσμε
        const uint8_t str5[] = {0x3D, 0xF4, 0x8F, 0xBF, 0xBF, 0x01, 0xE0, 0xA0, 0x83, 0x22, 0xEF, 0xBF, 0xBF, 0x00};
        const uint8_t str6[] = {0xE5, 0xA2, 0x82, 0xE0, 0xA0, 0x80, 0xC2, 0x83, 0xC2, 0x80, 0xF4,
                                0x8F, 0xBF, 0xBF, 0xF4, 0x8F, 0xBF, 0xBF, 0xDF, 0xBF, 0x21, 0x00};

        const uint8_t * good_strings[] = {single1, single2, single3, single4, single5, single6, single7, single8,
                                         str1,    str2,    str3,    str4,    str5,    str6,    NULL};
        const uint8_t **str_ptr;

        for (str_ptr = &good_strings[0]; *str_ptr != NULL; str_ptr++)
        {
            if (!spinel_validate_utf8(*str_ptr))
            {
                printf("error: spinel_validate_utf8() did not correctly detect a valid UTF8 sequence!\n");
                goto bail;
            }
        }
    }

    {
        // Test UTF8 validation - Bad/Invalid strings

        // Single symbols (invalid)
        const uint8_t single1[] = {0xF8, 0x00};
        const uint8_t single2[] = {0xF9, 0x00};
        const uint8_t single3[] = {0xFA, 0x00};
        const uint8_t single4[] = {0xFF, 0x00};

        // Bad continuations
        const uint8_t bad1[] = {0xDF, 0x0F, 0x00};
        const uint8_t bad2[] = {0xE0, 0xA0, 0x10, 0x00};
        const uint8_t bad3[] = {0xF0, 0x90, 0x80, 0x60, 0x00};
        const uint8_t bad4[] = {0xF4, 0x8F, 0xBF, 0x0F, 0x00};
        const uint8_t bad5[] = {0x21, 0xA0, 0x00};
        const uint8_t bad6[] = {0xCE, 0xBA, 0xE1, 0xBD, 0xB9, 0xCF, 0x83, 0xCE, 0xBC, 0xCE, 0x00};

        const uint8_t * bad_strings[] = {single1, single2, single3, single4, bad1, bad2, bad3, bad4, bad5, bad6, NULL};
        const uint8_t **str_ptr;

        for (str_ptr = &bad_strings[0]; *str_ptr != NULL; str_ptr++)
        {
            if (spinel_validate_utf8(*str_ptr))
            {
                printf("error: spinel_validate_utf8() did not correctly detect an invalid UTF8 sequence\n");
                goto bail;
            }
        }
    }

    printf("OK\n");
    ret = 0;
    return ret;

bail:
    printf("FAILURE\n");
    return ret;
}

#endif // #if SPINEL_SELF_TEST
