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

#include <alibabacloud/schedulerx2/model/CreateAppGroupRequest.h>

using AlibabaCloud::Schedulerx2::Model::CreateAppGroupRequest;

CreateAppGroupRequest::CreateAppGroupRequest() :
	RpcServiceRequest("schedulerx2", "2019-04-30", "CreateAppGroup")
{
	setMethod(HttpRequest::Method::Get);
}

CreateAppGroupRequest::~CreateAppGroupRequest()
{}

int CreateAppGroupRequest::getMaxJobs()const
{
	return maxJobs_;
}

void CreateAppGroupRequest::setMaxJobs(int maxJobs)
{
	maxJobs_ = maxJobs;
	setParameter("MaxJobs", std::to_string(maxJobs));
}

std::string CreateAppGroupRequest::getNamespaceName()const
{
	return namespaceName_;
}

void CreateAppGroupRequest::setNamespaceName(const std::string& namespaceName)
{
	namespaceName_ = namespaceName;
	setParameter("NamespaceName", namespaceName);
}

std::string CreateAppGroupRequest::getNamespaceSource()const
{
	return namespaceSource_;
}

void CreateAppGroupRequest::setNamespaceSource(const std::string& namespaceSource)
{
	namespaceSource_ = namespaceSource;
	setParameter("NamespaceSource", namespaceSource);
}

std::string CreateAppGroupRequest::getMetricsThresholdJson()const
{
	return metricsThresholdJson_;
}

void CreateAppGroupRequest::setMetricsThresholdJson(const std::string& metricsThresholdJson)
{
	metricsThresholdJson_ = metricsThresholdJson;
	setParameter("MetricsThresholdJson", metricsThresholdJson);
}

std::string CreateAppGroupRequest::getGroupId()const
{
	return groupId_;
}

void CreateAppGroupRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string CreateAppGroupRequest::getDescription()const
{
	return description_;
}

void CreateAppGroupRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

std::string CreateAppGroupRequest::getAppName()const
{
	return appName_;
}

void CreateAppGroupRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string CreateAppGroupRequest::get_Namespace()const
{
	return _namespace_;
}

void CreateAppGroupRequest::set_Namespace(const std::string& _namespace)
{
	_namespace_ = _namespace;
	setParameter("_Namespace", _namespace);
}

std::string CreateAppGroupRequest::getAlarmJson()const
{
	return alarmJson_;
}

void CreateAppGroupRequest::setAlarmJson(const std::string& alarmJson)
{
	alarmJson_ = alarmJson;
	setParameter("AlarmJson", alarmJson);
}

