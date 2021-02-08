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

#ifndef ALIBABACLOUD_IVISION_MODEL_DESCRIBESTREAMPREDICTRESULTREQUEST_H_
#define ALIBABACLOUD_IVISION_MODEL_DESCRIBESTREAMPREDICTRESULTREQUEST_H_

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
			class ALIBABACLOUD_IVISION_EXPORT DescribeStreamPredictResultRequest : public RpcServiceRequest
			{

			public:
				DescribeStreamPredictResultRequest();
				~DescribeStreamPredictResultRequest();

				std::string getNextPageToken()const;
				void setNextPageToken(const std::string& nextPageToken);
				std::string getStartTime()const;
				void setStartTime(const std::string& startTime);
				std::string getPredictId()const;
				void setPredictId(const std::string& predictId);
				long getPageSize()const;
				void setPageSize(long pageSize);
				std::string getProbabilityThreshold()const;
				void setProbabilityThreshold(const std::string& probabilityThreshold);
				std::string getShowLog()const;
				void setShowLog(const std::string& showLog);
				std::string getModelId()const;
				void setModelId(const std::string& modelId);
				std::string getEndTime()const;
				void setEndTime(const std::string& endTime);
				long getCurrentPage()const;
				void setCurrentPage(long currentPage);
				long getOwnerId()const;
				void setOwnerId(long ownerId);

            private:
				std::string nextPageToken_;
				std::string startTime_;
				std::string predictId_;
				long pageSize_;
				std::string probabilityThreshold_;
				std::string showLog_;
				std::string modelId_;
				std::string endTime_;
				long currentPage_;
				long ownerId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IVISION_MODEL_DESCRIBESTREAMPREDICTRESULTREQUEST_H_