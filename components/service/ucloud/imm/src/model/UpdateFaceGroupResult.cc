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

#include <alibabacloud/imm/model/UpdateFaceGroupResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

UpdateFaceGroupResult::UpdateFaceGroupResult() :
	ServiceResult()
{}

UpdateFaceGroupResult::UpdateFaceGroupResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

UpdateFaceGroupResult::~UpdateFaceGroupResult()
{}

void UpdateFaceGroupResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["SetId"].isNull())
		setId_ = value["SetId"].asString();
	if(!value["GroupId"].isNull())
		groupId_ = value["GroupId"].asString();

}

std::string UpdateFaceGroupResult::getSetId()const
{
	return setId_;
}

std::string UpdateFaceGroupResult::getGroupId()const
{
	return groupId_;
}

