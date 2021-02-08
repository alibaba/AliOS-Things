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

#include <alibabacloud/imm/model/CreateMergeFaceGroupsJobResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

CreateMergeFaceGroupsJobResult::CreateMergeFaceGroupsJobResult() :
	ServiceResult()
{}

CreateMergeFaceGroupsJobResult::CreateMergeFaceGroupsJobResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

CreateMergeFaceGroupsJobResult::~CreateMergeFaceGroupsJobResult()
{}

void CreateMergeFaceGroupsJobResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["JobId"].isNull())
		jobId_ = value["JobId"].asString();
	if(!value["SetId"].isNull())
		setId_ = value["SetId"].asString();
	if(!value["JobType"].isNull())
		jobType_ = value["JobType"].asString();
	if(!value["GroupIdTo"].isNull())
		groupIdTo_ = value["GroupIdTo"].asString();
	if(!value["GroupIdFrom"].isNull())
		groupIdFrom_ = value["GroupIdFrom"].asString();

}

std::string CreateMergeFaceGroupsJobResult::getGroupIdFrom()const
{
	return groupIdFrom_;
}

std::string CreateMergeFaceGroupsJobResult::getJobType()const
{
	return jobType_;
}

std::string CreateMergeFaceGroupsJobResult::getSetId()const
{
	return setId_;
}

std::string CreateMergeFaceGroupsJobResult::getGroupIdTo()const
{
	return groupIdTo_;
}

std::string CreateMergeFaceGroupsJobResult::getJobId()const
{
	return jobId_;
}

