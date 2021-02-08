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

#include <alibabacloud/iot/model/UpdateSubscribeRelationRequest.h>

using AlibabaCloud::Iot::Model::UpdateSubscribeRelationRequest;

UpdateSubscribeRelationRequest::UpdateSubscribeRelationRequest() :
	RpcServiceRequest("iot", "2018-01-20", "UpdateSubscribeRelation")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateSubscribeRelationRequest::~UpdateSubscribeRelationRequest()
{}

bool UpdateSubscribeRelationRequest::getOtaEventFlag()const
{
	return otaEventFlag_;
}

void UpdateSubscribeRelationRequest::setOtaEventFlag(bool otaEventFlag)
{
	otaEventFlag_ = otaEventFlag;
	setParameter("OtaEventFlag", otaEventFlag ? "true" : "false");
}

bool UpdateSubscribeRelationRequest::getDeviceTopoLifeCycleFlag()const
{
	return deviceTopoLifeCycleFlag_;
}

void UpdateSubscribeRelationRequest::setDeviceTopoLifeCycleFlag(bool deviceTopoLifeCycleFlag)
{
	deviceTopoLifeCycleFlag_ = deviceTopoLifeCycleFlag;
	setParameter("DeviceTopoLifeCycleFlag", deviceTopoLifeCycleFlag ? "true" : "false");
}

std::string UpdateSubscribeRelationRequest::getType()const
{
	return type_;
}

void UpdateSubscribeRelationRequest::setType(const std::string& type)
{
	type_ = type;
	setParameter("Type", type);
}

bool UpdateSubscribeRelationRequest::getDeviceLifeCycleFlag()const
{
	return deviceLifeCycleFlag_;
}

void UpdateSubscribeRelationRequest::setDeviceLifeCycleFlag(bool deviceLifeCycleFlag)
{
	deviceLifeCycleFlag_ = deviceLifeCycleFlag;
	setParameter("DeviceLifeCycleFlag", deviceLifeCycleFlag ? "true" : "false");
}

std::string UpdateSubscribeRelationRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void UpdateSubscribeRelationRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

bool UpdateSubscribeRelationRequest::getDeviceStatusChangeFlag()const
{
	return deviceStatusChangeFlag_;
}

void UpdateSubscribeRelationRequest::setDeviceStatusChangeFlag(bool deviceStatusChangeFlag)
{
	deviceStatusChangeFlag_ = deviceStatusChangeFlag;
	setParameter("DeviceStatusChangeFlag", deviceStatusChangeFlag ? "true" : "false");
}

std::vector<std::string> UpdateSubscribeRelationRequest::getConsumerGroupIds()const
{
	return consumerGroupIds_;
}

void UpdateSubscribeRelationRequest::setConsumerGroupIds(const std::vector<std::string>& consumerGroupIds)
{
	consumerGroupIds_ = consumerGroupIds;
	for(int dep1 = 0; dep1!= consumerGroupIds.size(); dep1++) {
		setParameter("ConsumerGroupIds."+ std::to_string(dep1), consumerGroupIds.at(dep1));
	}
}

std::string UpdateSubscribeRelationRequest::getProductKey()const
{
	return productKey_;
}

void UpdateSubscribeRelationRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

bool UpdateSubscribeRelationRequest::getThingHistoryFlag()const
{
	return thingHistoryFlag_;
}

void UpdateSubscribeRelationRequest::setThingHistoryFlag(bool thingHistoryFlag)
{
	thingHistoryFlag_ = thingHistoryFlag;
	setParameter("ThingHistoryFlag", thingHistoryFlag ? "true" : "false");
}

bool UpdateSubscribeRelationRequest::getFoundDeviceListFlag()const
{
	return foundDeviceListFlag_;
}

void UpdateSubscribeRelationRequest::setFoundDeviceListFlag(bool foundDeviceListFlag)
{
	foundDeviceListFlag_ = foundDeviceListFlag;
	setParameter("FoundDeviceListFlag", foundDeviceListFlag ? "true" : "false");
}

std::string UpdateSubscribeRelationRequest::getApiProduct()const
{
	return apiProduct_;
}

void UpdateSubscribeRelationRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

bool UpdateSubscribeRelationRequest::getDeviceDataFlag()const
{
	return deviceDataFlag_;
}

void UpdateSubscribeRelationRequest::setDeviceDataFlag(bool deviceDataFlag)
{
	deviceDataFlag_ = deviceDataFlag;
	setParameter("DeviceDataFlag", deviceDataFlag ? "true" : "false");
}

std::string UpdateSubscribeRelationRequest::getApiRevision()const
{
	return apiRevision_;
}

void UpdateSubscribeRelationRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string UpdateSubscribeRelationRequest::getMnsConfiguration()const
{
	return mnsConfiguration_;
}

void UpdateSubscribeRelationRequest::setMnsConfiguration(const std::string& mnsConfiguration)
{
	mnsConfiguration_ = mnsConfiguration;
	setParameter("MnsConfiguration", mnsConfiguration);
}

