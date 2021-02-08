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

#include <alibabacloud/cloudapi/model/ModifyTrafficControlRequest.h>

using AlibabaCloud::CloudAPI::Model::ModifyTrafficControlRequest;

ModifyTrafficControlRequest::ModifyTrafficControlRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "ModifyTrafficControl")
{
	setMethod(HttpRequest::Method::Post);
}

ModifyTrafficControlRequest::~ModifyTrafficControlRequest()
{}

std::string ModifyTrafficControlRequest::getTrafficControlId()const
{
	return trafficControlId_;
}

void ModifyTrafficControlRequest::setTrafficControlId(const std::string& trafficControlId)
{
	trafficControlId_ = trafficControlId;
	setParameter("TrafficControlId", trafficControlId);
}

std::string ModifyTrafficControlRequest::getTrafficControlName()const
{
	return trafficControlName_;
}

void ModifyTrafficControlRequest::setTrafficControlName(const std::string& trafficControlName)
{
	trafficControlName_ = trafficControlName;
	setParameter("TrafficControlName", trafficControlName);
}

std::string ModifyTrafficControlRequest::getDescription()const
{
	return description_;
}

void ModifyTrafficControlRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

int ModifyTrafficControlRequest::getUserDefault()const
{
	return userDefault_;
}

void ModifyTrafficControlRequest::setUserDefault(int userDefault)
{
	userDefault_ = userDefault;
	setParameter("UserDefault", std::to_string(userDefault));
}

std::string ModifyTrafficControlRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ModifyTrafficControlRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

int ModifyTrafficControlRequest::getApiDefault()const
{
	return apiDefault_;
}

void ModifyTrafficControlRequest::setApiDefault(int apiDefault)
{
	apiDefault_ = apiDefault;
	setParameter("ApiDefault", std::to_string(apiDefault));
}

std::string ModifyTrafficControlRequest::getSecurityToken()const
{
	return securityToken_;
}

void ModifyTrafficControlRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string ModifyTrafficControlRequest::getTrafficControlUnit()const
{
	return trafficControlUnit_;
}

void ModifyTrafficControlRequest::setTrafficControlUnit(const std::string& trafficControlUnit)
{
	trafficControlUnit_ = trafficControlUnit;
	setParameter("TrafficControlUnit", trafficControlUnit);
}

int ModifyTrafficControlRequest::getAppDefault()const
{
	return appDefault_;
}

void ModifyTrafficControlRequest::setAppDefault(int appDefault)
{
	appDefault_ = appDefault;
	setParameter("AppDefault", std::to_string(appDefault));
}

