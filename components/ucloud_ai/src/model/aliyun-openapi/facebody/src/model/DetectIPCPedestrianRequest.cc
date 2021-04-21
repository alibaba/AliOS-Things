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

#include <alibabacloud/facebody/model/DetectIPCPedestrianRequest.h>

using AlibabaCloud::Facebody::Model::DetectIPCPedestrianRequest;

DetectIPCPedestrianRequest::DetectIPCPedestrianRequest() :
	RpcServiceRequest("facebody", "2019-12-30", "DetectIPCPedestrian")
{
	setMethod(HttpRequest::Method::Post);
}

DetectIPCPedestrianRequest::~DetectIPCPedestrianRequest()
{}

std::string DetectIPCPedestrianRequest::getImageData()const
{
	return imageData_;
}

void DetectIPCPedestrianRequest::setImageData(const std::string& imageData)
{
	imageData_ = imageData;
	setBodyParameter("ImageData", imageData);
}

std::vector<DetectIPCPedestrianRequest::URLList> DetectIPCPedestrianRequest::getURLList()const
{
	return uRLList_;
}

void DetectIPCPedestrianRequest::setURLList(const std::vector<URLList>& uRLList)
{
	uRLList_ = uRLList;
	for(int dep1 = 0; dep1!= uRLList.size(); dep1++) {
		auto uRLListObj = uRLList.at(dep1);
		std::string uRLListObjStr = "URLList." + std::to_string(dep1 + 1);
		setParameter(uRLListObjStr + ".DataId", uRLListObj.dataId);
		setParameter(uRLListObjStr + ".ImageURL", uRLListObj.imageURL);
	}
}

bool DetectIPCPedestrianRequest::getContinueOnError()const
{
	return continueOnError_;
}

void DetectIPCPedestrianRequest::setContinueOnError(bool continueOnError)
{
	continueOnError_ = continueOnError;
	setBodyParameter("ContinueOnError", continueOnError ? "true" : "false");
}

int DetectIPCPedestrianRequest::getWidth()const
{
	return width_;
}

void DetectIPCPedestrianRequest::setWidth(int width)
{
	width_ = width;
	setBodyParameter("Width", std::to_string(width));
}

int DetectIPCPedestrianRequest::getHeight()const
{
	return height_;
}

void DetectIPCPedestrianRequest::setHeight(int height)
{
	height_ = height;
	setBodyParameter("Height", std::to_string(height));
}

