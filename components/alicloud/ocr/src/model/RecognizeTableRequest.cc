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

#include <alibabacloud/ocr/model/RecognizeTableRequest.h>

using AlibabaCloud::Ocr::Model::RecognizeTableRequest;

RecognizeTableRequest::RecognizeTableRequest() :
	RpcServiceRequest("ocr", "2019-12-30", "RecognizeTable")
{
	setMethod(HttpRequest::Method::Post);
}

RecognizeTableRequest::~RecognizeTableRequest()
{}

int RecognizeTableRequest::getImageType()const
{
	return imageType_;
}

void RecognizeTableRequest::setImageType(int imageType)
{
	imageType_ = imageType;
	setBodyParameter("ImageType", std::to_string(imageType));
}

bool RecognizeTableRequest::getUseFinanceModel()const
{
	return useFinanceModel_;
}

void RecognizeTableRequest::setUseFinanceModel(bool useFinanceModel)
{
	useFinanceModel_ = useFinanceModel;
	setBodyParameter("UseFinanceModel", useFinanceModel ? "true" : "false");
}

bool RecognizeTableRequest::getSkipDetection()const
{
	return skipDetection_;
}

void RecognizeTableRequest::setSkipDetection(bool skipDetection)
{
	skipDetection_ = skipDetection;
	setBodyParameter("SkipDetection", skipDetection ? "true" : "false");
}

std::string RecognizeTableRequest::getImageURL()const
{
	return imageURL_;
}

void RecognizeTableRequest::setImageURL(const std::string& imageURL)
{
	imageURL_ = imageURL;
	setBodyParameter("ImageURL", imageURL);
}

std::string RecognizeTableRequest::getOutputFormat()const
{
	return outputFormat_;
}

void RecognizeTableRequest::setOutputFormat(const std::string& outputFormat)
{
	outputFormat_ = outputFormat;
	setBodyParameter("OutputFormat", outputFormat);
}

bool RecognizeTableRequest::getAssureDirection()const
{
	return assureDirection_;
}

void RecognizeTableRequest::setAssureDirection(bool assureDirection)
{
	assureDirection_ = assureDirection;
	setBodyParameter("AssureDirection", assureDirection ? "true" : "false");
}

bool RecognizeTableRequest::getHasLine()const
{
	return hasLine_;
}

void RecognizeTableRequest::setHasLine(bool hasLine)
{
	hasLine_ = hasLine;
	setBodyParameter("HasLine", hasLine ? "true" : "false");
}

