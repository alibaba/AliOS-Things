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

#include <alibabacloud/rtc/model/CreateMPULayoutRequest.h>

using AlibabaCloud::Rtc::Model::CreateMPULayoutRequest;

CreateMPULayoutRequest::CreateMPULayoutRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "CreateMPULayout")
{
	setMethod(HttpRequest::Method::Post);
}

CreateMPULayoutRequest::~CreateMPULayoutRequest()
{}

std::vector<CreateMPULayoutRequest::Panes> CreateMPULayoutRequest::getPanes()const
{
	return panes_;
}

void CreateMPULayoutRequest::setPanes(const std::vector<Panes>& panes)
{
	panes_ = panes;
	for(int dep1 = 0; dep1!= panes.size(); dep1++) {
		auto panesObj = panes.at(dep1);
		std::string panesObjStr = "Panes." + std::to_string(dep1 + 1);
		setParameter(panesObjStr + ".PaneId", std::to_string(panesObj.paneId));
		setParameter(panesObjStr + ".MajorPane", std::to_string(panesObj.majorPane));
		setParameter(panesObjStr + ".X", std::to_string(panesObj.x));
		setParameter(panesObjStr + ".Y", std::to_string(panesObj.y));
		setParameter(panesObjStr + ".Width", std::to_string(panesObj.width));
		setParameter(panesObjStr + ".Height", std::to_string(panesObj.height));
		setParameter(panesObjStr + ".ZOrder", std::to_string(panesObj.zOrder));
	}
}

std::string CreateMPULayoutRequest::getShowLog()const
{
	return showLog_;
}

void CreateMPULayoutRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long CreateMPULayoutRequest::getOwnerId()const
{
	return ownerId_;
}

void CreateMPULayoutRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string CreateMPULayoutRequest::getAppId()const
{
	return appId_;
}

void CreateMPULayoutRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string CreateMPULayoutRequest::getName()const
{
	return name_;
}

void CreateMPULayoutRequest::setName(const std::string& name)
{
	name_ = name;
	setParameter("Name", name);
}

int CreateMPULayoutRequest::getAudioMixCount()const
{
	return audioMixCount_;
}

void CreateMPULayoutRequest::setAudioMixCount(int audioMixCount)
{
	audioMixCount_ = audioMixCount;
	setParameter("AudioMixCount", std::to_string(audioMixCount));
}

