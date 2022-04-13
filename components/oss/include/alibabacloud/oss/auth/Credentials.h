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
#include <alibabacloud/oss/Export.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT Credentials
    {
        public:
            Credentials(const std::string& accessKeyId, const std::string& accessKeySecret,
                const std::string& sessionToken="");
            ~Credentials();
            const std::string& AccessKeyId () const;
            const std::string& AccessKeySecret () const;
            const std::string& SessionToken () const;
            void setAccessKeyId(const std::string& accessKeyId);
            void setAccessKeySecret(const std::string& accessKeySecret);
            void setSessionToken (const std::string& sessionToken);
        private:
            std::string accessKeyId_;
            std::string accessKeySecret_;
            std::string sessionToken_;
    };
}
}
