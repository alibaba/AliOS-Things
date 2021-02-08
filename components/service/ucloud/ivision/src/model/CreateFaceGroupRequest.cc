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

#include <alibabacloud/ivision/model/CreateFaceGroupRequest.h>

using AlibabaCloud::Ivision::Model::CreateFaceGroupRequest;

CreateFaceGroupRequest::CreateFaceGroupRequest() :
	RpcServiceRequest("ivision", "2019-03-08", "CreateFaceGroup")
{
	setMethod(HttpRequest::Method::Get);
}

CreateFaceGroupRequest::~CreateFaceGroupRequest()
{}

std::string CreateFaceGroupRequest::getShowLog()const
{
	return showLog_;
}

void CreateFaceGroupRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long CreateFaceGroupRequest::getOwnerId()const
{
	return ownerId_;
}

void CreateFaceGroupRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string CreateFaceGroupRequest::getName()const
{
	return name_;
}

void CreateFaceGroupRequest::setName(const std::string& name)
{
	name_ = name;
	setParameter("Name", name);
}

