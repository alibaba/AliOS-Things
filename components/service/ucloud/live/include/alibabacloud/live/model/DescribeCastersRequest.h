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

#ifndef ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERSREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERSREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/live/LiveExport.h>

namespace AlibabaCloud
{
	namespace Live
	{
		namespace Model
		{
			class ALIBABACLOUD_LIVE_EXPORT DescribeCastersRequest : public RpcServiceRequest
			{

			public:
				DescribeCastersRequest();
				~DescribeCastersRequest();

				std::string getStartTime()const;
				void setStartTime(const std::string& startTime);
				int getPageNum()const;
				void setPageNum(int pageNum);
				std::string getCasterName()const;
				void setCasterName(const std::string& casterName);
				int getPageSize()const;
				void setPageSize(int pageSize);
				std::string getCasterId()const;
				void setCasterId(const std::string& casterId);
				std::string getEndTime()const;
				void setEndTime(const std::string& endTime);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				int getStatus()const;
				void setStatus(int status);

            private:
				std::string startTime_;
				int pageNum_;
				std::string casterName_;
				int pageSize_;
				std::string casterId_;
				std::string endTime_;
				long ownerId_;
				int status_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERSREQUEST_H_