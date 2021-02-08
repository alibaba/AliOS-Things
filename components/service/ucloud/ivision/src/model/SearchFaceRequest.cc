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

#include <alibabacloud/ivision/model/SearchFaceRequest.h>

using AlibabaCloud::Ivision::Model::SearchFaceRequest;

SearchFaceRequest::SearchFaceRequest() :
	RpcServiceRequest("ivision", "2019-03-08", "SearchFace")
{
	setMethod(HttpRequest::Method::Get);
}

SearchFaceRequest::~SearchFaceRequest()
{}

std::string SearchFaceRequest::getContent()const
{
	return content_;
}

void SearchFaceRequest::setContent(const std::string& content)
{
	content_ = content;
	setParameter("Content", content);
}

std::string SearchFaceRequest::getDataType()const
{
	return dataType_;
}

void SearchFaceRequest::setDataType(const std::string& dataType)
{
	dataType_ = dataType;
	setParameter("DataType", dataType);
}

float SearchFaceRequest::getProbabilityThreshold()const
{
	return probabilityThreshold_;
}

void SearchFaceRequest::setProbabilityThreshold(float probabilityThreshold)
{
	probabilityThreshold_ = probabilityThreshold;
	setParameter("ProbabilityThreshold", std::to_string(probabilityThreshold));
}

std::string SearchFaceRequest::getShowLog()const
{
	return showLog_;
}

void SearchFaceRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string SearchFaceRequest::getGroupId()const
{
	return groupId_;
}

void SearchFaceRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

int SearchFaceRequest::getCount()const
{
	return count_;
}

void SearchFaceRequest::setCount(int count)
{
	count_ = count;
	setParameter("Count", std::to_string(count));
}

long SearchFaceRequest::getOwnerId()const
{
	return ownerId_;
}

void SearchFaceRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

