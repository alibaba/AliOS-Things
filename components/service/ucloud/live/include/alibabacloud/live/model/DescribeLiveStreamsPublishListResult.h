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

#ifndef ALIBABACLOUD_LIVE_MODEL_DESCRIBELIVESTREAMSPUBLISHLISTRESULT_H_
#define ALIBABACLOUD_LIVE_MODEL_DESCRIBELIVESTREAMSPUBLISHLISTRESULT_H_

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
			class ALIBABACLOUD_LIVE_EXPORT DescribeLiveStreamsPublishListResult : public ServiceResult
			{
			public:
				struct LiveStreamPublishInfo
				{
					std::string publishUrl;
					std::string clientAddr;
					std::string domainName;
					std::string transcodeId;
					std::string streamUrl;
					std::string publishType;
					std::string appName;
					std::string streamName;
					std::string edgeNodeAddr;
					std::string publishDomain;
					std::string transcoded;
					std::string publishTime;
					std::string stopTime;
				};


				DescribeLiveStreamsPublishListResult();
				explicit DescribeLiveStreamsPublishListResult(const std::string &payload);
				~DescribeLiveStreamsPublishListResult();
				int getTotalNum()const;
				int getPageNum()const;
				int getPageSize()const;
				int getTotalPage()const;
				std::vector<LiveStreamPublishInfo> getPublishInfo()const;

			protected:
				void parse(const std::string &payload);
			private:
				int totalNum_;
				int pageNum_;
				int pageSize_;
				int totalPage_;
				std::vector<LiveStreamPublishInfo> publishInfo_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_DESCRIBELIVESTREAMSPUBLISHLISTRESULT_H_