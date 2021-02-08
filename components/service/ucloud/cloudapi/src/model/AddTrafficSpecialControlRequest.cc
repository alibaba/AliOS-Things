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

#include <alibabacloud/cloudapi/model/AddTrafficSpecialControlRequest.h>

using AlibabaCloud::CloudAPI::Model::AddTrafficSpecialControlRequest;

AddTrafficSpecialControlRequest::AddTrafficSpecialControlRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "AddTrafficSpecialControl")
{
	setMethod(HttpRequest::Method::Post);
}

AddTrafficSpecialControlRequest::~AddTrafficSpecialControlRequest()
{}

std::string AddTrafficSpecialControlRequest::getTrafficControlId()const
{
	return trafficControlId_;
}

void AddTrafficSpecialControlRequest::setTrafficControlId(const std::string& trafficControlId)
{
	trafficControlId_ = trafficControlId;
	setParameter("TrafficControlId", trafficControlId);
}

std::string AddTrafficSpecialControlRequest::getSpecialKey()const
{
	return specialKey_;
}

void AddTrafficSpecialControlRequest::setSpecialKey(const std::string& specialKey)
{
	specialKey_ = specialKey;
	setParameter("SpecialKey", specialKey);
}

int AddTrafficSpecialControlRequest::getTrafficValue()const
{
	return trafficValue_;
}

void AddTrafficSpecialControlRequest::setTrafficValue(int trafficValue)
{
	trafficValue_ = trafficValue;
	setParameter("TrafficValue", std::to_string(trafficValue));
}

std::string AddTrafficSpecialControlRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void AddTrafficSpecialControlRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string AddTrafficSpecialControlRequest::getSecurityToken()const
{
	return securityToken_;
}

void AddTrafficSpecialControlRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string AddTrafficSpecialControlRequest::getSpecialType()const
{
	return specialType_;
}

void AddTrafficSpecialControlRequest::setSpecialType(const std::string& specialType)
{
	specialType_ = specialType;
	setParameter("SpecialType", specialType);
}

