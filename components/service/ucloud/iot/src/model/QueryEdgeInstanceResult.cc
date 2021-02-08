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

#include <alibabacloud/iot/model/QueryEdgeInstanceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryEdgeInstanceResult::QueryEdgeInstanceResult() :
	ServiceResult()
{}

QueryEdgeInstanceResult::QueryEdgeInstanceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryEdgeInstanceResult::~QueryEdgeInstanceResult()
{}

void QueryEdgeInstanceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Total"].isNull())
		data_.total = std::stoi(dataNode["Total"].asString());
	if(!dataNode["PageSize"].isNull())
		data_.pageSize = std::stoi(dataNode["PageSize"].asString());
	if(!dataNode["CurrentPage"].isNull())
		data_.currentPage = std::stoi(dataNode["CurrentPage"].asString());
	auto allInstanceListNode = dataNode["InstanceList"]["Instance"];
	for (auto dataNodeInstanceListInstance : allInstanceListNode)
	{
		Data::Instance instanceObject;
		if(!dataNodeInstanceListInstance["InstanceId"].isNull())
			instanceObject.instanceId = dataNodeInstanceListInstance["InstanceId"].asString();
		if(!dataNodeInstanceListInstance["Name"].isNull())
			instanceObject.name = dataNodeInstanceListInstance["Name"].asString();
		if(!dataNodeInstanceListInstance["Tags"].isNull())
			instanceObject.tags = dataNodeInstanceListInstance["Tags"].asString();
		if(!dataNodeInstanceListInstance["Type"].isNull())
			instanceObject.type = std::stoi(dataNodeInstanceListInstance["Type"].asString());
		if(!dataNodeInstanceListInstance["GmtCreate"].isNull())
			instanceObject.gmtCreate = dataNodeInstanceListInstance["GmtCreate"].asString();
		if(!dataNodeInstanceListInstance["GmtModified"].isNull())
			instanceObject.gmtModified = dataNodeInstanceListInstance["GmtModified"].asString();
		if(!dataNodeInstanceListInstance["RoleArn"].isNull())
			instanceObject.roleArn = dataNodeInstanceListInstance["RoleArn"].asString();
		if(!dataNodeInstanceListInstance["RoleName"].isNull())
			instanceObject.roleName = dataNodeInstanceListInstance["RoleName"].asString();
		if(!dataNodeInstanceListInstance["RoleAttachTime"].isNull())
			instanceObject.roleAttachTime = dataNodeInstanceListInstance["RoleAttachTime"].asString();
		if(!dataNodeInstanceListInstance["Spec"].isNull())
			instanceObject.spec = std::stoi(dataNodeInstanceListInstance["Spec"].asString());
		if(!dataNodeInstanceListInstance["BizEnable"].isNull())
			instanceObject.bizEnable = dataNodeInstanceListInstance["BizEnable"].asString() == "true";
		if(!dataNodeInstanceListInstance["LatestDeploymentStatus"].isNull())
			instanceObject.latestDeploymentStatus = std::stoi(dataNodeInstanceListInstance["LatestDeploymentStatus"].asString());
		if(!dataNodeInstanceListInstance["LatestDeploymentType"].isNull())
			instanceObject.latestDeploymentType = dataNodeInstanceListInstance["LatestDeploymentType"].asString();
		if(!dataNodeInstanceListInstance["GmtCreateTimestamp"].isNull())
			instanceObject.gmtCreateTimestamp = std::stol(dataNodeInstanceListInstance["GmtCreateTimestamp"].asString());
		if(!dataNodeInstanceListInstance["GmtModifiedTimestamp"].isNull())
			instanceObject.gmtModifiedTimestamp = std::stol(dataNodeInstanceListInstance["GmtModifiedTimestamp"].asString());
		if(!dataNodeInstanceListInstance["RoleAttachTimestamp"].isNull())
			instanceObject.roleAttachTimestamp = std::stol(dataNodeInstanceListInstance["RoleAttachTimestamp"].asString());
		data_.instanceList.push_back(instanceObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

QueryEdgeInstanceResult::Data QueryEdgeInstanceResult::getData()const
{
	return data_;
}

std::string QueryEdgeInstanceResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryEdgeInstanceResult::getCode()const
{
	return code_;
}

bool QueryEdgeInstanceResult::getSuccess()const
{
	return success_;
}

