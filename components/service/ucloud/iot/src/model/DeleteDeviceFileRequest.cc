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

#include <alibabacloud/iot/model/DeleteDeviceFileRequest.h>

using AlibabaCloud::Iot::Model::DeleteDeviceFileRequest;

DeleteDeviceFileRequest::DeleteDeviceFileRequest() :
	RpcServiceRequest("iot", "2018-01-20", "DeleteDeviceFile")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteDeviceFileRequest::~DeleteDeviceFileRequest()
{}

std::string DeleteDeviceFileRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteDeviceFileRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteDeviceFileRequest::getIotId()const
{
	return iotId_;
}

void DeleteDeviceFileRequest::setIotId(const std::string& iotId)
{
	iotId_ = iotId;
	setParameter("IotId", iotId);
}

std::string DeleteDeviceFileRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void DeleteDeviceFileRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string DeleteDeviceFileRequest::getFileId()const
{
	return fileId_;
}

void DeleteDeviceFileRequest::setFileId(const std::string& fileId)
{
	fileId_ = fileId;
	setParameter("FileId", fileId);
}

std::string DeleteDeviceFileRequest::getProductKey()const
{
	return productKey_;
}

void DeleteDeviceFileRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string DeleteDeviceFileRequest::getApiProduct()const
{
	return apiProduct_;
}

void DeleteDeviceFileRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string DeleteDeviceFileRequest::getApiRevision()const
{
	return apiRevision_;
}

void DeleteDeviceFileRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string DeleteDeviceFileRequest::getDeviceName()const
{
	return deviceName_;
}

void DeleteDeviceFileRequest::setDeviceName(const std::string& deviceName)
{
	deviceName_ = deviceName;
	setParameter("DeviceName", deviceName);
}

