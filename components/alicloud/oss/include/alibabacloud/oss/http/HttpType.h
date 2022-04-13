/*
 * Copyright 2009-2017 Alibaba Cloud All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <string>
#include <map>
#include <alibabacloud/oss/Export.h>

namespace AlibabaCloud
{
namespace OSS
{

    class  ALIBABACLOUD_OSS_EXPORT Http
    {
        public:
            enum Method
            {
                Get,
                Head,
                Post,
                Put,
                Delete,
                Connect,
                Options,
                Patch,
                Trace
            };
            enum Scheme
            {
                HTTP,
                HTTPS
            };
            static std::string MethodToString(Method method);
            static std::string SchemeToString(Scheme scheme);

            //HEADERS
            static const char* ACCEPT;
            static const char* ACCEPT_CHARSET;
            static const char* ACCEPT_ENCODING;
            static const char* ACCEPT_LANGUAGE;
            static const char* AUTHORIZATION;
            static const char* CACHE_CONTROL;
            static const char* CONTENT_DISPOSITION;
            static const char* CONTENT_ENCODING;
            static const char* CONTENT_LENGTH;
            static const char* CONTENT_MD5;
            static const char* CONTENT_RANGE;
            static const char* CONTENT_TYPE;
            static const char* DATE;
            static const char* EXPECT;
            static const char* EXPIRES;
            static const char* ETAG;
            static const char* LAST_MODIFIED;
            static const char* RANGE;
            static const char* USER_AGENT;

    };
}
}
