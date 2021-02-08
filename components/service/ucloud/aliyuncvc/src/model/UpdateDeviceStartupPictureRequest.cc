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

#include <alibabacloud/aliyuncvc/model/UpdateDeviceStartupPictureRequest.h>

using AlibabaCloud::Aliyuncvc::Model::UpdateDeviceStartupPictureRequest;

UpdateDeviceStartupPictureRequest::UpdateDeviceStartupPictureRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "UpdateDeviceStartupPicture")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateDeviceStartupPictureRequest::~UpdateDeviceStartupPictureRequest()
{}

std::string UpdateDeviceStartupPictureRequest::getSerialNumber()const
{
	return serialNumber_;
}

void UpdateDeviceStartupPictureRequest::setSerialNumber(const std::string& serialNumber)
{
	serialNumber_ = serialNumber;
	setBodyParameter("SerialNumber", serialNumber);
}

std::string UpdateDeviceStartupPictureRequest::getPicture()const
{
	return picture_;
}

void UpdateDeviceStartupPictureRequest::setPicture(const std::string& picture)
{
	picture_ = picture;
	setBodyParameter("Picture", picture);
}

