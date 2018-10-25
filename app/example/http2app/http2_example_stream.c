/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "iot_export.h"
#include "iot_export_http2_stream.h"
#include "app_entry.h"
#include "utils_httpc.h"

#ifndef ON_DAILY
#define ON_DAILY
#endif  
#if defined(ON_DAILY)
	#define HTTP2_ONLINE_SERVER_URL       "10.101.12.205"
	#define HTTP2_ONLINE_SERVER_PORT      9999
	#define HTTP2_PRODUCT_KEY             "a1L5EUOh21s"
	#define HTTP2_DEVICE_NAME             "zhangmei_test01"
	#define HTTP2_DEVICE_SECRET           "KQCftfEDCx35LChyEwZoYY6FCYidTOp0"
#elif defined(ON_PRE)
	#define HTTP2_ONLINE_SERVER_URL       "100.67.141.158/"
	#define HTTP2_ONLINE_SERVER_PORT      8443
	#define HTTP2_PRODUCT_KEY             "b1XVhqfan1X"
	#define HTTP2_DEVICE_NAME             "YvhjziEQmKusCFUgRpeo"
	#define HTTP2_DEVICE_SECRET           "QjkhCrAX0SbNWgKpIamuiDdLkk23Q1r7"
#else
	#define HTTP2_ONLINE_SERVER_URL       NULL
	#define HTTP2_ONLINE_SERVER_PORT      443
	#define HTTP2_PRODUCT_KEY             "DM5b8zbTWJs"
	#define HTTP2_DEVICE_NAME             "mydevice1"
	#define HTTP2_DEVICE_SECRET           "q4tiwQuICYfr6JQ8aUFjWxocuXJ5ruEx"
#endif

#define EXAMPLE_TRACE(fmt, ...)                        \
    do {                                               \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__);                \
        HAL_Printf("%s", "\r\n");                      \
    } while (0)


void*   pid1;

