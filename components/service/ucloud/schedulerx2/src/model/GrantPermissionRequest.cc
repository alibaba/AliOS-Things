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

#include <alibabacloud/schedulerx2/model/GrantPermissionRequest.h>

using AlibabaCloud::Schedulerx2::Model::GrantPermissionRequest;

GrantPermissionRequest::GrantPermissionRequest() :
	RpcServiceRequest("schedulerx2", "2019-04-30", "GrantPermission")
{
	setMethod(HttpRequest::Method::Post);
}

GrantPermissionRequest::~GrantPermissionRequest()
{}

std::string GrantPermissionRequest::getNamespaceSource()const
{
	return namespaceSource_;
}

void GrantPermissionRequest::setNamespaceSource(const std::string& namespaceSource)
{
	namespaceSource_ = namespaceSource;
	setParameter("NamespaceSource", namespaceSource);
}

std::string GrantPermissionRequest::getGroupId()const
{
	return groupId_;
}

void GrantPermissionRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string GrantPermissionRequest::getUserId()const
{
	return userId_;
}

void GrantPermissionRequest::setUserId(const std::string& userId)
{
	userId_ = userId;
	setParameter("UserId", userId);
}

bool GrantPermissionRequest::getGrantOption()const
{
	return grantOption_;
}

void GrantPermissionRequest::setGrantOption(bool grantOption)
{
	grantOption_ = grantOption;
	setParameter("GrantOption", grantOption ? "true" : "false");
}

std::string GrantPermissionRequest::get_Namespace()const
{
	return _namespace_;
}

void GrantPermissionRequest::set_Namespace(const std::string& _namespace)
{
	_namespace_ = _namespace;
	setParameter("_Namespace", _namespace);
}

std::string GrantPermissionRequest::getUserName()const
{
	return userName_;
}

void GrantPermissionRequest::setUserName(const std::string& userName)
{
	userName_ = userName;
	setParameter("UserName", userName);
}

