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

#include <alibabacloud/cloudapi/model/CreateTrafficControlRequest.h>

using AlibabaCloud::CloudAPI::Model::CreateTrafficControlRequest;

CreateTrafficControlRequest::CreateTrafficControlRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "CreateTrafficControl")
{
	setMethod(HttpRequest::Method::Post);
}

CreateTrafficControlRequest::~CreateTrafficControlRequest()
{}

std::string CreateTrafficControlRequest::getTrafficControlName()const
{
	return trafficControlName_;
}

void CreateTrafficControlRequest::setTrafficControlName(const std::string& trafficControlName)
{
	trafficControlName_ = trafficControlName;
	setParameter("TrafficControlName", trafficControlName);
}

std::string CreateTrafficControlRequest::getDescription()const
{
	return description_;
}

void CreateTrafficControlRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

int CreateTrafficControlRequest::getUserDefault()const
{
	return userDefault_;
}

void CreateTrafficControlRequest::setUserDefault(int userDefault)
{
	userDefault_ = userDefault;
	setParameter("UserDefault", std::to_string(userDefault));
}

std::string CreateTrafficControlRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateTrafficControlRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

int CreateTrafficControlRequest::getApiDefault()const
{
	return apiDefault_;
}

void CreateTrafficControlRequest::setApiDefault(int apiDefault)
{
	apiDefault_ = apiDefault;
	setParameter("ApiDefault", std::to_string(apiDefault));
}

std::string CreateTrafficControlRequest::getSecurityToken()const
{
	return securityToken_;
}

void CreateTrafficControlRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string CreateTrafficControlRequest::getTrafficControlUnit()const
{
	return trafficControlUnit_;
}

void CreateTrafficControlRequest::setTrafficControlUnit(const std::string& trafficControlUnit)
{
	trafficControlUnit_ = trafficControlUnit;
	setParameter("TrafficControlUnit", trafficControlUnit);
}

int CreateTrafficControlRequest::getAppDefault()const
{
	return appDefault_;
}

void CreateTrafficControlRequest::setAppDefault(int appDefault)
{
	appDefault_ = appDefault;
	setParameter("AppDefault", std::to_string(appDefault));
}

