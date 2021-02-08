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

#include <alibabacloud/iot/model/SetProductCertInfoRequest.h>

using AlibabaCloud::Iot::Model::SetProductCertInfoRequest;

SetProductCertInfoRequest::SetProductCertInfoRequest() :
	RpcServiceRequest("iot", "2018-01-20", "SetProductCertInfo")
{
	setMethod(HttpRequest::Method::Post);
}

SetProductCertInfoRequest::~SetProductCertInfoRequest()
{}

std::string SetProductCertInfoRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void SetProductCertInfoRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string SetProductCertInfoRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void SetProductCertInfoRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string SetProductCertInfoRequest::getProductKey()const
{
	return productKey_;
}

void SetProductCertInfoRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string SetProductCertInfoRequest::getApiProduct()const
{
	return apiProduct_;
}

void SetProductCertInfoRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string SetProductCertInfoRequest::getApiRevision()const
{
	return apiRevision_;
}

void SetProductCertInfoRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

int SetProductCertInfoRequest::getIssueModel()const
{
	return issueModel_;
}

void SetProductCertInfoRequest::setIssueModel(int issueModel)
{
	issueModel_ = issueModel;
	setParameter("IssueModel", std::to_string(issueModel));
}

