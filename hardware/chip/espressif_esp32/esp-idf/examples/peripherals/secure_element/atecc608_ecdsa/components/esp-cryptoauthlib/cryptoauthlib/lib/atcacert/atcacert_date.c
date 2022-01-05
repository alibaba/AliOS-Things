/**
 * \file
 * \brief Date handling with regard to certificates.
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#include <string.h>
#include "atcacert_date.h"


const size_t ATCACERT_DATE_FORMAT_SIZES[ATCACERT_DATE_FORMAT_SIZES_COUNT] = {
    DATEFMT_ISO8601_SEP_SIZE,
    DATEFMT_RFC5280_UTC_SIZE,
    DATEFMT_POSIX_UINT32_BE_SIZE,
    DATEFMT_POSIX_UINT32_LE_SIZE,
    DATEFMT_RFC5280_GEN_SIZE
};

int atcacert_date_enc(atcacert_date_format_t   format,
                      const atcacert_tm_utc_t* timestamp,
                      uint8_t*                 formatted_date,
                      size_t*                  formatted_date_size)
{
    if (timestamp == NULL || formatted_date_size == NULL || format < 0 || format >= sizeof(ATCACERT_DATE_FORMAT_SIZES) / sizeof(ATCACERT_DATE_FORMAT_SIZES[0]))
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    if (formatted_date != NULL && *formatted_date_size < ATCACERT_DATE_FORMAT_SIZES[format])
    {
        *formatted_date_size = ATCACERT_DATE_FORMAT_SIZES[format];
        return ATCACERT_E_BUFFER_TOO_SMALL;
    }
    *formatted_date_size = ATCACERT_DATE_FORMAT_SIZES[format];
    if (formatted_date == NULL)
    {
        return ATCACERT_E_SUCCESS;  // Caller just wanted

    }
    switch (format)
    {
    case DATEFMT_ISO8601_SEP:     return atcacert_date_enc_iso8601_sep(timestamp, formatted_date);
    case DATEFMT_RFC5280_UTC:     return atcacert_date_enc_rfc5280_utc(timestamp, formatted_date);
    case DATEFMT_POSIX_UINT32_BE: return atcacert_date_enc_posix_uint32_be(timestamp, formatted_date);
    case DATEFMT_POSIX_UINT32_LE: return atcacert_date_enc_posix_uint32_le(timestamp, formatted_date);
    case DATEFMT_RFC5280_GEN:     return atcacert_date_enc_rfc5280_gen(timestamp, formatted_date);
    default: return ATCACERT_E_BAD_PARAMS;
    }

    return ATCACERT_E_BAD_PARAMS;
}

int atcacert_date_dec(atcacert_date_format_t format,
                      const uint8_t*         formatted_date,
                      size_t                 formatted_date_size,
                      atcacert_tm_utc_t*     timestamp)
{
    if (formatted_date == NULL || timestamp == NULL || format < 0 || format >= sizeof(ATCACERT_DATE_FORMAT_SIZES) / sizeof(ATCACERT_DATE_FORMAT_SIZES[0]))
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    if (formatted_date_size < ATCACERT_DATE_FORMAT_SIZES[format])
    {
        return ATCACERT_E_DECODING_ERROR;  // Not enough data to parse this date format

    }
    switch (format)
    {
    case DATEFMT_ISO8601_SEP:     return atcacert_date_dec_iso8601_sep(formatted_date, timestamp);
    case DATEFMT_RFC5280_UTC:     return atcacert_date_dec_rfc5280_utc(formatted_date, timestamp);
    case DATEFMT_POSIX_UINT32_BE: return atcacert_date_dec_posix_uint32_be(formatted_date, timestamp);
    case DATEFMT_POSIX_UINT32_LE: return atcacert_date_dec_posix_uint32_le(formatted_date, timestamp);
    case DATEFMT_RFC5280_GEN:     return atcacert_date_dec_rfc5280_gen(formatted_date, timestamp);
    default: return ATCACERT_E_BAD_PARAMS;
    }

    return ATCACERT_E_SUCCESS;
}

int atcacert_date_get_max_date(atcacert_date_format_t format, atcacert_tm_utc_t* timestamp)
{

    if (timestamp == NULL || format < 0 || format >= sizeof(ATCACERT_DATE_FORMAT_SIZES) / sizeof(ATCACERT_DATE_FORMAT_SIZES[0]))
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    switch (format)
    {
    case DATEFMT_ISO8601_SEP:
        timestamp->tm_year = 9999 - 1900;
        timestamp->tm_mon  = 12 - 1;
        timestamp->tm_mday = 31;
        timestamp->tm_hour = 23;
        timestamp->tm_min  = 59;
        timestamp->tm_sec  = 59;
        break;

    case DATEFMT_RFC5280_UTC:
        timestamp->tm_year = 2049 - 1900;
        timestamp->tm_mon  = 12 - 1;
        timestamp->tm_mday = 31;
        timestamp->tm_hour = 23;
        timestamp->tm_min  = 59;
        timestamp->tm_sec  = 59;
        break;

    case DATEFMT_POSIX_UINT32_BE:
        timestamp->tm_year = 2106 - 1900;
        timestamp->tm_mon  = 2 - 1;
        timestamp->tm_mday = 7;
        timestamp->tm_hour = 6;
        timestamp->tm_min  = 28;
        timestamp->tm_sec  = 15;
        break;

    case DATEFMT_POSIX_UINT32_LE:
        timestamp->tm_year = 2106 - 1900;
        timestamp->tm_mon  = 2 - 1;
        timestamp->tm_mday = 7;
        timestamp->tm_hour = 6;
        timestamp->tm_min  = 28;
        timestamp->tm_sec  = 15;
        break;

    case DATEFMT_RFC5280_GEN:
        timestamp->tm_year = 9999 - 1900;
        timestamp->tm_mon  = 12 - 1;
        timestamp->tm_mday = 31;
        timestamp->tm_hour = 23;
        timestamp->tm_min  = 59;
        timestamp->tm_sec  = 59;
        break;

    default: return ATCACERT_E_BAD_PARAMS;
    }

    return ATCACERT_E_SUCCESS;
}

/**
 * \brief Convert an unsigned integer to a zero padded string with no terminating null.
 */
