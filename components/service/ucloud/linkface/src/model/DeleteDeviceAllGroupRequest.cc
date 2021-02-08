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

#include <alibabacloud/linkface/model/DeleteDeviceAllGroupRequest.h>

using AlibabaCloud::LinkFace::Model::DeleteDeviceAllGroupRequest;

DeleteDeviceAllGroupRequest::DeleteDeviceAllGroupRequest() :
	RpcServiceRequest("linkface", "2018-07-20", "DeleteDeviceAllGroup")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteDeviceAllGroupRequest::~DeleteDeviceAllGroupRequest()
{}

std::string DeleteDeviceAllGroupRequest::getIotId()const
{
	return iotId_;
}

void DeleteDeviceAllGroupRequest::setIotId(const std::string& iotId)
{
	iotId_ = iotId;
	setBodyParameter("IotId", iotId);
}

std::string DeleteDeviceAllGroupRequest::getProductKey()const
{
	return productKey_;
}

void DeleteDeviceAllGroupRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setBodyParameter("ProductKey", productKey);
}

std::string DeleteDeviceAllGroupRequest::getApiProduct()const
{
	return apiProduct_;
}

void DeleteDeviceAllGroupRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string DeleteDeviceAllGroupRequest::getApiRevision()const
{
	return apiRevision_;
}

void DeleteDeviceAllGroupRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string DeleteDeviceAllGroupRequest::getDeviceName()const
{
	return deviceName_;
}

void DeleteDeviceAllGroupRequest::setDeviceName(const std::string& deviceName)
{
	deviceName_ = deviceName;
	setBodyParameter("DeviceName", deviceName);
}

