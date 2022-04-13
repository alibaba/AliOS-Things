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

#include "Utils.h"
#include "LogUtils.h"
#include <iostream>
#include <memory>
#include <cstdarg>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <thread>
#include <cstdarg>
using namespace AlibabaCloud::OSS;

static LogLevel    gOssLogLevel = LogLevel::LogAll;
static LogCallback gLogCallback = nullptr;
const static char *EnvLogLevels[] =
{
    "off", "fatal", "error", "warn",
    "info", "debug", "trace", "all"
};

static std::string LogPrefix(LogLevel logLevel, const char* tag)
{
    static const char *LogStr[] = {"[OFF]", "[FATAL]", "[ERROR]", "[WARN]", "[INFO]" , "[DEBUG]", "[TRACE]", "[ALL]"};
    int index = logLevel - LogLevel::LogOff;
    std::stringstream ss;
    auto tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto ms = tp.time_since_epoch().count() % 1000;
    auto t = std::chrono::system_clock::to_time_t(tp);
    struct tm tm;
#ifdef WIN32
    ::localtime_s(&tm, &t);
#else
    ::localtime_r(&t, &tm);
#endif
#if defined(__GNUG__) && __GNUC__ < 5
    char tmbuff[64];
    strftime(tmbuff, 64, "%Y-%m-%d %H:%M:%S.", &tm);
    ss << "[" << tmbuff << std::setw(3) << std::setfill('0') << ms << "]";
#else
    ss << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S.") << std::setw(3) << std::setfill('0') << ms << "]";
#endif
    ss << LogStr[index];
    ss << "[" << tag << "]";
    ss << "[" << std::this_thread::get_id() << "]";
    return ss.str();
}

void AlibabaCloud::OSS::FormattedLog(LogLevel logLevel, const char* tag, const char* fmt, ...)
{
    std::stringstream ss;
    ss << LogPrefix(logLevel, tag);
    char buffer[2050];
    int i = 0;
    va_list args;
    va_start(args, fmt);
#ifdef WIN32
    i = vsnprintf_s(buffer, sizeof(buffer) - 1, _TRUNCATE, fmt, args);
#else
    i = vsnprintf(buffer, sizeof(buffer) - 1, fmt, args);
#endif
    va_end(args);

    while (i > 0 && buffer[i - 1] == '\n') {
        i--;
        buffer[i] = '\0';
    }

    std::cout << buffer << std::endl;
    if (gLogCallback) {
        gLogCallback(logLevel, ss.str());
    }
}

static void DefaultLogCallbackFunc(LogLevel level, const std::string &stream)
{
    UNUSED_PARAM(level);
    std::cerr << stream;
}

LogLevel AlibabaCloud::OSS::GetLogLevelInner()
{
    return gOssLogLevel;
}

LogCallback AlibabaCloud::OSS::GetLogCallbackInner()
{
    return gLogCallback;
}

void AlibabaCloud::OSS::SetLogLevelInner(LogLevel level)
{
    gOssLogLevel = level;
}

void AlibabaCloud::OSS::SetLogCallbackInner(LogCallback callback)
{
    gLogCallback = callback;
}

void AlibabaCloud::OSS::InitLogInner()
{
    gOssLogLevel = LogLevel::LogOff;
    gLogCallback = nullptr;
    auto value = std::getenv("OSS_SDK_LOG_LEVEL");
    if (value) {
        auto level = ToLower(Trim(value).c_str());
        const auto size = sizeof(EnvLogLevels)/sizeof(EnvLogLevels[0]);
        for (auto i = 0U; i < size; i++) {
            if (level.compare(EnvLogLevels[i]) == 0) {
                gOssLogLevel = static_cast<decltype(LogLevel::LogOff)>(static_cast<decltype(i)>(LogLevel::LogOff) + i);
                gLogCallback = DefaultLogCallbackFunc;
                break;
            }
        }
    }
}

void AlibabaCloud::OSS::DeinitLogInner()
{
    gOssLogLevel = LogLevel::LogOff;
    gLogCallback = nullptr;
}
