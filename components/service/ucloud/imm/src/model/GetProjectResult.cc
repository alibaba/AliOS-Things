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

#include <alibabacloud/imm/model/GetProjectResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

GetProjectResult::GetProjectResult() :
	ServiceResult()
{}

GetProjectResult::GetProjectResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetProjectResult::~GetProjectResult()
{}

void GetProjectResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["Project"].isNull())
		project_ = value["Project"].asString();
	if(!value["ServiceRole"].isNull())
		serviceRole_ = value["ServiceRole"].asString();
	if(!value["Endpoint"].isNull())
		endpoint_ = value["Endpoint"].asString();
	if(!value["CreateTime"].isNull())
		createTime_ = value["CreateTime"].asString();
	if(!value["ModifyTime"].isNull())
		modifyTime_ = value["ModifyTime"].asString();
	if(!value["Type"].isNull())
		type_ = value["Type"].asString();
	if(!value["CU"].isNull())
		cU_ = std::stoi(value["CU"].asString());
	if(!value["BillingType"].isNull())
		billingType_ = value["BillingType"].asString();
	if(!value["RegionId"].isNull())
		regionId_ = value["RegionId"].asString();

}

std::string GetProjectResult::getProject()const
{
	return project_;
}

std::string GetProjectResult::getModifyTime()const
{
	return modifyTime_;
}

std::string GetProjectResult::getType()const
{
	return type_;
}

int GetProjectResult::getCU()const
{
	return cU_;
}

std::string GetProjectResult::getServiceRole()const
{
	return serviceRole_;
}

std::string GetProjectResult::getEndpoint()const
{
	return endpoint_;
}

std::string GetProjectResult::getCreateTime()const
{
	return createTime_;
}

std::string GetProjectResult::getRegionId()const
{
	return regionId_;
}

std::string GetProjectResult::getBillingType()const
{
	return billingType_;
}

