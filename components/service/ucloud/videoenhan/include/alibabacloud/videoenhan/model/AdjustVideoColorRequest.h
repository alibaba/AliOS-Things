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

#ifndef ALIBABACLOUD_VIDEOENHAN_MODEL_ADJUSTVIDEOCOLORREQUEST_H_
#define ALIBABACLOUD_VIDEOENHAN_MODEL_ADJUSTVIDEOCOLORREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/videoenhan/VideoenhanExport.h>

namespace AlibabaCloud
{
	namespace Videoenhan
	{
		namespace Model
		{
			class ALIBABACLOUD_VIDEOENHAN_EXPORT AdjustVideoColorRequest : public RpcServiceRequest
			{

			public:
				AdjustVideoColorRequest();
				~AdjustVideoColorRequest();

				std::string getMode()const;
				void setMode(const std::string& mode);
				bool getAsync()const;
				void setAsync(bool async);
				std::string getVideoUrl()const;
				void setVideoUrl(const std::string& videoUrl);
				std::string getVideoBitrate()const;
				void setVideoBitrate(const std::string& videoBitrate);
				std::string getVideoCodec()const;
				void setVideoCodec(const std::string& videoCodec);
				std::string getVideoFormat()const;
				void setVideoFormat(const std::string& videoFormat);

            private:
				std::string mode_;
				bool async_;
				std::string videoUrl_;
				std::string videoBitrate_;
				std::string videoCodec_;
				std::string videoFormat_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_VIDEOENHAN_MODEL_ADJUSTVIDEOCOLORREQUEST_H_