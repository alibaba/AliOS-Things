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
#include <alibabacloud/oss/OssRequest.h>
#include <alibabacloud/oss/model/InputFormat.h>


namespace AlibabaCloud
{
namespace OSS
{

    class ALIBABACLOUD_OSS_EXPORT CreateSelectObjectMetaRequest : public OssObjectRequest
    {
    public:
        CreateSelectObjectMetaRequest(const std::string& bucket, const std::string& key);

        void setOverWriteIfExists(bool overWriteIfExist);
        void setInputFormat(InputFormat& inputFormat);
   
    protected:
        virtual int validate() const;
        virtual std::string payload() const;
        virtual ParameterCollection specialParameters() const;

    private:
        InputFormat *inputFormat_;
        bool overWriteIfExists_;
    };
}
}