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

#include <alibabacloud/oss/model/ListBucketsRequest.h>

using namespace AlibabaCloud::OSS;

ListBucketsRequest::ListBucketsRequest() :
    OssRequest(),
    prefixIsSet_(false),
    markerIsSet_(false),
    maxKeysIsSet_(false),
    tagIsSet(false)
{
}
ListBucketsRequest::ListBucketsRequest(const std::string& prefix, const std::string& marker, int maxKeys) :
    OssRequest(),
    prefix_(prefix), prefixIsSet_(true),
    marker_(marker), markerIsSet_(true),
    maxKeys_(maxKeys), maxKeysIsSet_(true),
    tagIsSet(false)
{
}

ParameterCollection ListBucketsRequest::specialParameters() const
{
    ParameterCollection params;
    if (prefixIsSet_) params["prefix"] = prefix_;
    if (markerIsSet_) params["marker"] = marker_;
    if (maxKeysIsSet_) params["max-keys"] = std::to_string(maxKeys_);
    if (tagIsSet) {
        if (!tag_.Key().empty()) {
            params["tag-key"] = tag_.Key();

            if (!tag_.Value().empty()) {
                params["tag-value"] = tag_.Value();
            }
        }
    }
    return params;
}