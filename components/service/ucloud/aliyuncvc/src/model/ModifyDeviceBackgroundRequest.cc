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

#include <alibabacloud/aliyuncvc/model/ModifyDeviceBackgroundRequest.h>

using AlibabaCloud::Aliyuncvc::Model::ModifyDeviceBackgroundRequest;

ModifyDeviceBackgroundRequest::ModifyDeviceBackgroundRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "ModifyDeviceBackground")
{
	setMethod(HttpRequest::Method::Post);
}

ModifyDeviceBackgroundRequest::~ModifyDeviceBackgroundRequest()
{}

std::string ModifyDeviceBackgroundRequest::getSerialNumber()const
{
	return serialNumber_;
}

void ModifyDeviceBackgroundRequest::setSerialNumber(const std::string& serialNumber)
{
	serialNumber_ = serialNumber;
	setBodyParameter("SerialNumber", serialNumber);
}

std::string ModifyDeviceBackgroundRequest::getPicture()const
{
	return picture_;
}

void ModifyDeviceBackgroundRequest::setPicture(const std::string& picture)
{
	picture_ = picture;
	setBodyParameter("Picture", picture);
}

