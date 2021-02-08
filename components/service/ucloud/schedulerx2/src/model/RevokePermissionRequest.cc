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

#include <alibabacloud/schedulerx2/model/RevokePermissionRequest.h>

using AlibabaCloud::Schedulerx2::Model::RevokePermissionRequest;

RevokePermissionRequest::RevokePermissionRequest() :
	RpcServiceRequest("schedulerx2", "2019-04-30", "RevokePermission")
{
	setMethod(HttpRequest::Method::Post);
}

RevokePermissionRequest::~RevokePermissionRequest()
{}

std::string RevokePermissionRequest::getNamespaceSource()const
{
	return namespaceSource_;
}

void RevokePermissionRequest::setNamespaceSource(const std::string& namespaceSource)
{
	namespaceSource_ = namespaceSource;
	setParameter("NamespaceSource", namespaceSource);
}

std::string RevokePermissionRequest::getGroupId()const
{
	return groupId_;
}

void RevokePermissionRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string RevokePermissionRequest::get_Namespace()const
{
	return _namespace_;
}

void RevokePermissionRequest::set_Namespace(const std::string& _namespace)
{
	_namespace_ = _namespace;
	setParameter("_Namespace", _namespace);
}

std::string RevokePermissionRequest::getUserId()const
{
	return userId_;
}

void RevokePermissionRequest::setUserId(const std::string& userId)
{
	userId_ = userId;
	setParameter("UserId", userId);
}

