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

#include <alibabacloud/cloudphoto/model/ListPhotoStoresResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

ListPhotoStoresResult::ListPhotoStoresResult() :
	ServiceResult()
{}

ListPhotoStoresResult::ListPhotoStoresResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListPhotoStoresResult::~ListPhotoStoresResult()
{}

void ListPhotoStoresResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allPhotoStoresNode = value["PhotoStores"]["PhotoStore"];
	for (auto valuePhotoStoresPhotoStore : allPhotoStoresNode)
	{
		PhotoStore photoStoresObject;
		if(!valuePhotoStoresPhotoStore["Id"].isNull())
			photoStoresObject.id = std::stol(valuePhotoStoresPhotoStore["Id"].asString());
		if(!valuePhotoStoresPhotoStore["IdStr"].isNull())
			photoStoresObject.idStr = valuePhotoStoresPhotoStore["IdStr"].asString();
		if(!valuePhotoStoresPhotoStore["Name"].isNull())
			photoStoresObject.name = valuePhotoStoresPhotoStore["Name"].asString();
		if(!valuePhotoStoresPhotoStore["Remark"].isNull())
			photoStoresObject.remark = valuePhotoStoresPhotoStore["Remark"].asString();
		if(!valuePhotoStoresPhotoStore["AutoCleanEnabled"].isNull())
			photoStoresObject.autoCleanEnabled = valuePhotoStoresPhotoStore["AutoCleanEnabled"].asString() == "true";
		if(!valuePhotoStoresPhotoStore["AutoCleanDays"].isNull())
			photoStoresObject.autoCleanDays = std::stoi(valuePhotoStoresPhotoStore["AutoCleanDays"].asString());
		if(!valuePhotoStoresPhotoStore["DefaultQuota"].isNull())
			photoStoresObject.defaultQuota = std::stol(valuePhotoStoresPhotoStore["DefaultQuota"].asString());
		if(!valuePhotoStoresPhotoStore["Ctime"].isNull())
			photoStoresObject.ctime = std::stol(valuePhotoStoresPhotoStore["Ctime"].asString());
		if(!valuePhotoStoresPhotoStore["Mtime"].isNull())
			photoStoresObject.mtime = std::stol(valuePhotoStoresPhotoStore["Mtime"].asString());
		auto allBucketsNode = allPhotoStoresNode["Buckets"]["Bucket"];
		for (auto allPhotoStoresNodeBucketsBucket : allBucketsNode)
		{
			PhotoStore::Bucket bucketsObject;
			if(!allPhotoStoresNodeBucketsBucket["Name"].isNull())
				bucketsObject.name = allPhotoStoresNodeBucketsBucket["Name"].asString();
			if(!allPhotoStoresNodeBucketsBucket["Region"].isNull())
				bucketsObject.region = allPhotoStoresNodeBucketsBucket["Region"].asString();
			if(!allPhotoStoresNodeBucketsBucket["State"].isNull())
				bucketsObject.state = allPhotoStoresNodeBucketsBucket["State"].asString();
			photoStoresObject.buckets.push_back(bucketsObject);
		}
		photoStores_.push_back(photoStoresObject);
	}
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Action"].isNull())
		action_ = value["Action"].asString();

}

std::string ListPhotoStoresResult::getAction()const
{
	return action_;
}

std::string ListPhotoStoresResult::getMessage()const
{
	return message_;
}

std::vector<ListPhotoStoresResult::PhotoStore> ListPhotoStoresResult::getPhotoStores()const
{
	return photoStores_;
}

std::string ListPhotoStoresResult::getCode()const
{
	return code_;
}

