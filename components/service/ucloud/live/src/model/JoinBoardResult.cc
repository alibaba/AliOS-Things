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

#include <alibabacloud/live/model/JoinBoardResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

JoinBoardResult::JoinBoardResult() :
	ServiceResult()
{}

JoinBoardResult::JoinBoardResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

JoinBoardResult::~JoinBoardResult()
{}

void JoinBoardResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["Token"].isNull())
		token_ = value["Token"].asString();
	if(!value["BoardId"].isNull())
		boardId_ = value["BoardId"].asString();
	if(!value["TopicId"].isNull())
		topicId_ = value["TopicId"].asString();
	if(!value["KeepaliveTopic"].isNull())
		keepaliveTopic_ = value["KeepaliveTopic"].asString();
	if(!value["KeepaliveInterval"].isNull())
		keepaliveInterval_ = std::stoi(value["KeepaliveInterval"].asString());

}

std::string JoinBoardResult::getBoardId()const
{
	return boardId_;
}

std::string JoinBoardResult::getToken()const
{
	return token_;
}

std::string JoinBoardResult::getTopicId()const
{
	return topicId_;
}

std::string JoinBoardResult::getKeepaliveTopic()const
{
	return keepaliveTopic_;
}

int JoinBoardResult::getKeepaliveInterval()const
{
	return keepaliveInterval_;
}