static uint8_t* uint_to_str(uint32_t num, int width, uint8_t* str)
{
    uint8_t* ret = str + width;
    int i;

    // Pre-fill the string width with zeros
    for (i = 0; i < width; i++)
    {
        *(str++) = '0';
    }
    // Convert the number from right to left
    for (; num; num /= 10)
    {
        *(--str) = '0' + (num % 10);
    }

    return ret;
}

/**
 * \brief Convert a number string as a zero padded unsigned integer back into a number
 */
static const uint8_t* str_to_uint(const uint8_t* str, int width, uint32_t* num)
{
    const uint8_t* error_ret = str;
    const uint8_t* good_ret = str + width;
    uint32_t prev_num = 0;
    uint32_t digit_value = 1;
    int digit;

    str += width - 1;
    *num = 0;
    for (digit = 0; digit < width; digit++)
    {
        if (*str < '0' || *str > '9')
        {
            return error_ret;  // Character is not a digit
        }
        if (digit >= 10)
        {
            if (*str != '0')
            {
                return error_ret;  // Number is larger than the output can handle
            }
            continue;
        }
        if (digit == 9 && *str > '4')
        {
            return error_ret;  // Number is larger than the output can handle

        }
        *num += digit_value * (*str - '0');
        if (*num < prev_num)
        {
            return error_ret;  // Number rolled over, it is larger than the output can handle

        }
        digit_value *= 10;
        prev_num = *num;
        str--;
    }

    return good_ret;
}

/**
 * \brief Convert a number string as a zero padded unsigned integer back into a number constrained
 *        to an integer's size.
 */
static const uint8_t* str_to_int(const uint8_t* str, int width, int* num)
{
    uint32_t unum = 0;
    const uint8_t* ret = str_to_uint(str, width, &unum);

    if (ret != str && unum > 2147483647UL)
    {
        ret = str;  // Number exceeds int32's range
    }
    *num = (int)unum;
    return ret;
}

