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

#ifndef ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERSRESULT_H_
#define ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERSRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/live/LiveExport.h>

namespace AlibabaCloud
{
	namespace Live
	{
		namespace Model
		{
			class ALIBABACLOUD_LIVE_EXPORT DescribeCastersResult : public ServiceResult
			{
			public:
				struct Caster
				{
					int status;
					std::string purchaseTime;
					std::string casterTemplate;
					std::string casterName;
					int channelEnable;
					std::string chargeType;
					std::string createTime;
					std::string startTime;
					int normType;
					std::string expireTime;
					std::string casterId;
				};


				DescribeCastersResult();
				explicit DescribeCastersResult(const std::string &payload);
				~DescribeCastersResult();
				int getTotal()const;
				std::vector<Caster> getCasterList()const;

			protected:
				void parse(const std::string &payload);
			private:
				int total_;
				std::vector<Caster> casterList_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERSRESULT_H_