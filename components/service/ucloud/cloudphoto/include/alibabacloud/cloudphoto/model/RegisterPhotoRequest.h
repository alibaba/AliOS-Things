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

#ifndef ALIBABACLOUD_CLOUDPHOTO_MODEL_REGISTERPHOTOREQUEST_H_
#define ALIBABACLOUD_CLOUDPHOTO_MODEL_REGISTERPHOTOREQUEST_H_

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
			class ALIBABACLOUD_CLOUDPHOTO_EXPORT RegisterPhotoRequest : public RpcServiceRequest
			{

			public:
				RegisterPhotoRequest();
				~RegisterPhotoRequest();

				float getLatitude()const;
				void setLatitude(float latitude);
				std::string getPhotoTitle()const;
				void setPhotoTitle(const std::string& photoTitle);
				std::string getStoreName()const;
				void setStoreName(const std::string& storeName);
				std::string getIsVideo()const;
				void setIsVideo(const std::string& isVideo);
				std::string getRemark()const;
				void setRemark(const std::string& remark);
				long getTakenAt()const;
				void setTakenAt(long takenAt);
				float getLongitude()const;
				void setLongitude(float longitude);
				int getHeight()const;
				void setHeight(int height);
				std::string getLibraryId()const;
				void setLibraryId(const std::string& libraryId);
				long getSize()const;
				void setSize(long size);
				int getWidth()const;
				void setWidth(int width);
				std::string getLocation()const;
				void setLocation(const std::string& location);
				std::string getMd5()const;
				void setMd5(const std::string& md5);

            private:
				float latitude_;
				std::string photoTitle_;
				std::string storeName_;
				std::string isVideo_;
				std::string remark_;
				long takenAt_;
				float longitude_;
				int height_;
				std::string libraryId_;
				long size_;
				int width_;
				std::string location_;
				std::string md5_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDPHOTO_MODEL_REGISTERPHOTOREQUEST_H_