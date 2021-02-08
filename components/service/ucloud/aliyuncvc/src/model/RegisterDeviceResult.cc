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

#include <alibabacloud/aliyuncvc/model/RegisterDeviceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

RegisterDeviceResult::RegisterDeviceResult() :
	ServiceResult()
{}

RegisterDeviceResult::RegisterDeviceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RegisterDeviceResult::~RegisterDeviceResult()
{}

void RegisterDeviceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto deviceInfoNode = value["DeviceInfo"];
	if(!deviceInfoNode["ChannelType"].isNull())
		deviceInfo_.channelType = deviceInfoNode["ChannelType"].asString();
	if(!deviceInfoNode["RegisterTime"].isNull())
		deviceInfo_.registerTime = std::stol(deviceInfoNode["RegisterTime"].asString());
	if(!deviceInfoNode["DeviceName"].isNull())
		deviceInfo_.deviceName = deviceInfoNode["DeviceName"].asString();
	if(!deviceInfoNode["DeviceSessionId"].isNull())
		deviceInfo_.deviceSessionId = deviceInfoNode["DeviceSessionId"].asString();
	if(!deviceInfoNode["MessageKey"].isNull())
		deviceInfo_.messageKey = deviceInfoNode["MessageKey"].asString();
	if(!deviceInfoNode["ScreenCode"].isNull())
		deviceInfo_.screenCode = deviceInfoNode["ScreenCode"].asString();
	auto mqttParamNode = deviceInfoNode["MqttParam"];
	if(!mqttParamNode["ClientId"].isNull())
		deviceInfo_.mqttParam.clientId = mqttParamNode["ClientId"].asString();
	if(!mqttParamNode["GroupId"].isNull())
		deviceInfo_.mqttParam.groupId = mqttParamNode["GroupId"].asString();
	if(!mqttParamNode["CleanSession"].isNull())
		deviceInfo_.mqttParam.cleanSession = mqttParamNode["CleanSession"].asString();
	if(!mqttParamNode["Password"].isNull())
		deviceInfo_.mqttParam.password = mqttParamNode["Password"].asString();
	if(!mqttParamNode["Port"].isNull())
		deviceInfo_.mqttParam.port = mqttParamNode["Port"].asString();
	if(!mqttParamNode["Host"].isNull())
		deviceInfo_.mqttParam.host = mqttParamNode["Host"].asString();
	if(!mqttParamNode["Topic"].isNull())
		deviceInfo_.mqttParam.topic = mqttParamNode["Topic"].asString();
	if(!mqttParamNode["SDKClientPort"].isNull())
		deviceInfo_.mqttParam.sDKClientPort = mqttParamNode["SDKClientPort"].asString();
	if(!mqttParamNode["TLSPort"].isNull())
		deviceInfo_.mqttParam.tLSPort = mqttParamNode["TLSPort"].asString();
	if(!mqttParamNode["UseTLS"].isNull())
		deviceInfo_.mqttParam.useTLS = mqttParamNode["UseTLS"].asString();
	if(!mqttParamNode["UserName"].isNull())
		deviceInfo_.mqttParam.userName = mqttParamNode["UserName"].asString();
	if(!mqttParamNode["ReconnectTimeout"].isNull())
		deviceInfo_.mqttParam.reconnectTimeout = mqttParamNode["ReconnectTimeout"].asString();
	auto authWsChannelConfigNode = deviceInfoNode["AuthWsChannelConfig"];
	if(!authWsChannelConfigNode["AuthWsUrl"].isNull())
		deviceInfo_.authWsChannelConfig.authWsUrl = authWsChannelConfigNode["AuthWsUrl"].asString();
	if(!authWsChannelConfigNode["WsOuterReconnTime"].isNull())
		deviceInfo_.authWsChannelConfig.wsOuterReconnTime = std::stoi(authWsChannelConfigNode["WsOuterReconnTime"].asString());
	if(!authWsChannelConfigNode["Token"].isNull())
		deviceInfo_.authWsChannelConfig.token = authWsChannelConfigNode["Token"].asString();
	auto slsConfigNode = deviceInfoNode["SlsConfig"];
	if(!slsConfigNode["Project"].isNull())
		deviceInfo_.slsConfig.project = slsConfigNode["Project"].asString();
	if(!slsConfigNode["LogStore"].isNull())
		deviceInfo_.slsConfig.logStore = slsConfigNode["LogStore"].asString();
	if(!slsConfigNode["LogServiceEndpoint"].isNull())
		deviceInfo_.slsConfig.logServiceEndpoint = slsConfigNode["LogServiceEndpoint"].asString();
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();

}

std::string RegisterDeviceResult::getMessage()const
{
	return message_;
}

RegisterDeviceResult::DeviceInfo RegisterDeviceResult::getDeviceInfo()const
{
	return deviceInfo_;
}

int RegisterDeviceResult::getErrorCode()const
{
	return errorCode_;
}

bool RegisterDeviceResult::getSuccess()const
{
	return success_;
}

