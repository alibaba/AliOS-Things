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

#ifndef ALIBABACLOUD_VIDEOSEG_MODEL_SEGMENTHALFBODYREQUEST_H_
#define ALIBABACLOUD_VIDEOSEG_MODEL_SEGMENTHALFBODYREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/videoseg/VideosegExport.h>

namespace AlibabaCloud
{
	namespace Videoseg
	{
		namespace Model
		{
			class ALIBABACLOUD_VIDEOSEG_EXPORT SegmentHalfBodyRequest : public RpcServiceRequest
			{

			public:
				SegmentHalfBodyRequest();
				~SegmentHalfBodyRequest();

				bool getAsync()const;
				void setAsync(bool async);
				std::string getVideoUrl()const;
				void setVideoUrl(const std::string& videoUrl);

            private:
				bool async_;
				std::string videoUrl_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_VIDEOSEG_MODEL_SEGMENTHALFBODYREQUEST_H_