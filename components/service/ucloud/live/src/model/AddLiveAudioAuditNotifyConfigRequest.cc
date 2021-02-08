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

#include <alibabacloud/live/model/AddLiveAudioAuditNotifyConfigRequest.h>

using AlibabaCloud::Live::Model::AddLiveAudioAuditNotifyConfigRequest;

AddLiveAudioAuditNotifyConfigRequest::AddLiveAudioAuditNotifyConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "AddLiveAudioAuditNotifyConfig")
{
	setMethod(HttpRequest::Method::Post);
}

AddLiveAudioAuditNotifyConfigRequest::~AddLiveAudioAuditNotifyConfigRequest()
{}

std::string AddLiveAudioAuditNotifyConfigRequest::getDomainName()const
{
	return domainName_;
}

void AddLiveAudioAuditNotifyConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long AddLiveAudioAuditNotifyConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void AddLiveAudioAuditNotifyConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string AddLiveAudioAuditNotifyConfigRequest::getCallbackTemplate()const
{
	return callbackTemplate_;
}

void AddLiveAudioAuditNotifyConfigRequest::setCallbackTemplate(const std::string& callbackTemplate)
{
	callbackTemplate_ = callbackTemplate;
	setParameter("CallbackTemplate", callbackTemplate);
}

std::string AddLiveAudioAuditNotifyConfigRequest::getCallback()const
{
	return callback_;
}

void AddLiveAudioAuditNotifyConfigRequest::setCallback(const std::string& callback)
{
	callback_ = callback;
	setParameter("Callback", callback);
}

