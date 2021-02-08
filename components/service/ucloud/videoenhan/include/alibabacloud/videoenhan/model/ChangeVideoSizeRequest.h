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

#ifndef ALIBABACLOUD_VIDEOENHAN_MODEL_CHANGEVIDEOSIZEREQUEST_H_
#define ALIBABACLOUD_VIDEOENHAN_MODEL_CHANGEVIDEOSIZEREQUEST_H_

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
			class ALIBABACLOUD_VIDEOENHAN_EXPORT ChangeVideoSizeRequest : public RpcServiceRequest
			{

			public:
				ChangeVideoSizeRequest();
				~ChangeVideoSizeRequest();

				int getHeight()const;
				void setHeight(int height);
				int getB()const;
				void setB(int b);
				std::string getFillType()const;
				void setFillType(const std::string& fillType);
				int getG()const;
				void setG(int g);
				std::string getCropType()const;
				void setCropType(const std::string& cropType);
				bool getAsync()const;
				void setAsync(bool async);
				int getR()const;
				void setR(int r);
				std::string getVideoUrl()const;
				void setVideoUrl(const std::string& videoUrl);
				int getWidth()const;
				void setWidth(int width);
				float getTightness()const;
				void setTightness(float tightness);

            private:
				int height_;
				int b_;
				std::string fillType_;
				int g_;
				std::string cropType_;
				bool async_;
				int r_;
				std::string videoUrl_;
				int width_;
				float tightness_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_VIDEOENHAN_MODEL_CHANGEVIDEOSIZEREQUEST_H_