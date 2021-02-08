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

#ifndef ALIBABACLOUD_CLOUDPHOTO_MODEL_EDITEVENTREQUEST_H_
#define ALIBABACLOUD_CLOUDPHOTO_MODEL_EDITEVENTREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/cloudphoto/CloudPhotoExport.h>

namespace AlibabaCloud
{
	namespace CloudPhoto
	{
		namespace Model
		{
			class ALIBABACLOUD_CLOUDPHOTO_EXPORT EditEventRequest : public RpcServiceRequest
			{

			public:
				EditEventRequest();
				~EditEventRequest();

				std::string getEventId()const;
				void setEventId(const std::string& eventId);
				std::string getStoreName()const;
				void setStoreName(const std::string& storeName);
				std::string getRemark()const;
				void setRemark(const std::string& remark);
				std::string getTitle()const;
				void setTitle(const std::string& title);
				long getEndAt()const;
				void setEndAt(long endAt);
				std::string getIdentity()const;
				void setIdentity(const std::string& identity);
				std::string getWatermarkPhotoId()const;
				void setWatermarkPhotoId(const std::string& watermarkPhotoId);
				std::string getSplashPhotoId()const;
				void setSplashPhotoId(const std::string& splashPhotoId);
				std::string getLibraryId()const;
				void setLibraryId(const std::string& libraryId);
				std::string getWeixinTitle()const;
				void setWeixinTitle(const std::string& weixinTitle);
				std::string getBannerPhotoId()const;
				void setBannerPhotoId(const std::string& bannerPhotoId);
				long getStartAt()const;
				void setStartAt(long startAt);

            private:
				std::string eventId_;
				std::string storeName_;
				std::string remark_;
				std::string title_;
				long endAt_;
				std::string identity_;
				std::string watermarkPhotoId_;
				std::string splashPhotoId_;
				std::string libraryId_;
				std::string weixinTitle_;
				std::string bannerPhotoId_;
				long startAt_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDPHOTO_MODEL_EDITEVENTREQUEST_H_