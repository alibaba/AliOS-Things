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

#include <alibabacloud/live/model/AddCasterVideoResourceRequest.h>

using AlibabaCloud::Live::Model::AddCasterVideoResourceRequest;

AddCasterVideoResourceRequest::AddCasterVideoResourceRequest() :
	RpcServiceRequest("live", "2016-11-01", "AddCasterVideoResource")
{
	setMethod(HttpRequest::Method::Post);
}

AddCasterVideoResourceRequest::~AddCasterVideoResourceRequest()
{}

int AddCasterVideoResourceRequest::getEndOffset()const
{
	return endOffset_;
}

void AddCasterVideoResourceRequest::setEndOffset(int endOffset)
{
	endOffset_ = endOffset;
	setParameter("EndOffset", std::to_string(endOffset));
}

std::string AddCasterVideoResourceRequest::getMaterialId()const
{
	return materialId_;
}

void AddCasterVideoResourceRequest::setMaterialId(const std::string& materialId)
{
	materialId_ = materialId;
	setParameter("MaterialId", materialId);
}

std::string AddCasterVideoResourceRequest::getVodUrl()const
{
	return vodUrl_;
}

void AddCasterVideoResourceRequest::setVodUrl(const std::string& vodUrl)
{
	vodUrl_ = vodUrl;
	setParameter("VodUrl", vodUrl);
}

std::string AddCasterVideoResourceRequest::getStreamId()const
{
	return streamId_;
}

void AddCasterVideoResourceRequest::setStreamId(const std::string& streamId)
{
	streamId_ = streamId;
	setParameter("StreamId", streamId);
}

std::string AddCasterVideoResourceRequest::getCasterId()const
{
	return casterId_;
}

void AddCasterVideoResourceRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

long AddCasterVideoResourceRequest::getOwnerId()const
{
	return ownerId_;
}

void AddCasterVideoResourceRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

int AddCasterVideoResourceRequest::getBeginOffset()const
{
	return beginOffset_;
}

void AddCasterVideoResourceRequest::setBeginOffset(int beginOffset)
{
	beginOffset_ = beginOffset;
	setParameter("BeginOffset", std::to_string(beginOffset));
}

std::string AddCasterVideoResourceRequest::getLiveStreamUrl()const
{
	return liveStreamUrl_;
}

void AddCasterVideoResourceRequest::setLiveStreamUrl(const std::string& liveStreamUrl)
{
	liveStreamUrl_ = liveStreamUrl;
	setParameter("LiveStreamUrl", liveStreamUrl);
}

std::string AddCasterVideoResourceRequest::getLocationId()const
{
	return locationId_;
}

void AddCasterVideoResourceRequest::setLocationId(const std::string& locationId)
{
	locationId_ = locationId;
	setParameter("LocationId", locationId);
}

int AddCasterVideoResourceRequest::getPtsCallbackInterval()const
{
	return ptsCallbackInterval_;
}

void AddCasterVideoResourceRequest::setPtsCallbackInterval(int ptsCallbackInterval)
{
	ptsCallbackInterval_ = ptsCallbackInterval;
	setParameter("PtsCallbackInterval", std::to_string(ptsCallbackInterval));
}

std::string AddCasterVideoResourceRequest::getResourceName()const
{
	return resourceName_;
}

void AddCasterVideoResourceRequest::setResourceName(const std::string& resourceName)
{
	resourceName_ = resourceName;
	setParameter("ResourceName", resourceName);
}

int AddCasterVideoResourceRequest::getRepeatNum()const
{
	return repeatNum_;
}

void AddCasterVideoResourceRequest::setRepeatNum(int repeatNum)
{
	repeatNum_ = repeatNum;
	setParameter("RepeatNum", std::to_string(repeatNum));
}

