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

#include <alibabacloud/iot/model/CreateSubscribeRelationRequest.h>

using AlibabaCloud::Iot::Model::CreateSubscribeRelationRequest;

CreateSubscribeRelationRequest::CreateSubscribeRelationRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateSubscribeRelation")
{
	setMethod(HttpRequest::Method::Post);
}

CreateSubscribeRelationRequest::~CreateSubscribeRelationRequest()
{}

bool CreateSubscribeRelationRequest::getOtaEventFlag()const
{
	return otaEventFlag_;
}

void CreateSubscribeRelationRequest::setOtaEventFlag(bool otaEventFlag)
{
	otaEventFlag_ = otaEventFlag;
	setParameter("OtaEventFlag", otaEventFlag ? "true" : "false");
}

bool CreateSubscribeRelationRequest::getDeviceTopoLifeCycleFlag()const
{
	return deviceTopoLifeCycleFlag_;
}

void CreateSubscribeRelationRequest::setDeviceTopoLifeCycleFlag(bool deviceTopoLifeCycleFlag)
{
	deviceTopoLifeCycleFlag_ = deviceTopoLifeCycleFlag;
	setParameter("DeviceTopoLifeCycleFlag", deviceTopoLifeCycleFlag ? "true" : "false");
}

bool CreateSubscribeRelationRequest::getDeviceLifeCycleFlag()const
{
	return deviceLifeCycleFlag_;
}

void CreateSubscribeRelationRequest::setDeviceLifeCycleFlag(bool deviceLifeCycleFlag)
{
	deviceLifeCycleFlag_ = deviceLifeCycleFlag;
	setParameter("DeviceLifeCycleFlag", deviceLifeCycleFlag ? "true" : "false");
}

std::string CreateSubscribeRelationRequest::getType()const
{
	return type_;
}

void CreateSubscribeRelationRequest::setType(const std::string& type)
{
	type_ = type;
	setParameter("Type", type);
}

std::string CreateSubscribeRelationRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateSubscribeRelationRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

bool CreateSubscribeRelationRequest::getDeviceStatusChangeFlag()const
{
	return deviceStatusChangeFlag_;
}

void CreateSubscribeRelationRequest::setDeviceStatusChangeFlag(bool deviceStatusChangeFlag)
{
	deviceStatusChangeFlag_ = deviceStatusChangeFlag;
	setParameter("DeviceStatusChangeFlag", deviceStatusChangeFlag ? "true" : "false");
}

std::vector<std::string> CreateSubscribeRelationRequest::getConsumerGroupIds()const
{
	return consumerGroupIds_;
}

void CreateSubscribeRelationRequest::setConsumerGroupIds(const std::vector<std::string>& consumerGroupIds)
{
	consumerGroupIds_ = consumerGroupIds;
	for(int dep1 = 0; dep1!= consumerGroupIds.size(); dep1++) {
		setParameter("ConsumerGroupIds."+ std::to_string(dep1), consumerGroupIds.at(dep1));
	}
}

std::string CreateSubscribeRelationRequest::getProductKey()const
{
	return productKey_;
}

void CreateSubscribeRelationRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

bool CreateSubscribeRelationRequest::getThingHistoryFlag()const
{
	return thingHistoryFlag_;
}

void CreateSubscribeRelationRequest::setThingHistoryFlag(bool thingHistoryFlag)
{
	thingHistoryFlag_ = thingHistoryFlag;
	setParameter("ThingHistoryFlag", thingHistoryFlag ? "true" : "false");
}

bool CreateSubscribeRelationRequest::getFoundDeviceListFlag()const
{
	return foundDeviceListFlag_;
}

void CreateSubscribeRelationRequest::setFoundDeviceListFlag(bool foundDeviceListFlag)
{
	foundDeviceListFlag_ = foundDeviceListFlag;
	setParameter("FoundDeviceListFlag", foundDeviceListFlag ? "true" : "false");
}

std::string CreateSubscribeRelationRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateSubscribeRelationRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

bool CreateSubscribeRelationRequest::getDeviceDataFlag()const
{
	return deviceDataFlag_;
}

void CreateSubscribeRelationRequest::setDeviceDataFlag(bool deviceDataFlag)
{
	deviceDataFlag_ = deviceDataFlag;
	setParameter("DeviceDataFlag", deviceDataFlag ? "true" : "false");
}

std::string CreateSubscribeRelationRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateSubscribeRelationRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string CreateSubscribeRelationRequest::getMnsConfiguration()const
{
	return mnsConfiguration_;
}

void CreateSubscribeRelationRequest::setMnsConfiguration(const std::string& mnsConfiguration)
{
	mnsConfiguration_ = mnsConfiguration;
	setParameter("MnsConfiguration", mnsConfiguration);
}