static const char UPLOAD_STRING[] =
            "{\"schema\":\"https://iotx-tsl.oss-ap-southeast-1.aliyuncs.com/"
            "schema.json\",\"profile\":{\"productKey\":\"a1X2bEnP82z\"},\"services\":[{"
            "\"outputData\":[],\"identifier\":\"set\",\"inputData\":[{\"identifier\":"
            "\"LightSwitch\",\"dataType\":{\"specs\":{\"0\":\"关闭\",\"1\":\"开启\"},"
            "\"type\":\"bool\"},\"name\":\"主灯开关\"},{\"identifier\":\"WIFI_Band\","
            "\"dataType\":{\"specs\":{\"length\":\"255\"},\"type\":\"text\"},\"name\":"
            "\"频段\"},{\"identifier\":\"WiFI_RSSI\",\"dataType\":{\"specs\":{\"min\":\"-"
            "127\",\"unitName\":\"无\",\"max\":\"-1\",\"step\":\"1\"},\"type\":\"int\"},"
            "\"name\":\"信号强度\"},{\"identifier\":\"WIFI_AP_BSSID\",\"dataType\":{"
            "\"specs\":{\"length\":\"255\"},\"type\":\"text\"},\"name\":\"热点BSSID\"},{"
            "\"identifier\":\"WIFI_Channel\",\"dataType\":{\"specs\":{\"min\":\"1\","
            "\"unitName\":\"无\",\"max\":\"255\",\"step\":\"1\"},\"type\":\"int\"},"
            "\"name\":\"信道\"},{\"identifier\":\"WiFI_SNR\",\"dataType\":{\"specs\":{"
            "\"min\":\"-127\",\"unitName\":\"无\",\"max\":\"127\",\"step\":\"1\"},"
            "\"type\":\"int\"},\"name\":\"信噪比\"},{\"identifier\":\"WIFI_Tx_Rate\","
            "\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"99999\",\"step\":\"1\"},"
            "\"type\":\"int\"},\"name\":\"WIFI_Tx_Rate_Name\"},{\"identifier\":\"WIFI_Rx_"
            "Rate\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"99999\",\"step\":"
            "\"1\"},\"type\":\"int\"},\"name\":\"WIFI_Rx_Rate_Name\"},{\"identifier\":"
            "\"RGBColor\",\"dataType\":{\"specs\":[{\"identifier\":\"Red\",\"dataType\":{"
            "\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"255\",\"step\":\"1\"}"
            ",\"type\":\"int\"},\"name\":\"红色\"},{\"identifier\":\"Green\","
            "\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"255\","
            "\"step\":\"1\"},\"type\":\"int\"},\"name\":\"绿色\"},{\"identifier\":"
            "\"Blue\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":"
            "\"255\",\"step\":\"1\"},\"type\":\"int\"},\"name\":\"蓝色\"}],\"type\":"
            "\"struct\"},\"name\":\"RGB调色\"},{\"identifier\":\"HSVColor\",\"dataType\":"
            "{\"specs\":[{\"identifier\":\"Hue\",\"dataType\":{\"specs\":{\"unit\":\"°\","
            "\"min\":\"0\",\"unitName\":\"度\",\"max\":\"360\",\"step\":\"0.01\"},"
            "\"type\":\"double\"},\"name\":\"色调\"},{\"identifier\":\"Saturation\","
            "\"dataType\":{\"specs\":{\"unit\":\"%\",\"min\":\"0\",\"unitName\":"
            "\"百分比\",\"max\":\"100\",\"step\":\"0.01\"},\"type\":\"double\"},\"name\":"
            "\"饱和度\"},{\"identifier\":\"Value\",\"dataType\":{\"specs\":{\"unit\":\"%"
            "\",\"min\":\"0\",\"unitName\":\"百分比\",\"max\":\"100\",\"step\":\"0.01\"},"
            "\"type\":\"double\"},\"name\":\"明度\"}],\"type\":\"struct\"},\"name\":"
            "\"HSV调色\"},{\"identifier\":\"HSLColor\",\"dataType\":{\"specs\":[{"
            "\"identifier\":\"Hue\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":"
            "\"0\",\"unitName\":\"度\",\"max\":\"360\",\"step\":\"0.01\"},\"type\":"
            "\"double\"},\"name\":\"色调\"},{\"identifier\":\"Saturation\",\"dataType\":{"
            "\"specs\":{\"unit\":\"%\",\"min\":\"0\",\"unitName\":\"百分比\",\"max\":"
            "\"100\",\"step\":\"0.01\"},\"type\":\"double\"},\"name\":\"饱和度\"},{"
            "\"identifier\":\"Lightness\",\"dataType\":{\"specs\":{\"unit\":\"%\","
            "\"min\":\"0\",\"unitName\":\"百分比\",\"max\":\"100\",\"step\":\"0.01\"},"
            "\"type\":\"double\"},\"name\":\"亮度\"}],\"type\":\"struct\"},\"name\":"
            "\"HSL调色\"},{\"identifier\":\"WorkMode\",\"dataType\":{\"specs\":{\"0\":"
            "\"手动\",\"1\":\"阅读\",\"2\":\"影院\",\"3\":\"夜灯\",\"4\":\"生活\",\"5\":"
            "\"柔和\"},\"type\":\"enum\"},\"name\":\"工作模式\"},{\"identifier\":"
            "\"NightLightSwitch\",\"dataType\":{\"specs\":{\"0\":\"关闭\",\"1\":\"开启\"}"
            ",\"type\":\"bool\"},\"name\":\"夜灯开关\"},{\"identifier\":\"Brightness\","
            "\"dataType\":{\"specs\":{\"unit\":\"%\",\"min\":\"0\",\"unitName\":"
            "\"百分比\",\"max\":\"100\",\"step\":\"1\"},\"type\":\"int\"},\"name\":"
            "\"明暗度\"},{\"identifier\":\"ColorTemperature\",\"dataType\":{\"specs\":{"
            "\"unit\":\"K\",\"min\":\"2000\",\"unitName\":\"开尔文\",\"max\":\"7000\","
            "\"step\":\"1\"},\"type\":\"int\"},\"name\":\"冷暖色温\"},{\"identifier\":"
            "\"PropertyCharacter\",\"dataType\":{\"specs\":{\"length\":\"255\"},\"type\":"
            "\"text\"},\"name\":\"PropertyCharacter_Name\"},{\"identifier\":"
            "\"Propertypoint\",\"dataType\":{\"specs\":{\"min\":\"-100\",\"max\":\"100\","
            "\"step\":\"0.01\"},\"type\":\"double\"},\"name\":\"Propertypoint_Name\"}],"
            "\"method\":\"thing.service.property.set\",\"name\":\"set\",\"required\":"
            "true,\"callType\":\"async\",\"desc\":\"属性设置\"},{\"outputData\":[{"
            "\"identifier\":\"LightSwitch\",\"dataType\":{\"specs\":{\"0\":\"关闭\","
            "\"1\":\"开启\"},\"type\":\"bool\"},\"name\":\"主灯开关\"},{\"identifier\":"
            "\"WIFI_Band\",\"dataType\":{\"specs\":{\"length\":\"255\"},\"type\":"
            "\"text\"},\"name\":\"频段\"},{\"identifier\":\"WiFI_RSSI\",\"dataType\":{"
            "\"specs\":{\"min\":\"-127\",\"unitName\":\"无\",\"max\":\"-1\",\"step\":"
            "\"1\"},\"type\":\"int\"},\"name\":\"信号强度\"},{\"identifier\":\"WIFI_AP_"
            "BSSID\",\"dataType\":{\"specs\":{\"length\":\"255\"},\"type\":\"text\"},"
            "\"name\":\"热点BSSID\"},{\"identifier\":\"WIFI_Channel\",\"dataType\":{"
            "\"specs\":{\"min\":\"1\",\"unitName\":\"无\",\"max\":\"255\",\"step\":\"1\"}"
            ",\"type\":\"int\"},\"name\":\"信道\"},{\"identifier\":\"WiFI_SNR\","
            "\"dataType\":{\"specs\":{\"min\":\"-127\",\"unitName\":\"无\",\"max\":"
            "\"127\",\"step\":\"1\"},\"type\":\"int\"},\"name\":\"信噪比\"},{"
            "\"identifier\":\"WIFI_Tx_Rate\",\"dataType\":{\"specs\":{\"min\":\"0\","
            "\"max\":\"99999\",\"step\":\"1\"},\"type\":\"int\"},\"name\":\"WIFI_Tx_Rate_"
            "Name\"},{\"identifier\":\"WIFI_Rx_Rate\",\"dataType\":{\"specs\":{\"min\":"
            "\"0\",\"max\":\"99999\",\"step\":\"1\"},\"type\":\"int\"},\"name\":\"WIFI_"
            "Rx_Rate_Name\"},{\"identifier\":\"RGBColor\",\"dataType\":{\"specs\":[{"
            "\"identifier\":\"Red\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":"
            "\"无\",\"max\":\"255\",\"step\":\"1\"},\"type\":\"int\"},\"name\":\"红色\"},"
            "{\"identifier\":\"Green\",\"dataType\":{\"specs\":{\"min\":\"0\","
            "\"unitName\":\"无\",\"max\":\"255\",\"step\":\"1\"},\"type\":\"int\"},"
            "\"name\":\"绿色\"},{\"identifier\":\"Blue\",\"dataType\":{\"specs\":{"
            "\"min\":\"0\",\"unitName\":\"无\",\"max\":\"255\",\"step\":\"1\"},\"type\":"
            "\"int\"},\"name\":\"蓝色\"}],\"type\":\"struct\"},\"name\":\"RGB调色\"},{"
            "\"identifier\":\"HSVColor\",\"dataType\":{\"specs\":[{\"identifier\":"
            "\"Hue\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"0\",\"unitName\":"
            "\"度\",\"max\":\"360\",\"step\":\"0.01\"},\"type\":\"double\"},\"name\":"
            "\"色调\"},{\"identifier\":\"Saturation\",\"dataType\":{\"specs\":{\"unit\":"
            "\"%\",\"min\":\"0\",\"unitName\":\"百分比\",\"max\":\"100\",\"step\":\"0."
            "01\"},\"type\":\"double\"},\"name\":\"饱和度\"},{\"identifier\":\"Value\","
            "\"dataType\":{\"specs\":{\"unit\":\"%\",\"min\":\"0\",\"unitName\":"
            "\"百分比\",\"max\":\"100\",\"step\":\"0.01\"},\"type\":\"double\"},\"name\":"
            "\"明度\"}],\"type\":\"struct\"},\"name\":\"HSV调色\"},{\"identifier\":"
            "\"HSLColor\",\"dataType\":{\"specs\":[{\"identifier\":\"Hue\",\"dataType\":{"
            "\"specs\":{\"unit\":\"°\",\"min\":\"0\",\"unitName\":\"度\",\"max\":\"360\","
            "\"step\":\"0.01\"},\"type\":\"double\"},\"name\":\"色调\"},{\"identifier\":"
            "\"Saturation\",\"dataType\":{\"specs\":{\"unit\":\"%\",\"min\":\"0\","
            "\"unitName\":\"百分比\",\"max\":\"100\",\"step\":\"0.01\"},\"type\":"
            "\"double\"},\"name\":\"饱和度\"},{\"identifier\":\"Lightness\",\"dataType\":"
            "{\"specs\":{\"unit\":\"%\",\"min\":\"0\",\"unitName\":\"百分比\",\"max\":"
            "\"100\",\"step\":\"0.01\"},\"type\":\"double\"},\"name\":\"亮度\"}],"
            "\"type\":\"struct\"},\"name\":\"HSL调色\"},{\"identifier\":\"WorkMode\","
            "\"dataType\":{\"specs\":{\"0\":\"手动\",\"1\":\"阅读\",\"2\":\"影院\",\"3\":"
            "\"夜灯\",\"4\":\"生活\",\"5\":\"柔和\"},\"type\":\"enum\"},\"name\":"
            "\"工作模式\"},{\"identifier\":\"NightLightSwitch\",\"dataType\":{\"specs\":{"
            "\"0\":\"关闭\",\"1\":\"开启\"},\"type\":\"bool\"},\"name\":\"夜灯开关\"},{"
            "\"identifier\":\"Brightness\",\"dataType\":{\"specs\":{\"unit\":\"%\","
            "\"min\":\"0\",\"unitName\":\"百分比\",\"max\":\"100\",\"step\":\"1\"},"
            "\"type\":\"int\"},\"name\":\"明暗度\"},{\"identifier\":\"ColorTemperature\","
            "\"dataType\":{\"specs\":{\"unit\":\"K\",\"min\":\"2000\",\"unitName\":"
            "\"开尔文\",\"max\":\"7000\",\"step\":\"1\"},\"type\":\"int\"},\"name\":"
            "\"冷暖色温\"},{\"identifier\":\"PropertyCharacter\",\"dataType\":{\"specs\":"
            "{\"length\":\"255\"},\"type\":\"text\"},\"name\":\"PropertyCharacter_Name\"}"
            ",{\"identifier\":\"Propertypoint\",\"dataType\":{\"specs\":{\"min\":\"-"
            "100\",\"max\":\"100\",\"step\":\"0.01\"},\"type\":\"double\"},\"name\":"
            "\"Propertypoint_Name\"}],\"identifier\":\"get\",\"inputData\":["
            "\"LightSwitch\",\"WIFI_Band\",\"WiFI_RSSI\",\"WIFI_AP_BSSID\",\"WIFI_"
            "Channel\",\"WiFI_SNR\",\"WIFI_Tx_Rate\",\"WIFI_Rx_Rate\",\"RGBColor\","
            "\"HSVColor\",\"HSLColor\",\"WorkMode\",\"NightLightSwitch\",\"Brightness\","
            "\"ColorTemperature\",\"PropertyCharacter\",\"Propertypoint\"],\"method\":"
            "\"thing.service.property.get\",\"name\":\"get\",\"required\":true,"
            "\"callType\":\"async\",\"desc\":\"属性获取\"}],\"properties\":[{"
            "\"identifier\":\"LightSwitch\",\"dataType\":{\"specs\":{\"0\":\"关闭\","
            "\"1\":\"开启\"},\"type\":\"bool\"},\"name\":\"主灯开关\",\"accessMode\":"
            "\"rw\",\"required\":true},{\"identifier\":\"WIFI_Band\",\"dataType\":{"
            "\"specs\":{\"length\":\"255\"},\"type\":\"text\"},\"name\":\"频段\","
            "\"accessMode\":\"rw\",\"required\":true},{\"identifier\":\"WiFI_RSSI\","
            "\"dataType\":{\"specs\":{\"min\":\"-127\",\"unitName\":\"无\",\"max\":\"-"
            "1\",\"step\":\"1\"},\"type\":\"int\"},\"name\":\"信号强度\",\"accessMode\":"
            "\"rw\",\"required\":true},{\"identifier\":\"WIFI_AP_BSSID\",\"dataType\":{"
            "\"specs\":{\"length\":\"255\"},\"type\":\"text\"},\"name\":\"热点BSSID\","
            "\"accessMode\":\"rw\",\"required\":true},{\"identifier\":\"WIFI_Channel\","
            "\"dataType\":{\"specs\":{\"min\":\"1\",\"unitName\":\"无\",\"max\":\"255\","
            "\"step\":\"1\"},\"type\":\"int\"},\"name\":\"信道\",\"accessMode\":\"rw\","
            "\"required\":true},{\"identifier\":\"WiFI_SNR\",\"dataType\":{\"specs\":{"
            "\"min\":\"-127\",\"unitName\":\"无\",\"max\":\"127\",\"step\":\"1\"},"
            "\"type\":\"int\"},\"name\":\"信噪比\",\"accessMode\":\"rw\",\"required\":"
            "true},{\"identifier\":\"WIFI_Tx_Rate\",\"dataType\":{\"specs\":{\"min\":"
            "\"0\",\"max\":\"99999\",\"step\":\"1\"},\"type\":\"int\"},\"name\":\"WIFI_"
            "Tx_Rate_Name\",\"accessMode\":\"rw\",\"required\":false},{\"identifier\":"
            "\"WIFI_Rx_Rate\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"99999\","
            "\"step\":\"1\"},\"type\":\"int\"},\"name\":\"WIFI_Rx_Rate_Name\","
            "\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"RGBColor\","
            "\"dataType\":{\"specs\":[{\"identifier\":\"Red\",\"dataType\":{\"specs\":{"
            "\"min\":\"0\",\"unitName\":\"无\",\"max\":\"255\",\"step\":\"1\"},\"type\":"
            "\"int\"},\"name\":\"红色\"},{\"identifier\":\"Green\",\"dataType\":{"
            "\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"255\",\"step\":\"1\"}"
            ",\"type\":\"int\"},\"name\":\"绿色\"},{\"identifier\":\"Blue\",\"dataType\":"
            "{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"255\",\"step\":"
            "\"1\"},\"type\":\"int\"},\"name\":\"蓝色\"}],\"type\":\"struct\"},\"name\":"
            "\"RGB调色\",\"accessMode\":\"rw\",\"required\":false},{\"identifier\":"
            "\"HSVColor\",\"dataType\":{\"specs\":[{\"identifier\":\"Hue\",\"dataType\":{"
            "\"specs\":{\"unit\":\"°\",\"min\":\"0\",\"unitName\":\"度\",\"max\":\"360\","
            "\"step\":\"0.01\"},\"type\":\"double\"},\"name\":\"色调\"},{\"identifier\":"
            "\"Saturation\",\"dataType\":{\"specs\":{\"unit\":\"%\",\"min\":\"0\","
            "\"unitName\":\"百分比\",\"max\":\"100\",\"step\":\"0.01\"},\"type\":"
            "\"double\"},\"name\":\"饱和度\"},{\"identifier\":\"Value\",\"dataType\":{"
            "\"specs\":{\"unit\":\"%\",\"min\":\"0\",\"unitName\":\"百分比\",\"max\":"
            "\"100\",\"step\":\"0.01\"},\"type\":\"double\"},\"name\":\"明度\"}],"
            "\"type\":\"struct\"},\"name\":\"HSV调色\",\"accessMode\":\"rw\","
            "\"required\":false},{\"identifier\":\"HSLColor\",\"dataType\":{\"specs\":[{"
            "\"identifier\":\"Hue\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":"
            "\"0\",\"unitName\":\"度\",\"max\":\"360\",\"step\":\"0.01\"},\"type\":"
            "\"double\"},\"name\":\"色调\"},{\"identifier\":\"Saturation\",\"dataType\":{"
            "\"specs\":{\"unit\":\"%\",\"min\":\"0\",\"unitName\":\"百分比\",\"max\":"
            "\"100\",\"step\":\"0.01\"},\"type\":\"double\"},\"name\":\"饱和度\"},{"
            "\"identifier\":\"Lightness\",\"dataType\":{\"specs\":{\"unit\":\"%\","
            "\"min\":\"0\",\"unitName\":\"百分比\",\"max\":\"100\",\"step\":\"0.01\"},"
            "\"type\":\"double\"},\"name\":\"亮度\"}],\"type\":\"struct\"},\"name\":"
            "\"HSL调色\",\"accessMode\":\"rw\",\"required\":false},{\"identifier\":"
            "\"WorkMode\",\"dataType\":{\"specs\":{\"0\":\"手动\",\"1\":\"阅读\",\"2\":"
            "\"影院\",\"3\":\"夜灯\",\"4\":\"生活\",\"5\":\"柔和\"},\"type\":\"enum\"},"
            "\"name\":\"工作模式\",\"accessMode\":\"rw\",\"required\":false},{"
            "\"identifier\":\"NightLightSwitch\",\"dataType\":{\"specs\":{\"0\":\"关闭\","
            "\"1\":\"开启\"},\"type\":\"bool\"},\"name\":\"夜灯开关\",\"accessMode\":"
            "\"rw\",\"required\":false},{\"identifier\":\"Brightness\",\"dataType\":{"
            "\"specs\":{\"unit\":\"%\",\"min\":\"0\",\"unitName\":\"百分比\",\"max\":"
            "\"100\",\"step\":\"1\"},\"type\":\"int\"},\"name\":\"明暗度\","
            "\"accessMode\":\"rw\",\"required\":false},{\"identifier\":"
            "\"ColorTemperature\",\"dataType\":{\"specs\":{\"unit\":\"K\",\"min\":"
            "\"2000\",\"unitName\":\"开尔文\",\"max\":\"7000\",\"step\":\"1\"},\"type\":"
            "\"int\"},\"name\":\"冷暖色温\",\"accessMode\":\"rw\",\"required\":false},{"
            "\"identifier\":\"PropertyCharacter\",\"dataType\":{\"specs\":{\"length\":"
            "\"255\"},\"type\":\"text\"},\"name\":\"PropertyCharacter_Name\","
            "\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"Propertypoint\","
            "\"dataType\":{\"specs\":{\"min\":\"-100\",\"max\":\"100\",\"step\":\"0.01\"}"
            ",\"type\":\"double\"},\"name\":\"Propertypoint_Name\",\"accessMode\":\"rw\","
            "\"required\":false}],\"events\":[{\"outputData\":[{\"identifier\":"
            "\"LightSwitch\",\"dataType\":{\"specs\":{\"0\":\"关闭\",\"1\":\"开启\"},"
            "\"type\":\"bool\"},\"name\":\"主灯开关\"},{\"identifier\":\"WIFI_Band\","
            "\"dataType\":{\"specs\":{\"length\":\"255\"},\"type\":\"text\"},\"name\":"
            "\"频段\"},{\"identifier\":\"WiFI_RSSI\",\"dataType\":{\"specs\":{\"min\":\"-"
            "127\",\"unitName\":\"无\",\"max\":\"-1\",\"step\":\"1\"},\"type\":\"int\"},"
            "\"name\":\"信号强度\"},{\"identifier\":\"WIFI_AP_BSSID\",\"dataType\":{"
            "\"specs\":{\"length\":\"255\"},\"type\":\"text\"},\"name\":\"热点BSSID\"},{"
            "\"identifier\":\"WIFI_Channel\",\"dataType\":{\"specs\":{\"min\":\"1\","
            "\"unitName\":\"无\",\"max\":\"255\",\"step\":\"1\"},\"type\":\"int\"},"
            "\"name\":\"信道\"},{\"identifier\":\"WiFI_SNR\",\"dataType\":{\"specs\":{"
            "\"min\":\"-127\",\"unitName\":\"无\",\"max\":\"127\",\"step\":\"1\"},"
            "\"type\":\"int\"},\"name\":\"信噪比\"},{\"identifier\":\"WIFI_Tx_Rate\","
            "\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"99999\",\"step\":\"1\"},"
            "\"type\":\"int\"},\"name\":\"WIFI_Tx_Rate_Name\"},{\"identifier\":\"WIFI_Rx_"
            "Rate\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"99999\",\"step\":"
            "\"1\"},\"type\":\"int\"},\"name\":\"WIFI_Rx_Rate_Name\"},{\"identifier\":"
            "\"RGBColor\",\"dataType\":{\"specs\":[{\"identifier\":\"Red\",\"dataType\":{"
            "\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"255\",\"step\":\"1\"}"
            ",\"type\":\"int\"},\"name\":\"红色\"},{\"identifier\":\"Green\","
            "\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"255\","
            "\"step\":\"1\"},\"type\":\"int\"},\"name\":\"绿色\"},{\"identifier\":"
            "\"Blue\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":"
            "\"255\",\"step\":\"1\"},\"type\":\"int\"},\"name\":\"蓝色\"}],\"type\":"
            "\"struct\"},\"name\":\"RGB调色\"},{\"identifier\":\"HSVColor\",\"dataType\":"
            "{\"specs\":[{\"identifier\":\"Hue\",\"dataType\":{\"specs\":{\"unit\":\"°\","
            "\"min\":\"0\",\"unitName\":\"度\",\"max\":\"360\",\"step\":\"0.01\"},"
            "\"type\":\"double\"},\"name\":\"色调\"},{\"identifier\":\"Saturation\","
            "\"dataType\":{\"specs\":{\"unit\":\"%\",\"min\":\"0\",\"unitName\":"
            "\"百分比\",\"max\":\"100\",\"step\":\"0.01\"},\"type\":\"double\"},\"name\":"
            "\"饱和度\"},{\"identifier\":\"Value\",\"dataType\":{\"specs\":{\"unit\":\"%"
            "\",\"min\":\"0\",\"unitName\":\"百分比\",\"max\":\"100\",\"step\":\"0.01\"},"
            "\"type\":\"double\"},\"name\":\"明度\"}],\"type\":\"struct\"},\"name\":"
            "\"HSV调色\"},{\"identifier\":\"HSLColor\",\"dataType\":{\"specs\":[{"
            "\"identifier\":\"Hue\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":"
            "\"0\",\"unitName\":\"度\",\"max\":\"360\",\"step\":\"0.01\"},\"type\":"
            "\"double\"},\"name\":\"色调\"},{\"identifier\":\"Saturation\",\"dataType\":{"
            "\"specs\":{\"unit\":\"%\",\"min\":\"0\",\"unitName\":\"百分比\",\"max\":"
            "\"100\",\"step\":\"0.01\"},\"type\":\"double\"},\"name\":\"饱和度\"},{"
            "\"identifier\":\"Lightness\",\"dataType\":{\"specs\":{\"unit\":\"%\","
            "\"min\":\"0\",\"unitName\":\"百分比\",\"max\":\"100\",\"step\":\"0.01\"},"
            "\"type\":\"double\"},\"name\":\"亮度\"}],\"type\":\"struct\"},\"name\":"
            "\"HSL调色\"},{\"identifier\":\"WorkMode\",\"dataType\":{\"specs\":{\"0\":"
            "\"手动\",\"1\":\"阅读\",\"2\":\"影院\",\"3\":\"夜灯\",\"4\":\"生活\",\"5\":"
            "\"柔和\"},\"type\":\"enum\"},\"name\":\"工作模式\"},{\"identifier\":"
            "\"NightLightSwitch\",\"dataType\":{\"specs\":{\"0\":\"关闭\",\"1\":\"开启\"}"
            ",\"type\":\"bool\"},\"name\":\"夜灯开关\"},{\"identifier\":\"Brightness\","
            "\"dataType\":{\"specs\":{\"unit\":\"%\",\"min\":\"0\",\"unitName\":"
            "\"百分比\",\"max\":\"100\",\"step\":\"1\"},\"type\":\"int\"},\"name\":"
            "\"明暗度\"},{\"identifier\":\"ColorTemperature\",\"dataType\":{\"specs\":{"
            "\"unit\":\"K\",\"min\":\"2000\",\"unitName\":\"开尔文\",\"max\":\"7000\","
            "\"step\":\"1\"},\"type\":\"int\"},\"name\":\"冷暖色温\"},{\"identifier\":"
            "\"PropertyCharacter\",\"dataType\":{\"specs\":{\"length\":\"255\"},\"type\":"
            "\"text\"},\"name\":\"PropertyCharacter_Name\"},{\"identifier\":"
            "\"Propertypoint\",\"dataType\":{\"specs\":{\"min\":\"-100\",\"max\":\"100\","
            "\"step\":\"0.01\"},\"type\":\"double\"},\"name\":\"Propertypoint_Name\"}],"
            "\"identifier\":\"post\",\"method\":\"thing.event.property.post\",\"name\":"
            "\"post\",\"type\":\"info_upload\",\"required\":true,\"desc\":\"属性上报\"},{"
            "\"outputData\":[{\"identifier\":\"ErrorCode\",\"dataType\":{\"specs\":{"
            "\"0\":\"恢复正常\"},\"type\":\"enum\"},\"name\":\"故障代码\"}],"
            "\"identifier\":\"Error\",\"method\":\"thing.event.Error.post\",\"name\":"
            "\"故障上报\",\"type\":\"error\",\"required\":true}]}";

