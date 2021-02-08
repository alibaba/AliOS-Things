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

#include <alibabacloud/iot/model/CreateOTAVerifyJobRequest.h>

using AlibabaCloud::Iot::Model::CreateOTAVerifyJobRequest;

CreateOTAVerifyJobRequest::CreateOTAVerifyJobRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateOTAVerifyJob")
{
	setMethod(HttpRequest::Method::Post);
}

CreateOTAVerifyJobRequest::~CreateOTAVerifyJobRequest()
{}

int CreateOTAVerifyJobRequest::getTimeoutInMinutes()const
{
	return timeoutInMinutes_;
}

void CreateOTAVerifyJobRequest::setTimeoutInMinutes(int timeoutInMinutes)
{
	timeoutInMinutes_ = timeoutInMinutes;
	setParameter("TimeoutInMinutes", std::to_string(timeoutInMinutes));
}

std::string CreateOTAVerifyJobRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateOTAVerifyJobRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateOTAVerifyJobRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateOTAVerifyJobRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CreateOTAVerifyJobRequest::getFirmwareId()const
{
	return firmwareId_;
}

void CreateOTAVerifyJobRequest::setFirmwareId(const std::string& firmwareId)
{
	firmwareId_ = firmwareId;
	setParameter("FirmwareId", firmwareId);
}

std::string CreateOTAVerifyJobRequest::getProductKey()const
{
	return productKey_;
}

void CreateOTAVerifyJobRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string CreateOTAVerifyJobRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateOTAVerifyJobRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateOTAVerifyJobRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateOTAVerifyJobRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::vector<std::string> CreateOTAVerifyJobRequest::getTargetDeviceName()const
{
	return targetDeviceName_;
}

void CreateOTAVerifyJobRequest::setTargetDeviceName(const std::vector<std::string>& targetDeviceName)
{
	targetDeviceName_ = targetDeviceName;
	for(int dep1 = 0; dep1!= targetDeviceName.size(); dep1++) {
		setParameter("TargetDeviceName."+ std::to_string(dep1), targetDeviceName.at(dep1));
	}
}

