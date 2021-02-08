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

#include <alibabacloud/iot/model/QueryCertUrlByApplyIdRequest.h>

using AlibabaCloud::Iot::Model::QueryCertUrlByApplyIdRequest;

QueryCertUrlByApplyIdRequest::QueryCertUrlByApplyIdRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryCertUrlByApplyId")
{
	setMethod(HttpRequest::Method::Post);
}

QueryCertUrlByApplyIdRequest::~QueryCertUrlByApplyIdRequest()
{}

std::string QueryCertUrlByApplyIdRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryCertUrlByApplyIdRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryCertUrlByApplyIdRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryCertUrlByApplyIdRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

long QueryCertUrlByApplyIdRequest::getApplyId()const
{
	return applyId_;
}

void QueryCertUrlByApplyIdRequest::setApplyId(long applyId)
{
	applyId_ = applyId;
	setParameter("ApplyId", std::to_string(applyId));
}

std::string QueryCertUrlByApplyIdRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryCertUrlByApplyIdRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryCertUrlByApplyIdRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryCertUrlByApplyIdRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

