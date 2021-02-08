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

#include <alibabacloud/rtc/model/ModifyMPULayoutRequest.h>

using AlibabaCloud::Rtc::Model::ModifyMPULayoutRequest;

ModifyMPULayoutRequest::ModifyMPULayoutRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "ModifyMPULayout")
{
	setMethod(HttpRequest::Method::Post);
}

ModifyMPULayoutRequest::~ModifyMPULayoutRequest()
{}

long ModifyMPULayoutRequest::getLayoutId()const
{
	return layoutId_;
}

void ModifyMPULayoutRequest::setLayoutId(long layoutId)
{
	layoutId_ = layoutId;
	setParameter("LayoutId", std::to_string(layoutId));
}

std::vector<ModifyMPULayoutRequest::Panes> ModifyMPULayoutRequest::getPanes()const
{
	return panes_;
}

void ModifyMPULayoutRequest::setPanes(const std::vector<Panes>& panes)
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

std::string ModifyMPULayoutRequest::getShowLog()const
{
	return showLog_;
}

void ModifyMPULayoutRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long ModifyMPULayoutRequest::getOwnerId()const
{
	return ownerId_;
}

void ModifyMPULayoutRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string ModifyMPULayoutRequest::getAppId()const
{
	return appId_;
}

void ModifyMPULayoutRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string ModifyMPULayoutRequest::getName()const
{
	return name_;
}

void ModifyMPULayoutRequest::setName(const std::string& name)
{
	name_ = name;
	setParameter("Name", name);
}

int ModifyMPULayoutRequest::getAudioMixCount()const
{
	return audioMixCount_;
}

void ModifyMPULayoutRequest::setAudioMixCount(int audioMixCount)
{
	audioMixCount_ = audioMixCount;
	setParameter("AudioMixCount", std::to_string(audioMixCount));
}

