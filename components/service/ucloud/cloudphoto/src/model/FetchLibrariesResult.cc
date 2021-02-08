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

#include <alibabacloud/cloudphoto/model/FetchLibrariesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

FetchLibrariesResult::FetchLibrariesResult() :
	ServiceResult()
{}

FetchLibrariesResult::FetchLibrariesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

FetchLibrariesResult::~FetchLibrariesResult()
{}

void FetchLibrariesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allLibrariesNode = value["Libraries"]["Library"];
	for (auto valueLibrariesLibrary : allLibrariesNode)
	{
		Library librariesObject;
		if(!valueLibrariesLibrary["LibraryId"].isNull())
			librariesObject.libraryId = valueLibrariesLibrary["LibraryId"].asString();
		if(!valueLibrariesLibrary["Ctime"].isNull())
			librariesObject.ctime = std::stol(valueLibrariesLibrary["Ctime"].asString());
		if(!valueLibrariesLibrary["TotalQuota"].isNull())
			librariesObject.totalQuota = std::stol(valueLibrariesLibrary["TotalQuota"].asString());
		libraries_.push_back(librariesObject);
	}
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["Action"].isNull())
		action_ = value["Action"].asString();

}

int FetchLibrariesResult::getTotalCount()const
{
	return totalCount_;
}

std::string FetchLibrariesResult::getAction()const
{
	return action_;
}

std::string FetchLibrariesResult::getMessage()const
{
	return message_;
}

std::vector<FetchLibrariesResult::Library> FetchLibrariesResult::getLibraries()const
{
	return libraries_;
}

std::string FetchLibrariesResult::getCode()const
{
	return code_;
}

