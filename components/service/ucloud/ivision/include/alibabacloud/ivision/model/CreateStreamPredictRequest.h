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

#ifndef ALIBABACLOUD_IVISION_MODEL_CREATESTREAMPREDICTREQUEST_H_
#define ALIBABACLOUD_IVISION_MODEL_CREATESTREAMPREDICTREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/ivision/IvisionExport.h>

namespace AlibabaCloud
{
	namespace Ivision
	{
		namespace Model
		{
			class ALIBABACLOUD_IVISION_EXPORT CreateStreamPredictRequest : public RpcServiceRequest
			{

			public:
				CreateStreamPredictRequest();
				~CreateStreamPredictRequest();

				std::string getClientToken()const;
				void setClientToken(const std::string& clientToken);
				std::string getAutoStart()const;
				void setAutoStart(const std::string& autoStart);
				std::string getNotify()const;
				void setNotify(const std::string& notify);
				std::string getOutput()const;
				void setOutput(const std::string& output);
				std::string getShowLog()const;
				void setShowLog(const std::string& showLog);
				std::string getStreamType()const;
				void setStreamType(const std::string& streamType);
				std::string getFaceGroupId()const;
				void setFaceGroupId(const std::string& faceGroupId);
				std::string getStreamId()const;
				void setStreamId(const std::string& streamId);
				std::string getPredictTemplateId()const;
				void setPredictTemplateId(const std::string& predictTemplateId);
				std::string getDetectIntervals()const;
				void setDetectIntervals(const std::string& detectIntervals);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				std::string getProbabilityThresholds()const;
				void setProbabilityThresholds(const std::string& probabilityThresholds);
				std::string getModelIds()const;
				void setModelIds(const std::string& modelIds);
				std::string getModelUserData()const;
				void setModelUserData(const std::string& modelUserData);

            private:
				std::string clientToken_;
				std::string autoStart_;
				std::string notify_;
				std::string output_;
				std::string showLog_;
				std::string streamType_;
				std::string faceGroupId_;
				std::string streamId_;
				std::string predictTemplateId_;
				std::string detectIntervals_;
				long ownerId_;
				std::string probabilityThresholds_;
				std::string modelIds_;
				std::string modelUserData_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IVISION_MODEL_CREATESTREAMPREDICTREQUEST_H_