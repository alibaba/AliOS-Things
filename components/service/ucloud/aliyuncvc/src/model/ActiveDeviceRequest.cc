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

#include <alibabacloud/aliyuncvc/model/ActiveDeviceRequest.h>

using AlibabaCloud::Aliyuncvc::Model::ActiveDeviceRequest;

ActiveDeviceRequest::ActiveDeviceRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "ActiveDevice")
{
	setMethod(HttpRequest::Method::Post);
}

ActiveDeviceRequest::~ActiveDeviceRequest()
{}

std::string ActiveDeviceRequest::getIP()const
{
	return iP_;
}

void ActiveDeviceRequest::setIP(const std::string& iP)
{
	iP_ = iP;
	setBodyParameter("IP", iP);
}

std::string ActiveDeviceRequest::getActiveCode()const
{
	return activeCode_;
}

void ActiveDeviceRequest::setActiveCode(const std::string& activeCode)
{
	activeCode_ = activeCode;
	setBodyParameter("ActiveCode", activeCode);
}

std::string ActiveDeviceRequest::getMac()const
{
	return mac_;
}

void ActiveDeviceRequest::setMac(const std::string& mac)
{
	mac_ = mac;
	setBodyParameter("Mac", mac);
}

std::string ActiveDeviceRequest::getDeviceVersion()const
{
	return deviceVersion_;
}

void ActiveDeviceRequest::setDeviceVersion(const std::string& deviceVersion)
{
	deviceVersion_ = deviceVersion;
	setBodyParameter("DeviceVersion", deviceVersion);
}

std::string ActiveDeviceRequest::getSN()const
{
	return sN_;
}

void ActiveDeviceRequest::setSN(const std::string& sN)
{
	sN_ = sN;
	setBodyParameter("SN", sN);
}

