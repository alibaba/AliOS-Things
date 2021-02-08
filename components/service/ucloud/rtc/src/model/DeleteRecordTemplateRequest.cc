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

#include <alibabacloud/rtc/model/DeleteRecordTemplateRequest.h>

using AlibabaCloud::Rtc::Model::DeleteRecordTemplateRequest;

DeleteRecordTemplateRequest::DeleteRecordTemplateRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DeleteRecordTemplate")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteRecordTemplateRequest::~DeleteRecordTemplateRequest()
{}

std::string DeleteRecordTemplateRequest::getShowLog()const
{
	return showLog_;
}

void DeleteRecordTemplateRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long DeleteRecordTemplateRequest::getOwnerId()const
{
	return ownerId_;
}

void DeleteRecordTemplateRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DeleteRecordTemplateRequest::getTemplateId()const
{
	return templateId_;
}

void DeleteRecordTemplateRequest::setTemplateId(const std::string& templateId)
{
	templateId_ = templateId;
	setParameter("TemplateId", templateId);
}

std::string DeleteRecordTemplateRequest::getAppId()const
{
	return appId_;
}

void DeleteRecordTemplateRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

