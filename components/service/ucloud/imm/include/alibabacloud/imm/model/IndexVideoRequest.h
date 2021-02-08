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

#ifndef ALIBABACLOUD_IMM_MODEL_INDEXVIDEOREQUEST_H_
#define ALIBABACLOUD_IMM_MODEL_INDEXVIDEOREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/imm/ImmExport.h>

namespace AlibabaCloud
{
	namespace Imm
	{
		namespace Model
		{
			class ALIBABACLOUD_IMM_EXPORT IndexVideoRequest : public RpcServiceRequest
			{

			public:
				IndexVideoRequest();
				~IndexVideoRequest();

				std::string getGrabType()const;
				void setGrabType(const std::string& grabType);
				std::string getProject()const;
				void setProject(const std::string& project);
				std::string getExternalId()const;
				void setExternalId(const std::string& externalId);
				std::string getStartTime()const;
				void setStartTime(const std::string& startTime);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getNotifyEndpoint()const;
				void setNotifyEndpoint(const std::string& notifyEndpoint);
				std::string getNotifyTopicName()const;
				void setNotifyTopicName(const std::string& notifyTopicName);
				std::string getRemarksB()const;
				void setRemarksB(const std::string& remarksB);
				std::string getRemarksA()const;
				void setRemarksA(const std::string& remarksA);
				std::string getEndTime()const;
				void setEndTime(const std::string& endTime);
				std::string getVideoUri()const;
				void setVideoUri(const std::string& videoUri);
				bool getSaveType()const;
				void setSaveType(bool saveType);
				std::string getRemarksD()const;
				void setRemarksD(const std::string& remarksD);
				std::string getRemarksC()const;
				void setRemarksC(const std::string& remarksC);
				std::string getSetId()const;
				void setSetId(const std::string& setId);
				std::string getInterval()const;
				void setInterval(const std::string& interval);
				std::string getTgtUri()const;
				void setTgtUri(const std::string& tgtUri);

            private:
				std::string grabType_;
				std::string project_;
				std::string externalId_;
				std::string startTime_;
				std::string accessKeyId_;
				std::string notifyEndpoint_;
				std::string notifyTopicName_;
				std::string remarksB_;
				std::string remarksA_;
				std::string endTime_;
				std::string videoUri_;
				bool saveType_;
				std::string remarksD_;
				std::string remarksC_;
				std::string setId_;
				std::string interval_;
				std::string tgtUri_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_INDEXVIDEOREQUEST_H_