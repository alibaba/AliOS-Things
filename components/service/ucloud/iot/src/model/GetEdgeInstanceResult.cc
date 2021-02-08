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

#include <alibabacloud/iot/model/GetEdgeInstanceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

GetEdgeInstanceResult::GetEdgeInstanceResult() :
	ServiceResult()
{}

GetEdgeInstanceResult::GetEdgeInstanceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetEdgeInstanceResult::~GetEdgeInstanceResult()
{}

void GetEdgeInstanceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["InstanceId"].isNull())
		data_.instanceId = dataNode["InstanceId"].asString();
	if(!dataNode["Name"].isNull())
		data_.name = dataNode["Name"].asString();
	if(!dataNode["Tags"].isNull())
		data_.tags = dataNode["Tags"].asString();
	if(!dataNode["Type"].isNull())
		data_.type = dataNode["Type"].asString();
	if(!dataNode["Spec"].isNull())
		data_.spec = std::stoi(dataNode["Spec"].asString());
	if(!dataNode["BizEnable"].isNull())
		data_.bizEnable = dataNode["BizEnable"].asString() == "true";
	if(!dataNode["GmtCreate"].isNull())
		data_.gmtCreate = dataNode["GmtCreate"].asString();
	if(!dataNode["GmtModified"].isNull())
		data_.gmtModified = dataNode["GmtModified"].asString();
	if(!dataNode["RoleArn"].isNull())
		data_.roleArn = dataNode["RoleArn"].asString();
	if(!dataNode["RoleName"].isNull())
		data_.roleName = dataNode["RoleName"].asString();
	if(!dataNode["RoleAttachTime"].isNull())
		data_.roleAttachTime = dataNode["RoleAttachTime"].asString();
	if(!dataNode["LatestDeploymentStatus"].isNull())
		data_.latestDeploymentStatus = std::stoi(dataNode["LatestDeploymentStatus"].asString());
	if(!dataNode["LatestDeploymentType"].isNull())
		data_.latestDeploymentType = dataNode["LatestDeploymentType"].asString();
	if(!dataNode["GmtCreateTimestamp"].isNull())
		data_.gmtCreateTimestamp = std::stol(dataNode["GmtCreateTimestamp"].asString());
	if(!dataNode["GmtModifiedTimestamp"].isNull())
		data_.gmtModifiedTimestamp = std::stol(dataNode["GmtModifiedTimestamp"].asString());
	if(!dataNode["RoleAttachTimestamp"].isNull())
		data_.roleAttachTimestamp = std::stol(dataNode["RoleAttachTimestamp"].asString());
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

GetEdgeInstanceResult::Data GetEdgeInstanceResult::getData()const
{
	return data_;
}

std::string GetEdgeInstanceResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string GetEdgeInstanceResult::getCode()const
{
	return code_;
}

bool GetEdgeInstanceResult::getSuccess()const
{
	return success_;
}

