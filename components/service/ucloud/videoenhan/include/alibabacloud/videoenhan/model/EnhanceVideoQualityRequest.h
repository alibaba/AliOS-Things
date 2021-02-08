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

#ifndef ALIBABACLOUD_VIDEOENHAN_MODEL_ENHANCEVIDEOQUALITYREQUEST_H_
#define ALIBABACLOUD_VIDEOENHAN_MODEL_ENHANCEVIDEOQUALITYREQUEST_H_

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
			class ALIBABACLOUD_VIDEOENHAN_EXPORT EnhanceVideoQualityRequest : public RpcServiceRequest
			{

			public:
				EnhanceVideoQualityRequest();
				~EnhanceVideoQualityRequest();

				std::string getHDRFormat()const;
				void setHDRFormat(const std::string& hDRFormat);
				int getFrameRate()const;
				void setFrameRate(int frameRate);
				int getMaxIlluminance()const;
				void setMaxIlluminance(int maxIlluminance);
				int getBitrate()const;
				void setBitrate(int bitrate);
				int getOutPutWidth()const;
				void setOutPutWidth(int outPutWidth);
				int getOutPutHeight()const;
				void setOutPutHeight(int outPutHeight);
				bool getAsync()const;
				void setAsync(bool async);
				std::string getVideoURL()const;
				void setVideoURL(const std::string& videoURL);

            private:
				std::string hDRFormat_;
				int frameRate_;
				int maxIlluminance_;
				int bitrate_;
				int outPutWidth_;
				int outPutHeight_;
				bool async_;
				std::string videoURL_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_VIDEOENHAN_MODEL_ENHANCEVIDEOQUALITYREQUEST_H_