static void on_header(char *stream_id,int cat,const uint8_t *name,size_t namelen, 
                              const uint8_t *value,size_t valuelen, uint8_t flags)
{
    EXAMPLE_TRACE("~~~~~name = %s namelen=%d ,value = %s ,valuelen = %d flag = %d\n", name,namelen,value,valuelen,flags);
}

static void on_chunk_recv(char *stream_id,const uint8_t *data, size_t len,uint8_t flags)
{
     EXAMPLE_TRACE("~~~~~stream_id = %s data = %s ,len = %d flag = %d\n", stream_id,data,len,flags);
}
static void on_stream_close(char *stream_id,uint32_t error_code)
{
     EXAMPLE_TRACE("~~~~~stream_id = %s error_code = %d\n", stream_id,error_code);
}

static http2_stream_cb_t my_cb = {
    .on_stream_header_cb = on_header,
    .on_stream_chunk_recv_cb = on_chunk_recv,
    .on_stream_close_cb = on_stream_close,
};


static int http2_stream_test()
{
    int ret;
    device_conn_info_t conn_info;
    stream_handle_t *handle;
    memset(&conn_info, 0, sizeof( device_conn_info_t));
    conn_info.product_key = HTTP2_PRODUCT_KEY;
    conn_info.device_name = HTTP2_DEVICE_NAME;
    conn_info.device_secret = HTTP2_DEVICE_SECRET;
    conn_info.url = HTTP2_ONLINE_SERVER_URL;
    conn_info.port = HTTP2_ONLINE_SERVER_PORT;

    handle = IOT_HTTP2_Stream_Connect(&conn_info,&my_cb);
    if(handle == NULL) {
        return -1;
    }
    header_ext_info_t my_header_info = {
        { 
            MAKE_HEADER("test_name", "test_http2_header"),
            MAKE_HEADER_CS("hello", "world"),
        },
        2
    };
    stream_data_info_t info_upload, info_download;
    memset(&info_upload,0,sizeof(stream_data_info_t));

    //info_upload.identify = "com/aliyun/iotx/vision/picture/device/upstream";
    info_upload.identify = "iotx/vision/voice/intercom/live";

    memset(&info_download, 0, sizeof(stream_data_info_t));
    info_download.identify = "iotx/vision/voice/intercom/live";

    ret = IOT_HTTP2_Stream_Open(handle, &info_download, &my_header_info);
    if (ret < 0) {
        EXAMPLE_TRACE("=========iotx_http2_downstream_open failed %d!!!!!\n", ret);
        IOT_HTTP2_Stream_Disconnect(handle);
        return -1;
    }

    ret = IOT_HTTP2_Stream_Query(handle, &info_download);
    if (ret < 0) {
        EXAMPLE_TRACE("=========iotx_http2_downstream_query failed %d!!!!!\n", ret);
        IOT_HTTP2_Stream_Disconnect(handle);
        return -1; 
    }

    ret = IOT_HTTP2_Stream_Open(handle, &info_upload, &my_header_info);
    if(ret < 0) {
        EXAMPLE_TRACE("=========iotx_http2_upstream_open failed %d!!!!!\n", ret);
        IOT_HTTP2_Stream_Disconnect(handle);
        return -1;
    }
    //send request 1
    info_upload.stream = (char *)UPLOAD_STRING;
    info_upload.stream_len= sizeof(UPLOAD_STRING);
    info_upload.send_len = 0;
    info_upload.packet_len=2048;

    while(info_upload.send_len<info_upload.stream_len) {
        info_upload.stream = (char *)UPLOAD_STRING + info_upload.send_len;
        if(info_upload.stream_len-info_upload.send_len<info_upload.packet_len) {
            info_upload.packet_len = info_upload.stream_len-info_upload.send_len;
        }
        ret = IOT_HTTP2_Stream_Send(handle, &info_upload);
        if(ret <0 ) {
            EXAMPLE_TRACE("send err, ret = %d\n",ret);
            break;
        }
        EXAMPLE_TRACE("iotx_http2_stream_send info_upload.send_len =%d ret = %d\n", info_upload.send_len,ret);
    }

    //send request 2
    info_upload.stream = (char *)UPLOAD_STRING;
    info_upload.stream_len= sizeof(UPLOAD_STRING);
    info_upload.send_len = 0;
    info_upload.packet_len=1024;

    while(info_upload.send_len<info_upload.stream_len) {
        info_upload.stream = (char *)UPLOAD_STRING + info_upload.send_len;
        if(info_upload.stream_len-info_upload.send_len<info_upload.packet_len) {
            info_upload.packet_len = info_upload.stream_len-info_upload.send_len;
        }
        ret = IOT_HTTP2_Stream_Send(handle, &info_upload);
        if(ret <0 ) {
            EXAMPLE_TRACE("send err, ret = %d\n",ret);
            break;
        }
        EXAMPLE_TRACE("iotx_http2_stream_send info_upload.send_len =%d ret = %d\n", info_upload.send_len,ret);
    }

    EXAMPLE_TRACE("=========Wait 20 seconds to receive downstream data\n");
    HAL_SleepMs(20000);

    IOT_HTTP2_Stream_Close(handle, &info_upload);
    IOT_HTTP2_Stream_Close(handle, &info_download);
    HAL_SleepMs(2000);

    ret = IOT_HTTP2_Stream_Disconnect(handle);
    EXAMPLE_TRACE("close connect %d\n",ret);
    return 0;
}

int linkkit_main(void *paras)
{
    int ret;

    IOT_SetLogLevel(IOT_LOG_DEBUG);

    ret = http2_stream_test();
    return ret;
}
