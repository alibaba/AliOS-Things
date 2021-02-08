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

#include <alibabacloud/rtc/model/DeleteMPURuleRequest.h>

using AlibabaCloud::Rtc::Model::DeleteMPURuleRequest;

DeleteMPURuleRequest::DeleteMPURuleRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DeleteMPURule")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteMPURuleRequest::~DeleteMPURuleRequest()
{}

std::string DeleteMPURuleRequest::getShowLog()const
{
	return showLog_;
}

void DeleteMPURuleRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long DeleteMPURuleRequest::getOwnerId()const
{
	return ownerId_;
}

void DeleteMPURuleRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DeleteMPURuleRequest::getAppId()const
{
	return appId_;
}

void DeleteMPURuleRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

long DeleteMPURuleRequest::getRuleId()const
{
	return ruleId_;
}

void DeleteMPURuleRequest::setRuleId(long ruleId)
{
	ruleId_ = ruleId;
	setParameter("RuleId", std::to_string(ruleId));
}