int atcacert_date_enc_iso8601_sep(const atcacert_tm_utc_t* timestamp,
                                  uint8_t                  formatted_date[DATEFMT_ISO8601_SEP_SIZE])
{
    uint8_t* cur_pos = formatted_date;
    int year = 0;

    if (timestamp == NULL || formatted_date == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    year = timestamp->tm_year + 1900;

    if (year < 0 || year > 9999)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    cur_pos = uint_to_str(year, 4, cur_pos);

    *(cur_pos++) = '-';

    if (timestamp->tm_mon < 0 || timestamp->tm_mon > 11)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    cur_pos = uint_to_str(timestamp->tm_mon + 1, 2, cur_pos);

    *(cur_pos++) = '-';

    if (timestamp->tm_mday < 1 || timestamp->tm_mday > 31)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    cur_pos = uint_to_str(timestamp->tm_mday, 2, cur_pos);

    *(cur_pos++) = 'T';

    if (timestamp->tm_hour < 0 || timestamp->tm_hour > 23)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    cur_pos = uint_to_str(timestamp->tm_hour, 2, cur_pos);

    *(cur_pos++) = ':';

    if (timestamp->tm_min < 0 || timestamp->tm_min > 59)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    cur_pos = uint_to_str(timestamp->tm_min, 2, cur_pos);

    *(cur_pos++) = ':';

    if (timestamp->tm_sec < 0 || timestamp->tm_sec > 59)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    cur_pos = uint_to_str(timestamp->tm_sec, 2, cur_pos);

    *(cur_pos++) = 'Z';

    return ATCACERT_E_SUCCESS;
}

int atcacert_date_dec_iso8601_sep(const uint8_t      formatted_date[DATEFMT_ISO8601_SEP_SIZE],
                                  atcacert_tm_utc_t* timestamp)
{
    const uint8_t* cur_pos = formatted_date;
    const uint8_t* new_pos = NULL;

    if (formatted_date == NULL || timestamp == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    memset(timestamp, 0, sizeof(*timestamp));

    new_pos = str_to_int(cur_pos, 4, &timestamp->tm_year);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;
    timestamp->tm_year -= 1900;

    if (*(cur_pos++) != '-')
    {
        return ATCACERT_E_DECODING_ERROR;  // Unexpected separator

    }
    new_pos = str_to_int(cur_pos, 2, &timestamp->tm_mon);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;
    timestamp->tm_mon -= 1;

    if (*(cur_pos++) != '-')
    {
        return ATCACERT_E_DECODING_ERROR;  // Unexpected separator

    }
    new_pos = str_to_int(cur_pos, 2, &timestamp->tm_mday);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;

    if (*(cur_pos++) != 'T')
    {
        return ATCACERT_E_DECODING_ERROR;  // Unexpected separator

    }
    new_pos = str_to_int(cur_pos, 2, &timestamp->tm_hour);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;

    if (*(cur_pos++) != ':')
    {
        return ATCACERT_E_DECODING_ERROR;  // Unexpected separator

    }
    new_pos = str_to_int(cur_pos, 2, &timestamp->tm_min);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;

    if (*(cur_pos++) != ':')
    {
        return ATCACERT_E_DECODING_ERROR;  // Unexpected separator

    }
    new_pos = str_to_int(cur_pos, 2, &timestamp->tm_sec);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;

    if (*(cur_pos++) != 'Z')
    {
        return ATCACERT_E_DECODING_ERROR;  // Unexpected UTC marker

    }
    return ATCACERT_E_SUCCESS;
}

int atcacert_date_enc_rfc5280_utc(const atcacert_tm_utc_t* timestamp,
                                  uint8_t                  formatted_date[DATEFMT_RFC5280_UTC_SIZE])
{
    uint8_t* cur_pos = formatted_date;
    int year = 0;

    if (timestamp == NULL || formatted_date == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    year = timestamp->tm_year + 1900;

    if (year >= 1950 && year <= 1999)
    {
        year = year - 1900;
    }
    else if (year >= 2000 && year <= 2049)
    {
        year = year - 2000;
    }
    else
    {
        return ATCACERT_E_INVALID_DATE;  // Year out of range for RFC2459 UTC format
    }
    cur_pos = uint_to_str(year, 2, cur_pos);

    if (timestamp->tm_mon < 0 || timestamp->tm_mon > 11)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    cur_pos = uint_to_str(timestamp->tm_mon + 1, 2, cur_pos);

    if (timestamp->tm_mday < 1 || timestamp->tm_mday > 31)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    cur_pos = uint_to_str(timestamp->tm_mday, 2, cur_pos);

    if (timestamp->tm_hour < 0 || timestamp->tm_hour > 23)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    cur_pos = uint_to_str(timestamp->tm_hour, 2, cur_pos);

    if (timestamp->tm_min < 0 || timestamp->tm_min > 59)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    cur_pos = uint_to_str(timestamp->tm_min, 2, cur_pos);

    if (timestamp->tm_sec < 0 || timestamp->tm_sec > 59)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    cur_pos = uint_to_str(timestamp->tm_sec, 2, cur_pos);

    *(cur_pos++) = 'Z';

    return ATCACERT_E_SUCCESS;
}

int atcacert_date_dec_rfc5280_utc(const uint8_t      formatted_date[DATEFMT_RFC5280_UTC_SIZE],
                                  atcacert_tm_utc_t* timestamp)
{
    const uint8_t* cur_pos = formatted_date;
    const uint8_t* new_pos = NULL;

    if (formatted_date == NULL || timestamp == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    memset(timestamp, 0, sizeof(*timestamp));

    new_pos = str_to_int(cur_pos, 2, &timestamp->tm_year);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;
    if (timestamp->tm_year < 50)
    {
        timestamp->tm_year += 2000;
    }
    else
    {
        timestamp->tm_year += 1900;
    }
    timestamp->tm_year -= 1900;

    new_pos = str_to_int(cur_pos, 2, &timestamp->tm_mon);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;
    timestamp->tm_mon -= 1;

    new_pos = str_to_int(cur_pos, 2, &timestamp->tm_mday);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;

    new_pos = str_to_int(cur_pos, 2, &timestamp->tm_hour);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;

    new_pos = str_to_int(cur_pos, 2, &timestamp->tm_min);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;

    new_pos = str_to_int(cur_pos, 2, &timestamp->tm_sec);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;

    if (*(cur_pos++) != 'Z')
    {
        return ATCACERT_E_DECODING_ERROR;  // Unexpected UTC marker

    }
    return ATCACERT_E_SUCCESS;
}

int atcacert_date_enc_rfc5280_gen(const atcacert_tm_utc_t* timestamp,
                                  uint8_t                  formatted_date[DATEFMT_RFC5280_GEN_SIZE])
{
    uint8_t* cur_pos = formatted_date;
    int year = 0;

    if (timestamp == NULL || formatted_date == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    year = timestamp->tm_year + 1900;

    if (year < 0 || year > 9999)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    cur_pos = uint_to_str(year, 4, cur_pos);

    if (timestamp->tm_mon < 0 || timestamp->tm_mon > 11)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    cur_pos = uint_to_str(timestamp->tm_mon + 1, 2, cur_pos);

    if (timestamp->tm_mday < 1 || timestamp->tm_mday > 31)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    cur_pos = uint_to_str(timestamp->tm_mday, 2, cur_pos);

    if (timestamp->tm_hour < 0 || timestamp->tm_hour > 23)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    cur_pos = uint_to_str(timestamp->tm_hour, 2, cur_pos);

    if (timestamp->tm_min < 0 || timestamp->tm_min > 59)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    cur_pos = uint_to_str(timestamp->tm_min, 2, cur_pos);

    if (timestamp->tm_sec < 0 || timestamp->tm_sec > 59)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    cur_pos = uint_to_str(timestamp->tm_sec, 2, cur_pos);

    *(cur_pos++) = 'Z';

    return ATCACERT_E_SUCCESS;
}

int atcacert_date_dec_rfc5280_gen(const uint8_t      formatted_date[DATEFMT_RFC5280_GEN_SIZE],
                                  atcacert_tm_utc_t* timestamp)
{
    const uint8_t* cur_pos = formatted_date;
    const uint8_t* new_pos = NULL;

    if (formatted_date == NULL || timestamp == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    memset(timestamp, 0, sizeof(*timestamp));

    new_pos = str_to_int(cur_pos, 4, &timestamp->tm_year);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;
    timestamp->tm_year -= 1900;

    new_pos = str_to_int(cur_pos, 2, &timestamp->tm_mon);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;
    timestamp->tm_mon -= 1;

    new_pos = str_to_int(cur_pos, 2, &timestamp->tm_mday);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;

    new_pos = str_to_int(cur_pos, 2, &timestamp->tm_hour);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;

    new_pos = str_to_int(cur_pos, 2, &timestamp->tm_min);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;

    new_pos = str_to_int(cur_pos, 2, &timestamp->tm_sec);
    if (new_pos == cur_pos)
    {
        return ATCACERT_E_DECODING_ERROR;  // There was a problem converting the string to a number
    }
    cur_pos = new_pos;

    if (*(cur_pos++) != 'Z')
    {
        return ATCACERT_E_DECODING_ERROR;  // Unexpected UTC marker

    }
    return ATCACERT_E_SUCCESS;
}

static int is_leap_year(int year)
{
    return (year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0));
}

static uint32_t get_year_secs(int year)
{
    if (is_leap_year(year))
    {
        return (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31) * 86400;
    }
    else
    {
        return (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31) * 86400;
    }
}

static uint32_t get_month_secs(int year, int mon)
{
    static const uint32_t month_secs[] = { 2678400, 2419200, 2678400, 2592000, 2678400, 2592000, 2678400, 2678400, 2592000, 2678400, 2592000, 2678400 };

    if (mon == 1 && is_leap_year(year))
    {
        return 2505600;
    }
    else if (mon < 12)
    {
        return month_secs[mon];
    }
    else
    {
        return 0;
    }
}

static atcacert_tm_utc_t *atcacert_gmtime32(const uint32_t *posix_time, atcacert_tm_utc_t *result)
{
    uint32_t secs_remaining = *posix_time;
    uint32_t secs = 0;

    result->tm_year = 1970;
    result->tm_mon = 0;
    result->tm_mday = 1;
    result->tm_hour = 0;
    result->tm_min = 0;
    result->tm_sec = 0;

    secs = get_year_secs(result->tm_year);
    while (secs_remaining >= secs)
    {
        result->tm_year++;
        secs_remaining -= secs;
        secs = get_year_secs(result->tm_year);
    }

    secs = get_month_secs(result->tm_year, result->tm_mon);
    while (secs_remaining >= secs)
    {
        result->tm_mon++;
        secs_remaining -= secs;
        secs = get_month_secs(result->tm_year, result->tm_mon);
    }

    result->tm_year -= 1900;

    result->tm_mday += secs_remaining / 86400;
    secs_remaining %= 86400;

    result->tm_hour += secs_remaining / 3600;
    secs_remaining %= 3600;

    result->tm_min += secs_remaining / 60;
    secs_remaining %= 60;

    result->tm_sec += secs_remaining;

    return result;
}

static uint32_t atcacert_mkgmtime32(const atcacert_tm_utc_t *timeptr)
{
    uint32_t posix_time = 0;
    int cur_value = 0;
    int year = timeptr->tm_year + 1900;

    cur_value = year - 1;
    while (cur_value >= 1970)
    {
        posix_time += get_year_secs(cur_value--);
    }

    cur_value = timeptr->tm_mon - 1;
    while (cur_value >= 0)
    {
        posix_time += get_month_secs(year, cur_value--);
    }

    posix_time += (uint32_t)(timeptr->tm_mday - 1) * 86400;
    posix_time += (uint32_t)timeptr->tm_hour * 3600;
    posix_time += (uint32_t)timeptr->tm_min * 60;
    posix_time += (uint32_t)timeptr->tm_sec;

    return posix_time;
}

static int atcacert_date_enc_posix_uint32(const atcacert_tm_utc_t* timestamp, uint32_t* posix_uint32)
{
    //atcacert_tm_utc_t timestamp_nc;
    //time_t posix_time = 0;
    int year = 0;

    if (timestamp == NULL || posix_uint32 == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    year = timestamp->tm_year + 1900;

    if (year > 2106 || year < 1970)
    {
        return ATCACERT_E_INVALID_DATE;  //Timestamp out of range for POSIX time.
    }
    if (timestamp->tm_mon < 0 || timestamp->tm_mon > 11)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    if (timestamp->tm_mday < 1 || timestamp->tm_mday > 31)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    if (timestamp->tm_hour < 0 || timestamp->tm_hour > 23)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    if (timestamp->tm_min < 0 || timestamp->tm_min > 59)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    if (timestamp->tm_sec < 0 || timestamp->tm_sec > 59)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    // Check for date past max date for POSIX time
    if (year == 2106)
    {
        if (timestamp->tm_mon > 1)
        {
            return ATCACERT_E_INVALID_DATE;
        }
        if (timestamp->tm_mon == 1)
        {
            if (timestamp->tm_mday > 7)
            {
                return ATCACERT_E_INVALID_DATE;
            }
            if (timestamp->tm_mday == 7)
            {
                if (timestamp->tm_hour > 6)
                {
                    return ATCACERT_E_INVALID_DATE;
                }
                if (timestamp->tm_hour == 6)
                {
                    if (timestamp->tm_min > 28)
                    {
                        return ATCACERT_E_INVALID_DATE;
                    }
                    if (timestamp->tm_min == 28)
                    {
                        if (timestamp->tm_sec > 14)
                        {
                            return ATCACERT_E_INVALID_DATE;
                        }
                    }
                }
            }
        }
    }

//#ifdef WIN32
//	timestamp_nc = *timestamp;
//	posix_time = _mkgmtime(&timestamp_nc);
//	if (posix_time == -1)
//		return ATCACERT_E_INVALID_DATE;
//#elif defined _BSD_SOURCE || defined _SVID_SOURCE
//	timestamp_nc = *timestamp;
//	posix_time = timegm(&timestamp_nc);
//	if (posix_time == -1)
//		return ATCACERT_E_INVALID_DATE;
//#else
//	// In order for this to work, we need to make sure mktime is using GMT. Since this can
//	// vary from system to system, I'm putting this "assert" in every time since it may not
//	// be caught early on.
//	memset(&timestamp_nc, 0, sizeof(timestamp_nc));
//	timestamp_nc.tm_year = 2013 - 1900;
//	timestamp_nc.tm_mon = 11 - 1;
//	timestamp_nc.tm_mday = 10;
//	timestamp_nc.tm_hour = 9;
//	timestamp_nc.tm_min = 8;
//	timestamp_nc.tm_sec = 7;
//	posix_time = mktime(&timestamp_nc);
//	if (posix_time != 1384074487)
//		return ATCACERT_E_UNIMPLEMENTED; // mktime isn't using GMT as the timezone, this needs to be fixed
//
//	timestamp_nc = *timestamp;
//	posix_time = mktime(&timestamp_nc);
//	if (posix_time == -1)
//		return ATCACERT_E_INVALID_DATE;
//#endif
//
//	*posix_uint32 = (uint32_t)posix_time;

    *posix_uint32 = atcacert_mkgmtime32(timestamp);

    return ATCACERT_E_SUCCESS;
}

int atcacert_date_enc_posix_uint32_be(const atcacert_tm_utc_t* timestamp,
                                      uint8_t                  formatted_date[DATEFMT_POSIX_UINT32_BE_SIZE])
{
    uint32_t posix_uint32 = 0;
    int ret = 0;

    if (timestamp == NULL || formatted_date == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    ret = atcacert_date_enc_posix_uint32(timestamp, &posix_uint32);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }

    formatted_date[0] = (uint8_t)((posix_uint32 >> 24) & 0xFF);
    formatted_date[1] = (uint8_t)((posix_uint32 >> 16) & 0xFF);
    formatted_date[2] = (uint8_t)((posix_uint32 >> 8) & 0xFF);
    formatted_date[3] = (uint8_t)((posix_uint32 >> 0) & 0xFF);

    return ATCACERT_E_SUCCESS;
}

static int atcacert_date_dec_posix_uint32(uint32_t           posix_uint32,
                                          atcacert_tm_utc_t* timestamp)
{
//#ifdef WIN32
//	time_t posix_time = (time_t)posix_uint32;
//	errno_t ret = 0;
//
//	if (timestamp == NULL)
//		return ATCACERT_E_BAD_PARAMS;
//
//	memset(timestamp, 0, sizeof(*timestamp));
//	ret = gmtime_s(timestamp, &posix_time);
//	if (ret != 0)
//		return ATCACERT_E_DECODING_ERROR; // Failed to convert to timestamp structure
//#else
//	time_t posix_time = (time_t)posix_uint32;
//	atcacert_tm_utc_t* ret = NULL;
//	if (timestamp == NULL)
//		return ATCACERT_E_BAD_PARAMS;
//
//	memset(timestamp, 0, sizeof(*timestamp));
//	ret = gmtime_r(&posix_time, timestamp);
//	if (ret == NULL)
//		return ATCACERT_E_DECODING_ERROR; // Failed to convert to timestamp structure
//#endif
    atcacert_gmtime32(&posix_uint32, timestamp);

    return ATCACERT_E_SUCCESS;
}

int atcacert_date_dec_posix_uint32_be(const uint8_t      formatted_date[DATEFMT_POSIX_UINT32_BE_SIZE],
                                      atcacert_tm_utc_t* timestamp)
{
    uint32_t posix_uint32 = 0;

    if (formatted_date == NULL || timestamp == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    posix_uint32 =
        ((uint32_t)formatted_date[0] << 24) |
        ((uint32_t)formatted_date[1] << 16) |
        ((uint32_t)formatted_date[2] << 8) |
        ((uint32_t)formatted_date[3]);

    return atcacert_date_dec_posix_uint32(posix_uint32, timestamp);
}

int atcacert_date_enc_posix_uint32_le(const atcacert_tm_utc_t* timestamp,
                                      uint8_t                  formatted_date[DATEFMT_POSIX_UINT32_LE_SIZE])
{
    uint32_t posix_uint32 = 0;
    int ret = 0;

    if (timestamp == NULL || formatted_date == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    ret = atcacert_date_enc_posix_uint32(timestamp, &posix_uint32);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }

    formatted_date[0] = (uint8_t)((posix_uint32 >> 0) & 0xFF);
    formatted_date[1] = (uint8_t)((posix_uint32 >> 8) & 0xFF);
    formatted_date[2] = (uint8_t)((posix_uint32 >> 16) & 0xFF);
    formatted_date[3] = (uint8_t)((posix_uint32 >> 24) & 0xFF);

    return ATCACERT_E_SUCCESS;
}

int atcacert_date_dec_posix_uint32_le(const uint8_t      formatted_date[DATEFMT_POSIX_UINT32_LE_SIZE],
                                      atcacert_tm_utc_t* timestamp)
{
    uint32_t posix_uint32 = 0;

    if (formatted_date == NULL || timestamp == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    posix_uint32 =
        ((uint32_t)formatted_date[3] << 24) |
        ((uint32_t)formatted_date[2] << 16) |
        ((uint32_t)formatted_date[1] << 8) |
        ((uint32_t)formatted_date[0]);

    return atcacert_date_dec_posix_uint32(posix_uint32, timestamp);
}

int atcacert_date_enc_compcert(const atcacert_tm_utc_t* issue_date,
                               uint8_t                  expire_years,
                               uint8_t                  enc_dates[3])
{
    /*
     * Issue and expire dates are compressed/encoded as below
     * +---------------+---------------+---------------+
     * | Byte 1        | Byte 2        | Byte 3        |
     * +---------------+---------------+---------------+
     * | | | | | | | | | | | | | | | | | | | | | | | | |
     * | 5 bits  | 4 bits| 5 bits  | 5 bits  | 5 bits  |
     * | Year    | Month | Day     | Hour    | Expire  |
     * |         |       |         |         | Years   |
     * +---------+-------+---------+---------+---------+
     *
     * Minutes and seconds are always zero.
     */
    if (issue_date == NULL || enc_dates == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    if ((issue_date->tm_year + 1900) < 2000 || (issue_date->tm_year + 1900) > 2031)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    if (issue_date->tm_mon < 0 || issue_date->tm_mon > 11)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    if (issue_date->tm_mday < 1 || issue_date->tm_mday > 31)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    if (issue_date->tm_hour < 0 || issue_date->tm_hour > 23)
    {
        return ATCACERT_E_INVALID_DATE;
    }
    if (expire_years > 31)
    {
        return ATCACERT_E_INVALID_DATE;
    }

    memset(enc_dates, 0, 3);

    enc_dates[0] = (enc_dates[0] & 0x07) | (((issue_date->tm_year + 1900 - 2000) & 0x1F) << 3);
    enc_dates[0] = (enc_dates[0] & 0xF8) | (((issue_date->tm_mon + 1) & 0x0F) >> 1);
    enc_dates[1] = (enc_dates[1] & 0x7F) | (((issue_date->tm_mon + 1) & 0x0F) << 7);
    enc_dates[1] = (enc_dates[1] & 0x83) | ((issue_date->tm_mday & 0x1F) << 2);
    enc_dates[1] = (enc_dates[1] & 0xFC) | ((issue_date->tm_hour & 0x1F) >> 3);
    enc_dates[2] = (enc_dates[2] & 0x1F) | ((issue_date->tm_hour & 0x1F) << 5);
    enc_dates[2] = (enc_dates[2] & 0xE0) | (expire_years & 0x1F);

    return ATCACERT_E_SUCCESS;
}

int atcacert_date_dec_compcert(const uint8_t          enc_dates[3],
                               atcacert_date_format_t expire_date_format,
                               atcacert_tm_utc_t*     issue_date,
                               atcacert_tm_utc_t*     expire_date)
{
    int ret = ATCACERT_E_SUCCESS;
    uint8_t expire_years = 0;

    /*
     * Issue and expire dates are compressed/encoded as below
     * +---------------+---------------+---------------+
     * | Byte 1        | Byte 2        | Byte 3        |
     * +---------------+---------------+---------------+
     * | | | | | | | | | | | | | | | | | | | | | | | | |
     * | 5 bits  | 4 bits| 5 bits  | 5 bits  | 5 bits  |
     * | Year    | Month | Day     | Hour    | Expire  |
     * |         |       |         |         | Years   |
     * +---------+-------+---------+---------+---------+
     *
     * Minutes and seconds are always zero.
     */

    if (enc_dates == NULL || issue_date == NULL || expire_date == NULL || expire_date_format < 0 || expire_date_format >= sizeof(ATCACERT_DATE_FORMAT_SIZES) / sizeof(ATCACERT_DATE_FORMAT_SIZES[0]))
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    memset(issue_date, 0, sizeof(*issue_date));
    memset(expire_date, 0, sizeof(*expire_date));

    issue_date->tm_year = (enc_dates[0] >> 3) + 2000 - 1900;
    issue_date->tm_mon  = (((enc_dates[0] & 0x07) << 1) | ((enc_dates[1] & 0x80) >> 7)) - 1;
    issue_date->tm_mday = ((enc_dates[1] & 0x7C) >> 2);
    issue_date->tm_hour = ((enc_dates[1] & 0x03) << 3) | ((enc_dates[2] & 0xE0) >> 5);

    expire_years = (enc_dates[2] & 0x1F);

    if (expire_years != 0)
    {
        expire_date->tm_year = issue_date->tm_year + expire_years;
        expire_date->tm_mon  = issue_date->tm_mon;
        expire_date->tm_mday = issue_date->tm_mday;
        expire_date->tm_hour = issue_date->tm_hour;
    }
    else
    {
        // Expire years of 0, means no expiration. Set to max date for the given expiration date format.
        ret = atcacert_date_get_max_date(expire_date_format, expire_date);
        if (ret != ATCACERT_E_SUCCESS)
        {
            return ret;
        }
    }

    return ATCACERT_E_SUCCESS;
}