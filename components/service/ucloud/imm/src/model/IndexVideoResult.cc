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

#include <alibabacloud/imm/model/IndexVideoResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

IndexVideoResult::IndexVideoResult() :
	ServiceResult()
{}

IndexVideoResult::IndexVideoResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

IndexVideoResult::~IndexVideoResult()
{}

void IndexVideoResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["SetId"].isNull())
		setId_ = value["SetId"].asString();
	if(!value["VideoUri"].isNull())
		videoUri_ = value["VideoUri"].asString();
	if(!value["RemarksA"].isNull())
		remarksA_ = value["RemarksA"].asString();
	if(!value["RemarksB"].isNull())
		remarksB_ = value["RemarksB"].asString();
	if(!value["CreateTime"].isNull())
		createTime_ = value["CreateTime"].asString();
	if(!value["ModifyTime"].isNull())
		modifyTime_ = value["ModifyTime"].asString();
	if(!value["Interval"].isNull())
		interval_ = std::stof(value["Interval"].asString());
	if(!value["GrabType"].isNull())
		grabType_ = value["GrabType"].asString();
	if(!value["StartTime"].isNull())
		startTime_ = value["StartTime"].asString();
	if(!value["EndTime"].isNull())
		endTime_ = value["EndTime"].asString();
	if(!value["SaveType"].isNull())
		saveType_ = value["SaveType"].asString() == "true";
	if(!value["TgtUri"].isNull())
		tgtUri_ = value["TgtUri"].asString();
	if(!value["RemarksC"].isNull())
		remarksC_ = value["RemarksC"].asString();
	if(!value["RemarksD"].isNull())
		remarksD_ = value["RemarksD"].asString();
	if(!value["ExternalId"].isNull())
		externalId_ = value["ExternalId"].asString();

}

std::string IndexVideoResult::getModifyTime()const
{
	return modifyTime_;
}

std::string IndexVideoResult::getEndTime()const
{
	return endTime_;
}

bool IndexVideoResult::getSaveType()const
{
	return saveType_;
}

std::string IndexVideoResult::getCreateTime()const
{
	return createTime_;
}

std::string IndexVideoResult::getExternalId()const
{
	return externalId_;
}

std::string IndexVideoResult::getStartTime()const
{
	return startTime_;
}

std::string IndexVideoResult::getVideoUri()const
{
	return videoUri_;
}

std::string IndexVideoResult::getGrabType()const
{
	return grabType_;
}

std::string IndexVideoResult::getRemarksA()const
{
	return remarksA_;
}

std::string IndexVideoResult::getRemarksB()const
{
	return remarksB_;
}

std::string IndexVideoResult::getRemarksC()const
{
	return remarksC_;
}

std::string IndexVideoResult::getTgtUri()const
{
	return tgtUri_;
}

std::string IndexVideoResult::getRemarksD()const
{
	return remarksD_;
}

std::string IndexVideoResult::getSetId()const
{
	return setId_;
}

float IndexVideoResult::getInterval()const
{
	return interval_;
}

