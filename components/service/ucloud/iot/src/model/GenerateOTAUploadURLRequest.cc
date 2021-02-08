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

#include <alibabacloud/iot/model/GenerateOTAUploadURLRequest.h>

using AlibabaCloud::Iot::Model::GenerateOTAUploadURLRequest;

GenerateOTAUploadURLRequest::GenerateOTAUploadURLRequest() :
	RpcServiceRequest("iot", "2018-01-20", "GenerateOTAUploadURL")
{
	setMethod(HttpRequest::Method::Post);
}

GenerateOTAUploadURLRequest::~GenerateOTAUploadURLRequest()
{}

std::string GenerateOTAUploadURLRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void GenerateOTAUploadURLRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string GenerateOTAUploadURLRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void GenerateOTAUploadURLRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string GenerateOTAUploadURLRequest::getApiProduct()const
{
	return apiProduct_;
}

void GenerateOTAUploadURLRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string GenerateOTAUploadURLRequest::getApiRevision()const
{
	return apiRevision_;
}

void GenerateOTAUploadURLRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

