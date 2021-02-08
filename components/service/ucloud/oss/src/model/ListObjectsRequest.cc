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


#include <alibabacloud/oss/model/ListObjectsRequest.h>
#include "../utils/Utils.h"

using namespace AlibabaCloud::OSS;

ParameterCollection ListObjectsRequest::specialParameters() const
{
    ParameterCollection params;
    if (delimiterIsSet_) params["delimiter"] = delimiter_;
    if (markerIsSet_) params["marker"] = marker_;
    if (maxKeysIsSet_) params["max-keys"] = std::to_string(maxKeys_);
    if (prefixIsSet_) params["prefix"] = prefix_;
    if (encodingTypeIsSet_) params["encoding-type"] = encodingType_;
    return params;
}

HeaderCollection ListObjectsRequest::specialHeaders() const
{
    HeaderCollection headers;
    if (requestPayer_ == RequestPayer::Requester) {
        headers["x-oss-request-payer"] = ToLower(ToRequestPayerName(RequestPayer::Requester));
    }
    return headers;
}
