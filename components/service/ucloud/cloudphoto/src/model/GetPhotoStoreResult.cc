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

#include <alibabacloud/cloudphoto/model/GetPhotoStoreResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

GetPhotoStoreResult::GetPhotoStoreResult() :
	ServiceResult()
{}

GetPhotoStoreResult::GetPhotoStoreResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetPhotoStoreResult::~GetPhotoStoreResult()
{}

void GetPhotoStoreResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto photoStoreNode = value["PhotoStore"];
	if(!photoStoreNode["Id"].isNull())
		photoStore_.id = std::stol(photoStoreNode["Id"].asString());
	if(!photoStoreNode["IdStr"].isNull())
		photoStore_.idStr = photoStoreNode["IdStr"].asString();
	if(!photoStoreNode["Name"].isNull())
		photoStore_.name = photoStoreNode["Name"].asString();
	if(!photoStoreNode["Remark"].isNull())
		photoStore_.remark = photoStoreNode["Remark"].asString();
	if(!photoStoreNode["AutoCleanEnabled"].isNull())
		photoStore_.autoCleanEnabled = photoStoreNode["AutoCleanEnabled"].asString() == "true";
	if(!photoStoreNode["AutoCleanDays"].isNull())
		photoStore_.autoCleanDays = std::stoi(photoStoreNode["AutoCleanDays"].asString());
	if(!photoStoreNode["DefaultQuota"].isNull())
		photoStore_.defaultQuota = std::stol(photoStoreNode["DefaultQuota"].asString());
	if(!photoStoreNode["DefaultTrashQuota"].isNull())
		photoStore_.defaultTrashQuota = std::stol(photoStoreNode["DefaultTrashQuota"].asString());
	if(!photoStoreNode["Ctime"].isNull())
		photoStore_.ctime = std::stol(photoStoreNode["Ctime"].asString());
	if(!photoStoreNode["Mtime"].isNull())
		photoStore_.mtime = std::stol(photoStoreNode["Mtime"].asString());
	auto allBucketsNode = photoStoreNode["Buckets"]["Bucket"];
	for (auto photoStoreNodeBucketsBucket : allBucketsNode)
	{
		PhotoStore::Bucket bucketObject;
		if(!photoStoreNodeBucketsBucket["Name"].isNull())
			bucketObject.name = photoStoreNodeBucketsBucket["Name"].asString();
		if(!photoStoreNodeBucketsBucket["Region"].isNull())
			bucketObject.region = photoStoreNodeBucketsBucket["Region"].asString();
		if(!photoStoreNodeBucketsBucket["State"].isNull())
			bucketObject.state = photoStoreNodeBucketsBucket["State"].asString();
		if(!photoStoreNodeBucketsBucket["Acl"].isNull())
			bucketObject.acl = photoStoreNodeBucketsBucket["Acl"].asString();
		photoStore_.buckets.push_back(bucketObject);
	}
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Action"].isNull())
		action_ = value["Action"].asString();

}

std::string GetPhotoStoreResult::getAction()const
{
	return action_;
}

std::string GetPhotoStoreResult::getMessage()const
{
	return message_;
}

GetPhotoStoreResult::PhotoStore GetPhotoStoreResult::getPhotoStore()const
{
	return photoStore_;
}

std::string GetPhotoStoreResult::getCode()const
{
	return code_;
}

