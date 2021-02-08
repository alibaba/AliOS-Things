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

#include <alibabacloud/imm/model/UpdateSetResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

UpdateSetResult::UpdateSetResult() :
	ServiceResult()
{}

UpdateSetResult::UpdateSetResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

UpdateSetResult::~UpdateSetResult()
{}

void UpdateSetResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["SetId"].isNull())
		setId_ = value["SetId"].asString();
	if(!value["SetName"].isNull())
		setName_ = value["SetName"].asString();
	if(!value["CreateTime"].isNull())
		createTime_ = value["CreateTime"].asString();
	if(!value["ModifyTime"].isNull())
		modifyTime_ = value["ModifyTime"].asString();

}

std::string UpdateSetResult::getModifyTime()const
{
	return modifyTime_;
}

std::string UpdateSetResult::getCreateTime()const
{
	return createTime_;
}

std::string UpdateSetResult::getSetName()const
{
	return setName_;
}

std::string UpdateSetResult::getSetId()const
{
	return setId_;
}

