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

#include <alibabacloud/ivision/model/CreateStreamPredictRequest.h>

using AlibabaCloud::Ivision::Model::CreateStreamPredictRequest;

CreateStreamPredictRequest::CreateStreamPredictRequest() :
	RpcServiceRequest("ivision", "2019-03-08", "CreateStreamPredict")
{
	setMethod(HttpRequest::Method::Post);
}

CreateStreamPredictRequest::~CreateStreamPredictRequest()
{}

std::string CreateStreamPredictRequest::getClientToken()const
{
	return clientToken_;
}

void CreateStreamPredictRequest::setClientToken(const std::string& clientToken)
{
	clientToken_ = clientToken;
	setParameter("ClientToken", clientToken);
}

std::string CreateStreamPredictRequest::getAutoStart()const
{
	return autoStart_;
}

void CreateStreamPredictRequest::setAutoStart(const std::string& autoStart)
{
	autoStart_ = autoStart;
	setParameter("AutoStart", autoStart);
}

std::string CreateStreamPredictRequest::getNotify()const
{
	return notify_;
}

void CreateStreamPredictRequest::setNotify(const std::string& notify)
{
	notify_ = notify;
	setParameter("Notify", notify);
}

std::string CreateStreamPredictRequest::getOutput()const
{
	return output_;
}

void CreateStreamPredictRequest::setOutput(const std::string& output)
{
	output_ = output;
	setParameter("Output", output);
}

std::string CreateStreamPredictRequest::getShowLog()const
{
	return showLog_;
}

void CreateStreamPredictRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string CreateStreamPredictRequest::getStreamType()const
{
	return streamType_;
}

void CreateStreamPredictRequest::setStreamType(const std::string& streamType)
{
	streamType_ = streamType;
	setParameter("StreamType", streamType);
}

std::string CreateStreamPredictRequest::getFaceGroupId()const
{
	return faceGroupId_;
}

void CreateStreamPredictRequest::setFaceGroupId(const std::string& faceGroupId)
{
	faceGroupId_ = faceGroupId;
	setParameter("FaceGroupId", faceGroupId);
}

std::string CreateStreamPredictRequest::getStreamId()const
{
	return streamId_;
}

void CreateStreamPredictRequest::setStreamId(const std::string& streamId)
{
	streamId_ = streamId;
	setParameter("StreamId", streamId);
}

std::string CreateStreamPredictRequest::getPredictTemplateId()const
{
	return predictTemplateId_;
}

void CreateStreamPredictRequest::setPredictTemplateId(const std::string& predictTemplateId)
{
	predictTemplateId_ = predictTemplateId;
	setParameter("PredictTemplateId", predictTemplateId);
}

std::string CreateStreamPredictRequest::getDetectIntervals()const
{
	return detectIntervals_;
}

void CreateStreamPredictRequest::setDetectIntervals(const std::string& detectIntervals)
{
	detectIntervals_ = detectIntervals;
	setParameter("DetectIntervals", detectIntervals);
}

long CreateStreamPredictRequest::getOwnerId()const
{
	return ownerId_;
}

void CreateStreamPredictRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string CreateStreamPredictRequest::getProbabilityThresholds()const
{
	return probabilityThresholds_;
}

void CreateStreamPredictRequest::setProbabilityThresholds(const std::string& probabilityThresholds)
{
	probabilityThresholds_ = probabilityThresholds;
	setParameter("ProbabilityThresholds", probabilityThresholds);
}

std::string CreateStreamPredictRequest::getModelIds()const
{
	return modelIds_;
}

void CreateStreamPredictRequest::setModelIds(const std::string& modelIds)
{
	modelIds_ = modelIds;
	setParameter("ModelIds", modelIds);
}

std::string CreateStreamPredictRequest::getModelUserData()const
{
	return modelUserData_;
}

void CreateStreamPredictRequest::setModelUserData(const std::string& modelUserData)
{
	modelUserData_ = modelUserData;
	setParameter("ModelUserData", modelUserData);
}

