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

#include <alibabacloud/oss/model/SetBucketQosInfoRequest.h>
using namespace AlibabaCloud::OSS;

SetBucketQosInfoRequest::SetBucketQosInfoRequest(const std::string& bucket) :
    SetBucketQosInfoRequest(bucket, QosConfiguration())
{
}

SetBucketQosInfoRequest::SetBucketQosInfoRequest(const std::string& bucket, const QosConfiguration& qos) :
    OssBucketRequest(bucket),
    qosInfo_(qos)
{
    setFlags(Flags() | REQUEST_FLAG_CONTENTMD5);
}

std::string SetBucketQosInfoRequest::payload() const
{
    std::string str;
    str.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    str.append("<QoSConfiguration>\n");
    str.append("<TotalUploadBandwidth>");
    str.append(std::to_string(qosInfo_.TotalUploadBandwidth()));
    str.append("</TotalUploadBandwidth>\n");
    str.append("<IntranetUploadBandwidth>");
    str.append(std::to_string(qosInfo_.IntranetUploadBandwidth()));
    str.append("</IntranetUploadBandwidth>\n");
    str.append("<ExtranetUploadBandwidth>");
    str.append(std::to_string(qosInfo_.ExtranetUploadBandwidth()));
    str.append("</ExtranetUploadBandwidth>\n");
    str.append("<TotalDownloadBandwidth>");
    str.append(std::to_string(qosInfo_.TotalDownloadBandwidth()));
    str.append("</TotalDownloadBandwidth>\n");
    str.append("<IntranetDownloadBandwidth>");
    str.append(std::to_string(qosInfo_.IntranetDownloadBandwidth()));
    str.append("</IntranetDownloadBandwidth>\n");
    str.append("<ExtranetDownloadBandwidth>");
    str.append(std::to_string(qosInfo_.ExtranetDownloadBandwidth()));
    str.append("</ExtranetDownloadBandwidth>\n");
    str.append("<TotalQps>");
    str.append(std::to_string(qosInfo_.TotalQps()));
    str.append("</TotalQps>\n");
    str.append("<IntranetQps>");
    str.append(std::to_string(qosInfo_.IntranetQps()));
    str.append("</IntranetQps>\n");
    str.append("<ExtranetQps>");
    str.append(std::to_string(qosInfo_.ExtranetQps()));
    str.append("</ExtranetQps>\n");
    str.append("</QoSConfiguration>");
    return str;
}

ParameterCollection SetBucketQosInfoRequest::specialParameters() const
{
    ParameterCollection parameters;
    parameters["qosInfo"] = "";
    return parameters;
}

