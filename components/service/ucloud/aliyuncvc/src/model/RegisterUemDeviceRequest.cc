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

#include <alibabacloud/aliyuncvc/model/RegisterUemDeviceRequest.h>

using AlibabaCloud::Aliyuncvc::Model::RegisterUemDeviceRequest;

RegisterUemDeviceRequest::RegisterUemDeviceRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "RegisterUemDevice")
{
	setMethod(HttpRequest::Method::Post);
}

RegisterUemDeviceRequest::~RegisterUemDeviceRequest()
{}

std::string RegisterUemDeviceRequest::getIP()const
{
	return iP_;
}

void RegisterUemDeviceRequest::setIP(const std::string& iP)
{
	iP_ = iP;
	setParameter("IP", iP);
}

std::string RegisterUemDeviceRequest::getGroupId()const
{
	return groupId_;
}

void RegisterUemDeviceRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string RegisterUemDeviceRequest::getOwnerId()const
{
	return ownerId_;
}

void RegisterUemDeviceRequest::setOwnerId(const std::string& ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", ownerId);
}

std::string RegisterUemDeviceRequest::getDeviceId()const
{
	return deviceId_;
}

void RegisterUemDeviceRequest::setDeviceId(const std::string& deviceId)
{
	deviceId_ = deviceId;
	setParameter("DeviceId", deviceId);
}

std::string RegisterUemDeviceRequest::getGroupName()const
{
	return groupName_;
}

void RegisterUemDeviceRequest::setGroupName(const std::string& groupName)
{
	groupName_ = groupName;
	setParameter("GroupName", groupName);
}

std::string RegisterUemDeviceRequest::getMac()const
{
	return mac_;
}

void RegisterUemDeviceRequest::setMac(const std::string& mac)
{
	mac_ = mac;
	setParameter("Mac", mac);
}

std::string RegisterUemDeviceRequest::getDeviceVersion()const
{
	return deviceVersion_;
}

void RegisterUemDeviceRequest::setDeviceVersion(const std::string& deviceVersion)
{
	deviceVersion_ = deviceVersion;
	setParameter("DeviceVersion", deviceVersion);
}

