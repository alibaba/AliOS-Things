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

#include <alibabacloud/ocr/model/TrimDocumentRequest.h>

using AlibabaCloud::Ocr::Model::TrimDocumentRequest;

TrimDocumentRequest::TrimDocumentRequest() :
	RpcServiceRequest("ocr", "2019-12-30", "TrimDocument")
{
	setMethod(HttpRequest::Method::Post);
}

TrimDocumentRequest::~TrimDocumentRequest()
{}

std::string TrimDocumentRequest::getFileType()const
{
	return fileType_;
}

void TrimDocumentRequest::setFileType(const std::string& fileType)
{
	fileType_ = fileType;
	setBodyParameter("FileType", fileType);
}

bool TrimDocumentRequest::getAsync()const
{
	return async_;
}

void TrimDocumentRequest::setAsync(bool async)
{
	async_ = async;
	setBodyParameter("Async", async ? "true" : "false");
}

std::string TrimDocumentRequest::getFileURL()const
{
	return fileURL_;
}

void TrimDocumentRequest::setFileURL(const std::string& fileURL)
{
	fileURL_ = fileURL;
	setBodyParameter("FileURL", fileURL);
}

std::string TrimDocumentRequest::getOutputType()const
{
	return outputType_;
}

void TrimDocumentRequest::setOutputType(const std::string& outputType)
{
	outputType_ = outputType;
	setBodyParameter("OutputType", outputType);
}

