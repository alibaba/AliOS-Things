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

#include <alibabacloud/rtc/model/CreateServiceLinkedRoleForRtcRequest.h>

using AlibabaCloud::Rtc::Model::CreateServiceLinkedRoleForRtcRequest;

CreateServiceLinkedRoleForRtcRequest::CreateServiceLinkedRoleForRtcRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "CreateServiceLinkedRoleForRtc")
{
	setMethod(HttpRequest::Method::Post);
}

CreateServiceLinkedRoleForRtcRequest::~CreateServiceLinkedRoleForRtcRequest()
{}

std::string CreateServiceLinkedRoleForRtcRequest::getShowLog()const
{
	return showLog_;
}

void CreateServiceLinkedRoleForRtcRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long CreateServiceLinkedRoleForRtcRequest::getOwnerId()const
{
	return ownerId_;
}

void CreateServiceLinkedRoleForRtcRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

