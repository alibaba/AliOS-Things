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

#include <alibabacloud/nlp-automl/model/GetAsyncPredictRequest.h>

using AlibabaCloud::Nlp_automl::Model::GetAsyncPredictRequest;

GetAsyncPredictRequest::GetAsyncPredictRequest() :
	RpcServiceRequest("nlp-automl", "2019-11-11", "GetAsyncPredict")
{
	setMethod(HttpRequest::Method::Get);
}

GetAsyncPredictRequest::~GetAsyncPredictRequest()
{}

int GetAsyncPredictRequest::getAsyncPredictId()const
{
	return asyncPredictId_;
}

void GetAsyncPredictRequest::setAsyncPredictId(int asyncPredictId)
{
	asyncPredictId_ = asyncPredictId;
	setParameter("AsyncPredictId", std::to_string(asyncPredictId));
}

