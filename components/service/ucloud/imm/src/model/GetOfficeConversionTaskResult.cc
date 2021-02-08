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

#include <alibabacloud/imm/model/GetOfficeConversionTaskResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

GetOfficeConversionTaskResult::GetOfficeConversionTaskResult() :
	ServiceResult()
{}

GetOfficeConversionTaskResult::GetOfficeConversionTaskResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetOfficeConversionTaskResult::~GetOfficeConversionTaskResult()
{}

void GetOfficeConversionTaskResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto failDetailNode = value["FailDetail"];
	if(!failDetailNode["Code"].isNull())
		failDetail_.code = failDetailNode["Code"].asString();
	if(!value["TgtType"].isNull())
		tgtType_ = value["TgtType"].asString();
	if(!value["Status"].isNull())
		status_ = value["Status"].asString();
	if(!value["Percent"].isNull())
		percent_ = std::stoi(value["Percent"].asString());
	if(!value["PageCount"].isNull())
		pageCount_ = std::stoi(value["PageCount"].asString());
	if(!value["TaskId"].isNull())
		taskId_ = value["TaskId"].asString();
	if(!value["TgtUri"].isNull())
		tgtUri_ = value["TgtUri"].asString();
	if(!value["ImageSpec"].isNull())
		imageSpec_ = value["ImageSpec"].asString();
	if(!value["NotifyTopicName"].isNull())
		notifyTopicName_ = value["NotifyTopicName"].asString();
	if(!value["NotifyEndpoint"].isNull())
		notifyEndpoint_ = value["NotifyEndpoint"].asString();
	if(!value["ExternalID"].isNull())
		externalID_ = value["ExternalID"].asString();
	if(!value["CreateTime"].isNull())
		createTime_ = value["CreateTime"].asString();
	if(!value["FinishTime"].isNull())
		finishTime_ = value["FinishTime"].asString();
	if(!value["SrcUri"].isNull())
		srcUri_ = value["SrcUri"].asString();

}

std::string GetOfficeConversionTaskResult::getStatus()const
{
	return status_;
}

GetOfficeConversionTaskResult::FailDetail GetOfficeConversionTaskResult::getFailDetail()const
{
	return failDetail_;
}

std::string GetOfficeConversionTaskResult::getTgtType()const
{
	return tgtType_;
}

std::string GetOfficeConversionTaskResult::getTaskId()const
{
	return taskId_;
}

int GetOfficeConversionTaskResult::getPercent()const
{
	return percent_;
}

std::string GetOfficeConversionTaskResult::getExternalID()const
{
	return externalID_;
}

std::string GetOfficeConversionTaskResult::getCreateTime()const
{
	return createTime_;
}

std::string GetOfficeConversionTaskResult::getImageSpec()const
{
	return imageSpec_;
}

std::string GetOfficeConversionTaskResult::getNotifyEndpoint()const
{
	return notifyEndpoint_;
}

std::string GetOfficeConversionTaskResult::getTgtUri()const
{
	return tgtUri_;
}

int GetOfficeConversionTaskResult::getPageCount()const
{
	return pageCount_;
}

std::string GetOfficeConversionTaskResult::getFinishTime()const
{
	return finishTime_;
}

std::string GetOfficeConversionTaskResult::getNotifyTopicName()const
{
	return notifyTopicName_;
}

std::string GetOfficeConversionTaskResult::getSrcUri()const
{
	return srcUri_;
}

