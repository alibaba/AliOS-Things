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

#include <alibabacloud/aliyuncvc/model/GetDeviceTokenRequest.h>

using AlibabaCloud::Aliyuncvc::Model::GetDeviceTokenRequest;

GetDeviceTokenRequest::GetDeviceTokenRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "GetDeviceToken")
{
	setMethod(HttpRequest::Method::Post);
}

GetDeviceTokenRequest::~GetDeviceTokenRequest()
{}

std::string GetDeviceTokenRequest::getToken()const
{
	return token_;
}

void GetDeviceTokenRequest::setToken(const std::string& token)
{
	token_ = token;
	setParameter("Token", token);
}

std::string GetDeviceTokenRequest::getSN()const
{
	return sN_;
}

void GetDeviceTokenRequest::setSN(const std::string& sN)
{
	sN_ = sN;
	setParameter("SN", sN);
}

