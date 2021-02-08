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

#include <alibabacloud/cloudphoto/model/GetVideoCoverResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

GetVideoCoverResult::GetVideoCoverResult() :
	ServiceResult()
{}

GetVideoCoverResult::GetVideoCoverResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetVideoCoverResult::~GetVideoCoverResult()
{}

void GetVideoCoverResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["VideoCoverUrl"].isNull())
		videoCoverUrl_ = value["VideoCoverUrl"].asString();
	if(!value["Action"].isNull())
		action_ = value["Action"].asString();

}

std::string GetVideoCoverResult::getAction()const
{
	return action_;
}

std::string GetVideoCoverResult::getMessage()const
{
	return message_;
}

std::string GetVideoCoverResult::getVideoCoverUrl()const
{
	return videoCoverUrl_;
}

std::string GetVideoCoverResult::getCode()const
{
	return code_;
}

