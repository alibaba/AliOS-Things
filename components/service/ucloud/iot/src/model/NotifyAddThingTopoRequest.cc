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

#include <alibabacloud/iot/model/NotifyAddThingTopoRequest.h>

using AlibabaCloud::Iot::Model::NotifyAddThingTopoRequest;

NotifyAddThingTopoRequest::NotifyAddThingTopoRequest() :
	RpcServiceRequest("iot", "2018-01-20", "NotifyAddThingTopo")
{
	setMethod(HttpRequest::Method::Post);
}

NotifyAddThingTopoRequest::~NotifyAddThingTopoRequest()
{}

std::string NotifyAddThingTopoRequest::getGwProductKey()const
{
	return gwProductKey_;
}

void NotifyAddThingTopoRequest::setGwProductKey(const std::string& gwProductKey)
{
	gwProductKey_ = gwProductKey;
	setParameter("GwProductKey", gwProductKey);
}

std::string NotifyAddThingTopoRequest::getDeviceListStr()const
{
	return deviceListStr_;
}

void NotifyAddThingTopoRequest::setDeviceListStr(const std::string& deviceListStr)
{
	deviceListStr_ = deviceListStr;
	setParameter("DeviceListStr", deviceListStr);
}

std::string NotifyAddThingTopoRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void NotifyAddThingTopoRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string NotifyAddThingTopoRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void NotifyAddThingTopoRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string NotifyAddThingTopoRequest::getGwIotId()const
{
	return gwIotId_;
}

void NotifyAddThingTopoRequest::setGwIotId(const std::string& gwIotId)
{
	gwIotId_ = gwIotId;
	setParameter("GwIotId", gwIotId);
}

std::string NotifyAddThingTopoRequest::getGwDeviceName()const
{
	return gwDeviceName_;
}

void NotifyAddThingTopoRequest::setGwDeviceName(const std::string& gwDeviceName)
{
	gwDeviceName_ = gwDeviceName;
	setParameter("GwDeviceName", gwDeviceName);
}

std::string NotifyAddThingTopoRequest::getApiProduct()const
{
	return apiProduct_;
}

void NotifyAddThingTopoRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string NotifyAddThingTopoRequest::getApiRevision()const
{
	return apiRevision_;
}

void NotifyAddThingTopoRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

