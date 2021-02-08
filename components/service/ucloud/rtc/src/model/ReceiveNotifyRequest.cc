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

#include <alibabacloud/rtc/model/ReceiveNotifyRequest.h>

using AlibabaCloud::Rtc::Model::ReceiveNotifyRequest;

ReceiveNotifyRequest::ReceiveNotifyRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "ReceiveNotify")
{
	setMethod(HttpRequest::Method::Post);
}

ReceiveNotifyRequest::~ReceiveNotifyRequest()
{}

std::string ReceiveNotifyRequest::getTraceId()const
{
	return traceId_;
}

void ReceiveNotifyRequest::setTraceId(const std::string& traceId)
{
	traceId_ = traceId;
	setParameter("TraceId", traceId);
}

std::string ReceiveNotifyRequest::getContent()const
{
	return content_;
}

void ReceiveNotifyRequest::setContent(const std::string& content)
{
	content_ = content;
	setParameter("Content", content);
}

std::string ReceiveNotifyRequest::getEvent()const
{
	return event_;
}

void ReceiveNotifyRequest::setEvent(const std::string& event)
{
	event_ = event;
	setParameter("Event", event);
}

std::string ReceiveNotifyRequest::getShowLog()const
{
	return showLog_;
}

void ReceiveNotifyRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long ReceiveNotifyRequest::getOwnerId()const
{
	return ownerId_;
}

void ReceiveNotifyRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string ReceiveNotifyRequest::getContentType()const
{
	return contentType_;
}

void ReceiveNotifyRequest::setContentType(const std::string& contentType)
{
	contentType_ = contentType;
	setParameter("ContentType", contentType);
}

std::string ReceiveNotifyRequest::getBizId()const
{
	return bizId_;
}

void ReceiveNotifyRequest::setBizId(const std::string& bizId)
{
	bizId_ = bizId;
	setParameter("BizId", bizId);
}

