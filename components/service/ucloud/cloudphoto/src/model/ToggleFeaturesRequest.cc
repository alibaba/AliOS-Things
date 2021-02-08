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

#include <alibabacloud/cloudphoto/model/ToggleFeaturesRequest.h>

using AlibabaCloud::CloudPhoto::Model::ToggleFeaturesRequest;

ToggleFeaturesRequest::ToggleFeaturesRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "ToggleFeatures")
{
	setMethod(HttpRequest::Method::Post);
}

ToggleFeaturesRequest::~ToggleFeaturesRequest()
{}

std::vector<std::string> ToggleFeaturesRequest::getDisabledFeatures()const
{
	return disabledFeatures_;
}

void ToggleFeaturesRequest::setDisabledFeatures(const std::vector<std::string>& disabledFeatures)
{
	disabledFeatures_ = disabledFeatures;
	for(int dep1 = 0; dep1!= disabledFeatures.size(); dep1++) {
		setParameter("DisabledFeatures."+ std::to_string(dep1), disabledFeatures.at(dep1));
	}
}

std::string ToggleFeaturesRequest::getStoreName()const
{
	return storeName_;
}

void ToggleFeaturesRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

std::vector<std::string> ToggleFeaturesRequest::getEnabledFeatures()const
{
	return enabledFeatures_;
}

void ToggleFeaturesRequest::setEnabledFeatures(const std::vector<std::string>& enabledFeatures)
{
	enabledFeatures_ = enabledFeatures;
	for(int dep1 = 0; dep1!= enabledFeatures.size(); dep1++) {
		setParameter("EnabledFeatures."+ std::to_string(dep1), enabledFeatures.at(dep1));
	}
}

