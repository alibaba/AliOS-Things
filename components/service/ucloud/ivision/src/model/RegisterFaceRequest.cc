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

#include <alibabacloud/ivision/model/RegisterFaceRequest.h>

using AlibabaCloud::Ivision::Model::RegisterFaceRequest;

RegisterFaceRequest::RegisterFaceRequest() :
	RpcServiceRequest("ivision", "2019-03-08", "RegisterFace")
{
	setMethod(HttpRequest::Method::Get);
}

RegisterFaceRequest::~RegisterFaceRequest()
{}

std::string RegisterFaceRequest::getContent()const
{
	return content_;
}

void RegisterFaceRequest::setContent(const std::string& content)
{
	content_ = content;
	setParameter("Content", content);
}

std::string RegisterFaceRequest::getDataType()const
{
	return dataType_;
}

void RegisterFaceRequest::setDataType(const std::string& dataType)
{
	dataType_ = dataType;
	setParameter("DataType", dataType);
}

std::string RegisterFaceRequest::getShowLog()const
{
	return showLog_;
}

void RegisterFaceRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string RegisterFaceRequest::getGroupId()const
{
	return groupId_;
}

void RegisterFaceRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

long RegisterFaceRequest::getOwnerId()const
{
	return ownerId_;
}

void RegisterFaceRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

