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

#include <alibabacloud/ivision/model/StartStreamPredictRequest.h>

using AlibabaCloud::Ivision::Model::StartStreamPredictRequest;

StartStreamPredictRequest::StartStreamPredictRequest() :
	RpcServiceRequest("ivision", "2019-03-08", "StartStreamPredict")
{
	setMethod(HttpRequest::Method::Post);
}

StartStreamPredictRequest::~StartStreamPredictRequest()
{}

std::string StartStreamPredictRequest::getPredictId()const
{
	return predictId_;
}

void StartStreamPredictRequest::setPredictId(const std::string& predictId)
{
	predictId_ = predictId;
	setParameter("PredictId", predictId);
}

std::string StartStreamPredictRequest::getShowLog()const
{
	return showLog_;
}

void StartStreamPredictRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long StartStreamPredictRequest::getOwnerId()const
{
	return ownerId_;
}

void StartStreamPredictRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

