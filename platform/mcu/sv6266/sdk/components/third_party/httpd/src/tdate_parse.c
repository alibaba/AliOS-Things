/*
 * Copyright (c) Cameron Rich
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of the axTLS project nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//#include <sys/types.h>
//#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// HTTPD
#include "os_port.h"
#include "axhttp.h"

struct day_mon_map
{
    const char* s;
    uint8_t l;
};

const static struct day_mon_map wday_tab[] =
{
    /* sorted */
    { "Fri", 5 }, { "Mon", 1 }, { "Sat", 6 }, { "Sun", 0 },
    { "Thu", 4 }, { "Tue", 2 }, { "Wed", 3 },
};

const static struct day_mon_map mon_tab[] =
{
    /* sorted */
    { "Apr", 3 }, { "Aug", 7 }, { "Dec", 11 }, { "Feb", 1 },
    { "Jan", 0 }, { "Jul", 6 }, { "Jun", 5 }, { "Mar", 2 },
    { "May", 4 }, { "Nov", 10 }, { "Oct", 9 }, { "Sep", 8 },
};

static int day_mon_map_compare(const char *v1, const char *v2)
{
    return strcmp(((struct day_mon_map*)v1)->s, ((struct day_mon_map*)v2)->s);
}

void tdate_init(void)
{
    HTTPD_DEBUGF((httpd, "[HTTPD] tdate_init()\n"));
    /* pre sorted, no need to do sort.
    qsort(wday_tab, sizeof(wday_tab)/sizeof(struct day_mon_map),
            sizeof(struct day_mon_map),
            (int (*)(const void *, const void *))day_mon_map_compare);
    qsort(mon_tab, sizeof(mon_tab)/sizeof(struct day_mon_map),
            sizeof(struct day_mon_map),
            (int (*)(const void *, const void *))day_mon_map_compare);
    */
}

static int8_t day_mon_map_search(const char* str,
                            const struct day_mon_map* tab, int n)
{
    struct day_mon_map *search = bsearch(&str, tab, n,
            sizeof(struct day_mon_map),
                (int (*)(const void *, const void *))day_mon_map_compare);

    HTTPD_DEBUGF((httpd, "[HTTPD] day_mon_map_search()\n"));

    return search ? search->l : -1;
}

time_t tdate_parse(const char* str)
{
    struct tm tm;
    char str_mon[4], str_wday[4];
    int tm_sec, tm_min, tm_hour, tm_mday, tm_year;

    HTTPD_DEBUGF((httpd, "[HTTPD] tdate_parse()\n"));

    /* Initialize. */
    memset(&tm, 0, sizeof(struct tm));

    /* wdy, DD mth YY HH:MM:SS GMT */
    if (sscanf(str, "%3[a-zA-Z], %d %3[a-zA-Z] %d %d:%d:%d GMT",
                str_wday, &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min,
                    &tm_sec) == 7)
    {
        int8_t tm_wday = day_mon_map_search(str_wday, wday_tab,
                        sizeof(wday_tab)/sizeof(struct day_mon_map));
        int8_t tm_mon = day_mon_map_search(str_mon, mon_tab,
                        sizeof(mon_tab)/sizeof(struct day_mon_map));

        if (tm_wday < 0 || tm_mon < 0)
            // return -1;
            return 0; /* error */

        tm.tm_wday = tm_wday;
        tm.tm_mon = tm_mon;
        tm.tm_mday = tm_mday;
        tm.tm_hour = tm_hour;
        tm.tm_min = tm_min;
        tm.tm_sec = tm_sec;
        tm.tm_year = tm_year - 1900;
        return mktime(&tm);
    }

    // return -1;  /* error */    
    return 0; /* error */
}
