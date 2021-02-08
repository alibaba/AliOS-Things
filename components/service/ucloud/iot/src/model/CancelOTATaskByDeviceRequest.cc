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

#include <alibabacloud/iot/model/CancelOTATaskByDeviceRequest.h>

using AlibabaCloud::Iot::Model::CancelOTATaskByDeviceRequest;

CancelOTATaskByDeviceRequest::CancelOTATaskByDeviceRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CancelOTATaskByDevice")
{
	setMethod(HttpRequest::Method::Post);
}

CancelOTATaskByDeviceRequest::~CancelOTATaskByDeviceRequest()
{}

std::string CancelOTATaskByDeviceRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CancelOTATaskByDeviceRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CancelOTATaskByDeviceRequest::getJobId()const
{
	return jobId_;
}

void CancelOTATaskByDeviceRequest::setJobId(const std::string& jobId)
{
	jobId_ = jobId;
	setParameter("JobId", jobId);
}

std::string CancelOTATaskByDeviceRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CancelOTATaskByDeviceRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CancelOTATaskByDeviceRequest::getFirmwareId()const
{
	return firmwareId_;
}

void CancelOTATaskByDeviceRequest::setFirmwareId(const std::string& firmwareId)
{
	firmwareId_ = firmwareId;
	setParameter("FirmwareId", firmwareId);
}

std::string CancelOTATaskByDeviceRequest::getProductKey()const
{
	return productKey_;
}

void CancelOTATaskByDeviceRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string CancelOTATaskByDeviceRequest::getApiProduct()const
{
	return apiProduct_;
}

void CancelOTATaskByDeviceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CancelOTATaskByDeviceRequest::getApiRevision()const
{
	return apiRevision_;
}

void CancelOTATaskByDeviceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::vector<std::string> CancelOTATaskByDeviceRequest::getDeviceName()const
{
	return deviceName_;
}

void CancelOTATaskByDeviceRequest::setDeviceName(const std::vector<std::string>& deviceName)
{
	deviceName_ = deviceName;
	for(int dep1 = 0; dep1!= deviceName.size(); dep1++) {
		setParameter("DeviceName."+ std::to_string(dep1), deviceName.at(dep1));
	}
}

