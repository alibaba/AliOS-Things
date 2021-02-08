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

#include <alibabacloud/aliyuncvc/model/CreateUserRequest.h>

using AlibabaCloud::Aliyuncvc::Model::CreateUserRequest;

CreateUserRequest::CreateUserRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "CreateUser")
{
	setMethod(HttpRequest::Method::Post);
}

CreateUserRequest::~CreateUserRequest()
{}

int CreateUserRequest::getCount()const
{
	return count_;
}

void CreateUserRequest::setCount(int count)
{
	count_ = count;
	setBodyParameter("Count", std::to_string(count));
}

std::string CreateUserRequest::getUserInfo()const
{
	return userInfo_;
}

void CreateUserRequest::setUserInfo(const std::string& userInfo)
{
	userInfo_ = userInfo;
	setBodyParameter("UserInfo", userInfo);
}

