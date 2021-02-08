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

#include <alibabacloud/linkface/model/QueryAllGroupsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::LinkFace;
using namespace AlibabaCloud::LinkFace::Model;

QueryAllGroupsResult::QueryAllGroupsResult() :
	ServiceResult()
{}

QueryAllGroupsResult::QueryAllGroupsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryAllGroupsResult::~QueryAllGroupsResult()
{}

void QueryAllGroupsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
		auto allGroups = dataNode["Groups"]["Groups"];
		for (auto value : allGroups)
			data_.groups.push_back(value.asString());
	if(!value["Code"].isNull())
		code_ = std::stoi(value["Code"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["PageCount"].isNull())
		pageCount_ = std::stoi(value["PageCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["Page"].isNull())
		page_ = std::stoi(value["Page"].asString());
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string QueryAllGroupsResult::getMessage()const
{
	return message_;
}

int QueryAllGroupsResult::getPageCount()const
{
	return pageCount_;
}

int QueryAllGroupsResult::getPageSize()const
{
	return pageSize_;
}

int QueryAllGroupsResult::getTotal()const
{
	return total_;
}

QueryAllGroupsResult::Data QueryAllGroupsResult::getData()const
{
	return data_;
}

int QueryAllGroupsResult::getPage()const
{
	return page_;
}

int QueryAllGroupsResult::getCode()const
{
	return code_;
}

bool QueryAllGroupsResult::getSuccess()const
{
	return success_;
}

