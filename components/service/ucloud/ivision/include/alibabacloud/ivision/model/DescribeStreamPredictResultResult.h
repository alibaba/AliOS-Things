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

#ifndef ALIBABACLOUD_IVISION_MODEL_DESCRIBESTREAMPREDICTRESULTRESULT_H_
#define ALIBABACLOUD_IVISION_MODEL_DESCRIBESTREAMPREDICTRESULTRESULT_H_

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
			class ALIBABACLOUD_IVISION_EXPORT DescribeStreamPredictResultResult : public ServiceResult
			{
			public:
				struct StreamPredictData
				{
					std::string status;
					std::string predictResult;
					std::string dataUrl;
					std::string predictTime;
					std::string predictId;
					long timestamp;
					std::string modelId;
				};


				DescribeStreamPredictResultResult();
				explicit DescribeStreamPredictResultResult(const std::string &payload);
				~DescribeStreamPredictResultResult();
				std::vector<StreamPredictData> getStreamPredictDatas()const;
				long getTotalNum()const;
				long getPageSize()const;
				long getCurrentPage()const;
				std::string getNextPageToken()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<StreamPredictData> streamPredictDatas_;
				long totalNum_;
				long pageSize_;
				long currentPage_;
				std::string nextPageToken_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IVISION_MODEL_DESCRIBESTREAMPREDICTRESULTRESULT_H_