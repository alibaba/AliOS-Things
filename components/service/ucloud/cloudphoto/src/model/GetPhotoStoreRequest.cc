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

#include <alibabacloud/cloudphoto/model/GetPhotoStoreRequest.h>

using AlibabaCloud::CloudPhoto::Model::GetPhotoStoreRequest;

GetPhotoStoreRequest::GetPhotoStoreRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "GetPhotoStore")
{
	setMethod(HttpRequest::Method::Post);
}

GetPhotoStoreRequest::~GetPhotoStoreRequest()
{}

std::string GetPhotoStoreRequest::getStoreName()const
{
	return storeName_;
}

void GetPhotoStoreRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

