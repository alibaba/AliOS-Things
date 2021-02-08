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

#ifndef ALIBABACLOUD_IVISION_MODEL_DESCRIBESTREAMPREDICTSRESULT_H_
#define ALIBABACLOUD_IVISION_MODEL_DESCRIBESTREAMPREDICTSRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/ivision/IvisionExport.h>

namespace AlibabaCloud
{
	namespace Ivision
	{
		namespace Model
		{
			class ALIBABACLOUD_IVISION_EXPORT DescribeStreamPredictsResult : public ServiceResult
			{
			public:
				struct StreamPredict
				{
					std::string status;
					std::string streamType;
					std::string probabilityThresholds;
					std::string streamId;
					std::string notify;
					std::string userData;
					std::string predictId;
					std::string detectIntervals;
					std::string modelUserData;
					std::string predictTemplateId;
					std::string output;
					std::string creationTime;
					std::string modelIds;
					std::string faceGroupId;
					std::string autoStart;
				};


				DescribeStreamPredictsResult();
				explicit DescribeStreamPredictsResult(const std::string &payload);
				~DescribeStreamPredictsResult();
				long getTotalNum()const;
				long getPageSize()const;
				long getCurrentPage()const;
				std::string getNextPageToken()const;
				std::vector<StreamPredict> getStreamPredicts()const;

			protected:
				void parse(const std::string &payload);
			private:
				long totalNum_;
				long pageSize_;
				long currentPage_;
				std::string nextPageToken_;
				std::vector<StreamPredict> streamPredicts_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IVISION_MODEL_DESCRIBESTREAMPREDICTSRESULT_H_