/*
 *  Copyright (c) 2020, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include "lib/url/url.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/common/code_utils.hpp"

namespace ot {
namespace Url {

otError Url::Init(char *aUrl)
{
    otError error = OT_ERROR_NONE;
    char *  url   = aUrl;

    mEnd      = aUrl + strlen(aUrl);
    mProtocol = aUrl;

    url = strstr(aUrl, "://");
    VerifyOrExit(url != nullptr, error = OT_ERROR_PARSE);
    *url = '\0';
    url += sizeof("://") - 1;
    mPath = url;

    url = strstr(url, "?");

    if (url != nullptr)
    {
        mQuery = ++url;

        for (char *cur = strtok(url, "&"); cur != nullptr; cur = strtok(nullptr, "&"))
        {
            cur[-1] = '\0';
        }
    }
    else
    {
        mQuery = mEnd;
    }

exit:
    return error;
}

const char *Url::GetValue(const char *aName, const char *aLastValue) const
{
    const char * rval = nullptr;
    const size_t len  = strlen(aName);
    const char * start;

    if (aLastValue == nullptr)
    {
        start = mQuery;
    }
    else
    {
        VerifyOrExit(aLastValue > mQuery && aLastValue < mEnd);
        start = aLastValue + strlen(aLastValue) + 1;
    }

    while (start < mEnd)
    {
        const char *last = nullptr;

        if (!strncmp(aName, start, len))
        {
            if (start[len] == '=')
            {
                ExitNow(rval = &start[len + 1]);
            }
            else if (start[len] == '\0')
            {
                ExitNow(rval = &start[len]);
            }
        }
        last  = start;
        start = last + strlen(last) + 1;
    }

exit:
    return rval;
}

} // namespace Url
} // namespace ot

#ifndef SELF_TEST
#define SELF_TEST 0
#endif

#if SELF_TEST
#include <assert.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"

void TestSimple(void)
{
    char         url[] = "spinel:///dev/ttyUSB0?baudrate=115200";
    ot::Url::Url args;

    assert(!args.Init(url));

    assert(!strcmp(args.GetPath(), "/dev/ttyUSB0"));
    assert(!strcmp(args.GetValue("baudrate"), "115200"));
    assert(args.GetValue("not-exists") == nullptr);
    assert(args.GetValue("last-value-wrong-position", url) == nullptr);
    assert(args.GetValue("last-value-before-url", url - 1) == nullptr);
    assert(args.GetValue("last-value-after-url", url + sizeof(url)) == nullptr);

    printf("PASS %s\r\n", __func__);
}

void TestSimpleNoQueryString(void)
{
    char         url[] = "spinel:///dev/ttyUSB0";
    ot::Url::Url args;

    assert(!args.Init(url));
    assert(!strcmp(args.GetPath(), "/dev/ttyUSB0"));
    assert(args.GetValue("last-value-wrong-position", url) == nullptr);
    assert(args.GetValue("last-value-before-url", url - 1) == nullptr);
    assert(args.GetValue("last-value-after-url", url + sizeof(url)) == nullptr);

    printf("PASS %s\r\n", __func__);
}

void TestEmptyValue(void)
{
    char         url[] = "spinel:///dev/ttyUSB0?rtscts&baudrate=115200&verbose&verbose&verbose";
    ot::Url::Url args;
    const char * arg = nullptr;

    assert(!args.Init(url));
    assert(!strcmp(args.GetPath(), "/dev/ttyUSB0"));
    assert((arg = args.GetValue("rtscts")) != nullptr);
    assert(args.GetValue("rtscts", arg) == nullptr);
    assert((arg = args.GetValue("verbose", arg)) != nullptr);
    assert((arg = args.GetValue("verbose", arg)) != nullptr);
    assert((arg = args.GetValue("verbose", arg)) != nullptr);
    assert((arg = args.GetValue("verbose", arg)) == nullptr);

    printf("PASS %s\r\n", __func__);
}

void TestMultipleProtocols(void)
{
    char         url[] = "spinel+spi:///dev/ttyUSB0?baudrate=115200";
    ot::Url::Url args;

    assert(!args.Init(url));
    assert(!strcmp(args.GetPath(), "/dev/ttyUSB0"));
    assert(!strcmp(args.GetValue("baudrate"), "115200"));

    printf("PASS %s\r\n", __func__);
}

void TestMultipleProtocolsAndDuplicateParameters(void)
{
    char         url[] = "spinel+exec:///path/to/ot-rcp?arg=1&arg=arg2&arg=3";
    ot::Url::Url args;
    const char * arg = nullptr;

    assert(!args.Init(url));
    assert(!strcmp(args.GetPath(), "/path/to/ot-rcp"));

    arg = args.GetValue("arg");
    assert(!strcmp(arg, "1"));

    arg = args.GetValue("arg", arg);
    assert(!strcmp(arg, "arg2"));

    arg = args.GetValue("arg", arg);
    assert(!strcmp(arg, "3"));

    assert(args.GetValue("arg", url) == nullptr);
    assert(args.GetValue("arg", url - 1) == nullptr);
    assert(args.GetValue("arg", url + sizeof(url)) == nullptr);

    printf("PASS %s\r\n", __func__);
}

#pragma GCC diagnostic pop

int main(void)
{
    TestSimple();
    TestSimpleNoQueryString();
    TestEmptyValue();
    TestMultipleProtocols();
    TestMultipleProtocolsAndDuplicateParameters();

    return 0;
}

#endif // SELF_TEST
