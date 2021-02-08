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

#include <alibabacloud/iot/model/DeleteOTAFirmwareRequest.h>

using AlibabaCloud::Iot::Model::DeleteOTAFirmwareRequest;

DeleteOTAFirmwareRequest::DeleteOTAFirmwareRequest() :
	RpcServiceRequest("iot", "2018-01-20", "DeleteOTAFirmware")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteOTAFirmwareRequest::~DeleteOTAFirmwareRequest()
{}

std::string DeleteOTAFirmwareRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteOTAFirmwareRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteOTAFirmwareRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void DeleteOTAFirmwareRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string DeleteOTAFirmwareRequest::getFirmwareId()const
{
	return firmwareId_;
}

void DeleteOTAFirmwareRequest::setFirmwareId(const std::string& firmwareId)
{
	firmwareId_ = firmwareId;
	setParameter("FirmwareId", firmwareId);
}

std::string DeleteOTAFirmwareRequest::getApiProduct()const
{
	return apiProduct_;
}

void DeleteOTAFirmwareRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string DeleteOTAFirmwareRequest::getApiRevision()const
{
	return apiRevision_;
}

void DeleteOTAFirmwareRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

