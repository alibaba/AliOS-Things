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

#include <alibabacloud/ocr/model/RecognizeTrainTicketRequest.h>

using AlibabaCloud::Ocr::Model::RecognizeTrainTicketRequest;

RecognizeTrainTicketRequest::RecognizeTrainTicketRequest() :
	RpcServiceRequest("ocr", "2019-12-30", "RecognizeTrainTicket")
{
	setMethod(HttpRequest::Method::Post);
}

RecognizeTrainTicketRequest::~RecognizeTrainTicketRequest()
{}

int RecognizeTrainTicketRequest::getImageType()const
{
	return imageType_;
}

void RecognizeTrainTicketRequest::setImageType(int imageType)
{
	imageType_ = imageType;
	setBodyParameter("ImageType", std::to_string(imageType));
}

std::string RecognizeTrainTicketRequest::getImageURL()const
{
	return imageURL_;
}

void RecognizeTrainTicketRequest::setImageURL(const std::string& imageURL)
{
	imageURL_ = imageURL;
	setBodyParameter("ImageURL", imageURL);
}

