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

#include <alibabacloud/iot/model/QueryDeviceFileRequest.h>

using AlibabaCloud::Iot::Model::QueryDeviceFileRequest;

QueryDeviceFileRequest::QueryDeviceFileRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryDeviceFile")
{
	setMethod(HttpRequest::Method::Post);
}

QueryDeviceFileRequest::~QueryDeviceFileRequest()
{}

std::string QueryDeviceFileRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryDeviceFileRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryDeviceFileRequest::getIotId()const
{
	return iotId_;
}

void QueryDeviceFileRequest::setIotId(const std::string& iotId)
{
	iotId_ = iotId;
	setParameter("IotId", iotId);
}

std::string QueryDeviceFileRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryDeviceFileRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string QueryDeviceFileRequest::getFileId()const
{
	return fileId_;
}

void QueryDeviceFileRequest::setFileId(const std::string& fileId)
{
	fileId_ = fileId;
	setParameter("FileId", fileId);
}

std::string QueryDeviceFileRequest::getProductKey()const
{
	return productKey_;
}

void QueryDeviceFileRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string QueryDeviceFileRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryDeviceFileRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryDeviceFileRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryDeviceFileRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string QueryDeviceFileRequest::getDeviceName()const
{
	return deviceName_;
}

void QueryDeviceFileRequest::setDeviceName(const std::string& deviceName)
{
	deviceName_ = deviceName;
	setParameter("DeviceName", deviceName);
}

