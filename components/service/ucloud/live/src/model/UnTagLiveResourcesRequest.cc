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

#include <alibabacloud/live/model/UnTagLiveResourcesRequest.h>

using AlibabaCloud::Live::Model::UnTagLiveResourcesRequest;

UnTagLiveResourcesRequest::UnTagLiveResourcesRequest() :
	RpcServiceRequest("live", "2016-11-01", "UnTagLiveResources")
{
	setMethod(HttpRequest::Method::Post);
}

UnTagLiveResourcesRequest::~UnTagLiveResourcesRequest()
{}

bool UnTagLiveResourcesRequest::getAll()const
{
	return all_;
}

void UnTagLiveResourcesRequest::setAll(bool all)
{
	all_ = all;
	setParameter("All", all ? "true" : "false");
}

std::vector<std::string> UnTagLiveResourcesRequest::getResourceId()const
{
	return resourceId_;
}

void UnTagLiveResourcesRequest::setResourceId(const std::vector<std::string>& resourceId)
{
	resourceId_ = resourceId;
	for(int dep1 = 0; dep1!= resourceId.size(); dep1++) {
		setParameter("ResourceId."+ std::to_string(dep1), resourceId.at(dep1));
	}
}

long UnTagLiveResourcesRequest::getOwnerId()const
{
	return ownerId_;
}

void UnTagLiveResourcesRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string UnTagLiveResourcesRequest::getResourceType()const
{
	return resourceType_;
}

void UnTagLiveResourcesRequest::setResourceType(const std::string& resourceType)
{
	resourceType_ = resourceType;
	setParameter("ResourceType", resourceType);
}

std::vector<std::string> UnTagLiveResourcesRequest::getTagKey()const
{
	return tagKey_;
}

void UnTagLiveResourcesRequest::setTagKey(const std::vector<std::string>& tagKey)
{
	tagKey_ = tagKey;
	for(int dep1 = 0; dep1!= tagKey.size(); dep1++) {
		setParameter("TagKey."+ std::to_string(dep1), tagKey.at(dep1));
	}
}

