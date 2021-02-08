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

#include <alibabacloud/live/model/SetCasterConfigRequest.h>

using AlibabaCloud::Live::Model::SetCasterConfigRequest;

SetCasterConfigRequest::SetCasterConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "SetCasterConfig")
{
	setMethod(HttpRequest::Method::Post);
}

SetCasterConfigRequest::~SetCasterConfigRequest()
{}

int SetCasterConfigRequest::getChannelEnable()const
{
	return channelEnable_;
}

void SetCasterConfigRequest::setChannelEnable(int channelEnable)
{
	channelEnable_ = channelEnable;
	setParameter("ChannelEnable", std::to_string(channelEnable));
}

int SetCasterConfigRequest::getProgramEffect()const
{
	return programEffect_;
}

void SetCasterConfigRequest::setProgramEffect(int programEffect)
{
	programEffect_ = programEffect;
	setParameter("ProgramEffect", std::to_string(programEffect));
}

std::string SetCasterConfigRequest::getProgramName()const
{
	return programName_;
}

void SetCasterConfigRequest::setProgramName(const std::string& programName)
{
	programName_ = programName;
	setParameter("ProgramName", programName);
}

std::string SetCasterConfigRequest::getRecordConfig()const
{
	return recordConfig_;
}

void SetCasterConfigRequest::setRecordConfig(const std::string& recordConfig)
{
	recordConfig_ = recordConfig;
	setParameter("RecordConfig", recordConfig);
}

std::string SetCasterConfigRequest::getUrgentMaterialId()const
{
	return urgentMaterialId_;
}

void SetCasterConfigRequest::setUrgentMaterialId(const std::string& urgentMaterialId)
{
	urgentMaterialId_ = urgentMaterialId;
	setParameter("UrgentMaterialId", urgentMaterialId);
}

std::string SetCasterConfigRequest::getTranscodeConfig()const
{
	return transcodeConfig_;
}

void SetCasterConfigRequest::setTranscodeConfig(const std::string& transcodeConfig)
{
	transcodeConfig_ = transcodeConfig;
	setParameter("TranscodeConfig", transcodeConfig);
}

std::string SetCasterConfigRequest::getCasterName()const
{
	return casterName_;
}

void SetCasterConfigRequest::setCasterName(const std::string& casterName)
{
	casterName_ = casterName;
	setParameter("CasterName", casterName);
}

std::string SetCasterConfigRequest::getSideOutputUrl()const
{
	return sideOutputUrl_;
}

void SetCasterConfigRequest::setSideOutputUrl(const std::string& sideOutputUrl)
{
	sideOutputUrl_ = sideOutputUrl;
	setParameter("SideOutputUrl", sideOutputUrl);
}

std::string SetCasterConfigRequest::getCasterId()const
{
	return casterId_;
}

void SetCasterConfigRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

std::string SetCasterConfigRequest::getDomainName()const
{
	return domainName_;
}

void SetCasterConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long SetCasterConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void SetCasterConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

float SetCasterConfigRequest::getDelay()const
{
	return delay_;
}

void SetCasterConfigRequest::setDelay(float delay)
{
	delay_ = delay;
	setParameter("Delay", std::to_string(delay));
}

std::string SetCasterConfigRequest::getCallbackUrl()const
{
	return callbackUrl_;
}

void SetCasterConfigRequest::setCallbackUrl(const std::string& callbackUrl)
{
	callbackUrl_ = callbackUrl;
	setParameter("CallbackUrl", callbackUrl);
}

