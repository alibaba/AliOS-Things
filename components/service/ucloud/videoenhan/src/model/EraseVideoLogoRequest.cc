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

#include <alibabacloud/videoenhan/model/EraseVideoLogoRequest.h>

using AlibabaCloud::Videoenhan::Model::EraseVideoLogoRequest;

EraseVideoLogoRequest::EraseVideoLogoRequest() :
	RpcServiceRequest("videoenhan", "2020-03-20", "EraseVideoLogo")
{
	setMethod(HttpRequest::Method::Post);
}

EraseVideoLogoRequest::~EraseVideoLogoRequest()
{}

std::vector<EraseVideoLogoRequest::Boxes> EraseVideoLogoRequest::getBoxes()const
{
	return boxes_;
}

void EraseVideoLogoRequest::setBoxes(const std::vector<Boxes>& boxes)
{
	boxes_ = boxes;
	for(int dep1 = 0; dep1!= boxes.size(); dep1++) {
		auto boxesObj = boxes.at(dep1);
		std::string boxesObjStr = "Boxes." + std::to_string(dep1 + 1);
		setParameter(boxesObjStr + ".W", std::to_string(boxesObj.w));
		setParameter(boxesObjStr + ".H", std::to_string(boxesObj.h));
		setParameter(boxesObjStr + ".X", std::to_string(boxesObj.x));
		setParameter(boxesObjStr + ".Y", std::to_string(boxesObj.y));
	}
}

bool EraseVideoLogoRequest::getAsync()const
{
	return async_;
}

void EraseVideoLogoRequest::setAsync(bool async)
{
	async_ = async;
	setBodyParameter("Async", async ? "true" : "false");
}

std::string EraseVideoLogoRequest::getVideoUrl()const
{
	return videoUrl_;
}

void EraseVideoLogoRequest::setVideoUrl(const std::string& videoUrl)
{
	videoUrl_ = videoUrl;
	setBodyParameter("VideoUrl", videoUrl);
}

