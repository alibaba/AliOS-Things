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

#ifndef ALIBABACLOUD_IOT_MODEL_QUERYSUMMARYSCENERULELOGREQUEST_H_
#define ALIBABACLOUD_IOT_MODEL_QUERYSUMMARYSCENERULELOGREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/iot/IotExport.h>

namespace AlibabaCloud
{
	namespace Iot
	{
		namespace Model
		{
			class ALIBABACLOUD_IOT_EXPORT QuerySummarySceneRuleLogRequest : public RpcServiceRequest
			{

			public:
				QuerySummarySceneRuleLogRequest();
				~QuerySummarySceneRuleLogRequest();

				int getStartTime()const;
				void setStartTime(int startTime);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getIotInstanceId()const;
				void setIotInstanceId(const std::string& iotInstanceId);
				int getPageSize()const;
				void setPageSize(int pageSize);
				int getEndTime()const;
				void setEndTime(int endTime);
				int getCurrentPage()const;
				void setCurrentPage(int currentPage);
				std::string getApiProduct()const;
				void setApiProduct(const std::string& apiProduct);
				std::string getApiRevision()const;
				void setApiRevision(const std::string& apiRevision);
				std::string getRuleId()const;
				void setRuleId(const std::string& ruleId);
				std::string getStatus()const;
				void setStatus(const std::string& status);

            private:
				int startTime_;
				std::string accessKeyId_;
				std::string iotInstanceId_;
				int pageSize_;
				int endTime_;
				int currentPage_;
				std::string apiProduct_;
				std::string apiRevision_;
				std::string ruleId_;
				std::string status_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IOT_MODEL_QUERYSUMMARYSCENERULELOGREQUEST_H_