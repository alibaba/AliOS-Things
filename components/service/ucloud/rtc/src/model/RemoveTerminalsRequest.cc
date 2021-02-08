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

#include <alibabacloud/rtc/model/RemoveTerminalsRequest.h>

using AlibabaCloud::Rtc::Model::RemoveTerminalsRequest;

RemoveTerminalsRequest::RemoveTerminalsRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "RemoveTerminals")
{
	setMethod(HttpRequest::Method::Post);
}

RemoveTerminalsRequest::~RemoveTerminalsRequest()
{}

std::vector<std::string> RemoveTerminalsRequest::getTerminalIds()const
{
	return terminalIds_;
}

void RemoveTerminalsRequest::setTerminalIds(const std::vector<std::string>& terminalIds)
{
	terminalIds_ = terminalIds;
	for(int dep1 = 0; dep1!= terminalIds.size(); dep1++) {
		setParameter("TerminalIds."+ std::to_string(dep1), terminalIds.at(dep1));
	}
}

std::string RemoveTerminalsRequest::getShowLog()const
{
	return showLog_;
}

void RemoveTerminalsRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long RemoveTerminalsRequest::getOwnerId()const
{
	return ownerId_;
}

void RemoveTerminalsRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string RemoveTerminalsRequest::getAppId()const
{
	return appId_;
}

void RemoveTerminalsRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string RemoveTerminalsRequest::getChannelId()const
{
	return channelId_;
}

void RemoveTerminalsRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

