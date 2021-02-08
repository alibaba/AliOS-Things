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

#include <alibabacloud/oss/model/SetBucketPaymentRequest.h>
#include <sstream>
#include "../utils/Utils.h"


using namespace AlibabaCloud::OSS;

SetBucketRequestPaymentRequest::SetBucketRequestPaymentRequest(const std::string& bucket) :
    SetBucketRequestPaymentRequest(bucket, RequestPayer::NotSet)
{
}

SetBucketRequestPaymentRequest::SetBucketRequestPaymentRequest(const std::string& bucket,
    RequestPayer payer) :
    OssBucketRequest(bucket),
    payer_(payer)
{
    setFlags(Flags() | REQUEST_FLAG_CONTENTMD5);
}

std::string SetBucketRequestPaymentRequest::payload() const
{
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    ss << "<RequestPaymentConfiguration>" << std::endl;
    ss << "<Payer>" << ToRequestPayerName(payer_) << "</Payer>" << std::endl;
    ss << "</RequestPaymentConfiguration>" << std::endl;
    return ss.str();
}

ParameterCollection SetBucketRequestPaymentRequest::specialParameters() const
{
    ParameterCollection parameters;
    parameters["requestPayment"] = "";
    return parameters;
}

