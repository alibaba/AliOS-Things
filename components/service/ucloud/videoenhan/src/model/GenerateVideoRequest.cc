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

#include <alibabacloud/videoenhan/model/GenerateVideoRequest.h>

using AlibabaCloud::Videoenhan::Model::GenerateVideoRequest;

GenerateVideoRequest::GenerateVideoRequest() :
	RpcServiceRequest("videoenhan", "2020-03-20", "GenerateVideo")
{
	setMethod(HttpRequest::Method::Post);
}

GenerateVideoRequest::~GenerateVideoRequest()
{}

std::string GenerateVideoRequest::getTransitionStyle()const
{
	return transitionStyle_;
}

void GenerateVideoRequest::setTransitionStyle(const std::string& transitionStyle)
{
	transitionStyle_ = transitionStyle;
	setBodyParameter("TransitionStyle", transitionStyle);
}

std::string GenerateVideoRequest::getScene()const
{
	return scene_;
}

void GenerateVideoRequest::setScene(const std::string& scene)
{
	scene_ = scene;
	setBodyParameter("Scene", scene);
}

float GenerateVideoRequest::getDuration()const
{
	return duration_;
}

void GenerateVideoRequest::setDuration(float duration)
{
	duration_ = duration;
	setBodyParameter("Duration", std::to_string(duration));
}

bool GenerateVideoRequest::getPuzzleEffect()const
{
	return puzzleEffect_;
}

void GenerateVideoRequest::setPuzzleEffect(bool puzzleEffect)
{
	puzzleEffect_ = puzzleEffect;
	setBodyParameter("PuzzleEffect", puzzleEffect ? "true" : "false");
}

int GenerateVideoRequest::getHeight()const
{
	return height_;
}

void GenerateVideoRequest::setHeight(int height)
{
	height_ = height;
	setBodyParameter("Height", std::to_string(height));
}

bool GenerateVideoRequest::getDurationAdaption()const
{
	return durationAdaption_;
}

void GenerateVideoRequest::setDurationAdaption(bool durationAdaption)
{
	durationAdaption_ = durationAdaption;
	setBodyParameter("DurationAdaption", durationAdaption ? "true" : "false");
}

std::vector<GenerateVideoRequest::FileList> GenerateVideoRequest::getFileList()const
{
	return fileList_;
}

void GenerateVideoRequest::setFileList(const std::vector<FileList>& fileList)
{
	fileList_ = fileList;
	for(int dep1 = 0; dep1!= fileList.size(); dep1++) {
		auto fileListObj = fileList.at(dep1);
		std::string fileListObjStr = "FileList." + std::to_string(dep1 + 1);
		setParameter(fileListObjStr + ".FileName", fileListObj.fileName);
		setParameter(fileListObjStr + ".FileUrl", fileListObj.fileUrl);
		setParameter(fileListObjStr + ".Type", fileListObj.type);
	}
}

bool GenerateVideoRequest::getMute()const
{
	return mute_;
}

void GenerateVideoRequest::setMute(bool mute)
{
	mute_ = mute;
	setBodyParameter("Mute", mute ? "true" : "false");
}

bool GenerateVideoRequest::getAsync()const
{
	return async_;
}

void GenerateVideoRequest::setAsync(bool async)
{
	async_ = async;
	setBodyParameter("Async", async ? "true" : "false");
}

bool GenerateVideoRequest::getSmartEffect()const
{
	return smartEffect_;
}

void GenerateVideoRequest::setSmartEffect(bool smartEffect)
{
	smartEffect_ = smartEffect;
	setBodyParameter("SmartEffect", smartEffect ? "true" : "false");
}

int GenerateVideoRequest::getWidth()const
{
	return width_;
}

void GenerateVideoRequest::setWidth(int width)
{
	width_ = width;
	setBodyParameter("Width", std::to_string(width));
}

std::string GenerateVideoRequest::getStyle()const
{
	return style_;
}

void GenerateVideoRequest::setStyle(const std::string& style)
{
	style_ = style;
	setBodyParameter("Style", style);
}

