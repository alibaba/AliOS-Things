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

#include <alibabacloud/iot/model/QueryOTAFirmwareRequest.h>

using AlibabaCloud::Iot::Model::QueryOTAFirmwareRequest;

QueryOTAFirmwareRequest::QueryOTAFirmwareRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryOTAFirmware")
{
	setMethod(HttpRequest::Method::Post);
}

QueryOTAFirmwareRequest::~QueryOTAFirmwareRequest()
{}

std::string QueryOTAFirmwareRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryOTAFirmwareRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryOTAFirmwareRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryOTAFirmwareRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string QueryOTAFirmwareRequest::getFirmwareId()const
{
	return firmwareId_;
}

void QueryOTAFirmwareRequest::setFirmwareId(const std::string& firmwareId)
{
	firmwareId_ = firmwareId;
	setParameter("FirmwareId", firmwareId);
}

std::string QueryOTAFirmwareRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryOTAFirmwareRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryOTAFirmwareRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryOTAFirmwareRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

