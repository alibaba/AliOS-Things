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

#include <alibabacloud/cloudphoto/model/GetLibraryResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

GetLibraryResult::GetLibraryResult() :
	ServiceResult()
{}

GetLibraryResult::GetLibraryResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetLibraryResult::~GetLibraryResult()
{}

void GetLibraryResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto libraryNode = value["Library"];
	if(!libraryNode["Ctime"].isNull())
		library_.ctime = std::stol(libraryNode["Ctime"].asString());
	auto quotaNode = libraryNode["Quota"];
	if(!quotaNode["TotalQuota"].isNull())
		library_.quota.totalQuota = std::stol(quotaNode["TotalQuota"].asString());
	if(!quotaNode["TotalTrashQuota"].isNull())
		library_.quota.totalTrashQuota = std::stol(quotaNode["TotalTrashQuota"].asString());
	if(!quotaNode["FacesCount"].isNull())
		library_.quota.facesCount = std::stoi(quotaNode["FacesCount"].asString());
	if(!quotaNode["PhotosCount"].isNull())
		library_.quota.photosCount = std::stoi(quotaNode["PhotosCount"].asString());
	if(!quotaNode["UsedQuota"].isNull())
		library_.quota.usedQuota = std::stol(quotaNode["UsedQuota"].asString());
	if(!quotaNode["VideosCount"].isNull())
		library_.quota.videosCount = std::stoi(quotaNode["VideosCount"].asString());
	if(!quotaNode["ActiveSize"].isNull())
		library_.quota.activeSize = std::stol(quotaNode["ActiveSize"].asString());
	if(!quotaNode["InactiveSize"].isNull())
		library_.quota.inactiveSize = std::stol(quotaNode["InactiveSize"].asString());
	auto autoCleanConfigNode = libraryNode["AutoCleanConfig"];
	if(!autoCleanConfigNode["AutoCleanEnabled"].isNull())
		library_.autoCleanConfig.autoCleanEnabled = autoCleanConfigNode["AutoCleanEnabled"].asString() == "true";
	if(!autoCleanConfigNode["AutoCleanDays"].isNull())
		library_.autoCleanConfig.autoCleanDays = std::stoi(autoCleanConfigNode["AutoCleanDays"].asString());
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Action"].isNull())
		action_ = value["Action"].asString();

}

std::string GetLibraryResult::getAction()const
{
	return action_;
}

std::string GetLibraryResult::getMessage()const
{
	return message_;
}

GetLibraryResult::Library GetLibraryResult::getLibrary()const
{
	return library_;
}

std::string GetLibraryResult::getCode()const
{
	return code_;
}

