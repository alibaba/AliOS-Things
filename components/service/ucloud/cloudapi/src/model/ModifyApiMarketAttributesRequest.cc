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

#include <alibabacloud/cloudapi/model/ModifyApiMarketAttributesRequest.h>

using AlibabaCloud::CloudAPI::Model::ModifyApiMarketAttributesRequest;

ModifyApiMarketAttributesRequest::ModifyApiMarketAttributesRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "ModifyApiMarketAttributes")
{
	setMethod(HttpRequest::Method::Post);
}

ModifyApiMarketAttributesRequest::~ModifyApiMarketAttributesRequest()
{}

std::string ModifyApiMarketAttributesRequest::getGroupId()const
{
	return groupId_;
}

void ModifyApiMarketAttributesRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string ModifyApiMarketAttributesRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ModifyApiMarketAttributesRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ModifyApiMarketAttributesRequest::getSecurityToken()const
{
	return securityToken_;
}

void ModifyApiMarketAttributesRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string ModifyApiMarketAttributesRequest::getMarketChargingMode()const
{
	return marketChargingMode_;
}

void ModifyApiMarketAttributesRequest::setMarketChargingMode(const std::string& marketChargingMode)
{
	marketChargingMode_ = marketChargingMode;
	setParameter("MarketChargingMode", marketChargingMode);
}

bool ModifyApiMarketAttributesRequest::getNeedCharging()const
{
	return needCharging_;
}

void ModifyApiMarketAttributesRequest::setNeedCharging(bool needCharging)
{
	needCharging_ = needCharging;
	setParameter("NeedCharging", needCharging ? "true" : "false");
}

std::string ModifyApiMarketAttributesRequest::getApiId()const
{
	return apiId_;
}

void ModifyApiMarketAttributesRequest::setApiId(const std::string& apiId)
{
	apiId_ = apiId;
	setParameter("ApiId", apiId);
}

