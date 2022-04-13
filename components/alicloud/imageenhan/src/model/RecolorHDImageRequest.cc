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

#include <alibabacloud/imageenhan/model/RecolorHDImageRequest.h>

using AlibabaCloud::Imageenhan::Model::RecolorHDImageRequest;

RecolorHDImageRequest::RecolorHDImageRequest() :
	RpcServiceRequest("imageenhan", "2019-09-30", "RecolorHDImage")
{
	setMethod(HttpRequest::Method::Post);
}

RecolorHDImageRequest::~RecolorHDImageRequest()
{}

std::vector<RecolorHDImageRequest::ColorTemplate> RecolorHDImageRequest::getColorTemplate()const
{
	return colorTemplate_;
}

void RecolorHDImageRequest::setColorTemplate(const std::vector<ColorTemplate>& colorTemplate)
{
	colorTemplate_ = colorTemplate;
	for(int dep1 = 0; dep1!= colorTemplate.size(); dep1++) {
		auto colorTemplateObj = colorTemplate.at(dep1);
		std::string colorTemplateObjStr = "ColorTemplate." + std::to_string(dep1 + 1);
		setParameter(colorTemplateObjStr + ".Color", colorTemplateObj.color);
	}
}

std::string RecolorHDImageRequest::getDegree()const
{
	return degree_;
}

void RecolorHDImageRequest::setDegree(const std::string& degree)
{
	degree_ = degree;
	setBodyParameter("Degree", degree);
}

std::string RecolorHDImageRequest::getUrl()const
{
	return url_;
}

void RecolorHDImageRequest::setUrl(const std::string& url)
{
	url_ = url;
	setBodyParameter("Url", url);
}

std::string RecolorHDImageRequest::getMode()const
{
	return mode_;
}

void RecolorHDImageRequest::setMode(const std::string& mode)
{
	mode_ = mode;
	setBodyParameter("Mode", mode);
}

bool RecolorHDImageRequest::getAsync()const
{
	return async_;
}

void RecolorHDImageRequest::setAsync(bool async)
{
	async_ = async;
	setBodyParameter("Async", async ? "true" : "false");
}

int RecolorHDImageRequest::getColorCount()const
{
	return colorCount_;
}

void RecolorHDImageRequest::setColorCount(int colorCount)
{
	colorCount_ = colorCount;
	setBodyParameter("ColorCount", std::to_string(colorCount));
}

std::string RecolorHDImageRequest::getRefUrl()const
{
	return refUrl_;
}

void RecolorHDImageRequest::setRefUrl(const std::string& refUrl)
{
	refUrl_ = refUrl;
	setBodyParameter("RefUrl", refUrl);
}

